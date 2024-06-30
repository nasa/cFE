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

/*
** Includes
*/
#include "time_UT.h"

/*
** External global variables
*/
const char *TIME_SYSLOG_MSGS[] = {NULL,
                                  "%s: Error reading cmd pipe,RC=0x%08X\n",
                                  "%s: Application Init Failed,RC=0x%08X\n",
                                  "%s: 1Hz OS_TimerAdd failed:RC=%ld\n",
                                  "%s: 1Hz OS_TimerSet failed:RC=%ld\n",
                                  "%s: Application Init Failed,RC=0x%08X\n",
                                  "%s: Failed invalid arguments\n"};

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define TIME_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TIME_##intf##_MID)

#define TIME_UT_MSG_DISPATCH(intf, cmd)       TIME_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_TIME_##cmd)
#define TIME_UT_CC_DISPATCH(intf, cc, cmd)    TIME_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define TIME_UT_ERROR_DISPATCH(intf, cc, err) TIME_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_HK =
    { TIME_UT_MSG_DISPATCH(SEND_HK,SendHkCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_TONE_CMD =
    { TIME_UT_MSG_DISPATCH(TONE_CMD,ToneSignalCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_DATA_CMD =
    { TIME_UT_MSG_DISPATCH(DATA_CMD,ToneDataCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_ONEHZ_CMD =
    { TIME_UT_MSG_DISPATCH(ONEHZ_CMD,OneHzCmd) };

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_CMD =
    { TIME_UT_MSG_DISPATCH(SEND_CMD,FakeToneCmd) };
#endif

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_NOOP_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_NOOP_CC, NoopCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_RESET_COUNTERS_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_RESET_COUNTERS_CC, ResetCountersCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SEND_DIAGNOSTIC_TLM_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SEND_DIAGNOSTIC_CC, SendDiagnosticCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STATE_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_STATE_CC, SetStateCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_SOURCE_CC, SetSourceCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_SIGNAL_CC, SetSignalCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_ADD_DELAY_CC, AddDelayCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_DELAY_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SUB_DELAY_CC, SubDelayCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_TIME_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_TIME_CC, SetTimeCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_MET_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_MET_CC, SetMETCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STCF_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_STCF_CC, SetSTCFCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SET_LEAP_SECONDS_CC, SetLeapSecondsCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_ADD_ADJUST_CC, AddAdjustCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_ADJUST_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SUB_ADJUST_CC, SubAdjustCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_ONEHZ_ADJUSTMENT_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_ADD_ONE_HZ_ADJUSTMENT_CC, AddOneHzAdjustmentCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_ONEHZ_ADJUSTMENT_CC =
    { TIME_UT_CC_DISPATCH(CMD, CFE_TIME_SUB_ONE_HZ_ADJUSTMENT_CC, SubOneHzAdjustmentCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_INVALID_CC =
    { TIME_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */

/*
** Global variables
*/
int32 ut_time_CallbackCalled;

/*
** Functions
*/
#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
/*****************************************************************************/
/**
** \brief OS_SelectTone stub function
**
** \par Description
**        This function is used as a placeholder for the OS API function
**        OS_SelectTone.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void OS_SelectTone(int16 Signal) {}
#endif

/*
 * A hook function for CFE_PSP_GetTime that updates the Reference State.
 * This mimics what would happen if a time update occurred at the moment
 * another task was reading the time.
 */
int32 UT_TimeRefUpdateHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    volatile CFE_TIME_ReferenceState_t *RefState;
    uint32 *                            UpdateCount = UserObj;
    uint32                              i;

    /*
     * NOTE: in order to trigger a read retry, this actually needs to do CFE_TIME_REFERENCE_BUF_DEPTH
     * updates, such that the buffer being read is overwritten.
     */
    if (*UpdateCount > 0)
    {
        for (i = 0; i < CFE_TIME_REFERENCE_BUF_DEPTH; ++i)
        {
            RefState                      = CFE_TIME_StartReferenceUpdate();
            RefState->AtToneLatch.Seconds = 1 + CallCount;
            RefState->ClockSetState       = CFE_TIME_SetState_WAS_SET;
            CFE_TIME_FinishReferenceUpdate(RefState);
        }
        --(*UpdateCount);
    }

    return StubRetcode;
}

void UtTest_Setup(void)
{
    /* Initialize unit test */
    UT_Init("time");
    UtPrintf("cFE TIME Unit Test Output File\n\n");

    UT_ADD_TEST(Test_Main);
    UT_ADD_TEST(Test_Init);
    UT_ADD_TEST(Test_GetTime);
    UT_ADD_TEST(Test_TimeOp);
    UT_ADD_TEST(Test_ConvertTime);
    UT_ADD_TEST(Test_Print);
    UT_ADD_TEST(Test_RegisterSyncCallbackTrue);
    UT_ADD_TEST(Test_ExternalTone);
    UT_ADD_TEST(Test_External);
    UT_ADD_TEST(Test_PipeCmds);
    UT_ADD_TEST(Test_ResetArea);
    UT_ADD_TEST(Test_State);
    UT_ADD_TEST(Test_GetReference);
    UT_ADD_TEST(Test_Tone);
    UT_ADD_TEST(Test_1Hz);
    UT_ADD_TEST(Test_UnregisterSynchCallback);
    UT_ADD_TEST(Test_CleanUpApp);
}

/*
** Test task entry point and main process loop
*/
void Test_Main(void)
{
    CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

    UtPrintf("Begin Test Main");

    /* Test error during Task Init */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);
    UtAssert_VOIDCALL(CFE_TIME_TaskMain());
    CFE_UtAssert_SYSLOG(TIME_SYSLOG_MSGS[5]);

    /* Test successful run through (a pipe read error is expected) */
    UT_InitData();

    /* Set up buffer for first cycle, pipe failure is on 2nd */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    CFE_TIME_TaskMain();
    CFE_UtAssert_SYSLOG(TIME_SYSLOG_MSGS[1]);
}

/*
** Test API and time task initialization (must be called prior to
** remaining tests)
*/
void Test_Init(void)
{
    int16 ExpRtn;
    int16 SubErrCnt = 0;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    int16 SubLocalErrCnt = 0;
#endif

    UtPrintf("Begin Test Init");

    /* Test successful API initialization */
    UT_InitData();
    ExpRtn = 3;
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    ExpRtn++;
#endif
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
#if (CFE_MISSION_TIME_CFG_FAKE_TONE == true)
    ExpRtn++;
#endif
#endif
    /* account for 1Hz command, which is always enabled */
    ExpRtn++;
    CFE_TIME_EarlyInit();
    UtAssert_STUB_COUNT(CFE_MSG_Init, ExpRtn);

    /* Test successful time task initialization */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TIME_TaskInit());

    /* Test response to a failure creating the first child task */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), -1);

    /*  Test response to a failure creating the second child task */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 2, -3);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), -3);

    /* Test response to an error creating a command pipe */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -1);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), -1);

    /* Test response to failure of the HK request subscription */
    UT_InitData();
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), -SubErrCnt);

    /* Test response to failure of the tone commands subscription */
    UT_InitData();
    ExpRtn = 0;

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    ExpRtn = -SubErrCnt;
#endif

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    SubLocalErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_SubscribeLocal), SubLocalErrCnt, -SubLocalErrCnt);
    ExpRtn = -SubLocalErrCnt;
#endif

    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), ExpRtn);

    /* Test response to failure of the time at the tone "data" commands
     * subscription
     */
    UT_InitData();
    ExpRtn = 0;

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    ExpRtn = -SubErrCnt;
#endif

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    SubLocalErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_SubscribeLocal), SubLocalErrCnt, -SubLocalErrCnt);
    ExpRtn = -SubLocalErrCnt;
#endif

    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), ExpRtn);

    /* Test response to failure of the 1Hz signal commands
     * subscription
     */
    UT_InitData();
    ExpRtn = 0;

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    ExpRtn = -SubErrCnt;
#endif

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    SubLocalErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_SubscribeLocal), SubLocalErrCnt, -SubLocalErrCnt);
    ExpRtn = -SubLocalErrCnt;
#endif

    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), ExpRtn);

    /* Test response to failure of the time at tone signal commands
     * subscription
     */
    UT_InitData();
#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    ExpRtn = -SubErrCnt;
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), ExpRtn);
#else
    SubErrCnt++;
    UtAssert_NA("*Not tested* Time at tone signal commands subscription failure");
#endif

    /* Test response to failure of the time task ground commands
     * subscription
     */
    UT_InitData();
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    ExpRtn = -SubErrCnt;
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), ExpRtn);

    /* Test response to failure creating a tone semaphore */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, -1);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test response to failure creating a local semaphore */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 2, -2);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test response to an EVS register failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), -1);

    /* Test response to an error sending an initialization event */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, -1);
    UtAssert_INT32_EQ(CFE_TIME_TaskInit(), -1);

    /* Test response to a failure to get the ID by name */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_TimeBaseGetIdByName), OS_ERROR);
    CFE_TIME_TaskInit();
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 0);

    /* Test response to an error setting up the 1Hz callback.
     * Note that this is only a SysLog message, it does not return the
     * error.  This allows the overall system to continue without the 1Hz
     */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerAdd), OS_ERROR);
    CFE_TIME_TaskInit();
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
    CFE_UtAssert_SYSLOG(TIME_SYSLOG_MSGS[3]);

    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_TimerSet), OS_ERROR);
    CFE_TIME_TaskInit();
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);
    CFE_UtAssert_SYSLOG((TIME_SYSLOG_MSGS[4]));
}

/*
** Test functions to retrieve time values
*/
void Test_GetTime(void)
{
    unsigned int result;
    uint16       StateFlags;
    /* Note: Time is in seconds + microseconds since epoch */
    unsigned int                        seconds   = 1041472984;
    unsigned int                        microsecs = 567890;
    volatile CFE_TIME_ReferenceState_t *RefState;
    CFE_TIME_SysTime_t                  time;
    CFE_TIME_SysTime_t                  expectedMET  = {.Seconds = 1041472994, .Subseconds = 2439068978};
    CFE_TIME_SysTime_t                  expectedTAI  = {.Seconds = 1041476594, .Subseconds = 2439068978};
    CFE_TIME_SysTime_t                  expectedUTC  = {.Seconds = 1041476562, .Subseconds = 2439068978};
    CFE_TIME_SysTime_t                  expectedSTCF = {.Seconds = 3600, .Subseconds = 0};

    UtPrintf("Begin Test Get Time");

    CFE_TIME_Global.LastVersionCounter = 0x1000;

    /* Test successfully retrieving the mission elapsed time */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    RefState                         = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds      = 20; /* 20.00000 */
    RefState->AtToneMET.Subseconds   = 0;
    RefState->AtToneSTCF.Seconds     = 3600; /* 01:00:00.00000 */
    RefState->AtToneSTCF.Subseconds  = 0;
    RefState->AtToneLeapSeconds      = 32;
    RefState->AtToneDelay.Seconds    = 0; /* 0.00000 */
    RefState->AtToneDelay.Subseconds = 0;
    RefState->AtToneLatch.Seconds    = 10; /* 10.00000 */
    RefState->AtToneLatch.Subseconds = 0;
    RefState->ClockSetState          = CFE_TIME_SetState_NOT_SET; /* Force invalid time */
    CFE_TIME_FinishReferenceUpdate(RefState);
    time = CFE_TIME_GetMET();
    UtAssert_UINT32_EQ(time.Seconds, expectedMET.Seconds);
    UtAssert_UINT32_EQ(time.Subseconds, expectedMET.Subseconds);

    /* Test successfully retrieving the mission elapsed time (seconds
     * portion)
     */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    result = seconds + RefState->AtToneMET.Seconds - RefState->AtToneLatch.Seconds;
    UtAssert_INT32_EQ(CFE_TIME_GetMETseconds(), result);

    /* Test successfully retrieving the mission elapsed time (sub-seconds
     * portion)
     */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    result = CFE_TIME_Micro2SubSecs(microsecs) + RefState->AtToneMET.Subseconds - RefState->AtToneLatch.Subseconds;
    UtAssert_INT32_EQ(CFE_TIME_GetMETsubsecs(), result);

    /* Test successfully retrieving the leap seconds */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TIME_GetLeapSeconds(), RefState->AtToneLeapSeconds);

    /* Test successfully retrieving the international atomic time (TAI) */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    time = CFE_TIME_GetTAI();
    UtAssert_UINT32_EQ(time.Seconds, expectedTAI.Seconds);
    UtAssert_UINT32_EQ(time.Subseconds, expectedTAI.Subseconds);

    /* Test successfully retrieving the coordinated universal time (UTC) */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    time = CFE_TIME_GetUTC();
    UtAssert_UINT32_EQ(time.Seconds, expectedUTC.Seconds);
    UtAssert_UINT32_EQ(time.Subseconds, expectedUTC.Subseconds);

    /* Test successfully retrieving the default time (UTC or TAI) */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    time = CFE_TIME_GetTime();

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)
    UtAssert_UINT32_EQ(time.Seconds, expectedTAI.Seconds);
    UtAssert_UINT32_EQ(time.Subseconds, expectedTAI.Subseconds);
#else
    UtAssert_UINT32_EQ(time.Seconds, expectedUTC.Seconds);
    UtAssert_UINT32_EQ(time.Subseconds, expectedUTC.Subseconds);
#endif

    /* Test successfully retrieving the spacecraft time correlation
     * factor (SCTF)
     */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    time = CFE_TIME_GetSTCF();
    UtAssert_UINT32_EQ(time.Seconds, expectedSTCF.Seconds);
    UtAssert_UINT32_EQ(time.Subseconds, expectedSTCF.Subseconds);

    /* Test retrieving the time status (invalid time is expected) */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TIME_GetClockState(), CFE_TIME_ClockState_INVALID);

    /* Test alternate flag values */
    RefState                         = CFE_TIME_StartReferenceUpdate();
    RefState->ClockSetState          = CFE_TIME_SetState_NOT_SET;
    RefState->ClockFlyState          = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.ClockSource      = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSignal      = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_Global.ServerFlyState   = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.Forced2Fly       = false;
    CFE_TIME_Global.OneTimeDirection = CFE_TIME_AdjustDirection_SUBTRACT;
    CFE_TIME_Global.OneHzDirection   = CFE_TIME_AdjustDirection_SUBTRACT;
    RefState->DelayDirection         = CFE_TIME_AdjustDirection_SUBTRACT;
    CFE_TIME_Global.IsToneGood       = false;
    CFE_TIME_Global.GetReferenceFail = false;
    CFE_TIME_FinishReferenceUpdate(RefState);
    StateFlags = 0;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    StateFlags |= CFE_TIME_FLAG_SERVER;
#endif

    UtAssert_UINT32_EQ(CFE_TIME_GetClockInfo(), StateFlags);

    /* Test successfully converting the clock state data to flag values */
    UT_InitData();
    RefState                 = CFE_TIME_StartReferenceUpdate();
    RefState->ClockSetState  = CFE_TIME_SetState_WAS_SET;
    RefState->ClockFlyState  = CFE_TIME_FlywheelState_IS_FLY;
    RefState->DelayDirection = CFE_TIME_AdjustDirection_ADD;
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_Global.ClockSource      = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_Global.ClockSignal      = CFE_TIME_ToneSignalSelect_PRIMARY;
    CFE_TIME_Global.ServerFlyState   = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_Global.Forced2Fly       = true;
    CFE_TIME_Global.OneTimeDirection = CFE_TIME_AdjustDirection_ADD;
    CFE_TIME_Global.OneHzDirection   = CFE_TIME_AdjustDirection_ADD;
    CFE_TIME_Global.IsToneGood       = true;
    CFE_TIME_Global.GetReferenceFail = true;

    StateFlags = CFE_TIME_FLAG_CLKSET | CFE_TIME_FLAG_FLYING | CFE_TIME_FLAG_SRCINT | CFE_TIME_FLAG_SIGPRI |
                 CFE_TIME_FLAG_SRVFLY | CFE_TIME_FLAG_CMDFLY | CFE_TIME_FLAG_ADD1HZ | CFE_TIME_FLAG_ADDADJ |
                 CFE_TIME_FLAG_ADDTCL | CFE_TIME_FLAG_GDTONE | CFE_TIME_FLAG_REFERR;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    StateFlags |= CFE_TIME_FLAG_SERVER;
#endif

    UtAssert_UINT32_EQ(CFE_TIME_GetClockInfo(), StateFlags);

    CFE_TIME_Global.GetReferenceFail = false;
}

/*
** Test operations on time (add, subtract, compare)
*/
void Test_TimeOp(void)
{
    CFE_TIME_SysTime_t time1, time2, result, exp_result;

    UtPrintf("Begin Test Time Operations");

    /* Initialize to zero time values */
    time1.Subseconds      = 0;
    time1.Seconds         = 0;
    time2.Subseconds      = 0;
    time2.Seconds         = 0;
    exp_result.Subseconds = 0;
    exp_result.Seconds    = 0;

    /* Test adding with both times equal zero */
    result = CFE_TIME_Add(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Add, Time A = time B = 0 seconds/subseconds");

    /* Test subtracting with both times equal zero */
    result = CFE_TIME_Subtract(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Subtract, Time A = time B = 0 seconds/subseconds");

    /* Test comparing with both times equal zero */
    UtAssert_INT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_EQUAL);

    /* Initialize to maximum time values */
    time1.Subseconds = 0xffffffff;
    time1.Seconds    = 0xffffffff;
    time2.Subseconds = 0xffffffff;
    time2.Seconds    = 0xffffffff;

    /* Test adding two maximum time values (extreme time rollover case) */
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds    = 0xffffffff;

    result = CFE_TIME_Add(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Add, Time A = time B = maximum seconds/subseconds (rollover)");

    /* Test subtracting two maximum time values (zero result) */
    exp_result.Subseconds = 0;
    exp_result.Seconds    = 0;

    result = CFE_TIME_Subtract(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Subtract, Time A = time B = maximum seconds/subseconds (zero result)");

    /* Test comparing two maximum time values */
    UtAssert_INT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_EQUAL);

    /* Initialize to single time value at maximum subseconds */
    time1.Subseconds = 0xffffffff;
    time1.Seconds    = 0xffff0000;
    time2.Subseconds = 0x00000001;
    time2.Seconds    = 0x0000ffff;

    /* Test adding two time values; time A > time B (minimal time
     * rollover case)
     */
    exp_result.Subseconds = 0;
    exp_result.Seconds    = 0;

    result = CFE_TIME_Add(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t), "CFE_TIME_Add, Time A > time B (rollover)");

    /* Test subtracting two time values; time A > time B */
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds    = 0xfffe0001;

    result = CFE_TIME_Subtract(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t), "CFE_TIME_Subtract, Time A > time B");

    /* Test comparing two time values; time A > time B (assumes time has
     * rolled over)
     */
    UtAssert_INT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_A_LT_B);

    /* Test adding two time values; time A < time B */
    exp_result.Subseconds = 0;
    exp_result.Seconds    = 0;

    result = CFE_TIME_Add(time2, time1);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t), "CFE_TIME_Add, Time A < time B");

    /* Test subtracting two time values; time A < time B (rollover) */
    exp_result.Subseconds = 0x00000002;
    exp_result.Seconds    = 0x0001fffe;

    result = CFE_TIME_Subtract(time2, time1);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t), "CFE_TIME_Subtract, Time A < time B (rollover)");

    /* Test comparing two time values; time A < time B (assumes time has
     * rolled over)
     */
    UtAssert_INT32_EQ(CFE_TIME_Compare(time2, time1), CFE_TIME_A_GT_B);

    /* Initialize so that only subseconds are different; seconds are
     * the same
     */
    time1.Subseconds = 30;
    time1.Seconds    = 3;
    time2.Subseconds = 29;
    time2.Seconds    = 3;

    /* Test adding two time values; time A subseconds > time B subseconds
     * (seconds same) */
    exp_result.Subseconds = 59;
    exp_result.Seconds    = 6;

    result = CFE_TIME_Add(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Add, Time A subseconds > time B subseconds (seconds same)");

    /* Test subtracting two time values; time A subseconds > time B
     * subseconds (seconds same)
     */
    exp_result.Subseconds = 1;
    exp_result.Seconds    = 0;

    result = CFE_TIME_Subtract(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Subtract, Time A subseconds > time B subseconds (seconds same)");

    /* Test comparing two time values; time A subseconds > time B subseconds
     * (seconds same)
     */
    UtAssert_INT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_A_GT_B);

    /* Test adding two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    exp_result.Subseconds = 59;
    exp_result.Seconds    = 6;

    result = CFE_TIME_Add(time2, time1);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Add, Time A subseconds < time B subseconds (seconds same)");

    /* Test subtracting two time values; time A subseconds < time B
     * subseconds (seconds same)
     */
    exp_result.Subseconds = 0xffffffff;
    exp_result.Seconds    = 0xffffffff;

    result = CFE_TIME_Subtract(time2, time1);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Subtract, Time A subseconds < time B subseconds (seconds same)");

    /* Test comparing two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    UtAssert_INT32_EQ(CFE_TIME_Compare(time2, time1), CFE_TIME_A_LT_B);

    /* Initialize so that only seconds are different; subseconds are
     * the same
     */
    time1.Subseconds = 18;
    time1.Seconds    = 8;
    time2.Subseconds = 18;
    time2.Seconds    = 7;

    /* Test adding two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    exp_result.Subseconds = 36;
    exp_result.Seconds    = 15;

    result = CFE_TIME_Add(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Add, Time A seconds > time B seconds (subseconds same)");

    /* Test subtracting two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    exp_result.Subseconds = 0;
    exp_result.Seconds    = 1;

    result = CFE_TIME_Subtract(time1, time2);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Subtract, Time A seconds > time B seconds (subseconds same)");

    /* Test comparing two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UtAssert_INT32_EQ(CFE_TIME_Compare(time1, time2), CFE_TIME_A_GT_B);

    /* Test adding two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    exp_result.Subseconds = 36;
    exp_result.Seconds    = 15;

    result = CFE_TIME_Add(time2, time1);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Add, Time A seconds < time B seconds (subseconds same)");

    /* Test subtracting two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    exp_result.Subseconds = 0;
    exp_result.Seconds    = 0xffffffff;

    result = CFE_TIME_Subtract(time2, time1);
    UtAssert_MemCmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t),
                    "CFE_TIME_Subtract, Time A seconds < time B seconds (subseconds same)");

    /* Test comparing two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TIME_Compare(time2, time1), CFE_TIME_A_LT_B);
}

/*
** Test time conversion functions
*/
void Test_ConvertTime(void)
{
    CFE_TIME_SysTime_t                  METTime;
    CFE_TIME_SysTime_t                  resultTime;
    CFE_TIME_SysTime_t                  expectedMET2SCTime;
    volatile CFE_TIME_ReferenceState_t *RefState;

    UtPrintf("Begin Test Convert Time");
    UT_InitData();

    /* Test MET to SCTF conversion */
    METTime.Seconds                 = 0;
    METTime.Subseconds              = 0;
    RefState                        = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneSTCF.Seconds    = 7240; /* 01:00:00.00000 */
    RefState->AtToneSTCF.Subseconds = 45;
    RefState->AtToneLeapSeconds     = 32;
#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)
    /* TAI time derived = MET + STCF */
    expectedMET2SCTime.Seconds = 7240;
#else
    /* UTC time derived = MET + STCF - Leaps */
    expectedMET2SCTime.Seconds = 7208;
#endif
    expectedMET2SCTime.Subseconds = 45;
    CFE_TIME_FinishReferenceUpdate(RefState);
    resultTime = CFE_TIME_MET2SCTime(METTime);
    UtAssert_UINT32_EQ(resultTime.Seconds, expectedMET2SCTime.Seconds);
    UtAssert_UINT32_EQ(resultTime.Subseconds, expectedMET2SCTime.Subseconds);

    /* NOTE: Microseconds <-> Subseconds conversion routines are implemented
     * as part of OS_time_t in OSAL, and are coverage tested there.  CFE time
     * conversions are now just wrappers of these OSAL routines.

     * This should only sanity-check basic values to get coverage of the CFE
     * wrappers.  Testing of value corner cases / rounding should be limited to
     * OSAL coverage test. */

    /* Test subseconds to microseconds conversion; zero subsecond value */
    UtAssert_UINT32_EQ(CFE_TIME_Sub2MicroSecs(0), 0);

    /* Test subseconds to microseconds conversion; half second */
    UtAssert_UINT32_EQ(CFE_TIME_Sub2MicroSecs(0x80000000), 500000);

    /* Test subseconds to microseconds conversion; subseconds exceeds
     * microseconds limit
     */
    UtAssert_UINT32_EQ(CFE_TIME_Sub2MicroSecs(0xffffffff), 999999);

    /* Test microseconds to subseconds conversion; zero microsecond value */
    UtAssert_UINT32_EQ(CFE_TIME_Micro2SubSecs(0), 0);

    /* Test microseconds to subseconds conversion; microseconds exceeds
     * maximum limit
     */
    UtAssert_UINT32_EQ(CFE_TIME_Micro2SubSecs(0xffffffff), 0xffffffff);
}

/*
** Test function for creating a text string representing the date and time
**
** NOTE: Test results depend on the epoch values in cfe_mission_cfg.h (the
**       tests below assume an epoch of 1980-001-00:00:00.00000).  Full
**       coverage is possible only when CFE_MISSION_TIME_EPOCH_SECOND > 0
*/
void Test_Print(void)
{
    char               timeBuf[CFE_TIME_PRINTED_STRING_SIZE];
    char               expectedBuf[CFE_TIME_PRINTED_STRING_SIZE];
    CFE_TIME_SysTime_t time;
    bool               usingDefaultEpoch = true;

    memset(&time, 0, sizeof(time));

    UtPrintf("Begin Test Print");

    if (CFE_MISSION_TIME_EPOCH_YEAR != 1980 || CFE_MISSION_TIME_EPOCH_DAY != 1 || CFE_MISSION_TIME_EPOCH_HOUR != 0 ||
        CFE_MISSION_TIME_EPOCH_MINUTE != 0 || CFE_MISSION_TIME_EPOCH_SECOND != 0 || CFE_MISSION_TIME_EPOCH_MICROS != 0)
    {
        UtPrintf("Custom epoch time requires manual inspection for CFE_TIME_Print");
        usingDefaultEpoch = false;
    }

    /* Test print with null print buffer argument */
    UT_InitData();
    UtAssert_VOIDCALL(CFE_TIME_Print(NULL, time));
    CFE_UtAssert_SYSLOG(TIME_SYSLOG_MSGS[6]);

    /* Test with zero time value */
    CFE_TIME_Print(timeBuf, time);
    if (usingDefaultEpoch)
    {
        strcpy(expectedBuf, "1980-001-00:00:00.00000");
        UtAssert_STRINGBUF_EQ(timeBuf, sizeof(timeBuf), expectedBuf, sizeof(expectedBuf));
    }
    else
    {
        UtAssert_MIR("Confirm adding seconds = %u, subseconds = %u to configured EPOCH results in time %s",
                     (unsigned int)time.Seconds, (unsigned int)time.Subseconds, timeBuf);
    }

    /* Test with a time value that causes seconds >= 60 when
     * CFE_MISSION_TIME_EPOCH_SECOND > 0
     */
    time.Subseconds = 0;
    time.Seconds    = 59;

    CFE_TIME_Print(timeBuf, time);
    if (usingDefaultEpoch)
    {
        strcpy(expectedBuf, "1980-001-00:00:59.00000");
        UtAssert_STRINGBUF_EQ(timeBuf, sizeof(timeBuf), expectedBuf, sizeof(expectedBuf));
    }
    else
    {
        UtAssert_MIR("Confirm adding seconds = %u, subseconds = %u to configured EPOCH results in time %s",
                     (unsigned int)time.Seconds, (unsigned int)time.Subseconds, timeBuf);
    }

    /* Test with mission representative time values */
    time.Subseconds = 215000;
    time.Seconds    = 1041472984;

    CFE_TIME_Print(timeBuf, time);
    if (usingDefaultEpoch)
    {
        strcpy(expectedBuf, "2013-001-02:03:04.00005");
        UtAssert_STRINGBUF_EQ(timeBuf, sizeof(timeBuf), expectedBuf, sizeof(expectedBuf));
    }
    else
    {
        UtAssert_MIR("Confirm adding seconds = %u, subseconds = %u to configured EPOCH results in time %s",
                     (unsigned int)time.Seconds, (unsigned int)time.Subseconds, timeBuf);
    }

    /* Test with maximum seconds and subseconds values */
    time.Subseconds = 0xffffffff;
    time.Seconds    = 0xffffffff;

    CFE_TIME_Print(timeBuf, time);
    if (usingDefaultEpoch)
    {
        strcpy(expectedBuf, "2116-038-06:28:15.99999");
        UtAssert_STRINGBUF_EQ(timeBuf, sizeof(timeBuf), expectedBuf, sizeof(expectedBuf));
    }
    else
    {
        UtAssert_MIR("Confirm adding seconds = %u, subseconds = %u to configured EPOCH results in time %s",
                     (unsigned int)time.Seconds, (unsigned int)time.Subseconds, timeBuf);
    }
}

/*
** Test function for use with register and unregister synch callback API tests
*/
int32 ut_time_MyCallbackFunc(void)
{
    ut_time_CallbackCalled++;
    return CFE_SUCCESS;
}

/*
** Test registering a synchronization callback function; report results
*/
void Test_RegisterSyncCallbackTrue(void)
{
    uint32 AppIndex;

    UtPrintf("Begin Test Register Synch Callback");
    UT_InitData();

    /*
     * Test calling api function with NULL argument
     */

    UT_InitData();
    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(NULL), CFE_TIME_BAD_ARGUMENT);

    /* CFE_ES_GetAppID failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, -1);
    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc), -1);

    /* CFE_ES_AppID_ToIndex failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, -2);
    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc), -2);

    /* Test registering the callback function the maximum number of times,
     * then attempt registering one more time
     */
    UT_InitData();
    CFE_TIME_Global.SynchCallback[0].Ptr = NULL;

    /*
     * One callback per application is allowed; the first should succeed,
     * the second should fail.
     */
    CFE_UtAssert_SUCCESS(CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc));

    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc), CFE_TIME_TOO_MANY_SYNCH_CALLBACKS);

    AppIndex = 2;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);

    CFE_UtAssert_SUCCESS(CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc));

    /*
     * This test case should not be possible in real flight as ES should never
     * return "success" with an appid out of range, but nonetheless
     * we need to make sure we do not overwrite our own memory here.
     */
    AppIndex = 99999;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    UtAssert_INT32_EQ(CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc), CFE_TIME_TOO_MANY_SYNCH_CALLBACKS);
}

/*
** Test external tone signal detection
*/
void Test_ExternalTone(void)
{
    UtPrintf("Begin Test External Tone");
    UT_InitData();

    UT_SetBSP_Time(123, 0);
    CFE_TIME_Global.ToneSignalLatch.Seconds    = 0;
    CFE_TIME_Global.ToneSignalLatch.Subseconds = 0;

    CFE_TIME_ExternalTone();

    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneSignalLatch.Seconds, 123);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneSignalLatch.Subseconds, 0);
}

/*
** Test setting time data from an external source
*/
void Test_External(void)
{
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true || CFE_PLATFORM_TIME_CFG_SRC_GPS == true || \
     CFE_PLATFORM_TIME_CFG_SRC_TIME == true)

    CFE_TIME_SysTime_t settime = {5, 4};
#endif

    UtPrintf("Begin Test External Time Set");

#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)
    /* Test setting time data from MET using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource   = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState = CFE_TIME_SetState_NOT_SET;
    CFE_TIME_Global.ExternalCount = 0;
    settime.Seconds               = 0;
    settime.Subseconds            = 0;
    CFE_TIME_ExternalMET(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ExternalCount, 1);

    /* Test setting time data from MET using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState            = CFE_TIME_SetState_WAS_SET;
    settime.Seconds                          = 0;
    settime.Subseconds                       = 0;
    CFE_TIME_Global.InternalCount            = 0;
    CFE_TIME_Global.AtToneMET.Seconds        = 10;
    CFE_TIME_Global.AtToneMET.Subseconds     = 0;
    CFE_TIME_Global.AtToneDelay.Seconds      = 0;
    CFE_TIME_Global.AtToneDelay.Subseconds   = 0;
    CFE_TIME_Global.AtToneLatch.Seconds      = 0;
    CFE_TIME_Global.AtToneLatch.Subseconds   = 0;
    CFE_TIME_Global.MaxDelta.Seconds         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);

    /* Test setting time data from MET using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState            = CFE_TIME_SetState_WAS_SET;
    settime.Seconds                          = 20;
    settime.Subseconds                       = 0;
    CFE_TIME_Global.InternalCount            = 0;
    CFE_TIME_Global.AtToneMET.Seconds        = 10;
    CFE_TIME_Global.AtToneMET.Subseconds     = 0;
    CFE_TIME_Global.AtToneDelay.Seconds      = 0;
    CFE_TIME_Global.AtToneDelay.Subseconds   = 0;
    CFE_TIME_Global.AtToneLatch.Seconds      = 0;
    CFE_TIME_Global.AtToneLatch.Subseconds   = 0;
    CFE_TIME_Global.MaxDelta.Seconds         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);

    /* Test setting time data from MET (external source, state set) */
    UT_InitData();
    CFE_TIME_Global.ClockSource              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState            = CFE_TIME_SetState_NOT_SET;
    settime.Seconds                          = 10;
    settime.Subseconds                       = 0;
    CFE_TIME_Global.ExternalCount            = 0;
    CFE_TIME_Global.AtToneMET.Seconds        = 10;
    CFE_TIME_Global.AtToneMET.Subseconds     = 0;
    CFE_TIME_Global.AtToneDelay.Seconds      = 0;
    CFE_TIME_Global.AtToneDelay.Subseconds   = 0;
    CFE_TIME_Global.AtToneLatch.Seconds      = 0;
    CFE_TIME_Global.AtToneLatch.Subseconds   = 0;
    CFE_TIME_Global.MaxDelta.Seconds         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ExternalCount, 1);

    /* Test setting time data from MET (internal source) */
    UT_InitData();
    CFE_TIME_Global.ClockSource   = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_Global.InternalCount = 0;
    CFE_TIME_ExternalMET(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);
#else
    UtAssert_NA("*Not tested* External MET - external source and clock state not set");
    UtAssert_NA("*Not tested* External MET - external source and time out of range (low)");
    UtAssert_NA("*Not tested* External MET - external source and time out of range (high)");
    UtAssert_NA("*Not tested* External MET - external source and time in range");
    UtAssert_NA("*Not tested* External MET (internal source)");
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
    /* Test setting time data from GPS using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource   = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState = CFE_TIME_SetState_NOT_SET;
    CFE_TIME_Global.ExternalCount = 0;
    settime.Seconds               = 0;
    settime.Subseconds            = 0;
    CFE_TIME_ExternalGPS(settime, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ExternalCount, 1);

    /* Test setting time data from GPS using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState            = CFE_TIME_SetState_WAS_SET;
    settime.Seconds                          = 0;
    settime.Subseconds                       = 0;
    CFE_TIME_Global.InternalCount            = 0;
    CFE_TIME_Global.AtToneMET.Seconds        = 0;
    CFE_TIME_Global.AtToneMET.Subseconds     = 0;
    CFE_TIME_Global.AtToneSTCF.Seconds       = 10;
    CFE_TIME_Global.AtToneSTCF.Subseconds    = 0;
    CFE_TIME_Global.AtToneDelay.Seconds      = 0;
    CFE_TIME_Global.AtToneDelay.Subseconds   = 0;
    CFE_TIME_Global.AtToneLatch.Seconds      = 0;
    CFE_TIME_Global.AtToneLatch.Subseconds   = 0;
    CFE_TIME_Global.MaxDelta.Seconds         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);

    /* Test setting time data from GPS using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState            = CFE_TIME_SetState_WAS_SET;
    settime.Seconds                          = 20;
    settime.Subseconds                       = 0;
    CFE_TIME_Global.InternalCount            = 0;
    CFE_TIME_Global.AtToneMET.Seconds        = 0;
    CFE_TIME_Global.AtToneMET.Subseconds     = 0;
    CFE_TIME_Global.AtToneSTCF.Seconds       = 10;
    CFE_TIME_Global.AtToneSTCF.Subseconds    = 0;
    CFE_TIME_Global.AtToneDelay.Seconds      = 0;
    CFE_TIME_Global.AtToneDelay.Subseconds   = 0;
    CFE_TIME_Global.AtToneLatch.Seconds      = 0;
    CFE_TIME_Global.AtToneLatch.Subseconds   = 0;
    CFE_TIME_Global.MaxDelta.Seconds         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);

    /* Test setting time data from GPS (external source, state set) */
    UT_InitData();
    CFE_TIME_Global.ClockSource              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSetState            = CFE_TIME_SetState_NOT_SET;
    settime.Seconds                          = 10;
    settime.Subseconds                       = 0;
    CFE_TIME_Global.ExternalCount            = 0;
    CFE_TIME_Global.AtToneMET.Seconds        = 0;
    CFE_TIME_Global.AtToneMET.Subseconds     = 0;
    CFE_TIME_Global.AtToneSTCF.Seconds       = 10;
    CFE_TIME_Global.AtToneSTCF.Subseconds    = 0;
    CFE_TIME_Global.AtToneDelay.Seconds      = 0;
    CFE_TIME_Global.AtToneDelay.Subseconds   = 0;
    CFE_TIME_Global.AtToneLatch.Seconds      = 0;
    CFE_TIME_Global.AtToneLatch.Subseconds   = 0;
    CFE_TIME_Global.MaxDelta.Seconds         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ExternalCount, 1);

    /* Test setting time data from GPS (internal source) */
    UT_InitData();
    CFE_TIME_Global.ClockSource   = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_Global.InternalCount = 0;
    CFE_TIME_ExternalGPS(settime, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);
#else
    UtAssert_NA("*Not tested* External GPS - external source and clock state not set");
    UtAssert_NA("*Not tested* External GPS - external source and time out of range (low)");
    UtAssert_NA("*Not tested* External GPS - external source and time out of range (high)");
    UtAssert_NA("*Not tested* External GPS - external source and time in range");
    UtAssert_NA("*Not tested* External GPS (internal source)");
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
    /* Test setting time data from Time using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource                     = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ReferenceState[0].ClockSetState = CFE_TIME_SetState_NOT_SET;
    CFE_TIME_Global.ExternalCount                   = 0;
    settime.Seconds                                 = 0;
    settime.Subseconds                              = 0;
    CFE_TIME_ExternalTime(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ExternalCount, 1);

    /* Test setting time data from Time using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource                              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ReferenceState[0].ClockSetState          = CFE_TIME_SetState_WAS_SET;
    settime.Seconds                                          = 0;
    settime.Subseconds                                       = 0;
    CFE_TIME_Global.InternalCount                            = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneMET.Seconds      = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneMET.Subseconds   = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneSTCF.Seconds     = 10;
    CFE_TIME_Global.ReferenceState[0].AtToneSTCF.Subseconds  = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneDelay.Seconds    = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneDelay.Subseconds = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneLatch.Seconds    = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneLatch.Subseconds = 0;
    CFE_TIME_Global.MaxDelta.Seconds                         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds                      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds                    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds                 = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UtAssert_INT32_EQ(CFE_TIME_Global.InternalCount, 1);

    /* Test setting time data from Time using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_Global.ClockSource                              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ReferenceState[0].ClockSetState          = CFE_TIME_SetState_WAS_SET;
    settime.Seconds                                          = 20;
    settime.Subseconds                                       = 0;
    CFE_TIME_Global.InternalCount                            = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneMET.Seconds      = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneMET.Subseconds   = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneSTCF.Seconds     = 10;
    CFE_TIME_Global.ReferenceState[0].AtToneSTCF.Subseconds  = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneDelay.Seconds    = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneDelay.Subseconds = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneLatch.Seconds    = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneLatch.Subseconds = 0;
    CFE_TIME_Global.MaxDelta.Seconds                         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds                      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds                    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds                 = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);

    /* Test setting time data from Time (external source, state set) */
    UT_InitData();
    CFE_TIME_Global.ClockSource                              = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ReferenceState[0].ClockSetState          = CFE_TIME_SetState_NOT_SET;
    settime.Seconds                                          = 10;
    settime.Subseconds                                       = 0;
    CFE_TIME_Global.ExternalCount                            = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneMET.Seconds      = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneMET.Subseconds   = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneSTCF.Seconds     = 10;
    CFE_TIME_Global.ReferenceState[0].AtToneSTCF.Subseconds  = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneDelay.Seconds    = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneDelay.Subseconds = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneLatch.Seconds    = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneLatch.Subseconds = 0;
    CFE_TIME_Global.ReferenceState[0].AtToneLeapSeconds      = 0;
    CFE_TIME_Global.MaxDelta.Seconds                         = 0;
    CFE_TIME_Global.MaxDelta.Subseconds                      = 1;
    CFE_TIME_Global.MaxLocalClock.Seconds                    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds                 = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ExternalCount, 1);

    /* Test setting time data from Time (internal source) */
    UT_InitData();
    CFE_TIME_Global.ClockSource   = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_Global.InternalCount = 0;
    CFE_TIME_ExternalTime(settime);
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, 1);
#else
    UtAssert_NA("*Not tested* External Time - external source and clock state not set");
    UtAssert_NA("*Not tested* External Time - external source and time out of range (low)");
    UtAssert_NA("*Not tested* External Time - external source and time out of range (high)");
    UtAssert_NA("*Not tested* External Time - external source and time in range");
    UtAssert_NA("*Not tested* External Time (internal source)");
#endif

    /* Reset to normal value for subsequent tests */
    CFE_TIME_Global.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
}

/*
** Test processing command pipe messages
*/
void Test_PipeCmds(void)
{
    union
    {
        CFE_MSG_Message_t                message;
        CFE_TIME_ToneDataCmd_t           tonedatacmd;
        CFE_TIME_ToneSignalCmd_t         tonesignalcmd;
        CFE_TIME_FakeToneCmd_t           timesendcmd;
        CFE_TIME_SendHkCmd_t             sendhkcmd;
        CFE_TIME_OneHzCmd_t              onehzcmd;
        CFE_TIME_NoopCmd_t               noopcmd;
        CFE_TIME_ResetCountersCmd_t      resetcounterscmd;
        CFE_TIME_SendDiagnosticCmd_t     diagtlmcmd;
        CFE_TIME_SetStateCmd_t           statecmd;
        CFE_TIME_SetSourceCmd_t          sourcecmd;
        CFE_TIME_SetSignalCmd_t          signalcmd;
        CFE_TIME_AddDelayCmd_t           adddelaycmd;
        CFE_TIME_SubDelayCmd_t           subdelaycmd;
        CFE_TIME_SetTimeCmd_t            settimecmd;
        CFE_TIME_SetMETCmd_t             setmetcmd;
        CFE_TIME_SetSTCFCmd_t            setstcfcmd;
        CFE_TIME_SetLeapSecondsCmd_t     leapscmd;
        CFE_TIME_AddAdjustCmd_t          addadjcmd;
        CFE_TIME_SubAdjustCmd_t          subadjcmd;
        CFE_TIME_AddOneHzAdjustmentCmd_t add1hzadjcmd;
        CFE_TIME_SubOneHzAdjustmentCmd_t sub1hzadjcmd;
    } CmdBuf;

    UT_SoftwareBusSnapshot_Entry_t LocalSnapshotData = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TIME_HK_TLM_MID)};

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    uint32 count;
#endif

    UtPrintf("Begin Test Pipe Commands");

    /* Test sending the housekeeping telemetry request command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetHookFunction(UT_KEY(CFE_SB_TransmitMsg), UT_SoftwareBusSnapshotHook, &LocalSnapshotData);
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sendhkcmd), UT_TPID_CFE_TIME_SEND_HK);
    UtAssert_INT32_EQ(LocalSnapshotData.Count, 1);

    /* Test sending the time at the tone "signal" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.ToneSignalCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.tonesignalcmd), UT_TPID_CFE_TIME_TONE_CMD);
    UtAssert_INT32_EQ(CFE_TIME_Global.ToneSignalCounter, 1);

    /* Test sending the time at the tone "data" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.ToneDataCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.tonedatacmd), UT_TPID_CFE_TIME_DATA_CMD);
    UtAssert_INT32_EQ(CFE_TIME_Global.ToneDataCounter, 1);

    /* Test sending the simulate time at the tone "signal" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetBSP_Time(123, 0);
    CFE_TIME_Global.ToneSignalLatch.Seconds    = 0;
    CFE_TIME_Global.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneSignalLatch.Seconds, 123);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneSignalLatch.Subseconds, 0);

    /* Test sending the request time at the tone "data" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    count = CFE_TIME_Global.InternalCount;
#endif

    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.timesendcmd), UT_TPID_CFE_TIME_SEND_CMD);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UtAssert_UINT32_EQ(CFE_TIME_Global.InternalCount, count + 1);
    CFE_UtAssert_EVENTNOTSENT(CFE_TIME_ID_ERR_EID);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_ID_ERR_EID);
#endif

    /* Test sending the no-op command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.noopcmd), UT_TPID_CFE_TIME_CMD_NOOP_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_NOOP_EID);

    /* Noop with bad size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_NOOP_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending the reset counters command */
    UT_InitData();
    CFE_TIME_Global.ToneMatchCounter      = 1;
    CFE_TIME_Global.ToneMatchErrorCounter = 1;
    CFE_TIME_Global.ToneSignalCounter     = 1;
    CFE_TIME_Global.ToneDataCounter       = 1;
    CFE_TIME_Global.ToneIntCounter        = 1;
    CFE_TIME_Global.ToneIntErrorCounter   = 1;
    CFE_TIME_Global.ToneTaskCounter       = 1;
    CFE_TIME_Global.LocalIntCounter       = 1;
    CFE_TIME_Global.LocalTaskCounter      = 1;
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.resetcounterscmd),
                    UT_TPID_CFE_TIME_CMD_RESET_COUNTERS_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_RESET_EID);

    /* Confirm error counters get reset to help cover requirements that are difficult operationally */
    UtAssert_ZERO(CFE_TIME_Global.ToneMatchCounter);
    UtAssert_ZERO(CFE_TIME_Global.ToneMatchErrorCounter);
    UtAssert_ZERO(CFE_TIME_Global.ToneSignalCounter);
    UtAssert_ZERO(CFE_TIME_Global.ToneDataCounter);
    UtAssert_ZERO(CFE_TIME_Global.ToneIntCounter);
    UtAssert_ZERO(CFE_TIME_Global.ToneIntErrorCounter);
    UtAssert_ZERO(CFE_TIME_Global.ToneTaskCounter);
    UtAssert_ZERO(CFE_TIME_Global.LocalIntCounter);
    UtAssert_ZERO(CFE_TIME_Global.LocalTaskCounter);

    /* Reset counters with bad size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_RESET_COUNTERS_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending the request diagnostics command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.diagtlmcmd),
                    UT_TPID_CFE_TIME_CMD_SEND_DIAGNOSTIC_TLM_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_DIAG_EID);

    /* Request diagnostics with bad size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SEND_DIAGNOSTIC_TLM_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending a clock state = invalid command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = CFE_TIME_ClockState_INVALID;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd), UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_STATE_EID);

    /* Test sending a clock state = valid command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = CFE_TIME_ClockState_VALID;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd), UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_STATE_EID);

    /* Test sending a clock state = flywheel command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = CFE_TIME_ClockState_FLYWHEEL;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd), UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_STATE_EID);

    /* Test response to sending a clock state command using an
     * invalid state
     */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = 99;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd), UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_STATE_ERR_EID);

    /* Set state with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending the set time source = internal command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.sourcecmd.Payload.TimeSource = CFE_TIME_SourceSelect_INTERNAL;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sourcecmd), UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC);

#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_SOURCE_EID);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_SOURCE_CFG_EID);
#endif

    /* Test sending the set time source = external command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.sourcecmd.Payload.TimeSource = CFE_TIME_SourceSelect_EXTERNAL;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sourcecmd), UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC);

#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_SOURCE_EID);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_SOURCE_CFG_EID);
#endif

    /* Test response to sending a set time source command using an
     * invalid source
     */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.sourcecmd.Payload.TimeSource = -1;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sourcecmd), UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_SOURCE_ERR_EID);

    /* Set time source with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending a set tone signal source = primary command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.signalcmd.Payload.ToneSource = CFE_TIME_ToneSignalSelect_PRIMARY;
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.signalcmd), UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC);

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_SIGNAL_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_SIGNAL_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Test sending a set tone signal source = redundant command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.signalcmd.Payload.ToneSource = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.signalcmd), UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC);

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_SIGNAL_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_SIGNAL_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Test response to sending a set tone signal source command using an
     * invalid source
     */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.signalcmd.Payload.ToneSource = -1;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.signalcmd), UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_SIGNAL_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Set tone signal source with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending a time tone add delay command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.adddelaycmd.Payload.MicroSeconds = 0;
    CmdBuf.adddelaycmd.Payload.Seconds      = 0;
    CFE_TIME_Global.CommandCounter          = 0;
    CFE_TIME_Global.CommandErrorCounter     = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.adddelaycmd), UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC);

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELAY_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELAY_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Time tone add delay with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending a time tone subtract delay command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.subdelaycmd), UT_TPID_CFE_TIME_CMD_SUB_DELAY_CC);

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELAY_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELAY_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Time tone subtract delay with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SUB_DELAY_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending a set time command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.settimecmd), UT_TPID_CFE_TIME_CMD_SET_TIME_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_TIME_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_TIME_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Set time with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SET_TIME_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending a set MET command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setmetcmd), UT_TPID_CFE_TIME_CMD_SET_MET_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_MET_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_MET_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Set MET with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SET_MET_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending a set STCF command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setstcfcmd), UT_TPID_CFE_TIME_CMD_SET_STCF_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_STCF_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_STCF_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Set STCF with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SET_STCF_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending an adjust STCF positive command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.addadjcmd), UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELTA_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELTA_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Adjust STCF positive with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending an adjust STCF negative command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.subadjcmd), UT_TPID_CFE_TIME_CMD_SUB_ADJUST_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELTA_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELTA_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Adjust STCF negative with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SUB_ADJUST_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending an adjust STCF 1 Hz positive command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.add1hzadjcmd),
                    UT_TPID_CFE_TIME_CMD_ADD_ONEHZ_ADJUSTMENT_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_ONEHZ_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_ONEHZ_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Adjust STCF 1Hz positive with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_ADD_ONEHZ_ADJUSTMENT_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test sending an adjust STCF 1 Hz negative command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sub1hzadjcmd),
                    UT_TPID_CFE_TIME_CMD_SUB_ONEHZ_ADJUSTMENT_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_ONEHZ_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_ONEHZ_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Adjust STCF 1 Hz negative with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SUB_ONEHZ_ADJUSTMENT_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test response to sending a tone delay command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.adddelaycmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.adddelaycmd), UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELAY_ERR_EID);

    /* Test response to sending a set time command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.settimecmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.settimecmd), UT_TPID_CFE_TIME_CMD_SET_TIME_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_TIME_ERR_EID);

    /* Test response to sending a set MET command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.setmetcmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setmetcmd), UT_TPID_CFE_TIME_CMD_SET_MET_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_MET_ERR_EID);

    /* Test response to sending a set STCF command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.setstcfcmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setstcfcmd), UT_TPID_CFE_TIME_CMD_SET_STCF_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_STCF_ERR_EID);

    /* Test response to sending an adjust STCF command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.setstcfcmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.addadjcmd), UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_DELTA_ERR_EID);

    /* Test sending a set leap seconds commands */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    CmdBuf.leapscmd.Payload.LeapSeconds = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.leapscmd),
                    UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEAPS_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 0);
#else
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEAPS_CFG_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);
#endif

    /* Set leap seconds with invalid size */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_Global.CommandCounter      = 0;
    CFE_TIME_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, 0, UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_LEN_ERR_EID);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandCounter, 0);
    UtAssert_UINT32_EQ(CFE_TIME_Global.CommandErrorCounter, 1);

    /* Test response to sending an invalid command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.noopcmd), UT_TPID_CFE_TIME_CMD_INVALID_CC);
    CFE_UtAssert_EVENTSENT(CFE_TIME_CC_ERR_EID);

    /* Test response to sending a command using an invalid message ID */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.noopcmd), UT_TPID_CFE_TIME_INVALID_MID);
    CFE_UtAssert_EVENTSENT(CFE_TIME_ID_ERR_EID);

    /* Call the Task Pipe with the 1Hz command. */
    /* In the 1Hz state machine it should call PSP GetTime as part,
        of latching the clock.  This is tested only to see that the latch executed. */
    UT_InitData();
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.onehzcmd), UT_TPID_CFE_TIME_ONEHZ_CMD);
    UtAssert_NONZERO(UT_GetStubCount(UT_KEY(CFE_PSP_GetTime)));
}

/*
** Test the reset area functionality
*/
void Test_ResetArea(void)
{
    CFE_TIME_Reference_t Reference;

    memset(&Reference, 0, sizeof(Reference));

    UtPrintf("Begin Test Reset Area");

    /* Test successfully updating the reset area */
    UT_InitData();
    CFE_TIME_Global.DataStoreStatus                 = CFE_TIME_RESET_AREA_EXISTING;
    CFE_TIME_Global.ClockSignal                     = CFE_TIME_ToneSignalSelect_PRIMARY;
    UT_GetResetDataPtr()->TimeResetVars.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_UpdateResetVars(&Reference);
    UtAssert_INT32_EQ(UT_GetResetDataPtr()->TimeResetVars.ClockSignal, CFE_TIME_Global.ClockSignal);

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_QueryResetVars();
    UtAssert_INT32_EQ(CFE_TIME_Global.DataStoreStatus, CFE_TIME_RESET_AREA_EXISTING);

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE, CFE_TIME_ToneSignalSelect_REDUNDANT);
    CFE_TIME_QueryResetVars();
    UtAssert_INT32_EQ(CFE_TIME_Global.DataStoreStatus, CFE_TIME_RESET_AREA_EXISTING);

    /* Test response to a bad reset area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_ERROR, CFE_TIME_RESET_SIGNATURE, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_QueryResetVars();
    UtAssert_INT32_EQ(CFE_TIME_Global.DataStoreStatus, CFE_TIME_RESET_AREA_BAD);

    /* Test initializing to default time values */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE + 1, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_QueryResetVars();
    UtAssert_INT32_EQ(CFE_TIME_Global.DataStoreStatus, CFE_TIME_RESET_AREA_NEW);

    /* Test response to a bad clock signal selection parameter */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE, CFE_TIME_ToneSignalSelect_REDUNDANT + 1);
    CFE_TIME_QueryResetVars();
    UtAssert_INT32_EQ(CFE_TIME_Global.DataStoreStatus, CFE_TIME_RESET_AREA_NEW);

    /* Test response to a reset area error */
    UT_InitData();
    CFE_TIME_Global.DataStoreStatus                 = CFE_TIME_RESET_AREA_ERROR;
    CFE_TIME_Global.ClockSignal                     = CFE_TIME_ToneSignalSelect_PRIMARY;
    UT_GetResetDataPtr()->TimeResetVars.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_UpdateResetVars(&Reference);
    UtAssert_INT32_EQ(UT_GetResetDataPtr()->TimeResetVars.ClockSignal, CFE_TIME_ToneSignalSelect_REDUNDANT);

    /* Test failure to get reset area updating the reset area */
    UT_InitData();
    UT_SetStatusBSPResetArea(CFE_PSP_ERROR, 0, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_Global.DataStoreStatus                 = CFE_TIME_RESET_AREA_EXISTING;
    CFE_TIME_Global.ClockSignal                     = CFE_TIME_ToneSignalSelect_PRIMARY;
    UT_GetResetDataPtr()->TimeResetVars.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_UpdateResetVars(&Reference);
    UtAssert_INT32_EQ(UT_GetResetDataPtr()->TimeResetVars.ClockSignal, CFE_TIME_ToneSignalSelect_REDUNDANT);
}

/*
** Test time state functions
*/
void Test_State(void)
{
    uint16                              flag;
    CFE_TIME_Reference_t                Reference;
    volatile CFE_TIME_ReferenceState_t *RefState;

    UtPrintf("Begin Test Time State");

    /* Test determining if the clock state is valid with the server state
     * in "no flywheel"
     */
    UT_InitData();
    Reference.ClockSetState        = CFE_TIME_SetState_WAS_SET;
    Reference.ClockFlyState        = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.ServerFlyState = CFE_TIME_FlywheelState_NO_FLY;
    UtAssert_INT32_EQ(CFE_TIME_CalculateState(&Reference), CFE_TIME_ClockState_VALID);

    /* Test determining if the clock state is valid with the server state
     * in "flywheel"
     */
    UT_InitData();
    Reference.ClockSetState        = CFE_TIME_SetState_WAS_SET;
    Reference.ClockFlyState        = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.ServerFlyState = CFE_TIME_FlywheelState_IS_FLY;
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    UtAssert_INT32_EQ(CFE_TIME_CalculateState(&Reference), CFE_TIME_ClockState_FLYWHEEL);
#else
    UtAssert_INT32_EQ(CFE_TIME_CalculateState(&Reference), CFE_TIME_ClockState_VALID);
#endif

    /* Test determining if the clock state = flywheel */
    UT_InitData();
    Reference.ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;
    UtAssert_INT32_EQ(CFE_TIME_CalculateState(&Reference), CFE_TIME_ClockState_FLYWHEEL);

    /* Test determining if the clock state = invalid */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_ClockState_INVALID;
    UtAssert_INT32_EQ(CFE_TIME_CalculateState(&Reference), CFE_TIME_ClockState_INVALID);

    /* Test alternate flag values */
    UT_InitData();

    /* set up initial state */
    CFE_TIME_SetState(CFE_TIME_ClockState_VALID);
    CFE_TIME_SetState(CFE_TIME_ClockState_FLYWHEEL);
    flag = CFE_TIME_GetClockInfo();

    /* Add server flag depending on configuration */
    RefState                 = CFE_TIME_StartReferenceUpdate();
    RefState->ClockSetState  = CFE_TIME_SetState_NOT_SET;
    RefState->ClockFlyState  = CFE_TIME_FlywheelState_NO_FLY;
    RefState->DelayDirection = CFE_TIME_AdjustDirection_SUBTRACT;

    /*
     * Atomicity Test Case: Check the flags _BEFORE_ calling FinishReferenceUpdate().
     * The function should return the same flags as the previous call, even though
     * the global data has been updated with new values.
     */
    UtAssert_INT32_EQ(CFE_TIME_GetClockInfo(), flag);

    /* Now finish the update and the flags should be different */
    CFE_TIME_FinishReferenceUpdate(RefState);

    flag &= ~CFE_TIME_FLAG_CLKSET;
    flag &= ~CFE_TIME_FLAG_FLYING;
    flag &= ~CFE_TIME_FLAG_ADDTCL;
    UtAssert_INT32_EQ(CFE_TIME_GetClockInfo(), flag);

    CFE_TIME_Global.ClockSource      = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.ClockSignal      = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_Global.ServerFlyState   = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.Forced2Fly       = false;
    CFE_TIME_Global.OneTimeDirection = CFE_TIME_AdjustDirection_SUBTRACT;
    CFE_TIME_Global.OneHzDirection   = CFE_TIME_AdjustDirection_SUBTRACT;

    flag = CFE_TIME_GetClockInfo();

    RefState                = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;

    UtAssert_INT32_EQ(CFE_TIME_GetClockInfo(), flag);

    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_Global.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
}

/*
** Test getting reference data (time at "tone")
*/
void Test_GetReference(void)
{
    CFE_TIME_Reference_t                Reference;
    volatile CFE_TIME_ReferenceState_t *RefState;
    uint32                              UpdateCount;

    UtPrintf("Begin Test Get Reference");

    /* Test with local clock rollover */
    UT_InitData();
    RefState                                 = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds              = 20;
    RefState->AtToneMET.Subseconds           = 0;
    RefState->AtToneDelay.Seconds            = 0;
    RefState->AtToneDelay.Subseconds         = 0;
    RefState->AtToneLatch.Seconds            = 10;
    RefState->AtToneLatch.Subseconds         = 0;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 1000;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + MaxLocalClock - AtToneLatch +
     *              BSP_Time [+ or - AtToneDelay]
     */
    UtAssert_UINT32_EQ(Reference.CurrentMET.Seconds, 1010);
    UtAssert_UINT32_EQ(Reference.CurrentMET.Subseconds, 0);

    /* Test without local clock rollover */
    UT_InitData();
    RefState                                 = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds              = 20;
    RefState->AtToneMET.Subseconds           = 0;
    RefState->AtToneDelay.Seconds            = 0;
    RefState->AtToneDelay.Subseconds         = 0;
    RefState->AtToneLatch.Seconds            = 10;
    RefState->AtToneLatch.Subseconds         = 0;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(15, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + BSP_Time - AtToneLatch [+ or - AtToneDelay]
     */
    UtAssert_UINT32_EQ(Reference.CurrentMET.Seconds, 25);
    UtAssert_UINT32_EQ(Reference.CurrentMET.Subseconds, 0);

    /* Use a hook function to test the behavior when the read needs to be retried */
    /* This just causes a single retry, the process should still succeed */
    UT_InitData();
    memset((void *)CFE_TIME_Global.ReferenceState, 0, sizeof(CFE_TIME_Global.ReferenceState));
    CFE_TIME_Global.GetReferenceFail = false;
    UpdateCount                      = 1;
    UT_SetHookFunction(UT_KEY(CFE_PSP_GetTime), UT_TimeRefUpdateHook, &UpdateCount);
    UT_SetBSP_Time(20, 0);
    UT_SetBSP_Time(20, 100);
    CFE_TIME_GetReference(&Reference);

    /* This should not have set the flag, and the output should be valid*/
    UtAssert_UINT32_EQ(CFE_TIME_Global.GetReferenceFail, false);
    UtAssert_UINT32_EQ(Reference.CurrentMET.Seconds, 19);
    UtAssert_UINT32_EQ(Reference.CurrentMET.Subseconds, 429497);
    UtAssert_UINT32_EQ(Reference.ClockSetState, CFE_TIME_SetState_WAS_SET);

    /* With multiple retries, it should fail */
    UpdateCount = 1000000;
    CFE_TIME_GetReference(&Reference);

    /* This should have set the flag, and the output should be all zero */
    UtAssert_UINT32_EQ(CFE_TIME_Global.GetReferenceFail, true);
    UtAssert_UINT32_EQ(Reference.CurrentMET.Seconds, 0);
    UtAssert_UINT32_EQ(Reference.CurrentMET.Subseconds, 0);
    UtAssert_UINT32_EQ(Reference.ClockSetState, CFE_TIME_SetState_NOT_SET);

    CFE_TIME_Global.GetReferenceFail = false;
}

/*
** Test send tone, and validate tone and data packet functions
*/
void Test_Tone(void)
{
    CFE_TIME_SysTime_t                  time1;
    CFE_TIME_SysTime_t                  time2;
    CFE_TIME_Reference_t                Reference;
    volatile CFE_TIME_ReferenceState_t *RefState;
    uint32                              VersionSave;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    int seconds        = 7654321;
    int virtualSeconds = 1234567;
#endif

    uint32 MinElapsed = CFE_TIME_Global.MinElapsed;
    uint32 MaxElapsed = CFE_TIME_Global.MaxElapsed;

    UtPrintf("Begin Test Tone");

    /* Test time at the tone in flywheel mode */
    UT_InitData();
    CFE_TIME_SetState(CFE_TIME_ClockState_FLYWHEEL);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_SetBSP_Time(seconds, 0);
    RefState                                                 = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds                              = 0; /* 20.00000 */
    RefState->AtToneMET.Subseconds                           = 0;
    RefState->AtToneDelay.Seconds                            = 0; /* 0.00000 */
    RefState->AtToneDelay.Subseconds                         = 0;
    RefState->AtToneLatch.Seconds                            = 0; /* 10.00000 */
    RefState->AtToneLatch.Subseconds                         = 0;
    CFE_TIME_Global.VirtualMET                               = virtualSeconds;
    CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Seconds    = 0;
    CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Subseconds = seconds;
    CFE_TIME_FinishReferenceUpdate(RefState);

    CFE_TIME_ToneSend();

#if (CFE_MISSION_TIME_AT_TONE_WILL_BE == true)
    seconds++;
#endif

#ifdef CFE_PLATFORM_TIME_CFG_BIGENDIAN
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Seconds, CFE_MAKE_BIG32(seconds));
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Subseconds, CFE_MAKE_BIG32(0));
#else  /* !CFE_PLATFORM_TIME_CFG_BIGENDIAN */
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Seconds, seconds);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Subseconds, 0);
#endif /* CFE_PLATFORM_TIME_CFG_BIGENDIAN */

    /* Test time at the tone when not in flywheel mode */
    UT_InitData();
    RefState                = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);

    CFE_TIME_ToneSend();

#if (CFE_MISSION_TIME_AT_TONE_WILL_BE == true)
    virtualSeconds++;
#endif

#ifdef CFE_PLATFORM_TIME_CFG_BIGENDIAN
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Seconds, CFE_MAKE_BIG32(virtualSeconds));
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Subseconds, CFE_MAKE_BIG32(0));
#else  /* !CFE_PLATFORM_TIME_CFG_BIGENDIAN */
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Seconds, virtualSeconds);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneDataCmd.Payload.AtToneMET.Subseconds, 0);
#endif /* CFE_PLATFORM_TIME_CFG_BIGENDIAN */

#else
    /* Added to prevent a missing test */
    UtAssert_NA("*Not tested* Send tone, flywheel ON");
    UtAssert_NA("*Not tested* Send tone, flywheel OFF");
#endif

    time1.Seconds           = 10;
    time1.Subseconds        = 0;
    time2.Seconds           = 10;
    time2.Subseconds        = 100;
    RefState                = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * false.  This test also serves to set the internal PrevTime variables
     * to known values
     */
    UT_InitData();
    VersionSave                      = CFE_TIME_Global.LastVersionCounter; /* Verifies 'ForcedToFly' path */
    CFE_TIME_Global.ToneMatchCounter = 0;
    CFE_TIME_Global.Forced2Fly       = false; /* Exercises '!ForcedToFly' path */
    CFE_TIME_ToneVerify(time1, time2);
    UtAssert_UINT32_EQ(CFE_TIME_Global.LastVersionCounter, VersionSave + 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchCounter, 1);

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * true (covers branch taken, and CFE_TIME_ToneUpdate not called).
     */
    UT_InitData();
    VersionSave                      = CFE_TIME_Global.LastVersionCounter;
    CFE_TIME_Global.ToneMatchCounter = 0;
    CFE_TIME_Global.Forced2Fly       = true;
    UtAssert_VOIDCALL(CFE_TIME_ToneVerify(time1, time2));
    UtAssert_UINT32_EQ(CFE_TIME_Global.LastVersionCounter, VersionSave);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchCounter, 0);

    /* Test tone validation when time 1 equals the previous time 1 value */
    UT_InitData();
    CFE_TIME_Global.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time1, time1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchErrorCounter, 1);

    /* Test tone validation when time 2 equals the previous time 2 value */
    UT_InitData();
    CFE_TIME_Global.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchErrorCounter, 1);

    /* Test tone validation with time 1 > time 2 value (clock rollover) */
    UT_InitData();
    time1.Seconds                            = 20;
    time2.Seconds                            = 0;
    time1.Subseconds                         = 0;
    time2.Subseconds                         = 100;
    CFE_TIME_Global.MaxLocalClock.Seconds    = 20; /* 1000.00000 */
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    CFE_TIME_Global.ToneMatchCounter         = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchCounter, 1);

    /* Test tone validation when time between packet and tone is out of
     * limits (seconds)
     */
    UT_InitData();
    time1.Seconds                         = 10;
    time2.Seconds                         = 11;
    time1.Subseconds                      = 0;
    time2.Subseconds                      = 0;
    CFE_TIME_Global.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchErrorCounter, 1);

    /* Test tone validation when time between packet and tone is out of
     * limits (subseconds low)
     */
    UT_InitData();
    time1.Seconds                         = 12;
    time2.Seconds                         = 12;
    time1.Subseconds                      = 0;
    time2.Subseconds                      = 10;
    CFE_TIME_Global.MinElapsed            = 20;
    CFE_TIME_Global.MaxElapsed            = 30;
    CFE_TIME_Global.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchErrorCounter, 1);

    /* Test tone validation when time between packet and tone is out of
     * limits (subseconds high)
     */
    UT_InitData();
    time1.Seconds                         = 13;
    time2.Seconds                         = 13;
    time1.Subseconds                      = 0;
    time2.Subseconds                      = 40;
    CFE_TIME_Global.MinElapsed            = 20;
    CFE_TIME_Global.MaxElapsed            = 30;
    CFE_TIME_Global.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchErrorCounter, 1);

    CFE_TIME_Global.MinElapsed = MinElapsed;
    CFE_TIME_Global.MaxElapsed = MaxElapsed;

    time1.Seconds    = 10;
    time1.Subseconds = 0;
    time2.Seconds    = 10;
    time2.Subseconds = 100;

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * false and the clock source is external
     */
    UT_InitData();
    VersionSave                        = CFE_TIME_Global.LastVersionCounter; /* Verifies 'ForcedToFly' path */
    CFE_TIME_Global.ToneMatchCounter   = 0;
    CFE_TIME_Global.Forced2Fly         = false; /* Exercises '!ForcedToFly' path */
    CFE_TIME_Global.ClockSource        = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_Global.VirtualMET         = 5;
    CFE_TIME_Global.PendingMET.Seconds = CFE_TIME_Global.VirtualMET;
    CFE_TIME_ToneVerify(time1, time2);
    UtAssert_UINT32_EQ(CFE_TIME_Global.LastVersionCounter, VersionSave + 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneMatchCounter, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.VirtualMET, 5);

    CFE_TIME_Global.ClockSource = CFE_TIME_SourceSelect_INTERNAL;

    /* Cover path for internal source and not flying */
    CFE_TIME_GetReference(&Reference);
    Reference.CurrentMET.Seconds = 10;
    RefState                     = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState      = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_Global.VirtualMET = 5;
    UtAssert_VOIDCALL(CFE_TIME_ToneUpdate());
    UtAssert_UINT32_EQ(CFE_TIME_Global.VirtualMET, 5);

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    /* Test tone update using an invalid pending state */
    UT_InitData();
    CFE_TIME_Global.PendingState   = CFE_TIME_ClockState_INVALID;
    CFE_TIME_Global.ClockSetState  = CFE_TIME_SetState_WAS_SET;
    CFE_TIME_Global.ServerFlyState = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_ToneUpdate();
    UtAssert_INT32_EQ(CFE_TIME_Global.ClockSetState, CFE_TIME_SetState_NOT_SET);
    UtAssert_INT32_EQ(CFE_TIME_Global.ServerFlyState, CFE_TIME_FlywheelState_NO_FLY);

    /* Test tone update using FLYWHEEL as the pending state */
    UT_InitData();
    CFE_TIME_Global.PendingState   = CFE_TIME_ClockState_FLYWHEEL;
    CFE_TIME_Global.ServerFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_ToneUpdate();
    UtAssert_INT32_EQ(CFE_TIME_Global.ServerFlyState, CFE_TIME_FlywheelState_IS_FLY);

#else
    UtAssert_NA("*Not tested* CFE_TIME_ToneUpdate() Invalid pending state, requires client configuration");
    UtAssert_NA("*Not tested* CFE_TIME_ToneUpdate() Pending state is FLYWHEEL, requires client configuration");
#endif
}

/*
** Test the 1Hz STCF adjustment, local interrupt service request and task
** (non-tone), and tone task functions
*/
void Test_1Hz(void)
{
    uint16                              Arg;
    uint16                              i;
    uint32                              delSec = 3;
    CFE_TIME_SysTime_t                  time1;
    CFE_TIME_SysTime_t                  time2;
    volatile CFE_TIME_ReferenceState_t *RefState;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    /* Test 1Hz STCF adjustment in positive direction */
    UT_InitData();
    RefState                        = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneSTCF.Seconds    = 20;
    RefState->AtToneSTCF.Subseconds = 60;
    CFE_TIME_FinishReferenceUpdate(RefState);
    time1.Seconds    = 10;
    time1.Subseconds = 30;
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_AdjustDirection_ADD);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UtAssert_UINT32_EQ(RefState->AtToneSTCF.Seconds, 30);
    UtAssert_UINT32_EQ(RefState->AtToneSTCF.Subseconds, 90);

    /* Test 1Hz STCF adjustment in negative direction */
    UT_InitData();
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_AdjustDirection_SUBTRACT);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UtAssert_UINT32_EQ(RefState->AtToneSTCF.Seconds, 20);
    UtAssert_UINT32_EQ(RefState->AtToneSTCF.Subseconds, 60);
#else
    /* These prevent missing tests when CFE_PLATFORM_TIME_CFG_SERVER is false */
    UtAssert_NA("(*Not tested*) Positive adjustment");
    UtAssert_NA("(*Not tested*) Negative adjustment");
#endif

    /* Test local 1Hz interrupt when enough time has elapsed (seconds) since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    RefState                               = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState                = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.AutoStartFly           = false;
    RefState->AtToneLatch.Seconds          = 1;
    RefState->AtToneLatch.Subseconds       = 0;
    CFE_TIME_Global.OneHzAdjust.Seconds    = 1;
    CFE_TIME_Global.OneHzAdjust.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Global.MaxLocalClock.Seconds    = CFE_PLATFORM_TIME_CFG_LATCH_FLY - 1;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    CFE_TIME_Local1HzStateMachine();
    UtAssert_BOOL_TRUE(CFE_TIME_Global.AutoStartFly);

    /* Test local 1Hz interrupt when enough time has elapsed since receiving a
     * time update to automatically update the MET
     */
    UT_InitData();
    time1.Seconds                          = 11;
    time2.Seconds                          = 1;
    RefState                               = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneDelay.Seconds          = 0;
    RefState->AtToneDelay.Subseconds       = 0;
    RefState->AtToneMET.Seconds            = time1.Seconds;
    RefState->AtToneMET.Subseconds         = 0;
    CFE_TIME_Global.OneHzAdjust.Seconds    = 0;
    CFE_TIME_Global.OneHzAdjust.Subseconds = 0;
    RefState->AtToneLatch.Seconds          = time2.Seconds;
    RefState->AtToneLatch.Subseconds       = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Global.MaxLocalClock.Seconds    = CFE_PLATFORM_TIME_CFG_LATCH_FLY + delSec;
    CFE_TIME_Global.MaxLocalClock.Subseconds = 0;
    RefState->ClockFlyState                  = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UtAssert_UINT32_EQ(RefState->AtToneMET.Seconds,
                       time1.Seconds + CFE_PLATFORM_TIME_CFG_LATCH_FLY + delSec - time2.Seconds);
    UtAssert_UINT32_EQ(RefState->AtToneLatch.Seconds, 0);

    /* Test the tone signal ISR when the tone doesn't occur ~1 second after
     * the previous one
     */
    UT_InitData();
    CFE_TIME_Global.IsToneGood = true;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Global.ToneSignalLatch.Seconds    = 1;
    CFE_TIME_Global.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UtAssert_BOOL_FALSE(CFE_TIME_Global.IsToneGood);

    /* Test the tone signal ISR call to the application synch callback
     * function by verifying the number of callbacks made matches the number
     * of registered callbacks
     */
    UT_InitData();
    UT_SetBSP_Time(1, 0);
    /* Set up the sync callback table with callbacks for 3 apps */
    for (i = 0; i < (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])); i++)
    {
        if (i < 3)
        {
            CFE_TIME_Global.SynchCallback[i].Ptr = &ut_time_MyCallbackFunc;
        }
        else
        {
            CFE_TIME_Global.SynchCallback[i].Ptr = NULL;
        }
    }
    ut_time_CallbackCalled = 0;
    CFE_TIME_Tone1HzISR();
    UtAssert_INT32_EQ(ut_time_CallbackCalled, 3);

    /* Test the local 1Hz ISR */
    UT_InitData();
    CFE_TIME_Global.LocalIntCounter = 1;
    UtAssert_VOIDCALL(CFE_TIME_Local1HzISR());
    UtAssert_STUB_COUNT(OS_BinSemGive, 1);
    UtAssert_UINT32_EQ(CFE_TIME_Global.LocalIntCounter, 2);

    /* Test the local 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_Global.AutoStartFly     = true;
    CFE_TIME_Global.LocalTaskCounter = 0;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 2, OS_ERROR);
    CFE_TIME_Local1HzTask();
    UtAssert_UINT32_EQ(CFE_TIME_Global.LocalTaskCounter, 1);

    /* Test the tone 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_Global.ToneTaskCounter = 0;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 2, OS_ERROR);
    CFE_TIME_Tone1HzTask();
    UtAssert_UINT32_EQ(CFE_TIME_Global.ToneTaskCounter, 1);

    /* Test the tone 1Hz task with the tone signal over the time limit */
    UT_InitData();
    UT_SetBSP_Time(1, CFE_TIME_Sub2MicroSecs(CFE_TIME_Global.ToneOverLimit));
    CFE_TIME_Global.IsToneGood                 = true;
    CFE_TIME_Global.MaxLocalClock.Seconds      = 0;
    CFE_TIME_Global.MaxLocalClock.Subseconds   = 0;
    CFE_TIME_Global.ToneSignalLatch.Seconds    = 0;
    CFE_TIME_Global.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UtAssert_BOOL_FALSE(CFE_TIME_Global.IsToneGood);

    /* Test the tone 1Hz task with the tone signal within the time limits */
    UT_InitData();
    UT_SetBSP_Time(0, CFE_TIME_Sub2MicroSecs(CFE_TIME_Global.ToneUnderLimit) + 1);
    CFE_TIME_Global.IsToneGood                 = false;
    CFE_TIME_Global.ToneSignalLatch.Seconds    = 0;
    CFE_TIME_Global.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UtAssert_BOOL_TRUE(CFE_TIME_Global.IsToneGood);

    /* Test the tone 1Hz task with the tone signal under the time limit */
    UT_InitData();
    UT_SetBSP_Time(0, CFE_TIME_Sub2MicroSecs(CFE_TIME_Global.ToneUnderLimit) - 1);
    CFE_TIME_Global.IsToneGood                 = true;
    CFE_TIME_Global.ToneSignalLatch.Seconds    = 0;
    CFE_TIME_Global.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UtAssert_BOOL_FALSE(CFE_TIME_Global.IsToneGood);

    /* Test local 1Hz interrupt when enough time has elapsed (subseconds) since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    RefState                               = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState                = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.AutoStartFly           = false;
    RefState->AtToneLatch.Seconds          = 1;
    RefState->AtToneLatch.Subseconds       = 0;
    CFE_TIME_Global.OneHzAdjust.Seconds    = 0;
    CFE_TIME_Global.OneHzAdjust.Subseconds = 1;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzStateMachine();
    UtAssert_BOOL_TRUE(CFE_TIME_Global.AutoStartFly);

    /* Test local 1Hz interrupt when enough time has not elapsed since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    RefState                               = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState                = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.OneHzAdjust.Seconds    = 0;
    CFE_TIME_Global.OneHzAdjust.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(1, 0);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UtAssert_INT32_EQ(RefState->ClockFlyState, CFE_TIME_FlywheelState_NO_FLY);

    /* Test the local 1Hz task where auto start flywheel is disabled */
    UT_InitData();
    CFE_TIME_Global.LocalTaskCounter = 0;
    CFE_TIME_Global.AutoStartFly     = false;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 2, OS_ERROR);
    CFE_TIME_Local1HzTask();
    RefState = CFE_TIME_GetReferenceState();
    UtAssert_INT32_EQ(RefState->ClockFlyState, CFE_TIME_FlywheelState_NO_FLY);
    CFE_UtAssert_EVENTNOTSENT(CFE_TIME_FLY_ON_EID);

    /* Test the CFE_TIME_Local1HzTimerCallback function */
    UT_InitData();
    CFE_TIME_Global.LocalIntCounter        = 1;
    RefState                               = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState                = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_Global.OneHzAdjust.Seconds    = 0;
    CFE_TIME_Global.OneHzAdjust.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzTimerCallback(OS_ObjectIdFromInteger(123), &Arg);
    UtAssert_UINT32_EQ(CFE_TIME_Global.LocalIntCounter, 2);
}

/*
** Test unregistering synchronization callback function
*/
void Test_UnregisterSynchCallback(void)
{
    uint32 i = 0;
    uint32 AppIndex;

    ut_time_CallbackCalled = 0;

    UtPrintf("Begin Test Unregister Callback");

    /* Test unregistering the callback function with NULL argument */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(NULL), CFE_TIME_BAD_ARGUMENT);

    /* Unregister callback function one too many times to test both valid and
     * invalid cases
     */
    UT_InitData();

    /* Set up the sync callback table with callbacks for 3 apps */
    for (i = 0; i < (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])); i++)
    {
        if (i < 3)
        {
            CFE_TIME_Global.SynchCallback[i].Ptr = &ut_time_MyCallbackFunc;
        }
        else
        {
            CFE_TIME_Global.SynchCallback[i].Ptr = NULL;
        }
    }

    /* App ID 4 should not have a callback */
    AppIndex = 4;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc), CFE_TIME_CALLBACK_NOT_REGISTERED);

    /*
     * One callback per application is allowed; the first should succeed,
     * the second should fail.
     */
    /* App ID 2 should have a callback */
    AppIndex = 2;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);

    CFE_UtAssert_SUCCESS(CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc));

    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc), CFE_TIME_CALLBACK_NOT_REGISTERED);

    /* Test unregistering the callback function with a bad application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, -1);
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc), -1);

    /* CFE_ES_AppID_ToIndex failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, -2);
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc), -2);

    /* App Index outside bounds of SynchCallback array */
    AppIndex = (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0]));
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    UtAssert_INT32_EQ(CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc), CFE_TIME_CALLBACK_NOT_REGISTERED);

    /* Test tone notification with an invalid time synch application */
    UT_InitData();
    CFE_TIME_Global.IsToneGood = true;
    for (i = 0; i < (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])); i++)
    {
        CFE_TIME_Global.SynchCallback[i].Ptr = NULL;
    }
    CFE_TIME_NotifyTimeSynchApps();
    UtAssert_INT32_EQ(ut_time_CallbackCalled, 0);
}

/*
** Test function to free resources associated with an application
*/
void Test_CleanUpApp(void)
{
    uint16         i;
    uint16         Count;
    uint32         AppIndex;
    CFE_ES_AppId_t TestAppId;

    UtPrintf("Begin Test Cleanup App");

    UT_InitData();
    CFE_ES_GetAppID(&TestAppId);

    /* Clear out the sync callback table */
    for (i = 0; i < (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])); i++)
    {
        CFE_TIME_Global.SynchCallback[i].Ptr = NULL;
    }

    /* Add callbacks for 3 apps into callback registry table */
    AppIndex = 1;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    AppIndex = 2;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    AppIndex = 3;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);

    /* Clean up an app which did not have a callback */
    AppIndex = 4;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    CFE_UtAssert_SUCCESS(CFE_TIME_CleanUpApp(TestAppId));

    Count = 0;
    for (i = 0; i < (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])); i++)
    {
        if (CFE_TIME_Global.SynchCallback[i].Ptr != NULL)
        {
            ++Count;
        }
    }

    /* should not have affected the callback table */
    UtAssert_UINT32_EQ(Count, 3);

    /* Clean up an app which did have a callback */
    AppIndex = 2;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    CFE_UtAssert_SUCCESS(CFE_TIME_CleanUpApp(TestAppId));

    Count = 0;
    for (i = 0; i < (sizeof(CFE_TIME_Global.SynchCallback) / sizeof(CFE_TIME_Global.SynchCallback[0])); i++)
    {
        if (CFE_TIME_Global.SynchCallback[i].Ptr != NULL)
        {
            ++Count;
        }
    }

    UtAssert_UINT32_EQ(Count, 2);

    /* Test response to a bad application ID -
     * This is effectively a no-op but here for coverage */
    AppIndex = 99999;
    UT_SetDataBuffer(UT_KEY(CFE_ES_AppID_ToIndex), &AppIndex, sizeof(AppIndex), false);
    UtAssert_INT32_EQ(CFE_TIME_CleanUpApp(CFE_ES_APPID_UNDEFINED), CFE_TIME_CALLBACK_NOT_REGISTERED);

    /* CFE_ES_AppID_ToIndex failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_AppID_ToIndex), 1, -2);
    UtAssert_INT32_EQ(CFE_TIME_CleanUpApp(CFE_ES_APPID_UNDEFINED), -2);
}
