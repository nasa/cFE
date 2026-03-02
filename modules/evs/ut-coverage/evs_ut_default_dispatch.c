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
**    evs_UT.c
**
** Purpose:
**    Event Services unit test
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
#include "evs_UT.h"
#include "cfe_evs_msg.h"
#include "utstubs.h"

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define EVS_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_##intf##_MID)

#define EVS_UT_MSG_DISPATCH(intf, cmd)       EVS_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_EVS_##cmd)
#define EVS_UT_CC_DISPATCH(intf, cc, cmd)    EVS_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define EVS_UT_ERROR_DISPATCH(intf, cc, err) EVS_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_NOOP_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_NOOP_CC, NoopCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_COUNTERS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_COUNTERS_CC, ResetCountersCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_EVENT_TYPE_CC, EnableEventTypeCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_EVENT_TYPE_CC, DisableEventTypeCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_EVENT_FORMAT_MODE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_SET_EVENT_FORMAT_MODE_CC, SetEventFormatModeCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, EnableAppEventTypeCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, DisableAppEventTypeCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_APP_EVENTS_CC, EnableAppEventsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_APP_EVENTS_CC, DisableAppEventsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_APP_COUNTER_CC, ResetAppCounterCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_SET_FILTER_CC, SetFilterCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_PORTS_CC, EnablePortsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_PORTS_CC, DisablePortsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_FILTER_CC, ResetFilterCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_ALL_FILTERS_CC, ResetAllFiltersCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ADD_EVENT_FILTER_CC, AddEventFilterCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DELETE_EVENT_FILTER_CC, DeleteEventFilterCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_WRITE_APP_DATA_FILE_CC, WriteAppDataFileCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_WRITE_LOG_DATA_FILE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_WRITE_LOG_DATA_FILE_CC, WriteLogDataFileCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_SET_LOG_MODE_CC, SetLogModeCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_CLEAR_LOG_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_CLEAR_LOG_CC, ClearLogCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_SEND_HK =
    { EVS_UT_MSG_DISPATCH(SEND_HK, SendHkCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_INVALID_CC =
    { EVS_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */
