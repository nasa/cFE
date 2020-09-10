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

/*
**  File:
**  cfe_es_global.h
**
**  Purpose:
**  This file contains the ES global data definitions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
*/

#ifndef _cfe_es_global_
#define _cfe_es_global_

/*
** Includes:
*/
#include "osapi.h"
#include "private/cfe_private.h"
#include "private/cfe_es_resetdata_typedef.h"
#include "cfe_es.h"
#include "cfe_es_apps.h"
#include "cfe_es_cds.h"
#include "cfe_es_perf.h"
#include "cfe_time.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs.h"
#include "cfe_psp.h"


/*
** Defines
*/

/*
** Typedefs
*/

/*
** CFE_ES_GenCounterRecord_t is an internal structure used to keep track of
** Generic Counters that are active in the system.
*/
typedef struct
{
   bool           RecordUsed;                     /* Is the record used(1) or available(0) ? */
   uint32         Counter;
   char           CounterName[OS_MAX_API_NAME];   /* Counter Name */
} CFE_ES_GenCounterRecord_t;

/*
 * Encapsulates the state of the ES background task
 */
typedef struct
{
    uint32 TaskID;          /**< OSAL ID of the background task */
    uint32 WorkSem;         /**< Semaphore that is given whenever background work is pending */
    uint32 NumJobsRunning;  /**< Current Number of active jobs (updated by background task) */
} CFE_ES_BackgroundTaskState_t;


/*
** Executive Services Global Memory Data
** This is the regular global data that is not preserved on a
**  processor reset.
*/
typedef struct
{
   /*
   ** Debug Variables
   */
   CFE_ES_DebugVariables_t DebugVars;

   /*
   ** Shared Data Semaphore
   */
   uint32 SharedDataMutex;

   /*
   ** Performance Data Mutex
   */
   uint32 PerfDataMutex;

   /*
   ** Startup Sync
   */
   uint32  SystemState;

   /*
   ** ES Task Table
   */
   uint32              RegisteredTasks;
   CFE_ES_TaskRecord_t TaskTable[OS_MAX_TASKS];

   /*
   ** ES App Table
   */
   uint32             RegisteredCoreApps;
   uint32             RegisteredExternalApps;
   CFE_ES_AppRecord_t AppTable[CFE_PLATFORM_ES_MAX_APPLICATIONS];

   /*
   ** ES Shared Library Table
   */
   uint32             RegisteredLibs;
   CFE_ES_LibRecord_t LibTable[CFE_PLATFORM_ES_MAX_LIBRARIES];

   /*
   ** ES Generic Counters Table
   */
   CFE_ES_GenCounterRecord_t CounterTable[CFE_PLATFORM_ES_MAX_GEN_COUNTERS];

   /*
   ** Critical Data Store Management Variables
   */
   CFE_ES_CDSVariables_t CDSVars;

   /*
    * Background task for handling long-running, non real time tasks
    * such as maintenance, file writes, and other items.
    */
   CFE_ES_BackgroundTaskState_t BackgroundTask;


} CFE_ES_Global_t;

/*
** The Executive Services Global Data declaration
*/
extern CFE_ES_Global_t CFE_ES_Global;

/*
** The Executive Services Nonvolatile Data declaration
*/
extern CFE_ES_ResetData_t *CFE_ES_ResetDataPtr;

/**
 * @brief Locate the app table entry correlating with a given app ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   AppID   the app ID to locate
 * @return pointer to App Table entry for the given app ID
 */
extern CFE_ES_AppRecord_t* CFE_ES_LocateAppRecordByID(uint32 AppID);

/**
 * @brief Locate the task table entry correlating with a given task ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   TaskID   the task ID to locate
 * @return pointer to Task Table entry for the given task ID
 */
extern CFE_ES_TaskRecord_t* CFE_ES_LocateTaskRecordByID(uint32 TaskID);

/**
 * @brief Locate the Counter table entry correlating with a given Counter ID.
 *
 * This only returns a pointer to the table entry and does _not_
 * otherwise check/validate the entry.
 *
 * @param[in]   CounterID   the Counter ID to locate
 * @return pointer to Counter Table entry for the given Counter ID
 */
extern CFE_ES_GenCounterRecord_t* CFE_ES_LocateCounterRecordByID(uint32 CounterID);

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
    return (AppRecPtr->AppState != CFE_ES_AppState_UNDEFINED);
}

/**
 * @brief Get the ID value from an app table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   AppRecPtr   pointer to app table entry
 * @returns AppID of entry
 */
static inline uint32 CFE_ES_AppRecordGetID(const CFE_ES_AppRecord_t *AppRecPtr)
{
    /*
     * The initial implementation does not store the ID in the entry;
     * the ID is simply the zero-based index into the table.
     */
    return (AppRecPtr - CFE_ES_Global.AppTable);
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
static inline void CFE_ES_AppRecordSetUsed(CFE_ES_AppRecord_t *AppRecPtr, uint32 AppID)
{
    AppRecPtr->AppState = CFE_ES_AppState_EARLY_INIT;
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
    AppRecPtr->AppState = CFE_ES_AppState_UNDEFINED;
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
static inline bool CFE_ES_AppRecordIsMatch(const CFE_ES_AppRecord_t *AppRecPtr, uint32 AppID)
{
    return (AppRecPtr != NULL && CFE_ES_AppRecordIsUsed(AppRecPtr) &&
            CFE_ES_AppRecordGetID(AppRecPtr) == AppID);
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
static inline uint32 CFE_ES_TaskRecordGetID(const CFE_ES_TaskRecord_t *TaskRecPtr)
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
    return (TaskRecPtr->RecordUsed);
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
static inline void CFE_ES_TaskRecordSetUsed(CFE_ES_TaskRecord_t *TaskRecPtr, uint32 TaskID)
{
    TaskRecPtr->TaskId = TaskID;
    TaskRecPtr->RecordUsed = true;
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
    TaskRecPtr->TaskId = 0;
    TaskRecPtr->RecordUsed = false;
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
static inline bool CFE_ES_TaskRecordIsMatch(const CFE_ES_TaskRecord_t *TaskRecPtr, uint32 TaskID)
{
    return (TaskRecPtr != NULL && CFE_ES_TaskRecordIsUsed(TaskRecPtr) &&
            CFE_ES_TaskRecordGetID(TaskRecPtr) == TaskID);
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
    return (CounterRecPtr->RecordUsed);
}

/**
 * @brief Get the ID value from an Counter table entry
 *
 * This routine converts the table entry back to an abstract ID.
 *
 * @param[in]   CounterRecPtr   pointer to Counter table entry
 * @returns CounterID of entry
 */
static inline uint32 CFE_ES_CounterRecordGetID(const CFE_ES_GenCounterRecord_t *CounterRecPtr)
{
    /*
     * The initial implementation does not store the ID in the entry;
     * the ID is simply the zero-based index into the table.
     */
    return (CounterRecPtr - CFE_ES_Global.CounterTable);
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
static inline void CFE_ES_CounterRecordSetUsed(CFE_ES_GenCounterRecord_t *CounterRecPtr, uint32 CounterID)
{
    CounterRecPtr->RecordUsed = true;
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
    CounterRecPtr->RecordUsed = false;
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
static inline bool CFE_ES_CounterRecordIsMatch(const CFE_ES_GenCounterRecord_t *CounterRecPtr, uint32 CounterID)
{
    return (CounterRecPtr != NULL && CFE_ES_CounterRecordIsUsed(CounterRecPtr) &&
            CFE_ES_CounterRecordGetID(CounterRecPtr) == CounterID);
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

/*
** Functions used to lock/unlock shared data
*/
extern int32 CFE_ES_GetAppIDInternal(uint32 *AppIdPtr);
extern void  CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber);
extern void  CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber);


#endif
