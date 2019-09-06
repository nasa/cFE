/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
** File: cfe_sb_priv.c
**
** Purpose:
**   This header file contains prototypes for private functions and type
**   definitions for cFE internal use.
**
** Author:   R.McGraw/SSI
**
** Notes: 

**      The following 4 terms have been or are used in the cFS architecture and implementation
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
**       Some functions have EXTERNAL SYNC REQUIREMENTS
**
**       SB functions marked with "Unsync" in their name are designated
**       as functions which are _not_ safe to be called concurrently by multiple
**       threads, and also do _not_ implement any locking or protection.  These
**       functions expect the caller to perform all thread synchronization before
**       calling it.
**
**       The synchronization requirement is across all functions; i.e. it is not safe
**       to call B_Unsync() while A_Unsync() is executing or vice-versa.  The external
**       lock must wait until A_Unsync() finishes before calling B_Unsync().
**
**       The expectation is that the required level of synchronization can be achieved
**       using the SB shared data lock.  
**
******************************************************************************/

/*
** Include Files
*/

#include "common_types.h"
#include "osapi.h"
#include "private/cfe_private.h"
#include "cfe_sb_priv.h"
#include "cfe_sb_msg_id_util.h"
#include "cfe_sb.h"
#include "ccsds.h"
#include "cfe_error.h"
#include "cfe_es.h"
#include "cfe_sb_msg_id_util.h"
#include <string.h>

/******************************************************************************
**  Function:  CFE_SB_InitIdxStack()
**
**  Purpose: Initialize a push/pop stack of routing table indexes.
**           On init each must be unique. After system initialization SB_Idx_top
**           will always point/index to the next available routing table index
**
**  Arguments:
**
**  Return:
**    None
*/

void CFE_SB_InitIdxStack(void)
{
   uint16 i;

   CFE_SB.RouteIdxTop = 0;
   for (i=0; i<CFE_PLATFORM_SB_MAX_MSG_IDS; i++)
    {
       CFE_SB.RouteIdxStack[i] = CFE_SB_ValueToRouteIdx(i);
    }
}


/******************************************************************************
**  Function:  CFE_SB_CleanUpApp()
**
**  Purpose:
**
**  Arguments:
**
**  Return:
**    None
*/
int32 CFE_SB_CleanUpApp(uint32 AppId){

  uint32 i;

  /* loop through the pipe table looking for pipes owned by AppId */
  for(i=0;i<CFE_PLATFORM_SB_MAX_PIPES;i++){
    if((CFE_SB.PipeTbl[i].InUse == CFE_SB_IN_USE)&&
       (CFE_SB.PipeTbl[i].AppId == AppId))
    {
      CFE_SB_DeletePipeWithAppId(CFE_SB.PipeTbl[i].PipeId,AppId);
    }/* end if */
  }/* end for */

  /* Release any zero copy buffers */
  CFE_SB_ZeroCopyReleaseAppId(AppId);

  return CFE_SUCCESS;

}/* end CFE_SB_CleanUpApp */


/******************************************************************************
**  Function:  CFE_SB_GetAvailPipeIdx()
**
**  Purpose:
**    SB internal function to get the next available Pipe descriptor. Typically
**    called when a pipe is being created.
**
**  Arguments:
**    None
**
**  Return:
**    Returns the index of an empty pipe descriptor (which is also the PipeId)
**    or CFE_SB_INVALID_PIPE if there are no pipe descriptors available.
*/
CFE_SB_PipeId_t CFE_SB_GetAvailPipeIdx(void){

    uint8 i;

    /* search for next available pipe entry */
    for(i=0;i<CFE_PLATFORM_SB_MAX_PIPES;i++){

        if(CFE_SB.PipeTbl[i].InUse == CFE_SB_NOT_IN_USE){
            return i;
        }/* end if */

    }/* end for */

    return CFE_SB_INVALID_PIPE;

}/* end CFE_SB_GetAvailPipeIdx */

/******************************************************************************
**  Function:  CFE_SB_RouteIdxPop_Unsync()
**
**  Purpose:
**    SB internal function to get the next available Routing Table element
**    (CFE_SB_RouteEntry_t). Typically called when an application subscribes
**    to a message.
**
**  Assumptions, External Events, and Notes:
**      Calls to this function assumed to be protected by a semaphore
**  Arguments:
**      None
**
**  Return:
**    Returns the index of an empty Routing Table element or
**    CFE_SB_INVALID_ROUTE_IDX if there are no more elements available.
*/
CFE_SB_MsgRouteIdx_t CFE_SB_RouteIdxPop_Unsync (void) {

    CFE_SB_MsgRouteIdx_t retValue;

    /* This stack grows from 0 to (CFE_PLATFORM_SB_MAX_MSG_IDS - 1) */
    if (CFE_SB.RouteIdxTop >= CFE_PLATFORM_SB_MAX_MSG_IDS) {
        retValue = CFE_SB_INVALID_ROUTE_IDX; /* no more Idx remaining, all used */
    } else {    
        retValue = CFE_SB.RouteIdxStack[CFE_SB.RouteIdxTop];
        ++CFE_SB.RouteIdxTop;
    }

    return (retValue);
} /* end CFE_SB_IdxPop_Unsync */


/******************************************************************************
**  Function:  CFE_SB_RouteIdxPush_Unsync()
**
**  Purpose:
**    SB internal function to return a Routing Table element to the available stack
**    (CFE_SB_RouteEntry_t). Typically called when an application un-subscribes
**    to a message. 0 is a valid idx.
**
**  Assumptions, External Events, and Notes:
**      Calls to this function assumed to be protected by a semaphore
** 
**  Arguments:
**    None
**
**  Return:
**    None
*/
void CFE_SB_RouteIdxPush_Unsync (CFE_SB_MsgRouteIdx_t idx) {

    /* This stack grows from 0 to (CFE_PLATFORM_SB_MAX_MSG_IDS - 1) */
    if (CFE_SB.RouteIdxTop > 0) {
        --CFE_SB.RouteIdxTop;
        CFE_SB.RouteIdxStack[CFE_SB.RouteIdxTop] = idx;
    }
} /* end CFE_SB_IdxPush_Unsync */

/******************************************************************************
**  Function:  CFE_SB_GetPipeIdx()
**
**  Purpose:
**    SB internal function to get the pipe table index for the given pipe id.
**
**  Arguments:
**    PipeId
**
**  Return:
**    Returns the pipe table index of the given pipe id or CFE_SB_INVALID_PIPE if
*     there was not an entry for the given pipe id.
*/
uint8 CFE_SB_GetPipeIdx(CFE_SB_PipeId_t PipeId){

    uint8  i;

    /* search the pipe table for the for the given pipe id */
    for(i=0;i<CFE_PLATFORM_SB_MAX_PIPES;i++){

        if((CFE_SB.PipeTbl[i].PipeId == PipeId)&&(CFE_SB.PipeTbl[i].InUse == 1)){
            return i;
        }/* end if */

    }/* end for */

    return CFE_SB_INVALID_PIPE;

}/* end CFE_SB_GetPipeIdx */


/******************************************************************************
**  Function:  CFE_SB_LockSharedData()
**
**  Purpose:
**    SB internal function to handle a semaphore take failure for the Shared
**    Data Mutex
**
**  Arguments:
**    FuncName   - the function name containing the code that generated the error.
**    LineNumber - the line number in the file of the code that generated the error.
**
**  Return:
**    None
*/
void CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber){

    int32   Status;
    uint32  AppId = 0xFFFFFFFF;

    Status = OS_MutSemTake(CFE_SB.SharedDataMutexId);
    if (Status != OS_SUCCESS) {

        CFE_ES_GetAppID(&AppId);

        CFE_ES_WriteToSysLog("SB SharedData Mutex Take Err Stat=0x%x,App=%d,Func=%s,Line=%d\n",
                (unsigned int)Status,(int)AppId,FuncName,(int)LineNumber);

    }/* end if */

    return;

}/* end CFE_SB_LockSharedData */



/******************************************************************************
**  Function:  CFE_SB_UnlockSharedData()
**
**  Purpose:
**    SB internal function to handle a semaphore give failure  for the Shared
**    Data Mutex
**
**  Arguments:
**    FuncName   - the function name containing the code that generated the error.
**    LineNumber - the line number in the file of the code that generated the error.
**
**  Return:
**    None
*/
void CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber){

   int32   Status;
   uint32  AppId = 0xFFFFFFFF;

    Status = OS_MutSemGive(CFE_SB.SharedDataMutexId);
    if (Status != OS_SUCCESS) {

        CFE_ES_GetAppID(&AppId);

        CFE_ES_WriteToSysLog("SB SharedData Mutex Give Err Stat=0x%x,App=%d,Func=%s,Line=%d\n",
                (unsigned int)Status,(int)AppId,FuncName,(int)LineNumber);

    }/* end if */

    return;

}/* end CFE_SB_UnlockSharedData */


/******************************************************************************
**  Function:  CFE_SB_GetPipePtr()
**
**  Purpose:
**    SB internal function to get a pointer to the pipe descriptor associated
**    with the given pipe id.
**
**  Arguments:
**    PipeId
**
**  Return:
**    Pointer to the descriptor for the pipe.  If the pipe ID is not valid,
**    a NULL pointer is returned.
*/

CFE_SB_PipeD_t *CFE_SB_GetPipePtr(CFE_SB_PipeId_t PipeId) {

   /*
   ** Verify that the pipeId is in the valid range and being used.
   ** If so, return the pointer to the pipe descriptor.
   */

    if(CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS){
        return NULL;
    }else{
        return &CFE_SB.PipeTbl[PipeId];
    }/* end if */

}/* end CFE_SB_GetPipePtr */



/******************************************************************************
**  Function:  CFE_SB_GetDestPtr()
**
**  Purpose:
**    SB internal function to get a pointer to the destination descriptor
**    associated with the given message id/pipe id combination.
**
**  Arguments:
**    MsgId  : ID of the message
**    PipeId : Pipe ID for the destination.
**
**  Return:
**    Pointer to the destination descriptor that corresponds to the msg/pipe
**    combination. If the destination does not exist, return NULL.
*/
CFE_SB_DestinationD_t  *CFE_SB_GetDestPtr(CFE_SB_MsgKey_t MsgKey,
                                          CFE_SB_PipeId_t PipeId){

    CFE_SB_MsgRouteIdx_t    Idx;
    CFE_SB_DestinationD_t   *DestPtr;

    Idx = CFE_SB_GetRoutingTblIdx(MsgKey);

    if(!CFE_SB_IsValidRouteIdx(Idx))
    {
        return NULL;
    }/* end if */

    DestPtr = CFE_SB_GetRoutePtrFromIdx(Idx)->ListHeadPtr;

    while(DestPtr != NULL){

        if(DestPtr -> PipeId == PipeId){
            return DestPtr;
        }/* end if */

        DestPtr = DestPtr->Next;

    }/* end while */

    return NULL;

}/* end CFE_SB_GetDestPtr */



/******************************************************************************
**  Function:  CFE_SB_GetRoutingTblIdx()
**
**  Purpose:
**    SB internal function to get the index of the routing table element
**    associated with the given message id.
**
**  Assumptions:
**    Calls to this are predicated by a call to CFE_SB_IsValidMsgKey
**    which already check the MsgKey argument
**
**  Arguments:
**    MsgKey  : ID of the message
**    PipeId : Pipe ID for the destination.
**
**  Return:
**    Will return the index of the routing table element for the given message ID
*/
CFE_SB_MsgRouteIdx_t CFE_SB_GetRoutingTblIdx(CFE_SB_MsgKey_t MsgKey){

    return CFE_SB.MsgMap[CFE_SB_MsgKeyToValue(MsgKey)];

}/* end CFE_SB_GetRoutingTblIdx */



/******************************************************************************
**  Function:  CFE_SB_SetRoutingTblIdx()
**
**  Purpose:
**    SB internal function to set a value in the message map. The "Value" is
**    the routing table index of the given message ID. The message map is used
**    for quick routing table index lookups of a given message ID. The cost of
**    this quick lookup is 8K bytes of memory(for CCSDS).
**
**  Assumptions:
**    Calls to this are predicated by a call to CFE_SB_IsValidMsgKey
**    which already check the MsgKey argument
**
**  Arguments:
**    MsgKey  : ID of the message
**    Value  : value to set.
**
**  Return:
**
*/
void CFE_SB_SetRoutingTblIdx(CFE_SB_MsgKey_t MsgKey, CFE_SB_MsgRouteIdx_t Value){

    CFE_SB.MsgMap[CFE_SB_MsgKeyToValue(MsgKey)] = Value;

}/* end CFE_SB_SetRoutingTblIdx */


/******************************************************************************
**  Function:  CFE_SB_GetRoutePtrFromIdx()
**
**  Purpose:
**    SB internal function to obtain a pointer to a routing table entry
**    based on a CFE_SB_MsgRouteIdx_t value.
**
**  Assumptions:
**    Calls to this are predicated by a call to CFE_SB_IsValidRouteIdx
**    which already check the RouteIdx argument
**
**  Arguments:
**    RouteIdx  : ID of the route to get
**
**  Return:
**    Pointer to route entry
**
*/
CFE_SB_RouteEntry_t* CFE_SB_GetRoutePtrFromIdx(CFE_SB_MsgRouteIdx_t RouteIdx)
{
    return &CFE_SB.RoutingTbl[CFE_SB_RouteIdxToValue(RouteIdx)];
} /* end CFE_SB_GetRouteFromIdx */


/******************************************************************************
**  Function:  CFE_SB_GetPipeName()
**
**  Purpose:
**    SB internal function to get the string of the pipe name of the given
**    Pipe ID.
**
**  Arguments:
**    MsgId  : ID of the message
**
**  Return:
**    Will return a pointer to the PipeName array in the pipe table if the the pipeid
**    is in range. Otherwise this function returns a pointer to the
**    PipeName4ErrCase[0], which is initialized with a null terminator.
**
*/
char *CFE_SB_GetPipeName(CFE_SB_PipeId_t PipeId){

    static char PipeName4ErrCase[1] = {'\0'};

    if(PipeId >= CFE_PLATFORM_SB_MAX_PIPES){
        return &PipeName4ErrCase[0];
    }else{
        return &CFE_SB.PipeTbl[PipeId].PipeName[0];
    }/* end if */

}/* end CFE_SB_GetPipeName */



/******************************************************************************
**  Function:  CFE_SB_DuplicateSubscribeCheck()
**
**  Purpose:
**    SB internal function to check for a duplicate subscription.
**
**  Arguments:
**    MsgId  : ID of the message
**    PipeId : ID of the pipe
**
**  Return:
**    Will return CFE_SB_DUPLICATE if the given MsgId/PipeId subscription
**    exists in SB routing tables, otherwise will return CFE_SB_NO_DUPLICATE.
*/
int32 CFE_SB_DuplicateSubscribeCheck(CFE_SB_MsgKey_t MsgKey,
                                       CFE_SB_PipeId_t PipeId){

    CFE_SB_MsgRouteIdx_t    Idx;
    CFE_SB_DestinationD_t   *DestPtr;

    Idx = CFE_SB_GetRoutingTblIdx(MsgKey);

    if(!CFE_SB_IsValidRouteIdx(Idx))
    {
        DestPtr = NULL;
    }
    else
    {
        DestPtr = CFE_SB_GetRoutePtrFromIdx(Idx)->ListHeadPtr;
    }/* end if */

    while(DestPtr != NULL){

        if(DestPtr -> PipeId == PipeId){
            return CFE_SB_DUPLICATE;
        }/* end if */

        DestPtr = DestPtr->Next;

    }/* end while */

    return CFE_SB_NO_DUPLICATE;

}/* end CFE_SB_DuplicateSubscribeCheck */



/******************************************************************************
**  Function:  CFE_SB_SetMsgSeqCnt()
**
**  Purpose:
**    SB internal function to set the sequence count of a message to a
**    particular value.
**
**  Arguments:
**    MsgPtr  : pointer to the message
**    Count   : sets the sequence count to this value
**
**  Return:
**    None
*/
void CFE_SB_SetMsgSeqCnt(CFE_SB_MsgPtr_t MsgPtr,uint32 Count){

#ifdef MESSAGE_FORMAT_IS_CCSDS
    CCSDS_WR_SEQ(MsgPtr->Hdr,Count);
#endif

}/* end CFE_SB_SetMsgSeqCnt */


/******************************************************************************
**  Function:  CFE_SB_ValidateMsgId()
**
**  Purpose:
**    SB internal function to validate a given MsgId.
**
**  Arguments:
**
**  Return:
**    None
*/
int32 CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId){

    if (!CFE_SB_IsValidMsgId(MsgId))
    {
        return CFE_SB_FAILED;
    }
    else
    {
        return CFE_SUCCESS;
    }/* end if */

}/* end CFE_SB_ValidateMsgId */


/******************************************************************************
**  Function:  CFE_SB_ValidatePipeId()
**
**  Purpose:
**    This function checks that the pipe id does not have an index larger than the
**    array and that the pipe is in use.
**
**  Arguments:
**
**  Return:
**    None
*/
int32 CFE_SB_ValidatePipeId(CFE_SB_PipeId_t PipeId){

    if((PipeId >= CFE_PLATFORM_SB_MAX_PIPES)||
       (CFE_SB.PipeTbl[PipeId].InUse == CFE_SB_NOT_IN_USE))
    {
        return CFE_SB_FAILED;
    }else{
        return CFE_SUCCESS;
    }/* end if */

}/* end CFE_SB_ValidatePipeId */


/******************************************************************************
**  Function:  CFE_SB_GetAppTskName()
**
**  Purpose:
**    This function returns a pointer to the app.tsk name string
**
**  Arguments:
**    TaskId - the task id of the app.task name desired
**    FullName - string buffer to store name
**
**  Return:
**    Pointer to App.Tsk Name
**
**  Note: With taskId, Parent App name and Child Task name can be queried from ES
**
*/
char *CFE_SB_GetAppTskName(uint32 TaskId,char *FullName){

    CFE_ES_TaskInfo_t  TaskInfo;
    CFE_ES_TaskInfo_t  *ptr = &TaskInfo;
    char               AppName[OS_MAX_API_NAME];
    char               TskName[OS_MAX_API_NAME];

    if(CFE_ES_GetTaskInfo(ptr, TaskId) != CFE_SUCCESS){

      /* unlikely, but possible if TaskId is bogus */
      strncpy(FullName,"Unknown",OS_MAX_API_NAME-1);
      FullName[OS_MAX_API_NAME-1] = '\0';

    }else if(strncmp((char *)ptr->AppName,(char *)ptr->TaskName,OS_MAX_API_NAME-1) == 0){

      /* if app name and task name are the same */
      strncpy(FullName,(char *)ptr->AppName,OS_MAX_API_NAME-1);
      FullName[OS_MAX_API_NAME-1] = '\0';

    }else{

      /* AppName and TskName buffers and strncpy are needed to limit string sizes */
      strncpy(AppName,(char *)ptr->AppName,OS_MAX_API_NAME-1);
      AppName[OS_MAX_API_NAME-1] = '\0';
      strncpy(TskName,(char *)ptr->TaskName,OS_MAX_API_NAME-1);
      TskName[OS_MAX_API_NAME-1] = '\0';

      sprintf(FullName,"%s.%s",AppName,TskName);

    }/* end if */

    return FullName;

}/* end CFE_SB_GetAppTskName */


/******************************************************************************
**  Function:  CFE_SB_GetPktType()
**
**  Purpose:
**    For CCSDS packets, this function returns the state of the cmd/tlm bit(12).
**    For cmd pkts, the state is 1. For tlm pkts, the state is 0.
**
**  Arguments:
**
**  Return:
**    None
*/
uint8 CFE_SB_GetPktType(CFE_SB_MsgId_t MsgId)
{
    CFE_SB_MsgId_Atom_t Val = MsgId;

#ifdef MESSAGE_FORMAT_IS_CCSDS

#ifndef MESSAGE_FORMAT_IS_CCSDS_VER_2
        return CFE_TST(Val,12);
#else
        return CFE_SB_RD_TYPE_FROM_MSGID(Val);
#endif /* MESSAGE_FORMAT_IS_CCSDS_VER_2 */
        
#endif /* MESSAGE_FORMAT_IS_CCSDS */

}/* end CFE_SB_GetPktType */


/******************************************************************************
**  Function:  CFE_SB_RequestToSendEvent()
**
**  Purpose:
**    This function will test the given bit for the given task. If the bit is set
**    this function will return CFE_SB_DENIED. If bit is not set, this function set
**    the bit and return CFE_SB_GRANTED. This will prevent recursive events from
**    occurring.
**
**  Arguments:
**
**  Return:
**    If the bit is set this function will return CFE_SB_DENIED.
**    If bit is not set, this function set the bit and return CFE_SB_GRANTED.
*/
uint32 CFE_SB_RequestToSendEvent(uint32 TaskId, uint32 Bit){

    OS_ConvertToArrayIndex(TaskId, &TaskId);

    /* if bit is set... */
    if(CFE_TST(CFE_SB.StopRecurseFlags[TaskId],Bit))
    {

      return CFE_SB_DENIED;

    }else{

      CFE_SET(CFE_SB.StopRecurseFlags[TaskId],Bit);
      return CFE_SB_GRANTED;

    }/* end if */

}/* end CFE_SB_RequestToSendEvent */

/******************************************************************************
**  Function:  CFE_SB_FinishSendEvent()
**
**  Purpose:
**    This function will clear the given bit for the given task. Called after
**    a successful CFE_SB_RequestToSendEvent()
**
**  Arguments:
**
**  Return:
**    If the bit is set this function will return CFE_SB_DENIED.
**    If bit is not set, this function set the bit and return CFE_SB_GRANTED.
*/
void CFE_SB_FinishSendEvent(uint32 TaskId, uint32 Bit){

    OS_ConvertToArrayIndex(TaskId, &TaskId);

    /* clear the bit so the task may send this event again */
    CFE_CLR(CFE_SB.StopRecurseFlags[TaskId],Bit);
}/* end CFE_SB_RequestToSendEvent */



/******************************************************************************
**  Function:  CFE_SB_AddDest()
**
**  Purpose:
**      This function will add the given node to the head of the list.
**
**  Arguments:
**      RtgTblIdx - Routing table index
**      Dest - Pointer to the destination block to add to the list
**
**  Return:
**
*/
int32 CFE_SB_AddDest(CFE_SB_RouteEntry_t *RouteEntry, CFE_SB_DestinationD_t *NewNode){

    CFE_SB_DestinationD_t *WBS;/* Will Be Second (WBS) node */

    /* if first node in list */
    if(RouteEntry->ListHeadPtr == NULL){

        /* initialize the new node */
        NewNode->Next = NULL;
        NewNode->Prev = NULL;

        /* insert the new node */
        RouteEntry->ListHeadPtr = NewNode;

    }else{

        WBS = RouteEntry->ListHeadPtr;

        /* initialize the new node */
        NewNode->Next = WBS;
        NewNode->Prev = NULL;

        /* insert the new node */
        WBS -> Prev = NewNode;
        RouteEntry->ListHeadPtr = NewNode;

    }/* end if */

    return CFE_SUCCESS;

}/* CFE_SB_AddDest */



/******************************************************************************
**  Function:  CFE_SB_RemoveDest()
**
**  Purpose:
**      This function will remove the given node from the list.
**      This function assumes there is at least one node in the list.
**
**  Arguments:
**      RtgTblIdx - Routing table index
**      Dest - Pointer to the destination block to remove from the list
**
**  Return:
**
*/
int32 CFE_SB_RemoveDest(CFE_SB_RouteEntry_t *RouteEntry, CFE_SB_DestinationD_t *NodeToRemove){

    CFE_SB_DestinationD_t *PrevNode;
    CFE_SB_DestinationD_t *NextNode;

    /* if this is the only node in the list */
    if((NodeToRemove->Prev == NULL) && (NodeToRemove->Next == NULL)){

        RouteEntry->ListHeadPtr = NULL;

    /* if first node in the list and list has more than one */
    }else if(NodeToRemove->Prev == NULL){

        NextNode = NodeToRemove->Next;

        NextNode -> Prev = NULL;

        RouteEntry->ListHeadPtr = NextNode;

    /* if last node in the list and list has more than one */
    }else if(NodeToRemove->Next == NULL){

        PrevNode = NodeToRemove->Prev;

        PrevNode -> Next = NULL;

    /* NodeToRemove has node(s) before and node(s) after */
    }else{

        PrevNode = NodeToRemove->Prev;
        NextNode = NodeToRemove->Next;

        PrevNode -> Next = NextNode;
        NextNode -> Prev = PrevNode;

    }/* end if */


    /* initialize the node before returning it to the heap */
    NodeToRemove -> Next = NULL;
    NodeToRemove -> Prev = NULL;

    return CFE_SUCCESS;

}/* CFE_SB_RemoveDest */


/******************************************************************************
** Name:    CFE_SB_ZeroCopyReleaseAppId
**
** Purpose: API used for releasing all pointers to a buffers (for zero copy mode
**          only) for a specific Application. This function is used for cleaning
**          up when an application crashes.
**
** Assumptions, External Events, and Notes:
**          None
**
** Date Written:
**          07/23/2009
**
** Input Arguments:
**          AppId
**
** Output Arguments:
**          None
**
** Return Values:
**          Status
**
******************************************************************************/
int32 CFE_SB_ZeroCopyReleaseAppId(uint32         AppId)
{
    CFE_SB_ZeroCopyD_t *prev = NULL;
    CFE_SB_ZeroCopyD_t *zcd = (CFE_SB_ZeroCopyD_t *) (CFE_SB.ZeroCopyTail);

    while(zcd != NULL){
        prev = (CFE_SB_ZeroCopyD_t *) (zcd->Prev);
        if(zcd->AppID == AppId){
            CFE_SB_ZeroCopyReleasePtr((CFE_SB_Msg_t *) zcd->Buffer, (CFE_SB_ZeroCopyHandle_t) zcd);
        }
        zcd = prev;
    }

    return CFE_SUCCESS;

}/* end CFE_SB_ZeroCopyReleasePtr */

/*****************************************************************************/

