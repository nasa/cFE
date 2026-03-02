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
**    time_UT.c
**
** Purpose:
**    Time Services unit test
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
#include "time_ut_helpers.h"

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define TIME_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TIME_##intf##_MID)

#define TIME_UT_MSG_DISPATCH(intf, cmd)       TIME_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_TIME_##cmd)
#define TIME_UT_CC_DISPATCH(intf, cc, cmd)    TIME_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define TIME_UT_ERROR_DISPATCH(intf, cc, err) TIME_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_HK =
    { TIME_UT_MSG_DISPATCH(SEND_HK,SendHkCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_TONE_CMD =
    { TIME_UT_MSG_DISPATCH(TONE_CMD,ToneSignalCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_DATA_CMD =
    { TIME_UT_MSG_DISPATCH(DATA_CMD,ToneDataCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_ONEHZ_CMD =
    { TIME_UT_MSG_DISPATCH(ONEHZ_CMD,OneHzCmd) };
/* jphfix check CFE_PLATFORM_TIME_CFG_SERVER */
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_CMD =
    { TIME_UT_MSG_DISPATCH(SEND_CMD,FakeToneCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_NOOP_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_NOOP_CC, NoopCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_RESET_COUNTERS_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_RESET_COUNTERS_CC, ResetCountersCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SEND_DIAGNOSTIC_TLM_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SEND_DIAGNOSTIC_CC, SendDiagnosticCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STATE_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_STATE_CC, SetStateCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_SOURCE_CC, SetSourceCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_SIGNAL_CC, SetSignalCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_ADD_DELAY_CC, AddDelayCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_DELAY_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SUB_DELAY_CC, SubDelayCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_TIME_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_TIME_CC, SetTimeCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_MET_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_MET_CC, SetMETCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STCF_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_STCF_CC, SetSTCFCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_LEAP_SECONDS_CC, SetLeapSecondsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_ADD_ADJUST_CC, AddAdjustCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_ADJUST_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SUB_ADJUST_CC, SubAdjustCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_ONEHZ_ADJUSTMENT_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC, AddOneHzAdjustmentCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_ONEHZ_ADJUSTMENT_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC, SubOneHzAdjustmentCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_INVALID_CC =
    { TIME_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */
