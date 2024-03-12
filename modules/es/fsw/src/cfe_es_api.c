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
**  File:
**    cfe_es_api.c
**
**  Purpose:
**    This file implements the cFE Executive Services API functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Required header files.
*/
#include "cfe_es_module_all.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr)
{
    if (ResetSubtypePtr != NULL)
    {
        *ResetSubtypePtr = CFE_ES_Global.ResetDataPtr->ResetVars.ResetSubtype;
    }

    return CFE_ES_Global.ResetDataPtr->ResetVars.ResetType;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_ResetCFE(uint32 ResetType)
{
    int32 ReturnCode;

    if (ResetType == CFE_PSP_RST_TYPE_PROCESSOR)
    {
        /*
        ** Increment the processor reset count
        */
        CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount++;

        /*
        ** Before doing a Processor reset, check to see
        ** if the maximum number has been exceeded
        */
        if (CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount >
            CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount)
        {
            CFE_ES_WriteToSysLog("%s: POWER ON RESET due to max proc resets (Commanded).\n", __func__);

            /*
            ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
            ** the entry just in case something fails.
            */
            CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, CFE_PSP_RST_SUBTYPE_RESET_COMMAND,
                                "POWER ON RESET due to max proc resets (Commanded).");
            /*
            ** Call the BSP reset routine
            */
            CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: PROCESSOR RESET called from CFE_ES_ResetCFE (Commanded).\n", __func__);

            /*
            ** Update the reset variables
            */
            CFE_ES_Global.ResetDataPtr->ResetVars.ES_CausedReset = true;

            /*
            ** Log the reset in the ER Log
            */
            CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_RESET_COMMAND,
                                "PROCESSOR RESET called from CFE_ES_ResetCFE (Commanded).");
            /*
            ** Call the BSP reset routine
            */
            CFE_PSP_Restart(CFE_PSP_RST_TYPE_PROCESSOR);
        }

        /*
        ** If the BSP routine is not implemented,
        ** it will return.
        */
        ReturnCode = CFE_ES_NOT_IMPLEMENTED;
    }
    else if (ResetType == CFE_PSP_RST_TYPE_POWERON)
    {
        CFE_ES_WriteToSysLog("%s: POWERON RESET called from CFE_ES_ResetCFE (Commanded).\n", __func__);

        /*
        ** Log the reset in the ER Log. The log will be wiped out, but it's good to have
        ** the entry just in case something fails.
        */
        CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, CFE_PSP_RST_SUBTYPE_RESET_COMMAND,
                            "POWERON RESET called from CFE_ES_ResetCFE (Commanded).");

        /*
        ** Call the BSP reset routine
        */
        CFE_PSP_Restart(CFE_PSP_RST_TYPE_POWERON);

        /*
        ** If the BSP routine is not implemented,
        ** it will return.
        */
        ReturnCode = CFE_ES_NOT_IMPLEMENTED;
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: Invalid Reset Type: %d.\n", __func__, (int)ResetType);
        ReturnCode = CFE_ES_BAD_ARGUMENT;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_RestartApp(CFE_ES_AppId_t AppID)
{
    int32               ReturnCode = CFE_SUCCESS;
    os_fstat_t          FileStatus;
    CFE_ES_AppRecord_t *AppRecPtr;

    AppRecPtr = CFE_ES_LocateAppRecordByID(AppID);
    if (AppRecPtr != NULL)
    {
        CFE_ES_LockSharedData(__func__, __LINE__);

        /*
        ** Check to see if the App is an external cFE App.
        */
        if (AppRecPtr->Type == CFE_ES_AppType_CORE)
        {
            CFE_ES_SysLogWrite_Unsync("%s: Cannot Restart a CORE Application: %s.\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
        else if (AppRecPtr->AppState != CFE_ES_AppState_RUNNING)
        {
            CFE_ES_SysLogWrite_Unsync("%s: Cannot Restart Application %s, It is not running.\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
        else
        {
            /*
            ** Check to see if the file exists
            */
            if (OS_stat(AppRecPtr->StartParams.BasicInfo.FileName, &FileStatus) == OS_SUCCESS)
            {
                CFE_ES_SysLogWrite_Unsync("%s: Restart Application %s Initiated\n", __func__,
                                          CFE_ES_AppRecordGetName(AppRecPtr));
                AppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RESTART;
            }
            else
            {
                CFE_ES_SysLogWrite_Unsync("%s: Cannot Restart Application %s, File %s does not exist.\n", __func__,
                                          CFE_ES_AppRecordGetName(AppRecPtr),
                                          AppRecPtr->StartParams.BasicInfo.FileName);
                ReturnCode = CFE_ES_FILE_IO_ERR;
            }
        }

        CFE_ES_UnlockSharedData(__func__, __LINE__);
    }
    else /* App ID is not valid */
    {
        ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;

        CFE_ES_WriteToSysLog("%s: Invalid Application ID received, AppID = %lu\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(AppID));
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_ReloadApp(CFE_ES_AppId_t AppID, const char *AppFileName)
{
    int32               ReturnCode = CFE_SUCCESS;
    os_fstat_t          FileStatus;
    CFE_ES_AppRecord_t *AppRecPtr = CFE_ES_LocateAppRecordByID(AppID);

    if (AppRecPtr != NULL)
    {
        CFE_ES_LockSharedData(__func__, __LINE__);

        /*
        ** Check to see if the App is an external cFE App.
        */
        if (AppRecPtr->Type == CFE_ES_AppType_CORE)
        {
            CFE_ES_SysLogWrite_Unsync("%s: Cannot Reload a CORE Application: %s.\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
        else if (AppRecPtr->AppState != CFE_ES_AppState_RUNNING)
        {
            CFE_ES_SysLogWrite_Unsync("%s: Cannot Reload Application %s, It is not running.\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
        else
        {
            /*
            ** Check to see if the file exists
            */
            if (OS_stat(AppFileName, &FileStatus) == OS_SUCCESS)
            {
                CFE_ES_SysLogWrite_Unsync("%s: Reload Application %s Initiated. New filename = %s\n", __func__,
                                          CFE_ES_AppRecordGetName(AppRecPtr), AppFileName);
                strncpy(AppRecPtr->StartParams.BasicInfo.FileName, AppFileName,
                        sizeof(AppRecPtr->StartParams.BasicInfo.FileName) - 1);
                AppRecPtr->StartParams.BasicInfo.FileName[sizeof(AppRecPtr->StartParams.BasicInfo.FileName) - 1] = 0;
                AppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RELOAD;
            }
            else
            {
                CFE_ES_SysLogWrite_Unsync("%s: Cannot Reload Application %s, File %s does not exist.\n", __func__,
                                          CFE_ES_AppRecordGetName(AppRecPtr), AppFileName);
                ReturnCode = CFE_ES_FILE_IO_ERR;
            }
        }

        CFE_ES_UnlockSharedData(__func__, __LINE__);
    }
    else /* App ID is not valid */
    {
        ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;

        CFE_ES_WriteToSysLog("%s: Invalid Application ID received, AppID = %lu\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(AppID));
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_DeleteApp(CFE_ES_AppId_t AppID)
{
    int32               ReturnCode = CFE_SUCCESS;
    CFE_ES_AppRecord_t *AppRecPtr  = CFE_ES_LocateAppRecordByID(AppID);

    if (AppRecPtr != NULL)
    {
        CFE_ES_LockSharedData(__func__, __LINE__);

        /*
        ** Check to see if the App is an external cFE App.
        */
        if (AppRecPtr->Type == CFE_ES_AppType_CORE)
        {
            CFE_ES_SysLogWrite_Unsync("%s: Cannot Delete a CORE Application: %s.\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
        else if (AppRecPtr->AppState != CFE_ES_AppState_RUNNING)
        {
            CFE_ES_SysLogWrite_Unsync("%s: Cannot Delete Application %s, It is not running.\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
        else
        {
            CFE_ES_SysLogWrite_Unsync("%s: Delete Application %s Initiated\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));
            AppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_DELETE;
        }

        CFE_ES_UnlockSharedData(__func__, __LINE__);
    }
    else /* App ID is not valid */
    {
        ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;

        CFE_ES_WriteToSysLog("%s: Invalid Application ID received, AppID = %lu\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(AppID));
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_ExitApp(uint32 ExitStatus)
{
    int32               ReturnCode;
    CFE_ES_AppRecord_t *AppRecPtr;

    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
     * This should only be called with a valid ExitStatus, anything else is invalid
     * and indicates a bug in the caller.
     */

    if (ExitStatus == CFE_ES_RunStatus_UNDEFINED || ExitStatus >= CFE_ES_RunStatus_MAX)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Called with invalid status (%u).\n", __func__, (unsigned int)ExitStatus);

        /* revert to the ERROR status */
        ExitStatus = CFE_ES_RunStatus_APP_ERROR;
    }

    AppRecPtr = CFE_ES_GetAppRecordByContext();
    if (AppRecPtr != NULL)
    {
        /*
         * Set the status in the global table.
         *
         * The passed-in status should only be stored if there was no already-pending
         * request from a ground command or other source, such as an exception, etc.
         *
         * If a control request is already pending, it is assumed that this exit is
         * part of an orderly shutdown caused by that request, and therefore it
         * should not be overwritten here.
         */
        if (AppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_APP_RUN)
        {
            AppRecPtr->ControlReq.AppControlRequest = ExitStatus;
        }

        /*
        ** Check to see if the App is an external cFE App.
        */
        if (AppRecPtr->Type == CFE_ES_AppType_CORE)
        {
            /*
            ** A core app should only call this function with one of two ExitStatus codes.
            */
            if (ExitStatus == CFE_ES_RunStatus_CORE_APP_INIT_ERROR)
            {
                CFE_ES_SysLogWrite_Unsync("%s: CORE Application %s Had an Init Error.\n", __func__,
                                          CFE_ES_AppRecordGetName(AppRecPtr));

                /*
                ** Unlock the ES Shared data before calling ResetCFE
                */
                CFE_ES_UnlockSharedData(__func__, __LINE__);

                /*
                ** Do a Processor Reset the cFE
                */
                ReturnCode = CFE_ES_ResetCFE(CFE_PSP_RST_TYPE_PROCESSOR);

                /*
                ** The CFE_ES_ResetCFE function does not normally return,
                ** but it may return during unit testing. If it does,
                ** log the return code (even if it claims CFE_SUCCESS).
                */
                CFE_ES_WriteToSysLog("%s: CORE Application Init Error Processor Reset, RC = 0x%08X\n", __func__,
                                     (unsigned int)ReturnCode);

                return;
            }
            else if (ExitStatus == CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR)
            {
                CFE_ES_SysLogWrite_Unsync("%s: CORE Application %s Had a Runtime Error.\n", __func__,
                                          CFE_ES_AppRecordGetName(AppRecPtr));

                /*
                ** Unlock the ES Shared data before killing the main task
                */
                CFE_ES_UnlockSharedData(__func__, __LINE__);

                /*
                ** Exit this task
                */
                OS_TaskExit();

                /*
                ** Code will not return, except under unit test
                */
                return;
            }
            else
            {
                CFE_ES_SysLogWrite_Unsync("%s: Cannot Exit CORE Application %s\n", __func__,
                                          CFE_ES_AppRecordGetName(AppRecPtr));
            }
        }
        else /* It is an external App */
        {
            CFE_ES_SysLogWrite_Unsync("%s: Application %s called CFE_ES_ExitApp\n", __func__,
                                      CFE_ES_AppRecordGetName(AppRecPtr));

            AppRecPtr->AppState = CFE_ES_AppState_STOPPED;

            /*
            ** Unlock the ES Shared data before suspending the app
            */
            CFE_ES_UnlockSharedData(__func__, __LINE__);

            /*
            ** Suspend the Application until ES kills it.
            ** It might be better to have a way of suspending the app in the OS
            */
            while (1)
            {
                OS_TaskDelay(500);
            }
        }

    } /* end if ReturnCode == CFE_SUCCESS */

    CFE_ES_UnlockSharedData(__func__, __LINE__);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_RunLoop(uint32 *RunStatus)
{
    bool                ReturnCode;
    CFE_ES_AppRecord_t *AppRecPtr;

    /*
     * call CFE_ES_IncrementTaskCounter() so this is
     * recorded as task activity for outgoing telemetry.
     *
     * This will update the counter for whatever task context
     * is calling this API, which is expected to be the main
     * task of the app.  This can be done outside of any lock
     * because each task has its own counter which is only updated
     * by itself.
     */
    CFE_ES_IncrementTaskCounter();

    /*
     * This API should generally only be called with the status as CFE_ES_RunStatus_APP_RUN.
     * Anything else gets an immediate "false" return which should cause the caller to
     * break out of its main loop.  There is no need to take the lock or do any other
     * accounting in that case.
     *
     * Note that the RunStatus really doesn't add much value here, so this also allows
     * this function to be called with NULL, with the possibility of phasing this out
     * entirely.
     */
    if (RunStatus != NULL && *RunStatus != CFE_ES_RunStatus_APP_RUN)
    {
        return false;
    }

    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
    ** Get App Record
    */
    AppRecPtr = CFE_ES_GetAppRecordByContext();
    if (AppRecPtr != NULL)
    {
        /*
         ** App state must be RUNNING (no-op if already set to running)
         */
        if (AppRecPtr->AppState < CFE_ES_AppState_RUNNING)
        {
            AppRecPtr->AppState = CFE_ES_AppState_RUNNING;
        }

        /*
         * Check if the control request is also set to "RUN"
         * Anything else should also return false, so the loop will exit.
         */
        if (AppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_APP_RUN)
        {
            ReturnCode = true;
        }
        else
        {
            /*
             * Just in case, also output the status, just in case the app looks at this.
             */
            if (RunStatus != NULL)
            {
                *RunStatus = AppRecPtr->ControlReq.AppControlRequest;
            }
            ReturnCode = false;
        }
    }
    else
    {
        /*
         * Cannot do anything without the AppID
         */
        CFE_ES_SysLogWrite_Unsync("%s: Error getting AppID for the caller\n", __func__);
        ReturnCode = false;

    } /* end if Status == CFE_SUCCESS */

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_WaitForSystemState(uint32 MinSystemState, uint32 TimeOutMilliseconds)
{
    int32               Status = CFE_SUCCESS;
    CFE_ES_AppRecord_t *AppRecPtr;
    uint32              RequiredAppState;
    uint32              WaitTime;
    uint32              WaitRemaining;

    /*
     * Calling app is assumed to have completed its own initialization up to the point
     * it is waiting for.
     *
     * Determine the implicit app state based on the system state it is indicating
     */
    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
    ** Get App Record
    */
    AppRecPtr = CFE_ES_GetAppRecordByContext();
    if (AppRecPtr != NULL)
    {
        RequiredAppState = CFE_ES_AppState_EARLY_INIT;
        /*
         * If a core app waits for anything above "CORE_READY" then it is assumed to be RUNNING
         *
         * External apps have additional finer-grained sync:
         *  - SYSTEM_STATE_APPS_INIT requires that all apps are at least up to LATE_INIT
         *  - SYSTEM_STATE_OPERATIONAL requires that all apps are RUNNING
         *  - SYSTEM_STATE_SHUTDOWN requires that all apps are STOPPED (in concept anyway)
         */
        if (AppRecPtr->Type == CFE_ES_AppType_CORE)
        {
            if (MinSystemState >= CFE_ES_SystemState_CORE_READY)
            {
                RequiredAppState = CFE_ES_AppState_RUNNING;
            }
        }
        else if (MinSystemState >= CFE_ES_SystemState_SHUTDOWN)
        {
            RequiredAppState = CFE_ES_AppState_STOPPED;
        }
        else if (MinSystemState >= CFE_ES_SystemState_OPERATIONAL)
        {
            RequiredAppState = CFE_ES_AppState_RUNNING;
        }
        else if (MinSystemState >= CFE_ES_SystemState_APPS_INIT)
        {
            RequiredAppState = CFE_ES_AppState_LATE_INIT;
        }

        /*
         * NOTE -- a call to "CFE_ES_WaitForSystemState()" implies that the calling app MUST also
         * be in the requisite state.  This is hooked into here to avoid needing to update all existing
         * apps to add an explicit state change call, but it makes sense because if this was not done an app could
         * be waiting for itself (which will always time out).
         */
        if (AppRecPtr->AppState < RequiredAppState)
        {
            AppRecPtr->AppState = RequiredAppState;
        }
    }
    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
     * Do the actual delay loop.
     *
     * This is only dependent on the main (startup) task updating the global variable
     * to be at least the state requested.
     */
    WaitRemaining = TimeOutMilliseconds;
    while (CFE_ES_Global.SystemState < MinSystemState)
    {
        /* TBD: Very Crude timing here, but not sure if it matters,
         * as this is only done during startup, not real work */
        if (WaitRemaining > CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC)
        {
            WaitTime = CFE_PLATFORM_ES_STARTUP_SYNC_POLL_MSEC;
        }
        else if (WaitRemaining > 0)
        {
            WaitTime = WaitRemaining;
        }
        else
        {
            Status = CFE_ES_OPERATION_TIMED_OUT;
            break;
        }

        OS_TaskDelay(WaitTime);
        WaitRemaining -= WaitTime;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds)
{
    CFE_ES_WaitForSystemState(CFE_ES_SystemState_OPERATIONAL, TimeOutMilliseconds);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetAppIDByName(CFE_ES_AppId_t *AppIdPtr, const char *AppName)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    int32               Result;

    if (AppName == NULL || AppIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    CFE_ES_LockSharedData(__func__, __LINE__);

    AppRecPtr = CFE_ES_LocateAppRecordByName(AppName);
    if (AppRecPtr == NULL)
    {
        /*
         * ensure the output value is set to a safe value,
         * in case the caller does not check the return code.
         */
        Result    = CFE_ES_ERR_NAME_NOT_FOUND;
        *AppIdPtr = CFE_ES_APPID_UNDEFINED;
    }
    else
    {
        Result    = CFE_SUCCESS;
        *AppIdPtr = CFE_ES_AppRecordGetID(AppRecPtr);
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetLibIDByName(CFE_ES_LibId_t *LibIdPtr, const char *LibName)
{
    CFE_ES_LibRecord_t *LibRecPtr;
    int32               Result;

    if (LibName == NULL || LibIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    CFE_ES_LockSharedData(__func__, __LINE__);

    LibRecPtr = CFE_ES_LocateLibRecordByName(LibName);
    if (LibRecPtr == NULL)
    {
        /*
         * ensure the output value is set to a safe value,
         * in case the caller does not check the return code.
         */
        Result    = CFE_ES_ERR_NAME_NOT_FOUND;
        *LibIdPtr = CFE_ES_LIBID_UNDEFINED;
    }
    else
    {
        Result    = CFE_SUCCESS;
        *LibIdPtr = CFE_ES_LibRecordGetID(LibRecPtr);
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetTaskIDByName(CFE_ES_TaskId_t *TaskIdPtr, const char *TaskName)
{
    osal_id_t    OsalId = OS_OBJECT_ID_UNDEFINED;
    int32        OsStatus;
    CFE_Status_t Result;

    if (TaskName == NULL || TaskIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /* For tasks IDs, defer to OSAL for name lookup */
    OsStatus = OS_TaskGetIdByName(&OsalId, TaskName);
    if (OsStatus == OS_SUCCESS)
    {
        Result     = CFE_SUCCESS;
        *TaskIdPtr = CFE_ES_TaskId_FromOSAL(OsalId);
    }
    else
    {
        Result     = CFE_ES_ERR_NAME_NOT_FOUND;
        *TaskIdPtr = CFE_ES_TASKID_UNDEFINED;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetAppID(CFE_ES_AppId_t *AppIdPtr)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    int32               Result;

    if (AppIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    CFE_ES_LockSharedData(__func__, __LINE__);

    AppRecPtr = CFE_ES_GetAppRecordByContext();

    if (AppRecPtr != NULL)
    {
        *AppIdPtr = CFE_ES_AppRecordGetID(AppRecPtr);
        Result    = CFE_SUCCESS;
    }
    else
    {
        *AppIdPtr = CFE_ES_APPID_UNDEFINED;
        Result    = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetTaskID(CFE_ES_TaskId_t *TaskIdPtr)
{
    int32                Result;
    CFE_ES_TaskRecord_t *TaskRecPtr;

    if (TaskIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    CFE_ES_LockSharedData(__func__, __LINE__);
    TaskRecPtr = CFE_ES_GetTaskRecordByContext();
    if (TaskRecPtr == NULL)
    {
        *TaskIdPtr = CFE_ES_TASKID_UNDEFINED;
        Result     = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }
    else
    {
        *TaskIdPtr = CFE_ES_TaskRecordGetID(TaskRecPtr);
        Result     = CFE_SUCCESS;
    }
    CFE_ES_UnlockSharedData(__func__, __LINE__);
    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetAppName(char *AppName, CFE_ES_AppId_t AppId, size_t BufferLength)
{
    int32               Result;
    CFE_ES_AppRecord_t *AppRecPtr;

    if (BufferLength == 0 || AppName == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /*
    ** Get App Record
    */
    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);

    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
     * confirm that the app record is a match,
     * which must be done while locked.
     */
    if (CFE_ES_AppRecordIsMatch(AppRecPtr, AppId))
    {
        strncpy(AppName, CFE_ES_AppRecordGetName(AppRecPtr), BufferLength - 1);
        AppName[BufferLength - 1] = '\0';
        Result                    = CFE_SUCCESS;
    }
    else
    {
        AppName[0] = 0;
        Result     = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetLibName(char *LibName, CFE_ES_LibId_t LibId, size_t BufferLength)
{
    int32               Result;
    CFE_ES_LibRecord_t *LibRecPtr;

    if (BufferLength == 0 || LibName == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /*
    ** Get Lib Record
    */
    LibRecPtr = CFE_ES_LocateLibRecordByID(LibId);

    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
     * confirm that the Lib record is a match,
     * which must be done while locked.
     */
    if (CFE_ES_LibRecordIsMatch(LibRecPtr, LibId))
    {
        strncpy(LibName, CFE_ES_LibRecordGetName(LibRecPtr), BufferLength - 1);
        LibName[BufferLength - 1] = '\0';
        Result                    = CFE_SUCCESS;
    }
    else
    {
        LibName[0] = 0;
        Result     = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetTaskName(char *TaskName, CFE_ES_TaskId_t TaskId, size_t BufferLength)
{
    int32     OsStatus;
    osal_id_t OsalId;

    if (BufferLength == 0 || TaskName == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    if (!CFE_RESOURCEID_TEST_DEFINED(TaskId))
    {
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    /*
     * Query OSAL to get the task name
     */
    OsalId   = CFE_ES_TaskId_ToOSAL(TaskId);
    OsStatus = OS_GetResourceName(OsalId, TaskName, BufferLength);

    if (OsStatus == OS_ERR_INVALID_ID)
    {
        /* Supplied ID is not a CFE task */
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }
    if (OsStatus == OS_ERR_NAME_TOO_LONG)
    {
        /* Name is too long to fit in supplied buffer */
        return CFE_ES_BAD_ARGUMENT;
    }
    if (OsStatus != OS_SUCCESS)
    {
        /* Some other uncaught error */
        CFE_ES_WriteToSysLog("%s(): Unexpected error from OS_GetResourceName(): %ld", __func__, (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, CFE_ES_AppId_t AppId)
{
    CFE_ES_AppRecord_t * AppRecPtr;
    CFE_ES_TaskRecord_t *TaskRecPtr;
    int32                Status;
    osal_id_t            ModuleId;
    uint32               i;

    if (AppInfo == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Invalid Parameter ( Null Pointer )\n", __func__);
        return CFE_ES_BAD_ARGUMENT;
    }

    memset(AppInfo, 0, sizeof(*AppInfo));
    ModuleId = OS_OBJECT_ID_UNDEFINED;

    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);

    CFE_ES_LockSharedData(__func__, __LINE__);

    if (!CFE_ES_AppRecordIsMatch(AppRecPtr, AppId))
    {
        /*
         * Log a message if called with an invalid ID.
         */
        CFE_ES_WriteToSysLog("%s: App ID not active: %lu\n", __func__, CFE_RESOURCEID_TO_ULONG(AppId));

        Status = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }
    else
    {
        AppInfo->ResourceId = CFE_RESOURCEID_UNWRAP(AppId); /* make into a generic resource ID */
        AppInfo->Type       = AppRecPtr->Type;

        strncpy(AppInfo->Name, CFE_ES_AppRecordGetName(AppRecPtr), sizeof(AppInfo->Name) - 1);

        CFE_ES_CopyModuleBasicInfo(&AppRecPtr->StartParams.BasicInfo, AppInfo);
        CFE_ES_CopyModuleStatusInfo(&AppRecPtr->LoadStatus, AppInfo);

        AppInfo->ExceptionAction = AppRecPtr->StartParams.ExceptionAction;
        AppInfo->MainTaskId      = AppRecPtr->MainTaskId;

        ModuleId = AppRecPtr->LoadStatus.ModuleId;

        /*
        ** Calculate the number of child tasks
        */
        AppInfo->NumOfChildTasks = 0;
        TaskRecPtr               = CFE_ES_Global.TaskTable;
        for (i = 0; i < OS_MAX_TASKS; i++)
        {
            if (CFE_ES_TaskRecordIsUsed(TaskRecPtr) && CFE_RESOURCEID_TEST_EQUAL(TaskRecPtr->AppId, AppId))
            {
                if (CFE_RESOURCEID_TEST_EQUAL(CFE_ES_TaskRecordGetID(TaskRecPtr), AppInfo->MainTaskId))
                {
                    /* This is the main task - capture its name and execution count */
                    AppInfo->ExecutionCounter = TaskRecPtr->ExecutionCounter;
                    strncpy(AppInfo->MainTaskName, TaskRecPtr->TaskName, sizeof(AppInfo->MainTaskName) - 1);
                    AppInfo->MainTaskName[sizeof(AppInfo->MainTaskName) - 1] = '\0';

                    AppInfo->StackSize = CFE_ES_MEMOFFSET_C(TaskRecPtr->StartParams.StackSize);
                    AppInfo->Priority  = TaskRecPtr->StartParams.Priority;
                }
                else
                {
                    /* This is a child task, no extra info, just increment count */
                    ++AppInfo->NumOfChildTasks;
                }
            }
            ++TaskRecPtr;
        }

        Status = CFE_SUCCESS;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
    ** Get the address information from the OSAL
    */
    if (Status == CFE_SUCCESS)
    {
        CFE_ES_CopyModuleAddressInfo(ModuleId, AppInfo);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GetLibInfo(CFE_ES_AppInfo_t *LibInfo, CFE_ES_LibId_t LibId)
{
    int32               Status;
    CFE_ES_LibRecord_t *LibRecPtr;
    osal_id_t           ModuleId;

    if (LibInfo == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Invalid Parameter ( Null Pointer )\n", __func__);
        return CFE_ES_BAD_ARGUMENT;
    }

    memset(LibInfo, 0, sizeof(*LibInfo));
    ModuleId = OS_OBJECT_ID_UNDEFINED;

    LibRecPtr = CFE_ES_LocateLibRecordByID(LibId);

    CFE_ES_LockSharedData(__func__, __LINE__);

    if (!CFE_ES_LibRecordIsMatch(LibRecPtr, LibId))
    {
        /*
         * Log a message if called with an invalid ID.
         */
        CFE_ES_SysLogWrite_Unsync("%s: Lib ID not active: %lu\n", __func__, CFE_RESOURCEID_TO_ULONG(LibId));

        Status = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }
    else
    {
        LibInfo->ResourceId = CFE_RESOURCEID_UNWRAP(LibId); /* make into generic ID */
        LibInfo->Type       = CFE_ES_AppType_LIBRARY;

        strncpy(LibInfo->Name, CFE_ES_LibRecordGetName(LibRecPtr), sizeof(LibInfo->Name) - 1);

        CFE_ES_CopyModuleBasicInfo(&LibRecPtr->LoadParams, LibInfo);
        CFE_ES_CopyModuleStatusInfo(&LibRecPtr->LoadStatus, LibInfo);

        ModuleId = LibRecPtr->LoadStatus.ModuleId;

        Status = CFE_SUCCESS;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
     ** Get the address information from the OSAL
     */
    if (Status == CFE_SUCCESS)
    {
        CFE_ES_CopyModuleAddressInfo(ModuleId, LibInfo);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_GetModuleInfo(CFE_ES_AppInfo_t *ModuleInfo, CFE_ResourceId_t ResourceId)
{
    int32 Status;

    /* Note - ModuleInfo NULL pointer check is performed by CFE_ES_GetAppInfo or CFE_ES_GetLibInfo */
    switch (CFE_ResourceId_GetBase(ResourceId))
    {
        case CFE_ES_APPID_BASE:
            Status = CFE_ES_GetAppInfo(ModuleInfo, CFE_ES_APPID_C(ResourceId));
            break;
        case CFE_ES_LIBID_BASE:
            Status = CFE_ES_GetLibInfo(ModuleInfo, CFE_ES_LIBID_C(ResourceId));
            break;
        default:
            /*
             * Log a message if called with an invalid ID.
             */
            CFE_ES_WriteToSysLog("%s: Resource ID not valid: %lu\n", __func__, CFE_ResourceId_ToInteger(ResourceId));
            Status = CFE_ES_ERR_RESOURCEID_NOT_VALID;
            break;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, CFE_ES_TaskId_t TaskId)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    CFE_ES_AppRecord_t * AppRecPtr;
    int32                Status;

    if (TaskInfo == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Invalid Parameter ( Null Pointer )\n", __func__);
        return CFE_ES_BAD_ARGUMENT;
    }

    memset(TaskInfo, 0, sizeof(*TaskInfo));

    TaskRecPtr = CFE_ES_LocateTaskRecordByID(TaskId);

    CFE_ES_LockSharedData(__func__, __LINE__);

    if (!CFE_ES_TaskRecordIsMatch(TaskRecPtr, TaskId))
    {
        /* task ID is bad */
        Status = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        CFE_ES_SysLogWrite_Unsync("%s: Task ID Not Active: %lu\n", __func__, CFE_RESOURCEID_TO_ULONG(TaskId));
    }
    else
    {
        /*
        ** Get the Application ID and Task Name
        */
        TaskInfo->AppId = TaskRecPtr->AppId;
        strncpy(TaskInfo->TaskName, CFE_ES_TaskRecordGetName(TaskRecPtr), sizeof(TaskInfo->TaskName) - 1);
        TaskInfo->TaskName[sizeof(TaskInfo->TaskName) - 1] = '\0';

        /*
        ** Store away the Task ID ( for the QueryAllTasks Cmd )
        */
        TaskInfo->TaskId = CFE_ES_TaskRecordGetID(TaskRecPtr);

        /*
        ** Get the other stats for the task
        */
        TaskInfo->ExecutionCounter = TaskRecPtr->ExecutionCounter;
        TaskInfo->StackSize        = CFE_ES_MEMOFFSET_C(TaskRecPtr->StartParams.StackSize);
        TaskInfo->Priority         = TaskRecPtr->StartParams.Priority;

        /*
        ** Get the Application Details
        */
        AppRecPtr = CFE_ES_LocateAppRecordByID(TaskRecPtr->AppId);
        if (CFE_ES_AppRecordIsMatch(AppRecPtr, TaskRecPtr->AppId))
        {
            strncpy(TaskInfo->AppName, CFE_ES_AppRecordGetName(AppRecPtr), sizeof(TaskInfo->AppName) - 1);
            TaskInfo->AppName[sizeof(TaskInfo->AppName) - 1] = '\0';
            Status                                           = CFE_SUCCESS;
        }
        else
        {
            /* task ID was OK but parent app ID is bad */
            Status = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_CreateChildTask(CFE_ES_TaskId_t *TaskIdPtr, const char *TaskName,
                                    CFE_ES_ChildTaskMainFuncPtr_t FunctionPtr, CFE_ES_StackPointer_t StackPtr,
                                    size_t StackSize, CFE_ES_TaskPriority_Atom_t Priority, uint32 Flags)
{
    int32                    ReturnCode;
    CFE_ES_AppRecord_t *     AppRecPtr;
    CFE_ES_AppId_t           ParentAppId;
    CFE_ES_TaskId_t          SelfTaskId;
    CFE_ES_TaskStartParams_t Params;

    ParentAppId = CFE_ES_APPID_UNDEFINED;

    memset(&Params, 0, sizeof(Params));
    Params.Priority  = Priority;
    Params.StackSize = StackSize;
    Params.StackPtr  = StackPtr;

    /*
    ** Validate some of the arguments
    */
    if (TaskIdPtr == NULL)
    {
        if (TaskName == NULL)
        {
            CFE_ES_WriteToSysLog("%s: Task Id and Name Pointer Parameters are NULL.\n", __func__);
            ReturnCode = CFE_ES_BAD_ARGUMENT;
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Task Id Pointer Parameter is NULL for Task '%s'.\n", __func__, TaskName);
            ReturnCode = CFE_ES_BAD_ARGUMENT;
        }
    }
    else if (TaskName == NULL)
    {
        CFE_ES_WriteToSysLog("%s: TaskName Parameter is NULL\n", __func__);
        ReturnCode = CFE_ES_BAD_ARGUMENT;
    }
    else if (FunctionPtr == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Function Pointer Parameter is NULL for Task '%s'\n", __func__, TaskName);
        ReturnCode = CFE_ES_BAD_ARGUMENT;
    }
    else
    {
        /*
        ** First, Make sure the Calling Task is a cFE Main task.
        ** TaskID must be the same as the Parent Task ID.
        */
        SelfTaskId = CFE_ES_TaskId_FromOSAL(OS_TaskGetId());

        CFE_ES_LockSharedData(__func__, __LINE__);

        /*
        ** Get the App Record of the calling Application
        */
        AppRecPtr = CFE_ES_GetAppRecordByContext();
        if (AppRecPtr == NULL)
        {
            CFE_ES_SysLogWrite_Unsync("%s: Invalid calling context when creating Task '%s'\n", __func__, TaskName);
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }
        else if (!CFE_RESOURCEID_TEST_EQUAL(SelfTaskId, AppRecPtr->MainTaskId))
        {
            CFE_ES_SysLogWrite_Unsync("%s: Error: Cannot call from a Child Task (for Task '%s').\n", __func__,
                                      TaskName);
            ReturnCode = CFE_ES_ERR_CHILD_TASK_CREATE;
        }
        else
        {
            ParentAppId = CFE_ES_AppRecordGetID(AppRecPtr);
            ReturnCode  = CFE_SUCCESS;
        } /* end If AppID is valid */

        CFE_ES_UnlockSharedData(__func__, __LINE__);

    } /* end if parameter checking */

    /*
    ** Step 2: Create the new task if the parameter validation succeeded
    */
    if (ReturnCode == CFE_SUCCESS)
    {
        ReturnCode = CFE_ES_StartAppTask(TaskIdPtr, TaskName, FunctionPtr, &Params, ParentAppId);
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_IncrementTaskCounter(void)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    CFE_ES_TaskId_t      TaskID;

    /*
     * Note this locates a task record but intentionally does _not_
     * lock the global data structure.  Locking is avoided for
     * efficiency reasons.
     *
     * As tasks can only increment their own counters, there is no risk
     * of concurrent access to the same counter.
     *
     * Because the global data is not locked, only minimal validation
     * is performed.
     */
    TaskID     = CFE_ES_TaskId_FromOSAL(OS_TaskGetId());
    TaskRecPtr = CFE_ES_LocateTaskRecordByID(TaskID);
    if (TaskRecPtr != NULL)
    {
        TaskRecPtr->ExecutionCounter++;
    }
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_DeleteChildTask(CFE_ES_TaskId_t TaskId)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    CFE_ES_AppRecord_t * AppRecPtr;
    uint32               i;
    bool                 TaskIsMain;
    int32                ReturnCode = CFE_SUCCESS;
    int32                OsStatus;
    osal_id_t            OsalId;

    /*
    ** Make sure the task ID is within range
    */
    TaskRecPtr = CFE_ES_LocateTaskRecordByID(TaskId);
    if (TaskRecPtr != NULL)
    {
        CFE_ES_LockSharedData(__func__, __LINE__);

        /*
        ** Make sure the task is active/valid
        */
        if (CFE_ES_TaskRecordIsMatch(TaskRecPtr, TaskId))
        {
            /*
            ** Search for this task ID in the ES App Table to make sure
            ** it is not a cFE App Main Task
            */
            TaskIsMain = false;
            AppRecPtr  = CFE_ES_Global.AppTable;
            for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
            {
                if (CFE_ES_AppRecordIsUsed(AppRecPtr))
                {
                    if (CFE_RESOURCEID_TEST_EQUAL(AppRecPtr->MainTaskId, TaskId))
                    {
                        /*
                        ** Error, the task Id is an App Main Task ID
                        */
                        TaskIsMain = true;
                        break;
                    }
                }
                ++AppRecPtr;
            }

            if (TaskIsMain == false)
            {
                /*
                ** Can delete the Task
                */
                OsalId   = CFE_ES_TaskId_ToOSAL(TaskId);
                OsStatus = OS_TaskDelete(OsalId);
                if (OsStatus == OS_SUCCESS)
                {
                    /*
                    ** Invalidate the task table entry
                    */
                    CFE_ES_TaskRecordSetFree(TaskRecPtr);
                    CFE_ES_Global.RegisteredTasks--;

                    /*
                    ** Report the task delete
                    */
                    CFE_ES_SysLogWrite_Unsync("%s: Task %lu Deleted\n", __func__, CFE_RESOURCEID_TO_ULONG(TaskId));
                    ReturnCode = CFE_SUCCESS;
                }
                else
                {
                    CFE_ES_SysLogWrite_Unsync("%s: Error Calling OS_TaskDelete: Task %lu, RC = %ld\n", __func__,
                                              CFE_RESOURCEID_TO_ULONG(TaskId), (long)OsStatus);
                    ReturnCode = CFE_ES_ERR_CHILD_TASK_DELETE;
                }
            }
            else
            {
                /*
                ** Error: The task is a cFE Application Main task
                */
                CFE_ES_SysLogWrite_Unsync("%s: Error: Task %lu is a cFE Main Task.\n", __func__,
                                          CFE_RESOURCEID_TO_ULONG(TaskId));
                ReturnCode = CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK;
            } /* end if TaskMain == false */
        }
        else
        {
            /*
            ** Task ID is not in use, so it is invalid
            */
            CFE_ES_SysLogWrite_Unsync("%s: Error: Task ID is not active: %lu\n", __func__,
                                      CFE_RESOURCEID_TO_ULONG(TaskId));
            ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
        }

        CFE_ES_UnlockSharedData(__func__, __LINE__);
    }
    else
    {
        /*
        ** Task ID is invalid ( too large )
        */
        CFE_ES_WriteToSysLog("%s: Error: Invalid Task ID: %lu\n", __func__, CFE_RESOURCEID_TO_ULONG(TaskId));
        ReturnCode = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }
    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_ExitChildTask(void)
{
    CFE_ES_AppRecord_t * AppRecPtr;
    CFE_ES_TaskRecord_t *TaskRecPtr;

    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
    ** Check to see if this is being called from a cFE Application's
    ** main task.
    */
    TaskRecPtr = CFE_ES_GetTaskRecordByContext();
    if (TaskRecPtr != NULL)
    {
        AppRecPtr = CFE_ES_LocateAppRecordByID(TaskRecPtr->AppId);

        if (CFE_ES_AppRecordIsMatch(AppRecPtr, TaskRecPtr->AppId) &&
            !CFE_ES_TaskRecordIsMatch(TaskRecPtr, AppRecPtr->MainTaskId))
        {
            /*
            ** Invalidate the task table entry
            */
            CFE_ES_TaskRecordSetFree(TaskRecPtr);
            CFE_ES_Global.RegisteredTasks--;

            CFE_ES_UnlockSharedData(__func__, __LINE__);

            /*
            ** Call the OS AL routine
            */
            OS_TaskExit();
            /*
            ** Does not return from OS_TaskExit, except under unit test
            */
            return;
        }
        else
        {
            CFE_ES_SysLogWrite_Unsync("%s: Error: Cannot Call from a cFE App Main Task. ID = %lu\n", __func__,
                                      CFE_RESOURCEID_TO_ULONG(CFE_ES_TaskRecordGetID(TaskRecPtr)));
        }
    }
    else
    {
        CFE_ES_SysLogWrite_Unsync("%s: Called from invalid task context\n", __func__);
    } /* end if GetAppId */

    CFE_ES_UnlockSharedData(__func__, __LINE__);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...)
{
    char    TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    int32   ReturnCode;
    va_list ArgPtr;

    if (SpecStringPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    va_start(ArgPtr, SpecStringPtr);
    CFE_ES_SysLog_vsnprintf(TmpString, sizeof(TmpString), SpecStringPtr, ArgPtr);
    va_end(ArgPtr);

    /*
     * Append to the syslog buffer, which must be done while locked.
     * Only one thread can actively write into the buffer at time.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);
    ReturnCode = CFE_ES_SysLogAppend_Unsync(TmpString);
    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /* Output the entry to the console */
    OS_printf("%s", TmpString);

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_ES_CalculateCRC(const void *DataPtr, size_t DataLength, uint32 InputCRC, CFE_ES_CrcType_Enum_t TypeCRC)
{
    const CFE_ES_ComputeCRC_Params_t *CrcParams;
    uint32                            CrcResult;

    /* Protect against bad pointer input - historical behavior is to just return the input */
    if (DataPtr == NULL || DataLength == 0)
    {
        CrcResult = InputCRC;
    }
    else
    {
        /* This always returns a valid object, even if it is not implemented */
        CrcParams = CFE_ES_ComputeCRC_GetParams(TypeCRC);
        CrcResult = CrcParams->Algorithm(DataPtr, DataLength, InputCRC);
    }

    return CrcResult;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *CDSHandlePtr, size_t BlockSize, const char *Name)
{
    int32          Status;
    size_t         NameLen;
    CFE_ES_AppId_t ThisAppId;

    char AppName[OS_MAX_API_NAME]                      = {"UNKNOWN"};
    char CDSName[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN] = {""};

    /* Check to make sure calling application is legit */
    Status = CFE_ES_GetAppID(&ThisAppId);

    if (CDSHandlePtr == NULL || Name == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Failed invalid arguments\n", __func__);
        return CFE_ES_BAD_ARGUMENT;
    }

    /* Initialize output to safe value, in case this fails */
    *CDSHandlePtr = CFE_ES_CDS_BAD_HANDLE;

    if (Status != CFE_SUCCESS) /* Application ID was invalid */
    {
        CFE_ES_WriteToSysLog("%s: Bad AppId context\n", __func__);
    }
    else if (!CFE_ES_Global.CDSIsAvailable)
    {
        CFE_ES_WriteToSysLog("%s: CDS not available\n", __func__);
        Status = CFE_ES_NOT_IMPLEMENTED;
    }
    else
    {
        /* Make sure specified CDS name is not too long or too short */
        NameLen = strlen(Name);
        if ((NameLen > CFE_MISSION_ES_CDS_MAX_NAME_LENGTH) || (NameLen == 0))
        {
            Status = CFE_ES_CDS_INVALID_NAME;

            /* Perform a buffer overrun safe copy of name for debug log message */

            strncpy(CDSName, Name, sizeof(CDSName) - 1);
            CDSName[sizeof(CDSName) - 1] = '\0';
            CFE_ES_WriteToSysLog("%s: CDS Name (%s) is too long\n", __func__, CDSName);
        }
        else
        {
            /* Modify specified name to be processor specific name */
            /* of the form "AppName.Name"                          */
            CFE_ES_FormCDSName(CDSName, Name, ThisAppId);

            /* Create CDS and designate it as NOT being a Critical Table */
            Status = CFE_ES_RegisterCDSEx(CDSHandlePtr, BlockSize, CDSName, false);

            /* If size is unacceptable, log it */
            if (Status == CFE_ES_CDS_INVALID_SIZE)
            {
                CFE_ES_WriteToSysLog("%s: CDS %s has invalid size (%lu)\n", __func__, Name, (unsigned long)BlockSize);
            }
        }
    }

    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

        CFE_EVS_SendEventWithAppID(CFE_ES_CDS_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR, ThisAppId,
                                   "%s Failed to Register CDS '%s', Status=0x%08X", AppName, Name,
                                   (unsigned int)Status);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetCDSBlockIDByName(CFE_ES_CDSHandle_t *BlockIdPtr, const char *BlockName)
{
    CFE_Status_t         Status;
    CFE_ES_CDS_RegRec_t *RegRecPtr;

    if (BlockName == NULL || BlockIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }
    if (!CFE_ES_Global.CDSIsAvailable)
    {
        return CFE_ES_NOT_IMPLEMENTED;
    }

    CFE_ES_LockCDS();

    RegRecPtr = CFE_ES_LocateCDSBlockRecordByName(BlockName);

    if (RegRecPtr != NULL)
    {
        *BlockIdPtr = CFE_ES_CDSBlockRecordGetID(RegRecPtr);
        Status      = CFE_SUCCESS;
    }
    else
    {
        *BlockIdPtr = CFE_ES_CDS_BAD_HANDLE;
        Status      = CFE_ES_ERR_NAME_NOT_FOUND;
    }

    CFE_ES_UnlockCDS();

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetCDSBlockName(char *BlockName, CFE_ES_CDSHandle_t BlockId, size_t BufferLength)
{
    CFE_Status_t         Status;
    CFE_ES_CDS_RegRec_t *RegRecPtr;

    if (BufferLength == 0 || BlockName == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }
    if (!CFE_ES_Global.CDSIsAvailable)
    {
        return CFE_ES_NOT_IMPLEMENTED;
    }

    RegRecPtr = CFE_ES_LocateCDSBlockRecordByID(BlockId);

    CFE_ES_LockCDS();

    if (CFE_ES_CDSBlockRecordIsMatch(RegRecPtr, BlockId))
    {
        strncpy(BlockName, RegRecPtr->Name, BufferLength - 1);
        BlockName[BufferLength - 1] = 0;
        Status                      = CFE_SUCCESS;
    }
    else
    {
        BlockName[0] = 0;
        Status       = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockCDS();

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, const void *DataToCopy)
{
    if (DataToCopy == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    return CFE_ES_CDSBlockWrite(Handle, DataToCopy);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle)
{
    if (RestoreToMemory == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    return CFE_ES_CDSBlockRead(RestoreToMemory, Handle);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_RegisterGenCounter(CFE_ES_CounterId_t *CounterIdPtr, const char *CounterName)
{
    CFE_ES_GenCounterRecord_t *CountRecPtr;
    CFE_ResourceId_t           PendingResourceId;
    int32                      Status;

    if (CounterName == NULL || CounterIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    if (strlen(CounterName) >= sizeof(CountRecPtr->CounterName))
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
     * Check for an existing entry with the same name.
     */
    CountRecPtr = CFE_ES_LocateCounterRecordByName(CounterName);
    if (CountRecPtr != NULL)
    {
        CFE_ES_SysLogWrite_Unsync("%s: Duplicate Counter name '%s'\n", __func__, CounterName);
        Status            = CFE_ES_ERR_DUPLICATE_NAME;
        PendingResourceId = CFE_RESOURCEID_UNDEFINED;
    }
    else
    {
        /* scan for a free slot */
        PendingResourceId = CFE_ResourceId_FindNext(CFE_ES_Global.LastCounterId, CFE_PLATFORM_ES_MAX_GEN_COUNTERS,
                                                    CFE_ES_CheckCounterIdSlotUsed);
        CountRecPtr       = CFE_ES_LocateCounterRecordByID(CFE_ES_COUNTERID_C(PendingResourceId));

        if (CountRecPtr == NULL)
        {
            CFE_ES_SysLogWrite_Unsync("%s: No free Counter slots available\n", __func__);
            Status = CFE_ES_NO_RESOURCE_IDS_AVAILABLE;
        }
        else
        {
            strncpy(CountRecPtr->CounterName, CounterName, sizeof(CountRecPtr->CounterName) - 1);
            CountRecPtr->CounterName[sizeof(CountRecPtr->CounterName) - 1] = '\0';
            CountRecPtr->Counter                                           = 0;
            CFE_ES_CounterRecordSetUsed(CountRecPtr, PendingResourceId);
            CFE_ES_Global.LastCounterId = PendingResourceId;
            Status                      = CFE_SUCCESS;
        }
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    *CounterIdPtr = CFE_ES_COUNTERID_C(PendingResourceId);
    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_DeleteGenCounter(CFE_ES_CounterId_t CounterId)
{
    CFE_ES_GenCounterRecord_t *CountRecPtr;
    int32                      Status = CFE_ES_BAD_ARGUMENT;

    CountRecPtr = CFE_ES_LocateCounterRecordByID(CounterId);
    if (CountRecPtr != NULL)
    {
        CFE_ES_LockSharedData(__func__, __LINE__);
        if (CFE_ES_CounterRecordIsMatch(CountRecPtr, CounterId))
        {
            CountRecPtr->Counter = 0;
            CFE_ES_CounterRecordSetFree(CountRecPtr);
            Status = CFE_SUCCESS;
        }
        CFE_ES_UnlockSharedData(__func__, __LINE__);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_IncrementGenCounter(CFE_ES_CounterId_t CounterId)
{
    int32                      Status = CFE_ES_BAD_ARGUMENT;
    CFE_ES_GenCounterRecord_t *CountRecPtr;

    CountRecPtr = CFE_ES_LocateCounterRecordByID(CounterId);
    if (CFE_ES_CounterRecordIsMatch(CountRecPtr, CounterId))
    {
        ++CountRecPtr->Counter;
        Status = CFE_SUCCESS;
    }
    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_SetGenCount(CFE_ES_CounterId_t CounterId, uint32 Count)
{
    int32                      Status = CFE_ES_BAD_ARGUMENT;
    CFE_ES_GenCounterRecord_t *CountRecPtr;

    CountRecPtr = CFE_ES_LocateCounterRecordByID(CounterId);
    if (CFE_ES_CounterRecordIsMatch(CountRecPtr, CounterId))
    {
        CountRecPtr->Counter = Count;
        Status               = CFE_SUCCESS;
    }
    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetGenCount(CFE_ES_CounterId_t CounterId, uint32 *Count)
{
    int32                      Status = CFE_ES_BAD_ARGUMENT;
    CFE_ES_GenCounterRecord_t *CountRecPtr;

    CountRecPtr = CFE_ES_LocateCounterRecordByID(CounterId);
    if (CFE_ES_CounterRecordIsMatch(CountRecPtr, CounterId) && Count != NULL)
    {
        *Count = CountRecPtr->Counter;
        Status = CFE_SUCCESS;
    }
    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetGenCounterIDByName(CFE_ES_CounterId_t *CounterIdPtr, const char *CounterName)
{
    CFE_ES_GenCounterRecord_t *CounterRecPtr;
    int32                      Result;

    if (CounterName == NULL || CounterIdPtr == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    /*
    ** Search the ES Generic Counter table for a counter with a matching name.
    */
    CFE_ES_LockSharedData(__func__, __LINE__);

    CounterRecPtr = CFE_ES_LocateCounterRecordByName(CounterName);
    if (CounterRecPtr == NULL)
    {
        /*
         * ensure the output value is set to a safe value,
         * in case the caller does not check the return code.
         */
        Result        = CFE_ES_ERR_NAME_NOT_FOUND;
        *CounterIdPtr = CFE_ES_COUNTERID_UNDEFINED;
    }
    else
    {
        Result        = CFE_SUCCESS;
        *CounterIdPtr = CFE_ES_CounterRecordGetID(CounterRecPtr);
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_GetGenCounterName(char *CounterName, CFE_ES_CounterId_t CounterId, size_t BufferLength)
{
    CFE_ES_GenCounterRecord_t *CountRecPtr;
    CFE_Status_t               Status;

    if (BufferLength == 0 || CounterName == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    CountRecPtr = CFE_ES_LocateCounterRecordByID(CounterId);

    CFE_ES_LockSharedData(__func__, __LINE__);

    if (CFE_ES_CounterRecordIsMatch(CountRecPtr, CounterId))
    {
        strncpy(CounterName, CFE_ES_CounterRecordGetName(CountRecPtr), BufferLength - 1);
        CounterName[BufferLength - 1] = 0;
        Status                        = CFE_SUCCESS;
    }
    else
    {
        CounterName[0] = 0;
        Status         = CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_AppID_ToIndex(CFE_ES_AppId_t AppID, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(AppID), CFE_ES_APPID_BASE, CFE_PLATFORM_ES_MAX_APPLICATIONS,
                                  Idx);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_LibID_ToIndex(CFE_ES_LibId_t LibId, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(LibId), CFE_ES_LIBID_BASE, CFE_PLATFORM_ES_MAX_LIBRARIES, Idx);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_TaskID_ToIndex(CFE_ES_TaskId_t TaskID, uint32 *Idx)
{
    osal_id_t    OsalID;
    osal_index_t OsalIndex = OSAL_INDEX_C(0);
    int32        OsStatus;

    if (!CFE_RESOURCEID_TEST_DEFINED(TaskID))
    {
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    if (Idx == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    OsalID   = CFE_ES_TaskId_ToOSAL(TaskID);
    OsStatus = OS_ObjectIdToArrayIndex(OS_OBJECT_TYPE_OS_TASK, OsalID, &OsalIndex);
    if (OsStatus != OS_SUCCESS)
    {
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    *Idx = OsalIndex;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_ES_CounterID_ToIndex(CFE_ES_CounterId_t CounterId, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(CounterId), CFE_ES_COUNTID_BASE,
                                  CFE_PLATFORM_ES_MAX_GEN_COUNTERS, Idx);
}

/***************************************************************************************
** Private API functions
*/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_LockSharedData(const char *FunctionName, int32 LineNumber)
{
    int32 OsStatus;

    OsStatus = OS_MutSemTake(CFE_ES_Global.SharedDataMutex);
    if (OsStatus != OS_SUCCESS)
    {
        /*
         * NOTE: this is going to write into a buffer that itself
         * is _supposed_ to be protected by this same mutex.
         */
        CFE_ES_SysLogWrite_Unsync("%s: SharedData Mutex Take Err Stat=%ld,Func=%s,Line=%d\n", __func__, (long)OsStatus,
                                  FunctionName, (int)LineNumber);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_UnlockSharedData(const char *FunctionName, int32 LineNumber)
{
    int32 OsStatus;

    OsStatus = OS_MutSemGive(CFE_ES_Global.SharedDataMutex);
    if (OsStatus != OS_SUCCESS)
    {
        /*
         * NOTE: this is going to write into a buffer that itself
         * is _supposed_ to be protected by this same mutex.
         */
        CFE_ES_SysLogWrite_Unsync("%s: SharedData Mutex Give Err Stat=%ld,Func=%s,Line=%d\n", __func__, (long)OsStatus,
                                  FunctionName, (int)LineNumber);
    }
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_ProcessAsyncEvent(void)
{
    /* This just wakes up the background task to log/handle the event. */
    CFE_ES_BackgroundWakeup();
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
char *CFE_ES_StatusToString(CFE_Status_t status, CFE_StatusString_t *status_string)
{
    char *string = NULL;

    if (status_string != NULL)
    {
        snprintf(*status_string, sizeof(*status_string), "0x%08x", (unsigned int)status);
        string = *status_string;
    }
    return string;
}
