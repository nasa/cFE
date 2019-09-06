/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
**  File:
**  cfe_esmempool.c
**
**  Purpose:
**  Set of services for management of discrete sized memory pools.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
*/

/*
** Includes
*/
#include "private/cfe_private.h"
#include "cfe_esmempool.h"
#include "cfe_es.h"
#include "cfe_es_task.h"
#include "cfe_es_log.h"
#include <stdio.h>

/**
 * Macro that determines the native alignment requirement of a specific type
 *
 * By getting the offset of the structure after following a single char,
 * this effectively gets how much padding the compiler added, which in turn reveals its
 * minimum alignment requirement.  (C99 is lacking a standardized "alignof" operator,
 * and this is intended to substitute).
 */
#define ALIGN_OF(type)      ((cpuaddr)&((struct { char Byte; type Align; } *)0)->Align)

/**
 * Union to assist/simplify the pointer manipulation when allocating buffers in a pool
 *
 * When allocating buffers, the memory is calculated using raw addresses (cpuaddr) and then
 * used as either buffer descriptor pointer (BD_t*) or user buffer pointers (uint32*).
 *
 * This union assists with casting between the 3 types.  It is still a cast, but at least
 * it limits the casting to these intended data types so it is slightly safer in that regard.
 */
typedef union
{
    BD_t *BdPtr;        /**< Use when interpreting pool memory as a descriptor */
    uint32 *UserPtr;    /**< Use when interpreting pool memory as a user buffer */
    cpuaddr Addr;       /**< Use when interpreting pool memory as a memory address */
} MemPoolAddr_t;

/*****************************************************************************/
/*
** Local Macro Definitions
*/
#define CFE_ES_CHECK_PATTERN           0x5a5a
#define CFE_ES_MEMORY_ALLOCATED        0xaaaa
#define CFE_ES_MEMORY_DEALLOCATED      0xdddd
/*****************************************************************************/
/*
** Type Definitions
*/


uint32 CFE_ES_MemPoolDefSize[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES] =
{
    CFE_PLATFORM_ES_MAX_BLOCK_SIZE,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_16,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_15,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_14,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_13,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_12,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_11,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_10,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_09,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_08,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_07,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_06,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_05,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_04,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_03,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_02,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_01
};

/*****************************************************************************/
/*
** Local Function Prototypes
*/
uint32 CFE_ES_GetBlockSize(Pool_t  *PoolPtr, uint32 Size);

/*****************************************************************************/
/*
** Functions
*/

/*
** CFE_ES_PoolCreateNoSem will initialize a pre-allocated memory pool without using a mutex.
*/
int32 CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t  *HandlePtr,
                             uint8               *MemPtr,
                             uint32               Size )
{
    return CFE_ES_PoolCreateEx(HandlePtr, MemPtr, Size, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, 
                               &CFE_ES_MemPoolDefSize[0],CFE_ES_NO_MUTEX);
}

/*
** CFE_ES_PoolCreate will initialize a pre-allocated memory pool while using a mutex.
*/
int32 CFE_ES_PoolCreate(CFE_ES_MemHandle_t  *HandlePtr,
                        uint8               *MemPtr,
                        uint32               Size )
{
    return CFE_ES_PoolCreateEx(HandlePtr, MemPtr, Size, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES, 
                               &CFE_ES_MemPoolDefSize[0],CFE_ES_USE_MUTEX);
}


int32 CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t  *HandlePtr,
                          uint8               *MemPtr,
                          uint32               Size,
                          uint32               NumBlockSizes,
                          uint32              *BlockSizes,
                          uint16               UseMutex )
{
    char     MutexName[OS_MAX_API_NAME];
    cpuaddr  MemAddr;
    cpuaddr  MemAlignSize;
    Pool_t  *PoolPtr;
    uint32   i;
    uint32   j;
    uint32   k;
    uint32  *BlockSizeArrayPtr;
    uint32   BlockSizeArraySize;
    uint32   MinBlockSize;

    /*
     * Verify basic sanity checks early, before doing anything.
     *
     * Previously this code would fill the PoolPtr structure prior to even verifying
     * if the size was at least sizeof(Pool_t) -- possibly overrunning memory.
     *
     * The absolute minimum memory required for a pool to be usable would be
     * enough to store a Pool_t plus at least sizeof(BD_t) plus alignment
     *
     * Note that alignment adjustments (if enabled) or a larger minimum block size may make
     * it so its not possible to obtain even a single buffer allocation, even if this criteria
     * is met.  But this will protect against overrunning memory which is the most important thing.
     */
    MinBlockSize = sizeof(Pool_t) + sizeof(BD_t) + sizeof(CFE_ES_STATIC_POOL_TYPE(CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN));
    if ( Size < MinBlockSize )
    {  /* must be able make Pool management structure and at least one of the smallest blocks  */
        CFE_ES_WriteToSysLog("CFE_ES:poolCreate Pool size(%u) too small for management structure, need >=(%u)\n",
                (unsigned int)Size,
                (unsigned int)MinBlockSize);
        return(CFE_ES_BAD_ARGUMENT);
    }

   /* If too many sizes are specified, return an error */
   if (NumBlockSizes > CFE_ES_MAX_MEMPOOL_BLOCK_SIZES)
   {
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Num Block Sizes (%d) greater than max (%d)\n",
                           (int)NumBlockSizes, CFE_ES_MAX_MEMPOOL_BLOCK_SIZES);
      return(CFE_ES_BAD_ARGUMENT);
   }

   if ((UseMutex != CFE_ES_USE_MUTEX) && (UseMutex != CFE_ES_NO_MUTEX))
   {
      CFE_ES_WriteToSysLog("CFE_ES:poolCreate Invalid Mutex Usage Option (%d), must be %d or %d\n",
                           UseMutex, CFE_ES_NO_MUTEX, CFE_ES_USE_MUTEX);
      return(CFE_ES_BAD_ARGUMENT);
   }

   /*
    * First align for the pool itself.
    * This only needs meet requirements to store a Pool_t
    */
   MemAlignSize = ALIGN_OF(Pool_t) - 1;
   MemAlignSize |= MemAlignSize >> 1;
   MemAlignSize |= MemAlignSize >> 2;
   MemAlignSize |= MemAlignSize >> 4;
   MemAlignSize |= MemAlignSize >> 8;
   MemAlignSize |= MemAlignSize >> 16;
   MemAddr = ((cpuaddr)MemPtr + MemAlignSize) & ~MemAlignSize;

   /*
    * Determine alignment requirements of pool buffers
    * This needs to meet alignment requirements of CFE_ES_PoolAlign_t, or
    * the user specified CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN, whichever is greater.
    */
   MemAlignSize = ALIGN_OF(CFE_ES_PoolAlign_t) - 1;
   MemAlignSize |= CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN - 1;
   MemAlignSize |= MemAlignSize >> 1;
   MemAlignSize |= MemAlignSize >> 2;
   MemAlignSize |= MemAlignSize >> 4;
   MemAlignSize |= MemAlignSize >> 8;
   MemAlignSize |= MemAlignSize >> 16;

   /* Set the pointers after alignment adjustment (if enabled) */
   PoolPtr         = (Pool_t *)MemAddr;
   *HandlePtr      = MemAddr;

   if (UseMutex == CFE_ES_USE_MUTEX)
   {
      /*
      ** Construct a name for the Mutex from the address
      ** This is needed only because OS_MutSemCreate requires
      ** a unique name for each semaphore created.
      */
      snprintf(MutexName, OS_MAX_API_NAME, "%08lX", (unsigned long)MemPtr);

      /* create a semphore to protect this memory pool */
      OS_MutSemCreate(&(PoolPtr->MutexId), MutexName, 0);
   }

   PoolPtr->PoolHandle    = MemAddr;
   PoolPtr->End           = (cpuaddr)MemPtr + Size;  /* End is based off the _original_ baseaddr and size */
   PoolPtr->Size          = PoolPtr->End - MemAddr;  /* Adjusts for any size lost due to alignment */
   PoolPtr->AlignMask     = MemAlignSize;
   PoolPtr->CurrentAddr   = MemAddr + sizeof(Pool_t);
   PoolPtr->SizeDescPtr   = NULL;
   PoolPtr->CheckErrCntr  = 0;
   PoolPtr->RequestCntr   = 0;
   
   PoolPtr->UseMutex     = UseMutex;

   for (i=0; i<CFE_ES_MAX_MEMPOOL_BLOCK_SIZES; i++)
   {
      PoolPtr->SizeDesc[i].NumCreated = 0;
      PoolPtr->SizeDesc[i].NumFree = 0;
      PoolPtr->SizeDesc[i].MaxSize = 0;
      PoolPtr->SizeDesc[i].Top = NULL;
   }

   /* Use default block sizes if none or too many sizes are specified */
   if ((NumBlockSizes == 0) || (BlockSizes == NULL))
   {
       BlockSizeArrayPtr = &CFE_ES_MemPoolDefSize[0];
       BlockSizeArraySize = CFE_ES_MAX_MEMPOOL_BLOCK_SIZES;
   }
   else
   {
       BlockSizeArrayPtr = BlockSizes;
       BlockSizeArraySize = NumBlockSizes;
   }
   
   /* Use specified block sizes but make sure they are ordered largest to smallest */
   MinBlockSize = 0xffffffff;
   for (i=0; i<BlockSizeArraySize; i++)
   {
        if ((BlockSizeArrayPtr[i] < MinBlockSize) && (BlockSizeArrayPtr[i] != 0))
        {
            MinBlockSize = BlockSizeArrayPtr[i];
        }
        
        j = 0;
        while (j<CFE_ES_MAX_MEMPOOL_BLOCK_SIZES)
        {
            if (BlockSizeArrayPtr[i] > PoolPtr->SizeDesc[j].MaxSize)
            {
                /* Make space for new size */
                for (k=i; k>j; k--)
                {
                    PoolPtr->SizeDesc[k].MaxSize = PoolPtr->SizeDesc[k-1].MaxSize;
                }
                
                /* Insert the size in the correct location */
                PoolPtr->SizeDesc[j].MaxSize = BlockSizeArrayPtr[i];
                j = CFE_ES_MAX_MEMPOOL_BLOCK_SIZES;
            }
            else
            {
                j++;
            }
        }
   }

   return(CFE_SUCCESS);
}

/*
** Function:
**   CFE_ES_GetPoolBuf
**
** Purpose:
**   CFE_ES_GetPoolBuf allocates a block from the memory pool.
*/
int32 CFE_ES_GetPoolBuf(uint32             **BufPtr,
                        CFE_ES_MemHandle_t   Handle,
                        uint32               Size )
{
   Pool_t  * PoolPtr = (Pool_t *)Handle;
   uint32   BlockSize;
   MemPoolAddr_t BlockAddr;
   uint32    AppId= 0xFFFFFFFF;

   if (PoolPtr != NULL)
   {
      if (Handle != PoolPtr->PoolHandle)
      {
         CFE_ES_GetAppID(&AppId);
         CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Bad handle(0x%08lX) AppId=%d\n",(unsigned long)Handle,(int)AppId);
         return(CFE_ES_ERR_MEM_HANDLE);
      }
   }
   else
   {
      CFE_ES_GetAppID(&AppId);
      CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Bad handle(0x%08lX) AppId=%d\n",(unsigned long)Handle,(int)AppId);
      return(CFE_ES_ERR_MEM_HANDLE);
   }


   if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
   {
      OS_MutSemTake(PoolPtr->MutexId);
   }

   *BufPtr = NULL;

   BlockSize = CFE_ES_GetBlockSize(PoolPtr, Size);
   if (BlockSize > PoolPtr->SizeDesc[0].MaxSize)
   {
      /* save off the max size prior to release, for the log message */
      BlockSize = PoolPtr->SizeDesc[0].MaxSize;
      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
      CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:size(%u) > max(%u).\n",
                  (unsigned int)Size,
                  (unsigned int)BlockSize);
      return(CFE_ES_ERR_MEM_BLOCK_SIZE);
   }

   /*
   ** Check if any of the requested size are available
   */
   if (PoolPtr->SizeDescPtr->Top != NULL) /* Set by GetBlockSize call */
   {
         /*
         ** Get it off the top on the list
         */
         BlockAddr.BdPtr = PoolPtr->SizeDescPtr->Top;

         PoolPtr->SizeDescPtr->Top = BlockAddr.BdPtr->Next;
         PoolPtr->SizeDescPtr->NumFree--;

         BlockAddr.BdPtr->CheckBits       = CFE_ES_CHECK_PATTERN;
         BlockAddr.BdPtr->Allocated       = CFE_ES_MEMORY_ALLOCATED; /* Flag memory block as allocated */
         BlockAddr.BdPtr->Size            = BlockSize;
         BlockAddr.BdPtr->Next            = NULL;

         ++BlockAddr.BdPtr;
         *BufPtr                = BlockAddr.UserPtr;
    }
    else /* go make one */
    {
         /*
          * Determine the memory address of the new user block,
          * which must be aligned according to the AlignMask member.
          *
          * Account for the space required for the buffer descriptor, which
          * be placed before the user buffer in memory.
          */
         BlockAddr.Addr = (PoolPtr->CurrentAddr + sizeof(BD_t) + PoolPtr->AlignMask) &
                 ~PoolPtr->AlignMask;

         /*
          * Check if there is enough space remaining in the pool -- the
          * proposed start address plus the block size must not exceed the pool end.
          * Note it is checked this way in case the pool is at the end of memory, i.e.
          * if the pool ends at 0xFFFFFFFF on a 32 bit machine, a comparison of simply
          * (addr > end) will not work.
          */
         if ( (PoolPtr->End - (BlockAddr.Addr + BlockSize)) >= PoolPtr->Size ){ /* can't fit in remaing mem */
            if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
            {
               OS_MutSemGive(PoolPtr->MutexId);
            }
            CFE_ES_WriteToSysLog("CFE_ES:getPoolBuf err:Request won't fit in remaining memory\n");
            return(CFE_ES_ERR_MEM_BLOCK_SIZE);
         }

         /* point to new memory block */
         *BufPtr = BlockAddr.UserPtr;

         /*
         ** adjust pool current pointer and other recordkeeping in the Pool_t
         */
         PoolPtr->CurrentAddr = BlockAddr.Addr + BlockSize;
         PoolPtr->SizeDescPtr->NumCreated++;
         PoolPtr->RequestCntr++;

         /*
         ** create the buffer descriptor at the front of it
         */
         --BlockAddr.BdPtr;
         BlockAddr.BdPtr->CheckBits = CFE_ES_CHECK_PATTERN;
         BlockAddr.BdPtr->Allocated = CFE_ES_MEMORY_ALLOCATED; /* Flag memory block as allocated */
         BlockAddr.BdPtr->Size      = BlockSize;
         BlockAddr.BdPtr->Next      = NULL;

     }

     if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
     {
        OS_MutSemGive(PoolPtr->MutexId);
     }
     return (int32)BlockSize;
}

/*
** CFE_ES_GetPoolBufInfo gets the size of the specified block (if it exists).
*/
int32 CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t   Handle,
                            uint32             * BufPtr)
{
  Pool_t   *PoolPtr =  (Pool_t *)Handle;
  MemPoolAddr_t BlockAddr;
  int32 Status;

  BlockAddr.UserPtr = BufPtr;

  if (PoolPtr == NULL || Handle != PoolPtr->PoolHandle)
  {
      /* bad handle */
      Status = CFE_ES_ERR_MEM_HANDLE;
  }
  else if ( (BlockAddr.Addr < (Handle + sizeof(Pool_t) + sizeof(BD_t))) ||
          (BlockAddr.Addr >= PoolPtr->End) )
  {
      /* sanity check */
      Status = CFE_ES_BUFFER_NOT_IN_POOL;
  }
  else
  {
      /*
      ** Move to the descriptor (immediately preceding the user buffer)
      ** and perform simple sanity checks for descriptor.
      ** This must be done while locked, if the USE_MUTEX flag is set.
      */
      --BlockAddr.BdPtr;

      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemTake(PoolPtr->MutexId);
      }

      /* If a block is no longer allocated, report an error */
      if (BlockAddr.BdPtr->Allocated != CFE_ES_MEMORY_ALLOCATED ||
              BlockAddr.BdPtr->CheckBits != CFE_ES_CHECK_PATTERN)
      {
          Status = CFE_ES_ERR_MEM_HANDLE;
      }
      else
      {
          Status = BlockAddr.BdPtr->Size;
      }

      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
  }
  
  return Status;
}

/*
** CFE_ES_putPoolBuf returns a block back to the memory pool.
*/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t   Handle,
                        uint32             * BufPtr)
{
  Pool_t   *PoolPtr =  (Pool_t *)Handle;
  MemPoolAddr_t BlockAddr;
  uint32 BlockSize;
  int32 Status;
  char LogMessage[CFE_ES_MAX_SYSLOG_MSG_SIZE];

  LogMessage[0] = 0;
  BlockAddr.UserPtr = BufPtr;

  if (PoolPtr == NULL || Handle != PoolPtr->PoolHandle)
  {
      /* bad handle */
      CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
              "CFE_ES:putPoolBuf err:Invalid Memory Handle (0x%08lX).\n", (unsigned long) Handle);
      Status = CFE_ES_ERR_MEM_HANDLE;
  }
  else if ( (BlockAddr.Addr < (Handle + sizeof(Pool_t) + sizeof(BD_t))) ||
          (BlockAddr.Addr >= PoolPtr->End) )
  {
      /* sanity check */
      CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
              "CFE_ES:putPoolBuf err:Invalid Memory Handle (0x%08lX) or memory block (0x%08lX).\n",
                           (unsigned long) Handle, (unsigned long)BufPtr);
      Status = CFE_ES_ERR_MEM_HANDLE;
  }
  else
  {
      /*
      ** Move to the descriptor (immediately preceding the user buffer)
      ** and perform simple sanity checks for descriptor.
      ** This must be done while locked, if the USE_MUTEX flag is set.
      */
      --BlockAddr.BdPtr;

      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemTake(PoolPtr->MutexId);
      }

      /* If a block is no longer allocated, report an error */
      if (BlockAddr.BdPtr->Allocated != CFE_ES_MEMORY_ALLOCATED)
      {
          PoolPtr->CheckErrCntr++;
          CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                  "CFE_ES:putPoolBuf err:Deallocating unallocated memory block @ 0x%08lX\n",
                  (unsigned long)BufPtr);
          Status = CFE_ES_ERR_MEM_HANDLE;
      }
      else if (BlockAddr.BdPtr->CheckBits != CFE_ES_CHECK_PATTERN)
      {
          PoolPtr->CheckErrCntr++;
          CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                  "CFE_ES:putPoolBuf err:Invalid/Corrupted Memory descriptor @ 0x%08lX\n",
                  (unsigned long)BufPtr);
          Status = CFE_ES_ERR_MEM_HANDLE;
      }
      else
      {
          BlockSize = CFE_ES_GetBlockSize(PoolPtr, BlockAddr.BdPtr->Size);

          if (BlockSize > PoolPtr->SizeDesc[0].MaxSize)
          {
              PoolPtr->CheckErrCntr++;
              CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                      "CFE_ES:putPoolBuf err:size(%d) > max(%d).\n",
                      (unsigned int)BlockAddr.BdPtr->Size,(unsigned int)PoolPtr->SizeDesc[0].MaxSize);
              Status = CFE_ES_ERR_MEM_HANDLE;
          }
          else
          {
              BlockAddr.BdPtr->Allocated = CFE_ES_MEMORY_DEALLOCATED;
              BlockAddr.BdPtr->Next = PoolPtr->SizeDescPtr->Top; /* Set by GetBlockSize call */
              PoolPtr->SizeDescPtr->Top = BlockAddr.BdPtr;
              PoolPtr->SizeDescPtr->NumFree++;
              Status = BlockSize;
          }
      }

      if (PoolPtr->UseMutex == CFE_ES_USE_MUTEX)
      {
         OS_MutSemGive(PoolPtr->MutexId);
      }
  }

  /* Output the message to syslog once the OTHER resource is unlocked */
  if (LogMessage[0] != 0)
  {
      CFE_ES_SYSLOG_APPEND(LogMessage);
  }

  return Status;
}

/*
** Function:
**   ES_GetBlockSize
**
** Purpose:
**
*/
uint32 CFE_ES_GetBlockSize(Pool_t  *PoolPtr, uint32 Size)
{
  uint32 i=0;

  PoolPtr->SizeDescPtr = NULL;

  /* If caller is requesting a size larger than the largest allowed */
  /* then return an error code */  
  if (Size > PoolPtr->SizeDesc[0].MaxSize)
  {
        return(0xFFFFFFFF);
  }
  
  /* Locate the smallest size that holds the desired size */
  while ((i < (CFE_ES_MAX_MEMPOOL_BLOCK_SIZES-1)) &&
         (Size <= PoolPtr->SizeDesc[i+1].MaxSize))
  {
        i++;
  }
  
  PoolPtr->SizeDescPtr = &PoolPtr->SizeDesc[i];
  return(PoolPtr->SizeDesc[i].MaxSize);
}


/*
** Function:
**   CFE_ES_GetMemPoolStats
**
** Purpose:
**
*/
int32 CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr,
                             CFE_ES_MemHandle_t  Handle)
{
    uint32    AppId = 0xFFFFFFFF;
    Pool_t   *PoolPtr;
    uint32    i;
    
    PoolPtr = (Pool_t *)Handle;

    if (PoolPtr == NULL || Handle != PoolPtr->PoolHandle)
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("CFE_ES:getMemPoolStats err:Bad handle(0x%08lX) AppId=%d\n", (unsigned long)Handle, (int)AppId);
        return(CFE_ES_ERR_MEM_HANDLE);
    }

    BufPtr->PoolSize = PoolPtr->Size;
    BufPtr->NumBlocksRequested = PoolPtr->RequestCntr;
    BufPtr->CheckErrCtr = PoolPtr->CheckErrCntr;
    BufPtr->NumFreeBytes = PoolPtr->End - PoolPtr->CurrentAddr;
    
    for (i=0; i<CFE_ES_MAX_MEMPOOL_BLOCK_SIZES; i++)
    {
        BufPtr->BlockStats[i].BlockSize = PoolPtr->SizeDesc[i].MaxSize;
        BufPtr->BlockStats[i].NumCreated = PoolPtr->SizeDesc[i].NumCreated;
        BufPtr->BlockStats[i].NumFree = PoolPtr->SizeDesc[i].NumFree;
    }
    
    return(CFE_SUCCESS);
}


/*
** Function:
**   CFE_ES_ValidateHandle
**
** Purpose:
**   Insures that the handle passed in meets all of the requirements of a valid handle.
*/
bool CFE_ES_ValidateHandle(CFE_ES_MemHandle_t  Handle)
{
    bool      HandleValid = true;
    Pool_t   *PoolPtr;
    
    PoolPtr = (Pool_t *)Handle;

    /* There are several tests to make sure the memory pool handle is valid */
   
    if ( PoolPtr == NULL )
    /* Test #1) Handle must not be a NULL pointer */
    {
        HandleValid = false;
    }
    /* Test #2) Handle must be a valid memory address (allows both RAM and EEPROM) */
    else if (CFE_PSP_MemValidateRange(Handle, sizeof(Pool_t), CFE_PSP_MEM_ANY) != CFE_PSP_SUCCESS)
    {
        HandleValid = false;
    }
    /* Test #3) First field of pool structure must be start address of Pool */
    else if (Handle != PoolPtr->PoolHandle)
    {
        HandleValid = false;
    }
    /* Test #4) Pool structure must have End ptr equal to Start plus Size */
    else if ((PoolPtr->PoolHandle + PoolPtr->Size) != PoolPtr->End)
    {
        HandleValid = false;
    }
    
    return(HandleValid);
}
