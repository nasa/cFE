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

/******************************************************************************
** File: cfe_sb_api.c
**
** Purpose:
**      This file contains the source code for the SB API's.
**
** Notes: The following 4 terms have been, or are used in the cFS architecture and implementation
**
**      StreamId - First 16 bits of CCSDS Space Packet Protocol (SPP) 133.0-B.1c2 Blue Book
**                 packet primary header. It contains the 3 bit Version Number, 1 bit Packet Type ID,
**                 1 bit Secondary Header flag, and 11 bit Application Process ID
**                 It was used in earlier cFS implementations and is defined here for historical reference
**                 It is NOT exposed to user applications.
**
**      MsgId    - Unique numeric message identifier within a mission namespace. It is used by cFS
**                 applications to the identify messages for publishing and subscribing
**                 It is used by the SB API and encoded in a mission defended way in the header of
**                 all cFS messages.
**                 It is exposed to all cFS applications
**
**      ApId     - CCSDS Application Process Id field in the primary header.
**                 It has default bit mask of 0x07FF and is part of the cFS message Id
**                 It should not be confused with the cFE Executive Services (ES) term appId which
**                 identifies the software application/component
**                 It is NOT exposed to user applications.
**
**      MsgIdkey - This is a unique numeric key within a mission namespace that is used with
**                 cFS software bus internal structures.
**                 It is algorithmically created in a mission defined way from the MsgId to support
**                 efficient lookup and mapping implementations
**                 It is NOT exposed to user applications.
**
** Author:   R.McGraw/SSI
**           J.Wilmot/NASA
**
******************************************************************************/

/*
** Include Files
*/
#include "cfe_sb_module_all.h"

#include <string.h>

/*
 * Macro to reflect size of PipeDepthStats Telemetry array -
 * this may or may not be the same as CFE_SB_MSG_MAX_PIPES
 */
#define CFE_SB_TLM_PIPEDEPTHSTATS_SIZE                         \
    (sizeof(CFE_SB_Global.StatTlmMsg.Payload.PipeDepthStats) / \
     sizeof(CFE_SB_Global.StatTlmMsg.Payload.PipeDepthStats[0]))

/* Local structure for remove pipe callbacks */
typedef struct
{
    const char *    FullName; /* Full name (app.task) for error reporting */
    CFE_SB_PipeId_t PipeId;   /* Pipe id to remove */
} CFE_SB_RemovePipeCallback_t;

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_PipeId_ToIndex(CFE_SB_PipeId_t PipeID, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(PipeID), CFE_SB_PIPEID_BASE, CFE_PLATFORM_SB_MAX_PIPES, Idx);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
    CFE_ES_AppId_t   AppId;
    CFE_ES_TaskId_t  TskId;
    osal_id_t        SysQueueId;
    int32            OsStatus;
    int32            Status;
    CFE_SB_PipeD_t * PipeDscPtr;
    CFE_ResourceId_t PendingPipeId = CFE_RESOURCEID_UNDEFINED;
    uint16           PendingEventId;
    char             FullName[(OS_MAX_API_NAME * 2)];

    Status         = CFE_SUCCESS;
    SysQueueId     = OS_OBJECT_ID_UNDEFINED;
    PendingEventId = 0;
    PipeDscPtr     = NULL;
    OsStatus       = OS_SUCCESS;

    /*
     * Get caller AppId.
     *
     * This is needed for both success and failure cases,
     * as it is stored in the Pipe Descriptor on success,
     * and used for events on failure,  so get it now.
     */
    CFE_ES_GetAppID(&AppId);

    /* get callers TaskId */
    CFE_ES_GetTaskID(&TskId);

    /* check input parameters */
    if ((PipeIdPtr == NULL) || (Depth > OS_QUEUE_MAX_DEPTH) || (Depth == 0) ||
        (PipeName != NULL && PipeName[0] == '\0'))
    {
        PendingEventId = CFE_SB_CR_PIPE_BAD_ARG_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        /* Get an available Pipe Descriptor which must be done while locked */
        CFE_SB_LockSharedData(__func__, __LINE__);

        /* get first available entry in pipe table */
        PendingPipeId =
            CFE_ResourceId_FindNext(CFE_SB_Global.LastPipeId, CFE_PLATFORM_SB_MAX_PIPES, CFE_SB_CheckPipeDescSlotUsed);
        PipeDscPtr = CFE_SB_LocatePipeDescByID(CFE_SB_PIPEID_C(PendingPipeId));

        /* if pipe table is full, send event and return error */
        if (PipeDscPtr == NULL)
        {
            PendingEventId = CFE_SB_MAX_PIPES_MET_EID;
            Status         = CFE_SB_MAX_PIPES_MET;
        }
        else
        {
            /* Fully clear the entry, just in case of stale data */
            memset(PipeDscPtr, 0, sizeof(*PipeDscPtr));

            CFE_SB_PipeDescSetUsed(PipeDscPtr, CFE_RESOURCEID_RESERVED);
            CFE_SB_Global.LastPipeId = PendingPipeId;
        }

        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    if (Status == CFE_SUCCESS)
    {
        /* create the queue */
        OsStatus = OS_QueueCreate(&SysQueueId, PipeName, Depth, sizeof(CFE_SB_BufferD_t *), 0);
        if (OsStatus != OS_SUCCESS)
        {
            if (OsStatus == OS_ERR_NAME_TAKEN)
            {
                PendingEventId = CFE_SB_CR_PIPE_NAME_TAKEN_EID;
            }
            else if (OsStatus == OS_ERR_NO_FREE_IDS)
            {
                PendingEventId = CFE_SB_CR_PIPE_NO_FREE_EID;
            }
            else
            {
                /* some other unexpected error */
                PendingEventId = CFE_SB_CR_PIPE_ERR_EID;
            }

            /* translate OSAL error to CFE error code */
            Status = CFE_SB_PIPE_CR_ERR;
        }
    }

    CFE_SB_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        /* fill in the pipe table fields */
        PipeDscPtr->SysQueueId    = SysQueueId;
        PipeDscPtr->MaxQueueDepth = Depth;
        PipeDscPtr->AppId         = AppId;

        CFE_SB_PipeDescSetUsed(PipeDscPtr, PendingPipeId);

        /* Increment the Pipes in use ctr and if it's > the high water mark,*/
        /* adjust the high water mark */
        CFE_SB_Global.StatTlmMsg.Payload.PipesInUse++;
        if (CFE_SB_Global.StatTlmMsg.Payload.PipesInUse > CFE_SB_Global.StatTlmMsg.Payload.PeakPipesInUse)
        {
            CFE_SB_Global.StatTlmMsg.Payload.PeakPipesInUse = CFE_SB_Global.StatTlmMsg.Payload.PipesInUse;
        }
    }
    else
    {
        /*
         * If a descriptor had been allocated, then free it.
         */
        if (PipeDscPtr != NULL)
        {
            CFE_SB_PipeDescSetFree(PipeDscPtr);
            PipeDscPtr = NULL;
        }
        PendingPipeId = CFE_RESOURCEID_UNDEFINED;

        /* Increment error counter for all errors */
        CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter++;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* Send any pending events now, after final unlock */
    switch (PendingEventId)
    {
        case CFE_SB_CR_PIPE_BAD_ARG_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_BAD_ARG_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "CreatePipeErr:Bad Input Arg:app=%s,ptr=0x%lx,depth=%d,maxdepth=%d",
                                       CFE_SB_GetAppTskName(TskId, FullName), (unsigned long)PipeIdPtr, (int)Depth,
                                       OS_QUEUE_MAX_DEPTH);
            break;

        case CFE_SB_MAX_PIPES_MET_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_MAX_PIPES_MET_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "CreatePipeErr:Max Pipes(%d)In Use.app %s", CFE_PLATFORM_SB_MAX_PIPES,
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;
        case CFE_SB_CR_PIPE_NAME_TAKEN_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_NAME_TAKEN_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "CreatePipeErr:OS_QueueCreate failed, name taken (app=%s, name=%s)",
                                       CFE_SB_GetAppTskName(TskId, FullName), PipeName);
            break;
        case CFE_SB_CR_PIPE_NO_FREE_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_NO_FREE_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "CreatePipeErr:OS_QueueCreate failed, no free id's (app=%s)",
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;
        case CFE_SB_CR_PIPE_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "CreatePipeErr:OS_QueueCreate returned %ld,app %s", (long)OsStatus,
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        default:
            break;
    }

    if (Status == CFE_SUCCESS)
    {
        /* send debug event */
        CFE_EVS_SendEventWithAppID(CFE_SB_PIPE_ADDED_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Pipe Created:name %s,id %d,app %s", PipeName,
                                   (int)CFE_ResourceId_ToInteger(PendingPipeId), CFE_SB_GetAppTskName(TskId, FullName));

        /* give the pipe handle to the caller */
        *PipeIdPtr = CFE_SB_PIPEID_C(PendingPipeId);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    CFE_ES_AppId_t CallerId;
    int32          Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_DeletePipeFull(PipeId, CallerId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId)
{
    int32 Status = 0;

    Status = CFE_SB_DeletePipeFull(PipeId, AppId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Callback for deleting a pipe from a route
 *
 *-----------------------------------------------------------------*/
void CFE_SB_RemovePipeFromRoute(CFE_SBR_RouteId_t RouteId, void *ArgPtr)
{
    CFE_SB_DestinationD_t *      destptr;
    CFE_SB_RemovePipeCallback_t *args;

    args = (CFE_SB_RemovePipeCallback_t *)ArgPtr;

    destptr = CFE_SB_GetDestPtr(RouteId, args->PipeId);

    if (destptr != NULL)
    {
        CFE_SB_RemoveDest(RouteId, destptr);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId)
{
    CFE_SB_PipeD_t *            PipeDscPtr;
    int32                       Status;
    CFE_ES_TaskId_t             TskId;
    CFE_SB_BufferD_t *          BufDscPtr;
    osal_id_t                   SysQueueId;
    char                        FullName[(OS_MAX_API_NAME * 2)];
    size_t                      BufDscSize;
    CFE_SB_RemovePipeCallback_t Args;
    uint16                      PendingEventID;

    Status         = CFE_SUCCESS;
    PendingEventID = 0;
    SysQueueId     = OS_OBJECT_ID_UNDEFINED;
    BufDscPtr      = NULL;

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check input parameter */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
    {
        PendingEventID = CFE_SB_DEL_PIPE_ERR1_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    /* check that the given AppId is the owner of the pipe */
    else if (!CFE_RESOURCEID_TEST_EQUAL(AppId, PipeDscPtr->AppId))
    {
        PendingEventID = CFE_SB_DEL_PIPE_ERR2_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        /* Remove the pipe from all routes */
        Args.PipeId   = PipeId;
        Args.FullName = FullName;
        CFE_SBR_ForEachRouteId(CFE_SB_RemovePipeFromRoute, &Args, NULL);

        /*
         * With the route removed there should be no new messages written to this pipe,
         *
         * but the pipe ID itself also needs to be invalidated now (before releasing lock) to make
         * sure that no no subscriptions/routes can be added either.
         *
         * However we must first save certain state data for later deletion.
         */
        SysQueueId = PipeDscPtr->SysQueueId;
        BufDscPtr  = PipeDscPtr->LastBuffer;

        /*
         * Mark entry as "reserved" so other resources can be deleted
         * while the SB global is unlocked.  This prevents other tasks
         * from trying to use this Pipe Desc slot, and also should prevents
         * any task from re-subscribing to this pipe.
         */
        CFE_SB_PipeDescSetUsed(PipeDscPtr, CFE_RESOURCEID_RESERVED);
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* remove any messages that might be on the pipe */
    if (Status == CFE_SUCCESS)
    {
        while (true)
        {
            /* decrement refcount of any previous buffer */
            if (BufDscPtr != NULL)
            {
                CFE_SB_LockSharedData(__func__, __LINE__);
                CFE_SB_DecrBufUseCnt(BufDscPtr);
                CFE_SB_UnlockSharedData(__func__, __LINE__);
                BufDscPtr = NULL;
            }

            if (OS_QueueGet(SysQueueId, &BufDscPtr, sizeof(BufDscPtr), &BufDscSize, OS_CHECK) != OS_SUCCESS)
            {
                /* no more messages */
                break;
            }
        }

        /* Delete the underlying OS queue */
        OS_QueueDelete(SysQueueId);
    }

    /*
     * Final cleanup with global data locked
     */
    CFE_SB_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        CFE_SB_PipeDescSetFree(PipeDscPtr);
        --CFE_SB_Global.StatTlmMsg.Payload.PipesInUse;
    }
    else
    {
        CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter++;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* Send Events */
    if (PendingEventID != 0)
    {
        /* get TaskId and name of caller for events */
        CFE_ES_GetTaskID(&TskId);
        CFE_SB_GetAppTskName(TskId, FullName);
    }
    else
    {
        TskId       = CFE_ES_TASKID_UNDEFINED;
        FullName[0] = 0;
    }

    switch (PendingEventID)
    {
        case CFE_SB_DEL_PIPE_ERR1_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR1_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Delete Error:Bad Argument,PipedId %ld,Requestor %s",
                                       CFE_RESOURCEID_TO_ULONG(PipeId), FullName);
            break;
        case CFE_SB_DEL_PIPE_ERR2_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR2_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Delete Error:Caller(%s) is not the owner of pipe %ld", FullName,
                                       CFE_RESOURCEID_TO_ULONG(PipeId));
            break;

        default:
            break;
    }

    if (Status == CFE_SUCCESS)
    {
        /*
         * Get the app name of the actual pipe owner for the event string
         * as this may be different than the task doing the deletion.
         *
         * Note: If this fails (e.g. bad AppID, it returns an empty string
         */
        CFE_ES_GetAppName(FullName, AppId, sizeof(FullName));

        CFE_EVS_SendEventWithAppID(CFE_SB_PIPE_DELETED_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Pipe Deleted:id %d,owner %s", (int)CFE_RESOURCEID_TO_ULONG(PipeId), FullName);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_SetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 Opts)
{
    CFE_SB_PipeD_t *PipeDscPtr;
    CFE_ES_AppId_t  AppID;
    CFE_ES_TaskId_t TskId;
    uint16          PendingEventID;
    int32           Status;
    char            FullName[(OS_MAX_API_NAME * 2)];

    PendingEventID = 0;

    Status = CFE_ES_GetAppID(&AppID);
    if (Status != CFE_SUCCESS)
    {
        /* shouldn't happen... */
        return Status;
    }

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check input parameter */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
    {
        PendingEventID = CFE_SB_SETPIPEOPTS_ID_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    /* check that the caller AppId is the owner of the pipe */
    else if (!CFE_RESOURCEID_TEST_EQUAL(AppID, PipeDscPtr->AppId))
    {
        PendingEventID = CFE_SB_SETPIPEOPTS_OWNER_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        PipeDscPtr->Opts = Opts;
    }

    /* If anything went wrong, increment the error counter before unlock */
    if (Status != CFE_SUCCESS)
    {
        CFE_SB_Global.HKTlmMsg.Payload.PipeOptsErrorCounter++;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* Send Events */
    if (PendingEventID != 0)
    {
        /* get TaskId of caller for events */
        CFE_ES_GetTaskID(&TskId);
    }
    else
    {
        TskId = CFE_ES_TASKID_UNDEFINED;
    }

    switch (PendingEventID)
    {
        case CFE_SB_SETPIPEOPTS_ID_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_ID_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Opts Error:Bad Argument,PipedId %lu,Requestor %s",
                                       CFE_RESOURCEID_TO_ULONG(PipeId), CFE_SB_GetAppTskName(TskId, FullName));
            break;
        case CFE_SB_SETPIPEOPTS_OWNER_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_OWNER_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Opts Set Error: Caller(%s) is not the owner of pipe %lu",
                                       CFE_SB_GetAppTskName(TskId, FullName), CFE_RESOURCEID_TO_ULONG(PipeId));
            break;

        default:
            break;
    }

    if (Status == CFE_SUCCESS)
    {
        /* get AppID of caller for events */
        CFE_ES_GetAppName(FullName, AppID, sizeof(FullName));

        CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Pipe opts set:id %lu,owner %s, opts=0x%02x", CFE_RESOURCEID_TO_ULONG(PipeId),
                                   FullName, (unsigned int)Opts);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptsPtr)
{
    int32           Status;
    CFE_ES_TaskId_t TskId;
    char            FullName[(OS_MAX_API_NAME * 2)];
    uint16          PendingEventID;
    CFE_SB_PipeD_t *PipeDscPtr;

    PendingEventID = 0;
    Status         = CFE_SUCCESS;

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check input parameter */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
    {
        PendingEventID = CFE_SB_GETPIPEOPTS_ID_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else if (OptsPtr == NULL)
    {
        PendingEventID = CFE_SB_GETPIPEOPTS_PTR_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        *OptsPtr = PipeDscPtr->Opts;
    }

    /* If anything went wrong, increment the error counter before unlock */
    if (Status != CFE_SUCCESS)
    {
        CFE_SB_Global.HKTlmMsg.Payload.PipeOptsErrorCounter++;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* Send Events */
    if (PendingEventID != 0)
    {
        /* get TaskId of caller for events */
        CFE_ES_GetTaskID(&TskId);
    }
    else
    {
        TskId = CFE_ES_TASKID_UNDEFINED;
    }

    switch (PendingEventID)
    {
        case CFE_SB_GETPIPEOPTS_PTR_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEOPTS_PTR_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Opts Error:Bad Argument,Requestor %s",
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;
        case CFE_SB_GETPIPEOPTS_ID_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEOPTS_ID_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Opts Error:Bad Argument,PipedId %lu,Requestor %s",
                                       CFE_RESOURCEID_TO_ULONG(PipeId), CFE_SB_GetAppTskName(TskId, FullName));
            break;
        default:
            break;
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEOPTS_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Pipe opts get:id %lu, opts=0x%02x", CFE_RESOURCEID_TO_ULONG(PipeId),
                                   (unsigned int)*OptsPtr);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_GetPipeName(char *PipeNameBuf, size_t PipeNameSize, CFE_SB_PipeId_t PipeId)
{
    int32           OsStatus;
    int32           Status;
    CFE_ES_TaskId_t TskId;
    char            FullName[(OS_MAX_API_NAME * 2)];
    uint16          PendingEventID;
    CFE_SB_PipeD_t *PipeDscPtr;
    osal_id_t       SysQueueId;

    PendingEventID = 0;
    Status         = CFE_SUCCESS;
    SysQueueId     = OS_OBJECT_ID_UNDEFINED;

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check input parameter */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
    {
        PendingEventID = CFE_SB_GETPIPENAME_ID_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        SysQueueId = PipeDscPtr->SysQueueId;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        if (PipeNameBuf == NULL || PipeNameSize == 0)
        {
            PendingEventID = CFE_SB_GETPIPENAME_NULL_PTR_EID;
            Status         = CFE_SB_BAD_ARGUMENT;
        }
        else
        {
            OsStatus = OS_GetResourceName(SysQueueId, PipeNameBuf, PipeNameSize);

            if (OsStatus == OS_SUCCESS)
            {
                Status = CFE_SUCCESS;
            }
            else
            {
                PendingEventID = CFE_SB_GETPIPENAME_ID_ERR_EID;
                Status         = CFE_SB_BAD_ARGUMENT;
            }
        }
    }

    /* Send Events */
    if (PendingEventID != 0)
    {
        /* get TaskId of caller for events */
        CFE_ES_GetTaskID(&TskId);
    }
    else
    {
        TskId = CFE_ES_TASKID_UNDEFINED;
    }

    switch (PendingEventID)
    {
        case CFE_SB_GETPIPENAME_NULL_PTR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_NULL_PTR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Name Error:NullPtr,Requestor %s", CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_GETPIPENAME_ID_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_ID_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Pipe Id Error:Bad Argument,Id=%lu,Requestor %s",
                                       CFE_RESOURCEID_TO_ULONG(PipeId), CFE_SB_GetAppTskName(TskId, FullName));
            break;

        default:
            break;
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "GetPipeName name=%s id=%lu", PipeNameBuf, CFE_RESOURCEID_TO_ULONG(PipeId));
    }
    else
    {
        if (PipeNameBuf != NULL && PipeNameSize > 0)
        {
            memset(PipeNameBuf, 0, PipeNameSize);
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_GetPipeIdByName(CFE_SB_PipeId_t *PipeIdPtr, const char *PipeName)
{
    int32           OsStatus;
    int32           Status;
    CFE_ES_TaskId_t TskId;
    uint32          Idx;
    char            FullName[(OS_MAX_API_NAME * 2)];
    uint16          PendingEventID;
    CFE_SB_PipeD_t *PipeDscPtr;
    osal_id_t       SysQueueId;

    PendingEventID = 0;
    SysQueueId     = OS_OBJECT_ID_UNDEFINED;

    if (PipeName == NULL || PipeIdPtr == NULL)
    {
        PendingEventID = CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        /* Get QueueID from OSAL */
        OsStatus = OS_QueueGetIdByName(&SysQueueId, PipeName);
        if (OsStatus == OS_SUCCESS)
        {
            Status = CFE_SUCCESS;
        }
        else
        {
            PendingEventID = CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID;
            Status         = CFE_SB_BAD_ARGUMENT;
        }
    }

    CFE_SB_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        Idx        = CFE_PLATFORM_SB_MAX_PIPES;
        PipeDscPtr = CFE_SB_Global.PipeTbl;
        while (true)
        {
            if (Idx == 0)
            {
                PendingEventID = CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID;
                Status         = CFE_SB_BAD_ARGUMENT;
                break;
            }

            if (OS_ObjectIdEqual(PipeDscPtr->SysQueueId, SysQueueId))
            {
                /* grab the ID before we release the lock */
                *PipeIdPtr = CFE_SB_PipeDescGetID(PipeDscPtr);
                break;
            }

            --Idx;
            ++PipeDscPtr;
        }
    }

    if (Status != CFE_SUCCESS)
    {
        ++CFE_SB_Global.HKTlmMsg.Payload.GetPipeIdByNameErrorCounter;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* Send Events */
    if (PendingEventID != 0)
    {
        /* get TaskId of caller for events */
        CFE_ES_GetTaskID(&TskId);
    }
    else
    {
        TskId = CFE_ES_TASKID_UNDEFINED;
    }

    switch (PendingEventID)
    {
        case CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_SB_Global.AppId, "Pipe ID By Name Error:Bad Argument,Requestor %s",
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_SB_Global.AppId, "Pipe ID By Name Error:Bad Argument,Requestor %s",
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        default:
            break;
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "PipeIdByName name=%s id=%lu", PipeName, CFE_RESOURCEID_TO_ULONG(*PipeIdPtr));
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_SubscribeEx(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId, PipeId, Quality, MsgLim, (uint8)CFE_SB_MSG_GLOBAL);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint16 MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId, PipeId, CFE_SB_DEFAULT_QOS, MsgLim, (uint8)CFE_SB_MSG_LOCAL);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    return CFE_SB_SubscribeFull(MsgId, PipeId, CFE_SB_DEFAULT_QOS, (uint16)CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT,
                                (uint8)CFE_SB_MSG_GLOBAL);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_SubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim,
                           uint8 Scope)
{
    CFE_SBR_RouteId_t      RouteId;
    CFE_SB_PipeD_t *       PipeDscPtr;
    int32                  Status;
    CFE_ES_TaskId_t        TskId;
    CFE_ES_AppId_t         AppId;
    CFE_SB_DestinationD_t *DestPtr;
    uint32                 DestCount;
    char                   FullName[(OS_MAX_API_NAME * 2)];
    char                   PipeName[OS_MAX_API_NAME];
    uint32                 Collisions;
    uint16                 PendingEventID;

    PendingEventID = 0;
    Status         = CFE_SUCCESS;
    DestPtr        = NULL;
    Collisions     = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&AppId);

    /* get TaskId of caller for events */
    CFE_ES_GetTaskID(&TskId);

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check that the pipe has been created */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
    {
        PendingEventID = CFE_SB_SUB_INV_PIPE_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else if (!CFE_RESOURCEID_TEST_EQUAL(PipeDscPtr->AppId, AppId))
    {
        PendingEventID = CFE_SB_SUB_INV_CALLER_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    /* check message id key and scope */
    else if (!CFE_SB_IsValidMsgId(MsgId) || (Scope > 1))
    {
        PendingEventID = CFE_SB_SUB_ARG_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        /* Get the route, adding one if it does not exist already */
        RouteId = CFE_SBR_GetRouteId(MsgId);

        if (!CFE_SBR_IsValidRouteId(RouteId))
        {
            /* Add the route */
            RouteId = CFE_SBR_AddRoute(MsgId, &Collisions);

            /* if all routing table elements are used, send event */
            if (!CFE_SBR_IsValidRouteId(RouteId))
            {
                PendingEventID = CFE_SB_MAX_MSGS_MET_EID;
                Status         = CFE_SB_MAX_MSGS_MET;
            }
            else
            {
                /* Increment the MsgIds in use ctr and if it's > the high water mark,*/
                /* adjust the high water mark */
                CFE_SB_Global.StatTlmMsg.Payload.MsgIdsInUse++;
                if (CFE_SB_Global.StatTlmMsg.Payload.MsgIdsInUse > CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse)
                {
                    CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse = CFE_SB_Global.StatTlmMsg.Payload.MsgIdsInUse;
                }
            }
        }
    }

    /* If successful up to this point, check if new dest should be added to this route */
    if (Status == CFE_SUCCESS)
    {
        DestCount = 0;
        for (DestPtr = CFE_SBR_GetDestListHeadPtr(RouteId); DestPtr != NULL; DestPtr = DestPtr->Next)
        {
            ++DestCount;

            /* Check if duplicate (status stays as CFE_SUCCESS) */
            if (CFE_RESOURCEID_TEST_EQUAL(DestPtr->PipeId, PipeId))
            {
                PendingEventID = CFE_SB_DUP_SUBSCRIP_EID;
                break;
            }

            /* Check if limit reached */
            if (DestCount >= CFE_PLATFORM_SB_MAX_DEST_PER_PKT)
            {
                PendingEventID = CFE_SB_MAX_DESTS_MET_EID;
                Status         = CFE_SB_MAX_DESTS_MET;
                break;
            }
        }

        /* If no existing dest found, add one now */
        if (DestPtr == NULL)
        {
            DestPtr = CFE_SB_GetDestinationBlk();
            if (DestPtr == NULL)
            {
                PendingEventID = CFE_SB_DEST_BLK_ERR_EID;
                Status         = CFE_SB_BUF_ALOC_ERR;
            }
            else
            {
                /* initialize destination block */
                DestPtr->PipeId        = PipeId;
                DestPtr->MsgId2PipeLim = MsgLim;
                DestPtr->Active        = CFE_SB_ACTIVE;
                DestPtr->BuffCount     = 0;
                DestPtr->DestCnt       = 0;
                DestPtr->Scope         = Scope;
                DestPtr->Prev          = NULL;
                DestPtr->Next          = NULL;

                /* add destination node */
                CFE_SB_AddDestNode(RouteId, DestPtr);

                CFE_SB_Global.StatTlmMsg.Payload.SubscriptionsInUse++;
                if (CFE_SB_Global.StatTlmMsg.Payload.SubscriptionsInUse >
                    CFE_SB_Global.StatTlmMsg.Payload.PeakSubscriptionsInUse)
                {
                    CFE_SB_Global.StatTlmMsg.Payload.PeakSubscriptionsInUse =
                        CFE_SB_Global.StatTlmMsg.Payload.SubscriptionsInUse;
                }
            }
        }
    }

    /* Increment counter before unlock */
    switch (PendingEventID)
    {
        case CFE_SB_SUB_INV_PIPE_EID:
        case CFE_SB_SUB_INV_CALLER_EID:
        case CFE_SB_SUB_ARG_ERR_EID:
        case CFE_SB_MAX_MSGS_MET_EID:
        case CFE_SB_DEST_BLK_ERR_EID:
        case CFE_SB_MAX_DESTS_MET_EID:
            CFE_SB_Global.HKTlmMsg.Payload.SubscribeErrorCounter++;
            break;
        case CFE_SB_DUP_SUBSCRIP_EID:
            CFE_SB_Global.HKTlmMsg.Payload.DuplicateSubscriptionsCounter++;
            break;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* Send events now - get the pipe name only if something is pending */
    if (PendingEventID != 0)
    {
        CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);
    }
    else
    {
        PipeName[0] = 0; /* make empty string */
    }

    switch (PendingEventID)
    {
        case CFE_SB_DUP_SUBSCRIP_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_DUP_SUBSCRIP_EID, CFE_EVS_EventType_INFORMATION, CFE_SB_Global.AppId,
                                       "Duplicate Subscription,MsgId 0x%x on %s pipe,app %s",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId), PipeName,
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_SUB_INV_CALLER_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_CALLER_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Subscribe Err:Caller(%s) is not the owner of pipe %lu,Msg=0x%x",
                                       CFE_SB_GetAppTskName(TskId, FullName), CFE_RESOURCEID_TO_ULONG(PipeId),
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;

        case CFE_SB_SUB_INV_PIPE_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_PIPE_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Subscribe Err:Invalid Pipe Id,Msg=0x%x,PipeId=%lu,App %s",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_DEST_BLK_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_DEST_BLK_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Subscribe Err:Request for Destination Blk failed for Msg 0x%x",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;

        case CFE_SB_MAX_DESTS_MET_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_MAX_DESTS_MET_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Subscribe Err:Max Dests(%d)In Use For Msg 0x%x,pipe %s,app %s",
                                       CFE_PLATFORM_SB_MAX_DEST_PER_PKT, (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                                       PipeName, CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_MAX_MSGS_MET_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_MAX_MSGS_MET_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Subscribe Err:Max Msgs(%d)In Use,MsgId 0x%x,pipe %s,app %s",
                                       CFE_PLATFORM_SB_MAX_MSG_IDS, (unsigned int)CFE_SB_MsgIdToValue(MsgId), PipeName,
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_SUB_ARG_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_SUB_ARG_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Subscribe Err:Bad Arg,MsgId 0x%x,PipeId %lu,app %s,scope %d",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                       CFE_SB_GetAppTskName(TskId, FullName), Scope);
            break;

        default:
            break;
    }

    /* If no other event pending, send a debug event indicating success */
    if (Status == CFE_SUCCESS && PendingEventID == 0)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RCVD_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Subscription Rcvd:MsgId 0x%x on PipeId %lu,app %s",
                                   (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                   CFE_SB_GetAppTskName(TskId, FullName));
    }

    if (Status == CFE_SUCCESS && Scope == CFE_SB_MSG_GLOBAL)
    {
        CFE_SB_SendSubscriptionReport(MsgId, PipeId, Quality);
    }

    if (Collisions != 0)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_HASHCOLLISION_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Msg hash collision: MsgId = 0x%x, collisions = %u",
                                   (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)Collisions);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    CFE_ES_AppId_t CallerId;
    int32          Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_MSG_GLOBAL, CallerId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    CFE_ES_AppId_t CallerId;
    int32          Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_MSG_LOCAL, CallerId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId)
{
    int32 Status = 0;

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_MSG_LOCAL, AppId);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint8 Scope, CFE_ES_AppId_t AppId)
{
    int32                  Status;
    CFE_SBR_RouteId_t      RouteId;
    CFE_ES_TaskId_t        TskId;
    CFE_SB_DestinationD_t *DestPtr;
    char                   FullName[(OS_MAX_API_NAME * 2)];
    char                   PipeName[OS_MAX_API_NAME];
    CFE_SB_PipeD_t *       PipeDscPtr;
    uint16                 PendingEventID;

    PendingEventID = 0;
    Status         = CFE_SUCCESS;
    DestPtr        = NULL;

    /* get TaskId of caller for events */
    CFE_ES_GetTaskID(&TskId);

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check that the pipe has been created */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
    {
        PendingEventID = CFE_SB_UNSUB_INV_PIPE_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    /* if given 'AppId' is not the owner of the pipe, send error event and return */
    else if (!CFE_RESOURCEID_TEST_EQUAL(PipeDscPtr->AppId, AppId))
    {
        PendingEventID = CFE_SB_UNSUB_INV_CALLER_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    /* check input parameters */
    else if (!CFE_SB_IsValidMsgId(MsgId) || (Scope > 1))
    {
        PendingEventID = CFE_SB_UNSUB_ARG_ERR_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        /* get routing id */
        RouteId = CFE_SBR_GetRouteId(MsgId);

        /* Status remains CFE_SUCCESS if route is valid or not */
        if (!CFE_SBR_IsValidRouteId(RouteId))
        {
            /* If there are no subscriptions, simply report via event */
            PendingEventID = CFE_SB_UNSUB_NO_SUBS_EID;
        }
        else
        {
            /* Get the destination pointer */
            DestPtr = CFE_SB_GetDestPtr(RouteId, PipeId);

            if (DestPtr != NULL)
            {
                /* match found, remove destination */
                CFE_SB_RemoveDest(RouteId, DestPtr);
            }
            else
            {
                PendingEventID = CFE_SB_UNSUB_NO_SUBS_EID;
            }
        }
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    switch (PendingEventID)
    {
        case CFE_SB_UNSUB_NO_SUBS_EID:
            CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);
            CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_NO_SUBS_EID, CFE_EVS_EventType_INFORMATION, CFE_SB_Global.AppId,
                                       "Unsubscribe Err:No subs for Msg 0x%x on %s,app %s",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId), PipeName,
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_UNSUB_INV_PIPE_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_PIPE_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Unsubscribe Err:Invalid Pipe Id Msg=0x%x,Pipe=%lu,app=%s",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                       CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_UNSUB_INV_CALLER_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_CALLER_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Unsubscribe Err:Caller(%s) is not the owner of pipe %lu,Msg=0x%x",
                                       CFE_SB_GetAppTskName(TskId, FullName), CFE_RESOURCEID_TO_ULONG(PipeId),
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId));
            break;

        case CFE_SB_UNSUB_ARG_ERR_EID:
            CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_ARG_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Unsubscribe Err:Bad Arg,MsgId 0x%x,PipeId %lu,app %s,scope %d",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                       CFE_SB_GetAppTskName(TskId, FullName), (int)Scope);
            break;

        default:
            break;
    }

    /* if no other event pending, send a debug event for successful unsubscribe */
    if (Status == CFE_SUCCESS && PendingEventID == 0)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_REMOVED_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Subscription Removed:Msg 0x%x on pipe %lu,app %s",
                                   (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                   CFE_SB_GetAppTskName(TskId, FullName));
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_ReceiveBuffer(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    CFE_SB_ReceiveTxn_State_t  TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;

    Txn = CFE_SB_ReceiveTxn_Init(&TxnBuf, BufPtr);

    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        CFE_SB_MessageTxn_SetTimeout(Txn, TimeOut);
    }

    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        CFE_SB_ReceiveTxn_SetPipeId(Txn, PipeId);

        /*
         * Set the verify flag true by default -
         * in the default impl verify is a no-op that always succeeds.
         * If an actual implementation is provided, it will be used here.
         */
        CFE_SB_MessageTxn_SetEndpoint(Txn, true);
    }

    if (BufPtr != NULL)
    {
        /*
         * Note - the API should qualify the parameter as "const", but this is
         * kept non-const for backward compatibility.  Callers should never write to
         * the returned buffer, it is const in practice.
         */
        *BufPtr = (CFE_SB_Buffer_t *)CFE_SB_ReceiveTxn_Execute(Txn);
    }

    CFE_SB_MessageTxn_ReportEvents(Txn);

    return CFE_SB_MessageTxn_GetStatus(Txn);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_Buffer_t *CFE_SB_AllocateMessageBuffer(size_t MsgSize)
{
    CFE_ES_AppId_t    AppId;
    char              AppName[OS_MAX_API_NAME] = {""};
    CFE_SB_BufferD_t *BufDscPtr;
    CFE_SB_Buffer_t * BufPtr;
    CFE_Status_t      Status;

    AppId     = CFE_ES_APPID_UNDEFINED;
    BufDscPtr = NULL;
    BufPtr    = NULL;

    Status = CFE_ES_GetAppID(&AppId);

    if (MsgSize > CFE_MISSION_SB_MAX_SB_MSG_SIZE)
    {
        CFE_ES_GetAppName(AppName, AppId, sizeof(AppName));
        CFE_ES_WriteToSysLog("%s %s: Failed, requested size %zu larger than allowed %d\n",
                             AppName, __func__, MsgSize, CFE_MISSION_SB_MAX_SB_MSG_SIZE);
        return NULL;
    }

    /* get callers AppId */
    if (Status == CFE_SUCCESS)
    {
        CFE_SB_LockSharedData(__func__, __LINE__);

        /*
         * All this needs to do is get a descriptor from the pool,
         * and associate that descriptor with this app ID, so it
         * can be freed if this app is deleted before it uses it.
         */
        BufDscPtr = CFE_SB_GetBufferFromPool(MsgSize);

        if (BufDscPtr != NULL)
        {
            /* Track the buffer as a zero-copy assigned to this app ID */
            BufDscPtr->AppId = AppId;
            BufPtr           = &BufDscPtr->Content;
            CFE_SB_TrackingListAdd(&CFE_SB_Global.ZeroCopyList, &BufDscPtr->Link);
        }

        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    if (BufPtr != NULL)
    {
        /*
         * If a buffer was obtained, wipe it now.
         * (This ensures the buffer is fully cleared at least once,
         * no stale data from a prior use of the same memory)
         */
        memset(BufPtr, 0, MsgSize);
    }

    return BufPtr;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_ReleaseMessageBuffer(CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_BufferD_t *BufDscPtr;
    int32             Status;

    /*
     * Sanity Check that the pointers are not NULL
     */
    if (BufPtr == NULL)
    {
        return CFE_SB_BAD_ARGUMENT;
    }

    CFE_SB_LockSharedData(__func__, __LINE__);

    Status = CFE_SB_ZeroCopyBufferValidate(BufPtr, &BufDscPtr);
    if (Status == CFE_SUCCESS)
    {
        /* Clear the ownership app ID and decrement use count (may also free) */
        BufDscPtr->AppId = CFE_ES_APPID_UNDEFINED;
        CFE_SB_DecrBufUseCnt(BufDscPtr);
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_TransmitBuffer(CFE_SB_Buffer_t *BufPtr, bool IsOrigination)
{
    CFE_SB_TransmitTxn_State_t TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;

    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, BufPtr);

    /* In this context, the user should have set the the size and MsgId in the content */
    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        CFE_SB_TransmitTxn_SetupFromMsg(Txn, &BufPtr->Msg);
    }

    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        /* Save passed-in parameters */
        CFE_SB_MessageTxn_SetEndpoint(Txn, IsOrigination);

        CFE_SB_TransmitTxn_Execute(Txn, BufPtr);
    }

    /* send an event for each pipe write error that may have occurred */
    CFE_SB_MessageTxn_ReportEvents(Txn);

    return CFE_SB_MessageTxn_GetStatus(Txn);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_SB_TransmitMsg(const CFE_MSG_Message_t *MsgPtr, bool IsOrigination)
{
    CFE_SB_TransmitTxn_State_t TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;
    CFE_SB_Buffer_t *          BufPtr;

    BufPtr = NULL;
    Txn    = CFE_SB_TransmitTxn_Init(&TxnBuf, MsgPtr);

    /* In this context, the user should have set the the size and MsgId in the content */
    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        CFE_SB_TransmitTxn_SetupFromMsg(Txn, MsgPtr);
    }

    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        /* Get buffer - note this pre-initializes the returned buffer with
         * a use count of 1, which refers to this task as it fills the buffer. */
        BufPtr = CFE_SB_AllocateMessageBuffer(CFE_SB_MessageTxn_GetContentSize(Txn));
        if (BufPtr == NULL)
        {
            CFE_SB_MessageTxn_SetEventAndStatus(Txn, CFE_SB_GET_BUF_ERR_EID, CFE_SB_BUF_ALOC_ERR);
        }
    }

    /*
     * If a buffer was obtained above, then copy the content into it
     * and broadcast it to all subscribers in the route.
     *
     * Note - if there is no route / no subscribers, the "Status" will
     * be CFE_SUCCESS because CFE_SB_TransmitMsgValidate() succeeded,
     * but there will be no buffer because CFE_SBR_IsValidRouteId() returned
     * false.
     *
     * But if the descriptor is non-null it means the message is valid and
     * there is a route to send it to.
     */
    if (CFE_SB_MessageTxn_IsOK(Txn))
    {
        /* Copy actual message content into buffer */
        memcpy(&BufPtr->Msg, MsgPtr, CFE_SB_MessageTxn_GetContentSize(Txn));

        /* Save passed-in parameters */
        CFE_SB_MessageTxn_SetEndpoint(Txn, IsOrigination);

        CFE_SB_TransmitTxn_Execute(Txn, BufPtr);

        /*
         * The broadcast function consumes the buffer, so it should not be
         * accessed in this function anymore
         */
        BufPtr = NULL;
    }

    /* send an event for each pipe write error that may have occurred */
    CFE_SB_MessageTxn_ReportEvents(Txn);

    return CFE_SB_MessageTxn_GetStatus(Txn);
}
