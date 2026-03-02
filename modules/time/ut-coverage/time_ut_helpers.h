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

#ifndef TIME_UT_HELPERS_H
#define TIME_UT_HELPERS_H

#include "time_UT.h"
#include "utstubs.h"

extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_HK;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_TONE_CMD;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_DATA_CMD;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_ONEHZ_CMD;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_CMD; /* jphfix - check refs */
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_NOOP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_RESET_COUNTERS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SEND_DIAGNOSTIC_TLM_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STATE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_DELAY_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_TIME_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_MET_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STCF_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_ADJUST_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_ONEHZ_ADJUSTMENT_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_ONEHZ_ADJUSTMENT_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_INVALID_MID;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_INVALID_CC;

/*
 * A hook function for CFE_PSP_GetTime that updates the Reference State.
 * This mimics what would happen if a time update occurred at the moment
 * another task was reading the time.
 */
int32 UT_TimeRefUpdateHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);

#endif /* TIME_UT_HELPERS_H */
