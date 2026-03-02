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
#ifndef EVS_UT_HELPERS_H
#define EVS_UT_HELPERS_H

/*
** Includes
*/
#include "evs_UT.h"
#include "cfe_evs.h"
#include "utstubs.h"

typedef struct
{
    uint16 EventID;
    uint16 Count;
} UT_EVS_EventCapture_t;

/* MSG Init hook data */
typedef struct
{
    CFE_MSG_Message_t *MsgPtr;
    CFE_SB_MsgId_t     MsgId;
    CFE_MSG_Size_t     Size;
} UT_EVS_MSGInitData_t;

typedef CFE_Status_t (*UT_EVS_SendEventFunc_t)(uint32);

void         UT_InitData_EVS(void);
int32        UT_EVS_MSGInitHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);
void         UT_EVS_DoDispatchCheckEvents(void *MsgPtr, size_t MsgSize, UT_TaskPipeDispatchId_t DispatchId,
                                          UT_EVS_EventCapture_t *EventCapture);
void         UT_EVS_DoDispatchCheckEventsShort(void *MsgPtr, size_t MsgSize, UT_TaskPipeDispatchId_t DispatchId,
                                               UT_EVS_EventCapture_t *EventCapture);
void         UT_EVS_DoGenericCheckEvents(void (*Func)(void), UT_EVS_EventCapture_t *EventCapture);
CFE_Status_t UT_EVS_SendSquelchedEvent(uint32 EventId);
CFE_Status_t UT_EVS_SendSquelchedEventWithAppId(uint32 EventId);
CFE_Status_t UT_EVS_SendSquelchedTimedEvent(uint32 EventId);
void         UT_EVS_ResetSquelchCurrentContext(void);
void         UT_EVS_DisableSquelch(void);
void         UT_EVS_ResetSquelch(void);

extern UT_EVS_EventCapture_t UT_EVS_EventBuf;

extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_NOOP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_COUNTERS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_EVENT_TYPE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_EVENT_TYPE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_EVENT_FORMAT_MODE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_FILTER_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_WRITE_LOG_DATA_FILE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_CLEAR_LOG_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_SEND_HK;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_INVALID_MID;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_INVALID_CC;

#endif
