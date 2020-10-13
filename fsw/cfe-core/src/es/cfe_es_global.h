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
#include "cfe_es_generic_pool.h"
#include "cfe_es_mempool.h"
#include "cfe_es_cds_mempool.h"
#include "cfe_time.h"
#include "cfe_platform_cfg.h"
#include "cfe_evs.h"
#include "cfe_psp.h"


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

/*
** Typedefs
*/

/*
** CFE_ES_GenCounterRecord_t is an internal structure used to keep track of
** Generic Counters that are active in the system.
*/
typedef struct
{
   CFE_ES_ResourceID_t CounterId;   /**< The actual counter ID of this entry, or undefined */
   uint32         Counter;
   char           CounterName[OS_MAX_API_NAME];   /* Counter Name */
} CFE_ES_GenCounterRecord_t;

/*
 * Encapsulates the state of the ES background task
 */
typedef struct
{
    CFE_ES_ResourceID_t TaskID; /**< ES ID of the background task */
    osal_id_t WorkSem;          /**< Semaphore that is given whenever background work is pending */
    uint32 NumJobsRunning;      /**< Current Number of active jobs (updated by background task) */
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
   osal_id_t SharedDataMutex;

   /*
   ** Performance Data Mutex
   */
   osal_id_t PerfDataMutex;

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
   CFE_ES_CDS_Instance_t CDSVars;
   bool                  CDSIsAvailable;        /**< \brief Whether or not the CDS service is active/valid */

   /*
    * Background task for handling long-running, non real time tasks
    * such as maintenance, file writes, and other items.
    */
   CFE_ES_BackgroundTaskState_t BackgroundTask;

   /*
   ** Memory Pools
   */
   CFE_ES_MemPoolRecord_t MemPoolTable[CFE_PLATFORM_ES_MAX_MEMORY_POOLS];

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
** Functions used to lock/unlock shared data
*/
extern int32 CFE_ES_GetAppIDInternal(CFE_ES_ResourceID_t *AppIdPtr);
extern void  CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber);
extern void  CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber);


#endif
