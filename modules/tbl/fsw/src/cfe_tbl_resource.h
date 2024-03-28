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
 * @brief Locate the app table entry correlating with a given app ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for applications, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID _should_ reside, if it is currently in use.
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
 * @param[in]   RegId   the app ID to locate
 * @return pointer to App Table entry for the given app ID, or NULL if out of range
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegistryRecordByID(CFE_TBL_RegId_t RegId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the task table entry correlating with a given task ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for tasks, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID _should_ reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_TBL_AccessDescriptorIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_AccessDescriptorIsMatch()
 *
 * @param[in]   TblHandle   the task ID to locate
 * @return pointer to Task Table entry for the given task ID, or NULL if out of range
 */
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateAccessDescriptorByHandle(CFE_TBL_Handle_t TblHandle);

#ifdef jphfix
/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an app record is in use or free/empty
 *
 * This routine checks if the App table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to app table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_RegistryRecordIsUsed(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->Taken;
}
#endif

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an app record is a match for the given RegId
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected app ID.
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
 * @param[in]   RegRecPtr   pointer to app table entry, or NULL
 * @param[in]   RegId       expected app ID
 * @returns true if the entry matches the given app ID
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

#ifdef jphfix
/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a Task table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccessDescPtr   pointer to Task table entry
 * @returns TblHandle of entry
 */
static inline CFE_TBL_Handle_t CFE_TBL_AccessDescriptorGetID(const CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    return AccessDescPtr->TblHandle;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Task record is in use or free/empty
 *
 * This routine checks if the Task table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccessDescPtr   pointer to task table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_AccessDescriptorIsUsed(const CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(AccessDescPtr->TblHandle);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a Task table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Task ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccessDescPtr   pointer to Task table entry
 * @param[in]   PendingId    the Task ID of this entry
 */
static inline void CFE_TBL_AccessDescriptorSetUsed(CFE_TBL_AccessDescriptor_t *AccessDescPtr,
                                                   CFE_ResourceId_t            PendingId)
{
    AccessDescPtr->TblHandle = CFE_TBL_ACCESSID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set a Task record table entry free
 *
 * This allows the table entry to be re-used by another Task.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccessDescPtr   pointer to task table entry
 */
static inline void CFE_TBL_AccessDescriptorSetFree(CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    AccessDescPtr->TblHandle = CFE_TBL_ACCESSID_UNDEFINED;
}
#endif

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Task record is a match for the given TblHandle
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Task ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateTaskRecordByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateTaskRecordByID
 *
 * @param[in]   AccessDescPtr   pointer to task table entry
 * @param[in]   TblHandle       The expected task ID to verify
 * @returns true if the entry matches the given task ID
 */
static inline bool CFE_TBL_AccessDescriptorIsMatch(const CFE_TBL_AccessDescriptor_t *AccessDescPtr,
                                                   CFE_TBL_Handle_t                  TblHandle)
{
    return (AccessDescPtr != NULL && AccessDescPtr->UsedFlag);
}

#ifdef jphfix
/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the name associated with the Task record
 *
 * Returns the name field from within the Task record
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccessDescPtr   pointer to Task table entry
 * @returns Pointer to Task name
 */
static inline const char *CFE_TBL_AccessDescriptorGetName(const CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    return AccessDescPtr->TaskName;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Counter record is in use or free/empty
 *
 * This routine checks if the Counter table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_CounterRecordIsUsed(const CFE_TBL_GenCounterRecord_t *CounterRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(CounterRecPtr->CounterId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a Counter table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns CounterID of entry
 */
static inline CFE_TBL_CounterId_t CFE_TBL_CounterRecordGetID(const CFE_TBL_GenCounterRecord_t *CounterRecPtr)
{
    return CounterRecPtr->CounterId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a Counter table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Counter ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @param[in]   PendingId       the Counter ID of this entry
 */
static inline void CFE_TBL_CounterRecordSetUsed(CFE_TBL_GenCounterRecord_t *CounterRecPtr, CFE_ResourceId_t PendingId)
{
    CounterRecPtr->CounterId = CFE_TBL_COUNTERID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set a Counter record table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 */
static inline void CFE_TBL_CounterRecordSetFree(CFE_TBL_GenCounterRecord_t *CounterRecPtr)
{
    CounterRecPtr->CounterId = CFE_TBL_COUNTERID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a Counter record is a match for the given CounterID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Counter ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateCounterRecordByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateCounterRecordByID
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @param[in]   CounterID       expected Counter ID
 * @returns true if the entry matches the given Counter ID
 */
static inline bool CFE_TBL_CounterRecordIsMatch(const CFE_TBL_GenCounterRecord_t *CounterRecPtr,
                                                CFE_TBL_CounterId_t               CounterID)
{
    return (CounterRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(CounterRecPtr->CounterId, CounterID));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the name associated with the counter record
 *
 * Returns the name field from within the counter record
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns Pointer to counter name
 */
static inline const char *CFE_TBL_CounterRecordGetName(const CFE_TBL_GenCounterRecord_t *CounterRecPtr)
{
    return CounterRecPtr->CounterName;
}
#endif

/*
 * Internal functions to perform name based resource lookups
 *
 * These functions do not lock, they must only be used internally by ES when
 * the lock is already held.
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Finds an application table record matching the given name
 *
 * Helper function, aids in finding an application record from a name string.
 * Must be called while locked.
 *
 * @returns pointer to table entry matching name, or NULL if not found
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegistryRecordByName(const char *Name);

#ifdef jphfix
/*---------------------------------------------------------------------------------------*/
/**
 * @brief Finds a library table record matching the given name
 *
 * Helper function, aids in finding a library record from a name string.
 * Must be called while locked.
 *
 * @returns pointer to table entry matching name, or NULL if not found
 */
CFE_TBL_LoadBuff_t *CFE_TBL_LocateLibRecordByName(const char *Name);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Finds a task table record matching the given name
 *
 * Helper function, aids in finding a task record from a name string.
 * Must be called while locked.
 *
 * @returns pointer to table entry matching name, or NULL if not found
 */
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateTaskRecordByName(const char *Name);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Finds a counter table record matching the given name
 *
 * Helper function, aids in finding a counter record from a name string.
 * Must be called while locked.
 *
 * @returns pointer to table entry matching name, or NULL if not found
 */
CFE_TBL_GenCounterRecord_t *CFE_TBL_LocateCounterRecordByName(const char *Name);
#endif

/*
 * Availability check functions used in conjunction with CFE_ResourceId_FindNext()
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if Application slot is currently used
 *
 * Helper function, Aids in allocating a new ID by checking if
 * a given ID is available.  Must be called while locked.
 *
 * @returns false if slot is unused/available, true if used/unavailable
 */
bool CFE_TBL_CheckRegIdSlotUsed(CFE_ResourceId_t CheckId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if Library slot is currently used
 *
 * Helper function, Aids in allocating a new ID by checking if
 * a given ID is available.  Must be called while locked.
 *
 * @returns false if slot is unused/available, true if used/unavailable
 */
bool CFE_TBL_CheckSharedBufferSlotUsed(CFE_ResourceId_t CheckId);

#ifdef jphfix
/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if Counter slot is currently used
 *
 * Helper function, Aids in allocating a new ID by checking if
 * a given ID is available.  Must be called while locked.
 *
 * @returns false if slot is unused/available, true if used/unavailable
 */
bool CFE_TBL_CheckCounterIdSlotUsed(CFE_ResourceId_t CheckId);
#endif

#endif /* CFE_TBL_RESOURCE_H */
