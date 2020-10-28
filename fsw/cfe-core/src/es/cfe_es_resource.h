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
 * \file cfe_es_resource.h
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
#include "cfe_es_global.h"

/*
** Defines
*/

/*
 * Limits/definitions related to CFE_ES_ResourceID_t values.
 *
 * Defining based on OSAL ID values makes this object a superset of
 * the OSAL ID type, such that OSAL IDs can be represented as ES resource IDs
 * and not conflict with/alias each other.
 *
 * NOTE: This reflects a bit if "inside knowledge" about how OSAL IDs are
 * constructed.  The overlap between OSAL IDs and ES IDs may not always be
 * consistent, and they can diverge in a future version.
 */
#define CFE_ES_RESOURCEID_SHIFT OS_OBJECT_TYPE_SHIFT
#define CFE_ES_RESOURCEID_MAX   ((1 << CFE_ES_RESOURCEID_SHIFT)-1)
#define CFE_ES_RESOURCEID_MARK  (0x02000000)

#define CFE_ES_APPID_BASE       (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+1) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_LIBID_BASE       (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+2) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_COUNTID_BASE     (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+3) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_POOLID_BASE      (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+4) << CFE_ES_RESOURCEID_SHIFT))
#define CFE_ES_CDSBLOCKID_BASE  (CFE_ES_RESOURCEID_MARK | ((OS_OBJECT_TYPE_USER+5) << CFE_ES_RESOURCEID_SHIFT))


/**
 * @brief Locate the next resource ID which does not map to an in-use table entry
 *
 * This begins searching from StartId which should be the most recently issued ID
 * for the resource category.  This will then search for the next ID which does
 * _not_ map to a table entry that is in use.  That is, it does not alias any
 * valid ID when converted to an array index.
 *
 * returns an undefined ID value if no open slots are available
 *
 * @param[in]   StartId   the last issued ID for the resource category (app, lib, etc).
 * @returns     Next ID value which does not map to a valid entry
 * @retval      #CFE_ES_RESOURCEID_UNDEFINED if no open slots.
 *
 */
CFE_ES_ResourceID_t CFE_ES_FindNextAvailableId(CFE_ES_ResourceID_t StartId, uint32 TableSize);


/**
 * @brief Locate the app table entry correlating with a given app ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   AppID   the app ID to locate
 * @return pointer to App Table entry for the given app ID
 */
extern CFE_ES_AppRecord_t* CFE_ES_LocateAppRecordByID(CFE_ES_ResourceID_t AppID);

/**
 * @brief Locate the Library table entry correlating with a given Lib ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   LibID   the Lib ID to locate
 * @return pointer to Library Table entry for the given Lib ID
 */
extern CFE_ES_LibRecord_t* CFE_ES_LocateLibRecordByID(CFE_ES_ResourceID_t LibID);

/**
 * @brief Locate the task table entry correlating with a given task ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   TaskID   the task ID to locate
 * @return pointer to Task Table entry for the given task ID
 */
extern CFE_ES_TaskRecord_t* CFE_ES_LocateTaskRecordByID(CFE_ES_ResourceID_t TaskID);

/**
 * @brief Locate the Counter table entry correlating with a given Counter ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   CounterID   the Counter ID to locate
 * @return pointer to Counter Table entry for the given Counter ID
 */
extern CFE_ES_GenCounterRecord_t* CFE_ES_LocateCounterRecordByID(CFE_ES_ResourceID_t CounterID);

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
    return CFE_ES_ResourceID_IsDefined(AppRecPtr->AppId);
}

/**
 * @brief Get the ID value from an app table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   AppRecPtr   pointer to app table entry
 * @returns AppID of entry
 */
static inline CFE_ES_ResourceID_t CFE_ES_AppRecordGetID(const CFE_ES_AppRecord_t *AppRecPtr)
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
 * @param[in]   AppID       the app ID of this entry
 */
static inline void CFE_ES_AppRecordSetUsed(CFE_ES_AppRecord_t *AppRecPtr, CFE_ES_ResourceID_t AppID)
{
    AppRecPtr->AppId = AppID;
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
    AppRecPtr->AppId = CFE_ES_RESOURCEID_UNDEFINED;
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
static inline bool CFE_ES_AppRecordIsMatch(const CFE_ES_AppRecord_t *AppRecPtr, CFE_ES_ResourceID_t AppID)
{
    return (AppRecPtr != NULL && CFE_ES_ResourceID_Equal(AppRecPtr->AppId, AppID));
}

/**
 * @brief Obtain the name associated with the Application record
 *
 * Returns the name field from within the Application record
 *
 * @param[in]   AppRecPtr   pointer to App table entry
 * @returns Pointer to Application name
 */
static inline const char* CFE_ES_AppRecordGetName(const CFE_ES_AppRecord_t *AppRecPtr)
{
    return AppRecPtr->StartParams.BasicInfo.Name;
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
    return CFE_ES_ResourceID_IsDefined(LibRecPtr->LibId);
}

/**
 * @brief Get the ID value from a Library table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 * @returns LibID of entry
 */
static inline CFE_ES_ResourceID_t CFE_ES_LibRecordGetID(const CFE_ES_LibRecord_t *LibRecPtr)
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
 * @param[in]   LibID       the Lib ID of this entry
 */
static inline void CFE_ES_LibRecordSetUsed(CFE_ES_LibRecord_t *LibRecPtr, CFE_ES_ResourceID_t LibID)
{
    LibRecPtr->LibId = LibID;
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
    LibRecPtr->LibId = CFE_ES_RESOURCEID_UNDEFINED;
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
static inline bool CFE_ES_LibRecordIsMatch(const CFE_ES_LibRecord_t *LibRecPtr, CFE_ES_ResourceID_t LibID)
{
    return (LibRecPtr != NULL && CFE_ES_ResourceID_Equal(LibRecPtr->LibId, LibID));
}

/**
 * @brief Obtain the name associated with the Library record
 *
 * Returns the name field from within the Library record
 *
 * @param[in]   LibRecPtr   pointer to Lib table entry
 * @returns Pointer to Library name
 */
static inline const char* CFE_ES_LibRecordGetName(const CFE_ES_LibRecord_t *LibRecPtr)
{
    return LibRecPtr->BasicInfo.Name;
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
static inline CFE_ES_ResourceID_t CFE_ES_TaskRecordGetID(const CFE_ES_TaskRecord_t *TaskRecPtr)
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
    return CFE_ES_ResourceID_IsDefined(TaskRecPtr->TaskId);
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
 * @param[in]   TaskID       the Task ID of this entry
 */
static inline void CFE_ES_TaskRecordSetUsed(CFE_ES_TaskRecord_t *TaskRecPtr, CFE_ES_ResourceID_t TaskID)
{
    TaskRecPtr->TaskId = TaskID;
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
    TaskRecPtr->TaskId = CFE_ES_RESOURCEID_UNDEFINED;
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
static inline bool CFE_ES_TaskRecordIsMatch(const CFE_ES_TaskRecord_t *TaskRecPtr, CFE_ES_ResourceID_t TaskID)
{
    return (TaskRecPtr != NULL && CFE_ES_ResourceID_Equal(TaskRecPtr->TaskId, TaskID));
}

/**
 * @brief Obtain the name associated with the Task record
 *
 * Returns the name field from within the Task record
 *
 * @param[in]   TaskRecPtr   pointer to Task table entry
 * @returns Pointer to Task name
 */
static inline const char* CFE_ES_TaskRecordGetName(const CFE_ES_TaskRecord_t *TaskRecPtr)
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
    return CFE_ES_ResourceID_IsDefined(CounterRecPtr->CounterId);
}

/**
 * @brief Get the ID value from an Counter table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns CounterID of entry
 */
static inline CFE_ES_ResourceID_t CFE_ES_CounterRecordGetID(const CFE_ES_GenCounterRecord_t *CounterRecPtr)
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
 * @param[in]   CounterID       the Counter ID of this entry
 */
static inline void CFE_ES_CounterRecordSetUsed(CFE_ES_GenCounterRecord_t *CounterRecPtr, CFE_ES_ResourceID_t CounterID)
{
    CounterRecPtr->CounterId = CounterID;
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
    CounterRecPtr->CounterId = CFE_ES_RESOURCEID_UNDEFINED;
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
static inline bool CFE_ES_CounterRecordIsMatch(const CFE_ES_GenCounterRecord_t *CounterRecPtr, CFE_ES_ResourceID_t CounterID)
{
    return (CounterRecPtr != NULL && CFE_ES_ResourceID_Equal(CounterRecPtr->CounterId, CounterID));
}

/**
 * @brief Obtain the name associated with the counter record
 *
 * Returns the name field from within the counter record
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns Pointer to counter name
 */
static inline const char* CFE_ES_CounterRecordGetName(const CFE_ES_GenCounterRecord_t *CounterRecPtr)
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
extern CFE_ES_AppRecord_t* CFE_ES_GetAppRecordByContext(void);

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
extern CFE_ES_TaskRecord_t* CFE_ES_GetTaskRecordByContext(void);

/**
 * @brief Convert an ES Task ID to an OSAL task ID
 *
 * Task IDs created via CFE ES are also OSAL task IDs, but technically
 * do refer to a different scope and therefore have a different type
 * to represent them.
 *
 * This function facilitates converting between the types.
 *
 * @note Currently the numeric values are the same and can be interchanged
 * for backward compatibility, however they may diverge in a future version.
 * New code should not assume equivalence between OSAL and ES task IDs.
 *
 * @sa CFE_ES_ResourceID_FromOSAL
 *
 * @param[in] id    The ES task ID
 * @returns         The OSAL task ID
 */
osal_id_t CFE_ES_ResourceID_ToOSAL(CFE_ES_ResourceID_t id);

/**
 * @brief Convert an ES Task ID to an OSAL task ID
 *
 * Task IDs created via CFE ES are also OSAL task IDs, but technically
 * do refer to a different scope and therefore have a different type
 * to represent them.
 *
 * This function facilitates converting between the types.
 *
 * @note Currently the numeric values are the same and can be interchanged
 * for backward compatibility, however they may diverge in a future version.
 * New code should not assume equivalence between OSAL and ES task IDs.
 *
 * @sa CFE_ES_ResourceID_ToOSAL
 *
 * @param[in] id    The OSAL task ID
 * @returns         The ES task ID
 */
CFE_ES_ResourceID_t CFE_ES_ResourceID_FromOSAL(osal_id_t id);

/**
 * @brief Internal routine to aid in converting an ES resource ID to an array index

 * @param[in]  Serial    The resource serial number (type info masked out)
 * @param[in]  TableSize The size of the internal table (MAX value)
 * @param[out] Idx       The output index
 * @returns Status code, CFE_SUCCESS if successful.
 */
int32 CFE_ES_ResourceID_ToIndex_Internal(uint32 Serial, uint32 TableSize, uint32 *Idx);

/*
 * Internal functions to perform name based resource lookups
 *
 * These functions do not lock, they must only be used internally by ES when
 * the lock is already held.
 */
CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByName(const char *Name);
CFE_ES_LibRecord_t *CFE_ES_LocateLibRecordByName(const char *Name);
CFE_ES_TaskRecord_t *CFE_ES_LocateTaskRecordByName(const char *Name);
CFE_ES_GenCounterRecord_t *CFE_ES_LocateCounterRecordByName(const char *Name);


#endif  /* CFE_ES_RESOURCE_H */
