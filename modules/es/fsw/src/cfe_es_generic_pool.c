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
**  cfe_es_generic_pool.c
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

/*****************************************************************************/
/*
** Functions
*/

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Find the appropriate bucket given a requested block size
 *
 *-----------------------------------------------------------------*/
uint16 CFE_ES_GenPoolFindBucket(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t ReqSize)
{
    uint16 Index;

    for (Index = 0; Index < PoolRecPtr->NumBuckets; ++Index)
    {
        if (ReqSize <= PoolRecPtr->Buckets[Index].BlockSize)
        {
            /* it fits - stop here */
            break;
        }
    }

    /*
     * Invert output such that if a bucket wasn't found, this
     * will return 0.  A valid bucket ID will be nonzero.
     */
    return (PoolRecPtr->NumBuckets - Index);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Obtain a pointer to the state structure associated with a given bucket ID
 *
 *-----------------------------------------------------------------*/
CFE_ES_GenPoolBucket_t *CFE_ES_GenPoolGetBucketState(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 BucketId)
{
    uint16 Index;

    Index = PoolRecPtr->NumBuckets - BucketId;
    if (Index >= PoolRecPtr->NumBuckets)
    {
        return NULL;
    }

    return &PoolRecPtr->Buckets[Index];
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Find and re-allocate a previously returned block
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenPoolRecyclePoolBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 BucketId, size_t NewSize,
                                     size_t *BlockOffsetPtr)
{
    CFE_ES_GenPoolBucket_t *BucketPtr;
    size_t                  DescOffset;
    size_t                  BlockOffset;
    size_t                  NextOffset;
    CFE_ES_GenPoolBD_t *    BdPtr;
    uint16                  RecycleBucketId;
    int32                   Status;

    BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);
    if (BucketPtr == NULL || BucketPtr->RecycleCount == BucketPtr->ReleaseCount || BucketPtr->FirstOffset == 0)
    {
        /* no buffers in pool to recycle */
        return CFE_ES_BUFFER_NOT_IN_POOL;
    }

    BlockOffset = BucketPtr->FirstOffset;
    DescOffset  = BlockOffset - CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE;
    Status      = PoolRecPtr->Retrieve(PoolRecPtr, DescOffset, &BdPtr);
    if (Status == CFE_SUCCESS)
    {
        RecycleBucketId = BdPtr->Allocated - CFE_ES_MEMORY_DEALLOCATED;
        if (BdPtr->CheckBits != CFE_ES_CHECK_PATTERN || RecycleBucketId != BucketId)
        {
            /* sanity check failed - possible pool corruption? */
            Status = CFE_ES_BUFFER_NOT_IN_POOL;
        }
        else
        {
            /*
             * Get it off the top on the list
             */
            NextOffset = BdPtr->NextOffset;

            BdPtr->Allocated  = CFE_ES_MEMORY_ALLOCATED + BucketId; /* Flag memory block as allocated */
            BdPtr->ActualSize = NewSize;
            BdPtr->NextOffset = 0;

            Status = PoolRecPtr->Commit(PoolRecPtr, DescOffset, BdPtr);
            if (Status == CFE_SUCCESS)
            {
                *BlockOffsetPtr        = BlockOffset;
                BucketPtr->FirstOffset = NextOffset;
                ++BucketPtr->RecycleCount;
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Create a new block of the given size
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenPoolCreatePoolBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 BucketId, size_t NewSize,
                                    size_t *BlockOffsetPtr)
{
    CFE_ES_GenPoolBucket_t *BucketPtr;
    size_t                  DescOffset;
    size_t                  BlockOffset;
    size_t                  NextTailPosition;
    CFE_ES_GenPoolBD_t *    BdPtr;
    int32                   Status;

    BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);
    if (BucketPtr == NULL)
    {
        /* no buffers in pool to create */
        return CFE_ES_BUFFER_NOT_IN_POOL;
    }

    /*
     * Determine the offsets of the new user block,
     * which must be aligned according to the AlignMask member.
     *
     * Note - just pre-calculating offsets here, nothing is committed yet.
     */
    BlockOffset = PoolRecPtr->TailPosition + CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE;
    BlockOffset += PoolRecPtr->AlignMask;
    BlockOffset &= ~PoolRecPtr->AlignMask;

    DescOffset       = BlockOffset - CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE;
    NextTailPosition = BlockOffset + BucketPtr->BlockSize;

    /*
     * Check if there is enough space remaining in the pool -- the
     * proposed start address plus the block size must not exceed the pool end.
     */
    if (NextTailPosition > PoolRecPtr->PoolMaxOffset)
    {
        /* can't fit in remaining mem */
        return CFE_ES_ERR_MEM_BLOCK_SIZE;
    }

    /*
     * Now commit the new block
     */
    Status = PoolRecPtr->Retrieve(PoolRecPtr, DescOffset, &BdPtr);
    if (Status == CFE_SUCCESS)
    {
        BdPtr->CheckBits  = CFE_ES_CHECK_PATTERN;
        BdPtr->Allocated  = CFE_ES_MEMORY_ALLOCATED + BucketId; /* Flag memory block as allocated */
        BdPtr->ActualSize = NewSize;
        BdPtr->NextOffset = 0;

        Status = PoolRecPtr->Commit(PoolRecPtr, DescOffset, BdPtr);
        if (Status == CFE_SUCCESS)
        {
            /*
            ** adjust pool current pointer and other record keeping
            */
            PoolRecPtr->TailPosition = NextTailPosition;
            ++BucketPtr->AllocationCount;
            ++PoolRecPtr->AllocationCount;

            *BlockOffsetPtr = BlockOffset;
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenPoolInitialize(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t StartOffset, size_t PoolSize,
                               size_t AlignSize, uint16 NumBlockSizes, const size_t *BlockSizeList,
                               CFE_ES_PoolRetrieve_Func_t RetrieveFunc, CFE_ES_PoolCommit_Func_t CommitFunc)
{
    cpuaddr                 AlignMask;
    uint32                  i;
    uint32                  j;
    CFE_ES_GenPoolBucket_t *BucketPtr;

    /*
     * Note - being an internal/non-public API this does not need to
     * check the directly-supplied arguments, it is assumed they are already
     * sanity checked.
     */
    memset(PoolRecPtr, 0, sizeof(*PoolRecPtr));

    /*
     * Convert alignment to a bit mask.
     */
    if (AlignSize <= 1)
    {
        AlignMask = 0;
    }
    else
    {
        AlignMask = AlignSize - 1;
    }

    /*
     * This confirms that the passed in value is a power of 2.
     * The result of this check should always be 0 if so.
     */
    if ((AlignMask & AlignSize) != 0)
    {
        CFE_ES_WriteToSysLog("%s: invalid alignment for pool: %lu\n", __func__, (unsigned long)AlignSize);
        return CFE_ES_BAD_ARGUMENT;
    }

    /* complete initialization of pool record entry */
    PoolRecPtr->AlignMask     = AlignMask;
    PoolRecPtr->PoolTotalSize = PoolSize;
    PoolRecPtr->PoolMaxOffset = PoolSize + StartOffset;
    PoolRecPtr->NumBuckets    = NumBlockSizes;
    PoolRecPtr->Retrieve      = RetrieveFunc;
    PoolRecPtr->Commit        = CommitFunc;
    PoolRecPtr->TailPosition  = StartOffset;

    /* initially copy all block sizes */
    BucketPtr = PoolRecPtr->Buckets;
    for (i = 0; i < NumBlockSizes; ++i)
    {
        BucketPtr->BlockSize = BlockSizeList[i];
        ++BucketPtr;
    }

    /* Sort by block size - a simple bubble sort -
     * this does not run often and the list is relatively small. */
    do
    {
        j         = 0;
        BucketPtr = PoolRecPtr->Buckets;
        for (i = 1; i < NumBlockSizes; ++i)
        {
            if (BucketPtr[0].BlockSize > BucketPtr[1].BlockSize)
            {
                /* swap */
                BucketPtr[0].BlockSize ^= BucketPtr[1].BlockSize;
                BucketPtr[1].BlockSize ^= BucketPtr[0].BlockSize;
                BucketPtr[0].BlockSize ^= BucketPtr[1].BlockSize;
                ++j;
            }
            ++BucketPtr;
        }
    } while (j > 0);

    /*
     * Additional sanity check - after sorting the list,
     * confirm that the smallest block size (first entry)
     * is not zero.
     */
    if (PoolRecPtr->Buckets[0].BlockSize == 0)
    {
        return CFE_ES_ERR_MEM_BLOCK_SIZE;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
size_t CFE_ES_GenPoolCalcMinSize(uint16 NumBlockSizes, const size_t *BlockSizeList, uint32 NumBlocks)
{
    uint16 BucketId;
    size_t MinBlockSize;

    MinBlockSize = 0;

    if (NumBlockSizes > 0)
    {
        MinBlockSize = BlockSizeList[0];
        for (BucketId = 1; BucketId < NumBlockSizes; ++BucketId)
        {
            if (BlockSizeList[BucketId] < MinBlockSize)
            {
                MinBlockSize = BlockSizeList[BucketId];
            }
        }
    }

    MinBlockSize += sizeof(CFE_ES_GenPoolBD_t);

    return (NumBlocks * MinBlockSize);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenPoolGetBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t *BlockOffsetPtr, size_t ReqSize)
{
    int32  Status;
    uint16 BucketId;

    /* Find the bucket which can accommodate the requested size. */
    BucketId = CFE_ES_GenPoolFindBucket(PoolRecPtr, ReqSize);
    if (BucketId == 0)
    {
        CFE_ES_WriteToSysLog("%s: Err:size(%lu) > max(%lu)\n", __func__, (unsigned long)ReqSize,
                             (unsigned long)PoolRecPtr->Buckets[PoolRecPtr->NumBuckets - 1].BlockSize);
        return CFE_ES_ERR_MEM_BLOCK_SIZE;
    }

    /* first attempt to recycle any buffers from the same bucket that were freed */
    Status = CFE_ES_GenPoolRecyclePoolBlock(PoolRecPtr, BucketId, ReqSize, BlockOffsetPtr);
    if (Status != CFE_SUCCESS)
    {
        /* recycling not available - try making a new one instead */
        Status = CFE_ES_GenPoolCreatePoolBlock(PoolRecPtr, BucketId, ReqSize, BlockOffsetPtr);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenPoolGetBlockSize(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t *BlockSizePtr, size_t BlockOffset)
{
    size_t                  DescOffset;
    CFE_ES_GenPoolBucket_t *BucketPtr;
    CFE_ES_GenPoolBD_t *    BdPtr;
    int32                   Status;
    uint16                  BucketId;

    if (BlockOffset >= PoolRecPtr->TailPosition || BlockOffset < CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE)
    {
        /* outside the bounds of the pool */
        return CFE_ES_BUFFER_NOT_IN_POOL;
    }

    DescOffset = BlockOffset - CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE;

    Status = PoolRecPtr->Retrieve(PoolRecPtr, DescOffset, &BdPtr);
    if (Status == CFE_SUCCESS)
    {
        BucketId  = BdPtr->Allocated - CFE_ES_MEMORY_ALLOCATED;
        BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);

        if (BdPtr->CheckBits != CFE_ES_CHECK_PATTERN || BucketPtr == NULL || BdPtr->ActualSize == 0 ||
            BucketPtr->BlockSize < BdPtr->ActualSize)
        {
            /* This does not appear to be a valid data buffer */
            Status = CFE_ES_POOL_BLOCK_INVALID;
        }
        else
        {
            *BlockSizePtr = BdPtr->ActualSize;
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenPoolPutBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t *BlockSizePtr, size_t BlockOffset)
{
    size_t                  DescOffset;
    CFE_ES_GenPoolBucket_t *BucketPtr;
    CFE_ES_GenPoolBD_t *    BdPtr;
    int32                   Status;
    uint16                  BucketId;

    if (BlockOffset >= PoolRecPtr->TailPosition || BlockOffset < CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE)
    {
        /* outside the bounds of the pool */
        return CFE_ES_BUFFER_NOT_IN_POOL;
    }

    DescOffset = BlockOffset - CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE;

    Status = PoolRecPtr->Retrieve(PoolRecPtr, DescOffset, &BdPtr);
    if (Status == CFE_SUCCESS)
    {
        BucketId  = BdPtr->Allocated - CFE_ES_MEMORY_ALLOCATED;
        BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);

        if (BdPtr->CheckBits != CFE_ES_CHECK_PATTERN || BucketPtr == NULL || BdPtr->ActualSize == 0 ||
            BucketPtr->BlockSize < BdPtr->ActualSize)
        {
            /* This does not appear to be a valid data buffer */
            ++PoolRecPtr->ValidationErrorCount;
            Status = CFE_ES_POOL_BLOCK_INVALID;
        }
        else
        {
            BdPtr->Allocated  = CFE_ES_MEMORY_DEALLOCATED + BucketId;
            BdPtr->NextOffset = BucketPtr->FirstOffset;
            *BlockSizePtr     = BdPtr->ActualSize;

            Status = PoolRecPtr->Commit(PoolRecPtr, DescOffset, BdPtr);
            if (Status == CFE_SUCCESS)
            {
                BucketPtr->FirstOffset = BlockOffset;
                ++BucketPtr->ReleaseCount;
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GenPoolRebuild(CFE_ES_GenPoolRecord_t *PoolRecPtr)
{
    int32                   Status;
    size_t                  DescOffset;
    size_t                  BlockOffset;
    CFE_ES_GenPoolBucket_t *BucketPtr;
    CFE_ES_GenPoolBD_t *    BdPtr;
    uint16                  BucketId;
    bool                    IsDeallocatedBlock;

    Status = CFE_SUCCESS;

    /* Scan the pool to find blocks that were created and freed */
    while (true)
    {
        IsDeallocatedBlock = false;
        BucketId           = 0;
        BucketPtr          = NULL;

        /*
         * Determine the offsets of the next user block,
         * which must be aligned according to the AlignMask member.
         */
        BlockOffset = PoolRecPtr->TailPosition + CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE;
        BlockOffset += PoolRecPtr->AlignMask;
        BlockOffset &= ~PoolRecPtr->AlignMask;

        if (BlockOffset > PoolRecPtr->PoolMaxOffset)
        {
            /* End of pool reached, stop now */
            break;
        }

        DescOffset = BlockOffset - CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE;
        Status     = PoolRecPtr->Retrieve(PoolRecPtr, DescOffset, &BdPtr);
        if (Status != CFE_SUCCESS)
        {
            /* Failed to read descriptor */
            break;
        }

        /*
         * If the CheckBits indicate the block was in use,
         * then do further inspection to find the block size
         * and allocated/deallocated status.
         */
        if (BdPtr->CheckBits == CFE_ES_CHECK_PATTERN)
        {
            /* Test if block is deallocated */
            BucketId  = BdPtr->Allocated - CFE_ES_MEMORY_DEALLOCATED;
            BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);
            if (BucketPtr != 0)
            {
                IsDeallocatedBlock = true;
            }
            else
            {
                /*
                 * Test if block is allocated.
                 * In this case there is nothing more to do, just
                 * get the size and skip the block.
                 */
                BucketId  = BdPtr->Allocated - CFE_ES_MEMORY_ALLOCATED;
                BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);
            }
        }

        /*
         * Sanity check that the actual size is less than the bucket size -
         * it always should be, as long as the pool was created with the same
         * set of bucket sizes.
         */
        if (BucketPtr == NULL || BucketPtr->BlockSize < BdPtr->ActualSize)
        {
            /* Not a valid block signature - stop recovery now */
            break;
        }

        PoolRecPtr->TailPosition = BlockOffset + BucketPtr->BlockSize;

        BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);
        ++BucketPtr->AllocationCount;
        ++PoolRecPtr->AllocationCount;

        /*
         * If it was a deallocated block, then add it to the local
         * pool linked list structure and rewrite the descriptor.
         */
        if (IsDeallocatedBlock)
        {
            ++BucketPtr->ReleaseCount;
            BdPtr->NextOffset      = BucketPtr->FirstOffset;
            BucketPtr->FirstOffset = BlockOffset;
            Status                 = PoolRecPtr->Commit(PoolRecPtr, DescOffset, BdPtr);
            if (Status != CFE_SUCCESS)
            {
                break;
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_GenPoolValidateState(const CFE_ES_GenPoolRecord_t *PoolRecPtr)
{
    return (PoolRecPtr->PoolTotalSize > 0 && PoolRecPtr->TailPosition <= PoolRecPtr->PoolMaxOffset &&
            PoolRecPtr->NumBuckets > 0 && PoolRecPtr->NumBuckets <= CFE_PLATFORM_ES_POOL_MAX_BUCKETS);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_GenPoolGetUsage(CFE_ES_GenPoolRecord_t *PoolRecPtr, CFE_ES_MemOffset_t *FreeSizeBuf,
                            CFE_ES_MemOffset_t *TotalSizeBuf)
{
    if (TotalSizeBuf != NULL)
    {
        *TotalSizeBuf = CFE_ES_MEMOFFSET_C(PoolRecPtr->PoolTotalSize);
    }
    if (FreeSizeBuf != NULL)
    {
        *FreeSizeBuf = CFE_ES_MEMOFFSET_C(PoolRecPtr->PoolMaxOffset - PoolRecPtr->TailPosition);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_GenPoolGetCounts(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 *NumBucketsBuf, uint32 *AllocCountBuf,
                             uint32 *ValidationErrorCountBuf)
{
    if (NumBucketsBuf != NULL)
    {
        *NumBucketsBuf = PoolRecPtr->NumBuckets;
    }
    if (AllocCountBuf != NULL)
    {
        *AllocCountBuf = PoolRecPtr->AllocationCount;
    }
    if (ValidationErrorCountBuf != NULL)
    {
        *ValidationErrorCountBuf = PoolRecPtr->ValidationErrorCount;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_GenPoolGetBucketUsage(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 BucketId,
                                  CFE_ES_BlockStats_t *BlockStatsBuf)
{
    const CFE_ES_GenPoolBucket_t *      BucketPtr;
    static const CFE_ES_GenPoolBucket_t ZeroBucket = {0};

    BucketPtr = CFE_ES_GenPoolGetBucketState(PoolRecPtr, BucketId);
    if (BucketPtr == NULL)
    {
        /* bucket ID is not valid */
        BucketPtr = &ZeroBucket;
    }

    if (BlockStatsBuf != NULL)
    {
        BlockStatsBuf->NumCreated = BucketPtr->AllocationCount;
        BlockStatsBuf->BlockSize  = CFE_ES_MEMOFFSET_C(BucketPtr->BlockSize);
        BlockStatsBuf->NumFree    = BucketPtr->ReleaseCount - BucketPtr->RecycleCount;
    }
}
