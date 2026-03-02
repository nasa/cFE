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

#ifndef SB_UT_HELPERS_H
#define SB_UT_HELPERS_H

/*
** Includes
*/
#include "sb_UT.h"
#include "cfe_msg.h"
#include "cfe_core_resourceid_basevalues.h"

/*
 * MSG ID constants for unit testing:
 * Unit test cases should not directly use integer MsgId values
 *
 * The following constants are of the CFE_SB_MsgId_t type
 */

extern const CFE_SB_MsgId_t SB_UT_CMD_MID;
extern const CFE_SB_MsgId_t SB_UT_TLM_MID;

extern const CFE_SB_MsgId_t SB_UT_CMD_MID1;
extern const CFE_SB_MsgId_t SB_UT_CMD_MID2;
extern const CFE_SB_MsgId_t SB_UT_CMD_MID3;
extern const CFE_SB_MsgId_t SB_UT_CMD_MID4;
extern const CFE_SB_MsgId_t SB_UT_CMD_MID5;
extern const CFE_SB_MsgId_t SB_UT_CMD_MID6;

extern const CFE_SB_MsgId_t SB_UT_TLM_MID1;
extern const CFE_SB_MsgId_t SB_UT_TLM_MID2;
extern const CFE_SB_MsgId_t SB_UT_TLM_MID3;
extern const CFE_SB_MsgId_t SB_UT_TLM_MID4;
extern const CFE_SB_MsgId_t SB_UT_TLM_MID5;
extern const CFE_SB_MsgId_t SB_UT_TLM_MID6;

extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_NOOP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_RESET_COUNTERS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_SEND_SB_STATS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_ROUTING_INFO_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_PIPE_INFO_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_MAP_INFO_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SEND_HK;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_SEND_PREV_SUBS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_ENABLE_SUB_REPORTING_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_DISABLE_SUB_REPORTING_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_BAD_FCNCODE;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTRL_BAD_FCNCODE;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_BAD_MSGID;

/*
 * A MsgId value which still qualifies as valid
 *
 * This is a  "borderline" value to test the limits of the validity checking
 * The specific value depends on how MsgId is actually defined internally
 */
extern const CFE_SB_MsgId_t SB_UT_LAST_VALID_MID;

/*
 * A MsgId value which still qualifies as valid
 *
 * This is a  "borderline" value to test the limits of the validity checking
 * The specific value depends on how MsgId is actually defined internally
 */
extern const CFE_SB_MsgId_t SB_UT_FIRST_VALID_MID;

/*
 * A MsgId value which is in the middle of the valid range
 *
 * The specific value depends on how MsgId is actually defined internally
 */
extern const CFE_SB_MsgId_t SB_UT_INTERMEDIATE_VALID_MID;

/*
 * A MsgId value which is not valid but also not equal to CFE_SB_INVALID_MSG_ID
 * Like CFE_SB_INVALID_MSG_ID, this should also _not_ pass the validity check.
 */
extern const CFE_SB_MsgId_t SB_UT_ALTERNATE_INVALID_MID;

/*
 * A MsgId value which is valid per CCSDS but does not have the secondary header bit set
 */
extern const CFE_SB_MsgId_t SB_UT_BARE_CMD_MID3;
extern const CFE_SB_MsgId_t SB_UT_BARE_TLM_MID3;

#define SB_UT_PIPEID_0                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 0))
#define SB_UT_PIPEID_1                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 1))
#define SB_UT_PIPEID_2                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 2))
#define SB_UT_PIPEID_3                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 3))
#define SB_UT_ALTERNATE_INVALID_PIPEID CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(0xDEADBEEF))

/*
 * Helper function to manufacture a fake pipe ID value that will validate
 */
CFE_ResourceId_t UT_SB_MakePipeIdForIndex(uint32 ArrayIdx);
CFE_ES_AppId_t   UT_SB_AppID_Modify(CFE_ES_AppId_t InitialID, int32 Modifier);

void SB_UT_BindDispatchHandlers(void);
void SB_ResetUnitTest(void);

CFE_Status_t SB_UT_OriginationActionHook(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                         const UT_StubContext_t *Context);
void         SB_UT_OriginationActionHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
CFE_Status_t SB_UT_RecieveBuffer_FalseEndpoint(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut);

void UT_SB_Setup_MsgHdrSize(bool HasSec, CFE_MSG_Type_t MsgType, CFE_MSG_Size_t TotalSize, size_t ExpectedPayloadOffset);

#endif /* SB_UT_HELPERS_H */
