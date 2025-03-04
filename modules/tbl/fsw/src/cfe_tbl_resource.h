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
 * Contains basic prototypes and definitions related to CFE TBL resource
 * management and related resource IDs.
 *
 * A CFE TBL Resource ID is a common way to identify CFE-managed resources such
 * as registry entries, buffers, state records, and other entities.
 *
 * ABOUT RESOURCE TABLE ACCESSORS
 * ==============================
 *
 * These accessors facilitate consistent lookup/matching/allocation/deallocation patterns
 * across all TBL resources.  The following types of resources can be managed in this
 * fashion:
 *
 *  - Access Descriptors (Table Handles, external identifiers)
 *  - Registry Records (Table registry, internal identifiers)
 *  - Load Buffers (both shared and table-specific)
 *  - Validation Results
 *  - Dump State
 *  - CDS registries
 *
 * A full set of accessors contains the following basic methods:
 *
 * | **Method**  | **Description**                                       |
 * |:------------|:------------------------------------------------------|
 * | LocateByID  | Returns a pointer to the entry associated with an ID  |
 * | ToIndex     | Converts an entry ID to a 0-based array index         |
 * | IsUsed      | Checks if a given entry is currently in use           |
 * | SetUsed     | Sets an entry as being in use / not available         |
 * | SetFree     | Sets an entry as being available / not in use         |
 * | GetId       | Gets the resource ID associated with an entry pointer |
 * | IsMatch     | Checks if an entry pointer is a match to the given ID |
 * | GetNext     | Returns the next/pending ID suitable for a new record |
 *
 * This file should implement each method for each supported resource type that
 * implements these access patterns.
 *
 */

#ifndef CFE_TBL_RESOURCE_H
#define CFE_TBL_RESOURCE_H

/*
** Include Files
*/
#include "cfe_resourceid.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_tbl_task.h"

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  VALIDATION RESULT TABLE ACCESSORS ~~~
 *
 * These operate on CFE_TBL_ValidationResult_t* and CFE_TBL_ValidationResultId_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the validation result table entry correlating with a given registry ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for applications, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_TBL_ValidationResultIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_ValidationResultIsMatch()
 *
 * @param[in]   ValResultId   the registry ID to locate
 * @return pointer to Validation Result Table entry for the given registry ID, or NULL if out of range
 */
CFE_TBL_ValidationResult_t *CFE_TBL_LocateValidationResultByID(CFE_TBL_ValidationResultId_t ValResultId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a Validation Result ID
 *
 * Calculates the array position/index of the global array entry for
 * the given result ID.
 *
 * @param[in]  ValResultId the ID of the validation result entry to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_ValidationResultId_ToIndex(CFE_TBL_ValidationResultId_t ValResultId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a validation result table entry is in use or free/empty
 *
 * This routine checks if the table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to validation result table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_ValidationResultIsUsed(const CFE_TBL_ValidationResult_t *BuffPtr)
{
    return (CFE_RESOURCEID_TEST_DEFINED(BuffPtr->ValId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a validation result table entry as in use (not avaliable)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given validation result ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to validation result table entry
 * @param[in]   PendingId        the ID of this entry that will be set
 */
static inline void CFE_TBL_ValidationResultSetUsed(CFE_TBL_ValidationResult_t *BuffPtr, CFE_ResourceId_t PendingId)
{
    BuffPtr->ValId = CFE_TBL_VALRESULTID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a validation result table entry as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any validation result ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to validation result table entry
 */
static inline void CFE_TBL_ValidationResultSetFree(CFE_TBL_ValidationResult_t *BuffPtr)
{
    BuffPtr->State = CFE_TBL_VALIDATION_FREE; /* for backward compatibility; not part of "IsUsed" check anymore */
    BuffPtr->ValId = CFE_TBL_VALRESULTID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a validation result table entry
 *
 * This routine converts the table entry pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to table entry
 * @returns ID of entry
 */
static inline CFE_TBL_ValidationResultId_t CFE_TBL_ValidationResultGetId(const CFE_TBL_ValidationResult_t *BuffPtr)
{
    return BuffPtr->ValId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a validation result entry is a match for the given ID
 *
 * This routine confirms that the previously-located result record is valid
 * and matches the expected validation result ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateValidationResultByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateValidationResultByID
 *
 * @param[in]   BuffPtr   pointer to validation result table entry, or NULL
 * @param[in]   ValId     expected validation result ID
 * @returns true if the entry matches the given ID
 */
static inline bool CFE_TBL_ValidationResultIsMatch(const CFE_TBL_ValidationResult_t *BuffPtr,
                                                   CFE_TBL_ValidationResultId_t      ValId)
{
    return (BuffPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(BuffPtr->ValId, ValId));
}

/**
 * @brief Determine the next ID to use for validation results
 *
 * Obtains an ID value that is usable for a new validation result.  If no validation
 * result entries are available, then UNDEFINED is returned.
 *
 * @returns ID to use for next result, or UNDEFINED if no slots available
 */
CFE_ResourceId_t CFE_TBL_GetNextValResultBlock(void);

/**
 * Test if a slot corresponding to a pending ID is used
 *
 * This is an internal helper function for CFE_ResourceId_FindNext(), and not
 * typically called directly. It is prototyped here for unit testing.
 *
 * @returns True if used, False if available
 */
bool CFE_TBL_CheckValidationResultSlotUsed(CFE_ResourceId_t CheckId);

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  DUMP CONTROL BLOCK ACCESSORS ~~~
 *
 * These operate on CFE_TBL_DumpControl_t* and CFE_TBL_DumpCtrlId_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a Dump Control Block ID
 *
 * Calculates the array position/index of the global array entry for
 * the given block ID.
 *
 * @param[in]  DumpCtrlId the ID/handle of the control block to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_DumpCtrlId_ToIndex(CFE_TBL_DumpCtrlId_t DumpCtrlId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the registry table entry correlating with a given registry ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for applications, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_TBL_DumpCtrlBlockIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_DumpCtrlBlockIsMatch()
 *
 * @param[in]   BlockId   the block ID to locate
 * @return pointer to dump control block for the given ID, or NULL if out of range
 */
CFE_TBL_DumpControl_t *CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DumpCtrlId_t BlockId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a dump control block is a match for the given ID
 *
 * This routine confirms that the previously-located block pointer is valid
 * and matches the expected block ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateDumpCtrlByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateDumpCtrlByID
 *
 * @param[in]   BlockPtr   pointer to validation result table entry, or NULL
 * @param[in]   BlockId     expected validation result ID
 * @returns true if the entry matches the given ID
 */
static inline bool CFE_TBL_DumpCtrlBlockIsMatch(const CFE_TBL_DumpControl_t *BlockPtr, CFE_TBL_DumpCtrlId_t BlockId)
{
    return (BlockPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(BlockPtr->BlockId, BlockId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a dump control block is in use or free/empty
 *
 * This routine checks if the block is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to dump control block
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_DumpCtrlBlockIsUsed(const CFE_TBL_DumpControl_t *BlockPtr)
{
    return (CFE_RESOURCEID_TEST_DEFINED(BlockPtr->BlockId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a dump control block as in use (not avaliable)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given block ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to dump control block
 * @param[in]   PendingId        the ID of this entry that will be set
 */
static inline void CFE_TBL_DumpCtrlBlockSetUsed(CFE_TBL_DumpControl_t *BlockPtr, CFE_ResourceId_t PendingId)
{
    BlockPtr->BlockId = CFE_TBL_DUMPCTRLID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a dump control block as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any block ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to dump control block
 */
static inline void CFE_TBL_DumpCtrlBlockSetFree(CFE_TBL_DumpControl_t *BlockPtr)
{
    BlockPtr->State   = CFE_TBL_DUMP_FREE;
    BlockPtr->BlockId = CFE_TBL_DUMPCTRLID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a dump control block
 *
 * This routine converts the block pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BlockPtr   pointer to block
 * @returns ID of entry
 */
static inline CFE_TBL_DumpCtrlId_t CFE_TBL_DumpCtrlBlockGetId(const CFE_TBL_DumpControl_t *BlockPtr)
{
    return BlockPtr->BlockId;
}

/*
 * Internal functions to perform name based resource lookups
 *
 * These functions do not lock, they must only be used internally by ES when
 * the lock is already held.
 */

/**
 * @brief Determine the next ID to use for a dump control block
 *
 * Obtains an ID value that is usable for a new dump control block.  If no blocks
 * are available, then UNDEFINED is returned.
 *
 * @returns ID to use for next control block, or UNDEFINED if no slots available
 */
CFE_ResourceId_t CFE_TBL_GetNextDumpCtrlBlock(void);

/**
 * Test if a slot corresponding to a pending ID is used
 *
 * This is an internal helper function for CFE_ResourceId_FindNext(), and not
 * typically called directly. It is prototyped here for unit testing.
 *
 * @returns True if used, False if available
 */
bool CFE_TBL_CheckDumpCtrlSlotUsed(CFE_ResourceId_t CheckId);

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  REGISTRY RECORD TABLE ACCESSORS ~~~
 *
 * These operate on CFE_TBL_RegistryRec_t* and CFE_TBL_RegId_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a Registry Record ID
 *
 * Calculates the array position/index of the global array entry for
 * the given registry ID.
 *
 * @param[in]  RegId the ID/handle of the registry record to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_RegId_ToIndex(CFE_TBL_RegId_t RegId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the registry table entry correlating with a given registry ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for applications, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_TBL_RegRecIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_RegRecIsMatch()
 *
 * @param[in]   RegId   the registry ID to locate
 * @return pointer to Registry Table entry for the given registry ID, or NULL if out of range
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByID(CFE_TBL_RegId_t RegId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate a Registry Record ID by name
 *
 * Returns a pointer to the registry table with a matching name
 *
 * @param[in] Name the name to search for (fully-qualified as AppName.TableNam)
 * @returns   Pointer to table entry with matching name
 * @retval    NULL if no matching entry was found
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByName(const char *Name);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a registry record is a match for the given RegId
 *
 * This routine confirms that the previously-located registry record is valid
 * and matches the expected registry ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateRegRecByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateRegRecByID
 *
 * @param[in]   RegRecPtr   pointer to registry table entry, or NULL
 * @param[in]   RegId       expected registry ID
 * @returns true if the entry matches the given registry ID
 */
static inline bool CFE_TBL_RegRecIsMatch(const CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_RegId_t RegId)
{
    /*
     * This technically should also check CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->OwnerAppId),
     * but that would currently break some registration actions (which should be fixed).
     */
    return (RegRecPtr != NULL);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a registry record
 *
 * This routine converts the registry record pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to table entry
 * @returns ID of entry
 */
CFE_TBL_RegId_t CFE_TBL_RegRecGetID(const CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a registry entry is in use or free/empty
 *
 * This routine checks if the entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to registry entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_RegRecIsUsed(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->OwnerAppId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a registry entry as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any registry ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to registry entry
 */
static inline void CFE_TBL_RegRecSetFree(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->OwnerAppId = CFE_TBL_NOT_OWNED;

    /* Also clear the table name, not strictly needed but good for consistency */
    RegRecPtr->Name[0] = '\0';
}

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  ACCESS DESCRIPTOR TABLE ACCESSORS ~~~
 *
 * These operate on CFE_TBL_AccessDescriptor_t* and CFE_TBL_Handle_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a table handle/access ID
 *
 * Calculates the array position/index of the global array entry for
 * the given handle.
 *
 * @param[in]  TblHandle the ID/handle of the access descriptor to retrieve
 * @param[out] Idx       Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_Handle_ToIndex(CFE_TBL_Handle_t TblHandle, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the access descriptor entry correlating with a given table handle.
 *
 * This only returns a pointer to the table entry where the descriptor
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for tasks, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned descriptor is a match to the expected ID before using
 * or modifying the data within the returned descriptor pointer.
 *
 * The CFE_TBL_AccDescIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_AccDescIsMatch()
 *
 * @param[in]   TblHandle   the table handle to locate
 * @return pointer to decriptor table entry for the given table handle, or NULL if out of range
 */
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateAccDescByHandle(CFE_TBL_Handle_t TblHandle);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the Handle ID from an an access descriptor pointer
 *
 * This routine converts a pointer to a handle table entry to an ID
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccDescPtr   pointer to access descriptor
 * @returns TblHandle of entry
 */
CFE_TBL_Handle_t CFE_TBL_AccDescGetHandle(const CFE_TBL_AccessDescriptor_t *AccDescPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an Access Descriptor is a match for the given TblHandle
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Task ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateAccDescByHandle()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateAccDescByHandle
 *
 * @param[in]   AccDescPtr   pointer to access descriptor table entry
 * @param[in]   TblHandle       The expected table handle to verify
 * @returns true if the descriptor entry matches the given table handle
 */
static inline bool CFE_TBL_AccDescIsMatch(const CFE_TBL_AccessDescriptor_t *AccDescPtr, CFE_TBL_Handle_t TblHandle)
{
    return (AccDescPtr != NULL && AccDescPtr->UsedFlag);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an access descriptor is in use or free/empty
 *
 * This routine checks if the entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccDescPtr   pointer to descriptor entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_AccDescIsUsed(const CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    return AccDescPtr->UsedFlag;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks an access descriptor entry as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any table handle ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccDescPtr   pointer to descriptor entry
 */
static inline void CFE_TBL_AccDescSetFree(CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    AccDescPtr->UsedFlag = false;
    AccDescPtr->AppId    = CFE_TBL_NOT_OWNED;
}

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  LOAD BUFFER ACCESSORS ~~~
 *
 * These operate on CFE_TBL_LoadBuff_t* and buffer ID
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a load buffer ID
 *
 * Calculates the array position/index of the global array entry for
 * the given load buffer ID.
 *
 * @param[in]  BuffId the ID/handle of the load buffer to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_LoadBuffId_ToIndex(CFE_TBL_LoadBuffId_t BuffId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the load buffer table entry correlating with a given load buffer ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for applications, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * @param[in]   ContextPtr The registry entry this buffer is associated with (NULL for temporary buffers)
 * @param[in]   BufferId   the buffer ID to locate
 * @return pointer to the buffer for the given ID, or NULL if out of range
 */
CFE_TBL_LoadBuff_t *CFE_TBL_LocateLoadBufferByID(CFE_TBL_RegistryRec_t *ContextPtr, CFE_TBL_LoadBuffId_t BufferId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a table buffer
 *
 * This routine converts the load buffer pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   LoadBuffPtr   pointer to buffer entry
 * @returns ID of entry
 */
CFE_TBL_LoadBuffId_t CFE_TBL_LoadBufferGetID(const CFE_TBL_LoadBuff_t *LoadBuffPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a load buffer is in use or free/empty
 *
 * This routine checks if the buffer is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to load buffer
 * @returns true if the buffer is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_LoadBuffIsUsed(const CFE_TBL_LoadBuff_t *BuffPtr)
{
    return BuffPtr->Taken;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a buffer as available (not in use)
 *
 * This clears the internal field(s) within this buffer, and marks
 * it as being available for future use.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   BuffPtr   pointer to buffer
 */
static inline void CFE_TBL_LoadBuffSetFree(CFE_TBL_LoadBuff_t *BuffPtr)
{
    BuffPtr->Taken = false;
}

#endif /* CFE_TBL_RESOURCE_H */
