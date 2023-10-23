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

/**
 * @file
 *
 *  Purpose:
 *  This file contains the Internal interface for the cFE Critical Data Store
 *  memory pool functions.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */

#ifndef CFE_ES_GENERIC_POOL_H
#define CFE_ES_GENERIC_POOL_H

/*
** Include Files
*/
#include "common_types.h"

/*
** Macro Definitions
*/
#define CFE_ES_CHECK_PATTERN      ((uint16)0x5a5a)
#define CFE_ES_MEMORY_ALLOCATED   ((uint16)0xaaaa)
#define CFE_ES_MEMORY_DEALLOCATED ((uint16)0xdddd)

#define CFE_ES_GENERIC_POOL_DESCRIPTOR_SIZE \
    sizeof(CFE_ES_GenPoolBD_t) /* amount of space to reserve with every allocation */

/*
** Type Definitions
*/

typedef struct CFE_ES_GenPoolBD
{
    uint16 CheckBits;  /**< Set to a fixed bit pattern after init */
    uint16 Allocated;  /**< Set to a bit pattern depending on allocation state */
    size_t ActualSize; /**< The actual requested size of the block */
    size_t NextOffset; /**< The offset of the next descriptor in the free stack */
} CFE_ES_GenPoolBD_t;

typedef struct CFE_ES_GenPoolBucket
{
    size_t BlockSize;
    size_t FirstOffset;     /**< Top of the "free stack" of buffers which have been returned */
    uint32 AllocationCount; /**< Total number of buffers of this block size that exist (initial get) */
    uint32 ReleaseCount;    /**< Total number of buffers that have been released (put back) */
    uint32 RecycleCount;    /**< Total number of buffers that have been recycled (get after put) */
} CFE_ES_GenPoolBucket_t;

/*
 * Forward struct typedef so it can be used in retrieve/commit prototype
 */
typedef struct CFE_ES_GenPoolRecord CFE_ES_GenPoolRecord_t;

/**
 * \brief Function to retrieve a buffer descriptor from the pool storage
 *
 * The generic pool implementation does not assume that buffers can be
 * directly accessed as memory.  This routine obtains a reference to
 * the descriptor data.  On memory mapped pools it may output a direct
 * pointer to the data instead of copying it.
 */
typedef int32 (*CFE_ES_PoolRetrieve_Func_t)(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset,
                                            CFE_ES_GenPoolBD_t **BdPtr);

/**
 * \brief Function to commit a buffer descriptor to the pool storage
 *
 * The generic pool implementation does not assume that buffers can be
 * directly accessed as memory.  This routine writes data back to pool
 * storage.  It may be a no-op for memory mapped pools.
 */
typedef int32 (*CFE_ES_PoolCommit_Func_t)(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset,
                                          const CFE_ES_GenPoolBD_t *BdPtr);

/**
 * \brief Generic Memory Pool Type
 */
struct CFE_ES_GenPoolRecord
{
    size_t PoolTotalSize; /**< Total size of the pool area, in bytes */
    size_t PoolMaxOffset; /**< End offset (position) of the pool */
    size_t AlignMask;     /**< Alignment mask applied to all new allocations */
    size_t TailPosition;  /**< Current high watermark of the pool, end of last allocation */

    CFE_ES_PoolRetrieve_Func_t Retrieve; /**< Function to access a buffer descriptor in the pool storage */
    CFE_ES_PoolCommit_Func_t   Commit;   /**< Function to commit a buffer descriptor to the pool storage */

    uint32 AllocationCount;      /**< Total number of block allocations of any size */
    uint32 ValidationErrorCount; /**< Count of validation errors */

    uint16                 NumBuckets; /**< Number of entries in the "Buckets" array that are valid */
    CFE_ES_GenPoolBucket_t Buckets[CFE_PLATFORM_ES_POOL_MAX_BUCKETS]; /**< Bucket States */
};

/*****************************************************************************/
/*
** Function prototypes
*/

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Initialize a generic pool structure
 *
 * Resets the pool to its initial state, given the size
 * and alignment specifications.
 *
 * \param[out]  PoolRecPtr    Pointer to pool structure
 * \param[in]   StartOffset   Initial starting location of pool
 * \param[in]   PoolSize      Size of pool (beyond start offset)
 * \param[in]   AlignSize     Required Alignment of blocks
 * \param[in]   NumBlockSizes Number of entries in the BlockSizeList
 * \param[in]   BlockSizeList Size of pool blocks
 * \param[in]   RetrieveFunc  Function to retrieve buffer descriptors
 * \param[in]   CommitFunc    Function to commit buffer descriptors
 *
 * \return #CFE_SUCCESS, or error code \ref CFEReturnCodes
 */
int32 CFE_ES_GenPoolInitialize(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t StartOffset, size_t PoolSize,
                               size_t AlignSize, uint16 NumBlockSizes, const size_t *BlockSizeList,
                               CFE_ES_PoolRetrieve_Func_t RetrieveFunc, CFE_ES_PoolCommit_Func_t CommitFunc);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Gets a block from the pool
 *
 * This may recycle a previously returned block or allocate
 * a new block, depending on availability.
 *
 * \param[inout] PoolRecPtr     Pointer to pool structure
 * \param[out]   BlockOffsetPtr  Location to output new block offset
 * \param[in]    ReqSize        Size of block requested
 *
 * \return #CFE_SUCCESS, or error code \ref CFEReturnCodes
 */
int32 CFE_ES_GenPoolGetBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t *BlockOffsetPtr, size_t ReqSize);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Create a new block of the given size.
 *
 * \note Internal helper routine only, not part of API.
 *
 * \param[inout] PoolRecPtr      Pointer to pool structure
 * \param[in]    BucketId        Bucket ID
 * \param[in]    NewSize         Size of block
 * \param[out]   BlockOffsetPtr  Location to output new block offset 
 *
 * \return #CFE_SUCCESS, or error code #CFE_ES_BUFFER_NOT_IN_POOL #CFE_ES_ERR_MEM_BLOCK_SIZE
 *         \ref CFEReturnCodes
 */
int32 CFE_ES_GenPoolCreatePoolBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 BucketId, size_t NewSize, 
                                    size_t *BlockOffsetPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Find and re-allocate a previously returned block
 *
 * \note Internal helper routine only, not part of API.
 *
 * \param[inout] PoolRecPtr      Pointer to pool structure
 * \param[in]    BucketId        Bucket ID
 * \param[in]    NewSize         Size of block
 * \param[out]   BlockOffsetPtr  Location to output new block offset 
 *
 * \return #CFE_SUCCESS, or error code #CFE_ES_BUFFER_NOT_IN_POOL \ref CFEReturnCodes
 */
int32 CFE_ES_GenPoolRecyclePoolBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 BucketId, size_t NewSize,
                                     size_t *BlockOffsetPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Returns a block to the pool
 *
 * This marks the previously allocated block as deallocated,
 * and allows it to be recycled on a future get request.
 *
 * \param[inout] PoolRecPtr     Pointer to pool structure
 * \param[out]   BlockSizePtr   Location to output original allocation size
 * \param[in]    BlockOffset    Offset of data block
 *
 * \return #CFE_SUCCESS, or error code \ref CFEReturnCodes
 */
int32 CFE_ES_GenPoolPutBlock(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t *BlockSizePtr, size_t BlockOffset);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Rebuild list of free blocks in pool
 *
 * If pools are stored in a nonvolatile memory area, then it is
 * possible to resume pool operation from a previously initialized
 * pool.  This function attempts to restore the state of the pool
 * by scanning for allocated and deallocated block markers.
 *
 * Before using this function, one should call CFE_ES_GenPoolInitialize()
 * to first configure the basic pool structure and block size list.
 *
 * This function will then attempt to recreate the internal free lists
 * based on descriptors/signatures already existing in the memory area.
 *
 * \param[inout] PoolRecPtr     Pointer to pool structure
 *
 * \return #CFE_SUCCESS, or error code \ref CFEReturnCodes
 */
int32 CFE_ES_GenPoolRebuild(CFE_ES_GenPoolRecord_t *PoolRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Get size of pool block
 *
 * Given a previously allocated block, look up its descriptor information
 * and return the actual size.
 *
 * \param[inout] PoolRecPtr     Pointer to pool structure
 * \param[out]   BlockSizePtr   Location to output original allocation size
 * \param[in]    BlockOffset    Offset of data block
 *
 * \return #CFE_SUCCESS, or error code \ref CFEReturnCodes
 */
int32 CFE_ES_GenPoolGetBlockSize(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t *BlockSizePtr, size_t BlockOffset);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Validate a pool structure
 *
 * Perform basic sanity checks on the pool internal data.
 *
 * \param[in] PoolRecPtr     Pointer to pool structure
 *
 * \return #CFE_SUCCESS, or error code \ref CFEReturnCodes
 */
bool CFE_ES_GenPoolValidateState(const CFE_ES_GenPoolRecord_t *PoolRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Query basic usage of the pool structure
 *
 * Obtain basic pool usage info for telemetry/statistics reporting.
 *
 * \param[in]  PoolRecPtr     Pointer to pool structure
 * \param[out] FreeSizeBuf    Buffer to store free size
 * \param[out] TotalSizeBuf   Buffer to store total size
 *
 * \note This function is intended for telemetry purposes, so it
 * uses the message size type (CFE_ES_MemOffset_t) rather
 * than size_t, to be compatible with the type used in telemetry
 * messages.
 */
void CFE_ES_GenPoolGetUsage(CFE_ES_GenPoolRecord_t *PoolRecPtr, CFE_ES_MemOffset_t *FreeSizeBuf,
                            CFE_ES_MemOffset_t *TotalSizeBuf);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Query counters associated with the pool structure
 *
 * Obtain pool counters for telemetry/statistics reporting.
 *
 * \param[in]  PoolRecPtr     Pointer to pool structure
 * \param[out] NumBucketsBuf  Buffer to store bucket count
 * \param[out] AllocCountBuf  Buffer to store allocation count
 * \param[out] ValidationErrorCountBuf  Buffer to store validation error count
 */
void CFE_ES_GenPoolGetCounts(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 *NumBucketsBuf, uint32 *AllocCountBuf,
                             uint32 *ValidationErrorCountBuf);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Query bucket-specific usage of the pool structure
 *
 * Obtain pool per-bucket stats for telemetry/statistics reporting.
 *
 * If the bucket number is not valid, this sets all output values to zero.
 *
 * \param[in]  PoolRecPtr     Pointer to pool structure
 * \param[in]  BucketId       Bucket number (non-zero)
 * \param[out] BlockStatsBuf  Buffer to store block stats
 */
void CFE_ES_GenPoolGetBucketUsage(CFE_ES_GenPoolRecord_t *PoolRecPtr, uint16 BucketId,
                                  CFE_ES_BlockStats_t *BlockStatsBuf);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Calculate the pool size required for the specified number of blocks
 *
 * Given a block size list, determine the amount of bytes required to allocate
 * the requested number of minimally-sized blocks, including descriptor overhead.
 *
 * \note This is intended only as a sanity check on pool sizes, and does not
 * guarantee the ability to actually allocate buffers in a real pool. In particular,
 * alignment is not factored into the size calculation, and this may require
 * some additional overhead.
 *
 * \param[in] NumBlockSizes     Number of entries in BlockSizeList
 * \param[in] BlockSizeList     Size of pool blocks
 * \param[in] NumBlocks         Number of blocks
 *
 * \return Minimum size required for requested number of blocks.
 */
size_t CFE_ES_GenPoolCalcMinSize(uint16 NumBlockSizes, const size_t *BlockSizeList, uint32 NumBlocks);

#endif /* CFE_ES_GENERIC_POOL_H */
