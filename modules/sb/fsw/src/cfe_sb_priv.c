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

#include "cfe_sb_module_all.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_CleanUpApp(CFE_ES_AppId_t AppId)
{
    uint32          i;
    uint32          DelCount;
    CFE_SB_PipeD_t *PipeDscPtr;
    CFE_SB_PipeId_t DelList[CFE_PLATFORM_SB_MAX_PIPES];

    PipeDscPtr = CFE_SB_Global.PipeTbl;
    DelCount   = 0;

    CFE_SB_LockSharedData(__func__, __LINE__);

    /* loop through the pipe table looking for pipes owned by AppId */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_PIPES; ++i)
    {
        if (CFE_SB_PipeDescIsUsed(PipeDscPtr) && CFE_RESOURCEID_TEST_EQUAL(PipeDscPtr->AppId, AppId))
        {
            DelList[DelCount] = CFE_SB_PipeDescGetID(PipeDscPtr);
            ++DelCount;
        }
        ++PipeDscPtr;
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    for (i = 0; i < DelCount; ++i)
    {
        CFE_SB_DeletePipeWithAppId(DelList[i], AppId);
    }

    /* Release any zero copy buffers */
    CFE_SB_ZeroCopyReleaseAppId(AppId);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_LockSharedData(const char *FuncName, int32 LineNumber)
{
    int32          OsStatus;
    CFE_ES_AppId_t AppId;

    OsStatus = OS_MutSemTake(CFE_SB_Global.SharedDataMutexId);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("%s: SharedData Mutex Take Err Stat=%ld,App=%lu,Func=%s,Line=%d\n", __func__,
                             (long)OsStatus, CFE_RESOURCEID_TO_ULONG(AppId), FuncName, (int)LineNumber);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_UnlockSharedData(const char *FuncName, int32 LineNumber)
{
    int32          OsStatus;
    CFE_ES_AppId_t AppId;

    OsStatus = OS_MutSemGive(CFE_SB_Global.SharedDataMutexId);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_GetAppID(&AppId);
        CFE_ES_WriteToSysLog("%s: SharedData Mutex Give Err Stat=%ld,App=%lu,Func=%s,Line=%d\n", __func__,
                             (long)OsStatus, CFE_RESOURCEID_TO_ULONG(AppId), FuncName, (int)LineNumber);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_DestinationD_t *CFE_SB_GetDestPtr(CFE_SBR_RouteId_t RouteId, CFE_SB_PipeId_t PipeId)
{
    CFE_SB_DestinationD_t *destptr;

    destptr = CFE_SBR_GetDestListHeadPtr(RouteId);

    /* Check all destinations */
    while (destptr != NULL)
    {
        if (CFE_RESOURCEID_TEST_EQUAL(destptr->PipeId, PipeId))
        {
            break;
        }
        destptr = destptr->Next;
    }

    return destptr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_ValidateMsgId(CFE_SB_MsgId_t MsgId)
{
    if (!CFE_SB_IsValidMsgId(MsgId))
    {
        return CFE_SB_FAILED;
    }
    else
    {
        return CFE_SUCCESS;
    }
}

/*********************************************************************/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_PipeD_t *CFE_SB_LocatePipeDescByID(CFE_SB_PipeId_t PipeId)
{
    CFE_SB_PipeD_t *PipeDscPtr;
    uint32          Idx;

    if (CFE_SB_PipeId_ToIndex(PipeId, &Idx) == CFE_SUCCESS)
    {
        PipeDscPtr = &CFE_SB_Global.PipeTbl[Idx];
    }
    else
    {
        PipeDscPtr = NULL;
    }

    return PipeDscPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_CheckPipeDescSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_SB_PipeD_t *PipeDscPtr;
    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(CFE_SB_PIPEID_C(CheckId));
    return (PipeDscPtr == NULL || CFE_SB_PipeDescIsUsed(PipeDscPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
char *CFE_SB_GetAppTskName(CFE_ES_TaskId_t TaskId, char *FullName)
{
    CFE_ES_TaskInfo_t  TaskInfo;
    CFE_ES_TaskInfo_t *ptr = &TaskInfo;
    char               AppName[OS_MAX_API_NAME];
    char               TskName[OS_MAX_API_NAME];

    if (CFE_ES_GetTaskInfo(ptr, TaskId) != CFE_SUCCESS)
    {
        /* unlikely, but possible if TaskId is bogus */
        strncpy(FullName, "Unknown", OS_MAX_API_NAME - 1);
        FullName[OS_MAX_API_NAME - 1] = '\0';
    }
    else if (strncmp((char *)ptr->AppName, (char *)ptr->TaskName, sizeof(ptr->AppName)) == 0)
    {
        /* if app name and task name are the same */
        strncpy(FullName, (char *)ptr->AppName, OS_MAX_API_NAME - 1);
        FullName[OS_MAX_API_NAME - 1] = '\0';
    }
    else
    {
        /* AppName and TskName buffers and strncpy are needed to limit string sizes */
        strncpy(AppName, (char *)ptr->AppName, sizeof(AppName) - 1);
        AppName[sizeof(AppName) - 1] = '\0';
        strncpy(TskName, (char *)ptr->TaskName, sizeof(TskName) - 1);
        TskName[sizeof(TskName) - 1] = '\0';

        sprintf(FullName, "%s.%s", AppName, TskName);
    }

    return FullName;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_SB_RequestToSendEvent(CFE_ES_TaskId_t TaskId, int32 Bit)
{
    uint32 Indx;

    if (Bit < 0)
    {
        return CFE_SB_GRANTED;
    }

    if (CFE_ES_TaskID_ToIndex(TaskId, &Indx) != CFE_SUCCESS)
    {
        return CFE_SB_DENIED;
    }

    /* if bit is set... */
    if (CFE_TST(CFE_SB_Global.StopRecurseFlags[Indx], Bit))
    {
        return CFE_SB_DENIED;
    }
    else
    {
        CFE_SET(CFE_SB_Global.StopRecurseFlags[Indx], Bit);
        return CFE_SB_GRANTED;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_FinishSendEvent(CFE_ES_TaskId_t TaskId, int32 Bit)
{
    uint32 Indx;

    if (Bit < 0)
    {
        return;
    }

    if (CFE_ES_TaskID_ToIndex(TaskId, &Indx) != CFE_SUCCESS)
    {
        return;
    }

    /* clear the bit so the task may send this event again */
    CFE_CLR(CFE_SB_Global.StopRecurseFlags[Indx], Bit);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_AddDestNode(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *NewNode)
{
    CFE_SB_DestinationD_t *WBS; /* Will Be Second (WBS) node */
    CFE_SB_DestinationD_t *listheadptr;

    listheadptr = CFE_SBR_GetDestListHeadPtr(RouteId);

    /* if first node in list */
    if (listheadptr == NULL)
    {
        /* initialize the new node */
        NewNode->Next = NULL;
        NewNode->Prev = NULL;
    }
    else
    {
        WBS = listheadptr;

        /* initialize the new node */
        NewNode->Next = WBS;
        NewNode->Prev = NULL;

        /* insert the new node */
        WBS->Prev = NewNode;
    }

    /* Update Head */
    CFE_SBR_SetDestListHeadPtr(RouteId, NewNode);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_RemoveDest(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *DestPtr)
{
    CFE_SB_RemoveDestNode(RouteId, DestPtr);
    CFE_SB_PutDestinationBlk(DestPtr);
    CFE_SB_Global.StatTlmMsg.Payload.SubscriptionsInUse--;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_RemoveDestNode(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *NodeToRemove)
{
    CFE_SB_DestinationD_t *PrevNode;
    CFE_SB_DestinationD_t *NextNode;

    if ((NodeToRemove->Prev == NULL) && (NodeToRemove->Next == NULL))
    {
        /* Clear destinations if this is the only node in the list */
        CFE_SBR_SetDestListHeadPtr(RouteId, NULL);
    }
    else if (NodeToRemove->Prev == NULL)
    {
        /* First in the list, set the next node to list head */
        NextNode       = NodeToRemove->Next;
        NextNode->Prev = NULL;
        CFE_SBR_SetDestListHeadPtr(RouteId, NextNode);
    }
    else if (NodeToRemove->Next == NULL)
    {
        /* Last in the list, remove previous pointer */
        PrevNode       = NodeToRemove->Prev;
        PrevNode->Next = NULL;
    }
    else
    {
        /* Middle of list, remove */
        PrevNode       = NodeToRemove->Prev;
        NextNode       = NodeToRemove->Next;
        PrevNode->Next = NextNode;
        NextNode->Prev = PrevNode;
    }

    /* initialize the node before returning it to the heap */
    NodeToRemove->Next = NULL;
    NodeToRemove->Prev = NULL;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_ZeroCopyReleaseAppId(CFE_ES_AppId_t AppId)
{
    CFE_SB_BufferLink_t *NextLink;
    CFE_SB_BufferD_t *   DscPtr;

    /*
     * First go through the "ZeroCopy" tracking list and find all nodes
     * with a matching AppID.  This needs to be done while locked to
     * prevent other tasks from changing the list at the same time.
     */
    if (CFE_RESOURCEID_TEST_DEFINED(AppId))
    {
        CFE_SB_LockSharedData(__func__, __LINE__);

        /* Get start of list */
        NextLink = CFE_SB_TrackingListGetNext(&CFE_SB_Global.ZeroCopyList);
        while (!CFE_SB_TrackingListIsEnd(&CFE_SB_Global.ZeroCopyList, NextLink))
        {
            /* Get buffer descriptor pointer */
            /* NOTE: casting via void* here rather than CFE_SB_BufferD_t* avoids a false
             * alignment warning on platforms with strict alignment requirements */
            DscPtr = (void *)NextLink;

            /* Read the next link now in case this node gets moved */
            NextLink = CFE_SB_TrackingListGetNext(NextLink);

            /* Check if it is a zero-copy buffer owned by this app */
            if (CFE_RESOURCEID_TEST_EQUAL(DscPtr->AppId, AppId))
            {
                /* If so, decrement the use count as the app has now gone away */
                CFE_SB_DecrBufUseCnt(DscPtr);
            }
        }

        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_ZeroCopyBufferValidate(CFE_SB_Buffer_t *BufPtr, CFE_SB_BufferD_t **BufDscPtr)
{
    cpuaddr BufDscAddr;

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

/******************************************************************
 *
 * MESSAGE TRANSACTION IMPLEMENTATION FUNCTIONS
 *
 ******************************************************************/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_Init(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *PipeSet, uint16 MaxPipes,
                            const void *RefMemPtr)
{
    memset(TxnPtr, 0, offsetof(CFE_SB_TransmitTxn_State_t, DestSet));
    TxnPtr->PipeSet  = PipeSet;
    TxnPtr->MaxPipes = MaxPipes;

    /* The reference pointer is kept just for (potential) reporting in events, etc.
     * It is not dereferenced from here. */
    TxnPtr->RefMemPtr = RefMemPtr;
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_SetTimeout(CFE_SB_MessageTxn_State_t *TxnPtr, int32 Timeout)
{
    TxnPtr->UserTimeoutParam = Timeout;
    if (Timeout > 0)
    {
        /* Convert to an absolute timeout */
        TxnPtr->TimeoutMode = CFE_SB_MessageTxn_TimeoutMode_TIMED;
        CFE_PSP_GetTime(&TxnPtr->AbsTimeout);
        TxnPtr->AbsTimeout = OS_TimeAdd(TxnPtr->AbsTimeout, OS_TimeFromTotalMilliseconds(Timeout));
    }
    else if (Timeout == CFE_SB_POLL)
    {
        TxnPtr->TimeoutMode = CFE_SB_MessageTxn_TimeoutMode_POLL;
    }
    else if (Timeout == CFE_SB_PEND_FOREVER)
    {
        TxnPtr->TimeoutMode = CFE_SB_MessageTxn_TimeoutMode_PEND;
    }
    else
    {
        CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, CFE_SB_RCV_BAD_ARG_EID, CFE_SB_BAD_ARGUMENT);
    }
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_GetEventDetails(const CFE_SB_MessageTxn_State_t *TxnPtr, const CFE_SB_PipeSetEntry_t *ContextPtr,
                                       uint16 EventId, CFE_ES_TaskId_t TskId, char *EvtMsg, size_t EvtMsgSize,
                                       CFE_EVS_EventType_Enum_t *EventType, int32 *ReqBit)
{
    char  FullName[(OS_MAX_API_NAME * 2)];
    char  PipeName[OS_MAX_API_NAME];
    int32 LocalOsStatus;

    if (ContextPtr == NULL)
    {
        LocalOsStatus = INT32_MIN; /* should not be used; do not alias any actual OS status */
    }
    else
    {
        LocalOsStatus = ContextPtr->OsStatus;
        if (ContextPtr->PendingEventId == CFE_SB_BAD_PIPEID_EID)
        {
            /* do not attempt to map to a pipe name if reporting a bad pipeID event - use its ID */
            snprintf(PipeName, sizeof(PipeName), "%lu", CFE_RESOURCEID_TO_ULONG(ContextPtr->PipeId));
        }
        else
        {
            CFE_SB_GetPipeName(PipeName, sizeof(PipeName), ContextPtr->PipeId);
        }
    }

    switch (EventId)
    {
        case CFE_SB_SEND_BAD_ARG_EID:
            *ReqBit    = CFE_SB_SEND_BAD_ARG_EID_BIT;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Send Err:Bad input argument,Arg 0x%lx,App %s",
                     (unsigned long)TxnPtr->RefMemPtr, CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_SEND_INV_MSGID_EID:
            *ReqBit    = CFE_SB_SEND_INV_MSGID_EID_BIT;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Send Err:Invalid MsgId(0x%x)in msg,App %s",
                     (unsigned int)CFE_SB_MsgIdToValue(TxnPtr->RoutingMsgId), CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_MSG_TOO_BIG_EID:
            *ReqBit    = CFE_SB_MSG_TOO_BIG_EID_BIT;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Send Err:Msg Too Big MsgId=0x%x,app=%s,size=%d,MaxSz=%d",
                     (unsigned int)CFE_SB_MsgIdToValue(TxnPtr->RoutingMsgId), CFE_SB_GetAppTskName(TskId, FullName),
                     (int)TxnPtr->ContentSize, CFE_MISSION_SB_MAX_SB_MSG_SIZE);
            break;

        case CFE_SB_SEND_NO_SUBS_EID:
            *ReqBit    = CFE_SB_SEND_NO_SUBS_EID_BIT;
            *EventType = CFE_EVS_EventType_INFORMATION;

            snprintf(EvtMsg, EvtMsgSize, "No subscribers for MsgId 0x%x,sender %s",
                     (unsigned int)CFE_SB_MsgIdToValue(TxnPtr->RoutingMsgId), CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_GET_BUF_ERR_EID:
            *ReqBit    = CFE_SB_GET_BUF_ERR_EID_BIT;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Send Err:Request for Buffer Failed. MsgId 0x%x,app %s,size %d",
                     (unsigned int)CFE_SB_MsgIdToValue(TxnPtr->RoutingMsgId), CFE_SB_GetAppTskName(TskId, FullName),
                     (int)TxnPtr->ContentSize);
            break;

        case CFE_SB_MSGID_LIM_ERR_EID:
            *ReqBit    = CFE_SB_MSGID_LIM_ERR_EID_BIT;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Msg Limit Err,MsgId 0x%x,pipe %s,sender %s",
                     (unsigned int)CFE_SB_MsgIdToValue(TxnPtr->RoutingMsgId), PipeName,
                     CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_Q_FULL_ERR_EID:
            *ReqBit    = CFE_SB_Q_FULL_ERR_EID_BIT;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Pipe Overflow,MsgId 0x%x,pipe %s,sender %s",
                     (unsigned int)CFE_SB_MsgIdToValue(TxnPtr->RoutingMsgId), PipeName,
                     CFE_SB_GetAppTskName(TskId, FullName));
            break;

        case CFE_SB_Q_WR_ERR_EID:
            *ReqBit    = CFE_SB_Q_WR_ERR_EID_BIT;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Pipe Write Err,MsgId 0x%x,pipe %s,sender %s,stat %ld",
                     (unsigned int)CFE_SB_MsgIdToValue(TxnPtr->RoutingMsgId), PipeName,
                     CFE_SB_GetAppTskName(TskId, FullName), (long)LocalOsStatus);
            break;

        case CFE_SB_Q_RD_ERR_EID:
            *ReqBit    = -1;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Pipe Read Err,pipe %s,app %s,stat %ld", PipeName,
                     CFE_SB_GetAppTskName(TskId, FullName), (long)LocalOsStatus);
            break;
        case CFE_SB_RCV_BAD_ARG_EID:
            *ReqBit    = -1;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Rcv Err:Bad Input Arg:BufPtr 0x%lx,pipe %s,t/o %d,app %s",
                     (unsigned long)TxnPtr->RefMemPtr, PipeName, (int)TxnPtr->UserTimeoutParam,
                     CFE_SB_GetAppTskName(TskId, FullName));
            break;
        case CFE_SB_BAD_PIPEID_EID:
            *ReqBit    = -1;
            *EventType = CFE_EVS_EventType_ERROR;

            snprintf(EvtMsg, EvtMsgSize, "Rcv Err:PipeId %s does not exist,app %s", PipeName,
                     CFE_SB_GetAppTskName(TskId, FullName));
            break;

        default:
            EvtMsg[0]  = 0;
            *ReqBit    = 0;
            *EventType = 0;
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_MessageTxn_ReportSingleEvent(const CFE_SB_MessageTxn_State_t *TxnPtr,
                                         const CFE_SB_PipeSetEntry_t *ContextPtr, uint16 EventId)
{
    CFE_ES_TaskId_t          TskId;
    CFE_EVS_EventType_Enum_t EventType;
    char                     Message[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    int32                    ReqBit;

    /* get task id for events and Sender Info*/
    CFE_ES_GetTaskID(&TskId);

    CFE_SB_MessageTxn_GetEventDetails(TxnPtr, ContextPtr, EventId, TskId, Message, sizeof(Message), &EventType,
                                      &ReqBit);

    if (EventType > 0 && CFE_SB_RequestToSendEvent(TskId, ReqBit) == CFE_SB_GRANTED)
    {
        CFE_EVS_SendEventWithAppID(EventId, EventType, CFE_SB_Global.AppId, "%s", Message);

        /* clear the bit so the task may send this event again */
        CFE_SB_FinishSendEvent(TskId, ReqBit);
    }

    /* If the event type was an error, this should increment the general error counter */
    return (EventType >= CFE_EVS_EventType_ERROR);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_ReportEvents(const CFE_SB_MessageTxn_State_t *TxnPtr)
{
    uint32 i;
    uint32 NumErrors;
    bool   IsError;

    NumErrors = 0;

    if (TxnPtr->TransactionEventId != 0)
    {
        IsError = CFE_SB_MessageTxn_ReportSingleEvent(TxnPtr, NULL, TxnPtr->TransactionEventId);
        if (IsError)
        {
            ++NumErrors;
        }
    }

    for (i = 0; i < TxnPtr->NumPipes; ++i)
    {
        if (TxnPtr->PipeSet[i].PendingEventId != 0)
        {
            IsError =
                CFE_SB_MessageTxn_ReportSingleEvent(TxnPtr, &TxnPtr->PipeSet[i], TxnPtr->PipeSet[i].PendingEventId);
            if (IsError)
            {
                ++NumErrors;
            }
        }
    }

    if (NumErrors > 0)
    {
        /*
         * Increment the error only if there was a real error,
         * such as a validation issue or failure to allocate a buffer.
         *
         * Note that transmit side just has one error counter, whereas
         * receive side has two - these differeniate between a bad passed-in
         * arg vs some other internal error such as queue access.
         */
        CFE_SB_LockSharedData(__func__, __LINE__);

        if (TxnPtr->IsTransmit)
        {
            CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter++;
        }
        else if (TxnPtr->Status == CFE_SB_BAD_ARGUMENT)
        {
            ++CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter;
        }
        else
        {
            /* For any other unexpected error (e.g. CFE_SB_Q_RD_ERR_EID) */
            ++CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter;
        }

        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_SetEventAndStatus(CFE_SB_MessageTxn_State_t *TxnPtr, uint16 EventId, CFE_Status_t Status)
{
    if (TxnPtr->TransactionEventId == 0)
    {
        TxnPtr->TransactionEventId = EventId;
    }
    if (TxnPtr->Status == CFE_SUCCESS)
    {
        TxnPtr->Status = Status;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_SetRoutingMsgId(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_MsgId_t RoutingMsgId)
{
    /* validate the msgid in the message */
    if (!CFE_SB_IsValidMsgId(RoutingMsgId))
    {
        CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, CFE_SB_SEND_INV_MSGID_EID, CFE_SB_BAD_ARGUMENT);
    }
    else
    {
        TxnPtr->RoutingMsgId = RoutingMsgId;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_SetContentSize(CFE_SB_MessageTxn_State_t *TxnPtr, size_t ContentSize)
{
    if (ContentSize > CFE_MISSION_SB_MAX_SB_MSG_SIZE)
    {
        CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, CFE_SB_MSG_TOO_BIG_EID, CFE_SB_MSG_TOO_BIG);
    }
    else
    {
        TxnPtr->ContentSize = ContentSize;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TransmitTxn_SetupFromMsg(CFE_SB_MessageTxn_State_t *TxnPtr, const CFE_MSG_Message_t *MsgPtr)
{
    CFE_Status_t   Status;
    CFE_MSG_Size_t MsgSize;
    CFE_SB_MsgId_t MsgId;

    if (CFE_SB_MessageTxn_IsOK(TxnPtr))
    {
        /* In this context, the user should have set the the size and MsgId in the content */
        Status = CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        if (Status == CFE_SUCCESS)
        {
            CFE_SB_MessageTxn_SetRoutingMsgId(TxnPtr, MsgId);
        }
        else
        {
            CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, CFE_SB_SEND_BAD_ARG_EID, Status);
        }
    }

    if (CFE_SB_MessageTxn_IsOK(TxnPtr))
    {
        Status = CFE_MSG_GetSize(MsgPtr, &MsgSize);
        if (Status == CFE_SUCCESS)
        {
            CFE_SB_MessageTxn_SetContentSize(TxnPtr, MsgSize);
        }
        else
        {
            CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, CFE_SB_SEND_BAD_ARG_EID, Status);
        }
    }
}

/******************************************************************
 *
 * TRANSMIT TRANSACTION IMPLEMENTATION FUNCTIONS
 *
 ******************************************************************/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MessageTxn_State_t *CFE_SB_TransmitTxn_Init(CFE_SB_TransmitTxn_State_t *TxnPtr, const void *RefMemPtr)
{
    CFE_SB_MessageTxn_Init(&TxnPtr->MessageTxn_State, TxnPtr->DestSet, CFE_PLATFORM_SB_MAX_DEST_PER_PKT, RefMemPtr);
    TxnPtr->MessageTxn_State.IsTransmit = true;

    /* No matter what, the mem pointer from the caller should not be NULL */
    if (RefMemPtr == NULL)
    {
        CFE_SB_MessageTxn_SetEventAndStatus(&TxnPtr->MessageTxn_State, CFE_SB_SEND_BAD_ARG_EID, CFE_SB_BAD_ARGUMENT);
    }

    return &TxnPtr->MessageTxn_State;
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TransmitTxn_FindDestinations(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_BufferD_t *BufDscPtr)
{
    CFE_SB_PipeD_t *       PipeDscPtr;
    CFE_SB_DestinationD_t *DestPtr;
    CFE_SB_PipeSetEntry_t *ContextPtr;
    CFE_ES_AppId_t         AppId;
    bool                   IsAcceptable;
    CFE_Status_t           Status;

    /*
     * get app id for loopback testing  -
     * This is only used if one or more of the destinations has its "IGNOREMINE" option set,
     * but it should NOT be gotten while locked.  So since we do not know (yet) if we need it,
     * it is better to get it and not need it than need it and not have it.
     */
    CFE_ES_GetAppID(&AppId);

    /* take semaphore to prevent a task switch during processing */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* Get the routing id */
    BufDscPtr->DestRouteId = CFE_SBR_GetRouteId(TxnPtr->RoutingMsgId);

    /* For an invalid route / no subscribers this whole logic can be skipped */
    if (CFE_SBR_IsValidRouteId(BufDscPtr->DestRouteId))
    {
        /* If this is the origination, then update the message content (while locked) before actually sending */
        if (TxnPtr->IsEndpoint)
        {
            CFE_SBR_IncrementSequenceCounter(BufDscPtr->DestRouteId);

            /* Set the sequence count from the route */
            CFE_MSG_SetSequenceCount(&BufDscPtr->Content.Msg, CFE_SBR_GetSequenceCounter(BufDscPtr->DestRouteId));
        }

        /* Send the packet to all destinations  */
        DestPtr = CFE_SBR_GetDestListHeadPtr(BufDscPtr->DestRouteId);
        while (DestPtr != NULL && TxnPtr->NumPipes < TxnPtr->MaxPipes)
        {
            ContextPtr = NULL;

            if (DestPtr->Active == CFE_SB_ACTIVE) /* destination is active */
            {
                PipeDscPtr = CFE_SB_LocatePipeDescByID(DestPtr->PipeId);
            }
            else
            {
                PipeDscPtr = NULL;
            }

            if (CFE_SB_PipeDescIsMatch(PipeDscPtr, DestPtr->PipeId))
            {
                if ((PipeDscPtr->Opts & CFE_SB_PIPEOPTS_IGNOREMINE) == 0 ||
                    !CFE_RESOURCEID_TEST_EQUAL(PipeDscPtr->AppId, AppId))
                {
                    ContextPtr = &TxnPtr->PipeSet[TxnPtr->NumPipes];
                    ++TxnPtr->NumPipes;
                }
            }

            if (ContextPtr != NULL)
            {
                memset(ContextPtr, 0, sizeof(*ContextPtr));

                ContextPtr->PipeId     = DestPtr->PipeId;
                ContextPtr->SysQueueId = PipeDscPtr->SysQueueId;

                /* if Msg limit exceeded, log event, increment counter */
                /* and go to next destination */
                if (DestPtr->BuffCount >= DestPtr->MsgId2PipeLim)
                {
                    ContextPtr->PendingEventId = CFE_SB_MSGID_LIM_ERR_EID;
                    ++CFE_SB_Global.HKTlmMsg.Payload.MsgLimitErrorCounter;
                    ++PipeDscPtr->SendErrors;
                    ++TxnPtr->NumPipeErrs;
                }
                else
                {
                    CFE_SB_IncrBufUseCnt(BufDscPtr);
                    ++DestPtr->BuffCount;

                    ++PipeDscPtr->CurrentQueueDepth;
                    if (PipeDscPtr->CurrentQueueDepth > PipeDscPtr->PeakQueueDepth)
                    {
                        PipeDscPtr->PeakQueueDepth = PipeDscPtr->CurrentQueueDepth;
                    }
                }
            }

            DestPtr = DestPtr->Next;
        }
    }
    else
    {
        /* if there have been no subscriptions for this pkt, */
        /* increment the dropped pkt cnt, send event and return success */
        CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter++;
        CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, CFE_SB_SEND_NO_SUBS_EID, CFE_SUCCESS);
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

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /*
     * Lastly, if this is the origination point, now that all headers should
     * have known values (including sequence) - invoke the mission-specific
     * message origination action.  This may update timestamps and/or compute
     * any required error control fields.
     */
    if (CFE_SB_MessageTxn_IsOK(TxnPtr) && TxnPtr->IsEndpoint)
    {
        /* Update any other system-specific MSG headers based on the current sequence */
        Status = CFE_MSG_OriginationAction(&BufDscPtr->Content.Msg, BufDscPtr->AllocatedSize, &IsAcceptable);
        if (Status != CFE_SUCCESS || !IsAcceptable)
        {
            CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, CFE_SB_SEND_MESSAGE_INTEGRITY_FAIL_EID, Status);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_MessageTxn_GetOsTimeout(const CFE_SB_MessageTxn_State_t *TxnPtr)
{
    int32     OsTimeout;
    OS_time_t TimeNow;

    switch (TxnPtr->TimeoutMode)
    {
        case CFE_SB_MessageTxn_TimeoutMode_PEND:
            OsTimeout = OS_PEND;
            break;
        case CFE_SB_MessageTxn_TimeoutMode_TIMED:
            CFE_PSP_GetTime(&TimeNow);
            OsTimeout = OS_TimeGetTotalMilliseconds(OS_TimeSubtract(TxnPtr->AbsTimeout, TimeNow));
            if (OsTimeout < 0)
            {
                /* timeout has already expired, so all remaining pipes should be CHECK only */
                OsTimeout = OS_CHECK;
            }
            break;
        case CFE_SB_MessageTxn_TimeoutMode_POLL:
        default:
            OsTimeout = OS_CHECK;
            break;
    }

    return OsTimeout;
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_TransmitTxn_PipeHandler(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *ContextPtr, void *Arg)
{
    CFE_SB_DestinationD_t *DestPtr;
    CFE_SB_PipeD_t *       PipeDscPtr;
    CFE_SB_BufferD_t *     BufDscPtr;

    BufDscPtr = Arg;

    /*
     * Write the buffer descriptor to the queue of the pipe.  Note that
     * accounting for depth and buffer limits was already done as part
     * of "FindDestinations" assuming this write will be successful - which
     * is the expected/typical result here.
     */
    ContextPtr->OsStatus =
        OS_QueuePut(ContextPtr->SysQueueId, &BufDscPtr, sizeof(BufDscPtr), CFE_SB_MessageTxn_GetOsTimeout(TxnPtr));

    /*
     * If it succeeded, nothing else to do.  But if it fails then we must undo the
     * optimistic depth accounting done earlier.
     */
    if (ContextPtr->OsStatus != OS_SUCCESS)
    {
        ++TxnPtr->NumPipeErrs;

        CFE_SB_LockSharedData(__func__, __LINE__);

        if (ContextPtr->OsStatus == OS_QUEUE_FULL)
        {
            ContextPtr->PendingEventId = CFE_SB_Q_FULL_ERR_EID;
            CFE_SB_Global.HKTlmMsg.Payload.PipeOverflowErrorCounter++;
        }
        else
        {
            /* Unexpected error while writing to queue. */
            ContextPtr->PendingEventId = CFE_SB_Q_WR_ERR_EID;
            CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter++;
        }

        PipeDscPtr = CFE_SB_LocatePipeDescByID(ContextPtr->PipeId);
        if (CFE_SB_PipeDescIsMatch(PipeDscPtr, ContextPtr->PipeId) && PipeDscPtr->CurrentQueueDepth > 0)
        {
            --PipeDscPtr->CurrentQueueDepth;
        }

        DestPtr = CFE_SB_GetDestPtr(BufDscPtr->DestRouteId, ContextPtr->PipeId);
        if (DestPtr != NULL && DestPtr->BuffCount > 0)
        {
            DestPtr->BuffCount--;
        }

        CFE_SB_DecrBufUseCnt(BufDscPtr);

        CFE_SB_UnlockSharedData(__func__, __LINE__);
    }

    /* always keep going when sending (broadcast) */
    return true;
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_ProcessPipes(CFE_SB_MessageTxn_PipeHandler_t HandlerFunc, CFE_SB_MessageTxn_State_t *TxnPtr,
                                    void *Arg)
{
    uint32                 i;
    CFE_SB_PipeSetEntry_t *ContextPtr;
    bool                   should_continue;

    should_continue = true;
    for (i = 0; should_continue && i < TxnPtr->NumPipes; ++i)
    {
        ContextPtr = &TxnPtr->PipeSet[i];

        if (ContextPtr->PendingEventId == 0)
        {
            should_continue = HandlerFunc(TxnPtr, ContextPtr, Arg);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TransmitTxn_Execute(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_Buffer_t *BufPtr)
{
    int32             Status;
    CFE_SB_BufferD_t *BufDscPtr;

    /* Sanity check on the input buffer - if this doesn't work, stop now */
    Status = CFE_SB_ZeroCopyBufferValidate(BufPtr, &BufDscPtr);
    if (Status != CFE_SUCCESS)
    {
        /* There is currently no event defined for this */
        CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, 0, Status);
        return;
    }

    /* Save passed-in routing parameters into the descriptor */
    BufDscPtr->ContentSize = CFE_SB_MessageTxn_GetContentSize(TxnPtr);
    BufDscPtr->MsgId       = CFE_SB_MessageTxn_GetRoutingMsgId(TxnPtr);

    /* Convert the route to a set of pipes/destinations */
    CFE_SB_TransmitTxn_FindDestinations(TxnPtr, BufDscPtr);

    /* Note the above function always succeeds - even if no pipes are subscribed,
     * the transaction will simply have 0 pipes and this next call becomes a no-op */
    CFE_SB_MessageTxn_ProcessPipes(CFE_SB_TransmitTxn_PipeHandler, TxnPtr, BufDscPtr);

    /*
     * Decrement the buffer UseCount - This means that the caller
     * should not use the buffer anymore after this call.
     */
    CFE_SB_LockSharedData(__func__, __LINE__);
    CFE_SB_DecrBufUseCnt(BufDscPtr);
    CFE_SB_UnlockSharedData(__func__, __LINE__);
}

/******************************************************************
 *
 * RECEIVE TRANSACTION IMPLEMENTATION FUNCTIONS
 *
 ******************************************************************/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MessageTxn_State_t *CFE_SB_ReceiveTxn_Init(CFE_SB_ReceiveTxn_State_t *TxnPtr, const void *RefMemPtr)
{
    CFE_SB_MessageTxn_Init(&TxnPtr->MessageTxn_State, &TxnPtr->Source, 1, RefMemPtr);
    TxnPtr->MessageTxn_State.IsTransmit = false;

    /* No matter what, the mem pointer from the caller should not be NULL */
    /* note that the event ID is different between send and recv */
    if (RefMemPtr == NULL)
    {
        CFE_SB_MessageTxn_SetEventAndStatus(&TxnPtr->MessageTxn_State, CFE_SB_RCV_BAD_ARG_EID, CFE_SB_BAD_ARGUMENT);
    }

    return &TxnPtr->MessageTxn_State;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_MessageTxn_SetEndpoint(CFE_SB_MessageTxn_State_t *TxnPtr, bool IsEndpoint)
{
    TxnPtr->IsEndpoint = IsEndpoint;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_ReceiveTxn_SetPipeId(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeId_t PipeId)
{
    CFE_SB_PipeD_t *       PipeDscPtr;
    CFE_SB_PipeSetEntry_t *ContextPtr;

    /* For now, there is just one of these */
    ContextPtr = TxnPtr->PipeSet;
    memset(ContextPtr, 0, sizeof(*ContextPtr));
    ContextPtr->PipeId = PipeId;
    TxnPtr->NumPipes   = 1;

    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);

    CFE_SB_LockSharedData(__func__, __LINE__);

    /* If the pipe does not exist or PipeId is out of range... */
    if (!CFE_SB_PipeDescIsMatch(PipeDscPtr, PipeId))
    {
        ContextPtr->PendingEventId = CFE_SB_BAD_PIPEID_EID;
        CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, 0, CFE_SB_BAD_ARGUMENT);
        ++TxnPtr->NumPipeErrs;
    }
    else
    {
        ContextPtr->SysQueueId = PipeDscPtr->SysQueueId;

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

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
void CFE_SB_ReceiveTxn_ExportReference(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *ContextPtr,
                                       CFE_SB_BufferD_t *BufDscPtr, CFE_SB_BufferD_t **ParentBufDscPtrP)
{
    CFE_SB_PipeD_t *       PipeDscPtr;
    CFE_SB_DestinationD_t *DestPtr;

    PipeDscPtr = CFE_SB_LocatePipeDescByID(ContextPtr->PipeId);

    /* Now re-lock to store the buffer in the pipe descriptor */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /*
     * NOTE: This uses the same PipeDscPtr that was found earlier.
     * But it has to be revalidated because its theoretically possible
     * the pipe got deleted between now and then.
     */
    if (CFE_SB_PipeDescIsMatch(PipeDscPtr, ContextPtr->PipeId))
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
        *ParentBufDscPtrP = BufDscPtr;

        /* get pointer to destination to be used in decrementing msg limit cnt*/
        DestPtr = CFE_SB_GetDestPtr(BufDscPtr->DestRouteId, ContextPtr->PipeId);

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
        CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, 0, CFE_SB_PIPE_RD_ERR);

        /* should send the bad pipe ID event here too */
        ContextPtr->PendingEventId = CFE_SB_BAD_PIPEID_EID;
    }

    /* Always decrement the use count, for the ref that was in the queue */
    CFE_SB_DecrBufUseCnt(BufDscPtr);

    CFE_SB_UnlockSharedData(__func__, __LINE__);
}

/*----------------------------------------------------------------
 *
 * Local Helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_ReceiveTxn_PipeHandler(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *ContextPtr, void *Arg)
{
    CFE_SB_BufferD_t * BufDscPtr;
    CFE_SB_BufferD_t **ParentBufDscPtrP;
    size_t             BufDscSize;

    ParentBufDscPtrP = Arg;

    /* Read the buffer descriptor address from the queue.  */
    ContextPtr->OsStatus = OS_QueueGet(ContextPtr->SysQueueId, &BufDscPtr, sizeof(BufDscPtr), &BufDscSize,
                                       CFE_SB_MessageTxn_GetOsTimeout(TxnPtr));

    /*
     * translate the return value -
     *
     * CFE functions have their own set of RC values should not directly return OSAL codes
     * The size should always match.  If it does not, then generate CFE_SB_Q_RD_ERR_EID.
     */

    if (ContextPtr->OsStatus == OS_SUCCESS && BufDscPtr != NULL && BufDscSize == sizeof(BufDscPtr))
    {
        CFE_SB_ReceiveTxn_ExportReference(TxnPtr, ContextPtr, BufDscPtr, ParentBufDscPtrP);
    }
    else
    {
        *ParentBufDscPtrP = NULL;

        if (ContextPtr->OsStatus == OS_QUEUE_EMPTY)
        {
            /* normal if using CFE_SB_POLL */
            CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, 0, CFE_SB_NO_MESSAGE);
        }
        else if (ContextPtr->OsStatus == OS_QUEUE_TIMEOUT)
        {
            /* normal if using a nonzero timeout */
            CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, 0, CFE_SB_TIME_OUT);
        }
        else
        {
            /* off-nominal condition, report an error event */
            CFE_SB_MessageTxn_SetEventAndStatus(TxnPtr, 0, CFE_SB_PIPE_RD_ERR);
            ContextPtr->PendingEventId = CFE_SB_Q_RD_ERR_EID;
        }
    }

    /* Read ops only process one pipe */
    return false;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
const CFE_SB_Buffer_t *CFE_SB_ReceiveTxn_Execute(CFE_SB_MessageTxn_State_t *TxnPtr)
{
    CFE_SB_BufferD_t *     BufDscPtr;
    const CFE_SB_Buffer_t *Result;
    bool                   IsAcceptable;
    CFE_Status_t           Status;

    Result = NULL;

    while (CFE_SB_MessageTxn_IsOK(TxnPtr))
    {
        BufDscPtr = NULL;

        /*
         * Read from the pipe(s).  Currently this is just one but someday could
         * become more than one e.g. a high-pri and a low-pri queue.
         */
        CFE_SB_MessageTxn_ProcessPipes(CFE_SB_ReceiveTxn_PipeHandler, TxnPtr, &BufDscPtr);

        /* If nothing received, then quit */
        if (BufDscPtr == NULL)
        {
            TxnPtr->RoutingMsgId = CFE_SB_INVALID_MSG_ID;
            TxnPtr->ContentSize  = 0;
            Result               = NULL;
            break;
        }

        if (TxnPtr->IsEndpoint)
        {
            Status = CFE_MSG_VerificationAction(&BufDscPtr->Content.Msg, BufDscPtr->AllocatedSize, &IsAcceptable);
            if (Status != CFE_SUCCESS)
            {
                /* This typically should not happen - only if VerificationAction got bad arguments */
                IsAcceptable = false;
            }
        }
        else
        {
            /* If no verification being done at this stage - consider everything "good" */
            IsAcceptable = true;
        }

        if (IsAcceptable)
        {
            /*
             * Replicate the buffer descriptor MsgId and ContentSize in the transaction.
             *
             * This does not use the functions that check the values because that will
             * send an event and set an error status if they are bad.  But on the recv side,
             * it is already here - whatever we got should be returned.  Error checking for
             * size and MsgId should have been done on the transmit side, so they should never
             * be bad at this point.
             */
            TxnPtr->RoutingMsgId = BufDscPtr->MsgId;
            TxnPtr->ContentSize  = BufDscPtr->ContentSize;
            Result               = &BufDscPtr->Content;
            break;
        }

        /* Report an event indicating the buffer is being dropped */
        CFE_SB_MessageTxn_ReportSingleEvent(TxnPtr, TxnPtr->PipeSet, CFE_SB_RCV_MESSAGE_INTEGRITY_FAIL_EID);

        /*
         * Also need to re-set the PipeId for proper accounting.  This buffer will be dropped,
         * and this decrements the use count and removes it from the LastBuffer pointer in the
         * Pipe Descriptor
         */
        CFE_SB_ReceiveTxn_SetPipeId(TxnPtr, TxnPtr->PipeSet->PipeId);
    }

    return Result;
}
