/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
**    cfe_es_apps.c
**
**  Purpose:
**    This file contains functions for starting cFE applications from a filesystem.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Includes
*/
#include "cfe_es_module_all.h"
#include "cfe_es_appctrl.h"

#include <stdio.h>
#include <string.h> /* memset() */
#include <fcntl.h>
#include <ctype.h>

#include "target_config.h"

/*
 * Mapping for control requests
 * Provides event details to send when the control request is processed
 */
typedef struct
{
    const char *ReqName; /* printable name */

    uint16 CleanupErrEventID; /* Event to send if cleanup failed (always an error) */
    uint16 StartupErrEventID; /* Event to send if startup failed (always an error)  */
    uint16 ErrorEventID;      /* Generic Error Event to send (valid for abnormal requests) */
    uint16 NominalEventID;    /* Generic Info Event to send (valid for normal requests) */
} CFE_ES_ControlReqMapEntry_t;

/*
****************************************************************************
** Functions
***************************************************************************
*/

/*----------------------------------------------------------------
 *
 * Local helper function
 * Sends the event after a control request error / invalid condition
 *
 *-----------------------------------------------------------------*/
static void CFE_ES_ProcessControlRequest_ErrorEvent(const CFE_ES_ControlReqMapEntry_t *ControlReqInfo,
                                                    const char                        *AppName,
                                                    uint32                             ControlReq)
{
    char        ControlReqBuff[16];
    const char *PrintableReqName;

    if (ControlReqInfo->ReqName == NULL)
    {
        /* This is for invalid request IDs - should never happen in operation. */
        snprintf(ControlReqBuff, sizeof(ControlReqBuff), "[Req%u]", (unsigned int)ControlReq);
        PrintableReqName = ControlReqBuff;
    }
    else
    {
        /* The request has a valid name, so use it directly */
        PrintableReqName = ControlReqInfo->ReqName;
    }

    CFE_EVS_SendEvent(ControlReqInfo->ErrorEventID,
                      CFE_EVS_EventType_ERROR,
                      "%s Application %s",
                      PrintableReqName,
                      AppName);
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Sends the event after successful processing of a control request
 *
 *-----------------------------------------------------------------*/
static void CFE_ES_ProcessControlRequest_SuccessEvent(const CFE_ES_ControlReqMapEntry_t *ControlReqInfo,
                                                      const char                        *AppName,
                                                      CFE_ES_AppId_t                     NewAppId)
{
    char AppIdStr[32];

    if (CFE_RESOURCEID_TEST_DEFINED(NewAppId))
    {
        /* Additional string showing the new appid */
        snprintf(AppIdStr, sizeof(AppIdStr), ", AppID=0x%lx", CFE_RESOURCEID_TO_ULONG(NewAppId));
    }
    else
    {
        /* No additional string */
        AppIdStr[0] = 0;
    }

    CFE_EVS_SendEvent(ControlReqInfo->NominalEventID,
                      CFE_EVS_EventType_INFORMATION,
                      "%s Application %s Success%s.",
                      ControlReqInfo->ReqName,
                      AppName,
                      AppIdStr);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_ES_ProcessControlRequest_Start(CFE_ES_AppId_t           AppId,
                                          char                    *OrigAppNameBuf,
                                          size_t                   OrigAppNameSz,
                                          CFE_ES_AppStartParams_t *RestartParamPtr)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    uint32              PendingControlReq;

    /* Init/clear all local state variables */
    PendingControlReq = 0;

    AppRecPtr = CFE_ES_LocateAppRecordByID(AppId);

    /*
     * Take a local snapshot of the important app record data
     * This way it becomes private and can be accessed without
     * concerns about other threads/tasks, even after the global
     * data records are eventually cleared.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);

    if (CFE_ES_AppRecordIsMatch(AppRecPtr, AppId))
    {
        PendingControlReq = AppRecPtr->ControlReq.AppControlRequest;
        strncpy(OrigAppNameBuf, AppRecPtr->AppName, OrigAppNameSz - 1);
        OrigAppNameBuf[OrigAppNameSz - 1] = 0;

        /* If a restart was requested, copy the parameters to re-use in new app */
        if (PendingControlReq == CFE_ES_RunStatus_SYS_RESTART || PendingControlReq == CFE_ES_RunStatus_SYS_RELOAD)
        {
            *RestartParamPtr = AppRecPtr->StartParams;
        }
    }

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    return PendingControlReq;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_ProcessControlRequest_Finish(uint32         ControlReq,
                                         const char    *AppName,
                                         CFE_Status_t   StartupStatus,
                                         CFE_Status_t   CleanupStatus,
                                         CFE_ES_AppId_t NewAppId)
{
    const CFE_ES_ControlReqMapEntry_t *ControlReqInfo;

    /*
     * NOTE: The ControlReqInfo entry should set the ErrorEventID (only) for
     * abnormal conditions.  Normal Request conditions should set NominalEventID
     * and should NOT set ErrorEventID.
     */

    static const CFE_ES_ControlReqMapEntry_t CONTROL_REQ_MAP[CFE_ES_RunStatus_MAX] =
    {
    [CFE_ES_RunStatus_UNDEFINED] =
    {
        /* catches all abnormal cases */
        .ErrorEventID = CFE_ES_PCR_ERR2_EID,
    },
    [CFE_ES_RunStatus_APP_RUN] =
    {
        /* while "APP_RUN" is normal, it should not show up in a control request */
        .ReqName = "Run",
        //jphfix .ErrorEventID = CFE_ES_PCR_ERR2_EID,
    },
    [CFE_ES_RunStatus_APP_EXIT] =
    {
        .ReqName = "Exit",
        .CleanupErrEventID = CFE_ES_EXIT_APP_ERR_EID,
        .NominalEventID = CFE_ES_EXIT_APP_INF_EID,
    },
    [CFE_ES_RunStatus_APP_ERROR] =
    {
        .ReqName = "ErrExit",
        .CleanupErrEventID = CFE_ES_ERREXIT_APP_ERR_EID,
        .NominalEventID = CFE_ES_ERREXIT_APP_INF_EID,
    },
    [CFE_ES_RunStatus_SYS_EXCEPTION] =
    {
        .ReqName = "Exception",
        .NominalEventID = CFE_ES_PCR_ERR1_EID,
    },
    [CFE_ES_RunStatus_SYS_RESTART] =
    {
        .ReqName = "Restart",
        .CleanupErrEventID = CFE_ES_RESTART_APP_ERR4_EID,
        .StartupErrEventID = CFE_ES_RESTART_APP_ERR3_EID,
        .NominalEventID = CFE_ES_RESTART_APP_INF_EID,
    },
    [CFE_ES_RunStatus_SYS_RELOAD] =
    {
        .ReqName = "Reload",
        .CleanupErrEventID = CFE_ES_RELOAD_APP_ERR4_EID,
        .StartupErrEventID = CFE_ES_RELOAD_APP_ERR3_EID,
        .NominalEventID = CFE_ES_RELOAD_APP_INF_EID,
    },
    [CFE_ES_RunStatus_SYS_DELETE] =
    {
        .ReqName = "Stop",
        .CleanupErrEventID = CFE_ES_STOP_ERR3_EID,
        .NominalEventID = CFE_ES_STOP_INF_EID,
    },
    [CFE_ES_RunStatus_CORE_APP_INIT_ERROR] =
    {
        .ReqName = "Core Init Err",
        .ErrorEventID = CFE_ES_PCR_ERR2_EID,
    },
    [CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR] =
    {
        .ReqName = "Core Runtime Err",
        .ErrorEventID = CFE_ES_PCR_ERR2_EID,
    },

    };

    if (ControlReq < CFE_ES_RunStatus_MAX)
    {
        ControlReqInfo = &CONTROL_REQ_MAP[ControlReq];
    }
    else
    {
        ControlReqInfo = &CONTROL_REQ_MAP[CFE_ES_RunStatus_UNDEFINED];
    }

    /* If the ErrorEventID is set, it will ALWAYS be sent */
    /* This also catches all invalid request IDs where ReqName == NULL.  */
    if (ControlReqInfo->ReqName == NULL || ControlReqInfo->ErrorEventID != 0)
    {
        /* This event sender can tolerate ReqName being NULL (unknown) */
        CFE_ES_ProcessControlRequest_ErrorEvent(ControlReqInfo, AppName, ControlReq);
    }
    else
    {
        /*
         * NOTE: all these event senders require that ReqName is valid/non-NULL
         * The CleanupStatus is a dont-care when CleanupErrEventID is not set
         * The StartupStatus is a dont-care when StartupErrEventID is not set
         */

        /* If there is a specific event for cleanup error, send it */
        if (CleanupStatus != CFE_SUCCESS && ControlReqInfo->CleanupErrEventID != 0)
        {
            /* Make detail message for event containing cleanup error code */
            CFE_EVS_SendEvent(ControlReqInfo->CleanupErrEventID,
                              CFE_EVS_EventType_ERROR,
                              "%s Application %s Failed: CleanUpApp Error 0x%08X.",
                              ControlReqInfo->ReqName,
                              AppName,
                              (unsigned int)CleanupStatus);
        }
        else if (StartupStatus != CFE_SUCCESS && ControlReqInfo->StartupErrEventID != 0)
        {
            /* Make detail message for event containing startup error code */
            /* The StartupStatus is a dont-care if StartupErrEventID is not set */
            CFE_EVS_SendEvent(ControlReqInfo->StartupErrEventID,
                              CFE_EVS_EventType_ERROR,
                              "%s Application %s Failed: AppCreate Error 0x%08X.",
                              ControlReqInfo->ReqName,
                              AppName,
                              (unsigned int)StartupStatus);
        }
        else if (ControlReqInfo->NominalEventID != 0)
        {
            /* Make detail message including new AppID, if valid */
            CFE_ES_ProcessControlRequest_SuccessEvent(ControlReqInfo, AppName, NewAppId);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_ProcessControlRequest(CFE_ES_AppId_t AppId)
{
    uint32                  PendingControlReq;
    CFE_ES_AppStartParams_t RestartParams;
    char                    OrigAppName[OS_MAX_API_NAME];
    CFE_Status_t            CleanupStatus;
    CFE_Status_t            StartupStatus;
    CFE_ES_AppId_t          NewAppId;

    /* Init/clear all local state variables */
    NewAppId = CFE_ES_APPID_UNDEFINED;
    memset(&RestartParams, 0, sizeof(RestartParams));
    memset(&OrigAppName, 0, sizeof(OrigAppName));

    /*
     * This subroutine looks up the app, gets the name, checks for the pending request
     */
    PendingControlReq = CFE_ES_ProcessControlRequest_Start(AppId, OrigAppName, sizeof(OrigAppName), &RestartParams);

    /*
     * All control requests start by deleting the app/task and
     * all associated resources.
     *
     * The reload/restart requests will start it again, and it gets
     * a new appID.  For other requests it just leaves it deleted.
     *
     * Note that Cleanup can fail for a variety of reasons, including
     * situations where e.g. a task ID had become stale because the task
     * already exited itself.  In most cases these are minor errors and
     * reflect problems with the consistency of the old app record.
     *
     * Even when this happens the cleanup should still do its best effort
     * to release all relevant global data entries.  So it should not
     * prevent starting the new app, if a restart/reload is indicated.
     */
    CleanupStatus = CFE_ES_CleanUpApp(AppId);

    /*
     * Attempt to restart the app if the request indicated to do so,
     * regardless of the CleanupStatus.
     */
    if (PendingControlReq == CFE_ES_RunStatus_SYS_RESTART || PendingControlReq == CFE_ES_RunStatus_SYS_RELOAD)
    {
        StartupStatus = CFE_ES_AppCreate(&NewAppId, OrigAppName, &RestartParams);
    }
    else
    {
        StartupStatus = CFE_STATUS_NOT_IMPLEMENTED;
    }

    /*
     * Send the event(s)
     *
     * Determine the event ID associated with the control request,
     * which indicates the success/failure of the operation and
     * any other relevant detail.
     *
     * Note that the specific event ID that gets generated is the only
     * other difference between all these control request types.
     */
    CFE_ES_ProcessControlRequest_Finish(PendingControlReq, OrigAppName, StartupStatus, CleanupStatus, NewAppId);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_RunAppTableScan(uint32 ElapsedTime, void *Arg)
{
    CFE_ES_AppTableScanState_t *State = (CFE_ES_AppTableScanState_t *)Arg;
    uint32                      i;
    CFE_ES_AppRecord_t         *AppPtr;
    CFE_ES_AppId_t              AppTimeoutList[CFE_PLATFORM_ES_MAX_APPLICATIONS];
    uint32                      NumAppTimeouts;

    if (State->PendingAppStateChanges == 0)
    {
        /*
         * If the command count changes, then a scan becomes due immediately.
         */
        if (State->LastScanCommandCount == CFE_ES_Global.TaskData.CommandCounter
            && State->BackgroundScanTimer > ElapsedTime)
        {
            /* no action at this time, background scan is not due yet */
            State->BackgroundScanTimer -= ElapsedTime;
            return false;
        }
    }

    /*
     * Every time a scan is initiated (for any reason)
     * reset the background scan timer to the full value,
     * and take a snapshot of the command counter.
     */
    NumAppTimeouts                = 0;
    State->BackgroundScanTimer    = CFE_PLATFORM_ES_APP_SCAN_RATE;
    State->LastScanCommandCount   = CFE_ES_Global.TaskData.CommandCounter;
    State->PendingAppStateChanges = 0;

    /*
     * Scan needs to be done with the table locked,
     * as these state changes need to be done atomically
     * with respect to other tasks that also access/update
     * the state.
     */
    CFE_ES_LockSharedData(__func__, __LINE__);

    /*
    ** Scan the ES Application table. Skip entries that are:
    **  - Not in use, or
    **  - cFE Core apps, or
    **  - Currently running
    */
    AppPtr = CFE_ES_Global.AppTable;
    for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; i++)
    {
        if (CFE_ES_AppRecordIsUsed(AppPtr) && AppPtr->Type == CFE_ES_AppType_EXTERNAL)
        {
            if (AppPtr->AppState > CFE_ES_AppState_RUNNING)
            {
                /*
                 * Increment the "pending" counter which reflects
                 * the number of apps that are in some phase of clean up.
                 */
                ++State->PendingAppStateChanges;

                /*
                 * Decrement the wait timer, if active.
                 * When the timeout value becomes zero, take the action to delete/restart/reload the app
                 */
                if (AppPtr->ControlReq.AppTimerMsec > ElapsedTime)
                {
                    AppPtr->ControlReq.AppTimerMsec -= ElapsedTime;
                }
                else
                {
                    AppPtr->ControlReq.AppTimerMsec = 0;

                    /* Add it to the list to be processed later */
                    AppTimeoutList[NumAppTimeouts] = CFE_ES_AppRecordGetID(AppPtr);
                    ++NumAppTimeouts;
                }
            }
            else if (AppPtr->AppState == CFE_ES_AppState_RUNNING
                     && AppPtr->ControlReq.AppControlRequest > CFE_ES_RunStatus_APP_RUN)
            {
                /* this happens after a command arrives to restart/reload/delete an app */
                /* switch to WAITING state, and set the timer for transition */
                AppPtr->AppState                = CFE_ES_AppState_WAITING;
                AppPtr->ControlReq.AppTimerMsec = CFE_PLATFORM_ES_APP_KILL_TIMEOUT * CFE_PLATFORM_ES_APP_SCAN_RATE;
            }
        }

        ++AppPtr;

    } /* end for loop */

    CFE_ES_UnlockSharedData(__func__, __LINE__);

    /*
     * Now invoke the CFE_ES_ProcessControlRequest() routine for any app
     * which has reached that point.
     */
    for (i = 0; i < NumAppTimeouts; i++)
    {
        /*
         * Call CFE_ES_ProcessControlRequest() with a reference to
         * the _copies_ of the app record details.  (This avoids
         * needing to access the global records outside of the lock).
         */
        CFE_ES_ProcessControlRequest(AppTimeoutList[i]);
    }

    /*
     * This state machine is considered active if there are any
     * pending app state changes.  Returning "true" will cause this job
     * to be called from the background task at a faster interval.
     */
    return (State->PendingAppStateChanges != 0);
}
