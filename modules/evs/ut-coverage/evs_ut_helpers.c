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
#include "cfe_evs.h"
#include "evs_ut_helpers.h"
#include "utstubs.h"

static UT_SoftwareBusSnapshot_Entry_t UT_EVS_LONGFMT_SNAPSHOTDATA = {
    .MsgId          = CFE_SB_MSGID_WRAP_VALUE(0),
    .SnapshotOffset = offsetof(CFE_EVS_LongEventTlm_t, Payload.PacketID.EventID),
    .SnapshotSize   = sizeof(uint16)};

static UT_SoftwareBusSnapshot_Entry_t UT_EVS_SHORTFMT_SNAPSHOTDATA = {
    .MsgId          = CFE_SB_MSGID_WRAP_VALUE(0),
    .SnapshotOffset = offsetof(CFE_EVS_ShortEventTlm_t, Payload.PacketID.EventID),
    .SnapshotSize   = sizeof(uint16)};

UT_EVS_EventCapture_t UT_EVS_EventBuf;

/* Custom time handler to avoid needing to provide buffer for every event call */
void UT_CFE_MSG_GetMsgTime_CustomHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context) {}

/* Add custom logic to cFE common UT_InitData */
void UT_InitData_EVS(void)
{
    UT_InitData();

    UT_EVS_LONGFMT_SNAPSHOTDATA.MsgId  = CFE_SB_MSGID_C(CFE_EVS_LONG_EVENT_MSG_MID);
    UT_EVS_SHORTFMT_SNAPSHOTDATA.MsgId = CFE_SB_MSGID_C(CFE_EVS_SHORT_EVENT_MSG_MID);

    UT_SetHandlerFunction(UT_KEY(CFE_MSG_GetMsgTime), UT_CFE_MSG_GetMsgTime_CustomHandler, NULL);
}

/* Message init hook to stora last MsgId passed in */
int32 UT_EVS_MSGInitHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    UT_EVS_MSGInitData_t *msgdataptr = UserObj;

    msgdataptr->MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
    msgdataptr->MsgId  = UT_Hook_GetArgValueByName(Context, "MsgId", CFE_SB_MsgId_t);
    msgdataptr->Size   = UT_Hook_GetArgValueByName(Context, "Size", CFE_MSG_Size_t);

    return StubRetcode;
}

static void UT_EVS_DoDispatchCheckEvents_Impl(void *MsgPtr, size_t MsgSize, UT_TaskPipeDispatchId_t DispatchId,
                                              const UT_SoftwareBusSnapshot_Entry_t *SnapshotCfg,
                                              UT_EVS_EventCapture_t *               EventCapture)
{
    UT_SoftwareBusSnapshot_Entry_t SnapshotData = *SnapshotCfg;

    EventCapture->EventID       = 0xFFFF;
    SnapshotData.SnapshotBuffer = &EventCapture->EventID;

    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &SnapshotData);
    UT_CallTaskPipe(CFE_EVS_ProcessCommandPacket, (CFE_MSG_Message_t *)MsgPtr, MsgSize, DispatchId);
    EventCapture->Count += SnapshotData.Count;

    /* be sure to clear the hook function since the SnapshotData is going out of scope */
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), NULL, NULL);
}

void UT_EVS_DoDispatchCheckEvents(void *MsgPtr, size_t MsgSize, UT_TaskPipeDispatchId_t DispatchId,
                                  UT_EVS_EventCapture_t *EventCapture)
{
    UT_EVS_DoDispatchCheckEvents_Impl(MsgPtr, MsgSize, DispatchId, &UT_EVS_LONGFMT_SNAPSHOTDATA, EventCapture);
}

void UT_EVS_DoDispatchCheckEventsShort(void *MsgPtr, size_t MsgSize, UT_TaskPipeDispatchId_t DispatchId,
                                       UT_EVS_EventCapture_t *EventCapture)
{
    UT_EVS_DoDispatchCheckEvents_Impl(MsgPtr, MsgSize, DispatchId, &UT_EVS_SHORTFMT_SNAPSHOTDATA, EventCapture);
}

void UT_EVS_DoGenericCheckEvents(void (*Func)(void), UT_EVS_EventCapture_t *EventCapture)
{
    UT_SoftwareBusSnapshot_Entry_t SnapshotData = UT_EVS_LONGFMT_SNAPSHOTDATA;

    EventCapture->EventID       = -1;
    SnapshotData.SnapshotBuffer = &EventCapture->EventID;

    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &SnapshotData);
    Func();
    EventCapture->Count += SnapshotData.Count;

    /* be sure to clear the hook function since the SnapshotData is going out of scope */
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), NULL, NULL);
}

CFE_Status_t UT_EVS_SendSquelchedEvent(uint32 EventId)
{
    return CFE_EVS_SendEvent(EventId, CFE_EVS_EventType_INFORMATION, "Suppressed Message");
}

CFE_Status_t UT_EVS_SendSquelchedEventWithAppId(uint32 EventId)
{
    CFE_ES_AppId_t AppID;
    CFE_ES_GetAppID(&AppID);
    return CFE_EVS_SendEventWithAppID(EventId, CFE_EVS_EventType_INFORMATION, AppID, "Suppressed Message");
}

CFE_Status_t UT_EVS_SendSquelchedTimedEvent(uint32 EventId)
{
    CFE_TIME_SysTime_t Time = {0, 0};
    return CFE_EVS_SendTimedEvent(Time, EventId, CFE_EVS_EventType_INFORMATION, "Suppressed Message");
}

void UT_EVS_ResetSquelchCurrentContext(void)
{
    EVS_AppData_t *AppDataPtr;

    EVS_GetCurrentContext(&AppDataPtr, NULL);
    if (AppDataPtr)
    {
        AppDataPtr->SquelchedCount            = 0;
        AppDataPtr->SquelchTokens             = CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST * 1000;
        AppDataPtr->LastSquelchCreditableTime = OS_TimeAssembleFromMilliseconds(0, 0);
    }
}

void UT_EVS_DisableSquelch(void)
{
    CFE_EVS_Global.EVS_EventBurstMax = 0;
}

void UT_EVS_ResetSquelch(void)
{
    CFE_EVS_Global.EVS_EventBurstMax = CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST;
}
