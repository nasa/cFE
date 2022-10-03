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
 * Contains data structure definitions used by the ES mempool implementation.
 *
 * The ES memory pools are now built on top of the generic memory pool implementation,
 * with a layer on top to translate into memory mapped buffer addresses.
 */

#ifndef CFE_ES_MEMPOOL_H
#define CFE_ES_MEMPOOL_H

/*
** Include Files
*/
#include "common_types.h"
#include "cfe_resourceid.h"
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
    CFE_ES_AppId_t OwnerAppID;

    /**
     * Optional Mutex for serializing get/put operations
     */
    osal_id_t MutexId;
} CFE_ES_MemPoolRecord_t;

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain an index value correlating to an ES Memory Pool ID
 *
 * This calculates a zero based integer value that may be used for indexing
 * into a local resource table/array.
 *
 * Index values are only guaranteed to be unique for resources of the same
 * type.  For instance, the indices corresponding to two [valid] Memory Pool
 * IDs will never overlap, but the index of a Memory Pool and a library ID
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

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the Pool table entry correlating with a given Pool ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for memory pools, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID _should_ reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_ES_MemPoolRecordIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_ES_MemPoolRecordIsMatch()
 *
 * @param[in]   PoolID   the Pool ID to locate
 * @return pointer to Pool Table entry for the given Pool ID
 */
CFE_ES_MemPoolRecord_t *CFE_ES_LocateMemPoolRecordByID(CFE_ES_MemHandle_t PoolID);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Memory Pool record is in use or free/empty
 *
 * This routine checks if the Pool table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_ES_MemPoolRecordIsUsed(const CFE_ES_MemPoolRecord_t *PoolRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(PoolRecPtr->PoolID);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a Memory Pool table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 * @returns PoolID of entry
 */
static inline CFE_ES_MemHandle_t CFE_ES_MemPoolRecordGetID(const CFE_ES_MemPoolRecord_t *PoolRecPtr)
{
    return PoolRecPtr->PoolID;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a Memory Pool table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Pool ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 * @param[in]   PendingId    the Pool ID of this entry
 */
static inline void CFE_ES_MemPoolRecordSetUsed(CFE_ES_MemPoolRecord_t *PoolRecPtr, CFE_ResourceId_t PendingId)
{
    PoolRecPtr->PoolID = CFE_ES_MEMHANDLE_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set a Memory Pool record table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 */
static inline void CFE_ES_MemPoolRecordSetFree(CFE_ES_MemPoolRecord_t *PoolRecPtr)
{
    PoolRecPtr->PoolID = CFE_ES_MEMHANDLE_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Mem Pool record is a match for the given Pool ID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Pool ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_ES_LocateMemPoolRecordByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_ES_LocateMemPoolRecordByID
 *
 * @param[in]   PoolRecPtr   pointer to Pool table entry
 * @param[in]   PoolID       expected Pool ID
 * @returns true if the entry matches the given pool ID
 */
static inline bool CFE_ES_MemPoolRecordIsMatch(const CFE_ES_MemPoolRecord_t *PoolRecPtr, CFE_ES_MemHandle_t PoolID)
{
    return (PoolRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(PoolRecPtr->PoolID, PoolID));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Pool ID table slot is used
 *
 * Checks if a table slot is available for a potential new ID
 * This is a helper function intended to be used with
 * CFE_ResourceId_FindNext() for allocating new IDs
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   CheckId       pending/candidate Pool ID to check
 * @returns true if the table slot for the ID is occupied, false if available
 */
bool CFE_ES_CheckMemPoolSlotUsed(CFE_ResourceId_t CheckId);

#endif /* CFE_ES_MEMPOOL_H */
