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
 *  This file contains the ES global data definitions.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 */

#ifndef CFE_ES_GLOBAL_H
#define CFE_ES_GLOBAL_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_es_msg.h"
#include "cfe_es_api_typedefs.h"

#include "cfe_es_erlog_typedef.h"
#include "cfe_es_resetdata_typedef.h"
#include "cfe_es_cds.h"

#include <signal.h> /* for sig_atomic_t */

/*
** Typedefs
*/

/*
** CFE_ES_GenCounterRecord_t is an internal structure used to keep track of
** Generic Counters that are active in the system.
*/
typedef struct
{
    CFE_ES_CounterId_t CounterId; /**< The actual counter ID of this entry, or undefined */
    uint32             Counter;
    char               CounterName[OS_MAX_API_NAME]; /* Counter Name */
} CFE_ES_GenCounterRecord_t;

/*
 * Encapsulates the state of the ES background task
 */
typedef struct
{
    CFE_ES_TaskId_t TaskID;         /**< ES ID of the background task */
    osal_id_t       WorkSem;        /**< Semaphore that is given whenever background work is pending */
    uint32          NumJobsRunning; /**< Current Number of active jobs (updated by background task) */
} CFE_ES_BackgroundTaskState_t;

/*
 * Background log dump state structure
 *
 * This structure is stored in global memory and keeps the state
 * of the log dump from one iteration to the next.
 *
 * NOTE: This is used for log structures which are expected to be small
 * enough so such that it is not necessary to throttle the file write or
 * spread it over time.
 *
 * Therefore, the only thing necessary to be stored is whether there
 * is a pending write request, and the data file name.
 *
 * Larger log files, such as the Perf log, must implement a state machine
 * with a dedicated state data structure.
 */
typedef struct
{
    CFE_FS_FileWriteMetaData_t FileWrite;   /**< FS state data - must be first */
    CFE_ES_ERLog_FileEntry_t   EntryBuffer; /**< Temp holding area for record to write */
} CFE_ES_BackgroundLogDumpGlobal_t;

/*
** Type definition (ES task global data)
*/
typedef struct
{
    /*
    ** ES Task command interface counters
    */
    uint8 CommandCounter;
    uint8 CommandErrorCounter;

    /*
    ** ES Task housekeeping telemetry
    */
    CFE_ES_HousekeepingTlm_t HkPacket;

    /*
    ** Single application telemetry
    */
    CFE_ES_OneAppTlm_t OneAppPacket;

    /*
    ** Memory statistics telemetry
    */
    CFE_ES_MemStatsTlm_t MemStatsPacket;

    /*
    ** ES Task operational data (not reported in housekeeping)
    */
    CFE_SB_PipeId_t CmdPipe;
} CFE_ES_TaskData_t;

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
    volatile sig_atomic_t SystemState;

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
    CFE_ResourceId_t   LastAppId;
    CFE_ES_AppRecord_t AppTable[CFE_PLATFORM_ES_MAX_APPLICATIONS];

    /*
    ** ES Shared Library Table
    */
    uint32             RegisteredLibs;
    CFE_ResourceId_t   LastLibId;
    CFE_ES_LibRecord_t LibTable[CFE_PLATFORM_ES_MAX_LIBRARIES];

    /*
    ** ES Generic Counters Table
    */
    CFE_ResourceId_t          LastCounterId;
    CFE_ES_GenCounterRecord_t CounterTable[CFE_PLATFORM_ES_MAX_GEN_COUNTERS];

    /*
    ** Critical Data Store Management Variables
    */
    CFE_ES_CDS_Instance_t CDSVars;
    bool                  CDSIsAvailable; /**< \brief Whether or not the CDS service is active/valid */

    /*
     * Background task for handling long-running, non real time tasks
     * such as maintenance, file writes, and other items.
     */
    CFE_ES_BackgroundTaskState_t BackgroundTask;

    /*
    ** Memory Pools
    */
    CFE_ResourceId_t       LastMemPoolId;
    CFE_ES_MemPoolRecord_t MemPoolTable[CFE_PLATFORM_ES_MAX_MEMORY_POOLS];

    /*
    ** ES Task initialization data (not reported in housekeeping)
    */
    CFE_ES_BackgroundLogDumpGlobal_t BackgroundERLogDumpState;

    /*
     * Persistent state data associated with performance log data file writes
     */
    CFE_ES_PerfDumpGlobal_t BackgroundPerfDumpState;

    /*
     * Persistent state data associated with background app table scans
     */
    CFE_ES_AppTableScanState_t BackgroundAppScanState;

    /*
     * Task global data (formerly a separate global).
     */
    CFE_ES_TaskData_t TaskData;

    /*
     * Pointer to the Reset data that is preserved on a processor reset
     */
    CFE_ES_ResetData_t *ResetDataPtr;
} CFE_ES_Global_t;

/*
** The Executive Services Global Data declaration
*/
extern CFE_ES_Global_t CFE_ES_Global;

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain exclusive access to the ES global data structures
 *
 * ES internal function to take the Shared Data Mutex and handle
 * error conditions.
 *
 * @param FunctionName   the name of the function/caller
 * @param LineNumber     the line number of the caller
 */
void CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Release exclusive access to the ES global data structures
 *
 * ES internal function to Release the shared data mutex and handle error
 * conditions.
 *
 * @param FunctionName   the name of the function/caller
 * @param LineNumber     the line number of the caller
 */
void CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber);

#endif /* CFE_ES_GLOBAL_H */
