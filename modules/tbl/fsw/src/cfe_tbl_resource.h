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
 */

#ifndef CFE_TBL_RESOURCE_H
#define CFE_TBL_RESOURCE_H

/*
** Include Files
*/
#include "cfe_resourceid.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_tbl_task.h"

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
 * The CFE_TBL_RegistryRecordIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_RegistryRecordIsMatch()
 *
 * @param[in]   RegId   the registry ID to locate
 * @return pointer to Registry Table entry for the given registry ID, or NULL if out of range
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegistryRecordByID(CFE_TBL_RegId_t RegId);

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
 * The CFE_TBL_AccessDescriptorIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_AccessDescriptorIsMatch()
 *
 * @param[in]   TblHandle   the table handle to locate
 * @return pointer to decriptor table entry for the given table handle, or NULL if out of range
 */
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateAccessDescriptorByHandle(CFE_TBL_Handle_t TblHandle);

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
 * This function may be used in conjunction with CFE_TBL_LocateRegistryRecordByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateRegistryRecordByID
 *
 * @param[in]   RegRecPtr   pointer to registry table entry, or NULL
 * @param[in]   RegId       expected registry ID
 * @returns true if the entry matches the given registry ID
 */
static inline bool CFE_TBL_RegistryRecordIsMatch(const CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_RegId_t RegId)
{
    /*
     * This technically should also check CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->OwnerAppId),
     * but that would currently break some registration actions (which should be fixed).
     */
    return (RegRecPtr != NULL);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the name associated with the Application record
 *
 * Returns the name field from within the Application record
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to App table entry
 * @returns Pointer to Application name
 */
static inline const char *CFE_TBL_RegistryRecordGetName(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->Name;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the Handle ID from an an access descriptor pointer
 *
 * This routine converts a pointer to a handle table entry to an ID
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccessDescPtr   pointer to access descriptor
 * @returns TblHandle of entry
 */
CFE_TBL_Handle_t CFE_TBL_AccessDescriptorGetHandle(const CFE_TBL_AccessDescriptor_t *AccessDescPtr);

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
 * This function may be used in conjunction with CFE_TBL_LocateAccessDescriptorByHandle()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateAccessDescriptorByHandle
 *
 * @param[in]   AccessDescPtr   pointer to access descriptor table entry
 * @param[in]   TblHandle       The expected table handle to verify
 * @returns true if the descriptor entry matches the given table handle
 */
static inline bool CFE_TBL_AccessDescriptorIsMatch(const CFE_TBL_AccessDescriptor_t *AccessDescPtr,
                                                   CFE_TBL_Handle_t                  TblHandle)
{
    return (AccessDescPtr != NULL && AccessDescPtr->UsedFlag);
}

#endif /* CFE_TBL_RESOURCE_H */
