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
**  cfe_es_cds_mempool.c
**
**  Purpose:
**  Set of services for management of the CDS discrete sized memory pools.
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
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_es_cds_mempool.h"
#include "cfe_es_global.h"
#include "cfe_es_log.h"
#include <stdio.h>

/*****************************************************************************/
/*
** Local Macro Definitions
*/
#define CFE_ES_CDS_CHECK_PATTERN   0x5a5a
#define CFE_ES_CDS_BLOCK_USED      0xaaaa
#define CFE_ES_CDS_BLOCK_UNUSED    0xdddd

/*****************************************************************************/
/*
** Type Definitions
*/

/*****************************************************************************/
/*
** File Global Data
*/

CFE_ES_CDSPool_t      CFE_ES_CDSMemPool;
CFE_ES_CDSBlockDesc_t CFE_ES_CDSBlockDesc;

uint32 CFE_ES_CDSMemPoolDefSize[CFE_ES_CDS_NUM_BLOCK_SIZES] = 
{
    CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_16,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_15,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_14,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_13,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_12,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_11,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_10,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_09,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_08,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_07,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_06,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_05,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_04,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_03,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_02,
    CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE_01
};

/*****************************************************************************/
/*
** Local Function Prototypes
*/
int32 CFE_ES_CDSGetBinIndex(uint32 DesiredSize);

/*****************************************************************************/
/*
** Functions
*/

/*
** CFE_ES_CreateCDSPool will initialize a pre-allocated memory pool.
**
** NOTE:
**  This function is only ever called during "Early Init" phase,
**  where it is not possible to have contention writing into the syslog.
**  Therefore the use of CFE_ES_SysLogWrite_Unsync() is acceptable
*/
int32 CFE_ES_CreateCDSPool(uint32  CDSPoolSize, uint32  StartOffset)
{
    char MutexName[10] = {"CDS_POOL"};
    uint32  i = 0;
    uint32  Size = (CDSPoolSize & 0xfffffffc);

    /* create a semphore to protect this memory pool */
    OS_MutSemCreate(&(CFE_ES_CDSMemPool.MutexId), MutexName, 0);

    /* Take the semaphore to ensure the mem pool is not being used during it's creation */
    OS_MutSemTake(CFE_ES_CDSMemPool.MutexId);

    CFE_ES_CDSMemPool.Start        = StartOffset;
    CFE_ES_CDSMemPool.End          = StartOffset + Size;
    CFE_ES_CDSMemPool.Size         = Size;
    CFE_ES_CDSMemPool.Current      = StartOffset;
    CFE_ES_CDSMemPool.SizeIndex    = -1;

    CFE_ES_CDSMemPool.CheckErrCntr = 0;
    CFE_ES_CDSMemPool.RequestCntr  = 0;

    for (i=0; i<CFE_ES_CDS_NUM_BLOCK_SIZES; i++)
    {
        CFE_ES_CDSMemPool.SizeDesc[i].Top = 0;
        CFE_ES_CDSMemPool.SizeDesc[i].NumCreated = 0;
        CFE_ES_CDSMemPool.SizeDesc[i].MaxSize = CFE_ES_CDSMemPoolDefSize[i];
    }

    if (CDSPoolSize < (CFE_ES_CDSMemPool.MinBlockSize + sizeof(CFE_ES_CDSBlockDesc_t)))
    {
        /* Must be able make Pool verification, block descriptor and at least one of the smallest blocks  */
        CFE_ES_SysLogWrite_Unsync("CFE_ES:CreateCDSPool-Pool size(%u) too small for one CDS Block, need >=%u\n",
                             (unsigned int)CDSPoolSize, (unsigned int)(CFE_ES_CDSMemPool.MinBlockSize + sizeof(CFE_ES_CDSBlockDesc_t)));
                        
        /* Give and delete semaphore since CDS Pool creation failed */     
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        OS_MutSemDelete(CFE_ES_CDSMemPool.MutexId);
        return(CFE_ES_BAD_ARGUMENT);
    }
    
    OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);

    return(CFE_SUCCESS);
}


/*
** Function:
**   CFE_ES_RebuildCDSPool
**
** Purpose:
**
** NOTE:
**  This function is only ever called during "Early Init" phase,
**  where it is not possible to have contention writing into the syslog.
**  Therefore the use of CFE_ES_SysLogWrite_Unsync() is acceptable
*/
int32 CFE_ES_RebuildCDSPool(uint32 CDSPoolSize, uint32 StartOffset)
{
    char MutexName[10] = {"CDS_POOL"};
    uint32 i = 0;
    uint32 Size = (CDSPoolSize & 0xfffffffc);
    int32  Status = OS_SUCCESS;
    uint32 Offset = StartOffset;
    int32  BinIndex = 0;

    /* create a semphore to protect this memory pool */
    OS_MutSemCreate(&(CFE_ES_CDSMemPool.MutexId), MutexName, 0);

    /* Take the semaphore to ensure the mem pool is not being used during it's creation */
    OS_MutSemTake(CFE_ES_CDSMemPool.MutexId);

    CFE_ES_CDSMemPool.Start        = StartOffset;
    CFE_ES_CDSMemPool.End          = StartOffset + Size;
    CFE_ES_CDSMemPool.Size         = Size;
    CFE_ES_CDSMemPool.Current      = 0;
    CFE_ES_CDSMemPool.SizeIndex    = -1;

    CFE_ES_CDSMemPool.CheckErrCntr = 0;
    CFE_ES_CDSMemPool.RequestCntr  = 0;

    for (i=0; i<CFE_ES_CDS_NUM_BLOCK_SIZES; i++)
    {
        CFE_ES_CDSMemPool.SizeDesc[i].Top = 0;
        CFE_ES_CDSMemPool.SizeDesc[i].NumCreated = 0;
        CFE_ES_CDSMemPool.SizeDesc[i].MaxSize = CFE_ES_CDSMemPoolDefSize[i];
    }
    
    if (CDSPoolSize < (CFE_ES_CDSMemPool.MinBlockSize + sizeof(CFE_ES_CDSBlockDesc_t)))
    {
        /* Must be able make Pool verification, block descriptor and at least one of the smallest blocks  */
        CFE_ES_SysLogWrite_Unsync("CFE_ES:RebuildCDSPool-Pool size(%u) too small for one CDS Block, need >=%u\n",
                             (unsigned int)CDSPoolSize, (unsigned int)(CFE_ES_CDSMemPool.MinBlockSize + sizeof(CFE_ES_CDSBlockDesc_t)));

        /* Give and delete semaphore since CDS Pool rebuild failed */     
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        OS_MutSemDelete(CFE_ES_CDSMemPool.MutexId);
        return(CFE_ES_BAD_ARGUMENT);
    }

    /* Scan the CDS memory trying to find blocks that were created but are now free */
    while ((Status == OS_SUCCESS) && 
           (Offset < (CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t))) &&
           (CFE_ES_CDSMemPool.Current == 0))
    {
        /* Read the block descriptor for the first block in the memory pool */
        Status = CFE_PSP_ReadFromCDS(&CFE_ES_CDSBlockDesc, Offset, sizeof(CFE_ES_CDSBlockDesc_t));
        
        if (Status == CFE_PSP_SUCCESS)
        {
            /* First, determine if the block is being or has been used */
            if (CFE_ES_CDSBlockDesc.CheckBits == CFE_ES_CDS_CHECK_PATTERN)
            {
                /* See if the block is currently being used */
                if (CFE_ES_CDSBlockDesc.AllocatedFlag != CFE_ES_CDS_BLOCK_USED)
                {
                    /* If the block is not currently being used, */
                    /* then add it to the appropriate linked list in the memory pool */
                    BinIndex = CFE_ES_CDSGetBinIndex(CFE_ES_CDSBlockDesc.SizeUsed);
                    
                    /* Sanity-check the block descriptor */
                    if (BinIndex >= 0)
                    {
                        CFE_ES_CDSBlockDesc.Next = CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top;
                        CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
                        CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top = Offset;

                        /* Store the new CDS Block Descriptor in the CDS */
                        Status = CFE_PSP_WriteToCDS(&CFE_ES_CDSBlockDesc, Offset, sizeof(CFE_ES_CDSBlockDesc_t));

                        if (Status != CFE_PSP_SUCCESS)
                        {
                            CFE_ES_SysLogWrite_Unsync("CFE_ES:RebuildCDS-Err writing to CDS (Stat=0x%08x)\n", (unsigned int)Status);
                            Status = CFE_ES_CDS_ACCESS_ERROR;
                        }
                    }
                    else
                    {
                        CFE_ES_CDSMemPool.CheckErrCntr++;
                        CFE_ES_SysLogWrite_Unsync("CFE_ES:RebuildCDS-Invalid Block Descriptor \n");
                        Status = CFE_ES_CDS_ACCESS_ERROR;
                    }
                }
                
                /* Skip to the next block of memory */
                Offset = Offset + CFE_ES_CDSBlockDesc.ActualSize + sizeof(CFE_ES_CDSBlockDesc_t);
            }
            else
            {
                /* If the block has never been used, then we should save the offset as the current offset */
                /* which in turn will finish the scan of the CDS memory */
                CFE_ES_CDSMemPool.Current = Offset;
            }
        }
        else
        {
            CFE_ES_SysLogWrite_Unsync("CFE_ES:RebuildCDS-Err reading from CDS (Stat=0x%08x)\n", (unsigned int)Status);
            Status = CFE_ES_CDS_ACCESS_ERROR;
        }
    }  /* end while */
    
    OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);

    return Status;
}


/*
** Function:
**   CFE_ES_GetCDSBlock
**
** Purpose:
**   CFE_ES_GetCDSBlock allocates a block from the CDS memory pool.
*/
int32 CFE_ES_GetCDSBlock(CFE_ES_CDSBlockHandle_t *BlockHandle,
                         uint32  BlockSize )
{
    int32                   BinIndex;
    int32                   Status;

    OS_MutSemTake(CFE_ES_CDSMemPool.MutexId);

    BinIndex = CFE_ES_CDSGetBinIndex(BlockSize);
    if (BinIndex < 0)
    {
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        CFE_ES_WriteToSysLog("CFE_ES:GetCDSBlock-err:size(%d) > max(%d).\n", (int)BlockSize, CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE);
        return(CFE_ES_ERR_MEM_BLOCK_SIZE);
    }

   /*
   ** Check if any of the requested size are available
   */
   if (CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top != 0)
   {
         /*
         ** Get it off the top on the list
         */
         Status = CFE_PSP_ReadFromCDS(&CFE_ES_CDSBlockDesc, 
                                    CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top, 
                                    sizeof(CFE_ES_CDSBlockDesc_t));
                    
         if (Status != CFE_PSP_SUCCESS)
         {
            OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
            CFE_ES_WriteToSysLog("CFE_ES:GetCDSBlock-Err reading from CDS (Stat=0x%08x)\n", (unsigned int)Status);
            return(CFE_ES_CDS_ACCESS_ERROR);
         }
                 
         /* The handle returned is the byte offset of the block in the CDS */
         *BlockHandle                             = CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top;
         
         /* A local version of the block descriptor is initialized */
         CFE_ES_CDSBlockDesc.CheckBits            = CFE_ES_CDS_CHECK_PATTERN;
         CFE_ES_CDSBlockDesc.AllocatedFlag        = CFE_ES_CDS_BLOCK_USED;
         CFE_ES_CDSBlockDesc.SizeUsed             = BlockSize;
         CFE_ES_CDSBlockDesc.ActualSize           = CFE_ES_CDSMemPool.SizeDesc[BinIndex].MaxSize;
         CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top = CFE_ES_CDSBlockDesc.Next;
         CFE_ES_CDSBlockDesc.CRC                  = 0;
         CFE_ES_CDSBlockDesc.Next                 = 0;
    }
    else /* Create a new block */
    {
         if ( (CFE_ES_CDSMemPool.Current == 0) ||
              (((uint32)CFE_ES_CDSMemPool.Current + 
                sizeof(CFE_ES_CDSBlockDesc_t) + 
                CFE_ES_CDSMemPool.SizeDesc[BinIndex].MaxSize ) >= CFE_ES_CDSMemPool.End) )
         {
            OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
            CFE_ES_WriteToSysLog("CFE_ES:GetCDSBlock-err:Request for %d bytes won't fit in remaining memory\n", (int)BlockSize);
            return(CFE_ES_ERR_MEM_BLOCK_SIZE);
         }

         *BlockHandle = (CFE_ES_CDSBlockHandle_t)CFE_ES_CDSMemPool.Current;

         CFE_ES_CDSMemPool.SizeDesc[BinIndex].NumCreated++;
         CFE_ES_CDSMemPool.RequestCntr++;

         /*
         ** Initialize the buffer descriptor that will be kept in front of the CDS Block
         */
         CFE_ES_CDSBlockDesc.CheckBits     = CFE_ES_CDS_CHECK_PATTERN;
         CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_USED;
         CFE_ES_CDSBlockDesc.SizeUsed      = BlockSize;
         CFE_ES_CDSBlockDesc.ActualSize    = CFE_ES_CDSMemPool.SizeDesc[BinIndex].MaxSize;
         CFE_ES_CDSBlockDesc.CRC           = 0;
         CFE_ES_CDSBlockDesc.Next          = 0;

         /*
         ** Adjust pool current pointer to first unallocated byte in CDS
         */
         CFE_ES_CDSMemPool.Current = CFE_ES_CDSMemPool.Current 
                                     + CFE_ES_CDSBlockDesc.ActualSize
                                     + sizeof(CFE_ES_CDSBlockDesc_t);
     }
     
     /* Store the new CDS Block Descriptor in the CDS */
     Status = CFE_PSP_WriteToCDS(&CFE_ES_CDSBlockDesc, *BlockHandle, sizeof(CFE_ES_CDSBlockDesc_t));

     if (Status != CFE_PSP_SUCCESS)
     {
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        CFE_ES_WriteToSysLog("CFE_ES:GetCDSBlock-Err writing to CDS (Stat=0x%08x)\n", (unsigned int)Status);
        return(CFE_ES_CDS_ACCESS_ERROR);
     }
     
     OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
     
     return Status;
}

/*
** CFE_ES_PutCDSBlock returns a block back to the CDS memory pool.
*/
int32 CFE_ES_PutCDSBlock(CFE_ES_CDSBlockHandle_t BlockHandle)
{
    int32 BinIndex;
    int32 Status;

    /* Perform some sanity checks on the BlockHandle */
    /* First check, is the handle within an acceptable range of CDS offsets */
    if ((BlockHandle < sizeof(CFE_ES_Global.CDSVars.ValidityField)) || 
        (BlockHandle > (CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) - 
                        CFE_ES_CDSMemPool.MinBlockSize - sizeof(CFE_ES_Global.CDSVars.ValidityField))))
    {
        CFE_ES_WriteToSysLog("CFE_ES:PutCDSBlock-Invalid Memory Handle.\n");
        return(CFE_ES_ERR_MEM_HANDLE);
    }

    OS_MutSemTake(CFE_ES_CDSMemPool.MutexId);

    /* Read a copy of the contents of the block descriptor being freed */
    Status = CFE_PSP_ReadFromCDS(&CFE_ES_CDSBlockDesc, BlockHandle, sizeof(CFE_ES_CDSBlockDesc_t));

    if (Status != CFE_PSP_SUCCESS)
    {
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        CFE_ES_WriteToSysLog("CFE_ES:PutCDSBlock-Err reading from CDS (Stat=0x%08x)\n", (unsigned int)Status);
        return(CFE_ES_CDS_ACCESS_ERROR);
    }
     
    /* Make sure the contents of the Block Descriptor look reasonable */
    if ((CFE_ES_CDSBlockDesc.CheckBits != CFE_ES_CDS_CHECK_PATTERN) ||
        (CFE_ES_CDSBlockDesc.AllocatedFlag != CFE_ES_CDS_BLOCK_USED))
    {
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        CFE_ES_WriteToSysLog("CFE_ES:PutCDSBlock-Invalid Handle or Block Descriptor.\n");
        return(CFE_ES_ERR_MEM_HANDLE);
    }

    BinIndex = CFE_ES_CDSGetBinIndex(CFE_ES_CDSBlockDesc.ActualSize);

    /* Final sanity check on block descriptor, is the Actual size reasonable */
    if (BinIndex < 0)
    {
        CFE_ES_CDSMemPool.CheckErrCntr++;
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        CFE_ES_WriteToSysLog("CFE_ES:PutCDSBlock-Invalid Block Descriptor\n");
        return(CFE_ES_ERR_MEM_HANDLE);
    }

    CFE_ES_CDSBlockDesc.Next = CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top;
    CFE_ES_CDSBlockDesc.AllocatedFlag = CFE_ES_CDS_BLOCK_UNUSED;
    CFE_ES_CDSMemPool.SizeDesc[BinIndex].Top = BlockHandle;

    /* Store the new CDS Block Descriptor in the CDS */
    Status = CFE_PSP_WriteToCDS(&CFE_ES_CDSBlockDesc, BlockHandle, sizeof(CFE_ES_CDSBlockDesc_t));

    if (Status != CFE_PSP_SUCCESS)
    {
        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
        CFE_ES_WriteToSysLog("CFE_ES:PutCDSBlock-Err writing to CDS (Stat=0x%08x)\n", (unsigned int)Status);
        return(CFE_ES_CDS_ACCESS_ERROR);
    }
     
    OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);

    return Status;
}

/*
** Function:
**   CFE_ES_CDSGetBinIndex
**
** Purpose:
**
*/
int32 CFE_ES_CDSGetBinIndex(uint32 DesiredSize)
{
    int32 i=0;
    
    if (DesiredSize > CFE_ES_CDSMemPool.SizeDesc[0].MaxSize)
    {
        return(-1);
    }
    
    /* Look ahead to see if the next bin has a size too small */
    while ((i < (CFE_ES_CDS_NUM_BLOCK_SIZES-1)) &&
           (DesiredSize <= CFE_ES_CDSMemPool.SizeDesc[i+1].MaxSize))
    {
        i++;
    }
    
    return(i);
}


/*
** Function:
**   CFE_ES_CDSBlockWrite
**
** Purpose:
**
*/
int32 CFE_ES_CDSBlockWrite(CFE_ES_CDSBlockHandle_t BlockHandle, void *DataToWrite)
{
    char  LogMessage[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    int32 Status = CFE_SUCCESS;
    int32 BinIndex = 0;
    
    /* Ensure the the log message is an empty string in case it is never written to */
    LogMessage[0] = 0;

    /* Validate the handle before doing anything */
    if ((BlockHandle < sizeof(CFE_ES_Global.CDSVars.ValidityField)) || 
        (BlockHandle > (CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) - 
                        CFE_ES_CDSMemPool.MinBlockSize - sizeof(CFE_ES_Global.CDSVars.ValidityField))))
    {
        CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                "CFE_ES:CDSBlkWrite-Invalid Memory Handle.\n");
        Status = CFE_ES_ERR_MEM_HANDLE;
    }
    else
    {
        OS_MutSemTake(CFE_ES_CDSMemPool.MutexId);

        /* Get a copy of the block descriptor associated with the specified handle */
        /* Read the block descriptor for the first block in the memory pool */
        Status = CFE_PSP_ReadFromCDS(&CFE_ES_CDSBlockDesc, BlockHandle, sizeof(CFE_ES_CDSBlockDesc_t));

        if (Status != CFE_PSP_SUCCESS)
        {
            CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                    "CFE_ES:CDSBlkWrite-Err reading from CDS (Stat=0x%08x)\n", (unsigned int)Status);
        }
        /* Validate the block to make sure it is still active and not corrupted */
        else if ((CFE_ES_CDSBlockDesc.CheckBits != CFE_ES_CDS_CHECK_PATTERN) ||
                (CFE_ES_CDSBlockDesc.AllocatedFlag != CFE_ES_CDS_BLOCK_USED))
        {
            CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                    "CFE_ES:CDSBlkWrite-Invalid Handle or Block Descriptor.\n");
            Status = CFE_ES_ERR_MEM_HANDLE;
        }
        else
        {
            BinIndex = CFE_ES_CDSGetBinIndex(CFE_ES_CDSBlockDesc.ActualSize);

            /* Final sanity check on block descriptor, is the Actual size reasonable */
            if (BinIndex < 0)
            {
                CFE_ES_CDSMemPool.CheckErrCntr++;
                CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                        "CFE_ES:CDSBlkWrite-Invalid Block Descriptor\n");
                Status = CFE_ES_ERR_MEM_HANDLE;
            }
            else
            {

                /* Use the size specified when the CDS was created to compute the CRC */
                CFE_ES_CDSBlockDesc.CRC = CFE_ES_CalculateCRC(DataToWrite, CFE_ES_CDSBlockDesc.SizeUsed, 0, CFE_MISSION_ES_DEFAULT_CRC);

                /* Write the new block descriptor for the data coming from the Application */
                Status = CFE_PSP_WriteToCDS(&CFE_ES_CDSBlockDesc, BlockHandle, sizeof(CFE_ES_CDSBlockDesc_t));

                if (Status == CFE_PSP_SUCCESS)
                {
                    /* Write the new data coming from the Application to the CDS */
                    Status = CFE_PSP_WriteToCDS(DataToWrite, (BlockHandle + sizeof(CFE_ES_CDSBlockDesc_t)), CFE_ES_CDSBlockDesc.SizeUsed);

                    if (Status != CFE_PSP_SUCCESS)
                    {
                        CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                                "CFE_ES:CDSBlkWrite-Err writing data to CDS (Stat=0x%08x) @Offset=0x%08x\n",
                                (unsigned int)Status, (unsigned int)(BlockHandle + sizeof(CFE_ES_CDSBlockDesc_t)));
                    }
                }
                else
                {
                    CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                            "CFE_ES:CDSBlkWrite-Err writing BlockDesc to CDS (Stat=0x%08x) @Offset=0x%08x\n",
                            (unsigned int)Status, (unsigned int)BlockHandle);
                }
            }
        }

        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
    }

    /* Do the actual syslog if something went wrong */
    if (LogMessage[0] != 0)
    {
        CFE_ES_SYSLOG_APPEND(LogMessage);
    }

    return Status;
}


/*
** Function:
**   CFE_ES_CDSBlockRead
**
** Purpose:
**
*/
int32 CFE_ES_CDSBlockRead(void *DataRead, CFE_ES_CDSBlockHandle_t BlockHandle)
{
    char   LogMessage[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    int32  Status = CFE_SUCCESS;
    uint32 CrcOfCDSData;
    int32  BinIndex;
    
    /* Validate the handle before doing anything */
    LogMessage[0] = 0;
    if ((BlockHandle < sizeof(CFE_ES_Global.CDSVars.ValidityField)) || 
        (BlockHandle > (CFE_ES_CDSMemPool.End - sizeof(CFE_ES_CDSBlockDesc_t) - 
                        CFE_ES_CDSMemPool.MinBlockSize - sizeof(CFE_ES_Global.CDSVars.ValidityField))))
    {
        CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                "CFE_ES:CDSBlkRd-Invalid Memory Handle.\n");
        Status = CFE_ES_ERR_MEM_HANDLE;
    }
    else
    {
        OS_MutSemTake(CFE_ES_CDSMemPool.MutexId);

        /* Get a copy of the block descriptor associated with the specified handle */
        /* Read the block descriptor for the first block in the memory pool */
        Status = CFE_PSP_ReadFromCDS(&CFE_ES_CDSBlockDesc, BlockHandle, sizeof(CFE_ES_CDSBlockDesc_t));

        if (Status != CFE_PSP_SUCCESS)
        {
            CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                            "CFE_ES:CDSBlkRd-Err reading from CDS (Stat=0x%08x)\n",
                            (unsigned int)Status);
        }
        /* Validate the block to make sure it is still active and not corrupted */
        else if ((CFE_ES_CDSBlockDesc.CheckBits != CFE_ES_CDS_CHECK_PATTERN) ||
                (CFE_ES_CDSBlockDesc.AllocatedFlag != CFE_ES_CDS_BLOCK_USED))
        {
            CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                            "CFE_ES:CDSBlkRd-Invalid Handle or Block Descriptor.\n");
            Status = CFE_ES_ERR_MEM_HANDLE;
        }
        else
        {
            BinIndex = CFE_ES_CDSGetBinIndex(CFE_ES_CDSBlockDesc.ActualSize);

            /* Final sanity check on block descriptor, is the Actual size reasonable */
            if (BinIndex < 0)
            {
                CFE_ES_CDSMemPool.CheckErrCntr++;
                CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                                "CFE_ES:CDSBlkRd-Invalid Block Descriptor\n");
                Status = CFE_ES_ERR_MEM_HANDLE;
            }
            else
            {
                /* Read the old data block */
                Status = CFE_PSP_ReadFromCDS(DataRead, (BlockHandle + sizeof(CFE_ES_CDSBlockDesc_t)), CFE_ES_CDSBlockDesc.SizeUsed);

                if (Status == CFE_PSP_SUCCESS)
                {
                    /* Compute the CRC for the data read from the CDS and determine if the data is still valid */
                    CrcOfCDSData = CFE_ES_CalculateCRC(DataRead, CFE_ES_CDSBlockDesc.SizeUsed, 0, CFE_MISSION_ES_DEFAULT_CRC);

                    /* If the CRCs do not match, report an error */
                    if (CrcOfCDSData != CFE_ES_CDSBlockDesc.CRC)
                    {
                        Status = CFE_ES_CDS_BLOCK_CRC_ERR;
                    }
                    else
                    {
                        Status = CFE_SUCCESS;
                    }
                }
                else
                {
                    CFE_ES_SysLog_snprintf(LogMessage, sizeof(LogMessage),
                                    "CFE_ES:CDSBlkRd-Err reading block from CDS (Stat=0x%08x) @Offset=0x%08x\n",
                                    (unsigned int)Status, (unsigned int)BlockHandle);
                }
            }
        }

        OS_MutSemGive(CFE_ES_CDSMemPool.MutexId);
    }

    /* Do the actual syslog if something went wrong */
    if (LogMessage[0] != 0)
    {
        CFE_ES_SYSLOG_APPEND(LogMessage);
    }
    
    return Status;
}

/*
** Function:
**   CFE_ES_CDSReqdMinSize
**
** Purpose:
**
*/
uint32 CFE_ES_CDSReqdMinSize(uint32 MaxNumBlocksToSupport)
{
    uint32 i;

    for (i=0; i<CFE_ES_CDS_NUM_BLOCK_SIZES; i++)
    {
        /* Assume the last non-zero block size is the minimum block size */
        if (CFE_ES_CDSMemPoolDefSize[i] > 0)
        {
            CFE_ES_CDSMemPool.MinBlockSize = CFE_ES_CDSMemPoolDefSize[i];
        }
    }
    
     return (MaxNumBlocksToSupport * (sizeof(CFE_ES_CDSBlockDesc_t)+CFE_ES_CDSMemPool.MinBlockSize));
}

