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

const char *UT_OSP_MESSAGES[] =
{
        [0] = NULL, /* Message 0 is reserved */
        /* ES Startup: Error Mounting Volatile(RAM) Volume. EC = 0x~ */
        [UT_OSP_MOUNT_VOLATILE] = "ES Startup: Error Mounting Volatile(RAM) Volume. EC = 0x%08X\n",
        /* CFE_ES_ExitApp: CORE Application CFE_ES Had a Runtime Error. */
        [UT_OSP_CORE_RUNTIME] = "CFE_ES_ExitApp: CORE Application %s Had a Runtime Error.\n",
        /* ES:Call to CFE_ES_RegisterApp Failed, RC = 0x~ */
        [UT_OSP_REGISTER_APP] = "ES:Call to CFE_ES_RegisterApp Failed, RC = 0x%08X\n",
        /* ES Startup: OS_TaskCreate error creating core App: CFE_TBL: EC = 0x~ */
        [UT_OSP_CORE_APP_CREATE] = "ES Startup: OS_TaskCreate error creating core App: %s: EC = 0x%08X\n",
        /* ES Startup: Error returned when calling function: CFE_TBL_EarlyInit: EC = 0x~ */
        [UT_OSP_EARLYINIT] = "ES Startup: Error returned when calling function: %s: EC = 0x%08X\n",
        /* ES Startup: Could not find Library Init symbol:TST_LIB_Init. EC = 0x~ */
        [UT_OSP_FIND_LIBRARY] = "ES Startup: Could not find Library Init symbol:%s. EC = %d\n",
        /* POWER ON RESET due to max proc resets (HW Spec Cmd). */
        [UT_OSP_POR_MAX_HW_SPECIAL] = "POWER ON RESET due to max proc resets (HW Spec Cmd).\n",
        /* CFE_ES_ExitChildTask Error: Cannot Call from a cFE App Main Task. ID = ~ */
        [UT_OSP_CANNOT_CALL_APP_MAIN] = "CFE_ES_ExitChildTask Error: Cannot Call from a cFE App Main Task. ID = %d\n",
        /* ES Startup: bad function pointer ( table entry = 1). */
        [UT_OSP_FUNCTION_POINTER] = "ES Startup: bad function pointer ( table entry = %d).\n",
        /* ES Startup: Error Reading Startup file. EC = 0x~ */
        [UT_OSP_STARTUP_READ] = "ES Startup: Error Reading Startup file. EC = 0x%08X\n",
        /* PROCESSOR RESET called from CFE_ES_ResetCFE (Commanded). */
        [UT_OSP_PROC_RESET_COMMANDED] = "PROCESSOR RESET called from CFE_ES_ResetCFE (Commanded).\n",
        /* ES SharedData Mutex Take Err Stat=0x~,App=1,Func=TestAPI,Line=12345 */
        [UT_OSP_MUTEX_TAKE] = "ES SharedData Mutex Take Err Stat=0x%x,Func=%s,Line=%d\n",
        /* ES Startup: Startup Sync failed - Applications may not have all started */
        [UT_OSP_STARTUP_SYNC_FAIL_1] = "ES Startup: Startup Sync failed - Applications may not have all started\n",
        /* Warning: System Log full, log entry discarded. */
        [UT_OSP_SYSTEM_LOG_FULL] = "Warning: System Log full, log entry discarded.\n",
        /* ES Startup: ES Startup File Line is too long: 137 bytes. */
        [UT_OSP_FILE_LINE_TOO_LONG] = "ES Startup: ES Startup File Line is too long: %u bytes.\n",
        /* ES Startup: Load Shared Library Init Error. */
        [UT_OSP_SHARED_LIBRARY_INIT] = "ES Startup: Load Shared Library Init Error = 0x%08x\n",
        /* ES Startup: Error Removing Volatile(RAM) Volume. EC = 0x~ */
        [UT_OSP_REMOVE_VOLATILE] = "ES Startup: Error Removing Volatile(RAM) Volume. EC = 0x%08X\n",
        /* POWER ON RESET due to other cause (See Subtype). */
        [UT_OSP_POR_OTHER] = "POWER ON RESET due to other cause (See Subtype).\n",
        /* ES Startup: Error Determining Blocks Free on Volume. EC = 0x~ */
        [UT_OSP_DETERMINE_BLOCKS] = "ES Startup: Error Determining Blocks Free on Volume. EC = 0x%08X\n",
        /* ES Startup: Startup Sync failed - Applications may not have all initialized */
        [UT_OSP_STARTUP_SYNC_FAIL_2] = "ES Startup: Startup Sync failed - Applications may not have all initialized\n",
        /* ES Startup: No free library slots available */
        [UT_OSP_LIBRARY_SLOTS] = "ES Startup: No free library slots available\n",
        /* ES Startup: Unable to extract filename from path: ut/filename.gz. */
        [UT_OSP_EXTRACT_FILENAME_UT] = "ES Startup: Unable to extract filename from path: %s.\n",
        /* ES Startup: Application path plus file name length (~) exceeds max allowed (~) */
        [UT_OSP_APP_PATH_FILE_TOO_LONG] = "ES Startup: Application path plus file name length (%d) exceeds max allowed (%d)\n",
        /* ES Startup: Error Re-Formating Volatile(RAM) Volume. EC = 0x~ */
        [UT_OSP_REFORMAT_VOLATILE] = "ES Startup: Error Re-Formating Volatile(RAM) Volume. EC = 0x%08X\n",
        /* ES Startup: Could not load cFE application file:ut/filename.x. EC = 0x~ */
        [UT_OSP_EXTRACT_FILENAME_UT55] = "ES Startup: Could not load file:%s. EC = 0x%08X\n",
        /* ES Startup: Unable to extract filename from path: ut46/ */
        [UT_OSP_EXTRACT_FILENAME_UT46] = "ES Startup: Unable to extract filename from path: %s.\n",
        /* ES Startup: No free application slots available */
        [UT_OSP_NO_FREE_APP_SLOTS] = "ES Startup: No free application slots available\n",
        /* ES Startup: Unable to extract filename from path: ut57/ */
        [UT_OSP_EXTRACT_FILENAME_UT57] = "ES Startup: Unable to extract filename from path: %s.\n",
        /* ES SharedData Mutex Give Err Stat=0x~,App=1,Func=TestAPI,Line=98765 */
        [UT_OSP_MUTEX_GIVE] = "ES SharedData Mutex Give Err Stat=0x%x,Func=%s,Line=%d\n",
        /* ES Startup: Could not find symbol:EntryPoint. EC = 0x~ */
        [UT_OSP_CANNOT_FIND_SYMBOL] = "ES Startup: Could not find symbol:%s. EC = 0x%08X\n",
        /* ES Startup: Error Initializing Volatile(RAM) Volume. EC = 0x~ */
        [UT_OSP_INIT_VOLATILE] = "ES Startup: Error Initializing Volatile(RAM) Volume. EC = 0x%08X\n",
        /* ES:Application Init Failed,RC=0x~ */
        [UT_OSP_APP_INIT] = "ES:Application Init Failed,RC=0x%08X\n",
        /* POWER ON RESET due to max proc resets (Commanded). */
        [UT_OSP_POR_MAX_PROC_RESETS] = "POWER ON RESET due to max proc resets (Commanded).\n",
        /* CFE_ES_RestartApp: Cannot Restart Application appName, It is not running. */
        [UT_OSP_CANNOT_RESTART_APP] = "CFE_ES_RestartApp: Cannot Restart Application %s, It is not running.\n",
        /* ES Startup: Insufficent Free Space on Volatile Disk, Reformatting. */
        [UT_OSP_INSUFF_FREE_SPACE] = "ES Startup: Insufficent Free Space on Volatile Disk, Reformatting.\n",
        /* ES Startup: Could not load cFE Shared Library */
        [UT_OSP_LOAD_SHARED_LIBRARY] = "ES Startup: Could not load cFE Shared Library\n",
        /* POWER ON RESET due to HW Special Cmd (Hw Spec Cmd). */
        [UT_OSP_POR_HW_SPECIAL] = "POWER ON RESET due to HW Special Cmd (Hw Spec Cmd).\n",
        /* ES Startup: AppCreate Error: TaskCreate AppName Failed. EC = 0x~! */
        [UT_OSP_APP_CREATE] = "ES Startup: AppCreate Error: TaskCreate %s Failed. EC = 0x%08X!\n",
        /* ES Startup: Error Creating Volatile(RAM) Volume. EC = 0x~ */
        [UT_OSP_CREATE_VOLATILE] = "ES Startup: Error Creating Volatile(RAM) Volume. EC = 0x%08X\n",
        /* ES Startup: Failed to unload APP: AppName. EC = 0x~ */
        [UT_OSP_MODULE_UNLOAD_FAILED] = "ES Startup: Failed to unload: %s. EC = 0x%08X\n",
        /* POWERON RESET called from CFE_ES_ResetCFE (Commanded). */
        [UT_OSP_POR_COMMANDED] = "POWERON RESET called from CFE_ES_ResetCFE (Commanded).\n",
        /* ES Startup: Error Re-Mounting Volatile(RAM) Volume. EC = 0x~ */
        [UT_OSP_REMOUNT_VOLATILE] = "ES Startup: Error Re-Mounting Volatile(RAM) Volume. EC = 0x%08X\n",
        /* CFE_ES_ExitApp, Cannot Exit CORE Application CFE_ES */
        [UT_OSP_CORE_APP_EXIT] = "CFE_ES_ExitApp, Cannot Exit CORE Application %s\n",
        /* ES Startup: Opened ES App Startup file:  */
        [UT_OSP_ES_APP_STARTUP_OPEN] = "ES Startup: Opened ES App Startup file: %s\n",
        /* CFE_ES_ExitApp: CORE Application CFE_ES Had an Init Error. */
        [UT_OSP_CORE_INIT] = "CFE_ES_ExitApp: CORE Application %s Had an Init Error.\n",
        /* PROCESSOR RESET due to Hardware Special Command (HW Spec Cmd). */
        [UT_OSP_PROC_RESET_MAX_HW_SPECIAL] = "PROCESSOR RESET due to Hardware Special Command (HW Spec Cmd).\n",
        /* ES:Error reading cmd pipe,RC=0x~ */
        [UT_OSP_COMMAND_PIPE] = "ES:Error reading cmd pipe,RC=0x%08X\n",
        /* ES Startup: Error Un-Mounting Volatile(RAM) Volume. EC = 0x~ */
        [UT_OSP_UNMOUNT_VOLATILE] = "ES Startup: Error Un-Mounting Volatile(RAM) Volume. EC = 0x%08X\n",
        /* ES Startup: Error: ES_TaskTable slot in use at task creation! */
        [UT_OSP_TABLE_SLOT_IN_USE] = "ES Startup: Error: ES_TaskTable slot in use at task creation!\n",
        /* ES Startup: Error, Can't Open ES App Startup file:  */
        [UT_OSP_CANNOT_OPEN_ES_APP_STARTUP] = "ES Startup: Error, Can't Open ES App Startup file: %s EC = 0x%08X\n",
        /* ES Startup: Unable to extract filename from path: /cf/apps/tst_lib.bundle.gz. */
        [UT_OSP_EXTRACT_FILENAME_CF] = "ES Startup: Unable to extract filename from path: %s.\n",
        /* Warning: Invalid System Log mode, log entry discarded. */
        [UT_OSP_INVALID_LOG_MODE] = "Warning: Invalid System Log mode, log entry discarded.\n",
        /* ES Startup: Library path plus file name length (~) exceeds max allowed (~) */
        [UT_OSP_LIB_PATH_FILE_TOO_LONG] = "ES Startup: Library path plus file name length (%d) exceeds max allowed (%d)\n",
        /* ES Startup: Unable to decompress Application File: ut/filename.gz */
        [UT_OSP_DECOMPRESS_APP] = "ES Startup: Unable to decompress Application File: %s\n",
        /* ES Startup: Unable to decompress library file: /cf/apps/tst_lib.bundle.gz */
        [UT_OSP_DECOMPRESS_LIBRARY] = "ES Startup: Unable to decompress library file: %s\n",
        /* CFE_ES_ExitChildTask Error Calling CFE_ES_GetAppID. Task ID = ~, RC = 0x~ */
        [UT_OSP_GET_APP_ID] = "CFE_ES_ExitChildTask Error Calling CFE_ES_GetAppID. Task ID = %d, RC = 0x%08X\n",
        /* ES Startup: Error, No free application slots available for CORE App! */
        [UT_OSP_NO_FREE_CORE_APP_SLOTS] = "ES Startup: Error, No free application slots available for CORE App!\n",
        /* ES Startup: CFE_ES_Global.TaskTable record used error for App: CFE_EVS, continuing. */
        [UT_OSP_RECORD_USED] = "ES Startup: Error: ES_TaskTable slot for ID %lx in use at task creation!\n",
        /* CFE_ES_ExitChildTask called from invalid task context */
        [UT_OSP_TASKEXIT_BAD_CONTEXT] = "CFE_ES_ExitChildTask called from invalid task context\n",
};

