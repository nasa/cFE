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
**                 It was used in earlier cFS implementaions and is defined here for historical reference
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

/*
 *  Function: CFE_SB_PipeId_ToIndex - See API and header file for details
 */
CFE_Status_t CFE_SB_PipeId_ToIndex(CFE_SB_PipeId_t PipeID, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(PipeID), CFE_SB_PIPEID_BASE, CFE_PLATFORM_SB_MAX_PIPES, Idx);
}

/*
 * Function: CFE_SB_CreatePipe - See API and header file for details
 */
int32 CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
    CFE_ES_AppId_t   AppId;
    CFE_ES_TaskId_t  TskId;
    osal_id_t        SysQueueId;
    int32            Status;
    CFE_SB_PipeD_t * PipeDscPtr;
    CFE_ResourceId_t PendingPipeId = CFE_RESOURCEID_UNDEFINED;
    uint16           PendingEventId;
    char             FullName[(OS_MAX_API_NAME * 2)];

    Status         = CFE_SUCCESS;
    SysQueueId     = OS_OBJECT_ID_UNDEFINED;
    PendingEventId = 0;
    PipeDscPtr     = NULL;

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
    if ((PipeIdPtr == NULL) || (Depth > OS_QUEUE_MAX_DEPTH) || (Depth == 0))
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
        Status = OS_QueueCreate(&SysQueueId, PipeName, Depth, sizeof(CFE_SB_BufferD_t *), 0);
        if (Status == OS_SUCCESS)
        {
            /* just translate the RC to CFE */
            Status = CFE_SUCCESS;
        }
        else
        {
            if (Status == OS_ERR_NAME_TAKEN)
            {
                PendingEventId = CFE_SB_CR_PIPE_NAME_TAKEN_EID;
            }
            else if (Status == OS_ERR_NO_FREE_IDS)
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
        } /* end if */
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
    if (Status == CFE_SUCCESS)
    {
        /* send debug event */
        CFE_EVS_SendEventWithAppID(CFE_SB_PIPE_ADDED_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Pipe Created:name %s,id %d,app %s", PipeName,
                                   (int)CFE_ResourceId_ToInteger(PendingPipeId), CFE_SB_GetAppTskName(TskId, FullName));

        /* give the pipe handle to the caller */
        *PipeIdPtr = CFE_SB_PIPEID_C(PendingPipeId);
    }
    else
    {
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
                                           "CreatePipeErr:OS_QueueCreate returned %d,app %s", (int)Status,
                                           CFE_SB_GetAppTskName(TskId, FullName));
                break;
        }
    }

    return Status;

} /* end CFE_SB_CreatePipe */

/*
 *  Function: CFE_SB_DeletePipe - See API and header file for details
 */
int32 CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    CFE_ES_AppId_t CallerId;
    int32          Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_DeletePipeFull(PipeId, CallerId);

    return Status;

} /* end CFE_SB_DeletePipe */

/******************************************************************************
**  Function:  CFE_SB_DeletePipeWithAppId()
**
**  Purpose:
**
**
**  Arguments:
**    PipeId - The ID of the pipe to delete.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId)
{
    int32 Status = 0;

    Status = CFE_SB_DeletePipeFull(PipeId, AppId);

    return Status;

} /* end CFE_SB_DeletePipeWithAppId */

/******************************************************************************
 * Local callback helper for deleting a pipe from a route
 */
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

/******************************************************************************
**  Function:  CFE_SB_DeletePipeFull()
**
**  Purpose:
**    Will unsubscribe to all routes associated with the given pipe id, then remove
**    pipe from the pipe table.
**
**  NOTE:This function cannot be called directly, it would not be semaphore protected
**
**  Arguments:
**    PipeId - The ID of the pipe to delete.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
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
    else if (PendingEventID != 0)
    {
        CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter++;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

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
    else
    {
        /* get TaskId and name of caller for events */
        CFE_ES_GetTaskID(&TskId);
        CFE_SB_GetAppTskName(TskId, FullName);

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
        }
    }

    return Status;

} /* end CFE_SB_DeletePipeFull */

/*
 *  Function: CFE_SB_SetPipeOpts - See API and header file for details
 */
int32 CFE_SB_SetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 Opts)
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

    /* Send events after unlocking SB */
    if (Status == CFE_SUCCESS)
    {
        /* get AppID of caller for events */
        CFE_ES_GetAppName(FullName, AppID, sizeof(FullName));

        CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Pipe opts set:id %lu,owner %s, opts=0x%02x", CFE_RESOURCEID_TO_ULONG(PipeId),
                                   FullName, (unsigned int)Opts);
    }
    else
    {
        /* get TaskId of caller for events */
        CFE_ES_GetTaskID(&TskId);

        switch (PendingEventID)
        {
            case CFE_SB_SETPIPEOPTS_ID_ERR_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_ID_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Pipe Opts Error:Bad Argument,PipedId %lu,Requestor %s",
                                           CFE_RESOURCEID_TO_ULONG(PipeId), CFE_SB_GetAppTskName(TskId, FullName));
                break;
            case CFE_SB_SETPIPEOPTS_OWNER_ERR_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_OWNER_ERR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_SB_Global.AppId,
                                           "Pipe Opts Set Error: Caller(%s) is not the owner of pipe %lu",
                                           CFE_SB_GetAppTskName(TskId, FullName), CFE_RESOURCEID_TO_ULONG(PipeId));
                break;
        }
    }

    return Status;
} /* end CFE_SB_SetPipeOpts */

/*
 *  Function:  CFE_SB_GetPipeOpts - See API and header file for details
 */
int32 CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptsPtr)
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

    /* Send events after unlocking SB */
    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEOPTS_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Pipe opts get:id %lu, opts=0x%02x", CFE_RESOURCEID_TO_ULONG(PipeId),
                                   (unsigned int)*OptsPtr);
    }
    else
    {
        /* get TaskId of caller for events */
        CFE_ES_GetTaskID(&TskId);

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
        }
    }

    return Status;
} /* end CFE_SB_GetPipeOpts */

/*
 *  Function:  CFE_SB_GetPipeName - See API and header file for details
 */
int32 CFE_SB_GetPipeName(char *PipeNameBuf, size_t PipeNameSize, CFE_SB_PipeId_t PipeId)
{
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
            Status = OS_GetResourceName(SysQueueId, PipeNameBuf, PipeNameSize);

            if (Status == OS_SUCCESS)
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
    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "GetPipeName name=%s id=%lu", PipeNameBuf, CFE_RESOURCEID_TO_ULONG(PipeId));
    }
    else
    {
        CFE_ES_GetTaskID(&TskId);

        switch (PendingEventID)
        {
            case CFE_SB_GETPIPENAME_NULL_PTR_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_NULL_PTR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_SB_Global.AppId, "Pipe Name Error:NullPtr,Requestor %s",
                                           CFE_SB_GetAppTskName(TskId, FullName));
                break;
            case CFE_SB_GETPIPENAME_ID_ERR_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_ID_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Pipe Id Error:Bad Argument,Id=%lu,Requestor %s",
                                           CFE_RESOURCEID_TO_ULONG(PipeId), CFE_SB_GetAppTskName(TskId, FullName));
                break;
        }

        if (PipeNameBuf != NULL && PipeNameSize > 0)
        {
            memset(PipeNameBuf, 0, PipeNameSize);
        }
    }

    return Status;

} /* end CFE_SB_GetPipeName */

/*
 *  Function:  CFE_SB_GetPipeIdByName - See API and header file for details
 */
int32 CFE_SB_GetPipeIdByName(CFE_SB_PipeId_t *PipeIdPtr, const char *PipeName)
{
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
        Status = OS_QueueGetIdByName(&SysQueueId, PipeName);
        if (Status == OS_SUCCESS)
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
            } /* end if */

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
    if (Status == CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "PipeIdByName name=%s id=%lu", PipeName, CFE_RESOURCEID_TO_ULONG(*PipeIdPtr));
    }
    else
    {
        /* get TaskId of caller for events */
        CFE_ES_GetTaskID(&TskId);

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
        }
    }

    return Status;

} /* end CFE_SB_GetPipeIdByName */

/*
 * Function: CFE_SB_SubscribeEx - See API and header file for details
 */
int32 CFE_SB_SubscribeEx(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId, PipeId, Quality, MsgLim, (uint8)CFE_SB_MSG_GLOBAL);

} /* end CFE_SB_SubscribeEx */

/*
 * Function: CFE_SB_SubscribeLocal - See API and header file for details
 */
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint16 MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId, PipeId, CFE_SB_DEFAULT_QOS, MsgLim, (uint8)CFE_SB_MSG_LOCAL);

} /* end CFE_SB_SubscribeLocal */

/*
 * Function: CFE_SB_Subscribe - See API and header file for details
 */
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    return CFE_SB_SubscribeFull(MsgId, PipeId, CFE_SB_DEFAULT_QOS, (uint16)CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT,
                                (uint8)CFE_SB_MSG_GLOBAL);

} /* end CFE_SB_Subscribe */

/******************************************************************************
** Name:    CFE_SB_SubscribeFull
**
** Purpose: CFE Internal API used to subscribe to a message. Contains an input
**          parameter for all possible subscription choices. This function is
**          called by CFE_SB_SubscribeEx, CFE_SB_Subscribe and
**          CFE_SB_SubscribeLocal.
**
** Assumptions, External Events, and Notes:
**          Has the same typedef as the message Id
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId - Mission unique identifier for the message being requested
**          PipeId  - The Pipe ID to send the message to
**          Quality - Quality of Service (Qos) - priority and reliability
**          MsgLim  - Max number of messages, with this MsgId, allowed on the
**                    pipe at any time.
**          Scope   - Local subscription or broadcasted to peers
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
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
                } /* end if */
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

    /* Send events now */
    if (PendingEventID != 0)
    {
        CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);

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
                                           CFE_PLATFORM_SB_MAX_MSG_IDS, (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                                           PipeName, CFE_SB_GetAppTskName(TskId, FullName));
                break;

            case CFE_SB_SUB_ARG_ERR_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_SUB_ARG_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Subscribe Err:Bad Arg,MsgId 0x%x,PipeId %lu,app %s,scope %d",
                                           (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                           CFE_SB_GetAppTskName(TskId, FullName), Scope);
                break;
        }
    }
    else if (Status == CFE_SUCCESS)
    {
        /* If no other event pending, send a debug event indicating success */
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

} /* end CFE_SB_SubscribeFull */

/*
 * Function: CFE_SB_Unsubscribe - See API and header file for details
 */
int32 CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    CFE_ES_AppId_t CallerId;
    int32          Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_MSG_GLOBAL, CallerId);

    return Status;

} /* end CFE_SB_Unsubscribe */

/*
 * Function: CFE_SB_UnsubscribeLocal - See API and header file for details
 */
int32 CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    CFE_ES_AppId_t CallerId;
    int32          Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_MSG_LOCAL, CallerId);

    return Status;

} /* end CFE_SB_UnsubscribeLocal */

/******************************************************************************
** Name:    CFE_SB_UnsubscribeAppId
**
** Purpose: CFE Internal API intented to be called by CFE_ES when an applications
**          SB resources need to be freed. The regular unsibscribe api won't work
**          because it does a check to ensure the caller is the owner of the pipe.
**
** Date Written:
**          03/15/2007
**
** Input Arguments:
**          MsgId
**          PipeId
**          AppId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_ES_AppId_t AppId)
{
    int32 Status = 0;

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_MSG_LOCAL, AppId);

    return Status;

} /* end CFE_SB_UnsubscribeWithAppId */

/******************************************************************************
** Name:    CFE_SB_UnsubscribeFull
**
** Purpose: CFE Internal API used to unsubscribe to a message.
**
** Assumptions, External Events, and Notes:
**
**
** Notes:This function cannot be called directly,it would not be semaphore protected.
**       Also,if more than one subscription is found, this function will remove all
**       entries that match.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId
**          PipeId
**          Scope
**          AppId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
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

        /* if there have never been subscriptions for this message id... */
        if (!CFE_SBR_IsValidRouteId(RouteId))
        {
            PendingEventID = CFE_SB_UNSUB_NO_SUBS_EID;
            /* Status stays CFE_SUCCESS here */
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
    } /* end if */

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    if (PendingEventID != 0)
    {
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
                                           "UnSubscribe Err:Bad Arg,MsgId 0x%x,PipeId %lu,app %s,scope %d",
                                           (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                           CFE_SB_GetAppTskName(TskId, FullName), (int)Scope);
                break;
        }
    }
    else if (Status == CFE_SUCCESS)
    {
        /* if no other event pending, send a debug event for successful unsubscribe */
        CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_REMOVED_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Subscription Removed:Msg 0x%x on pipe %lu,app %s",
                                   (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                   CFE_SB_GetAppTskName(TskId, FullName));
    }

    return Status;
} /* end CFE_SB_UnsubscribeFull */

/*
 * Function CFE_SB_TransmitMsg - See API and header file for details
 */
int32 CFE_SB_TransmitMsg(CFE_MSG_Message_t *MsgPtr, bool IncrementSequenceCount)
{
    int32             Status;
    CFE_MSG_Size_t    Size  = 0;
    CFE_SB_MsgId_t    MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_ES_TaskId_t   TskId;
    char              FullName[(OS_MAX_API_NAME * 2)];
    CFE_SB_BufferD_t *BufDscPtr;
    CFE_SBR_RouteId_t RouteId;
    uint16            PendingEventID;

    PendingEventID = 0;
    BufDscPtr      = NULL;
    RouteId        = CFE_SBR_INVALID_ROUTE_ID;

    Status = CFE_SB_TransmitMsgValidate(MsgPtr, &MsgId, &Size, &RouteId);

    CFE_SB_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS && CFE_SBR_IsValidRouteId(RouteId))
    {
        /* Get buffer - note this pre-initializes the returned buffer with
         * a use count of 1, which refers to this task as it fills the buffer. */
        BufDscPtr = CFE_SB_GetBufferFromPool(Size);
        if (BufDscPtr == NULL)
        {
            PendingEventID = CFE_SB_GET_BUF_ERR_EID;
            Status         = CFE_SB_BUF_ALOC_ERR;
        }
    }

    /*
     * Increment the MsgSendErrorCounter only if there was a real error,
     * such as a validation issue or failure to allocate a buffer.
     *
     * (This should NOT be done if simply no route)
     */
    if (Status != CFE_SUCCESS)
    {
        CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter++;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /*
     * If a buffer was obtained above, then copy the content into it
     * and broadcast it to all subscribers in the route.
     *
     * Note - if there is no route / no subscribers, the "Status" will
     * be CFE_SUCCESS because CFE_SB_TransmitMsgValidate() succeeded,
     * but there will be no buffer because CFE_SBR_IsValidRouteId() returned
     * false.
     *
     * But if the desciptor is non-null it means the message is valid and
     * there is a route to send it to.
     */
    if (BufDscPtr != NULL)
    {
        /* Copy actual message content into buffer and set its metadata */
        memcpy(&BufDscPtr->Content, MsgPtr, Size);
        BufDscPtr->MsgId        = MsgId;
        BufDscPtr->ContentSize  = Size;
        BufDscPtr->AutoSequence = IncrementSequenceCount;
        CFE_MSG_GetType(MsgPtr, &BufDscPtr->ContentType);

        /*
         * This routine will use best-effort to send to all subscribers,
         * increment the buffer use count for every successful delivery,
         * and send an event/increment counter for any unsucessful delivery.
         */
        CFE_SB_BroadcastBufferToRoute(BufDscPtr, RouteId);

        /*
         * The broadcast function consumes the buffer, so it should not be
         * accessed in this function anymore
         */
        BufDscPtr = NULL;
    }

    if (PendingEventID == CFE_SB_GET_BUF_ERR_EID)
    {
        /* Get task id for events and Sender Info*/
        CFE_ES_GetTaskID(&TskId);

        if (CFE_SB_RequestToSendEvent(TskId, CFE_SB_GET_BUF_ERR_EID_BIT) == CFE_SB_GRANTED)
        {
            CFE_EVS_SendEventWithAppID(CFE_SB_GET_BUF_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Send Err:Request for Buffer Failed. MsgId 0x%x,app %s,size %d",
                                       (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_SB_GetAppTskName(TskId, FullName),
                                       (int)Size);

            /* clear the bit so the task may send this event again */
            CFE_SB_FinishSendEvent(TskId, CFE_SB_GET_BUF_ERR_EID_BIT);
        }
    }

    return Status;
}

/*****************************************************************************/
/**
 * \brief Internal routine to validate a transmit message before sending
 *
 * \param[in]  MsgPtr     Pointer to the message to validate
 * \param[out] MsgIdPtr   Message Id of message
 * \param[out] SizePtr    Size of message
 * \param[out] RouteIdPtr Route ID of the message (invalid if none)
 */
int32 CFE_SB_TransmitMsgValidate(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t *MsgIdPtr, CFE_MSG_Size_t *SizePtr,
                                 CFE_SBR_RouteId_t *RouteIdPtr)
{
    CFE_ES_TaskId_t TskId;
    char            FullName[(OS_MAX_API_NAME * 2)];
    uint16          PendingEventID;
    int32           Status;

    PendingEventID = 0;
    Status         = CFE_SUCCESS;

    /* check input parameter */
    if (MsgPtr == NULL)
    {
        PendingEventID = CFE_SB_SEND_BAD_ARG_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_MSG_GetMsgId(MsgPtr, MsgIdPtr);

        /* validate the msgid in the message */
        if (!CFE_SB_IsValidMsgId(*MsgIdPtr))
        {
            PendingEventID = CFE_SB_SEND_INV_MSGID_EID;
            Status         = CFE_SB_BAD_ARGUMENT;
        }
    }

    if (Status == CFE_SUCCESS)
    {
        CFE_MSG_GetSize(MsgPtr, SizePtr);

        /* Verify the size of the pkt is < or = the mission defined max */
        if (*SizePtr > CFE_MISSION_SB_MAX_SB_MSG_SIZE)
        {
            PendingEventID = CFE_SB_MSG_TOO_BIG_EID;
            Status         = CFE_SB_MSG_TOO_BIG;
        } /* end if */
    }

    if (Status == CFE_SUCCESS)
    {
        /* check the route, which should be done while locked */
        CFE_SB_LockSharedData(__func__, __LINE__);

        /* Get the routing id */
        *RouteIdPtr = CFE_SBR_GetRouteId(*MsgIdPtr);

        /* if there have been no subscriptions for this pkt, */
        /* increment the dropped pkt cnt, send event and return success */
        if (!CFE_SBR_IsValidRouteId(*RouteIdPtr))
        {
            CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter++;
            PendingEventID = CFE_SB_SEND_NO_SUBS_EID;
        }

        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    if (PendingEventID != 0)
    {
        /* get task id for events */
        CFE_ES_GetTaskID(&TskId);

        switch (PendingEventID)
        {
            case CFE_SB_SEND_BAD_ARG_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_SEND_BAD_ARG_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Send Err:Bad input argument,Arg 0x%lx,App %s", (unsigned long)MsgPtr,
                                           CFE_SB_GetAppTskName(TskId, FullName));
                break;

            case CFE_SB_SEND_INV_MSGID_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_SEND_INV_MSGID_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Send Err:Invalid MsgId(0x%x)in msg,App %s",
                                           (unsigned int)CFE_SB_MsgIdToValue(*MsgIdPtr),
                                           CFE_SB_GetAppTskName(TskId, FullName));
                break;

            case CFE_SB_MSG_TOO_BIG_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_MSG_TOO_BIG_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Send Err:Msg Too Big MsgId=0x%x,app=%s,size=%d,MaxSz=%d",
                                           (unsigned int)CFE_SB_MsgIdToValue(*MsgIdPtr),
                                           CFE_SB_GetAppTskName(TskId, FullName), (int)*SizePtr,
                                           CFE_MISSION_SB_MAX_SB_MSG_SIZE);
                break;

            case CFE_SB_SEND_NO_SUBS_EID:
                /* Determine if event can be sent without causing recursive event problem */
                if (CFE_SB_RequestToSendEvent(TskId, CFE_SB_SEND_NO_SUBS_EID_BIT) == CFE_SB_GRANTED)
                {
                    CFE_EVS_SendEventWithAppID(CFE_SB_SEND_NO_SUBS_EID, CFE_EVS_EventType_INFORMATION,
                                               CFE_SB_Global.AppId, "No subscribers for MsgId 0x%x,sender %s",
                                               (unsigned int)CFE_SB_MsgIdToValue(*MsgIdPtr),
                                               CFE_SB_GetAppTskName(TskId, FullName));

                    /* clear the bit so the task may send this event again */
                    CFE_SB_FinishSendEvent(TskId, CFE_SB_SEND_NO_SUBS_EID_BIT);
                } /* end if */
                break;
        }

    } /* end if */

    return Status;
}

/*****************************************************************************/
/**
 * \brief Internal routine implements full send logic
 *
 * \param[in] BufDscPtr Pointer to the buffer description from the memory pool,
 *                      released prior to return
 * \param[in] RouteId   Route to send to
 */
void CFE_SB_BroadcastBufferToRoute(CFE_SB_BufferD_t *BufDscPtr, CFE_SBR_RouteId_t RouteId)
{
    CFE_ES_AppId_t         AppId;
    CFE_ES_TaskId_t        TskId;
    CFE_SB_DestinationD_t *DestPtr;
    CFE_SB_PipeD_t *       PipeDscPtr;
    CFE_SB_EventBuf_t      SBSndErr;
    int32                  Status;
    uint32                 i;
    char                   FullName[(OS_MAX_API_NAME * 2)];
    char                   PipeName[OS_MAX_API_NAME];

    SBSndErr.EvtsToSnd = 0;

    /* get app id for loopback testing */
    CFE_ES_GetAppID(&AppId);

    /* get task id for events and Sender Info*/
    CFE_ES_GetTaskID(&TskId);

    /* take semaphore to prevent a task switch during processing */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* For an invalid route / no subsribers this whole logic can be skipped */
    if (CFE_SBR_IsValidRouteId(RouteId))
    {
        /* Set the seq count if requested (while locked) before actually sending */
        /* For some reason this is only done for TLM types (historical, TBD) */
        if (BufDscPtr->AutoSequence && BufDscPtr->ContentType == CFE_MSG_Type_Tlm)
        {
            CFE_SBR_IncrementSequenceCounter(RouteId);

            /* Write the sequence into the message header itself (overwrites whatever was there) */
            CFE_MSG_SetSequenceCount(&BufDscPtr->Content.Msg, CFE_SBR_GetSequenceCounter(RouteId));
        }

        /* Send the packet to all destinations  */
        for (DestPtr = CFE_SBR_GetDestListHeadPtr(RouteId); DestPtr != NULL; DestPtr = DestPtr->Next)
        {
            if (DestPtr->Active == CFE_SB_ACTIVE) /* destination is active */
            {
                PipeDscPtr = CFE_SB_LocatePipeDescByID(DestPtr->PipeId);
            }
            else
            {
                PipeDscPtr = NULL;
            }

            if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, DestPtr->PipeId))
            {
                continue;
            }

            if ((PipeDscPtr->Opts & CFE_SB_PIPEOPTS_IGNOREMINE) != 0 &&
                CFE_RESOURCEID_TEST_EQUAL(PipeDscPtr->AppId, AppId))
            {
                continue;
            } /* end if */

            /* if Msg limit exceeded, log event, increment counter */
            /* and go to next destination */
            if (DestPtr->BuffCount >= DestPtr->MsgId2PipeLim)
            {
                SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
                SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_MSGID_LIM_ERR_EID;
                SBSndErr.EvtsToSnd++;
                CFE_SB_Global.HKTlmMsg.Payload.MsgLimitErrorCounter++;
                PipeDscPtr->SendErrors++;

                continue;
            } /* end if */

            /*
            ** Write the buffer descriptor to the queue of the pipe.  If the write
            ** failed, log info and increment the pipe's error counter.
            */
            Status = OS_QueuePut(PipeDscPtr->SysQueueId, &BufDscPtr, sizeof(BufDscPtr), 0);

            if (Status == OS_SUCCESS)
            {
                /* The queue now holds a ref to the buffer, so increment its ref count. */
                CFE_SB_IncrBufUseCnt(BufDscPtr);

                DestPtr->BuffCount++; /* used for checking MsgId2PipeLimit */
                DestPtr->DestCnt++;   /* used for statistics */
                ++PipeDscPtr->CurrentQueueDepth;
                if (PipeDscPtr->CurrentQueueDepth >= PipeDscPtr->PeakQueueDepth)
                {
                    PipeDscPtr->PeakQueueDepth = PipeDscPtr->CurrentQueueDepth;
                }
            }
            else if (Status == OS_QUEUE_FULL)
            {
                SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
                SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_Q_FULL_ERR_EID;
                SBSndErr.EvtsToSnd++;
                CFE_SB_Global.HKTlmMsg.Payload.PipeOverflowErrorCounter++;
                PipeDscPtr->SendErrors++;
            }
            else
            {
                /* Unexpected error while writing to queue. */
                SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
                SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_Q_WR_ERR_EID;
                SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].ErrStat = Status;
                SBSndErr.EvtsToSnd++;
                CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter++;
                PipeDscPtr->SendErrors++;
            } /*end if */

        } /* end loop over destinations */
    }

    /*
     * If any specific delivery issues occured, also increment the
     * general error count before releasing the lock.
     */
    if (SBSndErr.EvtsToSnd > 0)
    {
        CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter++;
    }

    /*
     * Remove this from whatever list it was in
     *
     * If it was a singleton/new buffer this has no effect.
     * If it was a zero-copy buffer this removes it from the ZeroCopyList.
     */
    CFE_SB_TrackingListRemove(&BufDscPtr->Link);

    /* clear the AppID field in case it was a zero copy buffer,
     * as it is no longer owned by that app after broadcasting */
    BufDscPtr->AppId = CFE_ES_APPID_UNDEFINED;

    /* track the buffer as an in-transit message */
    CFE_SB_TrackingListAdd(&CFE_SB_Global.InTransitList, &BufDscPtr->Link);

    /*
    ** Decrement the buffer UseCount and free buffer if cnt=0. This decrement is done
    ** because the use cnt is initialized to 1 in CFE_SB_GetBufferFromPool.
    ** Initializing the count to 1 (as opposed to zero) and decrementing it here are
    ** done to ensure the buffer gets released when there are destinations that have
    ** been disabled via ground command.
    */
    CFE_SB_DecrBufUseCnt(BufDscPtr);

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* send an event for each pipe write error that may have occurred */
    for (i = 0; i < SBSndErr.EvtsToSnd; i++)
    {
        if (SBSndErr.EvtBuf[i].EventId == CFE_SB_MSGID_LIM_ERR_EID)
        {

            /* Determine if event can be sent without causing recursive event problem */
            if (CFE_SB_RequestToSendEvent(TskId, CFE_SB_MSGID_LIM_ERR_EID_BIT) == CFE_SB_GRANTED)
            {

                CFE_SB_GetPipeName(PipeName, sizeof(PipeName), SBSndErr.EvtBuf[i].PipeId);

                CFE_ES_PerfLogEntry(CFE_MISSION_SB_MSG_LIM_PERF_ID);
                CFE_ES_PerfLogExit(CFE_MISSION_SB_MSG_LIM_PERF_ID);

                CFE_EVS_SendEventWithAppID(CFE_SB_MSGID_LIM_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Msg Limit Err,MsgId 0x%x,pipe %s,sender %s",
                                           (unsigned int)CFE_SB_MsgIdToValue(BufDscPtr->MsgId), PipeName,
                                           CFE_SB_GetAppTskName(TskId, FullName));

                /* clear the bit so the task may send this event again */
                CFE_SB_FinishSendEvent(TskId, CFE_SB_MSGID_LIM_ERR_EID_BIT);
            } /* end if */
        }
        else if (SBSndErr.EvtBuf[i].EventId == CFE_SB_Q_FULL_ERR_EID)
        {

            /* Determine if event can be sent without causing recursive event problem */
            if (CFE_SB_RequestToSendEvent(TskId, CFE_SB_Q_FULL_ERR_EID_BIT) == CFE_SB_GRANTED)
            {

                CFE_SB_GetPipeName(PipeName, sizeof(PipeName), SBSndErr.EvtBuf[i].PipeId);

                CFE_ES_PerfLogEntry(CFE_MISSION_SB_PIPE_OFLOW_PERF_ID);
                CFE_ES_PerfLogExit(CFE_MISSION_SB_PIPE_OFLOW_PERF_ID);

                CFE_EVS_SendEventWithAppID(CFE_SB_Q_FULL_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Pipe Overflow,MsgId 0x%x,pipe %s,sender %s",
                                           (unsigned int)CFE_SB_MsgIdToValue(BufDscPtr->MsgId), PipeName,
                                           CFE_SB_GetAppTskName(TskId, FullName));

                /* clear the bit so the task may send this event again */
                CFE_SB_FinishSendEvent(TskId, CFE_SB_Q_FULL_ERR_EID_BIT);
            } /* end if */
        }
        else
        {

            /* Determine if event can be sent without causing recursive event problem */
            if (CFE_SB_RequestToSendEvent(TskId, CFE_SB_Q_WR_ERR_EID_BIT) == CFE_SB_GRANTED)
            {

                CFE_SB_GetPipeName(PipeName, sizeof(PipeName), SBSndErr.EvtBuf[i].PipeId);

                CFE_EVS_SendEventWithAppID(CFE_SB_Q_WR_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Pipe Write Err,MsgId 0x%x,pipe %s,sender %s,stat 0x%x",
                                           (unsigned int)CFE_SB_MsgIdToValue(BufDscPtr->MsgId), PipeName,
                                           CFE_SB_GetAppTskName(TskId, FullName),
                                           (unsigned int)SBSndErr.EvtBuf[i].ErrStat);

                /* clear the bit so the task may send this event again */
                CFE_SB_FinishSendEvent(TskId, CFE_SB_Q_WR_ERR_EID_BIT);
            } /* end if */

        } /* end if */
    }
}

/*
 * Function: CFE_SB_ReceiveBuffer - See API and header file for details
 */
int32 CFE_SB_ReceiveBuffer(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    int32                  Status;
    int32                  RcvStatus;
    CFE_SB_BufferD_t *     BufDscPtr;
    size_t                 BufDscSize;
    CFE_SB_PipeD_t *       PipeDscPtr;
    CFE_SB_DestinationD_t *DestPtr;
    CFE_SBR_RouteId_t      RouteId;
    CFE_ES_TaskId_t        TskId;
    uint16                 PendingEventID;
    osal_id_t              SysQueueId;
    int32                  SysTimeout;
    char                   FullName[(OS_MAX_API_NAME * 2)];

    PendingEventID = 0;
    Status         = CFE_SUCCESS;
    SysTimeout     = OS_PEND;
    SysQueueId     = OS_OBJECT_ID_UNDEFINED;
    PipeDscPtr     = NULL;
    BufDscPtr      = NULL;
    DestPtr        = NULL;
    BufDscSize     = 0;
    RcvStatus      = OS_SUCCESS;

    /*
     * Check input args and see if any are bad, which require
     * a "BAD_ARG_EID" to be generated.
     *
     * Also translate the timeout here.  Timeouts greater than 0
     * may be passed to OSAL directly, but the two fixed constants
     * CFE_SB_PEND_FOREVER and CFE_SB_POLL are checked explicitly,
     * to maintain API independence - even though the values are
     * currently defined the same.
     */

    if (BufPtr == NULL)
    {
        PendingEventID = CFE_SB_RCV_BAD_ARG_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }
    else if (TimeOut > 0)
    {
        /* time outs greater than 0 can be passed to OSAL directly */
        SysTimeout = TimeOut;
    }
    else if (TimeOut == CFE_SB_POLL)
    {
        SysTimeout = OS_CHECK;
    }
    else if (TimeOut != CFE_SB_PEND_FOREVER)
    {
        /* any other timeout value is invalid */
        PendingEventID = CFE_SB_RCV_BAD_ARG_EID;
        Status         = CFE_SB_BAD_ARGUMENT;
    }

    /* If OK, then lock and pull relevent info from Pipe Descriptor */
    if (Status == CFE_SUCCESS)
    {
        CFE_SB_LockSharedData(__func__, __LINE__);

        PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);

        /* If the pipe does not exist or PipeId is out of range... */
        if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
        {
            PendingEventID = CFE_SB_BAD_PIPEID_EID;
            Status         = CFE_SB_BAD_ARGUMENT;
        }
        else
        {
            /* Grab the queue ID */
            SysQueueId = PipeDscPtr->SysQueueId;

            /*
             * Un-reference any previous buffer from the last call.
             *
             * NOTE: This is historical behavior where apps call CFE_SB_ReceiveBuffer()
             * in the loop within the app's main task.  There is currently no separate
             * API to "free" or unreference a buffer that was returned from SB.
             *
             * Instead, each time this function is invoked, it is implicitly interpreted
             * as an indication that the caller is done with the previous buffer.
             *
             * Unfortunately this prevents pipe IDs from being serviced/shared across
             * multiple child tasks in a worker pattern design.  This may be changed
             * in a future version of CFE to decouple these actions, to allow for
             * multiple workers to service the same pipe.
             */
            if (PipeDscPtr->LastBuffer != NULL)
            {
                /* Decrement the Buffer Use Count, which will Free buffer if it becomes 0 */
                CFE_SB_DecrBufUseCnt(PipeDscPtr->LastBuffer);
                PipeDscPtr->LastBuffer = NULL;
            }
        }

        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    /*
     * If everything validated, then proceed to get a buffer from the queue.
     * This must be done OUTSIDE the SB lock, as this call likely blocks.
     */
    if (Status == CFE_SUCCESS)
    {
        /* Read the buffer descriptor address from the queue.  */
        RcvStatus = OS_QueueGet(SysQueueId, &BufDscPtr, sizeof(BufDscPtr), &BufDscSize, SysTimeout);

        /*
         * translate the return value -
         *
         * CFE functions have their own set of RC values should not directly return OSAL codes
         * The size should always match.  If it does not, then generate CFE_SB_Q_RD_ERR_EID.
         */
        if (RcvStatus == OS_SUCCESS && BufDscPtr != NULL && BufDscSize == sizeof(BufDscPtr))
        {
            /* Pass through */
        }
        else if (RcvStatus == OS_QUEUE_EMPTY)
        {
            /* normal if using CFE_SB_POLL */
            Status = CFE_SB_NO_MESSAGE;
        }
        else if (RcvStatus == OS_QUEUE_TIMEOUT)
        {
            /* normal if using a nonzero timeout */
            Status = CFE_SB_TIME_OUT;
        }
        else
        {
            /* off-nominal condition, report an error event */
            PendingEventID = CFE_SB_Q_RD_ERR_EID;
            Status         = CFE_SB_PIPE_RD_ERR;
        }
    }

    /* Now re-lock to store the buffer in the pipe descriptor */
    CFE_SB_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        /*
         * NOTE: This uses the same PipeDscPtr that was found earlier.
         * Technically it is possible that the pipe was changed between now and then,
         * but the current PipeID definition doesn't really allow this to be detected.
         */
        if (CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
        {
            /*
            ** Load the pipe tables 'CurrentBuff' with the buffer descriptor
            ** ptr corresponding to the message just read. This is done so that
            ** the buffer can be released on the next receive call for this pipe.
            **
            ** This counts as a new reference as it is being stored in the PipeDsc
            */
            CFE_SB_IncrBufUseCnt(BufDscPtr);
            PipeDscPtr->LastBuffer = BufDscPtr;

            /*
             * Also set the Receivers pointer to the address of the actual message
             * (currently this is "borrowing" the ref above, not its own ref)
             */
            *BufPtr = &BufDscPtr->Content;

            /* get pointer to destination to be used in decrementing msg limit cnt*/
            RouteId = CFE_SBR_GetRouteId(BufDscPtr->MsgId);
            DestPtr = CFE_SB_GetDestPtr(RouteId, PipeId);

            /*
            ** DestPtr would be NULL if the msg is unsubscribed to while it is on
            ** the pipe. The BuffCount may be zero if the msg is unsubscribed to and
            ** then resubscribed to while it is on the pipe. Both of these cases are
            ** considered nominal and are handled by the code below.
            */
            if (DestPtr != NULL && DestPtr->BuffCount > 0)
            {
                DestPtr->BuffCount--;
            }

            if (PipeDscPtr->CurrentQueueDepth > 0)
            {
                --PipeDscPtr->CurrentQueueDepth;
            }
        }
        else
        {
            /* should send the bad pipe ID event here too */
            PendingEventID = CFE_SB_BAD_PIPEID_EID;
            Status         = CFE_SB_PIPE_RD_ERR;
        }

        /* Always decrement the use count, for the ref that was in the queue */
        CFE_SB_DecrBufUseCnt(BufDscPtr);
    }

    /* Before unlocking, check the PendingEventID and increment relevant error counter */
    if (Status != CFE_SUCCESS)
    {
        if (PendingEventID == CFE_SB_RCV_BAD_ARG_EID || PendingEventID == CFE_SB_BAD_PIPEID_EID)
        {
            ++CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter;
        }
        else
        {
            /* For any other unexpected error (e.g. CFE_SB_Q_RD_ERR_EID) */
            ++CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter;
        }
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* Now actually send the event, after unlocking (do not call EVS with SB locked) */
    if (PendingEventID != 0)
    {
        /* get task id for events */
        CFE_ES_GetTaskID(&TskId);

        switch (PendingEventID)
        {
            case CFE_SB_Q_RD_ERR_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_Q_RD_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Pipe Read Err,pipe %lu,app %s,stat 0x%x", CFE_RESOURCEID_TO_ULONG(PipeId),
                                           CFE_SB_GetAppTskName(TskId, FullName), (unsigned int)RcvStatus);
                break;
            case CFE_SB_RCV_BAD_ARG_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_RCV_BAD_ARG_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Rcv Err:Bad Input Arg:BufPtr 0x%lx,pipe %lu,t/o %d,app %s",
                                           (unsigned long)BufPtr, CFE_RESOURCEID_TO_ULONG(PipeId), (int)TimeOut,
                                           CFE_SB_GetAppTskName(TskId, FullName));
                break;
            case CFE_SB_BAD_PIPEID_EID:
                CFE_EVS_SendEventWithAppID(CFE_SB_BAD_PIPEID_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                           "Rcv Err:PipeId %lu does not exist,app %s", CFE_RESOURCEID_TO_ULONG(PipeId),
                                           CFE_SB_GetAppTskName(TskId, FullName));
                break;
        }
    }

    /* If not successful, set the output pointer to NULL */
    if (Status != CFE_SUCCESS && BufPtr != NULL)
    {
        *BufPtr = NULL;
    }

    return Status;
}

/*
 * Function: CFE_SB_AllocateMessageBuffer - See API and header file for details
 */
CFE_SB_Buffer_t *CFE_SB_AllocateMessageBuffer(size_t MsgSize)
{
    CFE_ES_AppId_t    AppId;
    CFE_SB_BufferD_t *BufDscPtr;
    CFE_SB_Buffer_t * BufPtr;

    AppId     = CFE_ES_APPID_UNDEFINED;
    BufDscPtr = NULL;
    BufPtr    = NULL;

    if (MsgSize > CFE_MISSION_SB_MAX_SB_MSG_SIZE)
    {
        CFE_ES_WriteToSysLog(" CFE_SB:ZeroCopyGetPtr-Failed, MsgSize is too large\n");
        return NULL;
    }

    /* get callers AppId */
    if (CFE_ES_GetAppID(&AppId) == CFE_SUCCESS)
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

} /* CFE_SB_AllocateMessageBuffer */

/*
 * Helper function to do sanity checks on the Zero Copy Buffer and
 * outputs the encapsulating descriptor if successful
 */
int32 CFE_SB_ZeroCopyBufferValidate(CFE_SB_Buffer_t *BufPtr, CFE_SB_BufferD_t **BufDscPtr)
{
    cpuaddr BufDscAddr;

    /*
     * Sanity Check that the pointers are not NULL
     */
    if (BufPtr == NULL)
    {
        return CFE_SB_BAD_ARGUMENT;
    }

    /*
     * Calculate descriptor pointer from buffer pointer -
     * The buffer is just a member (offset) in the descriptor
     */
    BufDscAddr = (cpuaddr)BufPtr - offsetof(CFE_SB_BufferD_t, Content);
    *BufDscPtr = (CFE_SB_BufferD_t *)BufDscAddr;

    /*
     * Check that the descriptor is actually a "zero copy" type,
     */
    if (!CFE_RESOURCEID_TEST_DEFINED((*BufDscPtr)->AppId))
    {
        return CFE_SB_BUFFER_INVALID;
    }

    /* Basic sanity check passed */
    return CFE_SUCCESS;
}

/*
 * Function: CFE_SB_ReleaseMessageBuffer - See API and header file for details
 */
CFE_Status_t CFE_SB_ReleaseMessageBuffer(CFE_SB_Buffer_t *BufPtr)
{
    CFE_SB_BufferD_t *BufDscPtr;
    int32             Status;

    Status = CFE_SB_ZeroCopyBufferValidate(BufPtr, &BufDscPtr);

    CFE_SB_LockSharedData(__func__, __LINE__);

    if (Status == CFE_SUCCESS)
    {
        /* Clear the ownership app ID and decrement use count (may also free) */
        BufDscPtr->AppId = CFE_ES_APPID_UNDEFINED;
        CFE_SB_DecrBufUseCnt(BufDscPtr);
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    return Status;

} /* end CFE_SB_ReleaseMessageBuffer */

/*
 * Function CFE_SB_TransmitBuffer - See API and header file for details
 */
int32 CFE_SB_TransmitBuffer(CFE_SB_Buffer_t *BufPtr, bool IncrementSequenceCount)
{
    int32             Status;
    CFE_SB_BufferD_t *BufDscPtr;
    CFE_SBR_RouteId_t RouteId;

    Status = CFE_SB_ZeroCopyBufferValidate(BufPtr, &BufDscPtr);

    if (Status == CFE_SUCCESS)
    {
        /* Validate the content and get the MsgId, store it in the descriptor */
        Status = CFE_SB_TransmitMsgValidate(&BufPtr->Msg, &BufDscPtr->MsgId, &BufDscPtr->ContentSize, &RouteId);

        /*
         * Broadcast the message if validation succeeded.
         *
         * Note that for the case of no subscribers, the validation returns CFE_SUCCESS
         * but the actual route ID may be invalid.  This is OK and considered normal-
         * the validation will increment the NoSubscribers count, but we should NOT
         * increment the MsgSendErrorCounter here - it is not really a sending error to
         * have no subscribers.  CFE_SB_BroadcastBufferToRoute() will not send to
         * anything if the route is not valid (benign).
         */
        if (Status == CFE_SUCCESS)
        {
            BufDscPtr->AutoSequence = IncrementSequenceCount;
            CFE_MSG_GetType(&BufPtr->Msg, &BufDscPtr->ContentType);

            /* Now broadcast the message, which consumes the buffer */
            CFE_SB_BroadcastBufferToRoute(BufDscPtr, RouteId);

            /*
             * IMPORTANT - the descriptor might be freed at any time after this,
             * so the descriptor should not be accessed again after this point.
             */
            BufDscPtr = NULL;
        }
    }

    if (Status != CFE_SUCCESS)
    {
        /* Increment send error counter for validation failure */
        CFE_SB_LockSharedData(__func__, __LINE__);
        CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter++;
        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    return Status;
}
