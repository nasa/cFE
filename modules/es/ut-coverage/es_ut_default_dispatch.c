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
**    es_UT.c
**
** Purpose:
**    Executive Services unit test
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
#include "es_UT.h"
#include "target_config.h"
#include "cfe_config.h"
#include <setjmp.h>

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define ES_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_##intf##_MID)

#define ES_UT_MSG_DISPATCH(intf, cmd)       ES_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_ES_##cmd)
#define ES_UT_CC_DISPATCH(intf, cc, cmd)    ES_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define ES_UT_ERROR_DISPATCH(intf, cc, err) ES_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_NOOP_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_NOOP_CC, NoopCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_RESET_COUNTERS_CC, ResetCountersCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESTART_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_RESTART_CC, RestartCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_START_APP_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_START_APP_CC, StartAppCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_STOP_APP_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_STOP_APP_CC, StopAppCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESTART_APP_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_RESTART_APP_CC, RestartAppCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RELOAD_APP_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_RELOAD_APP_CC, ReloadAppCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ONE_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_QUERY_ONE_CC, QueryOneCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ALL_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_QUERY_ALL_CC, QueryAllCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_QUERY_ALL_TASKS_CC, QueryAllTasksCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_CLEAR_SYS_LOG_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_CLEAR_SYS_LOG_CC, ClearSysLogCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_WRITE_SYS_LOG_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_WRITE_SYS_LOG_CC, WriteSysLogCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_OVER_WRITE_SYS_LOG_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_OVER_WRITE_SYS_LOG_CC, OverWriteSysLogCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_CLEAR_ER_LOG_CC, ClearERLogCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_WRITE_ER_LOG_CC, WriteERLogCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_START_PERF_DATA_CC, StartPerfDataCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_STOP_PERF_DATA_CC, StopPerfDataCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_SET_PERF_FILTER_MASK_CC, SetPerfFilterMaskCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_SET_PERF_TRIGGER_MASK_CC, SetPerfTriggerMaskCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_RESET_PR_COUNT_CC, ResetPRCountCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_SET_MAX_PR_COUNT_CC, SetMaxPRCountCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_DELETE_CDS_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_DELETE_CDS_CC, DeleteCDSCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_SEND_MEM_POOL_STATS_CC, SendMemPoolStatsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC =
    { ES_UT_CC_DISPATCH(CMD, CFE_ES_DUMP_CDS_REGISTRY_CC, DumpCDSRegistryCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_SEND_HK =
    { ES_UT_MSG_DISPATCH(SEND_HK, SendHkCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_INVALID_LENGTH =
    { ES_UT_ERROR_DISPATCH(CMD, 0, CFE_STATUS_WRONG_MSG_LENGTH) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_INVALID_CC =
    { ES_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */
