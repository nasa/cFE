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
** File: ut_osprintf_stubs.c
**
** Purpose:
** Unit test stubs for Operating System routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/

#include "ut_support.h"
#include "ut_osprintf_stubs.h"

/*
** Global variables
*/

const char *UT_OSP_MESSAGES[] = {
    [0]                                 = NULL, /* Message 0 is reserved */
    [UT_OSP_MOUNT_VOLATILE]             = "%s: Error Mounting Volatile(RAM) Volume. EC = %ld\n",
    [UT_OSP_CORE_RUNTIME]               = "%s: CORE Application %s Had a Runtime Error.\n",
    [UT_OSP_CORE_APP_CREATE]            = "%s: OS_TaskCreate error creating core App: %s: EC = %ld\n",
    [UT_OSP_EARLYINIT]                  = "%s: Error returned when calling function: %s: EC = 0x%08X\n",
    [UT_OSP_FIND_LIBRARY]               = "%s: Could not find Library Init symbol:%s. EC = %d\n",
    [UT_OSP_POR_MAX_HW_SPECIAL]         = "%s: POWER ON RESET due to max proc resets (HW Spec Cmd).\n",
    [UT_OSP_CANNOT_CALL_APP_MAIN]       = "%s: Error: Cannot Call from a cFE App Main Task. ID = %d\n",
    [UT_OSP_FUNCTION_POINTER]           = "%s: bad function pointer ( table entry = %d).\n",
    [UT_OSP_STARTUP_READ]               = "%s: Error Reading Startup file. EC = %ld\n",
    [UT_OSP_PROC_RESET_COMMANDED]       = "%s: PROCESSOR RESET called from CFE_ES_ResetCFE (Commanded).\n",
    [UT_OSP_MUTEX_TAKE]                 = "%s: SharedData Mutex Take Err Stat=%ld,Func=%s,Line=%d\n",
    [UT_OSP_STARTUP_SYNC_FAIL_1]        = "%s: Startup Sync failed - Applications may not have all started\n",
    [UT_OSP_SYSTEM_LOG_FULL]            = "Warning: System Log full, log entry discarded.\n",
    [UT_OSP_FILE_LINE_TOO_LONG]         = "%s: **WARNING** File Line %u is malformed: %u bytes, %u tokens.\n",
    [UT_OSP_SHARED_LIBRARY_INIT]        = "%s: Load Shared Library Init Error = %ld\n",
    [UT_OSP_REMOVE_VOLATILE]            = "%s: Error Removing Volatile(RAM) Volume. EC = %ld\n",
    [UT_OSP_POR_OTHER]                  = "%s: POWER ON RESET due to other cause (See Subtype).\n",
    [UT_OSP_DETERMINE_BLOCKS]           = "%s: Error Determining Blocks Free on Volume. EC = %ld\n",
    [UT_OSP_STARTUP_SYNC_FAIL_2]        = "%s: Startup Sync failed - Applications may not have all initialized\n",
    [UT_OSP_LIBRARY_SLOTS]              = "%s: No free library slots available\n",
    [UT_OSP_EXTRACT_FILENAME_UT]        = "%s: Unable to extract filename from path: %s.\n",
    [UT_OSP_APP_PATH_FILE_TOO_LONG]     = "%s: Application path plus file name length (%d) exceeds max allowed (%d)\n",
    [UT_OSP_REFORMAT_VOLATILE]          = "%s: Error Re-Formatting Volatile(RAM) Volume. EC = %ld\n",
    [UT_OSP_EXTRACT_FILENAME_UT55]      = "%s: Could not load file:%s. EC = %ld\n",
    [UT_OSP_EXTRACT_FILENAME_UT46]      = "%s: Unable to extract filename from path: %s.\n",
    [UT_OSP_NO_FREE_APP_SLOTS]          = "%s: No free application slots available\n",
    [UT_OSP_EXTRACT_FILENAME_UT57]      = "%s: Unable to extract filename from path: %s.\n",
    [UT_OSP_MUTEX_GIVE]                 = "%s: SharedData Mutex Give Err Stat=%ld,Func=%s,Line=%d\n",
    [UT_OSP_CANNOT_FIND_SYMBOL]         = "%s: Could not find symbol:%s. EC = %ld\n",
    [UT_OSP_INIT_VOLATILE]              = "%s: Error Initializing Volatile(RAM) Volume. EC = %ld\n",
    [UT_OSP_APP_INIT]                   = "%s: Application Init Failed,RC=0x%08X\n",
    [UT_OSP_POR_MAX_PROC_RESETS]        = "%s: POWER ON RESET due to max proc resets (Commanded).\n",
    [UT_OSP_CANNOT_RESTART_APP]         = "%s: Cannot Restart Application %s, It is not running.\n",
    [UT_OSP_INSUFF_FREE_SPACE]          = "%s: Insufficient Free Space on Volatile Disk, Reformatting.\n",
    [UT_OSP_LOAD_SHARED_LIBRARY]        = "%s: Could not load cFE Shared Library\n",
    [UT_OSP_POR_HW_SPECIAL]             = "%s: POWER ON RESET due to HW Special Cmd (Hw Spec Cmd).\n",
    [UT_OSP_APP_CREATE]                 = "%s: AppCreate Error: TaskCreate %s Failed. EC = %ld!\n",
    [UT_OSP_CREATE_VOLATILE]            = "%s: Error Creating Volatile(RAM) Volume. EC = %ld\n",
    [UT_OSP_MODULE_UNLOAD_FAILED]       = "%s: Failed to unload: %s. EC = %ld\n",
    [UT_OSP_POR_COMMANDED]              = "%s: POWERON RESET called from CFE_ES_ResetCFE (Commanded).\n",
    [UT_OSP_REMOUNT_VOLATILE]           = "%s: Error Re-Mounting Volatile(RAM) Volume. EC = %ld\n",
    [UT_OSP_CORE_APP_EXIT]              = "%s: Cannot Exit CORE Application %s\n",
    [UT_OSP_ES_APP_STARTUP_OPEN]        = "%s: Opened ES App Startup file: %s\n",
    [UT_OSP_CORE_INIT]                  = "%s: CORE Application %s Had an Init Error.\n",
    [UT_OSP_PROC_RESET_MAX_HW_SPECIAL]  = "%s: PROCESSOR RESET due to Hardware Special Command (HW Spec Cmd).\n",
    [UT_OSP_COMMAND_PIPE]               = "%s: Error reading cmd pipe,RC=%ld\n",
    [UT_OSP_UNMOUNT_VOLATILE]           = "%s: Error Un-Mounting Volatile(RAM) Volume. EC = %ld\n",
    [UT_OSP_TABLE_SLOT_IN_USE]          = "%s: Error: ES_TaskTable slot in use at task creation!\n",
    [UT_OSP_CANNOT_OPEN_ES_APP_STARTUP] = "%s: Error, Can't Open ES App Startup file: %s, EC = %ld\n",
    [UT_OSP_EXTRACT_FILENAME_CF]        = "%s: Unable to extract filename from path: %s.\n",
    [UT_OSP_INVALID_LOG_MODE]           = "Warning: Invalid System Log mode, log entry discarded.\n",
    [UT_OSP_LIB_PATH_FILE_TOO_LONG]     = "%s: Library path plus file name length (%d) exceeds max allowed (%d)\n",
    [UT_OSP_DECOMPRESS_APP]             = "%s: Unable to decompress Application File: %s\n",
    [UT_OSP_DECOMPRESS_LIBRARY]         = "%s: Unable to decompress library file: %s\n",
    [UT_OSP_GET_APP_ID]                 = "%s: Error Calling CFE_ES_GetAppID. Task ID = %d, RC = 0x%08X\n",
    [UT_OSP_NO_FREE_CORE_APP_SLOTS]     = "%s: Error, No free application slots available for CORE App!\n",
    [UT_OSP_RECORD_USED]                = "%s: Error: ES_TaskTable slot for ID %lx in use at task creation!\n",
    [UT_OSP_TASKEXIT_BAD_CONTEXT]       = "%s: Called from invalid task context\n",
    [UT_OSP_BACKGROUND_TAKE]            = "%s: Failed to take background sem: %08lx\n",
    [UT_OSP_INVALID_ID]                 = "%s: Invalid Application ID received, AppID = %lu\n",
    [UT_OSP_RESTART_NO_FILE]            = "%s: Cannot Restart Application %s, File %s does not exist.\n",
    [UT_OSP_CREATECHILD_FROM_CHILD]     = "%s: Error: Cannot call from a Child Task (for Task '%s').\n",
    [UT_OSP_DELETECHID_MAIN_TASK]       = "%s: Error: Task %lu is a cFE Main Task.\n",
    [UT_OSP_POOLCREATE_TOO_SMALL]       = "%s: Pool size(%lu) too small, need >=%lu bytes\n",
    [UT_OSP_GETPOOL_BAD_HANDLE]         = "%s: Err:Bad handle(0x%08lX) AppId=%lu\n",
    [UT_OSP_PUTPOOL_BAD_HANDLE]         = "%s: Err:Invalid Memory Handle (0x%08lX).\n",
    [UT_OSP_FORMAT_VOLATILE]            = "%s: Formatting Volatile(RAM) Volume.\n",
    [UT_OSP_RELOAD_NO_FILE]             = "%s: Cannot Reload Application %s, File %s does not exist.\n",
    [UT_OSP_EXTERNAL_APP_EXIT]          = "%s: Application %s called CFE_ES_ExitApp\n",
};
