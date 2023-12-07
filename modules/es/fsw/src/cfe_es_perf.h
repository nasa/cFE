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
 * Purpose: Performance Analyzer data structures
 *
 * Design Notes:
 *
 * References:
 *
 */

#ifndef CFE_ES_PERF_H
#define CFE_ES_PERF_H

/*
** Include Files
*/
#include "common_types.h"
#include "osconfig.h"
#include "cfe_es_api_typedefs.h"

/*
**  Defines
*/


/** @defgroup CFEESPerf Performance Analyzer Data Structures
 * @{
 */

/**
 * @brief Internal states for Performance Analyzer
 */
typedef enum CFE_ES_PerfState
{
    CFE_ES_PERF_IDLE = 0,
    CFE_ES_PERF_WAITING_FOR_TRIGGER,
    CFE_ES_PERF_TRIGGERED,
    CFE_ES_PERF_MAX_STATES
} CFE_ES_PerfState_t;

/**
 * @brief Perflog Dump Background Job states
 *
 * Writing performance log data is now handled by a state machine that runs
 * as a background job in Executive services.  When a performance log dump is
 * pending, each iteration of the state machine performs a limited amount of
 * work.  Each iteration resumes work where the last iteration left off.
 */
typedef enum
{
    CFE_ES_PerfDumpState_IDLE,                /* Placeholder for idle, no action */
    CFE_ES_PerfDumpState_INIT,                /* Placeholder for entry/init, no action */
    CFE_ES_PerfDumpState_OPEN_FILE,           /* Opening of the output file */
    CFE_ES_PerfDumpState_DELAY,               /* Wait-state to ensure in-progress writes are finished */
    CFE_ES_PerfDumpState_LOCK_DATA,           /* Locking of the global data structure */
    CFE_ES_PerfDumpState_WRITE_FS_HDR,        /* Write the CFE FS file header */
    CFE_ES_PerfDumpState_WRITE_PERF_METADATA, /* Write the Perf global metadata */
    CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES,  /* Write the Perf Log entries (throttled) */
    CFE_ES_PerfDumpState_CLEANUP,             /* Placeholder for cleanup, no action */
    CFE_ES_PerfDumpState_UNLOCK_DATA,         /* Unlocking of the global data structure */
    CFE_ES_PerfDumpState_CLOSE_FILE,          /* Closing of the output file */
    CFE_ES_PerfDumpState_MAX                  /* Placeholder for last state, no action, always last */
} CFE_ES_PerfDumpState_t;

/**
 * @brief Performance log dump state structure
 *
 * This structure is stored in global memory and keeps the state
 * of the performance log dump from one iteration to the next.
 *
 * When state is IDLE, the background task does nothing and does not
 * access or update any other members.
 *
 * The first state transition (IDLE->INIT) is triggered via ES command,
 * where the command processor sets the PendingState.
 *
 * Once state is non-IDLE, the structure becomes owned by the background
 * task.  It will progress through the remainder of the state machine,
 * eventually arriving back at IDLE when the request is completed.
 */
typedef struct
{
    CFE_ES_PerfDumpState_t CurrentState; /* the current state of the job */
    CFE_ES_PerfDumpState_t PendingState; /* the pending/next state, if transitioning */

    char      DataFileName[OS_MAX_PATH_LEN]; /* output file name from dump command */
    osal_id_t FileDesc;                      /* file descriptor for writing */
    uint32    WorkCredit;                    /* accumulator based on the passage of time */
    uint32    StateCounter;                  /* number of blocks/items left in current state */
    uint32    DataPos;                       /* last position within the Perf Log */
    size_t    FileSize;                      /* Total file size, for progress reporting in telemetry */
} CFE_ES_PerfDumpGlobal_t;

/**
 * @brief Helper function to obtain the progress/remaining items from
 * the background task that is writing the performance log data
 *
 * This is no longer just simply reading a single value from a struct,
 * as it depends on the state of the overall process.  The return value
 * from this should mimic the value which was historically
 * returned in the ES telemetry to report progress on this task.
 *
 * Foreground tasks/telemetry code shouldn't directly "peek"
 * into data structures which it does not own.
 */
uint32 CFE_ES_GetPerfLogDumpRemaining(void);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Write performance data to a file
 *
 * Implementation of the background state machine for writing
 * performance log data.
 *
 * This is implemented as a state machine that is invoked in the background
 * Each iteration should perform a limited amount of work, which will resume
 * on the next iteration.  State is kept in a global structure.
 */
bool CFE_ES_RunPerfLogDump(uint32 ElapsedTime, void *Arg);

/** @} */

#endif /* CFE_ES_PERF_H */
