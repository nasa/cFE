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
 * @file
 *
 * Contains basic prototypes and definitions related to CFE ES resource
 * management and related resource IDs.
 *
 * A CFE ES Resource ID is a common way to identify CFE-managed resources such
 * as apps, tasks, counters, memory pools, CDS blocks, and other entities.
 */

#ifndef CFE_ES_RESOURCE_H
#define CFE_ES_RESOURCE_H

/*
** Include Files
*/
#include "cfe_resourceid.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_es_global.h"

/**
 * @brief Locate the app table entry correlating with a given app ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   AppID   the app ID to locate
 * @return pointer to App Table entry for the given app ID
 */
extern CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByID(CFE_ES_AppId_t AppID);

/**
 * @brief Locate the Library table entry correlating with a given Lib ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   LibID   the Lib ID to locate
 * @return pointer to Library Table entry for the given Lib ID
 */
extern CFE_ES_LibRecord_t *CFE_ES_LocateLibRecordByID(CFE_ES_LibId_t LibID);

/**
 * @brief Locate the task table entry correlating with a given task ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   TaskID   the task ID to locate
 * @return pointer to Task Table entry for the given task ID
 */
extern CFE_ES_TaskRecord_t *CFE_ES_LocateTaskRecordByID(CFE_ES_TaskId_t TaskID);

/**
 * @brief Locate the Counter table entry correlating with a given Counter ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   CounterID   the Counter ID to locate
 * @return pointer to Counter Table entry for the given Counter ID
 */
extern CFE_ES_GenCounterRecord_t *CFE_ES_LocateCounterRecordByID(CFE_ES_CounterId_t CounterID);

/**
 * @brief Check if an app record is in use or free/empty
 *
 * This routine checks if the App table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   AppRecPtr   pointer to app table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_ES_AppRecordIsUsed(const CFE_ES_AppRecord_t *AppRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(AppRecPtr->AppId);
}

/**
 * @brief Get the ID value from an app table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   AppRecPtr   pointer to app table entry
 * @returns AppID of entry
 */
static inline CFE_ES_AppId_t CFE_ES_AppRecordGetID(const CFE_ES_AppRecord_t *AppRecPtr)
{
    return AppRecPtr->AppId;
}

/**
 * @brief Marks an app table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given app ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   AppRecPtr   pointer to app table entry
 * @param[in]   PendingId   the app ID of this entry
 */
static inline void CFE_ES_AppRecordSetUsed(CFE_ES_AppRecord_t *AppRecPtr, CFE_ResourceId_t PendingId)
{
    AppRecPtr->AppId = CFE_ES_APPID_C(PendingId);
}

/**
 * @brief Set an app record table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   AppRecPtr   pointer to app table entry
 */
static inline void CFE_ES_AppRecordSetFree(CFE_ES_AppRecord_t *AppRecPtr)
{
    AppRecPtr->AppId = CFE_ES_APPID_UNDEFINED;
}

/**
 * @brief Check if an app record is a match for the given AppID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected app ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   AppRecPtr   pointer to app table entry
 * @param[in]   AppID       expected app ID
 * @returns true if the entry matches the given app ID
 */
static inline bool CFE_ES_AppRecordIsMatch(const CFE_ES_AppRecord_t *AppRecPtr, CFE_ES_AppId_t AppID)
{
    return (AppRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(AppRecPtr->AppId, AppID));
}

/**
 * @brief Obtain the name associated with the Application record
 *
 * Returns the name field from within the Application record
 *
 * @param[in]   AppRecPtr   pointer to App table entry
 * @returns Pointer to Application name
 */
static inline const char *CFE_ES_AppRecordGetName(const CFE_ES_AppRecord_t *AppRecPtr)
{
    return AppRecPtr->AppName;
}

/**
 * @brief Check if a Library record is in use or free/empty
 *
 * This routine checks if the Lib table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_ES_LibRecordIsUsed(const CFE_ES_LibRecord_t *LibRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(LibRecPtr->LibId);
}

/**
 * @brief Get the ID value from a Library table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 * @returns LibID of entry
 */
static inline CFE_ES_LibId_t CFE_ES_LibRecordGetID(const CFE_ES_LibRecord_t *LibRecPtr)
{
    /*
     * The initial implementation does not store the ID in the entry;
     * the ID is simply the zero-based index into the table.
     */
    return (LibRecPtr->LibId);
}

/**
 * @brief Marks a Library table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Lib ID.
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 * @param[in]   PendingId   the Lib ID of this entry
 */
static inline void CFE_ES_LibRecordSetUsed(CFE_ES_LibRecord_t *LibRecPtr, CFE_ResourceId_t PendingId)
{
    LibRecPtr->LibId = CFE_ES_LIBID_C(PendingId);
}

/**
 * @brief Set a Library record table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 */
static inline void CFE_ES_LibRecordSetFree(CFE_ES_LibRecord_t *LibRecPtr)
{
    LibRecPtr->LibId = CFE_ES_LIBID_UNDEFINED;
}

/**
 * @brief Check if a Library record is a match for the given LibID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Lib ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 * @param[in]   LibID       expected Lib ID
 * @returns true if the entry matches the given Lib ID
 */
static inline bool CFE_ES_LibRecordIsMatch(const CFE_ES_LibRecord_t *LibRecPtr, CFE_ES_LibId_t LibID)
{
    return (LibRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(LibRecPtr->LibId, LibID));
}

/**
 * @brief Obtain the name associated with the Library record
 *
 * Returns the name field from within the Library record
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 * @returns Pointer to Library name
 */
static inline const char *CFE_ES_LibRecordGetName(const CFE_ES_LibRecord_t *LibRecPtr)
{
    return LibRecPtr->LibName;
}

/**
 * @brief Get the ID value from an Task table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   TaskRecPtr   pointer to Task table entry
 * @returns TaskID of entry
 */
static inline CFE_ES_TaskId_t CFE_ES_TaskRecordGetID(const CFE_ES_TaskRecord_t *TaskRecPtr)
{
    return (TaskRecPtr->TaskId);
}

/**
 * @brief Check if a Task record is in use or free/empty
 *
 * This routine checks if the Task table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   TaskRecPtr   pointer to task table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_ES_TaskRecordIsUsed(const CFE_ES_TaskRecord_t *TaskRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(TaskRecPtr->TaskId);
}

/**
 * @brief Marks an Task table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Task ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   TaskRecPtr   pointer to Task table entry
 * @param[in]   PendingId    the Task ID of this entry
 */
static inline void CFE_ES_TaskRecordSetUsed(CFE_ES_TaskRecord_t *TaskRecPtr, CFE_ResourceId_t PendingId)
{
    TaskRecPtr->TaskId = CFE_ES_TASKID_C(PendingId);
}

/**
 * @brief Set a Task record table entry free
 *
 * This allows the table entry to be re-used by another Task.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   TaskRecPtr   pointer to task table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline void CFE_ES_TaskRecordSetFree(CFE_ES_TaskRecord_t *TaskRecPtr)
{
    TaskRecPtr->TaskId = CFE_ES_TASKID_UNDEFINED;
}

/**
 * @brief Check if a Task record is a match for the given TaskID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Task ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   TaskRecPtr   pointer to task table entry
 * @param[in]   TaskID       The expected task ID to verify
 * @returns true if the entry matches the given task ID
 */
static inline bool CFE_ES_TaskRecordIsMatch(const CFE_ES_TaskRecord_t *TaskRecPtr, CFE_ES_TaskId_t TaskID)
{
    return (TaskRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(TaskRecPtr->TaskId, TaskID));
}

/**
 * @brief Obtain the name associated with the Task record
 *
 * Returns the name field from within the Task record
 *
 * @param[in]   TaskRecPtr   pointer to Task table entry
 * @returns Pointer to Task name
 */
static inline const char *CFE_ES_TaskRecordGetName(const CFE_ES_TaskRecord_t *TaskRecPtr)
{
    return TaskRecPtr->TaskName;
}

/**
 * @brief Check if an Counter record is in use or free/empty
 *
 * This routine checks if the Counter table entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_ES_CounterRecordIsUsed(const CFE_ES_GenCounterRecord_t *CounterRecPtr)
{
    return CFE_RESOURCEID_TEST_DEFINED(CounterRecPtr->CounterId);
}

/**
 * @brief Get the ID value from an Counter table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns CounterID of entry
 */
static inline CFE_ES_CounterId_t CFE_ES_CounterRecordGetID(const CFE_ES_GenCounterRecord_t *CounterRecPtr)
{
    return CounterRecPtr->CounterId;
}

/**
 * @brief Marks an Counter table entry as used (not free)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given Counter ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @param[in]   PendingId       the Counter ID of this entry
 */
static inline void CFE_ES_CounterRecordSetUsed(CFE_ES_GenCounterRecord_t *CounterRecPtr, CFE_ResourceId_t PendingId)
{
    CounterRecPtr->CounterId = CFE_ES_COUNTERID_C(PendingId);
}

/**
 * @brief Set an Counter record table entry free (not used)
 *
 * This clears the internal field(s) within this entry, and allows the
 * memory to be re-used in the future.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 */
static inline void CFE_ES_CounterRecordSetFree(CFE_ES_GenCounterRecord_t *CounterRecPtr)
{
    CounterRecPtr->CounterId = CFE_ES_COUNTERID_UNDEFINED;
}

/**
 * @brief Check if an Counter record is a match for the given CounterID
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Counter ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @param[in]   CounterID       expected Counter ID
 * @returns true if the entry matches the given Counter ID
 */
static inline bool CFE_ES_CounterRecordIsMatch(const CFE_ES_GenCounterRecord_t *CounterRecPtr,
                                               CFE_ES_CounterId_t               CounterID)
{
    return (CounterRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(CounterRecPtr->CounterId, CounterID));
}

/**
 * @brief Obtain the name associated with the counter record
 *
 * Returns the name field from within the counter record
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns Pointer to counter name
 */
static inline const char *CFE_ES_CounterRecordGetName(const CFE_ES_GenCounterRecord_t *CounterRecPtr)
{
    return CounterRecPtr->CounterName;
}

/**
 * Locate and validate the app record for the calling context.
 *
 * Finds and validates the ES AppTable entry corresponding to the
 * caller. This confirms that the fields within the table entry match the
 * expected value(s), otherwise NULL is returned if no matching entry
 * is found.
 *
 * The global data lock should be obtained prior to invoking this function.
 */
extern CFE_ES_AppRecord_t *CFE_ES_GetAppRecordByContext(void);

/**
 * Locate and validate the task record for the calling context.
 *
 * Finds and validates the ES TaskTable entry corresponding to the
 * caller. This confirms that the fields within the table entry match the
 * expected value(s), otherwise NULL is returned if no matching entry
 * is found.
 *
 * The global data lock should be obtained prior to invoking this function.
 */
extern CFE_ES_TaskRecord_t *CFE_ES_GetTaskRecordByContext(void);

/*
 * OSAL <-> CFE task ID conversion
 *
 * CFE ES does not currently allocate its own task IDs; instead it piggybacks on top
 * of the allocation that is already done by OSAL.  This is partly for backward
 * compatibility - historically the OSAL task IDs were used directly by CFE task APIs.
 *
 * This is _only_ used for tasks - for all other resource types ES should allocate
 * its own identifiers independently of any other subsystem.  This conversion may also
 * be removed in a future version of CFE, if ES starts allocating task IDs independently
 * of OSAL task IDs.
 */

/**
 * @brief Convert an ES Task ID to an OSAL task ID
 *
 * Task IDs created via CFE ES are also OSAL task IDs, but technically
 * do refer to a different scope and therefore have a different type
 * to represent them.
 *
 * This function facilitates converting between the types.
 *
 * @note With "simple" resource IDs, numeric values are the same and can be interchanged
 * for backward compatibility, however they will be different when using "strict" IDs.
 * New code should not assume equivalence between OSAL and ES task IDs.
 *
 * @sa CFE_ES_TaskId_FromOSAL
 *
 * @param[in] id    The CFE task ID
 * @returns         The OSAL task ID
 */
extern osal_id_t CFE_ES_TaskId_ToOSAL(CFE_ES_TaskId_t id);

/**
 * @brief Convert an ES Task ID to an OSAL task ID
 *
 * Task IDs created via CFE ES are also OSAL task IDs, but technically
 * do refer to a different scope and therefore have a different type
 * to represent them.
 *
 * This function facilitates converting between the types.
 *
 * @note With "simple" resource IDs, numeric values are the same and can be interchanged
 * for backward compatibility, however they will be different when using "strict" IDs.
 * New code should not assume equivalence between OSAL and ES task IDs.
 *
 * @sa CFE_ES_TaskId_ToOSAL
 *
 * @param[in] id    The OSAL task ID
 * @returns         The CFE task ID
 */
extern CFE_ES_TaskId_t CFE_ES_TaskId_FromOSAL(osal_id_t id);

/*
 * Internal functions to perform name based resource lookups
 *
 * These functions do not lock, they must only be used internally by ES when
 * the lock is already held.
 */
CFE_ES_AppRecord_t *       CFE_ES_LocateAppRecordByName(const char *Name);
CFE_ES_LibRecord_t *       CFE_ES_LocateLibRecordByName(const char *Name);
CFE_ES_TaskRecord_t *      CFE_ES_LocateTaskRecordByName(const char *Name);
CFE_ES_GenCounterRecord_t *CFE_ES_LocateCounterRecordByName(const char *Name);

/* Availability check functions used in conjunction with CFE_ResourceId_FindNext() */
bool CFE_ES_CheckAppIdSlotUsed(CFE_ResourceId_t CheckId);
bool CFE_ES_CheckLibIdSlotUsed(CFE_ResourceId_t CheckId);
bool CFE_ES_CheckCounterIdSlotUsed(CFE_ResourceId_t CheckId);

#endif /* CFE_ES_RESOURCE_H */
