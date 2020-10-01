/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
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

/**
 * \file cfe_es_mempool.h
 *
 * Contains data structure definitions used by the ES mempool implementation.
 *
 * The ES memory pools are now built on top of the generic memory pool implementation,
 * with a layer on top to translate into memory mapped buffer addresses.
 */

#ifndef _CFE_ES_MEMPOOL_H_
#define _CFE_ES_MEMPOOL_H_

/*
** Include Files
*/
#include "common_types.h"
#include "cfe_es_generic_pool.h"

typedef struct
{
    /*
     * The generic pool structure
     * This must be the first entry in this structure.
     */
    CFE_ES_GenPoolRecord_t Pool;

    /*
     * The ID of this pool record
     */
    CFE_ES_MemHandle_t PoolID;

    /**
     * This indicates the start/base address
     * of the memory block.
     */
    cpuaddr BaseAddr;

    /**
     * The "owner" field stores the AppID of the creator of the pool.
     * If an exception or other event occurs that causes this app to exit,
     * this allows ES to also release the memory pool entry.
     *
     * It is still possible for pools to be created outside the context of
     * an ES app, but in that case the resource cannot be cleaned up if the
     * app exits unexpectedly.
     */
    CFE_ES_ResourceID_t OwnerAppID;

    /**
     * Optional Mutex for serializing get/put operations
     */
    osal_id_t          MutexId;
} CFE_ES_MemPoolRecord_t;


/**
 * @brief Obtain an index value correlating to an ES Memory Pool ID
 *
 * This calculates a zero based integer value that may be used for indexing
 * into a local resource table/array.
 *
 * Index values are only guaranteed to be unique for resources of the same
 * type.  For instance, the indices corresponding to two [valid] Memory Pool
 * IDs will never overlap, but the index of an Memory Pool and a library ID
 * may be the same.  Furthermore, indices may be reused if a resource is
 * deleted and re-created.
 *
 * @note There is no inverse of this function - indices cannot be converted
 * back to the original PoolID value.  The caller should retain the original ID
 * for future use.
 *
 * @param[in]   PoolID Memory Pool ID to convert
 * @param[out]  Idx    Buffer where the calculated index will be stored
 *
 * @return Execution status, see @ref CFEReturnCodes
 * @retval #CFE_SUCCESS                 @copybrief CFE_SUCCESS
 */
int32 CFE_ES_MemPoolID_ToIndex(CFE_ES_MemHandle_t PoolID, uint32 *Idx);

/**
 * @brief Locate the Pool table entry correlating with a given Pool ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   PoolID   the Pool ID to locate
 * @return pointer to Pool Table entry for the given Pool ID
 */
CFE_ES_MemPoolRecord_t* CFE_ES_LocateMemPoolRecordByID(CFE_ES_MemHandle_t PoolID);

/**
 * @brief Check if a Memory Pool record is in use or free/empty
 *
 * This routine checks if the Pool table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_ES_MemPoolRecordIsUsed(const CFE_ES_MemPoolRecord_t *PoolRecPtr)
{
    return CFE_ES_ResourceID_IsDefined(PoolRecPtr->PoolID);
}

/**
 * @brief Get the ID value from a Memory Pool table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 * @returns PoolID of entry
 */
static inline CFE_ES_MemHandle_t CFE_ES_MemPoolRecordGetID(const CFE_ES_MemPoolRecord_t *PoolRecPtr)
{
    return (PoolRecPtr->PoolID);
}

/**
 * @brief Marks a Memory Pool table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Pool ID.
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 * @param[in]   PoolID       the Pool ID of this entry
 */
static inline void CFE_ES_MemPoolRecordSetUsed(CFE_ES_MemPoolRecord_t *PoolRecPtr, CFE_ES_MemHandle_t PoolID)
{
    PoolRecPtr->PoolID = PoolID;
}

/**
 * @brief Set a Memory Pool record table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 */
static inline void CFE_ES_MemPoolRecordSetFree(CFE_ES_MemPoolRecord_t *PoolRecPtr)
{
    PoolRecPtr->PoolID = CFE_ES_RESOURCEID_UNDEFINED;
}


static inline bool CFE_ES_MemPoolRecordIsMatch(const CFE_ES_MemPoolRecord_t *PoolRecPtr, CFE_ES_MemHandle_t PoolID)
{
    return (PoolRecPtr != NULL && CFE_ES_ResourceID_Equal(PoolRecPtr->PoolID, PoolID));
}



#endif  /* _CFE_ES_MEMPOOL_H_ */
