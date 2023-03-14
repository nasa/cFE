/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
**  File:
**  cfe_es_mempool.c
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
#include "cfe_es_module_all.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * Macro that determines the native alignment requirement of a specific type
 *
 * By getting the offset of the structure after following a single char,
 * this effectively gets how much padding the compiler added, which in turn reveals its
 * minimum alignment requirement.  (C99 is lacking a standardized "alignof" operator,
 * and this is intended to substitute).
 */
#define ALIGN_OF(type)           \
    ((cpuaddr) & ((struct {      \
                     char Byte;  \
                     type Align; \
                 } *)0)          \
                     ->Align)

/*****************************************************************************/
/*
** Type Definitions
*/

const size_t CFE_ES_MemPoolDefSize[CFE_PLATFORM_ES_POOL_MAX_BUCKETS] = {
    CFE_PLATFORM_ES_MAX_BLOCK_SIZE,    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_16, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_15,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_14, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_13, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_12,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_11, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_10, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_09,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_08, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_07, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_06,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_05, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_04, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_03,
    CFE_PLATFORM_ES_MEM_BLOCK_SIZE_02, CFE_PLATFORM_ES_MEM_BLOCK_SIZE_01};

/*****************************************************************************/
/*
** Functions
*/

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_MemPoolDirectRetrieve(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr)
{
    cpuaddr                 DataAddress;
    CFE_ES_MemPoolRecord_t *MemPoolRecPtr = (CFE_ES_MemPoolRecord_t *)PoolRecPtr;

    DataAddress = MemPoolRecPtr->BaseAddr + Offset;
    *BdPtr      = (CFE_ES_GenPoolBD_t *)DataAddress;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_MemPoolDirectCommit(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr)
{
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_MemPoolID_ToIndex(CFE_ES_MemHandle_t PoolID, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(PoolID), CFE_ES_POOLID_BASE, CFE_PLATFORM_ES_MAX_MEMORY_POOLS,
                                  Idx);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_CheckMemPoolSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_ES_MemPoolRecord_t *MemPoolRecPtr;
    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    MemPoolRecPtr = CFE_ES_LocateMemPoolRecordByID(CFE_ES_MEMHANDLE_C(CheckId));
    return (MemPoolRecPtr == NULL || CFE_ES_MemPoolRecordIsUsed(MemPoolRecPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_MemPoolRecord_t *CFE_ES_LocateMemPoolRecordByID(CFE_ES_MemHandle_t PoolID)
{
    CFE_ES_MemPoolRecord_t *MemPoolRecPtr;
    uint32                  Idx;

    if (CFE_ES_MemPoolID_ToIndex(PoolID, &Idx) == CFE_SUCCESS)
    {
        MemPoolRecPtr = &CFE_ES_Global.MemPoolTable[Idx];
    }
    else
    {
        MemPoolRecPtr = NULL;
    }

    return MemPoolRecPtr;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t *PoolID, void *MemPtr, size_t Size)
{
    return CFE_ES_PoolCreateEx(PoolID, MemPtr, Size, CFE_PLATFORM_ES_POOL_MAX_BUCKETS, &CFE_ES_MemPoolDefSize[0],
                               CFE_ES_NO_MUTEX);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_PoolCreate(CFE_ES_MemHandle_t *PoolID, void *MemPtr, size_t Size)
{
    return CFE_ES_PoolCreateEx(PoolID, MemPtr, Size, CFE_PLATFORM_ES_POOL_MAX_BUCKETS, &CFE_ES_MemPoolDefSize[0],
                               CFE_ES_USE_MUTEX);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t *PoolID, void *MemPtr, size_t Size, uint16 NumBlockSizes,
                                 const size_t *BlockSizes, bool UseMutex)
{
    int32                   OsStatus;
    int32                   Status;
    CFE_ResourceId_t        PendingID;
    CFE_ES_MemPoolRecord_t *PoolRecPtr;
    size_t                  Alignment;
    size_t                  MinimumSize;
    char                    MutexName[OS_MAX_API_NAME];

    /* Sanity Check inputs */
    if (MemPtr == NULL || PoolID == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /* If too many sizes are specified, return an error */
    if (NumBlockSizes > CFE_PLATFORM_ES_POOL_MAX_BUCKETS)
    {
        CFE_ES_WriteToSysLog("%s: Num Block Sizes (%d) greater than max (%d)\n", __func__, (int)NumBlockSizes,
                             CFE_PLATFORM_ES_POOL_MAX_BUCKETS);
        return CFE_ES_BAD_ARGUMENT;
    }

    /*
     * Use default block sizes if not specified
     */
    if (BlockSizes == NULL)
    {
        BlockSizes = CFE_ES_MemPoolDefSize;
        if (NumBlockSizes == 0)
        {
            NumBlockSizes = CFE_PLATFORM_ES_POOL_MAX_BUCKETS;
        }
    }

    /*
     * Sanity check the pool size
     */
    MinimumSize = CFE_ES_GenPoolCalcMinSize(NumBlockSizes, BlockSizes, 1);
    if (Size < MinimumSize)
    {
        CFE_ES_WriteToSysLog("%s: Pool size(%lu) too small, need >=%lu bytes\n", __func__, (unsigned long)Size,
                             (unsigned long)MinimumSize);
        return CFE_ES_BAD_ARGUMENT;
    }

    CFE_ES_LockSharedData(__func__, __LINE__);

    /* scan for a free slot */
    PendingID  = CFE_ResourceId_FindNext(CFE_ES_Global.LastMemPoolId, CFE_PLATFORM_ES_MAX_MEMORY_POOLS,
                                        CFE_ES_CheckMemPoolSlotUsed);
    PoolRecPtr = CFE_ES_LocateMemPoolRecordByID(CFE_ES_MEMHANDLE_C(PendingID));

    if (PoolRecPtr == NULL)
    {
        CFE_ES_SysLogWrite_Unsync("%s: No free MemPool slots available\n", __func__);
        Status = CFE_ES_NO_RESOURCE_IDS_AVAILABLE;
    }
    else
    {
        /* Fully clear the entry, just in case of stale data */
        memset(PoolRecPtr, 0, sizeof(*PoolRecPtr));
        CFE_ES_MemPoolRecordSetUsed(PoolRecPtr, CFE_RESOURCEID_RESERVED);
        CFE_ES_Global.LastMemPoolId = PendingID;
        Status                      = CFE_SUCCESS;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
     * If no open resource ID was found, return now.
     *
     * No more inline returns after this point; execution
     * must continue to the end of this function where the ID is freed
     * if not fully successful.
     */
    if (Status != CFE_SUCCESS)
    {
        return Status;
    }

    Alignment = ALIGN_OF(CFE_ES_PoolAlign_t); /* memory mapped pools should be aligned */
    if (Alignment < CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN)
    {
        /*
         * Note about path coverage testing - depending on the
         * system architecture and configuration this line may be
         * unreachable.  This is OK.
         */
        Alignment = CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN;
    }

    /*
     * Most of the work is done by the generic pool implementation.
     * This subsystem works in offsets, not pointers.
     */
    Status = CFE_ES_GenPoolInitialize(&PoolRecPtr->Pool, 0, Size, Alignment, NumBlockSizes, BlockSizes,
                                      CFE_ES_MemPoolDirectRetrieve, CFE_ES_MemPoolDirectCommit);

    /*
     * If successful, complete the process.
     */
    if (Status == CFE_SUCCESS && UseMutex == CFE_ES_USE_MUTEX)
    {
        /*
        ** Construct a name for the Mutex from the address
        ** This is needed only because OS_MutSemCreate requires
        ** a unique name for each semaphore created.
        */
        snprintf(MutexName, OS_MAX_API_NAME, "Pool%08lX", CFE_ResourceId_ToInteger(PendingID));

        /* create a mutex to protect this memory pool */
        OsStatus = OS_MutSemCreate(&PoolRecPtr->MutexId, MutexName, 0);
        if (OsStatus != OS_SUCCESS)
        {
            /* log error and rewrite to CFE status code */
            CFE_ES_WriteToSysLog("%s: OSAL error %ld while creating mutex\n", __func__, (long)OsStatus);

            Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }

    if (Status == CFE_SUCCESS)
    {
        /*
         * Store the base address.
         * This is only relevant for memory-mapped pools which is why it is done here.
         */
        PoolRecPtr->BaseAddr = (cpuaddr)MemPtr;

        /*
         * Get the calling context.
         * If this is not a valid CFE context, then AppID will be undefined.
         * We can still permit the creation of the pool but automatic cleanup
         * if an exception or other event occurs will not be possible.
         */
        CFE_ES_GetAppID(&PoolRecPtr->OwnerAppID);

        /*
         * Store the actual/correct pool ID in the record.
         */
        CFE_ES_MemPoolRecordSetUsed(PoolRecPtr, PendingID);
    }
    else
    {
        /*
         * Free the entry that was reserved earlier
         */
        CFE_ES_MemPoolRecordSetFree(PoolRecPtr);
        PendingID = CFE_RESOURCEID_UNDEFINED;
    }

    /*
     * Export pool ID to caller as handle
     */
    *PoolID = CFE_ES_MEMHANDLE_C(PendingID);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_PoolDelete(CFE_ES_MemHandle_t PoolID)
{
    CFE_ES_MemPoolRecord_t *PoolRecPtr;
    osal_id_t               MutexId;
    int32                   Status;
    int32                   OsStatus;

    PoolRecPtr = CFE_ES_LocateMemPoolRecordByID(PoolID);

    CFE_ES_LockSharedData(__func__, __LINE__);

    /* basic sanity check */
    if (CFE_ES_MemPoolRecordIsMatch(PoolRecPtr, PoolID))
    {
        MutexId = PoolRecPtr->MutexId; /* snapshot mutex ID, will be freed later */
        CFE_ES_MemPoolRecordSetFree(PoolRecPtr);
        Status = CFE_SUCCESS;
    }
    else
    {
        MutexId = OS_OBJECT_ID_UNDEFINED;
        Status  = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /* Release the mutex if it was configured.
     * This is done after releasing the ES lock, to avoid
     * potential conflict with holding two locks. */
    if (OS_ObjectIdDefined(MutexId))
    {
        OsStatus = OS_MutSemDelete(MutexId);
        if (OsStatus != OS_SUCCESS)
        {
            /*
             * Report to syslog for informational purposes only.
             *
             * The MemPool entry has already been deleted, so this
             * function should not return an error at this point.
             */
            CFE_ES_WriteToSysLog("%s: Error %ld deleting mutex\n", __func__, (long)OsStatus);
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GetPoolBuf(CFE_ES_MemPoolBuf_t *BufPtr, CFE_ES_MemHandle_t Handle, size_t Size)
{
    int32                   Status;
    CFE_ES_AppId_t          AppId;
    CFE_ES_MemPoolRecord_t *PoolRecPtr;
    size_t                  DataOffset;

    if (BufPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    PoolRecPtr = CFE_ES_LocateMemPoolRecordByID(Handle);

    /* basic sanity check */
    if (!CFE_ES_MemPoolRecordIsMatch(PoolRecPtr, Handle))
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("%s: Err:Bad handle(0x%08lX) AppId=%lu\n", __func__, CFE_RESOURCEID_TO_ULONG(Handle),
                             CFE_RESOURCEID_TO_ULONG(AppId));
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    /*
     * Real work begins here.
     * If pool is mutex-protected, take the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemTake(PoolRecPtr->MutexId);
    }

    /*
     * Fundamental work is done as a generic routine.
     *
     * If successful, this gets an offset, which can then
     * be translated into a pointer to return to the caller.
     */
    Status = CFE_ES_GenPoolGetBlock(&PoolRecPtr->Pool, &DataOffset, Size);

    /*
     * Real work ends here.
     * If pool is mutex-protected, release the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemGive(PoolRecPtr->MutexId);
    }

    /* If not successful, return error now */
    if (Status != CFE_SUCCESS)
    {
        return Status;
    }

    /* Compute the actual buffer address. */
    *BufPtr = CFE_ES_MEMPOOLBUF_C(PoolRecPtr->BaseAddr + DataOffset);

    return (int32)Size;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t Handle, CFE_ES_MemPoolBuf_t BufPtr)
{
    int32                   Status;
    CFE_ES_MemPoolRecord_t *PoolRecPtr;
    size_t                  DataOffset;
    size_t                  DataSize;

    if (BufPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    PoolRecPtr = CFE_ES_LocateMemPoolRecordByID(Handle);

    /* basic sanity check */
    if (!CFE_ES_MemPoolRecordIsMatch(PoolRecPtr, Handle))
    {
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    /*
     * Real work begins here.
     * If pool is mutex-protected, take the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemTake(PoolRecPtr->MutexId);
    }

    DataOffset = (cpuaddr)BufPtr - PoolRecPtr->BaseAddr;

    Status = CFE_ES_GenPoolGetBlockSize(&PoolRecPtr->Pool, &DataSize, DataOffset);

    /*
     * Real work ends here.
     * If pool is mutex-protected, release the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemGive(PoolRecPtr->MutexId);
    }

    if (Status == CFE_SUCCESS)
    {
        /*
         * Historically this function returns the size of the buffer
         * as an int32.  This is not workable for large (64 bit) pools.
         */
        Status = (int32)DataSize;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t Handle, CFE_ES_MemPoolBuf_t BufPtr)
{
    CFE_ES_MemPoolRecord_t *PoolRecPtr;
    size_t                  DataSize;
    size_t                  DataOffset;
    int32                   Status;

    if (BufPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    PoolRecPtr = CFE_ES_LocateMemPoolRecordByID(Handle);

    /* basic sanity check */
    if (!CFE_ES_MemPoolRecordIsMatch(PoolRecPtr, Handle))
    {
        CFE_ES_WriteToSysLog("%s: Err:Invalid Memory Handle (0x%08lX).\n", __func__, CFE_RESOURCEID_TO_ULONG(Handle));

        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    /*
     * Real work begins here.
     * If pool is mutex-protected, take the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemTake(PoolRecPtr->MutexId);
    }

    DataOffset = (cpuaddr)BufPtr - PoolRecPtr->BaseAddr;

    /*
     * Fundamental work is done as a generic routine.
     *
     * If successful, this gets an offset, which can then
     * be translated into a pointer to return to the caller.
     */
    Status = CFE_ES_GenPoolPutBlock(&PoolRecPtr->Pool, &DataSize, DataOffset);

    /*
     * Real work ends here.
     * If pool is mutex-protected, release the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemGive(PoolRecPtr->MutexId);
    }

    /*
     * If successful then modify return code to be
     * the size of the original buffer that was put (backward compatible)
     *
     * Otherwise if not successful, log the relevant detail
     */
    if (Status == CFE_SUCCESS)
    {
        Status = (int32)DataSize;
    }
    else if (Status == CFE_ES_POOL_BLOCK_INVALID)
    {
        CFE_ES_WriteToSysLog("%s: Err:Deallocating invalid or corrupt memory block @ 0x%08lX\n", __func__,
                             (unsigned long)BufPtr);
    }
    else if (Status == CFE_ES_BUFFER_NOT_IN_POOL)
    {
        CFE_ES_WriteToSysLog("%s: Err:Bad offset(%lu) outside pool boundary\n", __func__, (unsigned long)DataOffset);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t Handle)
{
    CFE_ES_AppId_t          AppId;
    CFE_ES_MemPoolRecord_t *PoolRecPtr;
    uint16                  NumBuckets;
    uint16                  Idx;

    if (BufPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    PoolRecPtr = CFE_ES_LocateMemPoolRecordByID(Handle);

    /* basic sanity check */
    if (!CFE_ES_MemPoolRecordIsMatch(PoolRecPtr, Handle))
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("%s: Err:Bad handle(0x%08lX) AppId=%lu\n", __func__, CFE_RESOURCEID_TO_ULONG(Handle),
                             CFE_RESOURCEID_TO_ULONG(AppId));
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    /*
     * Real work begins here.
     * If pool is mutex-protected, take the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemTake(PoolRecPtr->MutexId);
    }

    /*
     * Obtain the free and total byte count
     */
    CFE_ES_GenPoolGetUsage(&PoolRecPtr->Pool, &BufPtr->NumFreeBytes, &BufPtr->PoolSize);

    /*
     * Obtain the allocation and validation error counts
     */
    CFE_ES_GenPoolGetCounts(&PoolRecPtr->Pool, &NumBuckets, &BufPtr->NumBlocksRequested, &BufPtr->CheckErrCtr);

    for (Idx = 0; Idx < CFE_MISSION_ES_POOL_MAX_BUCKETS; ++Idx)
    {
        CFE_ES_GenPoolGetBucketUsage(&PoolRecPtr->Pool, NumBuckets, &BufPtr->BlockStats[Idx]);

        if (NumBuckets > 0)
        {
            --NumBuckets;
        }
    }

    /*
     * Real work ends here.
     * If pool is mutex-protected, release the mutex now.
     */
    if (OS_ObjectIdDefined(PoolRecPtr->MutexId))
    {
        OS_MutSemGive(PoolRecPtr->MutexId);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_ValidateHandle(CFE_ES_MemHandle_t Handle)
{
    CFE_ES_MemPoolRecord_t *PoolRecPtr;
    CFE_ES_MemOffset_t      TotalSize;

    /* Test #1) Handle must be valid */
    PoolRecPtr = CFE_ES_LocateMemPoolRecordByID(Handle);
    if (!CFE_ES_MemPoolRecordIsMatch(PoolRecPtr, Handle))
    {
        return false;
    }

    /* Test #2) Check critical internal fields are within reason */
    if (!CFE_ES_GenPoolValidateState(&PoolRecPtr->Pool))
    {
        return false;
    }

    /* Test #3) Check memory address in PSP (allows both RAM and EEPROM) */
    CFE_ES_GenPoolGetUsage(&PoolRecPtr->Pool, NULL, &TotalSize);
    if (CFE_PSP_MemValidateRange(PoolRecPtr->BaseAddr, CFE_ES_MEMOFFSET_TO_SIZET(TotalSize), CFE_PSP_MEM_ANY) !=
        CFE_PSP_SUCCESS)
    {
        return false;
    }

    return true;
}
