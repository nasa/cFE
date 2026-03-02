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

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define SB_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_SB_##intf##_MID)

#define SB_UT_MSG_DISPATCH(intf, cmd)       SB_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_SB_##cmd)
#define SB_UT_CC_DISPATCH(intf, cc, cmd)    SB_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define SB_UT_ERROR_DISPATCH(intf, cc, err) SB_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

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

void SB_UT_BindDispatchHandlers(void)
{
    /* nothing to do here in default dispatch mode */
}

void UT_SB_Setup_MsgHdrSize(bool HasSec, CFE_MSG_Type_t MsgType, CFE_MSG_Size_t TotalSize, size_t ExpectedPayloadOffset)
{
    UT_ResetState(UT_KEY(CFE_MSG_GetHasSecondaryHeader));
    UT_ResetState(UT_KEY(CFE_MSG_GetType));
    UT_ResetState(UT_KEY(CFE_MSG_GetSize));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &HasSec, sizeof(HasSec), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &MsgType, sizeof(MsgType), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &TotalSize, sizeof(TotalSize), true);
}
