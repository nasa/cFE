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

/*
** File: cfe_es_backgroundtask.c
**
** Purpose: This file contains the implementation of the ES "background task"
**
** This task sits idle most of the time, but is woken by the ES application
** for various maintenance duties that may take time to execute, such as
** writing status/log files.
**
*/

/*
** Include Section
*/

#include <string.h>

#include "cfe_es_module_all.h"
#include "cfe_fs_core_internal.h"

#define CFE_ES_BACKGROUND_SEM_NAME         "ES_BG_SEM"
#define CFE_ES_BACKGROUND_CHILD_NAME       "ES_BG_TASK"
#define CFE_ES_BACKGROUND_CHILD_STACK_PTR  CFE_ES_TASK_STACK_ALLOCATE
#define CFE_ES_BACKGROUND_CHILD_STACK_SIZE CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE
#define CFE_ES_BACKGROUND_CHILD_PRIORITY   CFE_PLATFORM_ES_PERF_CHILD_PRIORITY
#define CFE_ES_BACKGROUND_CHILD_FLAGS      0
#define CFE_ES_BACKGROUND_MAX_IDLE_DELAY   30000 /* 30 seconds */

typedef struct
{
    bool (*RunFunc)(uint32 ElapsedTime, void *Arg);
    void * JobArg;
    uint32 ActivePeriod; /**< max wait/delay time between calls when job is active */
    uint32 IdlePeriod;   /**< max wait/delay time between calls when job is idle */
} CFE_ES_BackgroundJobEntry_t;

/*
 * List of "background jobs"
 *
 * This is just a list of functions to periodically call from the context of the background task,
 * and can be added/extended as needed.
 *
 * Each Job function returns a boolean, and should return "true" if it is active, or "false" if it is idle.
 *
 * This uses "cooperative multitasking" -- the function should do some limited work, then return to the
 * background task.  It will be called again after a delay period to do more work.
 */
const CFE_ES_BackgroundJobEntry_t CFE_ES_BACKGROUND_JOB_TABLE[] = {
    {/* ES app table background scan */
     .RunFunc      = CFE_ES_RunAppTableScan,
     .JobArg       = &CFE_ES_Global.BackgroundAppScanState,
     .ActivePeriod = CFE_PLATFORM_ES_APP_SCAN_RATE / 4,
     .IdlePeriod   = CFE_PLATFORM_ES_APP_SCAN_RATE},
    {/* Performance Log Data Dump to file */
     .RunFunc      = CFE_ES_RunPerfLogDump,
     .JobArg       = &CFE_ES_Global.BackgroundPerfDumpState,
     .ActivePeriod = CFE_PLATFORM_ES_PERF_CHILD_MS_DELAY,
     .IdlePeriod   = CFE_PLATFORM_ES_PERF_CHILD_MS_DELAY * 1000},
    {/* Check for exceptions stored in the PSP */
     .RunFunc      = CFE_ES_RunExceptionScan,
     .JobArg       = NULL,
     .ActivePeriod = CFE_PLATFORM_ES_APP_SCAN_RATE,
     .IdlePeriod   = CFE_PLATFORM_ES_APP_SCAN_RATE},
    {/* Call FS to handle background file writes */
     .RunFunc      = CFE_FS_RunBackgroundFileDump,
     .JobArg       = NULL,
     .ActivePeriod = CFE_PLATFORM_ES_APP_SCAN_RATE,
     .IdlePeriod   = CFE_PLATFORM_ES_APP_SCAN_RATE}};

#define CFE_ES_BACKGROUND_NUM_JOBS (sizeof(CFE_ES_BACKGROUND_JOB_TABLE) / sizeof(CFE_ES_BACKGROUND_JOB_TABLE[0]))

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_BackgroundTask(void)
{
    int32                              OsStatus;
    uint32                             JobTotal;
    uint32                             NumJobsRunning;
    uint32                             NextDelay;
    uint32                             ElapsedTime;
    OS_time_t                          CurrTime;
    OS_time_t                          LastTime;
    const CFE_ES_BackgroundJobEntry_t *JobPtr;

    memset(&LastTime, 0, sizeof(LastTime));
    memset(&CurrTime, 0, sizeof(CurrTime));

    CFE_PSP_GetTime(&LastTime);

    while (true)
    {
        /*
         * compute the elapsed time (difference) between last
         * execution and now, in milliseconds.
         */
        CFE_PSP_GetTime(&CurrTime);
        ElapsedTime = OS_TimeGetTotalMilliseconds(OS_TimeSubtract(CurrTime, LastTime));
        LastTime    = CurrTime;

        NextDelay      = CFE_ES_BACKGROUND_MAX_IDLE_DELAY; /* default; will be adjusted based on active jobs */
        JobPtr         = CFE_ES_BACKGROUND_JOB_TABLE;
        JobTotal       = CFE_ES_BACKGROUND_NUM_JOBS;
        NumJobsRunning = 0;

        while (JobTotal > 0)
        {
            /*
             * call the background job -
             * if it returns "true" that means it is active,
             * if it returns "false" that means it is idle
             */
            if (JobPtr->RunFunc != NULL && JobPtr->RunFunc(ElapsedTime, JobPtr->JobArg))
            {
                ++NumJobsRunning;

                if (JobPtr->ActivePeriod != 0 && NextDelay > JobPtr->ActivePeriod)
                {
                    /* next delay is based on this active job wait time */
                    NextDelay = JobPtr->ActivePeriod;
                }
            }
            else if (JobPtr->IdlePeriod != 0 && NextDelay > JobPtr->IdlePeriod)
            {
                /* next delay is based on this idle job wait time */
                NextDelay = JobPtr->IdlePeriod;
            }
            --JobTotal;
            ++JobPtr;
        }

        CFE_ES_Global.BackgroundTask.NumJobsRunning = NumJobsRunning;

        OsStatus = OS_BinSemTimedWait(CFE_ES_Global.BackgroundTask.WorkSem, NextDelay);
        if (OsStatus != OS_SUCCESS && OsStatus != OS_SEM_TIMEOUT)
        {
            /* should never occur */
            CFE_ES_WriteToSysLog("%s: Failed to take background sem: %ld\n", __func__, (long)OsStatus);
            break;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_BackgroundInit(void)
{
    int32 status;
    int32 OsStatus;

    OsStatus = OS_BinSemCreate(&CFE_ES_Global.BackgroundTask.WorkSem, CFE_ES_BACKGROUND_SEM_NAME, 0, 0);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Failed to create background sem: %ld\n", __func__, (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /* Spawn a task to write the performance data to a file */
    status = CFE_ES_CreateChildTask(&CFE_ES_Global.BackgroundTask.TaskID, CFE_ES_BACKGROUND_CHILD_NAME,
                                    CFE_ES_BackgroundTask, CFE_ES_BACKGROUND_CHILD_STACK_PTR,
                                    CFE_ES_BACKGROUND_CHILD_STACK_SIZE, CFE_ES_BACKGROUND_CHILD_PRIORITY,
                                    CFE_ES_BACKGROUND_CHILD_FLAGS);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Failed to create background task: %08lx\n", __func__, (unsigned long)status);
        return status;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_BackgroundCleanup(void)
{
    CFE_ES_DeleteChildTask(CFE_ES_Global.BackgroundTask.TaskID);
    OS_BinSemDelete(CFE_ES_Global.BackgroundTask.WorkSem);

    CFE_ES_Global.BackgroundTask.TaskID  = CFE_ES_TASKID_UNDEFINED;
    CFE_ES_Global.BackgroundTask.WorkSem = OS_OBJECT_ID_UNDEFINED;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_BackgroundWakeup(void)
{
    /* wake up the background task by giving the sem.
     * This is "informational" and not strictly required,
     * but it will make the task immediately wake up and check for new
     * work if it was idle. */
    OS_BinSemGive(CFE_ES_Global.BackgroundTask.WorkSem);
}
