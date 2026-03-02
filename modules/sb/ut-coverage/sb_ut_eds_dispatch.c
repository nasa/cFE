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
** File:
**    sb_UT.c
**
** Purpose:
**    Software Bus Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/

/*
** Includes
*/
#include "sb_ut_helpers.h"

#include "edslib_datatypedb.h"
#include "cfe_missionlib_runtime.h"

/* This is a generated header that defines the dispatch table */
#include "cfe_sb_eds_interface.h"

/* EDS dispatching uses a generic function based on a lookup table.
 * This function is a stub so that stub just needs to know which entry to use. */
#define SB_UT_EDS_DISPATCH(intf, cmd)                      \
    .Method      = UT_TaskPipeDispatchMethod_TABLE_OFFSET, \
    .TableOffset = offsetof(EdsDispatchTable_EdsComponent_CFE_SB_Application_CFE_SB_Telecommand_t, intf.cmd)

#define SB_UT_MSG_DISPATCH(intf, cmd) SB_UT_EDS_DISPATCH(intf, indication), UT_TPD_SETSIZE(CFE_SB_##cmd)
#define SB_UT_CC_DISPATCH(intf, cc, cmd) \
    SB_UT_EDS_DISPATCH(intf, cmd##_indication), UT_TPD_SETSIZE(CFE_SB_##cmd), UT_TPD_SETCC(cc)
#define SB_UT_ERROR_DISPATCH(intf, cc, err) UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_NOOP_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_NOOP_CC,NoopCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_RESET_COUNTERS_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_RESET_COUNTERS_CC,ResetCountersCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_SEND_SB_STATS_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_SEND_SB_STATS_CC,SendSbStatsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_ROUTING_INFO_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_WRITE_ROUTING_INFO_CC,WriteRoutingInfoCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_PIPE_INFO_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_WRITE_PIPE_INFO_CC,WritePipeInfoCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_MAP_INFO_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_WRITE_MAP_INFO_CC,WriteMapInfoCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_ENABLE_ROUTE_CC,EnableRouteCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_DISABLE_ROUTE_CC,DisableRouteCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SEND_HK =
    { SB_UT_MSG_DISPATCH(SEND_HK,SendHkCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_SEND_PREV_SUBS_CC =
    { SB_UT_CC_DISPATCH(SUB_RPT_CTRL,CFE_SB_SEND_PREV_SUBS_CC,SendPrevSubsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_ENABLE_SUB_REPORTING_CC =
    { SB_UT_CC_DISPATCH(SUB_RPT_CTRL,CFE_SB_ENABLE_SUB_REPORTING_CC,DisableSubReportingCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_DISABLE_SUB_REPORTING_CC =
    { SB_UT_CC_DISPATCH(SUB_RPT_CTRL,CFE_SB_DISABLE_SUB_REPORTING_CC,EnableSubReportingCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_BAD_FCNCODE =
    { SB_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTRL_BAD_FCNCODE =
    { SB_UT_ERROR_DISPATCH(SUB_RPT_CTRL, -1, CFE_STATUS_BAD_COMMAND_CODE) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_BAD_MSGID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
/* clang-format on */

void UT_SB_Handler_MapPublisherComponent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_SoftwareBus_PubSub_Interface_t *Output =
        UT_Hook_GetArgValueByName(Context, "Output", CFE_SB_SoftwareBus_PubSub_Interface_t *);
    const CFE_SB_Publisher_Component_t *Input =
        UT_Hook_GetArgValueByName(Context, "Input", const CFE_SB_Publisher_Component_t *);
    int32 StatusCode;

    if (!UT_Stub_GetInt32StatusCode(Context, &StatusCode) && Input != NULL)
    {
        StatusCode = 0x800 | Input->Telemetry.TopicId;
    }

    if (Output != NULL)
    {
        Output->MsgId = CFE_SB_MSGID_C(StatusCode);
    }
}

void UT_SB_Handler_MapListenerComponent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_SoftwareBus_PubSub_Interface_t *Output =
        UT_Hook_GetArgValueByName(Context, "Output", CFE_SB_SoftwareBus_PubSub_Interface_t *);
    const CFE_SB_Listener_Component_t *Input =
        UT_Hook_GetArgValueByName(Context, "Input", const CFE_SB_Listener_Component_t *);
    int32 StatusCode;

    if (!UT_Stub_GetInt32StatusCode(Context, &StatusCode) && Input != NULL)
    {
        StatusCode = 0x1800 | Input->Telecommand.TopicId;
    }

    if (Output != NULL)
    {
        Output->MsgId = CFE_SB_MSGID_C(StatusCode);
    }
}

void UT_SB_Handler_UnmapPublisherComponent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_Publisher_Component_t *Output = UT_Hook_GetArgValueByName(Context, "Output", CFE_SB_Publisher_Component_t *);
    const CFE_SB_SoftwareBus_PubSub_Interface_t *Input =
        UT_Hook_GetArgValueByName(Context, "Input", const CFE_SB_SoftwareBus_PubSub_Interface_t *);
    int32 StatusCode;

    if (!UT_Stub_GetInt32StatusCode(Context, &StatusCode) && Input != NULL)
    {
        StatusCode = Input->MsgId.Value & 0xFF;
    }

    if (Output != NULL)
    {
        Output->Telemetry.TopicId        = StatusCode & 0xFFFF;
        Output->Telemetry.InstanceNumber = 1 + (StatusCode >> 16);
    }
}

void UT_SB_Handler_UnmapListenerComponent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_Listener_Component_t *Output = UT_Hook_GetArgValueByName(Context, "Output", CFE_SB_Listener_Component_t *);
    const CFE_SB_SoftwareBus_PubSub_Interface_t *Input =
        UT_Hook_GetArgValueByName(Context, "Input", const CFE_SB_SoftwareBus_PubSub_Interface_t *);
    int32 StatusCode;

    if (!UT_Stub_GetInt32StatusCode(Context, &StatusCode) && Input != NULL)
    {
        StatusCode = Input->MsgId.Value & 0xFF;
    }

    if (Output != NULL)
    {
        Output->Telecommand.TopicId        = StatusCode & 0xFFFF;
        Output->Telecommand.InstanceNumber = 1 + (StatusCode >> 16);
    }
}

void UT_SB_Handler_Get_PubSub_Parameters(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    EdsInterface_CFE_SB_SoftwareBus_PubSub_t *Params =
        UT_Hook_GetArgValueByName(Context, "Params", EdsInterface_CFE_SB_SoftwareBus_PubSub_t *);
    const EdsDataType_CFE_HDR_Message_t *Packet =
        UT_Hook_GetArgValueByName(Context, "Packet", const EdsDataType_CFE_HDR_Message_t *);
    int32 StatusCode;

    if (!UT_Stub_GetInt32StatusCode(Context, &StatusCode) && Packet != NULL)
    {
        StatusCode = Packet->CCSDS.CommonHdr.AppId;
    }

    if (Params != NULL)
    {
        Params->MsgId.Value = StatusCode;
    }
}

void UT_SB_Handler_EdsLib_DataTypeDB_GetMemberByIndex(void *UserObj, UT_EntryKey_t FuncKey,
                                                      const UT_StubContext_t *Context)
{
    EdsLib_DataTypeDB_EntityInfo_t *MemberInfo =
        UT_Hook_GetArgValueByName(Context, "MemberInfo", EdsLib_DataTypeDB_EntityInfo_t *);

    if (MemberInfo != NULL)
    {
        if (UserObj != NULL)
        {
            memcpy(MemberInfo, UserObj, sizeof(*MemberInfo));
        }
        else
        {
            memset(MemberInfo, 0, sizeof(*MemberInfo));
        }
    }
}

void SB_UT_BindDispatchHandlers(void)
{
    UT_SetHandlerFunction(UT_KEY(CFE_MissionLib_MapPublisherComponent), UT_SB_Handler_MapPublisherComponent, NULL);
    UT_SetHandlerFunction(UT_KEY(CFE_MissionLib_MapListenerComponent), UT_SB_Handler_MapListenerComponent, NULL);
}

void UT_SB_Setup_MsgHdrSize(bool HasSec, CFE_MSG_Type_t MsgType, CFE_MSG_Size_t TotalSize, size_t ExpectedPayloadOffset)
{
    static EdsLib_DataTypeDB_EntityInfo_t MemberInfo;

    UT_ResetState(UT_KEY(CFE_MSG_GetHasSecondaryHeader));
    UT_ResetState(UT_KEY(CFE_MSG_GetType));
    UT_ResetState(UT_KEY(CFE_MSG_GetSize));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &HasSec, sizeof(HasSec), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &MsgType, sizeof(MsgType), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &TotalSize, sizeof(TotalSize), true);

    UT_SetHandlerFunction(UT_KEY(CFE_MissionLib_Get_PubSub_Parameters), UT_SB_Handler_Get_PubSub_Parameters, NULL);

    /* Only one of these will be needed, but just register both to keep it simple here */
    UT_SetHandlerFunction(UT_KEY(CFE_MissionLib_UnmapPublisherComponent), UT_SB_Handler_UnmapPublisherComponent, NULL);
    UT_SetHandlerFunction(UT_KEY(CFE_MissionLib_UnmapListenerComponent), UT_SB_Handler_UnmapListenerComponent, NULL);

    /* These must output a nonzero topic id, whichever is used */
    UT_SetDefaultReturnValue(UT_KEY(CFE_MissionLib_UnmapPublisherComponent), 1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_MissionLib_UnmapListenerComponent), 1);

    memset(&MemberInfo, 0, sizeof(MemberInfo));
    MemberInfo.Offset.Bytes  = ExpectedPayloadOffset;
    MemberInfo.MaxSize.Bytes = TotalSize - ExpectedPayloadOffset;

    UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetMemberByIndex), UT_SB_Handler_EdsLib_DataTypeDB_GetMemberByIndex,
                          &MemberInfo);
}
