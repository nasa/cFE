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
/******************************************************************************
** Name:    CFE_SB_CreatePipe
**
** Purpose: API to create a pipe for receiving messages
**
** Assumptions, External Events, and Notes:
**
**          Note: Zero is a valid PipeId
**
** Date Written:
**          04/25/2005
**
** Inputs:
**          PipeIdPtr - Ptr to users empty PipeId variable, to be filled by
**                      this function.
**          Depth     - The depth of the pipe, synonymous to the max number
**                      of messages the pipe can hold at any time.
**          PipeName  - The name of the pipe displayed in event messages
**
** Outputs:
**          PipeId    - The handle of the pipe to be used when receiving
**                      messages.
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16  Depth, const char *PipeName)
{
    uint32          AppId = 0xFFFFFFFF;
    uint32          TskId = 0;
    uint32          SysQueueId = 0;
    int32           Status;
    CFE_SB_PipeId_t OriginalPipeIdParamValue = (PipeIdPtr == NULL) ? 0 : (*PipeIdPtr);
    CFE_SB_PipeId_t PipeTblIdx;
    char            AppName[OS_MAX_API_NAME] = {'\0'};
    char            FullName[(OS_MAX_API_NAME * 2)];

    /* get callers AppId */
    CFE_ES_GetAppID(&AppId);

    /* get callers name */
    CFE_ES_GetAppName(AppName, AppId, OS_MAX_API_NAME);

    /* Hardcode a NULL terminator, in case rcvd name was too long */
    AppName[OS_MAX_API_NAME-1]= '\0';

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    TskId = OS_TaskGetId();

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


/******************************************************************************
**  Function:  CFE_SB_DeletePipe()
**
**  Purpose:
**    Will unsubscribe to all routes associated with the given pipe id, then remove
**    pipe from the pipe table.
**
**  Arguments:
**    PipeId - The ID of the pipe to delete.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    uint32  CallerId = 0xFFFFFFFF;
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
int32 CFE_SB_DeletePipeWithAppId(CFE_SB_PipeId_t PipeId, uint32 AppId)
{
    int32   Status = 0;

    Status = CFE_SB_DeletePipeFull(PipeId,AppId);

    return Status;

}/* end CFE_SB_DeletePipeWithAppId */



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
int32 CFE_SB_DeletePipeFull(CFE_SB_PipeId_t PipeId,uint32 AppId)
{
    uint8         PipeTblIdx;
    int32         RtnFromVal,Stat;
    uint32        Owner,i;
    uint32        TskId = 0;
    CFE_SB_Msg_t  *PipeMsgPtr;
    CFE_SB_DestinationD_t *DestPtr = NULL;
    char          FullName[(OS_MAX_API_NAME * 2)];

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* get TaskId of caller for events */
    TskId = OS_TaskGetId();

    /* check input parameter */
    PipeTblIdx = CFE_SB_GetPipeIdx(PipeId);
    RtnFromVal = CFE_SB_ValidatePipeId(PipeId);
    if((RtnFromVal != CFE_SUCCESS)||(PipeTblIdx == CFE_SB_INVALID_PIPE))
    {
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR1_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Pipe Delete Error:Bad Argument,PipedId %d,Requestor %s,Idx %d,Stat %d",
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),(int)PipeTblIdx,(int)RtnFromVal);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    Owner = CFE_SB.PipeTbl[PipeTblIdx].AppId;

    /* check that the given AppId is the owner of the pipe */
    if(AppId != Owner){
        CFE_SB.HKTlmMsg.Payload.CreatePipeErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEL_PIPE_ERR2_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Pipe Delete Error:Caller(%s) is not the owner of pipe %d",
          CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check destination list of every in-use MsgId, for the given pipeid. */
    /* when found, remove the pipe ID from the destination list via 'unsubscribe' */
    for(i=0;i<CFE_PLATFORM_SB_MAX_MSG_IDS;i++)
    {
        if(CFE_SB_IsValidMsgId(CFE_SB.RoutingTbl[i].MsgId))
        {
            DestPtr = CFE_SB.RoutingTbl[i].ListHeadPtr;

            while(DestPtr != NULL){

                if(DestPtr -> PipeId == PipeId){
                    /* release the semaphore, unsubscribe will need to take it */
                    CFE_SB_UnlockSharedData(__func__,__LINE__);
                    CFE_SB_UnsubscribeWithAppId(CFE_SB.RoutingTbl[i].MsgId,
                                       PipeId,AppId);
                    CFE_SB_LockSharedData(__func__,__LINE__);
                }/* end if */

                DestPtr = DestPtr -> Next;

            }/* end while */

        }/* end if */
    }/* end for */

    if (CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff != NULL) {

        /* Decrement the Buffer Use Count and Free buffer if cnt=0) */
        CFE_SB_DecrBufUseCnt(CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff);
        CFE_SB.PipeTbl[PipeTblIdx].ToTrashBuff = NULL;

    }/* end if */


    /* remove any messages that might be on the pipe */
    /* this step will free the memory used to store the message */
    do{
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      Stat = CFE_SB_RcvMsg(&PipeMsgPtr,PipeId,CFE_SB_POLL);
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


/******************************************************************************
**  Function:  CFE_SB_SetPipeOpts()
**
**  Purpose:
**  Sets pipe options. Options are a bit field, setting each bit enables the 
**  option specified.
**
**  Arguments:
**    PipeId - The ID of the pipe for the options.
**    Opts   - The options to set on this pipe.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_SetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 Opts)
{
    uint8         PipeTblIdx = 0;
    int32         RtnFromVal = 0;
    uint32        Owner = 0, AppID = 0, TskId = 0, Status = 0;
    char          FullName[(OS_MAX_API_NAME * 2)];

    Status = CFE_ES_GetAppID(&AppID);
    if(Status != CFE_SUCCESS)
    {
        /* shouldn't happen... */
        return Status;
    }

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* get TaskId of caller for events */
    TskId = OS_TaskGetId();

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
    if(AppID != Owner){
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

/******************************************************************************
**  Function:  CFE_SB_GetPipeOpts()
**
**  Purpose:
**  Gets the current pipe options.
**
**  Arguments:
**    PipeId  - The ID of the pipe for the options.
**    OptsPtr - Pointer to receive the opts.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptsPtr)
{
    uint8         PipeTblIdx = 0;
    int32         RtnFromVal = 0;
    uint32        TskId = 0;
    char          FullName[(OS_MAX_API_NAME * 2)];

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

    /* get TaskId of caller for events */
    TskId = OS_TaskGetId();

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

/******************************************************************************
**  Function:  CFE_SB_GetPipeName()
**
**  Purpose:
**    SB internal function to get the string of the pipe name of the given
**    Pipe ID.
**
**  Arguments:
**    MsgId       : ID of the message
**    PipeNameBuf : Buffer to receive name, must be OS_MAX_API_NAME bytes long
**
**  Return:
**    Will return CFE_SUCCESS and populate PipeNameBuf with the name
**    of the pipe on success, otherwise PipeNameBuf is filled with NULLs
**    and this returns CFE_SB_BAD_ARGUMENT on error.
**
*/
int32 CFE_SB_GetPipeName(char *PipeNameBuf, size_t PipeNameSize, CFE_SB_PipeId_t PipeId){
    OS_queue_prop_t queue_prop;
    int32 Status = CFE_SUCCESS;
    uint32 TskId = 0;
    char FullName[(OS_MAX_API_NAME * 2)];

    if(PipeNameBuf == NULL || PipeNameSize == 0) {
        CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_NULL_PTR_EID, CFE_EVS_EventType_ERROR,
            CFE_SB.AppId, "Pipe Name Error:NullPtr,Requestor %s",
            CFE_SB_GetAppTskName(TskId,FullName));

        Status = CFE_SB_BAD_ARGUMENT;
    } else if(PipeId >= CFE_PLATFORM_SB_MAX_PIPES){
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
            CFE_EVS_SendEventWithAppID(CFE_SB_GETPIPENAME_ID_ERR_EID, CFE_EVS_EventType_ERROR,
                CFE_SB.AppId, "Pipe Id Error:Bad Argument,Id=%d,Requestor %s",
                PipeId,CFE_SB_GetAppTskName(TskId,FullName));

            memset(PipeNameBuf, 0, PipeNameSize);

            Status = CFE_SB_BAD_ARGUMENT;
        }/* end if */
    }/* end if */

    return Status;
}/* end CFE_SB_GetPipeName */

/******************************************************************************
**  Function:  CFE_SB_GetPipeIdByName()
**
**  Purpose:
**  Gets the pipe id given its name.
**
**  Arguments:
**    PipeIdPtr  - The ID of the pipe for the name.
**    PipeName   - The name of the pipe.
**
**  Return:
**    CFE_SUCCESS or cFE Error Code
*/
int32 CFE_SB_GetPipeIdByName(CFE_SB_PipeId_t *PipeIdPtr, const char *PipeName)
{
    uint8         PipeTblIdx = 0;
    int32         Status = CFE_SUCCESS;
    int32         RtnFromVal = 0;
    uint32        TskId = 0;
    uint32        QueueId = 0;
    char          FullName[(OS_MAX_API_NAME * 2)];

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
        /* get TaskId of caller for events */
        TskId = OS_TaskGetId();

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
                    && CFE_SB.PipeTbl[PipeTblIdx].SysQueueId == QueueId)
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


/******************************************************************************
** Name:    CFE_SB_SubscribeEx
**
** Purpose: API to globally subscribe to a message when QOS and MsgLim defaults
**          are insufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**          Note: MsgLim default: 4, see Input Arguments below for more detail
**          Note: Qos parameter is currently unused by the fsw. It is recommended
**                to give a value of zero for Quality.Priority and Quality.Reliability
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**          Quality - Quality of Service (Qos) - priority and reliability,
**                    see note above for recommended settings
**          MsgLim  - Max number of messages, with this MsgId, allowed on the
**                    pipe at any time.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SubscribeEx(CFE_SB_MsgId_t   MsgId,
                          CFE_SB_PipeId_t  PipeId,
                          CFE_SB_Qos_t     Quality,
                          uint16           MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,Quality,MsgLim,(uint8)CFE_SB_GLOBAL);

}/* end CFE_SB_SubscribeEx */




/******************************************************************************
** Name:    CFE_SB_SubscribeLocal
**
** Purpose: CFE Internal API to locally subscribe to a message when QOS and
**          MsgLim defaults are insufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**                MsgLim default: 4 - meaning at most 4 messages of given
**                                    MsgId allowed on the given PipeId.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**          MsgLim  - Max number of messages, with this MsgId, allowed on the
**                    pipe at any time.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t   MsgId,
                            CFE_SB_PipeId_t  PipeId,
                            uint16           MsgLim)
{
    return CFE_SB_SubscribeFull(MsgId,PipeId,CFE_SB_Default_Qos,MsgLim,
                                (uint8)CFE_SB_LOCAL);

}/* end CFE_SB_SubscribeLocal */




/******************************************************************************
** Name:    CFE_SB_Subscribe
**
** Purpose: API to locally subscribe to a message when QOS and MsgLim defaults
**          are sufficient.
**
** Assumptions, External Events, and Notes:
**
**          Note: Qos default: priority is low, reliability is low
**                MsgLim default: 4 - meaning at most 4 messages of given
**                                    MsgId allowed on the given PipeId.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId   - The requesting message
**          PipeId  - The Pipe ID to send the message to
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
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
    CFE_SB_MsgRouteIdx_t RouteIdx;
    CFE_SB_RouteEntry_t* RoutePtr;
    int32  Stat;
    CFE_SB_MsgKey_t   MsgKey;
    uint32 TskId = 0;
    uint32 AppId = 0xFFFFFFFF;
    uint8  PipeIdx;
    CFE_SB_DestinationD_t *DestBlkPtr = NULL;
    char   FullName[(OS_MAX_API_NAME * 2)];
    char   PipeName[OS_MAX_API_NAME] = {'\0'};

    CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* get task id for events */
    TskId = OS_TaskGetId();

    /* get the callers Application Id */
    CFE_ES_GetAppID(&AppId);

    /* check that the pipe has been created */
    PipeIdx = CFE_SB_GetPipeIdx(PipeId);
    if(PipeIdx==CFE_SB_INVALID_PIPE){
      CFE_SB.HKTlmMsg.Payload.SubscribeErrorCounter++;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_PIPE_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Subscribe Err:Invalid Pipe Id,Msg=0x%x,PipeId=%d,App %s",(unsigned int)MsgId,(int)PipeId,
          CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check that the requestor is the owner of the pipe */
    if(CFE_SB.PipeTbl[PipeIdx].AppId != AppId){
      CFE_SB.HKTlmMsg.Payload.SubscribeErrorCounter++;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUB_INV_CALLER_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Subscribe Err:Caller(%s) is not the owner of pipe %d,Msg=0x%x",
          CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId,(unsigned int)MsgId);
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* check message id key and scope */
    if(!CFE_SB_IsValidMsgId(MsgId) || (Scope > 1))
    {
        CFE_SB.HKTlmMsg.Payload.SubscribeErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SUB_ARG_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "Subscribe Err:Bad Arg,MsgId 0x%x,PipeId %d,app %s,scope %d",
          (unsigned int)MsgId,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),Scope);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* Convert the API MsgId into the SB internal representation MsgKey */
    MsgKey = CFE_SB_ConvertMsgIdtoMsgKey(MsgId);

    /* check for duplicate subscription */
    if(CFE_SB_DuplicateSubscribeCheck(MsgKey,PipeId)==CFE_SB_DUPLICATE){
        CFE_SB.HKTlmMsg.Payload.DuplicateSubscriptionsCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DUP_SUBSCRIP_EID,CFE_EVS_EventType_INFORMATION,CFE_SB.AppId,
          "Duplicate Subscription,MsgId 0x%x on %s pipe,app %s",
           (unsigned int)MsgId,PipeName,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SUCCESS;
    }/* end if */

    /*
    ** If there has been a subscription for this message id earlier,
    ** get the element number in the routing table.
    */
    RouteIdx = CFE_SB_GetRoutingTblIdx(MsgKey);

    /* if not first subscription for this message KEY ... */
    if(CFE_SB_IsValidRouteIdx(RouteIdx))
    {
        RoutePtr = CFE_SB_GetRoutePtrFromIdx(RouteIdx);

        /*
         * FIXME: If a hash or other conversion is used between MsgId and MsgKey,
         * then it is possible that this existing route is for a different MsgId.
         *
         * The MsgId should be checked against the "MsgId" in the route here.
         *
         * However it is not possible to have a mismatch in the default case where
         * MsgKey == MsgId
         */
    }
    else
    {
        /* Get the index to the first available element in the routing table */
        RouteIdx = CFE_SB_RouteIdxPop_Unsync();

        /* if all routing table elements are used, send event */
        if(!CFE_SB_IsValidRouteIdx(RouteIdx)){
            CFE_SB_UnlockSharedData(__func__,__LINE__);
            CFE_EVS_SendEventWithAppID(CFE_SB_MAX_MSGS_MET_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
              "Subscribe Err:Max Msgs(%d)In Use,MsgId 0x%x,pipe %s,app %s",
              CFE_PLATFORM_SB_MAX_MSG_IDS,(unsigned int)MsgId,PipeName,CFE_SB_GetAppTskName(TskId,FullName));
            return CFE_SB_MAX_MSGS_MET;
        }/* end if */

        /* Increment the MsgIds in use ctr and if it's > the high water mark,*/
        /* adjust the high water mark */
        CFE_SB.StatTlmMsg.Payload.MsgIdsInUse++;
        if(CFE_SB.StatTlmMsg.Payload.MsgIdsInUse > CFE_SB.StatTlmMsg.Payload.PeakMsgIdsInUse){
           CFE_SB.StatTlmMsg.Payload.PeakMsgIdsInUse = CFE_SB.StatTlmMsg.Payload.MsgIdsInUse;
        }/* end if */

        /* populate the look up table with the routing table index */
        CFE_SB_SetRoutingTblIdx(MsgKey,RouteIdx);

        /* label the new routing block with the message identifier */
        RoutePtr = CFE_SB_GetRoutePtrFromIdx(RouteIdx);
        RoutePtr->MsgId = MsgId;

    }/* end if */

    if(RoutePtr->Destinations >= CFE_PLATFORM_SB_MAX_DEST_PER_PKT){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_MAX_DESTS_MET_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Subscribe Err:Max Dests(%d)In Use For Msg 0x%x,pipe %s,app %s",
             CFE_PLATFORM_SB_MAX_DEST_PER_PKT,(unsigned int)MsgId,PipeName,
             CFE_SB_GetAppTskName(TskId,FullName));

        return CFE_SB_MAX_DESTS_MET;
    }/* end if */

    DestBlkPtr = CFE_SB_GetDestinationBlk();
    if(DestBlkPtr == NULL){
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_DEST_BLK_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Subscribe Err:Request for Destination Blk failed for Msg 0x%x", (unsigned int)MsgId);
        return CFE_SB_BUF_ALOC_ERR;
    }/* end if */

    /* initialize destination block */
    DestBlkPtr -> PipeId = PipeId;
    DestBlkPtr -> MsgId2PipeLim = (uint16)MsgLim;
    DestBlkPtr -> Active = CFE_SB_ACTIVE;
    DestBlkPtr -> BuffCount = 0;
    DestBlkPtr -> DestCnt = 0;
    DestBlkPtr -> Scope = Scope;
    DestBlkPtr -> Prev = NULL;
    DestBlkPtr -> Next = NULL;

    /* add destination block to head of list */
    CFE_SB_AddDest(RoutePtr, DestBlkPtr);

    RoutePtr->Destinations++;

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
      Stat = CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.SubRprtMsg);
      CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RPT_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
            "Sending Subscription Report Msg=0x%x,Pipe=%d,Stat=0x%x",
            (unsigned int)MsgId,(int)PipeId,(unsigned int)Stat);
      CFE_SB_LockSharedData(__func__,__LINE__);/* to prevent back-to-back unlock */
    }/* end if */

    /* release the semaphore */
    CFE_SB_UnlockSharedData(__func__,__LINE__);

    CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RCVD_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
        "Subscription Rcvd:MsgId 0x%x on %s(%d),app %s",
         (unsigned int)MsgId,PipeName,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));

    return CFE_SUCCESS;

}/* end CFE_SB_SubscribeFull */


/******************************************************************************
** Name:    CFE_SB_Unsubscribe
**
** Purpose: API used to unsubscribe to a message.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId
**          PipeId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    uint32  CallerId = 0xFFFFFFFF;
    int32   Status = 0;

    /* get the callers Application Id */
    CFE_ES_GetAppID(&CallerId);

    Status = CFE_SB_UnsubscribeFull(MsgId, PipeId, (uint8)CFE_SB_GLOBAL,CallerId);

    return Status;

}/* end CFE_SB_Unsubscribe */


/******************************************************************************
** Name:    CFE_SB_UnsubscribeLocal
**
** Purpose: CFE Internal API used to locally unsubscribe to a message. This
**          request to cancel a subscription will not be sent to peers.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgId
**          PipeId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    uint32  CallerId = 0xFFFFFFFF;
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
                              uint32 AppId)
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
                             uint8 Scope,uint32 AppId)
{
    CFE_SB_MsgKey_t MsgKey;
    CFE_SB_MsgRouteIdx_t RouteIdx;
    CFE_SB_RouteEntry_t* RoutePtr;
    uint32  PipeIdx;
    uint32  TskId = 0;
    bool    MatchFound = false;
    CFE_SB_DestinationD_t   *DestPtr = NULL;
    char    FullName[(OS_MAX_API_NAME * 2)];


    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    /* get task id for events */
    TskId = OS_TaskGetId();

    /* check that the pipe has been created */
    PipeIdx = CFE_SB_GetPipeIdx(PipeId);
    if(PipeIdx==CFE_SB_INVALID_PIPE){
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_PIPE_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Unsubscribe Err:Invalid Pipe Id Msg=0x%x,Pipe=%d,app=%s",
            (unsigned int)MsgId,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* if given 'AppId' is not the owner of the pipe, send error event and return */
    if(CFE_SB.PipeTbl[PipeIdx].AppId != AppId){
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_INV_CALLER_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Unsubscribe Err:Caller(%s) is not the owner of pipe %d,Msg=0x%x",
            CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId,(unsigned int)MsgId);
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
            (unsigned int)MsgId,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName),(int)Scope);
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* get index into routing table */
    MsgKey = CFE_SB_ConvertMsgIdtoMsgKey(MsgId);
    RouteIdx = CFE_SB_GetRoutingTblIdx(MsgKey);

    /* if there are no subscriptions for this message id... */
    if(!CFE_SB_IsValidRouteIdx(RouteIdx)){
        char    PipeName[OS_MAX_API_NAME] = {'\0'};

        CFE_SB_UnlockSharedData(__func__,__LINE__);

        CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId);

        CFE_EVS_SendEventWithAppID(CFE_SB_UNSUB_NO_SUBS_EID,CFE_EVS_EventType_INFORMATION,CFE_SB.AppId,
            "Unsubscribe Err:No subs for Msg 0x%x on %s,app %s",
            (unsigned int)MsgId,PipeName,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SUCCESS;
    }/* end if */

    /* At this point, there must be at least one destination. */
    /* So the value of 'ListHeadPtr' will not be NULL by design */
    RoutePtr = CFE_SB_GetRoutePtrFromIdx(RouteIdx);

    /* search the list for a matching pipe id */
    DestPtr = RoutePtr->ListHeadPtr;

    do{

        if(DestPtr->PipeId == PipeId){
            /* match found, remove node from list */
            CFE_SB_RemoveDest(RoutePtr,DestPtr);

            /* return node to memory pool */
            CFE_SB_PutDestinationBlk(DestPtr);

            RoutePtr->Destinations--;
            CFE_SB.StatTlmMsg.Payload.SubscriptionsInUse--;

            MatchFound = true;

        }/* end if */

        DestPtr = DestPtr->Next;

    }while((MatchFound == false)&&(DestPtr != NULL));

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_REMOVED_EID,CFE_EVS_EventType_DEBUG,CFE_SB.AppId,
            "Subscription Removed:Msg 0x%x on pipe %d,app %s",
            (unsigned int)MsgId,(int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));

    return CFE_SUCCESS;

}/* end CFE_SB_UnsubscribeFull */



/******************************************************************************
** Name:    CFE_SB_SendMsg
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function increments and tracks the source sequence
**                counter for all telemetry messages.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SendMsg(CFE_SB_Msg_t    *MsgPtr)
{
    int32   Status = 0;

    Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_INCREMENT_TLM,CFE_SB_SEND_ONECOPY);

    return Status;

}/* end CFE_SB_SendMsg */



/******************************************************************************
** Name:    CFE_SB_PassMsg
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function does NOT increment and track the source
**                sequence counter for telemetry messages.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_PassMsg(CFE_SB_Msg_t    *MsgPtr)
{
    int32   Status = 0;

    Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_DO_NOT_INCREMENT,CFE_SB_SEND_ONECOPY);

    return Status;

}/* end CFE_SB_PassMsg */



/******************************************************************************
** Name:    CFE_SB_SendMsgFull
**
** Purpose: API used to send a message on the software bus.
**
** Assumptions, External Events, and Notes:
**
**          Note: This function increments and tracks the source sequence
**                counter for all telemetry messages.
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr
**          TlmCntIncrements
**          CopyMode
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_SendMsgFull(CFE_SB_Msg_t    *MsgPtr,
                          uint32           TlmCntIncrements,
                          uint32           CopyMode)
{
    CFE_SB_MsgKey_t         MsgKey;
    CFE_SB_MsgId_t          MsgId;
    int32                   Status;
    CFE_SB_DestinationD_t   *DestPtr = NULL;
    CFE_SB_PipeD_t          *PipeDscPtr;
    CFE_SB_RouteEntry_t     *RtgTblPtr;
    CFE_SB_BufferD_t        *BufDscPtr;
    uint16                  TotalMsgSize;
    CFE_SB_MsgRouteIdx_t    RtgTblIdx;
    uint32                  TskId = 0;
    uint16                  i;
    char                    FullName[(OS_MAX_API_NAME * 2)];
    CFE_SB_EventBuf_t       SBSndErr;
    char                    PipeName[OS_MAX_API_NAME] = {'\0'};

    SBSndErr.EvtsToSnd = 0;

    /* get task id for events and Sender Info*/
    TskId = OS_TaskGetId();

    /* check input parameter */
    if(MsgPtr == NULL){
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_BAD_ARG_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Send Err:Bad input argument,Arg 0x%lx,App %s",
            (unsigned long)MsgPtr,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    MsgId = CFE_SB_GetMsgId(MsgPtr);

    /* validate the msgid in the message */
    if(!CFE_SB_IsValidMsgId(MsgId))
    {
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter++;
        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        {
            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
            CFE_SB_DecrBufUseCnt(BufDscPtr);
        }
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_SEND_INV_MSGID_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Send Err:Invalid MsgId(0x%x)in msg,App %s",
            (unsigned int)MsgId,CFE_SB_GetAppTskName(TskId,FullName));
        return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    TotalMsgSize = CFE_SB_GetTotalMsgLength(MsgPtr);

    /* Verify the size of the pkt is < or = the mission defined max */
    if(TotalMsgSize > CFE_MISSION_SB_MAX_SB_MSG_SIZE){
        CFE_SB_LockSharedData(__func__,__LINE__);
        CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter++;
        if (CopyMode == CFE_SB_SEND_ZEROCOPY)
        {
            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
            CFE_SB_DecrBufUseCnt(BufDscPtr);
        }
        CFE_SB_UnlockSharedData(__func__,__LINE__);
        CFE_EVS_SendEventWithAppID(CFE_SB_MSG_TOO_BIG_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
            "Send Err:Msg Too Big MsgId=0x%x,app=%s,size=%d,MaxSz=%d",
            (unsigned int)MsgId,CFE_SB_GetAppTskName(TskId,FullName),(int)TotalMsgSize,CFE_MISSION_SB_MAX_SB_MSG_SIZE);
        return CFE_SB_MSG_TOO_BIG;
    }/* end if */

    MsgKey = CFE_SB_ConvertMsgIdtoMsgKey(MsgId);

    /* take semaphore to prevent a task switch during this call */
    CFE_SB_LockSharedData(__func__,__LINE__);

    RtgTblIdx = CFE_SB_GetRoutingTblIdx(MsgKey);

    /* if there have been no subscriptions for this pkt, */
    /* increment the dropped pkt cnt, send event and return success */
    if(!CFE_SB_IsValidRouteIdx(RtgTblIdx)){

        CFE_SB.HKTlmMsg.Payload.NoSubscribersCounter++;

        if (CopyMode == CFE_SB_SEND_ZEROCOPY){
            BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
            CFE_SB_DecrBufUseCnt(BufDscPtr);
        }

        CFE_SB_UnlockSharedData(__func__,__LINE__);

        /* Determine if event can be sent without causing recursive event problem */
        if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT) == CFE_SB_GRANTED){

           CFE_EVS_SendEventWithAppID(CFE_SB_SEND_NO_SUBS_EID,CFE_EVS_EventType_INFORMATION,CFE_SB.AppId,
              "No subscribers for MsgId 0x%x,sender %s",
              (unsigned int)MsgId,CFE_SB_GetAppTskName(TskId,FullName));

           /* clear the bit so the task may send this event again */
           CFE_SB_FinishSendEvent(TskId,CFE_SB_SEND_NO_SUBS_EID_BIT);
        }/* end if */

        return CFE_SUCCESS;
    }/* end if */

    /* Allocate a new buffer. */
    if (CopyMode == CFE_SB_SEND_ZEROCOPY){
        BufDscPtr = CFE_SB_GetBufferFromCaller(MsgId, MsgPtr);
    }
    else{
        BufDscPtr = CFE_SB_GetBufferFromPool(MsgId, TotalMsgSize);
    }
    if (BufDscPtr == NULL){
        CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter++;
        CFE_SB_UnlockSharedData(__func__,__LINE__);

        /* Determine if event can be sent without causing recursive event problem */
        if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_GET_BUF_ERR_EID_BIT) == CFE_SB_GRANTED){

            CFE_EVS_SendEventWithAppID(CFE_SB_GET_BUF_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
              "Send Err:Request for Buffer Failed. MsgId 0x%x,app %s,size %d",
              (unsigned int)MsgId,CFE_SB_GetAppTskName(TskId,FullName),(int)TotalMsgSize);

            /* clear the bit so the task may send this event again */
            CFE_SB_FinishSendEvent(TskId,CFE_SB_GET_BUF_ERR_EID_BIT);
        }/* end if */

        return CFE_SB_BUF_ALOC_ERR;
    }/* end if */

    /* Copy the packet into the SB memory space */
    if (CopyMode != CFE_SB_SEND_ZEROCOPY){
        /* Copy the packet into the SB memory space */
        memcpy( BufDscPtr->Buffer, MsgPtr, (uint16)TotalMsgSize );
    }

    /* Obtain the actual routing table entry from the selected index */
    RtgTblPtr = CFE_SB_GetRoutePtrFromIdx(RtgTblIdx);

    /* For Tlm packets, increment the seq count if requested */
    if((CFE_SB_GetPktType(MsgId)==CFE_SB_TLM) &&
       (TlmCntIncrements==CFE_SB_INCREMENT_TLM)){
        RtgTblPtr->SeqCnt++;
        CFE_SB_SetMsgSeqCnt((CFE_SB_Msg_t *)BufDscPtr->Buffer,
                RtgTblPtr->SeqCnt);
    }/* end if */

    /* store the sender information */
    if(CFE_SB.SenderReporting != 0)
    {
       BufDscPtr->Sender.ProcessorId = CFE_PSP_GetProcessorId();
       strncpy(&BufDscPtr->Sender.AppName[0],CFE_SB_GetAppTskName(TskId,FullName),OS_MAX_API_NAME);
    }

    /* At this point there must be at least one destination for pkt */

    /* Send the packet to all destinations  */
    for (i=0, DestPtr = RtgTblPtr -> ListHeadPtr;
            i < RtgTblPtr -> Destinations; i++, DestPtr = DestPtr -> Next)
    {
        /* The DestPtr should never be NULL in this loop, this is just extra
           protection in case of the unforseen */
        if(DestPtr == NULL)
        {
          break;
        }

        if (DestPtr->Active == CFE_SB_INACTIVE)    /* destination is active */
        {
            continue;
        }/*end if */

        PipeDscPtr = &CFE_SB.PipeTbl[DestPtr->PipeId];

        if(PipeDscPtr->Opts & CFE_SB_PIPEOPTS_IGNOREMINE)
        {
            uint32 AppId = 0xFFFFFFFF;

            CFE_ES_GetAppID(&AppId);

            if(PipeDscPtr->AppId == AppId)
            {
                continue;
            }
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
                Status = OS_QueuePut(PipeDscPtr->SysQueueId,(void *)&BufDscPtr,
                                     sizeof(CFE_SB_BufferD_t *),0);

        if (Status == OS_SUCCESS) {
            BufDscPtr->UseCount++;    /* used for releasing buffer  */
            DestPtr->BuffCount++; /* used for checking MsgId2PipeLimit */
            DestPtr->DestCnt++;   /* used for statistics */
            if (DestPtr->PipeId < CFE_SB_TLM_PIPEDEPTHSTATS_SIZE)
            {
                CFE_SB_PipeDepthStats_t *StatObj =
                        &CFE_SB.StatTlmMsg.Payload.PipeDepthStats[DestPtr->PipeId];
                StatObj->InUse++;
                if(StatObj->InUse > StatObj->PeakInUse){
                    StatObj->PeakInUse = StatObj->InUse;
                }/* end if */
            }

        }else if(Status == OS_QUEUE_FULL) {

            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].PipeId  = DestPtr->PipeId;
            SBSndErr.EvtBuf[SBSndErr.EvtsToSnd].EventId = CFE_SB_Q_FULL_ERR_EID;
            SBSndErr.EvtsToSnd++;
            CFE_SB.HKTlmMsg.Payload.PipeOverflowErrorCounter++;
            PipeDscPtr->SendErrors++;


        }else{ /* Unexpected error while writing to queue. */

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
                (unsigned int)RtgTblPtr->MsgId, PipeName,
                CFE_SB_GetAppTskName(TskId,FullName));

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
                  (unsigned int)RtgTblPtr->MsgId, PipeName,
                  CFE_SB_GetAppTskName(TskId,FullName));

               /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_Q_FULL_ERR_EID_BIT);
            }/* end if */

        }else{

            /* Determine if event can be sent without causing recursive event problem */
            if(CFE_SB_RequestToSendEvent(TskId,CFE_SB_Q_WR_ERR_EID_BIT) == CFE_SB_GRANTED){

              CFE_SB_GetPipeName(PipeName, sizeof(PipeName), SBSndErr.EvtBuf[i].PipeId);

              CFE_EVS_SendEventWithAppID(CFE_SB_Q_WR_ERR_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
                "Pipe Write Err,MsgId 0x%x,pipe %s,sender %s,stat 0x%x",
                (unsigned int)RtgTblPtr->MsgId, PipeName,
                CFE_SB_GetAppTskName(TskId,FullName),
                (unsigned int)SBSndErr.EvtBuf[i].ErrStat);

               /* clear the bit so the task may send this event again */
              CFE_SB_FinishSendEvent(TskId,CFE_SB_Q_WR_ERR_EID_BIT);
            }/* end if */

        }/* end if */
    }


    return CFE_SUCCESS;

}/* end CFE_SB_SendMsgFull */



/******************************************************************************
** Name:    CFE_SB_RcvMsg
**
** Purpose: API used to receive a message from the software bus.
**
** Assumptions, External Events, and Notes:
**          Notes:
**
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          PipeId
**          BufPtr
**          timeOut - CFE_SB_PEND, CFE_SB_POLL or millisecond timeout
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32  CFE_SB_RcvMsg(CFE_SB_MsgPtr_t    *BufPtr,
                     CFE_SB_PipeId_t    PipeId,
                     int32              TimeOut)
{
    int32                  Status;
    CFE_SB_BufferD_t       *Message;
    CFE_SB_PipeD_t         *PipeDscPtr;
    CFE_SB_DestinationD_t  *DestPtr = NULL;
    uint32                 TskId = 0;
    char                   FullName[(OS_MAX_API_NAME * 2)];

    /* get task id for events */
    TskId = OS_TaskGetId();

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
        ** the buffer can be released on the next RcvMsg call for this pipe.
        */
        PipeDscPtr->CurrentBuff = Message;

        /* Set the Receivers pointer to the address of the actual message */
        *BufPtr = (CFE_SB_MsgPtr_t) Message->Buffer;

        /* get pointer to destination to be used in decrementing msg limit cnt*/
        DestPtr = CFE_SB_GetDestPtr(CFE_SB_ConvertMsgIdtoMsgKey(PipeDscPtr->CurrentBuff->MsgId), PipeDscPtr->PipeId);

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

}/* end CFE_SB_RcvMsg */


/******************************************************************************
** Name:    CFE_SB_GetLastSenderId
**
** Purpose: API used for receiving sender Information of the last message received on
**          the given pipe.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          06/13/2007
**
** Input Arguments:
**          Ptr -     caller declares a ptr of type CFE_SB_SenderId_t then gives the
**                    address of that pointer.
**          PipeId -  the Pipe Id that the message was received on.
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
uint32  CFE_SB_GetLastSenderId(CFE_SB_SenderId_t **Ptr,CFE_SB_PipeId_t  PipeId)
{

    CFE_SB_BufferD_t *Ptr2BufDescriptor;
    uint32            TskId = 0;
    uint32            AppId = 0xFFFFFFFF;
    char              FullName[(OS_MAX_API_NAME * 2)];

    TskId = OS_TaskGetId();

    /* validate ptr  - note: must validate ptr before pipe id validation  */
    /* because an invalid pipe id sets the callers pointer to NULL */
    if(Ptr == NULL){
      CFE_EVS_SendEventWithAppID(CFE_SB_LSTSNDER_ERR1_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "SB GetLastSender Err:Rcvd Null Ptr,Pipe=%d,App=%s",
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* validate pipe id */
    if(CFE_SB_ValidatePipeId(PipeId)!=CFE_SUCCESS){
      *Ptr = NULL;
      CFE_EVS_SendEventWithAppID(CFE_SB_LSTSNDER_ERR2_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "SB GetLastSender Err:Rcvd Invalid Pipe=%d,App=%s",
          (int)PipeId,CFE_SB_GetAppTskName(TskId,FullName));
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    CFE_ES_GetAppID(&AppId);

    CFE_SB_LockSharedData(__func__,__LINE__);

    /* verify requestor is owner of pipe */
    if(CFE_SB.PipeTbl[PipeId].AppId != AppId){
      *Ptr = NULL;
      CFE_SB_UnlockSharedData(__func__,__LINE__);
      CFE_EVS_SendEventWithAppID(CFE_SB_GLS_INV_CALLER_EID,CFE_EVS_EventType_ERROR,CFE_SB.AppId,
          "SB GetLastSender Err:Caller(%s) is not the owner of pipe %d",
          CFE_SB_GetAppTskName(TskId,FullName),(int)PipeId);
      return CFE_SB_BAD_ARGUMENT;
    }/* end if */

    /* Get ptr to buffer descriptor for the last msg received on the given pipe */
    Ptr2BufDescriptor = CFE_SB.PipeTbl[PipeId].CurrentBuff;

    /* Set the receivers pointer to the adr of 'Sender' struct in buf descriptor */
    *Ptr = (CFE_SB_SenderId_t *) &Ptr2BufDescriptor -> Sender;

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return CFE_SUCCESS;

}/* end CFE_SB_GetLastSenderId */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyGetPtr
**
** Purpose: API used for for getting a pointer to a buffer (for zero copy mode
**          only)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgSize
**
** Output Arguments:
**          BufferHandle
**
** Return Values:
**          Pointer to an empty buffer in SB Memory space, or NULL if the buffer
**          could not be allocated.
**
******************************************************************************/

CFE_SB_Msg_t  *CFE_SB_ZeroCopyGetPtr(uint16 MsgSize,
                                     CFE_SB_ZeroCopyHandle_t *BufferHandle)
{
   int32                stat1;
   uint32               AppId = 0xFFFFFFFF;
   cpuaddr              address = 0;
   CFE_SB_ZeroCopyD_t  *zcd = NULL;
   CFE_SB_BufferD_t    *bd = NULL;

    CFE_SB_LockSharedData(__func__,__LINE__);

    /* Allocate a new zero copy descriptor from the SB memory pool.*/
    stat1 = CFE_ES_GetPoolBuf((uint32 **)&zcd, CFE_SB.Mem.PoolHdl,  sizeof(CFE_SB_ZeroCopyD_t));
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
    stat1 = CFE_ES_GetPoolBuf((uint32 **)&bd, CFE_SB.Mem.PoolHdl, MsgSize + sizeof(CFE_SB_BufferD_t));
    if((stat1 < 0)||(bd==NULL)){
        /*deallocate the first buffer if the second buffer creation fails*/
        stat1 = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)zcd);
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

    return (CFE_SB_Msg_t *)address;

}/* CFE_SB_ZeroCopyGetPtr */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleasePtr
**
** Purpose: API used for releasing a pointer to a buffer (for zero copy mode
**          only) This function is typically not needed for zero copy transfers.
**          This function is needed only when a ptr is received via
**          CFE_SB_ZeroCopyGetPtr, but never used in a send.
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
int32 CFE_SB_ZeroCopyReleasePtr(CFE_SB_Msg_t  *Ptr2Release,
                                CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32    Status;
    int32    Stat2;
    cpuaddr  Addr = (cpuaddr)Ptr2Release;

    Status = CFE_SB_ZeroCopyReleaseDesc(Ptr2Release, BufferHandle);

    CFE_SB_LockSharedData(__func__,__LINE__);

    if(Status == CFE_SUCCESS){
        /* give the buffer back to the buffer pool */
        Stat2 = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl,
                                  (uint32 *) (Addr - sizeof(CFE_SB_BufferD_t)));
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
int32 CFE_SB_ZeroCopyReleaseDesc(CFE_SB_Msg_t  *Ptr2Release,
                                 CFE_SB_ZeroCopyHandle_t  BufferHandle)
{
    int32    Stat;
    CFE_SB_ZeroCopyD_t *zcd = (CFE_SB_ZeroCopyD_t *) BufferHandle;

    CFE_SB_LockSharedData(__func__,__LINE__);

    Stat = CFE_ES_GetPoolBufInfo(CFE_SB.Mem.PoolHdl, (uint32 *)zcd);

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
    Stat = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)zcd);
    if(Stat > 0){
        /* Substract the size of the actual buffer from the Memory in use ctr */
        CFE_SB.StatTlmMsg.Payload.MemInUse-=Stat;
    }/* end if */

    CFE_SB_UnlockSharedData(__func__,__LINE__);

    return CFE_SUCCESS;

}/* end CFE_SB_ZeroCopyReleaseDesc */


/******************************************************************************
** Name:    CFE_SB_ZeroCopySend
**
** Purpose: API for sending messages in zero copy mode (with telemetry source
**          sequence count incrementing)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr - Pointer to message to be sent
**          BufferHandle - Handle supplied by the get pointer call
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopySend(CFE_SB_Msg_t   *MsgPtr,
                          CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32   Status = 0;

    Status = CFE_SB_ZeroCopyReleaseDesc(MsgPtr, BufferHandle);

    if(Status == CFE_SUCCESS){
        Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_INCREMENT_TLM,CFE_SB_SEND_ZEROCOPY);
    }

    return Status;

}/* end CFE_SB_ZeroCopySend */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyPass
**
** Purpose: API for sending messages in zero copy mode (telemetry source
**          sequence count is preserved)
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          04/25/2005
**
** Input Arguments:
**          MsgPtr - Pointer to message to be sent
**          BufferHandle - Handle supplied by the get pointer call
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyPass(CFE_SB_Msg_t   *MsgPtr,
                          CFE_SB_ZeroCopyHandle_t BufferHandle)
{
    int32   Status = 0;

    Status = CFE_SB_ZeroCopyReleaseDesc(MsgPtr, BufferHandle);

    if(Status == CFE_SUCCESS){
        Status = CFE_SB_SendMsgFull(MsgPtr,CFE_SB_DO_NOT_INCREMENT,CFE_SB_SEND_ZEROCOPY);
    }

    return Status;

}/* end CFE_SB_ZeroCopyPass */


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
                         uint32                 TskId,
                         CFE_SB_TimeOut_t       Time_Out,
                         CFE_SB_BufferD_t       **Message)
{
    int32              Status,TimeOut;
    uint32             Nbytes;
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

