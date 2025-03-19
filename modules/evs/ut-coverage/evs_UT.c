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
#include "cfe_evs.h"
#include "utstubs.h"

static const char *EVS_SYSLOG_MSGS[] = {
    NULL,
    "%s: Call to CFE_PSP_GetResetArea failed, RC=0x%08x\n",
    "%s: Unexpected size from CFE_PSP_GetResetArea: expected = 0x%08lX, actual = 0x%08lX\n",
    "%s: OS_MutSemCreate failed, RC=%ld\n",
    "%s: Event Log cleared following power-on reset\n",
    "%s: Event Log cleared, n=%d, c=%d, f=%d, m=%d, o=%d\n",
    "%s: Event Log restored, n=%d, c=%d, f=%d, m=%d, o=%d\n",
    "%s: Application Init Failed,RC=0x%08X\n",
    "%s: Error reading cmd pipe,RC=0x%08X\n",
    NULL, /* old message removed - placeholder to maintain indices */
    "%s: Call to CFE_ES_GetAppID Failed:RC=0x%08X\n",
    "%s: Call to CFE_EVS_Register Failed:RC=0x%08X\n",
    "%s: Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n",
    "%s: Subscribing to Cmds Failed:RC=0x%08X\n",
    "%s: Subscribing to HK Request Failed:RC=0x%08X\n"};

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define EVS_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_##intf##_MID)

#define EVS_UT_MSG_DISPATCH(intf, cmd)       EVS_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_EVS_##cmd)
#define EVS_UT_CC_DISPATCH(intf, cc, cmd)    EVS_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define EVS_UT_ERROR_DISPATCH(intf, cc, err) EVS_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_NOOP_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_NOOP_CC, NoopCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_COUNTERS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_COUNTERS_CC, ResetCountersCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_EVENT_TYPE_CC, EnableEventTypeCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_EVENT_TYPE_CC, DisableEventTypeCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_EVENT_FORMAT_MODE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_SET_EVENT_FORMAT_MODE_CC, SetEventFormatModeCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_APP_EVENT_TYPE_CC, EnableAppEventTypeCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, DisableAppEventTypeCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_APP_EVENTS_CC, EnableAppEventsCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_APP_EVENTS_CC, DisableAppEventsCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_APP_COUNTER_CC, ResetAppCounterCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_SET_FILTER_CC, SetFilterCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ENABLE_PORTS_CC, EnablePortsCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DISABLE_PORTS_CC, DisablePortsCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_FILTER_CC, ResetFilterCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_RESET_ALL_FILTERS_CC, ResetAllFiltersCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_ADD_EVENT_FILTER_CC, AddEventFilterCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_DELETE_EVENT_FILTER_CC, DeleteEventFilterCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_WRITE_APP_DATA_FILE_CC, WriteAppDataFileCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_WRITE_LOG_DATA_FILE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_WRITE_LOG_DATA_FILE_CC, WriteLogDataFileCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_SET_LOG_MODE_CC, SetLogModeCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_CLEAR_LOG_CC =
    { EVS_UT_CC_DISPATCH(CMD, CFE_EVS_CLEAR_LOG_CC, ClearLogCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_SEND_HK =
    { EVS_UT_MSG_DISPATCH(SEND_HK, SendHkCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_EVS_CMD_INVALID_CC =
    { EVS_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */

static UT_SoftwareBusSnapshot_Entry_t UT_EVS_LONGFMT_SNAPSHOTDATA = {
    .MsgId          = CFE_SB_MSGID_WRAP_VALUE(0),
    .SnapshotOffset = offsetof(CFE_EVS_LongEventTlm_t, Payload.PacketID.EventID),
    .SnapshotSize   = sizeof(uint16)};

static UT_SoftwareBusSnapshot_Entry_t UT_EVS_SHORTFMT_SNAPSHOTDATA = {
    .MsgId          = CFE_SB_MSGID_WRAP_VALUE(0),
    .SnapshotOffset = offsetof(CFE_EVS_ShortEventTlm_t, Payload.PacketID.EventID),
    .SnapshotSize   = sizeof(uint16)};

typedef struct
{
    uint16 EventID;
    uint16 Count;
} UT_EVS_EventCapture_t;

static UT_EVS_EventCapture_t UT_EVS_EventBuf;

/* MSG Init hook data */
typedef struct
{
    CFE_MSG_Message_t *MsgPtr;
    CFE_SB_MsgId_t     MsgId;
    CFE_MSG_Size_t     Size;
} UT_EVS_MSGInitData_t;

typedef CFE_Status_t (*UT_EVS_SendEventFunc_t)(uint32);

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
static int32 UT_EVS_MSGInitHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
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

static void UT_EVS_DoDispatchCheckEvents(void *MsgPtr, size_t MsgSize, UT_TaskPipeDispatchId_t DispatchId,
                                         UT_EVS_EventCapture_t *EventCapture)
{
    UT_EVS_DoDispatchCheckEvents_Impl(MsgPtr, MsgSize, DispatchId, &UT_EVS_LONGFMT_SNAPSHOTDATA, EventCapture);
}

static void UT_EVS_DoDispatchCheckEventsShort(void *MsgPtr, size_t MsgSize, UT_TaskPipeDispatchId_t DispatchId,
                                              UT_EVS_EventCapture_t *EventCapture)
{
    UT_EVS_DoDispatchCheckEvents_Impl(MsgPtr, MsgSize, DispatchId, &UT_EVS_SHORTFMT_SNAPSHOTDATA, EventCapture);
}

static void UT_EVS_DoGenericCheckEvents(void (*Func)(void), UT_EVS_EventCapture_t *EventCapture)
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

static CFE_Status_t UT_EVS_SendSquelchedEvent(uint32 EventId)
{
    return CFE_EVS_SendEvent(EventId, CFE_EVS_EventType_INFORMATION, "Suppressed Message");
}

static CFE_Status_t UT_EVS_SendSquelchedEventWithAppId(uint32 EventId)
{
    CFE_ES_AppId_t AppID;
    CFE_ES_GetAppID(&AppID);
    return CFE_EVS_SendEventWithAppID(EventId, CFE_EVS_EventType_INFORMATION, AppID, "Suppressed Message");
}

static CFE_Status_t UT_EVS_SendSquelchedTimedEvent(uint32 EventId)
{
    CFE_TIME_SysTime_t Time = {0, 0};
    return CFE_EVS_SendTimedEvent(Time, EventId, CFE_EVS_EventType_INFORMATION, "Suppressed Message");
}

static void UT_EVS_ResetSquelchCurrentContext(void)
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

static void UT_EVS_DisableSquelch(void)
{
    CFE_EVS_Global.EVS_EventBurstMax = 0;
}

static void UT_EVS_ResetSquelch(void)
{
    CFE_EVS_Global.EVS_EventBurstMax = CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST;
}

/*
** Functions
*/
void UtTest_Setup(void)
{
    /* Initialize unit test */
    UT_Init("evs");
    UtPrintf("cFE EVS Unit Test Output File\n\n");

    /* Test_Init is a test but also MUST be called first and only once */
    UT_ADD_TEST(Test_Init);
    UT_ADD_TEST(Test_IllegalAppID);
    UT_ADD_TEST(Test_UnregisteredApp);
    UT_ADD_TEST(Test_FilterRegistration);
    UT_ADD_TEST(Test_FilterReset);
    UT_ADD_TEST(Test_Format);
    UT_ADD_TEST(Test_Ports);
    UT_ADD_TEST(Test_Logging);
    UT_ADD_TEST(Test_WriteApp);
    UT_ADD_TEST(Test_BadAppCmd);
    UT_ADD_TEST(Test_EventCmd);
    UT_ADD_TEST(Test_FilterCmd);
    UT_ADD_TEST(Test_InvalidCmd);
    UT_ADD_TEST(Test_Squelching);
    UT_ADD_TEST(Test_Misc);
}

/*
** Initialization test, must be called first
*/
void Test_Init(void)
{
    CFE_EVS_EnablePortsCmd_t        bitmaskcmd;
    CFE_EVS_EnableAppEventTypeCmd_t appbitcmd;

    CFE_SB_Buffer_t *bufPtr;
    CFE_EVS_NoopCmd_t testMsg;

    UtPrintf("Begin Test Init");

    memset(&bitmaskcmd, 0, sizeof(bitmaskcmd));
    memset(&appbitcmd, 0, sizeof(appbitcmd));

    strncpy(appbitcmd.Payload.AppName, "ut_cfe_evs", sizeof(appbitcmd.Payload.AppName) - 1);
    appbitcmd.Payload.AppName[sizeof(appbitcmd.Payload.AppName) - 1] = '\0';

    /* Test successful early initialization of the cFE EVS */
    UT_InitData_EVS();
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, CFE_PSP_RST_TYPE_POWERON);
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[4]);

    /* Task main with init failure */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, -1);
    UtAssert_VOIDCALL(CFE_EVS_TaskMain());

    /* Test TaskMain with a command pipe read failure due to an
     * invalid command packet
     */
    UtPrintf("CFE_EVS_TaskMain - Test error reading command pipe, unrecognized msgid");
    UT_InitData_EVS();

    /* Set unexpected message ID */
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_EVS_INVALID_MID, 0, true);

    bufPtr = (CFE_SB_Buffer_t *)&testMsg; /* Fake Test Message */
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), &bufPtr, sizeof(bufPtr), false);

    UT_EVS_DoGenericCheckEvents(CFE_EVS_TaskMain, &UT_EVS_EventBuf);
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[8]);
    UtAssert_INT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_MSGID_EID);

    /* Test early initialization with a get reset area failure */
    UT_InitData_EVS();
    UT_SetStatusBSPResetArea(-1, CFE_TIME_RESET_SIGNATURE, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_EVS_EarlyInit();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[1]);

    /* Test early initialization, restoring the event log */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, -1);
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[6]);

    /* Test early initialization, clearing the event log (log mode path) */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, -1);
    CFE_EVS_Global.EVS_LogPtr->LogMode     = CFE_EVS_LogMode_OVERWRITE + CFE_EVS_LogMode_DISCARD + 1;
    CFE_EVS_Global.EVS_LogPtr->LogFullFlag = false;
    CFE_EVS_Global.EVS_LogPtr->Next        = CFE_PLATFORM_EVS_LOG_MAX - 1;
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[5]);

    /* Test early initialization, clearing the event log (log full path) */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, -1);
    CFE_EVS_Global.EVS_LogPtr->LogMode     = CFE_EVS_LogMode_DISCARD;
    CFE_EVS_Global.EVS_LogPtr->LogFullFlag = 2;
    CFE_EVS_Global.EVS_LogPtr->Next        = CFE_PLATFORM_EVS_LOG_MAX - 1;
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[5]);

    /* Test early initialization, clearing the event log (next log path) */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, -1);
    CFE_EVS_Global.EVS_LogPtr->LogMode     = CFE_EVS_LogMode_OVERWRITE;
    CFE_EVS_Global.EVS_LogPtr->LogFullFlag = true;
    CFE_EVS_Global.EVS_LogPtr->Next        = CFE_PLATFORM_EVS_LOG_MAX;
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[5]);

    /* Test early initialization with a mutex creation failure */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, -1);
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[3]);

    /* Test early initialization with an unexpected size returned
     * by CFE_PSP_GetResetArea
     */
    UT_InitData_EVS();
    UT_SetSizeofESResetArea(0);
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[2]);

    /* Repeat successful initialization to configure log for later references */
    UT_InitData_EVS();
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, CFE_PSP_RST_TYPE_POWERON);
    CFE_EVS_EarlyInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[4]);

    /* Test task initialization where event services fails */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 2, -1); /* Set Failure in CFE_EVS_Register -> EVS_GetApp_ID */
    CFE_EVS_TaskInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[11]);

    /* Test task initialization where the pipe creation fails */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -1);
    CFE_EVS_TaskInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[12]);

    /* Test task initialization where command subscription fails */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, -1);
    CFE_EVS_TaskInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[13]);

    /* Test task initialization where HK request subscription fails */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -1);
    CFE_EVS_TaskInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[14]);

    /* Test task initialization where getting the application ID fails */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppID), -1);
    CFE_EVS_TaskInit();
    CFE_UtAssert_SYSLOG(EVS_SYSLOG_MSGS[10]);

    /* Test successful task initialization */
    UT_InitData_EVS();
    CFE_EVS_TaskInit();
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 0);

    /* Enable DEBUG message output */
    UT_InitData_EVS();
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAAPPEVTTYPE_EID);

    /* Disable ports */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT | CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_DISPORT_EID);

    UT_EVS_DisableSquelch();
}

/*
** Test functions using an illegal application ID
*/
void Test_IllegalAppID(void)
{
    CFE_TIME_SysTime_t time = {0, 0};
    CFE_ES_AppId_t     AppID;

    UtPrintf("Begin Test Illegal App ID");

    /* Set test up with illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* Test registering an event using an illegal application ID */
    UtAssert_INT32_EQ(CFE_EVS_Register(NULL, 0, 0), CFE_EVS_APP_ILLEGAL_APP_ID);

    /* Test sending events with a NULL spec */
    UtAssert_INT32_EQ(CFE_EVS_SendEvent(0, 0, NULL), CFE_EVS_INVALID_PARAMETER);
    UtAssert_INT32_EQ(CFE_EVS_SendEventWithAppID(0, 0, CFE_ES_APPID_UNDEFINED, NULL), CFE_EVS_INVALID_PARAMETER);
    UtAssert_INT32_EQ(CFE_EVS_SendTimedEvent(time, 0, 0, NULL), CFE_EVS_INVALID_PARAMETER);

    /* Test sending an event using an illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_EVS_SendEvent(0, 0, "NULL"), CFE_EVS_APP_ILLEGAL_APP_ID);

    /* Test sending an event using an illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(EVS_SendEvent(0, 0, "NULL"));

    /* Test sending a timed event using an illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_EVS_SendTimedEvent(time, 0, 0, "NULL"), CFE_EVS_APP_ILLEGAL_APP_ID);

    /* Test sending an event with app ID using an illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_EVS_SendEventWithAppID(0, 0, CFE_ES_APPID_UNDEFINED, "NULL"), CFE_EVS_APP_ILLEGAL_APP_ID);

    /* Test resetting a filter using an illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_EVS_ResetFilter(0), CFE_EVS_APP_ILLEGAL_APP_ID);

    /* Test resetting all filters using an illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_EVS_ResetAllFilters(), CFE_EVS_APP_ILLEGAL_APP_ID);

    /* Test application cleanup using an illegal application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_AppID_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_EVS_CleanUpApp(CFE_ES_APPID_UNDEFINED), CFE_EVS_APP_ILLEGAL_APP_ID);

    /* Test with out of range AppID */
    UT_InitData_EVS();
    AppID = CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_PLATFORM_ES_MAX_APPLICATIONS));
    UtAssert_INT32_EQ(CFE_EVS_SendEventWithAppID(0, 0, AppID, "NULL"), CFE_EVS_APP_ILLEGAL_APP_ID);
}

/*
** Test functions using an unregistered application
*/
void Test_UnregisteredApp(void)
{
    CFE_TIME_SysTime_t time = {0, 0};
    EVS_AppData_t *    AppDataPtr;
    CFE_ES_AppId_t     AppID;

    /* Get a local ref to the "current" AppData table entry */
    EVS_GetCurrentContext(&AppDataPtr, &AppID);

    UtPrintf("Begin Test Unregistered App");

    UT_InitData_EVS();

    /* Unregister the application (it was registered in CFE_EVS_TaskInit) */
    CFE_EVS_CleanUpApp(AppID);

    /* Test sending an event to an unregistered application */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_SendEvent(0, 0, "NULL"), CFE_EVS_APP_NOT_REGISTERED);

    /* Test resetting a filter using an unregistered application */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_ResetFilter(0), CFE_EVS_APP_NOT_REGISTERED);

    /* Test resetting all filters using an unregistered application */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_ResetAllFilters(), CFE_EVS_APP_NOT_REGISTERED);

    /* Test sending an event with app ID to an unregistered application */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_SendEventWithAppID(0, CFE_EVS_EventType_INFORMATION, AppID, "NULL"),
                      CFE_EVS_APP_NOT_REGISTERED);

    /* Test sending a timed event to an unregistered application */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_SendTimedEvent(time, CFE_EVS_EventType_INFORMATION, 0, "NULL"),
                      CFE_EVS_APP_NOT_REGISTERED);

    /* Test application cleanup using an unregistered application */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_CleanUpApp(AppID));

    /* Re-register the application for subsequent tests */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY));
}

/*
** Test filter registration related calls
*/
void Test_FilterRegistration(void)
{
    int i;

    CFE_EVS_BinFilter_t filter[CFE_PLATFORM_EVS_MAX_EVENT_FILTERS + 1];
    EVS_BinFilter_t *   FilterPtr = NULL;
    EVS_AppData_t *     AppDataPtr;
    CFE_ES_AppId_t      AppID;
    CFE_TIME_SysTime_t  time = {0, 0};

    /* Get a local ref to the "current" AppData table entry */
    EVS_GetCurrentContext(&AppDataPtr, &AppID);

    UtPrintf("Begin Test Filter Registration");

    CFE_EVS_Global.EVS_AppID                            = AppID;
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    /* Test filter registration using an invalid filter option */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY + 1), CFE_EVS_UNKNOWN_FILTER);

    /* Test successful filter registration with no filters */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY));

    /* Re-register to test valid unregistration */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY));

    /* Test successful app cleanup */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_CleanUpApp(AppID));

    /* Test successful filter registration with a valid filter */
    UT_InitData_EVS();
    filter[0].EventID = 0;
    filter[0].Mask    = 0x0001;
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(filter, 1, CFE_EVS_EventFilter_BINARY));

    /* Test successful multiple filter registration with valid filters */
    UT_InitData_EVS();

    for (i = 0; i < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS + 1; i++)
    {
        filter[i].EventID = i;
        filter[i].Mask    = 1;
    }

    CFE_UtAssert_SUCCESS(CFE_EVS_Register(filter, CFE_PLATFORM_EVS_MAX_EVENT_FILTERS + 1, CFE_EVS_EventFilter_BINARY));

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter = 0;

    /* Send 1st information message, should get through */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "OK"));
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter, 1);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg)), 1);

    /* Send 2nd information message, should be filtered */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "FAILED"));
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter, 1);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg)), 0);

    /* Send last information message, which should cause filtering to lock */
    UT_InitData_EVS();
    FilterPtr        = EVS_FindEventID(0, (EVS_BinFilter_t *)AppDataPtr->BinFilters);
    FilterPtr->Count = CFE_EVS_MAX_FILTER_COUNT - 1;
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "OK"));
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter, 3);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg)), 2);
    UtAssert_UINT32_EQ(FilterPtr->Count, CFE_EVS_MAX_FILTER_COUNT);

    /* Test that filter lock is applied */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "FAILED"));
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter, 3);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg)), 0);
    UtAssert_UINT32_EQ(FilterPtr->Count, CFE_EVS_MAX_FILTER_COUNT);

    /* Test that filter lock is (still) applied */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "FAILED"));
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter, 3);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg)), 0);
    UtAssert_UINT32_EQ(FilterPtr->Count, CFE_EVS_MAX_FILTER_COUNT);

    /* Return application to original state: re-register application */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY));

    /* Test sending an event with app ID to a registered, filtered
     * application
     */
    UT_InitData_EVS();
    AppDataPtr->AppID      = AppID;
    AppDataPtr->ActiveFlag = false;
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEventWithAppID(0, CFE_EVS_EventType_INFORMATION, AppID, "NULL"));

    /* Test sending a timed event to a registered, filtered application */
    UT_InitData_EVS();
    AppDataPtr->AppID      = AppID;
    AppDataPtr->ActiveFlag = false;
    CFE_UtAssert_SUCCESS(CFE_EVS_SendTimedEvent(time, CFE_EVS_EventType_INFORMATION, 0, "NULL"));
}

/*
** Test reset filter calls
*/
void Test_FilterReset(void)
{
    CFE_EVS_BinFilter_t filter;

    UtPrintf("Begin Test Filter Reset");

    /* Test successful filter registration */
    UT_InitData_EVS();
    filter.EventID = 1;
    filter.Mask    = 0x0001;
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(&filter, 1, CFE_EVS_EventFilter_BINARY));

    /* Test filter reset using an invalid event ID */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_ResetFilter(CFE_PLATFORM_EVS_MAX_EVENT_FILTERS + 1), CFE_EVS_EVT_NOT_REGISTERED);

    /* Test filter reset using an unregistered event ID */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_ResetFilter(-1));

    /* Test successful filter reset */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_ResetFilter(1));

    /* Test successful reset of all filters */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_ResetAllFilters());

    /* Return application to original state: re-register application */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY));
}

/*
** Test long and short format events, and event strings
** greater than the maximum length allowed
*/
void Test_Format(void)
{
    int    i;
    char   long_msg[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH + 2];
    uint16 EventID[2];

    CFE_TIME_SysTime_t              time = {0, 0};
    CFE_EVS_SetEventFormatModeCmd_t modecmd;
    CFE_EVS_EnableAppEventTypeCmd_t appbitcmd;
    CFE_EVS_PacketID_t              CapturedMsg;
    UT_SoftwareBusSnapshot_Entry_t  LongFmtSnapshotData = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_LONG_EVENT_MSG_MID),
                                                          .SnapshotBuffer = &CapturedMsg,
                                                          .SnapshotOffset =
                                                              offsetof(CFE_EVS_LongEventTlm_t, Payload.PacketID),
                                                          .SnapshotSize = sizeof(CapturedMsg)};
    UT_SoftwareBusSnapshot_Entry_t  ShortFmtSnapshotData = {
        .MsgId          = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_SHORT_EVENT_MSG_MID),
        .SnapshotBuffer = &CapturedMsg,
        .SnapshotOffset = offsetof(CFE_EVS_ShortEventTlm_t, Payload.PacketID),
        .SnapshotSize   = sizeof(CapturedMsg)};
    EVS_AppData_t *      AppDataPtr;
    CFE_ES_AppId_t       AppID;
    UT_EVS_MSGInitData_t MsgData;
    CFE_MSG_Message_t *  MsgSend;

    memset(&modecmd, 0, sizeof(modecmd));

    /* Get a local ref to the "current" AppData table entry */
    EVS_GetCurrentContext(&AppDataPtr, &AppID);

    UtPrintf("Begin Test Format");

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    /* Enable DEBUG message output */
    UT_InitData_EVS();
    strncpy(appbitcmd.Payload.AppName, "ut_cfe_evs", sizeof(appbitcmd.Payload.AppName) - 1);
    appbitcmd.Payload.AppName[sizeof(appbitcmd.Payload.AppName) - 1] = '\0';
    modecmd.Payload.MsgFormat                                        = CFE_EVS_MsgFormat_LONG;
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAAPPEVTTYPE_EID);

    /* Test set event format mode command using an invalid mode */
    UT_InitData_EVS();
    modecmd.Payload.MsgFormat = 0xff;
    UT_EVS_DoDispatchCheckEvents(&modecmd, sizeof(modecmd), UT_TPID_CFE_EVS_CMD_SET_EVENT_FORMAT_MODE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLEGALFMTMOD_EID);

    /* Test set event format mode command using a valid command to set short
     * format, reports implicitly via event
     */
    UT_InitData_EVS();
    modecmd.Payload.MsgFormat = CFE_EVS_MsgFormat_SHORT;
    UT_EVS_DoDispatchCheckEventsShort(&modecmd, sizeof(modecmd), UT_TPID_CFE_EVS_CMD_SET_EVENT_FORMAT_MODE_CC,
                                      &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_SETEVTFMTMOD_EID);

    UtPrintf("Test for short event sent when configured to do so ");
    UT_InitData_EVS();
    UT_SetHookFunction(UT_KEY(CFE_MSG_Init), UT_EVS_MSGInitHook, &MsgData);
    UT_SetDataBuffer(UT_KEY(CFE_SB_TransmitMsg), &MsgSend, sizeof(MsgSend), false);
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "Short format check 1");

    /* Note implementation initializes both short and long message */
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_MSG_Init)), 2);
    UtAssert_INT32_EQ(UT_GetStubCount(UT_KEY(CFE_SB_TransmitMsg)), 1);
    UtAssert_BOOL_TRUE(CFE_SB_MsgId_Equal(MsgData.MsgId, ShortFmtSnapshotData.MsgId));
    UtAssert_BOOL_FALSE(CFE_SB_MsgId_Equal(MsgData.MsgId, LongFmtSnapshotData.MsgId));

    /* Confirm the right message was sent */
    UtAssert_ADDRESS_EQ(MsgSend, MsgData.MsgPtr);

    /* Test set event format mode command using a valid command to set long
     * format, reports implicitly via event
     */
    UT_InitData_EVS();
    modecmd.Payload.MsgFormat = CFE_EVS_MsgFormat_LONG;
    UT_EVS_DoDispatchCheckEvents(&modecmd, sizeof(modecmd), UT_TPID_CFE_EVS_CMD_SET_EVENT_FORMAT_MODE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_SETEVTFMTMOD_EID);

    /* Test event long format mode command was successful (the following
     * messages are output if long format selection is successful)
     */
    UT_InitData_EVS();
    memset(&CapturedMsg, 0xFF, sizeof(CapturedMsg));
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &LongFmtSnapshotData);
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "Long format check (SendEvent)");
    EventID[0] = CapturedMsg.EventID;
    memset(&CapturedMsg, 0xFF, sizeof(CapturedMsg));
    CFE_EVS_SendTimedEvent(time, 0, CFE_EVS_EventType_INFORMATION, "Long format check (SendTimedEvent)");
    EventID[1] = CapturedMsg.EventID;
    memset(&CapturedMsg, 0xFF, sizeof(CapturedMsg));
    CFE_EVS_SendEventWithAppID(0, CFE_EVS_EventType_INFORMATION, AppID, "Long format check (SendEventWithAppID)");
    UtAssert_ZERO(EventID[0]);
    UtAssert_ZERO(EventID[1]);
    UtAssert_ZERO(CapturedMsg.EventID);

    /* Test sending an event using a string length greater than
     * the maximum allowed
     */
    UT_InitData_EVS();

    for (i = 0; i <= CFE_MISSION_EVS_MAX_MESSAGE_LENGTH; i++)
    {
        long_msg[i] = (char)(i % 10 + 48);
    }

    long_msg[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH + 1] = '\0';
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "%s", long_msg));

    /* Test sending an event with application ID using a string length
     * greater than the maximum allowed
     */
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEventWithAppID(0, CFE_EVS_EventType_INFORMATION, AppID, "%s", long_msg));

    /* Test sending a timed event using a string length greater than
     * the maximum allowed
     */
    CFE_UtAssert_SUCCESS(CFE_EVS_SendTimedEvent(time, 0, CFE_EVS_EventType_INFORMATION, "%s", long_msg));

    /* Force an invalid format and send for code coverage */
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG + 1;
    CFE_UtAssert_SUCCESS(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "%s", long_msg));
}

/*
** Test enable/disable of port outputs
*/
void Test_Ports(void)
{
    CFE_EVS_EnablePortsCmd_t       bitmaskcmd;
    UT_SoftwareBusSnapshot_Entry_t LocalSnapshotData = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_LONG_EVENT_MSG_MID)};
    CFE_TIME_SysTime_t             PacketTime;

    memset(&PacketTime, 0, sizeof(PacketTime));
    memset(&bitmaskcmd, 0, sizeof(bitmaskcmd));

    UtPrintf("Begin Test Ports");

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    /* Test enabling all ports; reports implicitly via port output */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT | CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAPORT_EID);

    /* Test that ports are enabled by sending a message */
    UT_InitData_EVS();
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &LocalSnapshotData);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgTime), &PacketTime, sizeof(PacketTime), false);
    UtAssert_UINT32_EQ(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "Test ports message"), CFE_SUCCESS);
    UtAssert_UINT32_EQ(LocalSnapshotData.Count, 1);
    UtAssert_STUB_COUNT(CFE_TIME_Print, 1);

    /* Disable all ports to cut down on unneeded output */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT | CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_DISPORT_EID);

    /* Test enabling a port using a bitmask that is out of range (high) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = 0xff;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test disabling a port using a bitmask that is out of range */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test enabling a port using a bitmask that is out of range (low) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test enabling a port 1 and 2, but not 3 and 4 (branch path coverage) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAPORT_EID);

    /* Test enabling a port 3 and 4, but not 1 and 2 (branch path coverage) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAPORT_EID);

    /* Test disabling a port using a bitmask that is out of range (low) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test disabling a port 1 and 2, but not 3 and 4 (branch path coverage) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT1_BIT | CFE_EVS_PORT2_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_DISPORT_EID);

    /* Test disabling a port 3 and 4, but not 1 and 2 (branch path coverage) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_PORT3_BIT | CFE_EVS_PORT4_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_DISPORT_EID);
}

/*
** Test event logging
*/
void Test_Logging(void)
{
    int    i;
    uint32 resetAreaSize              = 0;
    uint16 LogOverflowCounterExpected = 1;
    char   tmpString[100];
    union
    {
        CFE_EVS_NoopCmd_t             noopcmd;
        CFE_EVS_ClearLogCmd_t         clearlogcmd;
        CFE_EVS_SetLogModeCmd_t       modecmd;
        CFE_EVS_WriteLogDataFileCmd_t logfilecmd;
    } CmdBuf;
    cpuaddr             TempAddr = 0;
    CFE_ES_ResetData_t *CFE_EVS_ResetDataPtr;

    UtPrintf("Begin Test Logging");

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    /* Initialize */
    UT_InitData_EVS();
    memset(&CmdBuf, 0, sizeof(CmdBuf));

    /* Enable logging and set conditions to allow complete code coverage */
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogEnabled = true;
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, CFE_PSP_RST_TYPE_POWERON);
    CFE_PSP_GetResetArea(&TempAddr, &resetAreaSize);
    CFE_EVS_ResetDataPtr      = (CFE_ES_ResetData_t *)TempAddr;
    CFE_EVS_Global.EVS_LogPtr = &CFE_EVS_ResetDataPtr->EVS_Log;

    /* Test setting the logging mode using an invalid mode */
    UT_InitData_EVS();
    CmdBuf.modecmd.Payload.LogMode = 0xff;
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.modecmd, sizeof(CmdBuf.modecmd), UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_LOGMODE_EID);

    /* Test setting the logging mode to discard */
    UT_InitData_EVS();
    CmdBuf.modecmd.Payload.LogMode = CFE_EVS_LogMode_DISCARD;
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.modecmd, sizeof(CmdBuf.modecmd), UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LOGMODE_EID);

    /* Test overfilling the log in discard mode */
    UT_InitData_EVS();
    UtAssert_VOIDCALL(EVS_ClearLog());

    /* Ensure log is filled, then add one more, implicitly testing
     * EVS_AddLog
     */
    for (i = 0; i < CFE_PLATFORM_EVS_LOG_MAX; i++)
    {
        snprintf(tmpString, 100, "Log fill event %d", i);
        CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "%s", tmpString);
    }

    CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "Log overfill event discard");
    UtAssert_BOOL_TRUE(CFE_EVS_Global.EVS_LogPtr->LogFullFlag);
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_LogPtr->LogMode, CFE_EVS_LogMode_DISCARD);
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter, LogOverflowCounterExpected);

    /* Test setting the logging mode to overwrite */
    UT_InitData_EVS();
    CmdBuf.modecmd.Payload.LogMode = CFE_EVS_LogMode_OVERWRITE;
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.modecmd, sizeof(CmdBuf.modecmd), UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC,
                                 &UT_EVS_EventBuf);
    LogOverflowCounterExpected = CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter + 1;
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "Log overfill event overwrite");
    UtAssert_BOOL_TRUE(CFE_EVS_Global.EVS_LogPtr->LogFullFlag);
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_LogPtr->LogMode, CFE_EVS_LogMode_OVERWRITE);
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter, LogOverflowCounterExpected);

    /* Test sending a no op command */
    UT_InitData_EVS();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.noopcmd, sizeof(CmdBuf.noopcmd), UT_TPID_CFE_EVS_CMD_NOOP_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_NOOP_EID);

    /* Clear log for next test */
    UT_InitData_EVS();
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogEnabled = true;
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.clearlogcmd, sizeof(CmdBuf.clearlogcmd), UT_TPID_CFE_EVS_CMD_CLEAR_LOG_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_BOOL_FALSE(CFE_EVS_Global.EVS_LogPtr->LogFullFlag);
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_LogPtr->LogOverflowCounter, 0);

    /* Test setting the logging mode to overwrite */
    UT_InitData_EVS();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetResetType), 1, CFE_PSP_RST_TYPE_POWERON);
    CFE_PSP_GetResetArea(&TempAddr, &resetAreaSize);
    CFE_EVS_ResetDataPtr           = (CFE_ES_ResetData_t *)TempAddr;
    CFE_EVS_Global.EVS_LogPtr      = &CFE_EVS_ResetDataPtr->EVS_Log;
    CmdBuf.modecmd.Payload.LogMode = CFE_EVS_LogMode_OVERWRITE;
    CFE_UtAssert_SUCCESS(CFE_EVS_SetLogModeCmd(&CmdBuf.modecmd));

    /* Test successfully writing a single event log entry using the default
     * log name
     */
    UT_InitData_EVS();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    CmdBuf.logfilecmd.Payload.LogFilename[0] = '\0';
    CFE_UtAssert_SUCCESS(CFE_EVS_WriteLogDataFileCmd(&CmdBuf.logfilecmd));

    /* Test writing a log entry with a file name failure */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UtAssert_INT32_EQ(CFE_EVS_WriteLogDataFileCmd(&CmdBuf.logfilecmd), CFE_FS_INVALID_PATH);

    /* Test writing a log entry with a create failure */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_EVS_WriteLogDataFileCmd(&CmdBuf.logfilecmd), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test successfully writing all log entries */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    CFE_EVS_Global.EVS_LogPtr->LogCount = CFE_PLATFORM_EVS_LOG_MAX;
    CFE_UtAssert_SUCCESS(CFE_EVS_WriteLogDataFileCmd(&CmdBuf.logfilecmd));

    /* Test writing a log entry with a write failure */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    CFE_EVS_Global.EVS_LogPtr->LogCount = CFE_PLATFORM_EVS_LOG_MAX;
    UtAssert_INT32_EQ(CFE_EVS_WriteLogDataFileCmd(&CmdBuf.logfilecmd), CFE_EVS_FILE_WRITE_ERROR);

    /* Test successfully writing a single event log entry using a specified
     * log name
     */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_SUCCESS);
    strncpy(CmdBuf.logfilecmd.Payload.LogFilename, "LogFile", sizeof(CmdBuf.logfilecmd.Payload.LogFilename) - 1);
    CmdBuf.logfilecmd.Payload.LogFilename[sizeof(CmdBuf.logfilecmd.Payload.LogFilename) - 1] = '\0';
    CFE_UtAssert_SUCCESS(CFE_EVS_WriteLogDataFileCmd(&CmdBuf.logfilecmd));

    /* Test successfully writing a single event log entry with a failure
     * writing the header
     */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, sizeof(CFE_FS_Header_t) + 1);
    CmdBuf.logfilecmd.Payload.LogFilename[0] = '\0';
    UtAssert_INT32_EQ(CFE_EVS_WriteLogDataFileCmd(&CmdBuf.logfilecmd), CFE_EVS_FILE_WRITE_ERROR);
}

/*
** Test writing application data
*/
void Test_WriteApp(void)
{
    union
    {
        CFE_EVS_ResetCountersCmd_t      ResetCountersCmd;
        CFE_EVS_WriteAppDataFileCmd_t   AppDataCmd;
        CFE_EVS_EnableAppEventTypeCmd_t appbitcmd;
    } CmdBuf;

    UtPrintf("Begin Test Write App");

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    /* Enable DEBUG message output */
    UT_InitData_EVS();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.appbitcmd.Payload.AppName, "ut_cfe_evs", sizeof(CmdBuf.appbitcmd.Payload.AppName) - 1);
    CmdBuf.appbitcmd.Payload.AppName[sizeof(CmdBuf.appbitcmd.Payload.AppName) - 1] = '\0';
    CmdBuf.appbitcmd.Payload.BitMask =
        CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.appbitcmd, sizeof(CmdBuf.appbitcmd),
                                 UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAAPPEVTTYPE_EID);

    /* Test resetting counters */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.ResetCountersCmd, sizeof(CmdBuf.ResetCountersCmd),
                                 UT_TPID_CFE_EVS_CMD_RESET_COUNTERS_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_RSTCNT_EID);

    /* Test writing application data with a create failure using default
     * file name
     */
    UT_InitData_EVS();
    strncpy(CmdBuf.AppDataCmd.Payload.AppDataFilename, "ut_cfe_evs",
            sizeof(CmdBuf.AppDataCmd.Payload.AppDataFilename) - 1);
    CmdBuf.AppDataCmd.Payload.AppDataFilename[sizeof(CmdBuf.AppDataCmd.Payload.AppDataFilename) - 1] = '\0';
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.AppDataCmd, sizeof(CmdBuf.AppDataCmd),
                                 UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_CRDATFILE_EID);

    /* Test writing application data with bad file name */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.AppDataCmd, sizeof(CmdBuf.AppDataCmd),
                                 UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_CRDATFILE_EID);

    /* Test writing application data with a write/close failure */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_close), OS_ERROR);
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.AppDataCmd, sizeof(CmdBuf.AppDataCmd),
                                 UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_WRDATFILE_EID);

    /* Test successfully writing application data */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.AppDataCmd, sizeof(CmdBuf.AppDataCmd),
                                 UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_WRDAT_EID);

    /* Test writing application data with a create failure using specified
     * file name
     */
    UT_InitData_EVS();
    strncpy(CmdBuf.AppDataCmd.Payload.AppDataFilename, "AppDataFileName",
            sizeof(CmdBuf.AppDataCmd.Payload.AppDataFilename) - 1);
    CmdBuf.AppDataCmd.Payload.AppDataFilename[sizeof(CmdBuf.AppDataCmd.Payload.AppDataFilename) - 1] = '\0';
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_EVS_DoDispatchCheckEvents(&CmdBuf.AppDataCmd, sizeof(CmdBuf.AppDataCmd),
                                 UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_CRDATFILE_EID);

    /* Test writing application data with a failure writing the header */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, sizeof(CFE_FS_Header_t) + 1);
    CmdBuf.AppDataCmd.Payload.AppDataFilename[0] = '\0';
    UtAssert_INT32_EQ(CFE_EVS_WriteAppDataFileCmd(&CmdBuf.AppDataCmd), CFE_EVS_FILE_WRITE_ERROR);
}

/*
** Test commands with bad application names (unknown, illegal, unregistered)
*/
void Test_BadAppCmd(void)
{
    CFE_EVS_EnableAppEventTypeCmd_t appbitcmd;
    CFE_EVS_EnableAppEventsCmd_t    appnamecmd;
    CFE_EVS_AddEventFilterCmd_t     appmaskcmd;
    CFE_EVS_ResetFilterCmd_t        appcmdcmd;
    uint32                          TestAppIndex;

    UtPrintf("Begin Test Bad App Command");

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    UT_InitData_EVS();

    /* Set up event and mask */
    appbitcmd.Payload.BitMask  = 0;
    appmaskcmd.Payload.Mask    = 0;
    appmaskcmd.Payload.EventID = 0;
    appcmdcmd.Payload.EventID  = 0;

    strncpy(appbitcmd.Payload.AppName, "unknown_name", sizeof(appbitcmd.Payload.AppName) - 1);
    appbitcmd.Payload.AppName[sizeof(appbitcmd.Payload.AppName) - 1] = '\0';
    strncpy(appnamecmd.Payload.AppName, "unknown_name", sizeof(appnamecmd.Payload.AppName) - 1);
    appnamecmd.Payload.AppName[sizeof(appnamecmd.Payload.AppName) - 1] = '\0';
    strncpy(appmaskcmd.Payload.AppName, "unknown_name", sizeof(appmaskcmd.Payload.AppName) - 1);
    appmaskcmd.Payload.AppName[sizeof(appmaskcmd.Payload.AppName) - 1] = '\0';
    strncpy(appcmdcmd.Payload.AppName, "unknown_name", sizeof(appcmdcmd.Payload.AppName) - 1);
    appcmdcmd.Payload.AppName[sizeof(appcmdcmd.Payload.AppName) - 1] = '\0';

    /* Test disabling application event types with an unknown application ID */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test enabling application event types with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test disabling application events with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test enabling application events with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test resetting the application event counter with an unknown
     * application ID
     */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test modifying event filters with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test deleting event filters with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test setting the event filter mask with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_SET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test resetting the filter with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test resetting all filters with an unknown application ID */
    UT_InitData_EVS();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_GetAppIDByName), CFE_ES_ERR_NAME_NOT_FOUND);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_NOAPPIDFOUND_EID);

    /* Test disabling application event types with an illegal application ID */
    UT_InitData_EVS();
    strncpy(appbitcmd.Payload.AppName, "illegal_id", sizeof(appbitcmd.Payload.AppName) - 1);
    appbitcmd.Payload.AppName[sizeof(appbitcmd.Payload.AppName) - 1] = '\0';
    strncpy(appnamecmd.Payload.AppName, "illegal_id", sizeof(appnamecmd.Payload.AppName) - 1);
    appnamecmd.Payload.AppName[sizeof(appnamecmd.Payload.AppName) - 1] = '\0';
    strncpy(appmaskcmd.Payload.AppName, "illegal_id", sizeof(appmaskcmd.Payload.AppName) - 1);
    appmaskcmd.Payload.AppName[sizeof(appmaskcmd.Payload.AppName) - 1] = '\0';
    strncpy(appcmdcmd.Payload.AppName, "illegal_id", sizeof(appcmdcmd.Payload.AppName) - 1);
    appcmdcmd.Payload.AppName[sizeof(appcmdcmd.Payload.AppName) - 1] = '\0';

    /*
     * Generate an illegal AppID error when looking up the UT appID (first call),
     * but still allow the EVS AppID to send an event successfully (second call).
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test enabling application event types with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test disabling application events with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test enabling application events with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test resetting the application event counter with an illegal
     * application ID
     */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test adding the event filter with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test deleting the event filter with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test setting the filter mask with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_SET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test resetting the filter with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test resetting all filters with an illegal application ID */
    UT_InitData_EVS();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_ILLAPPIDRANGE_EID);

    /* Test disabling application event types with an unregistered
     * application ID
     */
    UT_InitData_EVS();
    TestAppIndex = 2;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    strncpy(appbitcmd.Payload.AppName, "unregistered_app", sizeof(appbitcmd.Payload.AppName) - 1);
    appbitcmd.Payload.AppName[sizeof(appbitcmd.Payload.AppName) - 1] = '\0';
    strncpy(appnamecmd.Payload.AppName, "unregistered_app", sizeof(appnamecmd.Payload.AppName) - 1);
    appnamecmd.Payload.AppName[sizeof(appnamecmd.Payload.AppName) - 1] = '\0';
    strncpy(appmaskcmd.Payload.AppName, "unregistered_app", sizeof(appmaskcmd.Payload.AppName) - 1);
    appmaskcmd.Payload.AppName[sizeof(appmaskcmd.Payload.AppName) - 1] = '\0';
    strncpy(appcmdcmd.Payload.AppName, "unregistered_app", sizeof(appcmdcmd.Payload.AppName) - 1);
    appcmdcmd.Payload.AppName[sizeof(appcmdcmd.Payload.AppName) - 1] = '\0';
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test enabling application event types with an unregistered
     * application ID
     */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test disabling application events with an unregistered application ID */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test enabling application events with an unregistered application ID */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test resetting the application event counter with an unregistered
     * application ID
     */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test adding the event filter with an unregistered application ID */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test deleting the event filter with an unregistered application ID */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test setting the filter mask with an unregistered application ID */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_SET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test resetting the filter with an unregistered application ID */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);

    /* Test resetting all filters with an unregistered application ID */
    UT_InitData_EVS();
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &TestAppIndex, sizeof(TestAppIndex), false);
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_APPNOREGS_EID);
}

/*
** Test event commands (and reset counter)
*/
void Test_EventCmd(void)
{
    uint16                          EventCount[4];
    CFE_EVS_EnablePortsCmd_t        bitmaskcmd;
    CFE_EVS_EnableAppEventTypeCmd_t appbitcmd;
    CFE_EVS_EnableAppEventsCmd_t    appnamecmd;
    UT_SoftwareBusSnapshot_Entry_t  LocalSnapshotData = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_LONG_EVENT_MSG_MID)};

    memset(&bitmaskcmd, 0, sizeof(bitmaskcmd));

    UtPrintf("Begin Test Event Command");

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    UT_InitData_EVS();

    /* Run the next series of tests with valid, registered application name */
    strncpy(appbitcmd.Payload.AppName, "ut_cfe_evs", sizeof(appbitcmd.Payload.AppName) - 1);
    appbitcmd.Payload.AppName[sizeof(appbitcmd.Payload.AppName) - 1] = '\0';
    strncpy(appnamecmd.Payload.AppName, "ut_cfe_evs", sizeof(appnamecmd.Payload.AppName) - 1);
    appnamecmd.Payload.AppName[sizeof(appnamecmd.Payload.AppName) - 1] = '\0';

    /* Test disabling of all events */
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);

    LocalSnapshotData.Count = 0;
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &LocalSnapshotData);
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_DEBUG, "FAIL : Debug message disabled");
    EventCount[0] = LocalSnapshotData.Count;
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "FAIL : Info message disabled");
    EventCount[1] = LocalSnapshotData.Count;
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_ERROR, "FAIL : Error message disabled");
    EventCount[2] = LocalSnapshotData.Count;
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_CRITICAL, "FAIL : Critical message disabled");
    EventCount[3] = LocalSnapshotData.Count;
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, 0xFFFF);
    UtAssert_ZERO(EventCount[0]);
    UtAssert_ZERO(EventCount[1]);
    UtAssert_ZERO(EventCount[2]);
    UtAssert_ZERO(EventCount[3]);

    /* Test enabling of all events */
    UT_InitData_EVS();
    LocalSnapshotData.Count = 0;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &LocalSnapshotData);
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_DEBUG, "Debug message enabled");
    EventCount[0] = LocalSnapshotData.Count;
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "Info message enabled");
    EventCount[1] = LocalSnapshotData.Count;
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_ERROR, "Error message enabled");
    EventCount[2] = LocalSnapshotData.Count;
    CFE_EVS_SendEvent(0, CFE_EVS_EventType_CRITICAL, "Critical message enabled");
    EventCount[3] = LocalSnapshotData.Count;
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAAPPEVTTYPE_EID);
    UtAssert_UINT32_EQ(EventCount[0], 1);
    UtAssert_UINT32_EQ(EventCount[1], 2);
    UtAssert_UINT32_EQ(EventCount[2], 3);
    UtAssert_UINT32_EQ(EventCount[3], 4);

    /* Test disabling event type using an illegal type */
    UT_InitData_EVS();
    appbitcmd.Payload.BitMask = 0xff;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    CFE_EVS_SendEvent(0, 0xffff, "FAIL : Illegal type disabled");
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test enabling event type using an illegal type */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test successful disabling of application events */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, 0xFFFF);

    /* Test successful enabling of application events */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAAPPEVT_EID);

    /* Test disabling event types (leave debug enabled) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_DISEVTTYPE_EID);

    /* Test enabling all event types (debug already enabled) */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAEVTTYPE_EID);

    /* Test successfully resetting the application event counter */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_RSTEVTCNT_EID);

    /* Test disabling an event type using an out of range bit mask (high) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = 0xff;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test enabling an event type using an out of range bit mask (high) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = 0xff;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test disabling an application event type using an out of
     * range bit mask (high)
     */
    UT_InitData_EVS();
    appbitcmd.Payload.BitMask = 0xff;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test enabling an application event type using an out of
     * range bit mask (high)
     */
    UT_InitData_EVS();
    appbitcmd.Payload.BitMask = 0xff;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test disabling an event type using an out of range bit mask (low) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_DISABLE_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test enabling an event type using an out of range bit mask (low) */
    UT_InitData_EVS();
    bitmaskcmd.Payload.BitMask = 0x0;
    UT_EVS_DoDispatchCheckEvents(&bitmaskcmd, sizeof(bitmaskcmd), UT_TPID_CFE_EVS_CMD_ENABLE_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test disabling an application event type using an out of
     * range bit mask (low)
     */
    UT_InitData_EVS();
    appbitcmd.Payload.BitMask = 0x0;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);

    /* Test enabling an application event type using an out of
     * range bit mask (low)
     */
    UT_InitData_EVS();
    appbitcmd.Payload.BitMask = 0x0;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_INVALID_BITMASK_EID);
}

/*
** Test filter commands
*/
void Test_FilterCmd(void)
{
    int                             i;
    CFE_EVS_EnableAppEventsCmd_t    appnamecmd;
    CFE_EVS_AddEventFilterCmd_t     appmaskcmd;
    CFE_EVS_ResetFilterCmd_t        appcmdcmd;
    CFE_EVS_EnableAppEventTypeCmd_t appbitcmd;

    UtPrintf("Begin Test Filter Command");

    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    UT_InitData_EVS();

    /* Set up event and mask */
    appmaskcmd.Payload.Mask    = 0;
    appmaskcmd.Payload.EventID = 0;
    appcmdcmd.Payload.EventID  = 0;

    /* Run the next series of tests with valid, registered application name */
    strncpy(appnamecmd.Payload.AppName, "ut_cfe_evs", sizeof(appnamecmd.Payload.AppName) - 1);
    appnamecmd.Payload.AppName[sizeof(appnamecmd.Payload.AppName) - 1] = '\0';
    strncpy(appmaskcmd.Payload.AppName, "ut_cfe_evs", sizeof(appmaskcmd.Payload.AppName) - 1);
    appmaskcmd.Payload.AppName[sizeof(appmaskcmd.Payload.AppName) - 1] = '\0';
    strncpy(appcmdcmd.Payload.AppName, "ut_cfe_evs", sizeof(appcmdcmd.Payload.AppName) - 1);
    appcmdcmd.Payload.AppName[sizeof(appcmdcmd.Payload.AppName) - 1] = '\0';
    strncpy(appbitcmd.Payload.AppName, "ut_cfe_evs", sizeof(appbitcmd.Payload.AppName) - 1);
    appbitcmd.Payload.AppName[sizeof(appbitcmd.Payload.AppName) - 1] = '\0';

    /* Enable all application event message output */
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAAPPEVTTYPE_EID);

    /* Ensure there is no filter for the next tests */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_EVTIDNOREGS_EID);

    /* Test setting a filter with an application that is not registered
     * for filtering
     */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_SET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_EVTIDNOREGS_EID);

    /* Test resetting a filter with an application that is not registered
     * for filtering
     */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_EVTIDNOREGS_EID);

    /* Test resetting all filters */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_RSTALLFILTER_EID);

    /* Test successfully adding an event filter */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ADDFILTER_EID);

    /* Test adding an event filter to an event already registered
     * for filtering
     */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_EVT_FILTERED_EID);

    /* Test successfully setting a filter mask */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_SET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_SETFILTERMSK_EID);

    /* Test successfully resetting a filter mask */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_RSTFILTER_EID);

    /* Test successfully resetting all filters */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appnamecmd, sizeof(appnamecmd), UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_RSTALLFILTER_EID);

    /* Test successfully deleting an event filter */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_DELFILTER_EID);

    /* Test filling the event filters */
    UT_InitData_EVS();
    UT_EVS_EventBuf.Count = 0;

    for (i = 0; i < CFE_PLATFORM_EVS_MAX_EVENT_FILTERS; i++)
    {
        UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                     &UT_EVS_EventBuf);
        appmaskcmd.Payload.EventID++;
    }

    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ADDFILTER_EID);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.Count, CFE_PLATFORM_EVS_MAX_EVENT_FILTERS);

    /* Test overfilling the event filters */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_MAXREGSFILTER_EID);

    /* Return application to original state, re-register application */
    UT_InitData_EVS();
    appmaskcmd.Payload.EventID = 0;
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY));

    /* Enable all application event message output */
    UT_InitData_EVS();
    appbitcmd.Payload.BitMask = CFE_EVS_DEBUG_BIT | CFE_EVS_INFORMATION_BIT | CFE_EVS_ERROR_BIT | CFE_EVS_CRITICAL_BIT;
    UT_EVS_DoDispatchCheckEvents(&appbitcmd, sizeof(appbitcmd), UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ENAAPPEVTTYPE_EID);

    /* Set-up to test filtering the same event twice */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ADDFILTER_EID);

    /* Test filtering the same event again */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appmaskcmd, sizeof(appmaskcmd), UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_EVT_FILTERED_EID);

    /* Test successful event filter deletion */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&appcmdcmd, sizeof(appcmdcmd), UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC,
                                 &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_DELFILTER_EID);

    /* Return application to original state, re-register application */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_Register(NULL, 0, CFE_EVS_EventFilter_BINARY));
}

/*
** Test commands with invalid command, command packet, and command length
*/
void Test_InvalidCmd(void)
{
    CFE_MSG_CommandHeader_t cmd;

    memset(&cmd, 0, sizeof(cmd));

    UtPrintf("Begin Test Invalid Command");

    /* Test invalid msg id event */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, sizeof(cmd), UT_TPID_CFE_EVS_INVALID_MID, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_MSGID_EID);

    /* Test invalid command code event */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, sizeof(cmd), UT_TPID_CFE_EVS_CMD_INVALID_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_ERR_CC_EID);

    /* Test invalid command length event */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_NOOP_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with reset counters command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_RESET_COUNTERS_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with enable event type command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_ENABLE_EVENT_TYPE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with disable event type command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_DISABLE_EVENT_TYPE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with set event format mode command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_SET_EVENT_FORMAT_MODE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with enable application event
     * type command
     */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENT_TYPE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with disable application event
     * type command
     */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENT_TYPE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with enable application events command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_ENABLE_APP_EVENTS_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with disable application events command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_DISABLE_APP_EVENTS_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with reset application counter command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_RESET_APP_COUNTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with set filter command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_SET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with enable ports command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_ENABLE_PORTS_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with disable ports command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_DISABLE_PORTS_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with reset filter command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_RESET_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with reset all filters command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_RESET_ALL_FILTERS_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with add event filter command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_ADD_EVENT_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with delete event filter command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_DELETE_EVENT_FILTER_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with write application data command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_WRITE_APP_DATA_FILE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with write log data command */
    UT_InitData_EVS();
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogEnabled = true;
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_WRITE_LOG_DATA_FILE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with set log mode command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);

    /* Test invalid command length with clear log command */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&cmd, 0, UT_TPID_CFE_EVS_CMD_CLEAR_LOG_CC, &UT_EVS_EventBuf);
    UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LEN_ERR_EID);
}

/*
** Test squelching
*/
void Test_Squelching(void)
{
    int                            i, j;
    OS_time_t                      InjectedTime;
    CFE_EVS_LongEventTlm_t         CapturedTlm;
    UT_SoftwareBusSnapshot_Entry_t SnapshotData = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_LONG_EVENT_MSG_MID),
                                                   .SnapshotBuffer = &CapturedTlm,
                                                   .SnapshotOffset = 0,
                                                   .SnapshotSize   = sizeof(CapturedTlm)};
    const uint32                   EVENT_ID     = 142;
    CFE_Status_t                   EVS_Retval;
    UT_EVS_SendEventFunc_t         SendEventFuncs[] = {UT_EVS_SendSquelchedEvent, UT_EVS_SendSquelchedEventWithAppId,
                                               UT_EVS_SendSquelchedTimedEvent};

    EVS_AppData_t *AppDataPtr;

    UtPrintf("Begin Test Squelching");

    UT_InitData_EVS();
    UT_EVS_ResetSquelch();
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &SnapshotData);

    for (j = 0; j < sizeof(SendEventFuncs) / sizeof(SendEventFuncs[0]); j++)
    {
        UT_EVS_ResetSquelchCurrentContext();
        EVS_GetCurrentContext(&AppDataPtr, NULL);
        AppDataPtr->SquelchedCount = CFE_EVS_MAX_SQUELCH_COUNT - CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST;

        SnapshotData.Count = 0;
        /*
         * Test CFE_EVS_SendEvent squelching
         */
        for (i = 0; i < CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST * 3; i++)
        {
            InjectedTime = OS_TimeAssembleFromMilliseconds(0, 0);

            UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &InjectedTime, sizeof(InjectedTime), false);
            EVS_Retval = SendEventFuncs[j](EVENT_ID);
            if (i < CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST)
            {
                UtAssert_UINT32_EQ(SnapshotData.Count, i + 1);
                UtAssert_UINT32_EQ(CapturedTlm.Payload.PacketID.EventID, EVENT_ID);
                UtAssert_UINT32_EQ(EVS_Retval, CFE_SUCCESS);
            }
            else if (i == CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST)
            {
                UtAssert_UINT32_EQ(SnapshotData.Count, i + 1);
                UtAssert_UINT32_EQ(CapturedTlm.Payload.PacketID.EventID, CFE_EVS_SQUELCHED_ERR_EID);
                UtAssert_UINT32_EQ(EVS_Retval, CFE_EVS_APP_SQUELCHED);
            }
            else
            {
                UtAssert_UINT32_EQ(SnapshotData.Count, CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST + 1);
                UtAssert_UINT32_EQ(EVS_Retval, CFE_EVS_APP_SQUELCHED);
            }
        }
        /*
         * Test unsquelching w/ passage of time
         */
        /* Allow time for tokens to return */
        InjectedTime = OS_TimeAssembleFromMilliseconds(0, 1000 * CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST /
                                                                  CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC +
                                                              1000 / CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC);
        UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &InjectedTime, sizeof(InjectedTime), false);
        EVS_Retval = SendEventFuncs[j](EVENT_ID);
        UtAssert_UINT32_EQ(SnapshotData.Count, CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST + 2);
        UtAssert_UINT32_EQ(CapturedTlm.Payload.PacketID.EventID, EVENT_ID);
        UtAssert_UINT32_EQ(EVS_Retval, CFE_SUCCESS);

        /*
         * Test re-squelching event message
         */
        UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &InjectedTime, sizeof(InjectedTime), false);
        EVS_Retval = SendEventFuncs[j](EVENT_ID);
        UtAssert_UINT32_EQ(SnapshotData.Count, CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST + 3);
        UtAssert_UINT32_EQ(CapturedTlm.Payload.PacketID.EventID, CFE_EVS_SQUELCHED_ERR_EID);
        UtAssert_UINT32_EQ(EVS_Retval, CFE_EVS_APP_SQUELCHED);

        /*
         * Test re-squelching
         */
        UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &InjectedTime, sizeof(InjectedTime), false);
        EVS_Retval = SendEventFuncs[j](EVENT_ID);
        UtAssert_UINT32_EQ(SnapshotData.Count, CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST + 3);
        UtAssert_UINT32_EQ(EVS_Retval, CFE_EVS_APP_SQUELCHED);

        /*
         * Test positive saturation of tokens
         */
        InjectedTime = OS_TimeAssembleFromMilliseconds(0, 3 * 1000 * CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST /
                                                                  CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC +
                                                              1000 / CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC);
        UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &InjectedTime, sizeof(InjectedTime), false);
        EVS_Retval = SendEventFuncs[j](EVENT_ID);
        UtAssert_UINT32_EQ(SnapshotData.Count, CFE_PLATFORM_EVS_MAX_APP_EVENT_BURST + 4);
        UtAssert_UINT32_EQ(CapturedTlm.Payload.PacketID.EventID, EVENT_ID);
        UtAssert_UINT32_EQ(EVS_Retval, CFE_SUCCESS);
        UtAssert_UINT32_EQ(AppDataPtr->SquelchedCount, CFE_EVS_MAX_SQUELCH_COUNT);

        /*
         * Test boundary condition where SquelchTokens > -EVENT_COST && CreditCount >= EVENT_COST
         */
        InjectedTime = OS_TimeAssembleFromMilliseconds(1, 1200 / CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC);
        UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &InjectedTime, sizeof(InjectedTime), false);
        AppDataPtr->SquelchedCount            = 0;
        AppDataPtr->SquelchTokens             = -1500;
        AppDataPtr->LastSquelchCreditableTime = OS_TimeAssembleFromMilliseconds(1, 0);
        EVS_Retval                            = SendEventFuncs[j](EVENT_ID);
        UtAssert_UINT32_EQ(EVS_Retval, CFE_EVS_APP_SQUELCHED);
    }

    UT_EVS_DisableSquelch();
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), NULL, NULL);
}

/*
** Test miscellaneous functionality
*/
void Test_Misc(void)
{
    union
    {
        CFE_MSG_Message_t             msg;
        CFE_EVS_SendHkCmd_t           sendhkcmd;
        CFE_EVS_SetLogModeCmd_t       modecmd;
        CFE_EVS_WriteLogDataFileCmd_t writelogdatacmd;
    } PktBuf;

    CFE_ES_AppId_t                 AppID;
    EVS_AppData_t *                AppDataPtr;
    int                            i;
    char                           msg[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH + 2];
    UT_SoftwareBusSnapshot_Entry_t HK_SnapshotData = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_EVS_HK_TLM_MID)};

    /* Cover null cases for EVS_GetCurrentContext */
    EVS_GetCurrentContext(NULL, &AppID);
    EVS_GetCurrentContext(&AppDataPtr, NULL);

    UtPrintf("Begin Test Miscellaneous");

    memset(&PktBuf, 0, sizeof(PktBuf));
    CFE_EVS_Global.EVS_AppID                            = AppID;
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageFormatMode = CFE_EVS_MsgFormat_LONG;

    /*
     * NOTE - events in the following tests depend on whether the debug events are enabled.
     * If debug is disabled, then no event will be sent (0xFFFF).  If enabled, then the event ID
     * must be the expected one.
     */

    /* Test successful log data file write */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&PktBuf.writelogdatacmd, sizeof(PktBuf.writelogdatacmd),
                                 UT_TPID_CFE_EVS_CMD_WRITE_LOG_DATA_FILE_CC, &UT_EVS_EventBuf);
    if (UT_EVS_EventBuf.EventID == 0xFFFF)
    {
        UtAssert_NA("No event from WRITE_LOG_DATA_FILE_CC, debug events disabled");
    }
    else
    {
        UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_WRLOG_EID);
    }

    /* Test successfully setting the logging mode */
    UT_InitData_EVS();
    UT_EVS_DoDispatchCheckEvents(&PktBuf.modecmd, sizeof(PktBuf.modecmd), UT_TPID_CFE_EVS_CMD_SET_LOG_MODE_CC,
                                 &UT_EVS_EventBuf);
    if (UT_EVS_EventBuf.EventID == 0xFFFF)
    {
        UtAssert_NA("No event from SET_LOG_MODE_CC, debug events disabled");
    }
    else
    {
        UtAssert_UINT32_EQ(UT_EVS_EventBuf.EventID, CFE_EVS_LOGMODE_EID);
    }

    /* Test housekeeping report with log enabled */
    UT_InitData_EVS();
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogEnabled = true;
    HK_SnapshotData.Count                        = 0;
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &HK_SnapshotData);
    UT_CallTaskPipe(CFE_EVS_ProcessCommandPacket, &PktBuf.msg, sizeof(PktBuf.sendhkcmd), UT_TPID_CFE_EVS_SEND_HK);
    UtAssert_UINT32_EQ(HK_SnapshotData.Count, 1);

    /* Test successful application cleanup */
    UT_InitData_EVS();
    CFE_UtAssert_SUCCESS(CFE_EVS_CleanUpApp(CFE_ES_APPID_UNDEFINED));

    /* Test registering an application with invalid filter argument */
    UT_InitData_EVS();
    UtAssert_INT32_EQ(CFE_EVS_Register(NULL, 1, 0), CFE_ES_BAD_ARGUMENT);

    /* Test housekeeping report with log disabled */
    UT_InitData_EVS();
    CFE_EVS_Global.EVS_TlmPkt.Payload.LogEnabled = false;
    HK_SnapshotData.Count                        = 0;
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &HK_SnapshotData);
    UT_CallTaskPipe(CFE_EVS_ProcessCommandPacket, &PktBuf.msg, sizeof(PktBuf.sendhkcmd), UT_TPID_CFE_EVS_SEND_HK);
    UtAssert_UINT32_EQ(HK_SnapshotData.Count, 1);

    /* Test sending a packet with the message counter and the event counter
     * at their maximum allowed values
     */
    UT_InitData_EVS();
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter = CFE_EVS_MAX_EVENT_SEND_COUNT;
    AppDataPtr->EventCount                               = CFE_EVS_MAX_EVENT_SEND_COUNT;
    EVS_SendEvent(0, 0, "Max Event Count");
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_TlmPkt.Payload.MessageSendCounter, CFE_EVS_MAX_EVENT_SEND_COUNT);
    UtAssert_UINT32_EQ(AppDataPtr->EventCount, CFE_EVS_MAX_EVENT_SEND_COUNT);

    /* Test sending a message with the message length greater than the
     * maximum allowed value
     */
    UT_InitData_EVS();

    for (i = 0; i <= CFE_MISSION_EVS_MAX_MESSAGE_LENGTH; i++)
    {
        msg[i] = 'a';
    }

    msg[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH]               = '\0';
    CFE_EVS_Global.EVS_TlmPkt.Payload.MessageTruncCounter = 0;
    EVS_AppDataSetUsed(AppDataPtr, AppID);
    AppDataPtr->ActiveFlag = true;
    AppDataPtr->EventTypesActiveFlag |= CFE_EVS_INFORMATION_BIT;
    EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, msg);
    UtAssert_UINT32_EQ(CFE_EVS_Global.EVS_TlmPkt.Payload.MessageTruncCounter, 1);

    /* Use all AppData and report housekeeping to get branch coverage */
    UT_InitData_EVS();
    for (i = 0; i < sizeof(CFE_EVS_Global.AppData) / sizeof(CFE_EVS_Global.AppData[0]); i++)
    {
        /* Doesn't matter here that AppID is all the same... */
        EVS_AppDataSetUsed(&CFE_EVS_Global.AppData[i], AppID);
    }
    UtAssert_UINT32_EQ(CFE_EVS_SendHkCmd(NULL), CFE_STATUS_NO_COUNTER_INCREMENT);
}
