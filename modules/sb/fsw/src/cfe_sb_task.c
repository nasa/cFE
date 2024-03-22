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
** File: cfe_sb_task.c
**
** Purpose:
**      This file contains the source code for the SB task.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

/* Include Files */

#include "cfe_sb_module_all.h"
#include "cfe_version.h"
#include "cfe_config.h" /* For version string construction */
#include "cfe_es_msg.h" /* needed for local use of CFE_ES_RestartCmd_t */
#include "cfe_sb_verify.h"

#include <string.h>

/*  Task Globals */
CFE_SB_Global_t CFE_SB_Global;

/* Local structure for file writing callbacks */
typedef struct
{
    const char *Filename;   /* File name for error reporting */
    osal_id_t   Fd;         /* File id for writing */
    size_t      FileSize;   /* File size for reporting */
    uint32      EntryCount; /* Entry count for reporting */
    int32       Status;     /* File write status */
} CFE_SB_FileWriteCallback_t;

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TaskMain(void)
{
    int32            Status;
    CFE_SB_Buffer_t *SBBufPtr;

    CFE_ES_PerfLogEntry(CFE_MISSION_SB_MAIN_PERF_ID);

    Status = CFE_SB_AppInit();

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Application Init Failed,RC=0x%08X\n", __func__, (unsigned int)Status);
        CFE_ES_PerfLogExit(CFE_MISSION_SB_MAIN_PERF_ID);
        /* Note: CFE_ES_ExitApp will not return */
        CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    }

    /*
     * Wait for other apps to start.
     * It is important that the core apps are present before this starts receiving
     * messages from the command pipe, as some of those handlers might depend on
     * the other core apps.
     */
    CFE_ES_WaitForSystemState(CFE_ES_SystemState_CORE_READY, CFE_PLATFORM_CORE_MAX_STARTUP_MSEC);

    /* Main loop */
    while (Status == CFE_SUCCESS)
    {
        /* Increment the Main task Execution Counter */
        CFE_ES_IncrementTaskCounter();

        CFE_ES_PerfLogExit(CFE_MISSION_SB_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_ReceiveBuffer(&SBBufPtr, CFE_SB_Global.CmdPipe, CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_MISSION_SB_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_SB_ProcessCmdPipePkt(SBBufPtr);
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Error reading cmd pipe,RC=0x%08X\n", __func__, (unsigned int)Status);
        }

    } /* end while */

    /* while loop exits only if CFE_SB_ReceiveBuffer returns error */
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_AppInit(void)
{
    uint32              CfgFileEventsToFilter = 0;
    CFE_ES_MemPoolBuf_t TmpPtr;
    int32               Status;
    char                VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /* Get the assigned Application ID for the SB Task */
    CFE_ES_GetAppID(&CFE_SB_Global.AppId);

    /* Process the platform cfg file events to be filtered */
    if (CFE_PLATFORM_SB_FILTERED_EVENT1 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT1;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK1;
        CfgFileEventsToFilter++;
    }

    if (CFE_PLATFORM_SB_FILTERED_EVENT2 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT2;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK2;
        CfgFileEventsToFilter++;
    }

    if (CFE_PLATFORM_SB_FILTERED_EVENT3 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT3;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK3;
        CfgFileEventsToFilter++;
    }

    if (CFE_PLATFORM_SB_FILTERED_EVENT4 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT4;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK4;
        CfgFileEventsToFilter++;
    }

    if (CFE_PLATFORM_SB_FILTERED_EVENT5 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT5;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK5;
        CfgFileEventsToFilter++;
    }

    if (CFE_PLATFORM_SB_FILTERED_EVENT6 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT6;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK6;
        CfgFileEventsToFilter++;
    }

    if (CFE_PLATFORM_SB_FILTERED_EVENT7 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT7;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK7;
        CfgFileEventsToFilter++;
    }

    if (CFE_PLATFORM_SB_FILTERED_EVENT8 != 0)
    {
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT8;
        CFE_SB_Global.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK8;
        CfgFileEventsToFilter++;
    }

    /* Be sure the number of events to register for filtering
    ** does not exceed CFE_PLATFORM_EVS_MAX_EVENT_FILTERS */
    if (CFE_PLATFORM_EVS_MAX_EVENT_FILTERS < CfgFileEventsToFilter)
    {
        CfgFileEventsToFilter = CFE_PLATFORM_EVS_MAX_EVENT_FILTERS;
    }

    /* Register event filter table... */
    Status = CFE_EVS_Register(CFE_SB_Global.EventFilters, CfgFileEventsToFilter, CFE_EVS_EventFilter_BINARY);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_EVS_Register Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    CFE_ES_WriteToSysLog("%s: Registered %d events for filtering\n", __func__, (int)CfgFileEventsToFilter);

    CFE_MSG_Init(CFE_MSG_PTR(CFE_SB_Global.HKTlmMsg.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID),
                 sizeof(CFE_SB_Global.HKTlmMsg));

    CFE_MSG_Init(CFE_MSG_PTR(CFE_SB_Global.PrevSubMsg.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID),
                 sizeof(CFE_SB_Global.PrevSubMsg));

    /* Populate the fixed fields in the HK Tlm Msg */
    CFE_SB_Global.HKTlmMsg.Payload.MemPoolHandle = CFE_SB_Global.Mem.PoolHdl;

    /* Populate the fixed fields in the Stat Tlm Msg */
    CFE_SB_Global.StatTlmMsg.Payload.MaxMsgIdsAllowed    = CFE_PLATFORM_SB_MAX_MSG_IDS;
    CFE_SB_Global.StatTlmMsg.Payload.MaxPipesAllowed     = CFE_PLATFORM_SB_MAX_PIPES;
    CFE_SB_Global.StatTlmMsg.Payload.MaxMemAllowed       = CFE_PLATFORM_SB_BUF_MEMORY_BYTES;
    CFE_SB_Global.StatTlmMsg.Payload.MaxPipeDepthAllowed = OS_QUEUE_MAX_DEPTH;
    CFE_SB_Global.StatTlmMsg.Payload.MaxSubscriptionsAllowed =
        ((CFE_PLATFORM_SB_MAX_MSG_IDS) * (CFE_PLATFORM_SB_MAX_DEST_PER_PKT));

    Status = CFE_SB_CreatePipe(&CFE_SB_Global.CmdPipe, CFE_SB_CMD_PIPE_DEPTH, CFE_SB_CMD_PIPE_NAME);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_SB_CMD_MID), CFE_SB_Global.CmdPipe);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Subscribe to Cmds Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_SB_SEND_HK_MID), CFE_SB_Global.CmdPipe);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Subscribe to HK Request Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    Status = CFE_SB_Subscribe(CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID), CFE_SB_Global.CmdPipe);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Subscribe to Subscription Report Request Failed:RC=0x%08X\n", __func__,
                             (unsigned int)Status);
        return Status;
    }

    /* Ensure a ground commanded reset does not get blocked if SB mem pool  */
    /* becomes fully configured (DCR6772) */
    Status = CFE_ES_GetPoolBuf(&TmpPtr, CFE_SB_Global.Mem.PoolHdl, sizeof(CFE_ES_RestartCmd_t));

    if (Status < 0)
    {
        CFE_ES_WriteToSysLog("%s: Init error, GetPool Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    /* Return mem block used on previous call,the actual memory is not needed.*/
    /* The SB mem pool is now configured with a block size for the reset cmd. */
    Status = CFE_ES_PutPoolBuf(CFE_SB_Global.Mem.PoolHdl, TmpPtr);

    if (Status < 0)
    {
        CFE_ES_WriteToSysLog("%s: Init error, PutPool Failed:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE",
        CFE_SRC_VERSION, CFE_BUILD_CODENAME, CFE_LAST_OFFICIAL);
    Status =
        CFE_EVS_SendEvent(CFE_SB_INIT_EID, CFE_EVS_EventType_INFORMATION, "cFE SB Initialized: %s", VersionString);
    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error sending init event:RC=0x%08X\n", __func__, (unsigned int)Status);
        return Status;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_NoopCmd(const CFE_SB_NoopCmd_t *data)
{
    char VersionString[CFE_CFG_MAX_VERSION_STR_LEN];
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE",
        CFE_SRC_VERSION, CFE_BUILD_CODENAME, CFE_LAST_OFFICIAL);
    CFE_EVS_SendEvent(CFE_SB_CMD0_RCVD_EID, CFE_EVS_EventType_INFORMATION, "No-op Cmd Rcvd: %s", VersionString);
    CFE_SB_Global.HKTlmMsg.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_ResetCountersCmd(const CFE_SB_ResetCountersCmd_t *data)
{
    CFE_EVS_SendEvent(CFE_SB_CMD1_RCVD_EID, CFE_EVS_EventType_DEBUG, "Reset Counters Cmd Rcvd");

    CFE_SB_ResetCounters();

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_EnableSubReportingCmd(const CFE_SB_EnableSubReportingCmd_t *data)
{
    CFE_SB_SetSubscriptionReporting(CFE_SB_ENABLE);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_DisableSubReportingCmd(const CFE_SB_DisableSubReportingCmd_t *data)
{
    CFE_SB_SetSubscriptionReporting(CFE_SB_DISABLE);
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_SendHKTlmCmd(const CFE_SB_SendHkCmd_t *data)
{
    CFE_SB_LockSharedData(__FILE__, __LINE__);

    CFE_SB_Global.HKTlmMsg.Payload.MemInUse = CFE_SB_Global.StatTlmMsg.Payload.MemInUse;
    CFE_SB_Global.HKTlmMsg.Payload.UnmarkedMem =
        CFE_PLATFORM_SB_BUF_MEMORY_BYTES - CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse;

    CFE_SB_UnlockSharedData(__FILE__, __LINE__);

    CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_SB_Global.HKTlmMsg.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_SB_Global.HKTlmMsg.TelemetryHeader), true);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_ResetCounters(void)
{
    CFE_SB_Global.HKTlmMsg.Payload.CommandCounter                = 0;
    CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter           = 0;
    CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter          = 0;
    CFE_SB_Global.HKTlmMsg.Payload.DuplicateSubscriptionsCounter = 0;
    CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter           = 0;
    CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter        = 0;
    CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter          = 0;
    CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter        = 0;
    CFE_SB_Global.HKTlmMsg.Payload.SubscribeErrorCounter         = 0;
    CFE_SB_Global.HKTlmMsg.Payload.PipeOverflowErrorCounter      = 0;
    CFE_SB_Global.HKTlmMsg.Payload.MsgLimitErrorCounter          = 0;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_EnableRouteCmd(const CFE_SB_EnableRouteCmd_t *data)
{
    CFE_SB_MsgId_t                   MsgId;
    CFE_SB_PipeD_t *                 PipeDscPtr;
    CFE_SB_DestinationD_t *          DestPtr;
    const CFE_SB_RouteCmd_Payload_t *CmdPtr;
    uint16                           PendingEventID;

    PendingEventID = 0;
    CmdPtr         = &data->Payload;

    MsgId = CmdPtr->MsgId;

    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check cmd parameters */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(CmdPtr->Pipe);
    if (!CFE_SB_IsValidMsgId(MsgId) || !CFE_SB_PipeDescIsMatch(PipeDscPtr, CmdPtr->Pipe))
    {
        PendingEventID = CFE_SB_ENBL_RTE3_EID;
        CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
    }
    else
    {
        DestPtr = CFE_SB_GetDestPtr(CFE_SBR_GetRouteId(MsgId), CmdPtr->Pipe);
        if (DestPtr == NULL)
        {
            PendingEventID = CFE_SB_ENBL_RTE1_EID;
            CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
        }
        else
        {
            DestPtr->Active = CFE_SB_ACTIVE;
            PendingEventID  = CFE_SB_ENBL_RTE2_EID;
            CFE_SB_Global.HKTlmMsg.Payload.CommandCounter++;
        }
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    switch (PendingEventID)
    {
        case CFE_SB_ENBL_RTE1_EID:
            CFE_EVS_SendEvent(CFE_SB_ENBL_RTE1_EID, CFE_EVS_EventType_ERROR,
                              "Enbl Route Cmd:Route does not exist.Msg 0x%x,Pipe %lu",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(CmdPtr->Pipe));
            break;
        case CFE_SB_ENBL_RTE3_EID:
            CFE_EVS_SendEvent(CFE_SB_ENBL_RTE3_EID, CFE_EVS_EventType_ERROR,
                              "Enbl Route Cmd:Invalid Param.Msg 0x%x,Pipe %lu",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(CmdPtr->Pipe));
            break;
        case CFE_SB_ENBL_RTE2_EID:
            CFE_EVS_SendEvent(CFE_SB_ENBL_RTE2_EID, CFE_EVS_EventType_DEBUG, "Enabling Route,Msg 0x%x,Pipe %lu",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(CmdPtr->Pipe));
            break;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_DisableRouteCmd(const CFE_SB_DisableRouteCmd_t *data)
{
    CFE_SB_MsgId_t                   MsgId;
    CFE_SB_PipeD_t *                 PipeDscPtr;
    CFE_SB_DestinationD_t *          DestPtr;
    const CFE_SB_RouteCmd_Payload_t *CmdPtr;
    uint16                           PendingEventID;

    PendingEventID = 0;
    CmdPtr         = &data->Payload;

    MsgId = CmdPtr->MsgId;

    CFE_SB_LockSharedData(__func__, __LINE__);

    /* check cmd parameters */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(CmdPtr->Pipe);
    if (!CFE_SB_IsValidMsgId(MsgId) || !CFE_SB_PipeDescIsMatch(PipeDscPtr, CmdPtr->Pipe))
    {
        PendingEventID = CFE_SB_DSBL_RTE3_EID;
        CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
    }
    else
    {
        DestPtr = CFE_SB_GetDestPtr(CFE_SBR_GetRouteId(MsgId), CmdPtr->Pipe);
        if (DestPtr == NULL)
        {
            PendingEventID = CFE_SB_DSBL_RTE1_EID;
            CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
        }
        else
        {
            DestPtr->Active = CFE_SB_INACTIVE;
            PendingEventID  = CFE_SB_DSBL_RTE2_EID;
            CFE_SB_Global.HKTlmMsg.Payload.CommandCounter++;
        }
    }

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    switch (PendingEventID)
    {
        case CFE_SB_DSBL_RTE1_EID:
            CFE_EVS_SendEvent(CFE_SB_DSBL_RTE1_EID, CFE_EVS_EventType_ERROR,
                              "Disable Route Cmd:Route does not exist,Msg 0x%x,Pipe %lu",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(CmdPtr->Pipe));
            break;
        case CFE_SB_DSBL_RTE3_EID:
            CFE_EVS_SendEvent(CFE_SB_DSBL_RTE3_EID, CFE_EVS_EventType_ERROR,
                              "Disable Route Cmd:Invalid Param.Msg 0x%x,Pipe %lu",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(CmdPtr->Pipe));
            break;
        case CFE_SB_DSBL_RTE2_EID:
            CFE_EVS_SendEvent(CFE_SB_DSBL_RTE2_EID, CFE_EVS_EventType_DEBUG, "Route Disabled,Msg 0x%x,Pipe %lu",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(CmdPtr->Pipe));
            break;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_SendStatsCmd(const CFE_SB_SendSbStatsCmd_t *data)
{
    uint32                   PipeDscCount;
    uint32                   PipeStatCount;
    CFE_SB_PipeD_t *         PipeDscPtr;
    CFE_SB_PipeDepthStats_t *PipeStatPtr;

    CFE_SB_LockSharedData(__FILE__, __LINE__);

    /* Collect data on pipes */
    PipeDscCount  = CFE_PLATFORM_SB_MAX_PIPES;
    PipeStatCount = CFE_MISSION_SB_MAX_PIPES;
    PipeDscPtr    = CFE_SB_Global.PipeTbl;
    PipeStatPtr   = CFE_SB_Global.StatTlmMsg.Payload.PipeDepthStats;

    while (PipeDscCount > 0 && PipeStatCount > 0)
    {
        if (CFE_SB_PipeDescIsUsed(PipeDscPtr))
        {
            PipeStatPtr->PipeId = PipeDscPtr->PipeId;

            /* Copy depth info */
            PipeStatPtr->CurrentQueueDepth = PipeDscPtr->CurrentQueueDepth;
            PipeStatPtr->PeakQueueDepth    = PipeDscPtr->PeakQueueDepth;
            PipeStatPtr->MaxQueueDepth     = PipeDscPtr->MaxQueueDepth;

            ++PipeStatPtr;
            --PipeStatCount;
        }

        --PipeDscCount;
        ++PipeDscPtr;
    }

    CFE_SB_UnlockSharedData(__FILE__, __LINE__);

    while (PipeStatCount > 0)
    {
        memset(PipeStatPtr, 0, sizeof(*PipeStatPtr));

        ++PipeStatPtr;
        --PipeStatCount;
    }

    CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_SB_Global.StatTlmMsg.TelemetryHeader));
    CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_SB_Global.StatTlmMsg.TelemetryHeader), true);

    CFE_EVS_SendEvent(CFE_SB_SND_STATS_EID, CFE_EVS_EventType_DEBUG, "Software Bus Statistics packet sent");

    CFE_SB_Global.HKTlmMsg.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_CollectRouteInfo(CFE_SBR_RouteId_t RouteId, void *ArgPtr)
{
    CFE_SB_DestinationD_t *             DestPtr;
    CFE_SB_PipeD_t *                    PipeDscPtr;
    CFE_SB_MsgId_t                      RouteMsgId;
    CFE_SB_BackgroundRouteInfoBuffer_t *RouteBufferPtr;
    CFE_SB_RoutingFileEntry_t *         FileEntryPtr;
    CFE_ES_AppId_t                      DestAppId[CFE_PLATFORM_SB_MAX_DEST_PER_PKT];
    uint32                              i;

    /* Cast arguments for local use */
    RouteBufferPtr = (CFE_SB_BackgroundRouteInfoBuffer_t *)ArgPtr;

    /* Extract data from runtime info, write into the temporary buffer */
    /* Data must be locked to snapshot the route info */
    CFE_SB_LockSharedData(__FILE__, __LINE__);

    RouteMsgId                      = CFE_SBR_GetMsgId(RouteId);
    RouteBufferPtr->NumDestinations = 0;

    /* If this is a valid route, get the destinations */
    if (CFE_SB_IsValidMsgId(RouteMsgId))
    {
        DestPtr = CFE_SBR_GetDestListHeadPtr(RouteId);

        /* copy relevant data from the destination list into the temp buffer */
        while (DestPtr != NULL && RouteBufferPtr->NumDestinations < CFE_PLATFORM_SB_MAX_DEST_PER_PKT)
        {
            PipeDscPtr = CFE_SB_LocatePipeDescByID(DestPtr->PipeId);

            /* If invalid id, continue on to next entry */
            if (CFE_SB_PipeDescIsMatch(PipeDscPtr, DestPtr->PipeId))
            {
                FileEntryPtr = &RouteBufferPtr->DestEntries[RouteBufferPtr->NumDestinations];

                /* clear all fields in the temp buffer before re-use */
                memset(FileEntryPtr, 0, sizeof(*FileEntryPtr));

                FileEntryPtr->PipeId = DestPtr->PipeId;
                FileEntryPtr->State  = DestPtr->Active;
                FileEntryPtr->MsgCnt = DestPtr->DestCnt;

                /* Stash the Pipe Owner AppId - App Name is looked up later (comes from ES) */
                DestAppId[RouteBufferPtr->NumDestinations] = PipeDscPtr->AppId;

                ++RouteBufferPtr->NumDestinations;
            }

            DestPtr = DestPtr->Next;
        }
    }

    CFE_SB_UnlockSharedData(__FILE__, __LINE__);

    /* Go through the temp buffer and fill in the remaining info for each dest */
    FileEntryPtr = RouteBufferPtr->DestEntries;
    for (i = 0; i < RouteBufferPtr->NumDestinations; ++i)
    {
        /* All dest entries refer to the same MsgId (based on the route) */
        FileEntryPtr->MsgId = RouteMsgId;

        /*
         * NOTE: as long as CFE_ES_GetAppName() is given a nonzero-length
         * output buffer, it guarantees null termination of the output, even
         * if the AppID is invalid - in which case it returns an empty string.
         */
        CFE_ES_GetAppName(FileEntryPtr->AppName, DestAppId[i], sizeof(FileEntryPtr->AppName));
        CFE_SB_GetPipeName(FileEntryPtr->PipeName, sizeof(FileEntryPtr->PipeName), FileEntryPtr->PipeId);

        ++FileEntryPtr;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_SendSubscriptionReport(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality)
{
    CFE_SB_SingleSubscriptionTlm_t SubRptMsg;
    int32                          Status = CFE_SUCCESS;

    memset(&SubRptMsg, 0, sizeof(SubRptMsg));

    if (CFE_SB_Global.SubscriptionReporting == CFE_SB_ENABLE)
    {
        CFE_MSG_Init(CFE_MSG_PTR(SubRptMsg.TelemetryHeader), CFE_SB_ValueToMsgId(CFE_SB_ONESUB_TLM_MID),
                     sizeof(SubRptMsg));

        SubRptMsg.Payload.MsgId   = MsgId;
        SubRptMsg.Payload.Pipe    = PipeId;
        SubRptMsg.Payload.Qos     = Quality;
        SubRptMsg.Payload.SubType = CFE_SB_SUBSCRIPTION;

        Status = CFE_SB_TransmitMsg(CFE_MSG_PTR(SubRptMsg.TelemetryHeader), true);
        CFE_EVS_SendEventWithAppID(CFE_SB_SUBSCRIPTION_RPT_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                   "Sending Subscription Report Msg=0x%x,Pipe=%lu,Stat=0x%x",
                                   (unsigned int)CFE_SB_MsgIdToValue(MsgId), CFE_RESOURCEID_TO_ULONG(PipeId),
                                   (unsigned int)Status);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_WriteRouteInfoDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    CFE_SB_BackgroundFileStateInfo_t *BgFilePtr;
    CFE_SBR_Throttle_t                Throttle;

    /* Cast arguments for local use */
    BgFilePtr = (CFE_SB_BackgroundFileStateInfo_t *)Meta;

    Throttle.StartIndex = RecordNum;
    Throttle.MaxLoop    = 1;
    Throttle.NextIndex  = 0;

    /* Reset NumDestinations to 0, just in case the CFE_SBR_ForEachRouteId() is a no-op */
    BgFilePtr->Buffer.RouteInfo.NumDestinations = 0;

    /* Collect info on the next route (limited to one per cycle via throttle) */
    CFE_SBR_ForEachRouteId(CFE_SB_CollectRouteInfo, &BgFilePtr->Buffer.RouteInfo, &Throttle);

    /* Pass the output of CFE_SB_CollectRouteInfo() back to be written */
    *Buffer  = &BgFilePtr->Buffer.RouteInfo.DestEntries;
    *BufSize = sizeof(CFE_SB_RoutingFileEntry_t) * BgFilePtr->Buffer.RouteInfo.NumDestinations;

    /* Check for EOF (last entry) - NextIndex is nonzero if more records left, zero at the end of the route table */
    return (Throttle.NextIndex == 0);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_BackgroundFileEventHandler(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum,
                                       size_t BlockSize, size_t Position)
{
    CFE_SB_BackgroundFileStateInfo_t *BgFilePtr;

    BgFilePtr = (CFE_SB_BackgroundFileStateInfo_t *)Meta;

    /*
     * Note that this runs in the context of ES background task (file writer background job)
     * It does NOT run in the context of the CFE_TBL app task.
     *
     * Events should use CFE_EVS_SendEventWithAppID() rather than CFE_EVS_SendEvent()
     * to get proper association with TBL task.
     */
    switch (Event)
    {
        case CFE_FS_FileWriteEvent_COMPLETE:
            CFE_EVS_SendEventWithAppID(CFE_SB_SND_RTG_EID, CFE_EVS_EventType_DEBUG, CFE_SB_Global.AppId,
                                       "%s written:Size=%d,Entries=%d", BgFilePtr->FileWrite.FileName, (int)Position,
                                       (int)RecordNum);
            break;

        case CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR:
        case CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR:
            CFE_EVS_SendEventWithAppID(CFE_SB_FILEWRITE_ERR_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "File write,byte cnt err,file %s,request=%d,actual=%d",
                                       BgFilePtr->FileWrite.FileName, (int)BlockSize, (int)Status);
            break;

        case CFE_FS_FileWriteEvent_CREATE_ERROR:
            CFE_EVS_SendEventWithAppID(CFE_SB_SND_RTG_ERR1_EID, CFE_EVS_EventType_ERROR, CFE_SB_Global.AppId,
                                       "Error creating file %s, stat=0x%x", BgFilePtr->FileWrite.FileName, (int)Status);
            break;

        default:
            /* unhandled event - ignore */
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_WriteRoutingInfoCmd(const CFE_SB_WriteRoutingInfoCmd_t *data)
{
    const CFE_SB_WriteFileInfoCmd_Payload_t *CmdPtr;
    CFE_SB_BackgroundFileStateInfo_t *       StatePtr;
    int32                                    Status;

    StatePtr = &CFE_SB_Global.BackgroundFile;
    CmdPtr   = &data->Payload;

    /* If a routing info dump was already pending, do not overwrite the current request */
    if (!CFE_FS_BackgroundFileDumpIsPending(&StatePtr->FileWrite))
    {
        /* Reset the entire state object (just for good measure, ensure no stale data) */
        memset(StatePtr, 0, sizeof(*StatePtr));

        /*
         * Fill out the remainder of meta data.
         * This data is currently the same for every request
         */
        StatePtr->FileWrite.FileSubType = CFE_FS_SubType_SB_ROUTEDATA;
        snprintf(StatePtr->FileWrite.Description, sizeof(StatePtr->FileWrite.Description), "SB Routing Information");

        StatePtr->FileWrite.GetData = CFE_SB_WriteRouteInfoDataGetter;
        StatePtr->FileWrite.OnEvent = CFE_SB_BackgroundFileEventHandler;

        /*
        ** Copy the filename into local buffer with default name/path/extension if not specified
        */
        Status = CFE_FS_ParseInputFileNameEx(StatePtr->FileWrite.FileName, CmdPtr->Filename,
                                             sizeof(StatePtr->FileWrite.FileName), sizeof(CmdPtr->Filename),
                                             CFE_PLATFORM_SB_DEFAULT_ROUTING_FILENAME,
                                             CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                             CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

        if (Status == CFE_SUCCESS)
        {
            Status = CFE_FS_BackgroundFileDumpRequest(&StatePtr->FileWrite);
        }
    }
    else
    {
        Status = CFE_STATUS_REQUEST_ALREADY_PENDING;
    }

    if (Status != CFE_SUCCESS)
    {
        /* generate the same event as is generated when unable to create the file (same thing, really) */
        CFE_SB_BackgroundFileEventHandler(StatePtr, CFE_FS_FileWriteEvent_CREATE_ERROR, Status, 0, 0, 0);
    }

    CFE_SB_IncrCmdCtr(Status);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_WritePipeInfoDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    CFE_SB_BackgroundFileStateInfo_t *BgFilePtr;
    CFE_SB_PipeInfoEntry_t *          PipeBufferPtr;
    CFE_SB_PipeD_t *                  PipeDscPtr;
    osal_id_t                         SysQueueId = OS_OBJECT_ID_UNDEFINED;
    bool                              PipeIsValid;

    BgFilePtr   = (CFE_SB_BackgroundFileStateInfo_t *)Meta;
    PipeDscPtr  = NULL;
    PipeIsValid = false;

    PipeBufferPtr = &BgFilePtr->Buffer.PipeInfo;

    if (RecordNum < CFE_PLATFORM_SB_MAX_PIPES)
    {
        PipeDscPtr = &CFE_SB_Global.PipeTbl[RecordNum];

        CFE_SB_LockSharedData(__FILE__, __LINE__);

        PipeIsValid = CFE_SB_PipeDescIsUsed(PipeDscPtr);

        if (PipeIsValid)
        {
            /*
             * Ensure any old data in the struct has been cleared
             */
            memset(PipeBufferPtr, 0, sizeof(*PipeBufferPtr));

            /*
             * Take a "snapshot" of the PipeDsc state while locked
             */
            PipeBufferPtr->PipeId = CFE_SB_PipeDescGetID(PipeDscPtr);
            PipeBufferPtr->AppId  = PipeDscPtr->AppId;
            PipeBufferPtr->Opts   = PipeDscPtr->Opts;

            /* copy stats info */
            PipeBufferPtr->SendErrors        = PipeDscPtr->SendErrors;
            PipeBufferPtr->MaxQueueDepth     = PipeDscPtr->MaxQueueDepth;
            PipeBufferPtr->CurrentQueueDepth = PipeDscPtr->CurrentQueueDepth;
            PipeBufferPtr->PeakQueueDepth    = PipeDscPtr->PeakQueueDepth;

            SysQueueId = PipeDscPtr->SysQueueId;
        }

        CFE_SB_UnlockSharedData(__FILE__, __LINE__);
    }

    if (PipeIsValid)
    {
        /*
         * Gather data from other subsystems while unlocked.
         * This might fail if the pipe is deleted simultaneously while this runs, but in
         * the unlikely event that happens, the name data will simply be blank as the ID(s)
         * will not validate.
         */
        OS_GetResourceName(SysQueueId, PipeBufferPtr->PipeName, sizeof(PipeBufferPtr->PipeName));
        CFE_ES_GetAppName(PipeBufferPtr->AppName, PipeBufferPtr->AppId, sizeof(PipeBufferPtr->AppName));

        *Buffer  = PipeBufferPtr;
        *BufSize = sizeof(*PipeBufferPtr);
    }
    else
    {
        *Buffer  = NULL;
        *BufSize = 0;
    }

    /* Check for EOF (last entry)  */
    return (RecordNum >= (CFE_PLATFORM_SB_MAX_PIPES - 1));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_WritePipeInfoCmd(const CFE_SB_WritePipeInfoCmd_t *data)
{
    const CFE_SB_WriteFileInfoCmd_Payload_t *CmdPtr;
    CFE_SB_BackgroundFileStateInfo_t *       StatePtr;
    int32                                    Status;

    StatePtr = &CFE_SB_Global.BackgroundFile;
    CmdPtr   = &data->Payload;

    /* If a pipe info dump was already pending, do not overwrite the current request */
    if (!CFE_FS_BackgroundFileDumpIsPending(&StatePtr->FileWrite))
    {
        /* Reset the entire state object (just for good measure, ensure no stale data) */
        memset(StatePtr, 0, sizeof(*StatePtr));

        /*
         * Fill out the remainder of meta data.
         * This data is currently the same for every request
         */
        StatePtr->FileWrite.FileSubType = CFE_FS_SubType_SB_PIPEDATA;
        snprintf(StatePtr->FileWrite.Description, sizeof(StatePtr->FileWrite.Description), "SB Pipe Information");

        StatePtr->FileWrite.GetData = CFE_SB_WritePipeInfoDataGetter;
        StatePtr->FileWrite.OnEvent = CFE_SB_BackgroundFileEventHandler;

        /*
        ** Copy the filename into local buffer with default name/path/extension if not specified
        */
        Status = CFE_FS_ParseInputFileNameEx(StatePtr->FileWrite.FileName, CmdPtr->Filename,
                                             sizeof(StatePtr->FileWrite.FileName), sizeof(CmdPtr->Filename),
                                             CFE_PLATFORM_SB_DEFAULT_PIPE_FILENAME,
                                             CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                             CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

        if (Status == CFE_SUCCESS)
        {
            Status = CFE_FS_BackgroundFileDumpRequest(&StatePtr->FileWrite);
        }
    }
    else
    {
        Status = CFE_STATUS_REQUEST_ALREADY_PENDING;
    }

    if (Status != CFE_SUCCESS)
    {
        /* generate the same event as is generated when unable to create the file (same thing, really) */
        CFE_SB_BackgroundFileEventHandler(StatePtr, CFE_FS_FileWriteEvent_CREATE_ERROR, Status, 0, 0, 0);
    }

    CFE_SB_IncrCmdCtr(Status);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_CollectMsgMapInfo(CFE_SBR_RouteId_t RouteId, void *ArgPtr)
{
    CFE_SB_MsgMapFileEntry_t *BufferPtr;

    /* Cast arguments for local use */
    BufferPtr = (CFE_SB_MsgMapFileEntry_t *)ArgPtr;

    /* Extract data from runtime info, write into the temporary buffer */
    /* Data must be locked to snapshot the route info */
    CFE_SB_LockSharedData(__FILE__, __LINE__);

    BufferPtr->MsgId = CFE_SBR_GetMsgId(RouteId);
    BufferPtr->Index = CFE_SBR_RouteIdToValue(RouteId);

    CFE_SB_UnlockSharedData(__FILE__, __LINE__);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_WriteMsgMapInfoDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    CFE_SB_BackgroundFileStateInfo_t *BgFilePtr;
    CFE_SBR_Throttle_t                Throttle;

    /* Cast arguments for local use */
    BgFilePtr = (CFE_SB_BackgroundFileStateInfo_t *)Meta;

    Throttle.StartIndex = RecordNum;
    Throttle.MaxLoop    = 1;
    Throttle.NextIndex  = 0;

    /* Set the MsgId initially - will be overwritten with real info in CFE_SB_CollectMsgMapInfo */
    BgFilePtr->Buffer.MsgMapInfo.MsgId = CFE_SB_INVALID_MSG_ID;

    /* Collect info on the next route (limited to one per cycle via throttle) */
    CFE_SBR_ForEachRouteId(CFE_SB_CollectMsgMapInfo, &BgFilePtr->Buffer.MsgMapInfo, &Throttle);

    /* If Map was valid, pass the output of CFE_SB_CollectMsgMapInfo() back to be written */
    if (CFE_SB_IsValidMsgId(BgFilePtr->Buffer.MsgMapInfo.MsgId))
    {
        *Buffer  = &BgFilePtr->Buffer.MsgMapInfo;
        *BufSize = sizeof(CFE_SB_MsgMapFileEntry_t);
    }
    else
    {
        *Buffer  = NULL;
        *BufSize = 0;
    }

    /* Check for EOF (last entry) - NextIndex is nonzero if more records left, zero at the end of the route table */
    return (Throttle.NextIndex == 0);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_WriteMapInfoCmd(const CFE_SB_WriteMapInfoCmd_t *data)
{
    const CFE_SB_WriteFileInfoCmd_Payload_t *CmdPtr;
    CFE_SB_BackgroundFileStateInfo_t *       StatePtr;
    int32                                    Status;

    StatePtr = &CFE_SB_Global.BackgroundFile;
    CmdPtr   = &data->Payload;

    /* If a pipe info dump was already pending, do not overwrite the current request */
    if (!CFE_FS_BackgroundFileDumpIsPending(&StatePtr->FileWrite))
    {
        /* Reset the entire state object (just for good measure, ensure no stale data) */
        memset(StatePtr, 0, sizeof(*StatePtr));

        /*
         * Fill out the remainder of meta data.
         * This data is currently the same for every request
         */
        StatePtr->FileWrite.FileSubType = CFE_FS_SubType_SB_MAPDATA;
        snprintf(StatePtr->FileWrite.Description, sizeof(StatePtr->FileWrite.Description), "SB Map Information");

        StatePtr->FileWrite.GetData = CFE_SB_WriteMsgMapInfoDataGetter;
        StatePtr->FileWrite.OnEvent = CFE_SB_BackgroundFileEventHandler;

        /*
        ** Copy the filename into local buffer with default name/path/extension if not specified
        */
        Status = CFE_FS_ParseInputFileNameEx(StatePtr->FileWrite.FileName, CmdPtr->Filename,
                                             sizeof(StatePtr->FileWrite.FileName), sizeof(CmdPtr->Filename),
                                             CFE_PLATFORM_SB_DEFAULT_MAP_FILENAME,
                                             CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                             CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

        if (Status == CFE_SUCCESS)
        {
            Status = CFE_FS_BackgroundFileDumpRequest(&StatePtr->FileWrite);
        }
    }
    else
    {
        Status = CFE_STATUS_REQUEST_ALREADY_PENDING;
    }

    if (Status != CFE_SUCCESS)
    {
        /* generate the same event as is generated when unable to create the file (same thing, really) */
        CFE_SB_BackgroundFileEventHandler(StatePtr, CFE_FS_FileWriteEvent_CREATE_ERROR, Status, 0, 0, 0);
    }

    CFE_SB_IncrCmdCtr(Status);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Callback for sending route subscriptions
 *
 *-----------------------------------------------------------------*/
void CFE_SB_SendRouteSub(CFE_SBR_RouteId_t RouteId, void *ArgPtr)
{
    CFE_SB_DestinationD_t *destptr;
    int32                  status;

    destptr = CFE_SBR_GetDestListHeadPtr(RouteId);

    /* Loop through destinations */
    while (destptr != NULL)
    {
        if (destptr->Scope == CFE_SB_MSG_GLOBAL)
        {
            /* ...add entry into pkt */
            CFE_SB_Global.PrevSubMsg.Payload.Entry[CFE_SB_Global.PrevSubMsg.Payload.Entries].MsgId =
                CFE_SBR_GetMsgId(RouteId);
            CFE_SB_Global.PrevSubMsg.Payload.Entry[CFE_SB_Global.PrevSubMsg.Payload.Entries].Qos.Priority    = 0;
            CFE_SB_Global.PrevSubMsg.Payload.Entry[CFE_SB_Global.PrevSubMsg.Payload.Entries].Qos.Reliability = 0;
            CFE_SB_Global.PrevSubMsg.Payload.Entries++;

            /* send pkt if full */
            if (CFE_SB_Global.PrevSubMsg.Payload.Entries >= CFE_SB_SUB_ENTRIES_PER_PKT)
            {
                CFE_SB_UnlockSharedData(__func__, __LINE__);
                status = CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_SB_Global.PrevSubMsg.TelemetryHeader), true);
                CFE_EVS_SendEvent(CFE_SB_FULL_SUB_PKT_EID, CFE_EVS_EventType_DEBUG,
                                  "Full Sub Pkt %d Sent,Entries=%d,Stat=0x%x\n",
                                  (int)CFE_SB_Global.PrevSubMsg.Payload.PktSegment,
                                  (int)CFE_SB_Global.PrevSubMsg.Payload.Entries, (unsigned int)status);
                CFE_SB_LockSharedData(__func__, __LINE__);
                CFE_SB_Global.PrevSubMsg.Payload.Entries = 0;
                CFE_SB_Global.PrevSubMsg.Payload.PktSegment++;
            }

            /*
             * break while loop through destinations, onto next route
             * This is done because we want only one network subscription per msgid
             * Later when Qos is used, we may want to take just the highest priority
             * subscription if there are more than one
             */
            break;
        }

        /* Advance to next destination */
        destptr = destptr->Next;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_SendPrevSubsCmd(const CFE_SB_SendPrevSubsCmd_t *data)
{
    int32 status;

    /* Take semaphore to ensure data does not change during this function */
    CFE_SB_LockSharedData(__func__, __LINE__);

    /* Initialize entry/segment tracking */
    CFE_SB_Global.PrevSubMsg.Payload.PktSegment = 1;
    CFE_SB_Global.PrevSubMsg.Payload.Entries    = 0;

    /* Send subscription for each route */
    CFE_SBR_ForEachRouteId(CFE_SB_SendRouteSub, NULL, NULL);

    CFE_SB_UnlockSharedData(__func__, __LINE__);

    /* if pkt has any number of entries, send it as a partial pkt */
    if (CFE_SB_Global.PrevSubMsg.Payload.Entries > 0)
    {
        status = CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_SB_Global.PrevSubMsg.TelemetryHeader), true);
        CFE_EVS_SendEvent(CFE_SB_PART_SUB_PKT_EID, CFE_EVS_EventType_DEBUG,
                          "Partial Sub Pkt %d Sent,Entries=%d,Stat=0x%x",
                          (int)CFE_SB_Global.PrevSubMsg.Payload.PktSegment,
                          (int)CFE_SB_Global.PrevSubMsg.Payload.Entries, (unsigned int)status);
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_IncrCmdCtr(int32 status)
{
    if (status == CFE_SUCCESS)
    {
        CFE_SB_Global.HKTlmMsg.Payload.CommandCounter++;
    }
    else
    {
        CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter++;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_SetSubscriptionReporting(uint32 state)
{
    CFE_SB_Global.SubscriptionReporting = state;
}
