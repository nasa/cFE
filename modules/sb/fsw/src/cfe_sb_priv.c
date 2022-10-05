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
uint32 CFE_SB_RequestToSendEvent(CFE_ES_TaskId_t TaskId, uint32 Bit)
{
    uint32 Indx;

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
void CFE_SB_FinishSendEvent(CFE_ES_TaskId_t TaskId, uint32 Bit)
{
    uint32 Indx;

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
