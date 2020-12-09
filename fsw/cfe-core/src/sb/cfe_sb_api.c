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
#include "common_types.h"
#include "private/cfe_private.h"
#include "cfe_sb_events.h"
#include "cfe_sb_priv.h"
#include "cfe_sb.h"
#include "osapi.h"
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_error.h"
#include <string.h>

/* 
 * Macro to reflect size of PipeDepthStats Telemetry array - 
 * this may or may not be the same as CFE_SB_MSG_MAX_PIPES
 */
#define CFE_SB_TLM_PIPEDEPTHSTATS_SIZE     (sizeof(CFE_SB.StatTlmMsg.Payload.PipeDepthStats) / sizeof(CFE_SB.StatTlmMsg.Payload.PipeDepthStats[0]))

/* Local structure for remove pipe callbacks */
typedef struct
{
    const char     *FullName;   /* Full name (app.task) for error reporting */
    CFE_SB_PipeId_t PipeId;     /* Pipe id to remove */
} CFE_SB_RemovePipeCallback_t;

/*
 * Function: CFE_SB_CreatePipe - See API and header file for details
 */
int32  CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16  Depth, const char *PipeName)
{
    CFE_ES_ResourceID_t  AppId;
    CFE_ES_ResourceID_t  TskId;
    osal_id_t       SysQueueId;
    int32           Status;
    CFE_SB_PipeId_t OriginalPipeIdParamValue = (PipeIdPtr == NULL) ? 0 : (*PipeIdPtr);
    CFE_SB_PipeId_t PipeTblIdx;
    char            AppName[OS_MAX_API_NAME] = {'\0'};
    char            FullName[(OS_MAX_API_NAME * 2)];

    /* get callers AppId */
    CFE_ES_GetAppID(&AppId);

    /* get callers TaskId */
    CFE_ES_GetTaskID(&TskId);

    /* get callers name */
    CFE_ES_GetAppName(AppName, AppId, sizeof(AppName));

    /* Hardcode a NULL terminator, in case rcvd name was too long */
    AppName[OS_MAX_API_NAME-1]= '\0';

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* set user's pipe id value to 'invalid' for error cases below */
    if(PipeIdPtr != NULL){
        *PipeIdPtr = CFE_SB_INVALID_PIPE;
    }/* end if */

    /* check input parameters */
    if((PipeIdPtr == NULL)||(Depth > CFE_PLATFORM_SB_MAX_PIPE_DEPTH)||(Depth == 0)){
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_BAD_ARG_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "CreatePipeErr:Bad Input Arg:app=%s,ptr=0x%lx,depth=%d,maxdepth=%d",
                CFE_SB_GetAppTskName(TskId,FullName),(unsigned long)PipeIdPtr,(int)Depth,CFE_PLATFORM_SB_MAX_PIPE_DEPTH);
        return CFE_SB_BAD_ARGUMENT;
    }/*end if*/

    /* get first available entry in pipe table */
    PipeTblIdx = CFE_SB_GetAvailPipeIdx();

    /* if pipe table is full, send event and return error */
    if(PipeTblIdx == CFE_SB_INVALID_PIPE){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_MAX_PIPES_MET_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "CreatePipeErr:Max Pipes(%d)In Use.app %s",
          CFE_PLATFORM_SB_MAX_PIPES,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_MAX_PIPES_MET;
    }/* end if */

    /* create the queue */
    Status = OS_QueueCreate(&SysQueueId,PipeName,Depth,sizeof(CFE_SB_BufferD_t *),0);
    if (Status != OS_SUCCESS) {
        CFE_SB_UnlockSharedData(__func__,__LINE__);

        /* if OS_QueueCreate() failed because the pipe name passed in was already in use... */
        /* let's make sure we don't alter the user's pipe ID data */
        switch(Status) {
            case OS_ERR_NAME_TAKEN:
                CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_NAME_TAKEN_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                    "CreatePipeErr:OS_QueueCreate failed, name taken (app=%s, name=%s)",
                    CFE_SB_GetAppTskName(TskId,FullName), PipeName);

                *PipeIdPtr = OriginalPipeIdParamValue;

                break;
            case OS_ERR_NO_FREE_IDS:
                CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_NO_FREE_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                    "CreatePipeErr:OS_QueueCreate failed, no free id's (app=%s)",
                    CFE_SB_GetAppTskName(TskId,FullName));

                break;
            default:
                CFE_EVS_SendEventWithAppID(CFE_SB_CR_PIPE_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                "CreatePipeErr:OS_QueueCreate returned %d,app %s",
                (int)Status,CFE_SB_GetAppTskName(TskId,FullName));
        }/* end switch(Status) */

        return CFE_SB_PIPE_CR_ERR;
    }/* end if */

    /* fill in the pipe table fields */
    CFE_SB.PipeTbl[PipeTblIdx].InUse       = CFE_SB_IN_USE;
    CFE_SB.PipeTbl[PipeTblIdx].SysQueueId  = SysQueueId;
    CFE_SB.PipeTbl[PipeTblIdx].PipeId      = PipeTblIdx;
    CFE_SB.PipeTbl[PipeTblIdx].QueueDepth  = Depth;
    CFE_SB.PipeTbl[PipeTblIdx].AppId       = AppId;
    CFE_SB.PipeTbl[PipeTblIdx].SendErrors  = 0;
    CFE_SB.PipeTbl[PipeTblIdx].CurrentBuff = NULL;
    CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff = NULL;
    strcpy(&CFE_SB.PipeTbl[PipeTblIdx].AppName[0],&AppName[0]);

    /* Increment the Pipes in use ctr and if it's > the high water mark,*/
    /* adjust the high water mark */
    CFE_SB.StatTlmMsg.Payload.PipesInUse++;
    if(CFE_SB.StatTlmMsg.Payload.PipesInUse > CFE_SB.StatTlmMsg.Payload.PeakPipesInUse){
       CFE_SB.StatTlmMsg.Payload.PeakPipesInUse = CFE_SB.StatTlmMsg.Payload.PipesInUse;
    }/* end if */

    /* Reset the pipe depth parameters in the statistics pkt */
    if (PipeTblIdx < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
    {
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PipeId = PipeTblIdx;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].Depth = Depth;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].InUse = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PeakInUse = 0;
    }

    /* give the pipe handle to the caller */
    *PipeIdPtr = PipeTblIdx;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /* send debug event */
    CFE_EVS_SendEventWithAppID(CFE_SB_PIPE_ADDED_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
          "Pipe Created:name %s,id %d,app %s",
          PipeName, (int)CFE_SB.PipeTbl[PipeTblIdx].PipeId,
          CFE_SB_GetAppTskName(TskId,FullName));

    return CFE_SUCCESS;

}/* end CFE_SB_CreatePipe */


/*
 *  Function: CFE_SB_DeletePipe - See API and header file for details
 */
int32 CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    CFE_ES_ResourceID_t CallerId;
    int32   Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_DeletePipeFull(PipeId,CallerId);

    return Status;

}/* end CFE_SB_DeletePipe */



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
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId, CFE_ES_ResourceID_t AppId)
{
    int32   Status = 0;

    Status = CFE_SB_DeletePipeFull(PipeId,AppId);

    return Status;

}/* end CFE_SB_DeletePipeWithAppId */

/******************************************************************************
 * Local callback helper for deleting a pipe from a route
 */
void CFE_SB_RemovePipeFromRoute(CFE_SBR_RouteId_t RouteId, void *ArgPtr)
{
    CFE_SB_DestinationD_t       *destptr;
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
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId,CFE_ES_ResourceID_t AppId)
{
    uint8                       PipeTblIdx;
    int32                       RtnFromVal;
    int32                       Stat;
    CFE_ES_ResourceID_t         Owner;
    CFE_ES_ResourceID_t         TskId;
    CFE_SB_Buffer_t            *BufPtr;
    char                        FullName[(OS_MAX_API_NAME * 2)];
    CFE_SB_RemovePipeCallback_t Args;

    /* get TaskId and name of caller for events */
    CFE_ES_GetTaskID(&TskId);
    CFE_SB_GetAppTskName(TskId, FullName);

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* check input parameter */
    PipeTblIdx = CFE_SB_GetPipeIdx(PipeId);
    RtnFromVal = CFE_SB_ValidatePipeId(PipeId);
    if((RtnFromVal != CFE_SUCCESS)||(PipeTblIdx == CFE_SB_INVALID_PIPE))
    {
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR1_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Pipe Delete Error:Bad Argument,PipedId %d,Requestor %s,Idx %d,Stat %d",
          (int)PipeId,FullName,(int)PipeTblIdx,(int)RtnFromVal);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    Owner = CFE_SB.PipeTbl[PipeTblIdx].AppId;

    /* check that the given AppId is the owner of the pipe */
    if( !CFE_ES_ResourceID_Equal(AppId, Owner) )
    {
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR2_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Pipe Delete Error:Caller(%s) is not the owner of pipe %d", FullName, (int)PipeId);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* Remove the pipe from all routes */
    Args.PipeId = PipeId;
    Args.FullName = FullName;
    CFE_SBR_ForEachRouteId(CFE_SB_RemovePipeFromRoute, &Args, NULL);

    if (CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff != NULL) {

        /* Decrement the Buffer Use Count and Free buffer if cnt=0) */
        CFE_SB_DecrBufUseCnt(CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff);
        CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff = NULL;

    }/* end if */

    /* remove any messages that might be on the pipe */
    /* this step will free the memory used to store the message */
    do{
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      Stat = CFE_SB_ReceiveBuffer(&BufPtr,PipeId,CFE_SB_POLL);
      CFE_SB_LockSharedData(__func__,__LINE__);
    }while(Stat == CFE_SUCCESS);

    /* Delete the underlying OS queue */
    OS_QueueDelete(CFE_SB.PipeTbl[PipeTblIdx].SysQueueId);

    /* remove the pipe from the pipe table */
    CFE_SB.PipeTbl[PipeTblIdx].InUse         = CFE_SB_NOT_IN_USE;
    CFE_SB.PipeTbl[PipeTblIdx].SysQueueId    = CFE_SB_UNUSED_QUEUE;
    CFE_SB.PipeTbl[PipeTblIdx].PipeId        = CFE_SB_INVALID_PIPE;
    CFE_SB.PipeTbl[PipeTblIdx].CurrentBuff   = NULL;

    /* zero out the pipe depth stats */
    if (PipeTblIdx < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
    {
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PipeId = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].Depth = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].InUse = 0;
    CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeTblIdx].PeakInUse = 0;
    }

    CFE_SB.StatTlmMsg.Payload.PipesInUse--;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /*
     * Get the app name of the actual pipe owner for the event string
     * as this may be different than the task doing the deletion.
     *
     * Note: If this fails (e.g. bad AppID, it returns an empty string
     */
    CFE_ES_GetAppName(FullName, Owner, sizeof(FullName));

    CFE_EVS_SendEventWithAppID(CFE_SB_PIPE_DELETED_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
          "Pipe Deleted:id %d,owner %s",(int)PipeId, FullName);

    return CFE_SUCCESS;

}/* end CFE_SB_DeletePipeFull */


/*
 *  Function: CFE_SB_SetPipeOpts - See API and header file for details
 */
int32 CFE_SB_SetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 Opts)
{
    uint8         PipeTblIdx = 0;
    int32         RtnFromVal = 0;
    CFE_ES_ResourceID_t        Owner;
    CFE_ES_ResourceID_t        AppID;
    CFE_ES_ResourceID_t        TskId;
    int32         Status;
    char          FullName[(OS_MAX_API_NAME * 2)];

    Status = CFE_ES_GetAppID(&AppID);
    if(Status != CFE_SUCCESS)
    {
        /* shouldn't happen... */
        return Status;
    }

    /* get TaskId of caller for events */
    Status = CFE_ES_GetTaskID(&TskId);
    if(Status != CFE_SUCCESS)
    {
        /* shouldn't happen... */
        return Status;
    }

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* check input parameter */
    PipeTblIdx = CFE_SB_GetPipeIdx(PipeId);
    RtnFromVal = CFE_SB_ValidatePipeId(PipeId);

    if((RtnFromVal != CFE_SUCCESS)||(PipeTblIdx == CFE_SB_INVALID_PIPE))
    {
        CFE_SB.HKTlmMsg.Payload.PipeOptsErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_ID_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Pipe Opts Error:Bad Argument,PipedId %d,Requestor %s,Idx %d,Stat %d",
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),(int)PipeTblIdx,(int)RtnFromVal);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check that the given AppId is the owner of the pipe */
    Owner = CFE_SB.PipeTbl[PipeTblIdx].AppId;
    if( !CFE_ES_ResourceID_Equal(AppID, Owner) )
    {
        CFE_SB.HKTlmMsg.Payload.PipeOptsErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_OWNER_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Pipe Opts Set Error: Caller(%s) is not the owner of pipe %d",
          CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    CFE_SB.PipeTbl[PipeTblIdx].Opts = Opts;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /*
     * Get the app name of the actual pipe owner for the event string
     * as this may be different than the task doing the deletion.
     *
     * Note: If this fails (e.g. bad AppID, it returns an empty string
     */
    CFE_ES_GetAppName(FullName, Owner, sizeof(FullName));

    CFE_EVS_SendEventWithAppID(CFE_SB_SETPIPEOPTS_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
          "Pipe opts set:id %d,owner %s, opts=0x%02x",(int)PipeId, FullName, (unsigned int)Opts);

    return CFE_SUCCESS;
}/* end CFE_SB_SetPipeOpts */

/*
 *  Function:  CFE_SB_GetPipeOpts - See API and header file for details
 */
int32 CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptsPtr)
{
    uint8         PipeTblIdx = 0;
    int32         RtnFromVal = 0;
    CFE_ES_ResourceID_t        TskId;
    char          FullName[(OS_MAX_API_NAME * 2)];

    /* get TaskId of caller for events */
    CFE_ES_GetTaskID(&TskId);

    if(OptsPtr == NULL)
    {
        CFE_SB.HKTlmMsg.Payload.PipeOptsErrorCounter++;
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEOPTS_PTR_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB.AppId,
          "Pipe Opts Error:Bad Argument,Requestor %s",
          CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* check input parameter */
    PipeTblIdx = CFE_SB_GetPipeIdx(PipeId);
    RtnFromVal = CFE_SB_ValidatePipeId(PipeId);

    if((RtnFromVal != CFE_SUCCESS)||(PipeTblIdx == CFE_SB_INVALID_PIPE))
    {
        CFE_SB.HKTlmMsg.Payload.PipeOptsErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEOPTS_ID_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB.AppId,
          "Pipe Opts Error:Bad Argument,PipedId %d,Requestor %s,Idx %d,Stat %d",
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),(int)PipeTblIdx,(int)RtnFromVal);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    *OptsPtr = CFE_SB.PipeTbl[PipeTblIdx].Opts;

    CFE_SB_UnlockSharedData(__func__,__LINE__);
    CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEOPTS_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
          "Pipe opts set:id %d, opts=0x%02x",(int)PipeId, (unsigned int)*OptsPtr);
    return CFE_SUCCESS;
}/* end CFE_SB_GetPipeOpts */

/*
 *  Function:  CFE_SB_GetPipeName - See API and header file for details
 */
int32 CFE_SB_GetPipeName(char *PipeNameBuf, size_t PipeNameSize, CFE_SB_PipeId_t PipeId){
    OS_queue_prop_t queue_prop;
    int32 Status = CFE_SUCCESS;
    CFE_ES_ResourceID_t        TskId;
    char FullName[(OS_MAX_API_NAME * 2)];

    if(PipeNameBuf == NULL || PipeNameSize == 0) {
        CFE_ES_GetTaskID(&TskId);
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_NULL_PTR_EID, CFE_EVS_EventType_ERROR,
            CFE_SB.AppId, "Pipe Name Error:NullPtr,Requestor %s",
            CFE_SB_GetAppTskName(TskId,FullName));

        Status = CFE_SB_BAD_ARGUMENT;
    } else if(PipeId >= CFE_PLATFORM_SB_MAX_PIPES){
        CFE_ES_GetTaskID(&TskId);
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_ID_ERR_EID, CFE_EVS_EventType_ERROR,
            CFE_SB.AppId, "Pipe Id Error:Bad Argument,Id=%d,Requestor %s",
            PipeId,CFE_SB_GetAppTskName(TskId,FullName));

        memset(PipeNameBuf, 0, PipeNameSize);

        Status = CFE_SB_BAD_ARGUMENT;
    }else{
        if (OS_QueueGetInfo(CFE_SB.PipeTbl[PipeId].SysQueueId, &queue_prop)
                == OS_SUCCESS){
            strncpy(PipeNameBuf, queue_prop.name, PipeNameSize-1);

            PipeNameBuf[PipeNameSize-1] = '\0'; 

            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_EID,
                CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
                "GetPipeName name=%s id=%d",
                PipeNameBuf, PipeId);
        } else{
            CFE_ES_GetTaskID(&TskId);
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_ID_ERR_EID, CFE_EVS_EventType_ERROR,
                CFE_SB.AppId, "Pipe Id Error:Bad Argument,Id=%d,Requestor %s",
                PipeId,CFE_SB_GetAppTskName(TskId,FullName));

            memset(PipeNameBuf, 0, PipeNameSize);

            Status = CFE_SB_BAD_ARGUMENT;
        }/* end if */
    }/* end if */

    return Status;
}/* end CFE_SB_GetPipeName */

/*
 *  Function:  CFE_SB_GetPipeIdByName - See API and header file for details
 */
int32 CFE_SB_GetPipeIdByName(CFE_SB_PipeId_t *PipeIdPtr, const char *PipeName)
{
    uint8         PipeTblIdx = 0;
    int32         Status = CFE_SUCCESS;
    int32         RtnFromVal = 0;
    CFE_ES_ResourceID_t        TskId;
    osal_id_t     QueueId;
    char          FullName[(OS_MAX_API_NAME * 2)];

    /* get TaskId of caller for events */
    CFE_ES_GetTaskID(&TskId);

    if(PipeName == NULL || PipeIdPtr == NULL)
    {
        CFE_SB.HKTlmMsg.Payload.GetPipeIdByNameErrorCounter++;

        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB.AppId,
          "Pipe ID By Name Error:Bad Argument,Requestor %s",
          CFE_SB_GetAppTskName(TskId,FullName));

        Status = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        RtnFromVal = OS_QueueGetIdByName(&QueueId, PipeName);

        if(RtnFromVal == OS_SUCCESS)
        {
            /* take semaphore to prevent a task switch while iterating
             * through the PipeTbl.
             */
            CFE_SB_LockSharedData(__func__,__LINE__);

            for(PipeTblIdx = 0;
                PipeTblIdx < CFE_PLATFORM_SB_MAX_PIPES;
                PipeTblIdx++)
            {
                if(CFE_SB.PipeTbl[PipeTblIdx].InUse != 0
                    && OS_ObjectIdEqual(CFE_SB.PipeTbl[PipeTblIdx].SysQueueId, QueueId))
                {
                    /* grab the ID before we release the lock */
                    *PipeIdPtr = CFE_SB.PipeTbl[PipeTblIdx].PipeId;

                    break;
                }/* end if */

            }/* end for */

            CFE_SB_UnlockSharedData(__func__,__LINE__);

            if(PipeTblIdx == CFE_PLATFORM_SB_MAX_PIPES)
            {
                /* should never get here! */
                CFE_SB.HKTlmMsg.Payload.GetPipeIdByNameErrorCounter++;

                CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID,
                    CFE_EVS_EventType_ERROR, CFE_SB.AppId,
                    "Pipe ID By Name Error:Bad Argument,Requestor %s",
                    CFE_SB_GetAppTskName(TskId,FullName));

                Status = CFE_SB_BAD_ARGUMENT;
            }
            else
            {
                CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_EID,
                    CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
                    "PipeIdByName name=%s id=%d",
                    PipeName, *PipeIdPtr);

                Status = CFE_SUCCESS;
            }
        }
        else
        {
            CFE_SB.HKTlmMsg.Payload.GetPipeIdByNameErrorCounter++;

            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID,
                CFE_EVS_EventType_ERROR, CFE_SB.AppId,
                "Pipe ID By Name Error:Bad Argument,Requestor %s",
                CFE_SB_GetAppTskName(TskId,FullName));

            Status = CFE_SB_BAD_ARGUMENT;
        }/* end if */

    }/* end if */

    return Status;

}/* end CFE_SB_GetPipeIdByName */


/*
 * Function: CFE_SB_SubscribeEx - See API and header file for details
 */
int32  CFE_SB_SubscribeEx(CFE_SB_MsgId_t   MsgId,
                          CFE_SB_PipeId_t  PipeId,
                          CFE_SB_Qos_t     Quality,
                          uint16           MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,Quality,MsgLim,(uint8)CFE_SB_GLOBAL);

}/* end CFE_SB_SubscribeEx */




/*
 * Function: CFE_SB_SubscribeLocal - See API and header file for details
 */
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t   MsgId,
                            CFE_SB_PipeId_t  PipeId,
                            uint16           MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,CFE_SB_Default_Qos,MsgLim,
                                (uint8)CFE_SB_LOCAL);

}/* end CFE_SB_SubscribeLocal */

/*
 * Function: CFE_SB_Subscribe - See API and header file for details
 */
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t   MsgId,
                       CFE_SB_PipeId_t  PipeId)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,CFE_SB_Default_Qos,
                               (uint16)CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT,
                               (uint8)CFE_SB_GLOBAL);

}/* end CFE_SB_Subscribe */


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
int32  CFE_SB_SubscribeFull(CFE_SB_MsgId_t   MsgId,
                            CFE_SB_PipeId_t  PipeId,
                            CFE_SB_Qos_t     Quality,
                            uint16           MsgLim,
                            uint8            Scope)
{
    CFE_SBR_RouteId_t RouteId;
    int32  Stat;
    CFE_ES_ResourceID_t        TskId;
    CFE_ES_ResourceID_t  AppId;
    uint8  PipeIdx;
    CFE_SB_DestinationD_t *DestPtr = NULL;
    uint32 DestCount = 0;
    char   FullName[(OS_MAX_API_NAME * 2)];
    char   PipeName[OS_MAX_API_NAME] = {'\0'};
    uint32 Collisions = 0;

    CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);

    /* get the callers Application Id */
    CFE_ES_GetAppID(&AppId);

    /* get TaskId of caller for events */
    CFE_ES_GetTaskID(&TskId);

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* check that the pipe has been created */
    PipeIdx = CFE_SB_GetPipeIdx(PipeId);
    if(PipeIdx==CFE_SB_INVALID_PIPE){
      CFE_SB.HKTlmMsg.Payload.SubscribeErrorCounter++;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_PIPE_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Subscribe Err:Invalid Pipe Id,Msg=0x%x,PipeId=%d,App %s",(unsigned int)CFE_SB_MsgIdToValue(MsgId),
          (int)PipeId, CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check that the requestor is the owner of the pipe */
    if( !CFE_ES_ResourceID_Equal(CFE_SB.PipeTbl[PipeIdx].AppId, AppId)){
      CFE_SB.HKTlmMsg.Payload.SubscribeErrorCounter++;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_CALLER_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Subscribe Err:Caller(%s) is not the owner of pipe %d,Msg=0x%x",
          CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId,(unsigned int)CFE_SB_MsgIdToValue(MsgId));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check message id key and scope */
    if(!CFE_SB_IsValidMsgId(MsgId) || (Scope > 1))
    {
        CFE_SB.HKTlmMsg.Payload.SubscribeErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SUB_ARG_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Subscribe Err:Bad Arg,MsgId 0x%x,PipeId %d,app %s,scope %d",
          (unsigned int)CFE_SB_MsgIdToValue(MsgId),
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),Scope);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    RouteId = CFE_SBR_GetRouteId(MsgId);
    if (CFE_SBR_IsValidRouteId(RouteId))
    {
        /* check for duplicate subscription */
        if(CFE_SB_GetDestPtr(RouteId, PipeId) != NULL)
        {
            CFE_SB.HKTlmMsg.Payload.DuplicateSubscriptionsCounter++;
            CFE_SB_UnlockSharedData(__func__,__LINE__);
            CFE_EVS_SendEventWithAppID(CFE_SB_DUP_SUBSCRIP_EID,CFE_EVS_EventType_INFORMATION,CFE_SB.AppId,
              "Duplicate Subscription,MsgId 0x%x on %s pipe,app %s",
               (unsigned int)CFE_SB_MsgIdToValue(MsgId),
               PipeName,CFE_SB_GetAppTskName(TskId,FullName));
            return CFE_SUCCESS;
        }

        /* Check for destination limit */
        for (DestPtr = CFE_SBR_GetDestListHeadPtr(RouteId); DestPtr != NULL; DestPtr = DestPtr->Next)
        {
            DestCount++;
        }
        if(DestCount >= CFE_PLATFORM_SB_MAX_DEST_PER_PKT){
            CFE_SB_UnlockSharedData(__func__,__LINE__);
            CFE_EVS_SendEventWithAppID(CFE_SB_MAX_DESTS_MET_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                "Subscribe Err:Max Dests(%d)In Use For Msg 0x%x,pipe %s,app %s",
                 CFE_PLATFORM_SB_MAX_DEST_PER_PKT,
                 (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                 PipeName, CFE_SB_GetAppTskName(TskId,FullName));

            return CFE_SB_MAX_DESTS_MET;
        }
    }
    else
    {
        /* Add the route */
        RouteId = CFE_SBR_AddRoute(MsgId, &Collisions);

        /* if all routing table elements are used, send event */
        if(!CFE_SBR_IsValidRouteId(RouteId)){
            CFE_SB_UnlockSharedData(__func__,__LINE__);
            CFE_EVS_SendEventWithAppID(CFE_SB_MAX_MSGS_MET_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
              "Subscribe Err:Max Msgs(%d)In Use,MsgId 0x%x,pipe %s,app %s",
              CFE_PLATFORM_SB_MAX_MSG_IDS,
              (unsigned int)CFE_SB_MsgIdToValue(MsgId),
              PipeName,CFE_SB_GetAppTskName(TskId,FullName));
            return CFE_SB_MAX_MSGS_MET;
        }/* end if */

        /* Increment the MsgIds in use ctr and if it's > the high water mark,*/
        /* adjust the high water mark */
        CFE_SB.StatTlmMsg.Payload.MsgIdsInUse++;
        if(CFE_SB.StatTlmMsg.Payload.MsgIdsInUse > CFE_SB.StatTlmMsg.Payload.PeakMsgIdsInUse){
           CFE_SB.StatTlmMsg.Payload.PeakMsgIdsInUse = CFE_SB.StatTlmMsg.Payload.MsgIdsInUse;
        }/* end if */

    }/* end if */

    DestPtr = CFE_SB_GetDestinationBlk();
    if(DestPtr == NULL){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEST_BLK_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Subscribe Err:Request for Destination Blk failed for Msg 0x%x", 
            (unsigned int)CFE_SB_MsgIdToValue(MsgId));
        return CFE_SB_BUF_ALOC_ERR;
    }/* end if */

    /* initialize destination block */
    DestPtr->PipeId = PipeId;
    DestPtr->MsgId2PipeLim = (uint16)MsgLim;
    DestPtr->Active = CFE_SB_ACTIVE;
    DestPtr->BuffCount = 0;
    DestPtr->DestCnt = 0;
    DestPtr->Scope = Scope;
    DestPtr->Prev = NULL;
    DestPtr->Next = NULL;

    /* add destination node */
    CFE_SB_AddDestNode(RouteId, DestPtr);

    CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse++;
    if(CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse > CFE_SB.StatTlmMsg.Payload.PeakSubscriptionsInUse)
    {
       CFE_SB.StatTlmMsg.Payload.PeakSubscriptionsInUse = CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse;
    }/* end if */

    if((CFE_SB.SubscriptionReporting == CFE_SB_ENABLE)&&(Scope==CFE_SB_GLOBAL)){
      CFE_SB.SubRprtMsg.Payload.MsgId = MsgId;
      CFE_SB.SubRprtMsg.Payload.Pipe = PipeId;
      CFE_SB.SubRprtMsg.Payload.Qos.Priority = Quality.Priority;
      CFE_SB.SubRprtMsg.Payload.Qos.Reliability = Quality.Reliability;
      CFE_SB.SubRprtMsg.Payload.SubType = CFE_SB_SUBSCRIPTION;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      Stat = CFE_SB_TransmitMsg(&CFE_SB.SubRprtMsg.Hdr.Msg, true);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RPT_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
            "Sending Subscription Report Msg=0x%x,Pipe=%d,Stat=0x%x",
            (unsigned int)CFE_SB_MsgIdToValue(MsgId),
            (int)PipeId,(unsigned int)Stat);
      CFE_SB_LockSharedData(__func__,__LINE__);/* to prevent back-to-back unlock */
    }/* end if */

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__,__LINE__);

    CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RCVD_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
        "Subscription Rcvd:MsgId 0x%x on %s(%d),app %s",
         (unsigned int)CFE_SB_MsgIdToValue(MsgId),
         PipeName,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));

    if (Collisions != 0)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_HASHCOLLISION_EID, CFE_EVS_EventType_DEBUG, CFE_SB.AppId,
        "Msg hash collision: MsgId = 0x%x, collisions = %u",
        (unsigned int)CFE_SB_MsgIdToValue(MsgId), (unsigned int)Collisions);
    }

    return CFE_SUCCESS;

}/* end CFE_SB_SubscribeFull */


/*
 * Function: CFE_SB_Unsubscribe - See API and header file for details
 */
int32 CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    CFE_ES_ResourceID_t CallerId;
    int32   Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_GLOBAL,CallerId);

    return Status;

}/* end CFE_SB_Unsubscribe */


/*
 * Function: CFE_SB_UnsubscribeLocal - See API and header file for details
 */
int32 CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    CFE_ES_ResourceID_t CallerId;
    int32   Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_LOCAL,CallerId);

    return Status;

}/* end CFE_SB_UnsubscribeLocal */


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
int32 CFE_SB_UnsubscribeWithAppId(CFE_SB_MsgId_t MsgId,
                              CFE_SB_PipeId_t PipeId,
                              CFE_ES_ResourceID_t AppId)
{
    int32   Status = 0;

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_LOCAL, AppId);

    return Status;

}/* end CFE_SB_UnsubscribeWithAppId */


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
int32 CFE_SB_UnsubscribeFull(CFE_SB_MsgId_t MsgId,CFE_SB_PipeId_t PipeId,
                             uint8 Scope,CFE_ES_ResourceID_t AppId)
{
    CFE_SBR_RouteId_t RouteId;
    uint32  PipeIdx;
    CFE_ES_ResourceID_t        TskId;
    CFE_SB_DestinationD_t   *DestPtr = NULL;
    char    FullName[(OS_MAX_API_NAME * 2)];
    char    PipeName[OS_MAX_API_NAME] = {'\0'};

    /* get TaskId of caller for events */
    CFE_ES_GetTaskID(&TskId);

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* check that the pipe has been created */
    PipeIdx = CFE_SB_GetPipeIdx(PipeId);
    if(PipeIdx==CFE_SB_INVALID_PIPE){
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_PIPE_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Unsubscribe Err:Invalid Pipe Id Msg=0x%x,Pipe=%d,app=%s",
            (unsigned int)CFE_SB_MsgIdToValue(MsgId),
            (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* if given 'AppId' is not the owner of the pipe, send error event and return */
    if( !CFE_ES_ResourceID_Equal(CFE_SB.PipeTbl[PipeIdx].AppId, AppId) )
    {
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_CALLER_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Unsubscribe Err:Caller(%s) is not the owner of pipe %d,Msg=0x%x",
            CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId,
            (unsigned int)CFE_SB_MsgIdToValue(MsgId));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check input parameters */
    if(!CFE_SB_IsValidMsgId(MsgId) ||
       (CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS)||
       (Scope > 1))
    {
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_ARG_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "UnSubscribe Err:Bad Arg,MsgId 0x%x,PipeId %d,app %s,scope %d",
            (unsigned int)CFE_SB_MsgIdToValue(MsgId),
            (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),(int)Scope);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* get routing id */
    RouteId = CFE_SBR_GetRouteId(MsgId);

    /* if there have never been subscriptions for this message id... */
    if(!CFE_SBR_IsValidRouteId(RouteId))
    {
        CFE_SB_UnlockSharedData(__func__,__LINE__);

        CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);

        CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_NO_SUBS_EID,CFE_EVS_EventType_INFORMATION,CFE_SB.AppId,
            "Unsubscribe Err:No subs for Msg 0x%x on %s,app %s",
            (unsigned int)CFE_SB_MsgIdToValue(MsgId),
            PipeName,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SUCCESS;
    }/* end if */

    /* Get the destination pointer */
    DestPtr = CFE_SB_GetDestPtr(RouteId, PipeId);

    if(DestPtr != NULL)
    {
        /* match found, remove destination */
        CFE_SB_RemoveDest(RouteId,DestPtr);

        CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_REMOVED_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
            "Subscription Removed:Msg 0x%x on pipe %d,app %s",
            (unsigned int)CFE_SB_MsgIdToValue(MsgId),
            (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
    }
    else
    {
        CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);

        CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_NO_SUBS_EID,CFE_EVS_EventType_INFORMATION,CFE_SB.AppId,
            "Unsubscribe Err:No subs for Msg 0x%x on %s,app %s",
            (unsigned int)CFE_SB_MsgIdToValue(MsgId),
            PipeName,CFE_SB_GetAppTskName(TskId,FullName));
    }/* end if */

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return CFE_SUCCESS;
}/* end CFE_SB_UnsubscribeFull */

/*
 * Function CFE_SB_TransmitMsg - See API and header file for details
 */
int32  CFE_SB_TransmitMsg(CFE_MSG_Message_t *MsgPtr, bool IncrementSequenceCount)
{
    int32               Status;
    CFE_MSG_Size_t      Size = 0;
    CFE_SB_MsgId_t      MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_ES_ResourceID_t TskId;
    char                FullName[(OS_MAX_API_NAME * 2)];
    CFE_SB_BufferD_t   *BufDscPtr;
    CFE_SBR_RouteId_t   RouteId;
    CFE_MSG_Type_t      MsgType;

    /* Get task id for events and Sender Info*/
    CFE_ES_GetTaskID(&TskId);

    Status = CFE_SB_TransmitMsgValidate(MsgPtr, &MsgId, &Size, &RouteId);

    /* Copy into buffer and send if route exists */
    if (Status == CFE_SUCCESS && CFE_SBR_IsValidRouteId(RouteId))
    {
        /* Get buffer */
        BufDscPtr = CFE_SB_GetBufferFromPool(MsgId, Size);
        if (BufDscPtr == NULL)
        {
            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_GET_BUF_ERR_EID_BIT) == CFE_SB_GRANTED)
            {
                CFE_EVS_SendEventWithAppID(CFE_SB_GET_BUF_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                  "Send Err:Request for Buffer Failed. MsgId 0x%x,app %s,size %d",
                  (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                  CFE_SB_GetAppTskName(TskId,FullName),(int)Size);

                /* clear the bit so the task may send this event again */
                CFE_SB_FinishSendEvent(TskId,CFE_SB_GET_BUF_ERR_EID_BIT);
            }

            Status = CFE_SB_BUF_ALOC_ERR;
        }
        else
        {
            /* For Tlm packets, increment the seq count if requested */
            CFE_MSG_GetType(MsgPtr, &MsgType);
            if((MsgType == CFE_MSG_Type_Tlm) && IncrementSequenceCount)
            {
                CFE_SBR_IncrementSequenceCounter(RouteId);
                CFE_MSG_SetSequenceCount(MsgPtr, CFE_SBR_GetSequenceCounter(RouteId));
            }

            /* Copy data into buffer and transmit */
            memcpy(BufDscPtr->Buffer, MsgPtr, Size);
            Status = CFE_SB_TransmitBufferFull(BufDscPtr, RouteId, MsgId);
        }
    }

    if (Status != CFE_SUCCESS)
    {
       /* Increment error counter (inside lock) if not success */
        CFE_SB_LockSharedData(__func__, __LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter++;
        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    return Status;
}

#ifndef CFE_OMIT_DEPRECATED_6_8
/*
 * Function: CFE_SB_SendMsg - See API and header file for details
 */
int32  CFE_SB_SendMsg(CFE_MSG_Message_t *MsgPtr)
{
    int32  Status = 0;

    Status = CFE_SB_TransmitMsg(MsgPtr, true);

    return Status;

}/* end CFE_SB_SendMsg */

/*
 * Function: CFE_SB_PassMsg - See API and header file for details
 */
int32  CFE_SB_PassMsg(CFE_MSG_Message_t *MsgPtr)
{
    int32   Status = 0;

    Status = CFE_SB_TransmitMsg(MsgPtr, false);

    return Status;

}/* end CFE_SB_PassMsg */
#endif /* CFE_OMIT_DEPRECATED_6_8 */

/*****************************************************************************/
/**
 * \brief Internal routine to validate a transmit message before sending
 *
 * \param[in]  MsgPtr     Pointer to the message to validate
 * \param[out] MsgIdPtr   Message Id of message
 * \param[out] SizePtr    Size of message
 * \param[out] RouteIdPtr Route ID of the message (invalid if none)
 */
int32 CFE_SB_TransmitMsgValidate(CFE_MSG_Message_t *MsgPtr,
                                 CFE_SB_MsgId_t    *MsgIdPtr,
                                 CFE_MSG_Size_t    *SizePtr,
                                 CFE_SBR_RouteId_t *RouteIdPtr)
{
    CFE_ES_ResourceID_t     TskId;
    char                    FullName[(OS_MAX_API_NAME * 2)];

    /* get task id for events and Sender Info*/
    CFE_ES_GetTaskID(&TskId);

    /* check input parameter */
    if(MsgPtr == NULL)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_BAD_ARG_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Send Err:Bad input argument,Arg 0x%lx,App %s",
            (unsigned long)MsgPtr,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    CFE_MSG_GetMsgId(MsgPtr, MsgIdPtr);

    /* validate the msgid in the message */
    if(!CFE_SB_IsValidMsgId(*MsgIdPtr))
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_INV_MSGID_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Send Err:Invalid MsgId(0x%x)in msg,App %s",
            (unsigned int)CFE_SB_MsgIdToValue(*MsgIdPtr),
            CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    CFE_MSG_GetSize(MsgPtr, SizePtr);

    /* Verify the size of the pkt is < or = the mission defined max */
    if(*SizePtr > CFE_MISSION_SB_MAX_SB_MSG_SIZE)
    {
        CFE_EVS_SendEventWithAppID(CFE_SB_MSG_TOO_BIG_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Send Err:Msg Too Big MsgId=0x%x,app=%s,size=%d,MaxSz=%d",
            (unsigned int)CFE_SB_MsgIdToValue(*MsgIdPtr),
            CFE_SB_GetAppTskName(TskId,FullName),(int)*SizePtr,CFE_MISSION_SB_MAX_SB_MSG_SIZE);
        return CFE_SB_MSG_TOO_BIG;
    }/* end if */

    /* Get the routing id */
    *RouteIdPtr = CFE_SBR_GetRouteId(*MsgIdPtr);

    /* if there have been no subscriptions for this pkt, */
    /* increment the dropped pkt cnt, send event and return success */
    if(!CFE_SBR_IsValidRouteId(*RouteIdPtr)){

        CFE_SB.HKTlmMsg.Payload.NoSubscribersCounter++;

        /* Determine if event can be sent without causing recursive event problem */
        if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT) == CFE_SB_GRANTED){

           CFE_EVS_SendEventWithAppID(CFE_SB_SEND_NO_SUBS_EID,CFE_EVS_EventType_INFORMATION,CFE_SB.AppId,
              "No subscribers for MsgId 0x%x,sender %s",
              (unsigned int)CFE_SB_MsgIdToValue(*MsgIdPtr),
              CFE_SB_GetAppTskName(TskId,FullName));

           /* clear the bit so the task may send this event again */
           CFE_SB_FinishSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT);
        }/* end if */

    }/* end if */

    return CFE_SUCCESS;
}

/*****************************************************************************/
/**
 * \brief Internal routine implements full send logic
 *
 * \param[in] BufDscPtr Pointer to the buffer description from the memory pool,
 *                      released prior to return
 * \param[in] RouteId   Route to send to
 * \param[in] MsgId     Message Id that is being sent
 */
int32  CFE_SB_TransmitBufferFull(CFE_SB_BufferD_t *BufDscPtr,
                                 CFE_SBR_RouteId_t RouteId,
                                 CFE_SB_MsgId_t    MsgId)
{
    CFE_ES_ResourceID_t     AppId;
    CFE_ES_ResourceID_t     TskId;
    CFE_SB_DestinationD_t   *DestPtr;
    CFE_SB_PipeD_t          *PipeDscPtr;
    CFE_SB_EventBuf_t       SBSndErr = {0};
    char                    PipeName[OS_MAX_API_NAME] = {'\0'};
    CFE_SB_PipeDepthStats_t *StatObj;
    int32                   Status;
    uint32                  i;
    char                    FullName[(OS_MAX_API_NAME * 2)];

    /* get app id for loopback testing */
    CFE_ES_GetAppID(&AppId);

    /* get task id for events and Sender Info*/
    CFE_ES_GetTaskID(&TskId);

    /* take semaphore to prevent a task switch during processing */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* Send the packet to all destinations  */
    for(DestPtr = CFE_SBR_GetDestListHeadPtr(RouteId); DestPtr != NULL; DestPtr = DestPtr->Next)
    {
        if (DestPtr->Active == CFE_SB_INACTIVE)    /* destination is active */
        {
            continue;
        }/*end if */

        PipeDscPtr = &CFE_SB.PipeTbl[DestPtr->PipeId];

        if((PipeDscPtr->Opts & CFE_SB_PIPEOPTS_IGNOREMINE) != 0 &&
                CFE_ES_ResourceID_Equal(PipeDscPtr->AppId, AppId))
        {
            continue;
        }/* end if */

        /* if Msg limit exceeded, log event, increment counter */
        /* and go to next destination */
        if(DestPtr->BuffCount >= DestPtr->MsgId2PipeLim){

            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_MSGID_LIM_ERR_EID;
            SBSndErr.EvtsToSnd++;
            CFE_SB.HKTlmMsg.Payload.MsgLimitErrorCounter++;
            PipeDscPtr->SendErrors++;

            continue;
        }/* end if */

        /*
        ** Write the buffer descriptor to the queue of the pipe.  If the write
        ** failed, log info and increment the pipe's error counter.
        */
        Status = OS_QueuePut(PipeDscPtr->SysQueueId, (void *)&BufDscPtr, sizeof(CFE_SB_BufferD_t *), 0);

        if (Status == OS_SUCCESS)
        {
            BufDscPtr->UseCount++;    /* used for releasing buffer  */
            DestPtr->BuffCount++; /* used for checking MsgId2PipeLimit */
            DestPtr->DestCnt++;   /* used for statistics */
            if (DestPtr->PipeId < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
            {
                StatObj = &CFE_SB.StatTlmMsg.Payload.PipeDepthStats[DestPtr->PipeId];
                StatObj->InUse++;
                if(StatObj->InUse > StatObj->PeakInUse)
                {
                    StatObj->PeakInUse = StatObj->InUse;
                }/* end if */
            }
        }
        else if(Status == OS_QUEUE_FULL)
        {
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_Q_FULL_ERR_EID;
            SBSndErr.EvtsToSnd++;
            CFE_SB.HKTlmMsg.Payload.PipeOverflowErrorCounter++;
            PipeDscPtr->SendErrors++;
        }
        else
        {
            /* Unexpected error while writing to queue. */
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_Q_WR_ERR_EID;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].ErrStat = Status;
            SBSndErr.EvtsToSnd++;
            CFE_SB.HKTlmMsg.Payload.InternalErrorCounter++;
            PipeDscPtr->SendErrors++;
        }/*end if */

    } /* end loop over destinations */
    
    /*
    ** Decrement the buffer UseCount and free buffer if cnt=0. This decrement is done
    ** because the use cnt is initialized to 1 in CFE_SB_GetBufferFromPool.
    ** Initializing the count to 1 (as opposed to zero) and decrementing it here are
    ** done to ensure the buffer gets released when there are destinations that have
    ** been disabled via ground command.
    */
    CFE_SB_DecrBufUseCnt(BufDscPtr);

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /* send an event for each pipe write error that may have occurred */
    for(i=0;i < SBSndErr.EvtsToSnd; i++)
    {
        if(SBSndErr.EvtBuf[i].EventId == CFE_SB_MSGID_LIM_ERR_EID)
        {

            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_MSGID_LIM_ERR_EID_BIT) == CFE_SB_GRANTED){

              CFE_SB_GetPipeName(PipeName, sizeof(PipeName), SBSndErr.EvtBuf[i].PipeId);

              CFE_ES_PerfLogEntry(CFE_MISSION_SB_MSG_LIM_PERF_ID);
              CFE_ES_PerfLogExit(CFE_MISSION_SB_MSG_LIM_PERF_ID);

              CFE_EVS_SendEventWithAppID(CFE_SB_MSGID_LIM_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                "Msg Limit Err,MsgId 0x%x,pipe %s,sender %s",
                (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                PipeName, CFE_SB_GetAppTskName(TskId,FullName));

              /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_MSGID_LIM_ERR_EID_BIT);
            }/* end if */

        }else if(SBSndErr.EvtBuf[i].EventId == CFE_SB_Q_FULL_ERR_EID){

            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_Q_FULL_ERR_EID_BIT) == CFE_SB_GRANTED){

              CFE_SB_GetPipeName(PipeName, sizeof(PipeName), SBSndErr.EvtBuf[i].PipeId);

              CFE_ES_PerfLogEntry(CFE_MISSION_SB_PIPE_OFLOW_PERF_ID);
              CFE_ES_PerfLogExit(CFE_MISSION_SB_PIPE_OFLOW_PERF_ID);

              CFE_EVS_SendEventWithAppID(CFE_SB_Q_FULL_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                  "Pipe Overflow,MsgId 0x%x,pipe %s,sender %s",
                  (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                  PipeName, CFE_SB_GetAppTskName(TskId,FullName));

               /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_Q_FULL_ERR_EID_BIT);
            }/* end if */

        }else{

            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_Q_WR_ERR_EID_BIT) == CFE_SB_GRANTED){

              CFE_SB_GetPipeName(PipeName, sizeof(PipeName), SBSndErr.EvtBuf[i].PipeId);

              CFE_EVS_SendEventWithAppID(CFE_SB_Q_WR_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                "Pipe Write Err,MsgId 0x%x,pipe %s,sender %s,stat 0x%x",
                (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                PipeName, CFE_SB_GetAppTskName(TskId,FullName),
                (unsigned int)SBSndErr.EvtBuf[i].ErrStat);

               /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_Q_WR_ERR_EID_BIT);
            }/* end if */

        }/* end if */
    }

    return CFE_SUCCESS;

}

#if CFE_OMIT_DEPRECATED_6_8
int32 CFE_SB_RcvMsg(CFE_SB_Buffer_t **BufPtr,
                    CFE_SB_PipeId_t   PipeId,
                    int32             TimeOut)
{
    return CFE_SB_ReceiveBuffer(BufPtr, PipeId, TimeOut);
}
#endif /* CFE_OMIT_DEPRECATED_6_8 */

/*
 * Function: CFE_SB_ReceiveBuffer - See API and header file for details
 */
int32  CFE_SB_ReceiveBuffer(CFE_SB_Buffer_t **BufPtr,
                            CFE_SB_PipeId_t   PipeId,
                            int32             TimeOut)
{
    int32                  Status;
    CFE_SB_BufferD_t       *Message;
    CFE_SB_PipeD_t         *PipeDscPtr;
    CFE_SB_DestinationD_t  *DestPtr = NULL;
    CFE_SBR_RouteId_t      RouteId;
    CFE_ES_ResourceID_t    TskId;
    char                   FullName[(OS_MAX_API_NAME * 2)];

    /* get task id for events */
    CFE_ES_GetTaskID(&TskId);

    /* Check input parameters */
    if((BufPtr == NULL)||(TimeOut < (-1))){
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgReceiveErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_RCV_BAD_ARG_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Rcv Err:Bad Input Arg:BufPtr 0x%lx,pipe %d,t/o %d,app %s",
            (unsigned long)BufPtr,(int)PipeId,(int)TimeOut,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    PipeDscPtr = CFE_SB_GetPipePtr(PipeId);
    /* If the pipe does not exist or PipeId is out of range... */
    if (PipeDscPtr == NULL) {
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgReceiveErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_BAD_PIPEID_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Rcv Err:PipeId %d does not exist,app %s",
            (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /*
    ** Save off any currently in use buffer to free later; this allows
    ** one fewer shared data locks
    */

    PipeDscPtr->ToTrashBuff = PipeDscPtr->CurrentBuff;
    PipeDscPtr->CurrentBuff = NULL;

    /*
    ** Read a new buffer from the queue of the pipe, using the
    ** specified timeout option.  If a buffer was obtained, return the
    ** packet to the task according to mode.  Otherwise, return a status
    ** code indicating that no buffer was read.
    */
    Status = CFE_SB_ReadQueue(PipeDscPtr, TskId, TimeOut, &Message);

    /* take semaphore again to protect the remaining code in this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* free any pending trash buffer */
    if (PipeDscPtr->ToTrashBuff != NULL) {

        /* Decrement the Buffer Use Count and Free buffer if cnt=0) */
        CFE_SB_DecrBufUseCnt(PipeDscPtr->ToTrashBuff);

        PipeDscPtr->ToTrashBuff = NULL;

    }/* end if */

    if (Status == CFE_SUCCESS) {

        /*
        ** Load the pipe tables 'CurrentBuff' with the buffer descriptor
        ** ptr corresponding to the message just read. This is done so that
        ** the buffer can be released on the next receive call for this pipe.
        */
        PipeDscPtr->CurrentBuff = Message;

        /* Set the Receivers pointer to the address of the actual message */
        *BufPtr = (CFE_SB_Buffer_t *) Message->Buffer;

        /* get pointer to destination to be used in decrementing msg limit cnt*/
        RouteId = CFE_SBR_GetRouteId(PipeDscPtr->CurrentBuff->MsgId);
        DestPtr = CFE_SB_GetDestPtr(RouteId, PipeDscPtr->PipeId);

        /*
        ** DestPtr would be NULL if the msg is unsubscribed to while it is on
        ** the pipe. The BuffCount may be zero if the msg is unsubscribed to and
        ** then resubscribed to while it is on the pipe. Both of these cases are
        ** considered nominal and are handled by the code below.
        */
        if(DestPtr != NULL){

            if (DestPtr->BuffCount > 0){
                DestPtr->BuffCount--;
            }

        }/* end if DestPtr != NULL */

        if (PipeDscPtr->PipeId < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
        {
            CFE_SB.StatTlmMsg.Payload.PipeDepthStats[PipeDscPtr->PipeId].InUse--;
        }

    }else{

        /* Set the users pointer to NULL indicating the CFE_SB_ReadQueue failed */
        *BufPtr = NULL;

    }/* end if */

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /*
    ** If status is not CFE_SUCCESS, then no packet was received.  If this was
    ** caused by an unexpected error, then CFE_SB_ReadQueue() will report the
    ** error.
    */
    return Status;

}


/*
 * Function: CFE_SB_ZeroCopyGetPtr - See API and header file for details
 */
CFE_SB_Buffer_t *CFE_SB_ZeroCopyGetPtr(size_t MsgSize,
                                       CFE_SB_ZeroCopyHandle_t *BufferHandle)
{
   int32                stat1;
   CFE_ES_ResourceID_t  AppId;
   cpuaddr              address = 0;
   CFE_SB_ZeroCopyD_t  *zcd = NULL;
   CFE_SB_BufferD_t    *bd = NULL;

    CFE_SB_LockSharedData(__func__,__LINE__);

    /* Allocate a new zero copy descriptor from the SB memory pool.*/
    stat1 = CFE_ES_GetPoolBuf((CFE_ES_MemPoolBuf_t*)&zcd, CFE_SB.Mem.PoolHdl,  sizeof(CFE_SB_ZeroCopyD_t));
    if(stat1 < 0){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        return NULL;
    }

    /* Add the size of a zero copy descriptor to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.MemInUse+=stat1;
    if(CFE_SB.StatTlmMsg.Payload.MemInUse > CFE_SB.StatTlmMsg.Payload.PeakMemInUse){
       CFE_SB.StatTlmMsg.Payload.PeakMemInUse = CFE_SB.StatTlmMsg.Payload.MemInUse;
    }/* end if */

    /* Allocate a new buffer (from the SB memory pool) to hold the message  */
    stat1 = CFE_ES_GetPoolBuf((CFE_ES_MemPoolBuf_t*)&bd, CFE_SB.Mem.PoolHdl, MsgSize + sizeof(CFE_SB_BufferD_t));
    if((stat1 < 0)||(bd==NULL)){
        /*deallocate the first buffer if the second buffer creation fails*/
        stat1 = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, zcd);
        if(stat1 > 0){
            CFE_SB.StatTlmMsg.Payload.MemInUse-=stat1;
        }
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        return NULL;
    }

    /* Increment the number of buffers in use by one even though two buffers */
    /* were allocated. SBBuffersInUse increments on a per-message basis */
    CFE_SB.StatTlmMsg.Payload.SBBuffersInUse++;
    if(CFE_SB.StatTlmMsg.Payload.SBBuffersInUse > CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse){
        CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse = CFE_SB.StatTlmMsg.Payload.SBBuffersInUse;
    }/* end if */

    /* Add the size of the actual buffer to the memory-in-use ctr and */
    /* adjust the high water mark if needed */
    CFE_SB.StatTlmMsg.Payload.MemInUse+=stat1;
    if(CFE_SB.StatTlmMsg.Payload.MemInUse > CFE_SB.StatTlmMsg.Payload.PeakMemInUse){
       CFE_SB.StatTlmMsg.Payload.PeakMemInUse = CFE_SB.StatTlmMsg.Payload.MemInUse;
    }/* end if */

    /* first set ptr to actual msg buffer the same as ptr to descriptor */
    address = (cpuaddr)bd;

    /* increment actual msg buffer ptr beyond the descriptor */
    address += sizeof(CFE_SB_BufferD_t);

    /* Initialize the zero copy descriptor structure. */
    zcd->Size      = MsgSize;
    zcd->Buffer    = (void *)address;
    zcd->Next      = NULL;

    /* Add this Zero Copy Descriptor to the end of the chain */
    if(CFE_SB.ZeroCopyTail != NULL){
        ((CFE_SB_ZeroCopyD_t *) CFE_SB.ZeroCopyTail)->Next = (void *)zcd;
    }
    zcd->Prev = CFE_SB.ZeroCopyTail;
    CFE_SB.ZeroCopyTail = (void *)zcd;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    /* get callers AppId */
    CFE_ES_GetAppID(&AppId);
    zcd->AppID     = AppId;

    (*BufferHandle) = (CFE_SB_ZeroCopyHandle_t) zcd;

    /* Initialize the buffer descriptor structure. */
    bd->UseCount  = 1;
    bd->Size      = MsgSize;
    bd->Buffer    = (void *)address;

    return (CFE_SB_Buffer_t *)address;

}/* CFE_SB_ZeroCopyGetPtr */


/*
 * Function: CFE_SB_ZeroCopyReleasePtr - See API and header file for details
 */
int32 CFE_SB_ZeroCopyReleasePtr(CFE_SB_Buffer_t        *Ptr2Release,
                                CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32    Status;
    int32    Stat2;
    CFE_ES_MemPoolBuf_t BufAddr;

    Status = CFE_SB_ZeroCopyReleaseDesc(Ptr2Release, BufferHandle);

    CFE_SB_LockSharedData(__func__,__LINE__);

    if(Status == CFE_SUCCESS){
        /* give the buffer back to the buffer pool */
        BufAddr = CFE_ES_MEMPOOLBUF_C((cpuaddr)Ptr2Release - sizeof(CFE_SB_BufferD_t));
        Stat2 = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, BufAddr);
        if(Stat2 > 0){
             /* Substract the size of the actual buffer from the Memory in use ctr */
            CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat2;
            CFE_SB.StatTlmMsg.Payload.SBBuffersInUse--;
        }/* end if */
    }

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return Status;

}/* end CFE_SB_ZeroCopyReleasePtr */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleaseDesc
**
** Purpose: API used for releasing a zero copy descriptor (for zero copy mode
**          only).
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          Ptr2Release
**          BufferHandle
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyReleaseDesc(CFE_SB_Buffer_t        *Ptr2Release,
                                 CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32    Stat;
    CFE_SB_ZeroCopyD_t *zcd = (CFE_SB_ZeroCopyD_t *) BufferHandle;

    CFE_SB_LockSharedData(__func__,__LINE__);

    Stat = CFE_ES_GetPoolBufInfo(CFE_SB.Mem.PoolHdl, zcd);

    if((Ptr2Release == NULL) || (Stat < 0) || (zcd->Buffer != (void *)Ptr2Release)){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        return CFE_SB_BUFFER_INVALID;
    }

    /* delink the descriptor */
    if(zcd->Prev != NULL){
        ((CFE_SB_ZeroCopyD_t *) (zcd->Prev))->Next = zcd->Next;
    }
    if(zcd->Next != NULL){
        ((CFE_SB_ZeroCopyD_t *) (zcd->Next))->Prev = zcd->Prev;
    }
    if(CFE_SB.ZeroCopyTail == (void *)zcd){
        CFE_SB.ZeroCopyTail = zcd->Prev;
    }

    /* give the descriptor back to the buffer pool */
    Stat = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, zcd);
    if(Stat > 0){
        /* Substract the size of the actual buffer from the Memory in use ctr */
        CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat;
    }/* end if */

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return CFE_SUCCESS;

}/* end CFE_SB_ZeroCopyReleaseDesc */

/*
 * Function CFE_SB_TransmitBuffer - See API and header file for details
 */
int32 CFE_SB_TransmitBuffer(CFE_SB_Buffer_t *BufPtr,
                            CFE_SB_ZeroCopyHandle_t ZeroCopyHandle,
                            bool IncrementSequenceCount)
{
    int32              Status;
    CFE_MSG_Size_t     Size = 0;
    CFE_SB_MsgId_t     MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_SB_BufferD_t  *BufDscPtr;
    CFE_SBR_RouteId_t  RouteId;
    CFE_MSG_Type_t     MsgType;

    /* Release zero copy handle */
    Status = CFE_SB_ZeroCopyReleaseDesc(BufPtr, ZeroCopyHandle);

    if (Status == CFE_SUCCESS)
    {
        Status = CFE_SB_TransmitMsgValidate(&BufPtr->Msg, &MsgId, &Size, &RouteId);

        /* Send if route exists */
        if (Status == CFE_SUCCESS)
        {
            /* Get buffer descriptor pointer */
            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, BufPtr);

            if(CFE_SBR_IsValidRouteId(RouteId))
            {
                /* For Tlm packets, increment the seq count if requested */
                CFE_MSG_GetType(&BufPtr->Msg, &MsgType);
                if((MsgType == CFE_MSG_Type_Tlm) && IncrementSequenceCount)
                {
                    CFE_SBR_IncrementSequenceCounter(RouteId);
                    CFE_MSG_SetSequenceCount(&BufPtr->Msg, CFE_SBR_GetSequenceCounter(RouteId));
                }

                Status = CFE_SB_TransmitBufferFull(BufDscPtr, RouteId, MsgId);
            }
            else
            {
                /* Decrement use count if transmit buffer full not called */
                CFE_SB_LockSharedData(__func__, __LINE__);
                CFE_SB_DecrBufUseCnt(BufDscPtr);
                CFE_SB_UnlockSharedData(__func__, __LINE__);
            }
        }
        else
        {
            /* Increment send error counter for validation failure */
            CFE_SB_LockSharedData(__func__, __LINE__);
            CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter++;
            CFE_SB_UnlockSharedData(__func__, __LINE__);
        }
    }

    return Status;
}

#ifndef CFE_OMIT_DEPRECATED_6_8
/*
 * Function: CFE_SB_ZeroCopySend - See API and header file for details
 */
int32 CFE_SB_ZeroCopySend(CFE_SB_Buffer_t        *BufPtr,
                          CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32  Status = 0;

    Status = CFE_SB_TransmitBuffer(BufPtr, BufferHandle, true);

    return Status;

}/* end CFE_SB_ZeroCopySend */


/*
 * Function: CFE_SB_ZeroCopyPass - See API and header file for details
 */
int32 CFE_SB_ZeroCopyPass(CFE_SB_Buffer_t        *BufPtr,
                          CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32  Status = 0;

    Status = CFE_SB_TransmitBuffer(BufPtr, BufferHandle, false);

    return Status;

}/* end CFE_SB_ZeroCopyPass */
#endif

/******************************************************************************
**  Function:  CFE_SB_ReadQueue()
**
**  Purpose:
**    Read an SB message from the system queue.  The message is represented
**    by a pointer to the buffer descriptor of the message.  Several options
**    are available for the timeout, as described below.
**
**  Arguments:
**    PipeDscPtr: Pointer to pipe descriptor.
**    AppId     : Application ID of the receiving task (used for error messages).
**    Time_Out  : Timeout option; one of the following:
**                  CFE_SB_PEND_FOREVER  = wait forever until a packet arrives
**                  CFE_SB_POLL = check the pipe for packets but don't wait
**                  value in milliseconds = wait up to a specified time
**    Message   : Pointer to a variable that will receive the buffer
**                descriptor of the message.
**
**  Return:
**    CFE_SB status code indicating the result of the operation:
**      CFE_SUCCESS         = message was successfully read
**      CFE_SB_NO_MESSAGE   = no message is present (for CFE_SB_POLL option)
**      CFE_SB_TIME_OUT     = timeout expired (for timeout option)
**      CFE_SB_PIPE_RD_ERR  = an unexpected queue read error occurred
*/

int32  CFE_SB_ReadQueue (CFE_SB_PipeD_t         *PipeDscPtr,
                         CFE_ES_ResourceID_t    TskId,
                         CFE_SB_TimeOut_t       Time_Out,
                         CFE_SB_BufferD_t       **Message)
{
    int32              Status,TimeOut;
    size_t             Nbytes;
    char               FullName[(OS_MAX_API_NAME * 2)];
    char               PipeName[OS_MAX_API_NAME] = {'\0'};

    /* translate the given Time_Out value */
    switch(Time_Out){

      case CFE_SB_PEND_FOREVER:
          TimeOut = OS_PEND;
          break;

      case CFE_SB_POLL:
          TimeOut = OS_CHECK;
          break;

      default:
          TimeOut = Time_Out;
          break;

    }/* end switch */

    /* Read the buffer descriptor address from the queue.  */
    Status = OS_QueueGet(PipeDscPtr->SysQueueId,
                        (void *)Message,
                        sizeof(CFE_SB_BufferD_t *),
                        &Nbytes,
                        TimeOut);

    /* translate the return value */
    switch(Status){

      case OS_SUCCESS:
          Status = CFE_SUCCESS;
          break;

      case OS_QUEUE_EMPTY:
          Status = CFE_SB_NO_MESSAGE;
          break;

      case OS_QUEUE_TIMEOUT:
          Status = CFE_SB_TIME_OUT;
          break;

      default:
          CFE_SB_LockSharedData(__func__,__LINE__);
          CFE_SB.HKTlmMsg.Payload.InternalErrorCounter++;
          CFE_SB_UnlockSharedData(__func__,__LINE__);
          /* Unexpected error while reading the queue. */
          CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeDscPtr->PipeId);
          CFE_EVS_SendEventWithAppID(CFE_SB_Q_RD_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
              "Pipe Read Err,pipe %s,app %s,stat 0x%x",
              PipeName,CFE_SB_GetAppTskName(TskId,FullName),(unsigned int)Status);

          Status = CFE_SB_PIPE_RD_ERR;
          break;

    }/* end switch */

    return (Status);
}/* end CFE_SB_ReadQueue */

/*****************************************************************************/

