/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

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
extern uint32              UT_AppID;

const char *TIME_SYSLOG_MSGS[] =
{
        NULL,
        "TIME:Error reading cmd pipe,RC=0x%08X\n",
        "TIME:Application Init Failed,RC=0x%08X\n",
        "TIME:1Hz OS_TimerAdd failed:RC=0x%08X\n",
        "TIME:1Hz OS_TimerSet failed:RC=0x%08X\n"
};

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_HK =
{
        .MsgId = CFE_TIME_SEND_HK_MID
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_TONE_CMD =
{
        .MsgId = CFE_TIME_TONE_CMD_MID
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_DATA_CMD =
{
        .MsgId = CFE_TIME_DATA_CMD_MID
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_1HZ_CMD =
{
        .MsgId = CFE_TIME_1HZ_CMD_MID
};

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_SEND_CMD =
{
        .MsgId = CFE_TIME_SEND_CMD_MID
};
#endif

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_NOOP_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_NOOP_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_RESET_COUNTERS_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_RESET_COUNTERS_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SEND_DIAGNOSTIC_TLM_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SEND_DIAGNOSTIC_TLM_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STATE_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SET_STATE_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SET_SOURCE_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SET_SIGNAL_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_ADD_DELAY_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_DELAY_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SUB_DELAY_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_TIME_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SET_TIME_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_MET_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SET_MET_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_STCF_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SET_STCF_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SET_LEAP_SECONDS_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_ADD_ADJUST_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_ADJUST_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SUB_ADJUST_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_ADD_1HZ_ADJUSTMENT_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_ADD_1HZ_ADJUSTMENT_CC
};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TIME_CMD_SUB_1HZ_ADJUSTMENT_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = CFE_TIME_SUB_1HZ_ADJUSTMENT_CC
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_TIME_INVALID_MID =
{
        .MsgId = 0xFFFF,
        .CommandCode = 0
};
static const UT_TaskPipeDispatchId_t  UT_TPID_CFE_TIME_CMD_INVALID_CC =
{
        .MsgId = CFE_TIME_CMD_MID,
        .CommandCode = 0x7F
};


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
void OS_SelectTone(int16 Signal)
{
}
#endif

void UtTest_Setup(void)
{
    /* Initialize unit test */
    UT_Init("time");
    UT_Text("cFE TIME Unit Test Output File\n\n");

    UT_ADD_TEST(Test_Main);
    UT_ADD_TEST(Test_Init);
    UT_ADD_TEST(Test_GetTime);
    UT_ADD_TEST(Test_TimeOp);
    UT_ADD_TEST(Test_ConvertTime);
    UT_ADD_TEST(Test_ConvertCFEFS);
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
#ifdef UT_VERBOSE
    UT_Text("Begin Test Main\n");
#endif

    /* Test successful run through (a pipe read error is expected) */
    UT_InitData();
    CFE_TIME_TaskMain();
    UT_Report(__FILE__, __LINE__,
              UT_SyslogIsInHistory(TIME_SYSLOG_MSGS[1]),
              "CFE_TIME_TaskMain",
              "Command pipe read error");

    /* Test with an application initialization failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterApp),1, -1 );
    CFE_TIME_TaskMain();
    UT_Report(__FILE__, __LINE__,
              UT_SyslogIsInHistory(TIME_SYSLOG_MSGS[2]),
              "CFE_TIME_TaskMain",
              "Application initialization fail");
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

#ifdef UT_VERBOSE
    UT_Text("Begin Test Init\n");
#endif

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
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_SB_SetMsgId)) == ExpRtn,
              "CFE_TIME_EarlyInit",
              "Successful");

    /* Test successful time task initialization */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == CFE_SUCCESS,
              "CFE_TIME_Task_Init",
              "Successful");

    /* Test response to a failure to register an application */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterApp), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Register application failure");

    /* Test response to a failure creating the first child task */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Child task 1 create failure");

    /*  Test response to a failure creating the second child task */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CreateChildTask), 2, -3);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -3,
              "CFE_TIME_Task_Init",
              "Child task 2 create failure");

    /* Test response to an error creating a command pipe */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Create pipe failure");

    /* Test response to failure of the HK request subscription */
    UT_InitData();
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -SubErrCnt,
              "CFE_TIME_Task_Init",
              "HK request subscription failure");

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

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Tone commands subscription failure (client)");

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

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Time data command subscription failure");

    /* Test response to failure of the fake tone signal commands
     * subscription
     */
    UT_InitData();
    ExpRtn = 0;

#if (CFE_MISSION_TIME_CFG_FAKE_TONE == true)
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

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Fake tone signal commands subscription failure");
#else
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_Task_Init",
              "*Not tested* Fake tone signal commands subscription failure");
#endif

    /* Test response to failure of the time at tone signal commands
     * subscription
     */
    UT_InitData();
#if (CFE_PLATFORM_TIME_CFG_SERVER == true && CFE_PLATFORM_TIME_CFG_SOURCE != true && \
     CFE_MISSION_TIME_CFG_FAKE_TONE != true)
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    ExpRtn = -SubErrCnt;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Time at tone signal commands subscription failure");
#else
    SubErrCnt++;
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_Task_Init",
              "*Not tested* Time at tone signal commands subscription failure");
#endif

    /* Test response to failure of the time task ground commands
     * subscription
     */
    UT_InitData();
    SubErrCnt++;
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), SubErrCnt, -SubErrCnt);
    ExpRtn = -SubErrCnt;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == ExpRtn,
              "CFE_TIME_Task_Init",
              "Time task ground commands subscription failure");

    /* Test response to failure creating a tone semaphore */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Tone semaphore create failure");

    /* Test response to failure creating a local semaphore */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 2, -2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -2,
              "CFE_TIME_Task_Init",
              "Local semaphore create failure");

    /* Test response to an EVS register failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "EVS register failure");

    /* Test response to an error sending an initialization event */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, -1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskInit() == -1,
              "CFE_TIME_Task_Init",
              "Send initialization event error");

    /* Test response to a failure to get the ID by name */
    UT_InitData();
    UT_SetForceFail(UT_KEY(OS_TimeBaseGetIdByName), OS_ERROR);
    CFE_TIME_TaskInit();
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0,
              "CFE_TIME_Task_Init",
              "Get ID by name failure");

    /* Test response to an error setting up the 1Hz callback.
     * Note that this is only a SysLog message, it does not return the
     * error.  This allows the overall system to continue without the 1Hz
     */
    UT_InitData();
    UT_SetForceFail(UT_KEY(OS_TimerAdd), OS_ERROR);
    CFE_TIME_TaskInit();
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1 &&
              UT_SyslogIsInHistory(TIME_SYSLOG_MSGS[3]),
              "CFE_TIME_Task_Init",
              "1Hz OS_TimerAdd failure");

    UT_InitData();
    UT_SetForceFail(UT_KEY(OS_TimerSet), OS_ERROR);
    CFE_TIME_TaskInit();
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1 && UT_SyslogIsInHistory((TIME_SYSLOG_MSGS[4])),
              "CFE_TIME_Task_Init",
              "1Hz OS_TimerSet failure");
}

/*
** Test functions to retrieve time values
*/
void Test_GetTime(void)
{
    int result;
    uint16 StateFlags, ActFlags;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    char timeBuf[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    /* Note: Time is in seconds + microseconds since 1980-001-00:00:00:00000 */
    /* The time below equals 2013-001-02:03:04.56789 */
    int seconds = 1041472984;
    int microsecs = 567890;
    int actual;
    const char *expectedMET = "2013-001-02:03:14.56789";
    const char *expectedTAI = "2013-001-03:03:14.56789";
    const char *expectedUTC = "2013-001-03:02:42.56789";
    const char *expectedSTCF = "1980-001-01:00:00.00000";
    volatile CFE_TIME_ReferenceState_t *RefState;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Time\n");
#endif

    CFE_TIME_TaskData.LastVersionCounter = 0x1000;

    /* Test successfully retrieving the mission elapsed time */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds = 20; /* 20.00000 */
    RefState->AtToneMET.Subseconds = 0;
    RefState->AtToneSTCF.Seconds = 3600; /* 01:00:00.00000 */
    RefState->AtToneSTCF.Subseconds = 0;
    RefState->AtToneLeapSeconds = 32;
    RefState->AtToneDelay.Seconds = 0; /* 0.00000 */
    RefState->AtToneDelay.Subseconds = 0;
    RefState->AtToneLatch.Seconds = 10; /* 10.00000 */
    RefState->AtToneLatch.Subseconds = 0;
    RefState->ClockSetState = CFE_TIME_SetState_NOT_SET; /* Force invalid time */
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_Print(timeBuf, CFE_TIME_GetMET());
    result = !strcmp(timeBuf, expectedMET);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedMET, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetMET",
              testDesc);

    /* Test successfully retrieving the mission elapsed time (seconds
     * portion)
     */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    actual = CFE_TIME_GetMETseconds();
    result = seconds + RefState->AtToneMET.Seconds -
            RefState->AtToneLatch.Seconds;
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d", result, actual);
    UT_Report(__FILE__, __LINE__,
              result == actual,
              "CFE_TIME_GetMETseconds",
              testDesc);

    /* Test successfully retrieving the mission elapsed time (sub-seconds
     * portion)
     */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    actual = CFE_TIME_GetMETsubsecs();
    result = CFE_TIME_Micro2SubSecs(microsecs) +
            RefState->AtToneMET.Subseconds -
            RefState->AtToneLatch.Subseconds;
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d", result, actual);
    UT_Report(__FILE__, __LINE__,
              result == actual,
              "CFE_TIME_GetMETsubsecs",
              testDesc);

    /* Test successfully retrieving the leap seconds */
    UT_InitData();
    actual = CFE_TIME_GetLeapSeconds();
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %d, actual = %d",
             RefState->AtToneLeapSeconds, actual);
    UT_Report(__FILE__, __LINE__,
              actual == RefState->AtToneLeapSeconds,
              "CFE_TIME_GetLeapSeconds",
              testDesc);

    /* Test successfully retrieving the international atomic time (TAI) */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    CFE_TIME_Print(timeBuf, CFE_TIME_GetTAI());
    result = !strcmp(timeBuf, expectedTAI);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedTAI, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetTAI",
              testDesc);

    /* Test successfully retrieving the coordinated universal time (UTC) */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    CFE_TIME_Print(timeBuf, CFE_TIME_GetUTC());
    result = !strcmp(timeBuf, expectedUTC);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedUTC, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetUTC",
              testDesc);

    /* Test successfully retrieving the default time (UTC or TAI) */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    CFE_TIME_Print(timeBuf, CFE_TIME_GetTime());

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)
    result = !strcmp(timeBuf, expectedTAI);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "(Default = TAI) Expected = %s, actual = %s",
             expectedTAI, timeBuf);
#else
    result = !strcmp(timeBuf, expectedUTC);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "(Default = UTC) Expected = %s, actual = %s",
             expectedUTC, timeBuf);
#endif
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetTime",
              testDesc);

    /* Test successfully retrieving the spacecraft time correlation
     * factor (SCTF)
     */
    UT_InitData();
    UT_SetBSP_Time(seconds, microsecs);
    CFE_TIME_Print(timeBuf, CFE_TIME_GetSTCF());
    result = !strcmp(timeBuf, expectedSTCF);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedSTCF, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_GetSTCF",
              testDesc);

    /* Test retrieving the time status (invalid time is expected) */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_GetClockState() == CFE_TIME_ClockState_INVALID,
              "CFE_TIME_GetClockState",
              "Invalid time");


    /* Test alternate flag values */
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockSetState = CFE_TIME_SetState_NOT_SET;
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.Forced2Fly = false;
    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_AdjustDirection_SUBTRACT;
    CFE_TIME_TaskData.OneHzDirection = CFE_TIME_AdjustDirection_SUBTRACT;
    RefState->DelayDirection = CFE_TIME_AdjustDirection_SUBTRACT;
    CFE_TIME_TaskData.IsToneGood = false;
    CFE_TIME_FinishReferenceUpdate(RefState);
    ActFlags = CFE_TIME_GetClockInfo();
    StateFlags = 0;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    StateFlags |= CFE_TIME_FLAG_SERVER;
#endif

    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = 0x%04X, actual = 0x%04X", StateFlags, ActFlags);
    UT_Report(__FILE__, __LINE__,
              ActFlags == StateFlags,
              "CFE_TIME_GetClockInfo",
              testDesc);

    /* Test successfully converting the clock state data to flag values */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockSetState = CFE_TIME_SetState_WAS_SET;
    RefState->ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;
    RefState->DelayDirection = CFE_TIME_AdjustDirection_ADD;
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_ToneSignalSelect_PRIMARY;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_TaskData.Forced2Fly = true;
    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_AdjustDirection_ADD;
    CFE_TIME_TaskData.OneHzDirection = CFE_TIME_AdjustDirection_ADD;
    CFE_TIME_TaskData.IsToneGood = true;

    StateFlags = CFE_TIME_FLAG_CLKSET |
                 CFE_TIME_FLAG_FLYING |
                 CFE_TIME_FLAG_SRCINT |
                 CFE_TIME_FLAG_SIGPRI |
                 CFE_TIME_FLAG_SRVFLY |
                 CFE_TIME_FLAG_CMDFLY |
                 CFE_TIME_FLAG_ADD1HZ |
                 CFE_TIME_FLAG_ADDADJ |
                 CFE_TIME_FLAG_ADDTCL |
                 CFE_TIME_FLAG_GDTONE;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    StateFlags |= CFE_TIME_FLAG_SERVER;
#endif

    ActFlags = CFE_TIME_GetClockInfo();
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = 0x%04X, actual = 0x%04X", StateFlags, ActFlags);
    UT_Report(__FILE__, __LINE__,
              ActFlags == StateFlags,
              "CFE_TIME_GetClockInfo",
              testDesc);
}

/*
** Test operations on time (add, subtract, compare)
*/
void Test_TimeOp(void)
{
    CFE_TIME_SysTime_t time1, time2, result, exp_result;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Time Operations\n");
#endif

    /* Initialize to zero time values */
    time1.Subseconds = 0;
    time1.Seconds = 0;
    time2.Subseconds = 0;
    time2.Seconds = 0;
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;

    /* Test adding with both times equal zero */
    UT_InitData();
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A = time B = 0 seconds/subseconds");

    /* Test subtracting with both times equal zero */
    UT_InitData();
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A = time B = 0 seconds/subseconds");

    /* Test comparing with both times equal zero */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_EQUAL,
              "CFE_TIME_Compare",
              "Time A = time B = 0 seconds/subseconds");

    /* Initialize to maximum time values */
    time1.Subseconds = 0xffffffff;
    time1.Seconds = 0xffffffff;
    time2.Subseconds = 0xffffffff;
    time2.Seconds = 0xffffffff;

    /* Test adding two maximum time values (extreme time rollover case) */
    UT_InitData();
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A = time B = maximum seconds/subseconds (rollover)");

    /* Test subtracting two maximum time values (zero result) */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A = time B = maximum seconds/subseconds (zero result)");

    /* Test comparing two maximum time values */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_EQUAL,
              "CFE_TIME_Compare",
              "Time A = time B = maximum seconds/subseconds");

    /* Initialize to single time value at maximum subseconds */
    time1.Subseconds = 0xffffffff;
    time1.Seconds = 0xffff0000;
    time2.Subseconds = 0x00000001;
    time2.Seconds = 0x0000ffff;

    /* Test adding two time values; time A > time B (minimal time
     * rollover case)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A > time B (rollover)");

    /* Test subtracting two time values; time A > time B */
    UT_InitData();
    exp_result.Subseconds = 0xfffffffe;
    exp_result.Seconds = 0xfffe0001;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A > time B");

    /* Test comparing two time values; time A > time B (assumes time has
     * rolled over)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A > time B");

    /* Test adding two time values; time A < time B */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A < time B");

    /* Test subtracting two time values; time A < time B (rollover) */
    UT_InitData();
    exp_result.Subseconds = 0x00000002;
    exp_result.Seconds = 0x0001fffe;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A < time B (rollover)");

    /* Test comparing two time values; time A < time B (assumes time has
     * rolled over)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time2, time1) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A < time B");

    /* Initialize so that only subseconds are different; seconds are
     * the same
     */
    time1.Subseconds = 30;
    time1.Seconds = 3;
    time2.Subseconds = 29;
    time2.Seconds = 3;

    /* Test adding two time values; time A subseconds > time B subseconds
     * (seconds same) */
    UT_InitData();
    exp_result.Subseconds = 59;
    exp_result.Seconds = 6;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A subseconds > time B subseconds (seconds same)");

    /* Test subtracting two time values; time A subseconds > time B
     * subseconds (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 1;
    exp_result.Seconds = 0;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A subseconds > time B subseconds (seconds same)");

    /* Test comparing two time values; time A subseconds > time B subseconds
     * (seconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A subseconds > time B subseconds (seconds same)");

    /* Test adding two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 59;
    exp_result.Seconds = 6;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A subseconds < time B subseconds (seconds same)");

    /* Test subtracting two time values; time A subseconds < time B
     * subseconds (seconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0xffffffff;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A subseconds < time B subseconds (seconds same)");

    /* Test comparing two time values; time A subseconds < time B subseconds
     * (seconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time2, time1) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A subseconds < time B subseconds (seconds same)");

    /* Initialize so that only seconds are different; subseconds are
     * the same
     */
    time1.Subseconds = 18;
    time1.Seconds = 8;
    time2.Subseconds = 18;
    time2.Seconds = 7;

    /* Test adding two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 36;
    exp_result.Seconds = 15;
    result = CFE_TIME_Add(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A seconds > time B seconds (subseconds same)");

    /* Test subtracting two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 1;
    result = CFE_TIME_Subtract(time1, time2);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A seconds > time B seconds (subseconds same)");

    /* Test comparing two time values; time A seconds > time B seconds
     * (subseconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time1, time2) == CFE_TIME_A_GT_B,
              "CFE_TIME_Compare",
              "Time A seconds > time B seconds (subseconds same)");

    /* Test adding two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 36;
    exp_result.Seconds = 15;
    result = CFE_TIME_Add(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Add",
              "Time A seconds < time B seconds (subseconds same)");

    /* Test subtracting two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    exp_result.Subseconds = 0;
    exp_result.Seconds = 0xffffffff;
    result = CFE_TIME_Subtract(time2, time1);
    UT_Report(__FILE__, __LINE__,
              memcmp(&result, &exp_result, sizeof(CFE_TIME_SysTime_t)) == 0,
              "CFE_TIME_Subtract",
              "Time A seconds < time B seconds (subseconds same)");

    /* Test comparing two time values; time A seconds < time B seconds
     * (subseconds same)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Compare(time2, time1) == CFE_TIME_A_LT_B,
              "CFE_TIME_Compare",
              "Time A seconds < time B seconds (subseconds same)");
}

/*
** Test time conversion functions
*/
void Test_ConvertTime(void)
{
    int result;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    char timeBuf[sizeof("yyyy-ddd-hh:mm:ss.xxxxx_")];
    CFE_TIME_SysTime_t METTime;
    volatile CFE_TIME_ReferenceState_t *RefState;

#if (CFE_MISSION_TIME_CFG_DEFAULT_TAI == true)
    /* TAI time derived = MET + STCF */
    const char *expectedSCTime = "1980-001-02:00:40.00000";
#else
    /* UTC time derived = MET + STCF - Leaps */
    const char *expectedSCTime = "1980-001-02:00:08.00000";
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test Convert Time\n");
#endif

    /* Test MET to SCTF conversion */
    UT_InitData();
    METTime.Seconds = 0;
    METTime.Subseconds = 0;
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneSTCF.Seconds = 7240; /* 01:00:00.00000 */
    RefState->AtToneSTCF.Subseconds = 0;
    RefState->AtToneLeapSeconds = 32;
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_Print(timeBuf, CFE_TIME_MET2SCTime(METTime));

    /* SC = MET + SCTF [- Leaps for UTC] */
    result = !strcmp(timeBuf, expectedSCTime);
    snprintf(testDesc, UT_MAX_MESSAGE_LENGTH,
             "Expected = %s, actual = %s", expectedSCTime, timeBuf);
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_MET2SCTime",
              testDesc);

    /* Test subseconds to microseconds conversion; zero subsecond value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0) == 0,
              "CFE_TIME_Sub2MicroSecs",
              "Convert 0 subsecond value");

    /* Test subseconds to microseconds conversion; positive microsecond
     * adjust
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0xffff) == 16,
              "CFE_TIME_Sub2MicroSecs",
              "+1 microsecond adjustment");

    /* Test subseconds to microseconds conversion; no microsecond adjust */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0x80000000) == 500000,
              "CFE_TIME_Sub2MicroSecs",
              "No microsecond adjustment");

    /* Test subseconds to microseconds conversion; negative microsecond
     * adjust
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0x80002000) == 500001,
              "CFE_TIME_Sub2MicroSecs",
              "-1 microsecond adjustment");

    /* Test subseconds to microseconds conversion; subseconds exceeds
     * microseconds limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Sub2MicroSecs(0xffffffff) == 999999,
              "CFE_TIME_Sub2MicroSecs",
              "Subseconds exceeds maximum microseconds value (limit ms)");

    /* Test microseconds to subseconds conversion; zero microsecond value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(0) == 0,
              "CFE_TIME_Micro2SubSecs",
              "Convert 0 microseconds to 0 subseconds");

    /* Test microseconds to subseconds conversion; no subsecond adjust */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(0xffff) == 281468928,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds below maximum value (no subsecond adjustment)");

    /* Test microseconds to subseconds conversion; microseconds below
     * maximum limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(999998) == 0xffffe000,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds below maximum value (subsecond adjustment)");

    /* Test microseconds to subseconds conversion; microseconds equals
     * maximum limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(999999) == 0xfffff000,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds equals maximum value (subsecond adjustment)");

    /* Test microseconds to subseconds conversion; microseconds exceeds
     * maximum limit
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_Micro2SubSecs(0xffffffff) == 0xffffffff,
              "CFE_TIME_Micro2SubSecs",
              "Microseconds exceeds maximum; set maximum subseconds value");
}

/*
** Test function for converting cFE seconds to file system (FS) seconds and
** vice versa
*/
void Test_ConvertCFEFS(void)
{
    uint32 result;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Convert cFE and FS Seconds\n");
#endif

    /* Test cFE to FS conversion using 0 for the cFE seconds value */
    UT_InitData();

    /* Calculate expected result based on macro value */
    if (CFE_MISSION_TIME_FS_FACTOR < 0 && -CFE_MISSION_TIME_FS_FACTOR > 0)
    {
        result = 0;
    }
    else
    {
        result = CFE_MISSION_TIME_FS_FACTOR;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CFE2FSSeconds(0) == result,
              "CFE_TIME_CFE2FSSeconds",
              "Convert 0 cFE seconds to FS seconds");

    /* Test cFE to FS conversion using mid-range value for cFE seconds */
    UT_InitData();

    /* Calculate expected result based on macro value */
    if (CFE_MISSION_TIME_FS_FACTOR < 0 && -CFE_MISSION_TIME_FS_FACTOR > 0xffff)
    {
        result = 0;
    }
    else
    {
        result = CFE_MISSION_TIME_FS_FACTOR + 0xffff;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CFE2FSSeconds(0xffff) == result,
              "CFE_TIME_CFE2FSSeconds",
              "Convert mid-range cFE seconds to FS seconds");

    /* Test cFE to FS conversion using the maximum cFE seconds value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CFE2FSSeconds(0xffffffff) ==
              (uint32) (CFE_MISSION_TIME_FS_FACTOR - 1),
              "CFE_TIME_CFE2FSSeconds",
              "Maximum cFE seconds value");

    /* Test FS to cFE conversion using 0 for the FS seconds value */
    UT_InitData();

    if (CFE_MISSION_TIME_FS_FACTOR > 0)
    {
        result = 0;
    }
    else
    {
        result = -(uint32) CFE_MISSION_TIME_FS_FACTOR;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(0) == result,
              "CFE_TIME_FS2CFESeconds",
              "Convert 0 FS seconds to cFE seconds");

    /* Test FS to cFE conversion response to a FS seconds value that results
     * in a negative cFE time (forces cFE seconds to zero)
     */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(CFE_MISSION_TIME_FS_FACTOR - 1) == 0,
              "CFE_TIME_FS2CFESeconds",
              "Negative cFE seconds conversion (force to zero)");

    /* Test FS to cFE conversion using the minimum convertible FS
     * seconds value
     */
    UT_InitData();

    if (CFE_MISSION_TIME_FS_FACTOR > (uint32) (CFE_MISSION_TIME_FS_FACTOR + 1))
    {
        result = 0;
    }
    else
    {
        result = 1;
    }

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(CFE_MISSION_TIME_FS_FACTOR + 1) == result,
              "CFE_TIME_FS2CFESeconds",
              "Minimum convertible FS seconds value");

    /* Test FS to cFE conversion using the maximum FS seconds value */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_FS2CFESeconds(0xffffffff) == 0xffffffff -
              CFE_MISSION_TIME_FS_FACTOR,
              "CFE_TIME_FS2CFESeconds",
              "Maximum FS seconds value");
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
    int result;
    char testDesc[UT_MAX_MESSAGE_LENGTH];
    CFE_TIME_SysTime_t time;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Print\n");
#endif

    /* Test with zero time value */
    UT_InitData();
    time.Subseconds = 0;
    time.Seconds = 0;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "1980-001-00:00:00.00000");
    strncat(testDesc," Zero time value", UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);

    /* Test with a time value that causes seconds >= 60 when
     * CFE_MISSION_TIME_EPOCH_SECOND > 0
     */
    UT_InitData();
    time.Subseconds = 0;
    time.Seconds = 59;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "1980-001-00:00:59.00000");
    strncat(testDesc,
            " Seconds overflow if CFE_MISSION_TIME_EPOCH_SECOND > 0",
            UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);

    /* Test with mission representative time values */
    UT_InitData();
    time.Subseconds = 215000;
    time.Seconds = 1041472984;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "2013-001-02:03:04.00005");
    strncat(testDesc," Mission representative time", UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);

    /* Test with maximum seconds and subseconds values */
    UT_InitData();
    time.Subseconds = 0xffffffff;
    time.Seconds = 0xffffffff;
    CFE_TIME_Print(testDesc, time);
    result = !strcmp(testDesc, "2116-038-06:28:15.99999");
    strncat(testDesc," Maximum seconds/subseconds values",
            UT_MAX_MESSAGE_LENGTH);
    testDesc[UT_MAX_MESSAGE_LENGTH - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              result,
              "CFE_TIME_Print",
              testDesc);
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
    int32   Result;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Register Synch Callback\n");
#endif

    /*
     * One callback per application is allowed; the first should succeed,
     * the second should fail.
     */

    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, -1);
    CFE_TIME_TaskData.SynchCallback[0].Ptr = NULL;

    Result = CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
          Result == -1,
              "CFE_TIME_RegisterSynchCallback",
              "Bad App ID");


    /* Test registering the callback function the maximum number of times,
     * then attempt registering one more time
     */
    UT_InitData();

    UT_SetAppID(1);

    /*
     * One callback per application is allowed; the first should succeed,
     * the second should fail.
     */
    Result = CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
          Result == CFE_SUCCESS,
              "CFE_TIME_RegisterSynchCallback",
              "Successfully registered callbacks");


    Result = CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
          Result == CFE_TIME_TOO_MANY_SYNCH_CALLBACKS,
              "CFE_TIME_RegisterSynchCallback",
              "Too Many registered callbacks");

    UT_SetAppID(2);

    Result = CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
          Result == CFE_SUCCESS,
              "CFE_TIME_RegisterSynchCallback",
              "Successfully registered callbacks");

    /*
     * This test case should not be possible in real flight as ES should never
     * return "success" with an appid out of range, but nonetheless
     * we need to make sure we do not overwrite our own memory here.
     */
    UT_SetAppID(CFE_PLATFORM_ES_MAX_APPLICATIONS);
    Result = CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
            Result == CFE_TIME_TOO_MANY_SYNCH_CALLBACKS,
              "CFE_TIME_RegisterSynchCallback",
              "App ID out of range");

}

/*
** Test external tone signal detection
*/
void Test_ExternalTone(void)
{
#ifdef UT_VERBOSE
    UT_Text("Begin Test External Tone\n");
#endif

    UT_InitData();
    UT_SetBSP_Time(123, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_ExternalTone();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneSignalLatch.Seconds == 123 &&
              CFE_TIME_TaskData.ToneSignalLatch.Subseconds == 0,
              "CFE_TIME_ExternalTone",
              "External tone");
}

/*
** Test setting time data from an external source
*/
void Test_External(void)
{
#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true || \
     CFE_PLATFORM_TIME_CFG_SRC_GPS == true || \
     CFE_PLATFORM_TIME_CFG_SRC_TIME == true)

    CFE_TIME_SysTime_t settime = {5, 4};
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test External Time Set\n");
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_MET == true)
    /* Test setting time data from MET using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_NOT_SET;
    CFE_TIME_TaskData.ExternalCount = 0;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and clock state not set");

    /* Test setting time data from MET using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 10;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and time out of range (low)");

    /* Test setting time data from MET using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 20;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 10;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and time out of range (high)");

    /* Test setting time data from MET (external source, state set) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 10;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 10;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET - external source and time in range");

    /* Test setting time data from MET (internal source) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_ExternalMET(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalMET",
              "External MET (internal source)");
#else
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and clock state "
                "not set");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and time out of "
                "range (low)");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and time out of "
                "range (high)");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET - external source and time in range");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalMET",
              "*Not tested* External MET (internal source)");
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_GPS == true)
    /* Test setting time data from GPS using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_NOT_SET;
    CFE_TIME_TaskData.ExternalCount = 0;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and clock state not set");

    /* Test setting time data from GPS using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and time out of range (low)");

    /* Test setting time data from GPS using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 20;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and time out of range (high)");

    /* Test setting time data from GPS (external source, state set) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 10;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS - external source and time in range");

    /* Test setting time data from GPS (internal source) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_ExternalGPS(settime, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalGPS",
              "External GPS (internal source)");
#else
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS - external source and clock state "
                "not set");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalGPS",
              "Not tested* External GPS - external source and time out of "
                "range (low)");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS - external source and time out of "
                "range (high)");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS - external source and time in range");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalGPS",
              "*Not tested* External GPS (internal source)");
#endif

#if (CFE_PLATFORM_TIME_CFG_SRC_TIME == true)
    /* Test setting time data from Time using an external source with the clock
     * state not set
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_NOT_SET;
    CFE_TIME_TaskData.ExternalCount = 0;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and clock state not set");

    /* Test setting time data from Time using an external source with the clock
     * state set and time less than the minimum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 0;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and time out of range (low)");

    /* Test setting time data from Time using an external source with the clock
     * state set and time greater than the maximum
     */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 20;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and time out of range (high)");

    /* Test setting time data from Time (external source, state set) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSetState = CFE_TIME_SetState_WAS_SET;
    settime.Seconds = 10;
    settime.Subseconds = 0;
    CFE_TIME_TaskData.ExternalCount = 0;
    CFE_TIME_TaskData.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.AtToneSTCF.Seconds = 10;
    CFE_TIME_TaskData.AtToneSTCF.Subseconds = 0;
   CFE_TIME_TaskData.AtToneDelay.Seconds = 0;
    CFE_TIME_TaskData.AtToneDelay.Subseconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Seconds = 0;
    CFE_TIME_TaskData.AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxDelta.Seconds = 0;
    CFE_TIME_TaskData.MaxDelta.Subseconds = 1;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ExternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time - external source and time in range");

    /* Test setting time data from Time (internal source) */
    UT_InitData();
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
    CFE_TIME_TaskData.InternalCount = 0;
    CFE_TIME_ExternalTime(settime);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.InternalCount == 1,
              "CFE_TIME_ExternalTime",
              "External Time (internal source)");
#else
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time - external source and clock state "
                "not set");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalTime",
              "Not tested* External Time - external source and time out of "
                "range (low)");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time - external source and time out of "
                "range (high)");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time - external source and time in range");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ExternalTime",
              "*Not tested* External Time (internal source)");
#endif

    /* Reset to normal value for subsequent tests */
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
}

/*
** Test processing command pipe messages
*/
void Test_PipeCmds(void)
{
    union
    {
        CFE_SB_Msg_t message;
        CCSDS_CommandPacket_t cmd;
        CFE_TIME_ToneDataCmd_t tonedatacmd;
        CFE_TIME_Noop_t noopcmd;
        CFE_TIME_ResetCounters_t resetcountercmd;
        CFE_TIME_SendDiagnosticTlm_t diagtlmcmd;
        CFE_TIME_SetState_t statecmd;
        CFE_TIME_SetSource_t sourcecmd;
        CFE_TIME_SetSignal_t signalcmd;
        CFE_TIME_AddDelay_t adddelaycmd;
        CFE_TIME_SubDelay_t subdelaycmd;
        CFE_TIME_SetTime_t settimecmd;
        CFE_TIME_SetMET_t setmetcmd;
        CFE_TIME_SetSTCF_t setstcfcmd;
        CFE_TIME_SetLeapSeconds_t leapscmd;
        CFE_TIME_AddAdjust_t addadjcmd;
        CFE_TIME_SubAdjust_t subadjcmd;
        CFE_TIME_Add1HZAdjustment_t add1hzadjcmd;
        CFE_TIME_Sub1HZAdjustment_t sub1hzadjcmd;
    } CmdBuf;

    UT_SoftwareBusSnapshot_Entry_t LocalSnapshotData =
    {
            .MsgId = CFE_TIME_HK_TLM_MID
    };

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    uint32 count;
#endif

#ifdef UT_VERBOSE
    UT_Text("Begin Test Pipe Commands\n");
#endif

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    /* Test sending the housekeeping telemetry request command */
    UT_InitData();
    UT_SetHookFunction(UT_KEY(CFE_SB_SendMsg), UT_SoftwareBusSnapshotHook, &LocalSnapshotData);
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd),
            UT_TPID_CFE_TIME_SEND_HK);
    UT_Report(__FILE__, __LINE__,
              LocalSnapshotData.Count == 1,
              "CFE_TIME_HousekeepingCmd",
              "Housekeeping telemetry request");

    /* Test sending the time at the tone "signal" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.ToneSignalCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd),
            UT_TPID_CFE_TIME_TONE_CMD);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneSignalCounter == 1,
              "CFE_TIME_ToneSignalCmd",
              "Time at tone signal");

    /* Test sending the time at the tone "data" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.ToneDataCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.tonedatacmd),
            UT_TPID_CFE_TIME_DATA_CMD);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCounter == 1,
              "CFE_TIME_ToneDataCmd",
              "Time at tone data");

    /* Test sending the simulate time at the tone "signal" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetBSP_Time(123, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneSignalLatch.Seconds == 123 &&
              CFE_TIME_TaskData.ToneSignalLatch.Subseconds == 0,
              "CFE_TIME_FakeToneCmd",
              "Simulated time at tone signal");

    /* Test sending the request time at the tone "data" command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    count = CFE_TIME_TaskData.InternalCount;
#endif

    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd),
            UT_TPID_CFE_TIME_SEND_CMD);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              !UT_EventIsInHistory(CFE_TIME_ID_ERR_EID) &&
              CFE_TIME_TaskData.InternalCount == count + 1,
              "CFE_TIME_ToneSendCmd",
              "Request time at tone data");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_ID_ERR_EID),
              "CFE_TIME_ToneSendCmd",
              "*Command not available* Request time at tone data");
#endif

    /* Test sending the no-op command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd),
            UT_TPID_CFE_TIME_CMD_NOOP_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_NOOP_EID),
              "CFE_TIME_NoopCmd",
              "No-op");

    /* Test sending the reset counters command */
    UT_InitData();
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd),
            UT_TPID_CFE_TIME_CMD_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_RESET_EID),
              "CFE_TIME_ResetCountersCmd",
              "Reset counters");

    /* Test sending the request diagnostics command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd),
            UT_TPID_CFE_TIME_CMD_SEND_DIAGNOSTIC_TLM_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DIAG_EID),
              "CFE_TIME_DiagCmd",
              "Request diagnostics");

    /* Test sending a clock state = invalid command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = CFE_TIME_ClockState_INVALID;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd),
            UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_STATE_EID),
              "CFE_TIME_SetStateCmd",
              "Set clock state = invalid");

    /* Test sending a clock state = valid command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = CFE_TIME_ClockState_VALID;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd),
            UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_STATE_EID),
              "CFE_TIME_SetStateCmd",
              "Set clock state = valid");

    /* Test sending a clock state = flywheel command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = CFE_TIME_ClockState_FLYWHEEL;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd),
            UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_STATE_EID),
              "CFE_TIME_SetStateCmd",
              "Set clock state = flywheel");

    /* Test response to sending a clock state command using an
     * invalid state
     */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.statecmd.Payload.ClockState = 99;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.statecmd),
            UT_TPID_CFE_TIME_CMD_SET_STATE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_STATE_ERR_EID),
              "CFE_TIME_SetStateCmd",
              "Invalid clock state");

    /* Test sending the set time source = internal command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.sourcecmd.Payload.TimeSource = CFE_TIME_SourceSelect_INTERNAL;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sourcecmd),
            UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC);

#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SOURCE_EID),
              "CFE_TIME_SetSourceCmd",
              "Set internal source");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SOURCE_CFG_EID),
              "CFE_TIME_SetSourceCmd",
              "Set internal source invalid");
#endif

    /* Test sending the set time source = external command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.sourcecmd.Payload.TimeSource = CFE_TIME_SourceSelect_EXTERNAL;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sourcecmd),
            UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC);

#if (CFE_PLATFORM_TIME_CFG_SOURCE == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SOURCE_EID),
              "CFE_TIME_SetSourceCmd",
              "Set external source");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SOURCE_CFG_EID),
              "CFE_TIME_SetSourceCmd",
              "Set external source invalid");
#endif

    /* Test response to sending a set time source command using an
     * invalid source
     */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.sourcecmd.Payload.TimeSource = -1;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sourcecmd),
            UT_TPID_CFE_TIME_CMD_SET_SOURCE_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SOURCE_ERR_EID),
              "CFE_TIME_SetSourceCmd",
              "Invalid time source");

    /* Test sending a set tone signal source = primary command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.signalcmd.Payload.ToneSource = CFE_TIME_ToneSignalSelect_PRIMARY;
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.signalcmd),
            UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC);

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SIGNAL_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = primary");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SIGNAL_CFG_EID) &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetSignalCmd",
              "Set tone source = primary invalid");
#endif

    /* Test sending a set tone signal source = redundant command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.signalcmd.Payload.ToneSource = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.signalcmd),
            UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC);

#if (CFE_PLATFORM_TIME_CFG_SIGNAL == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SIGNAL_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = redundant");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SIGNAL_CFG_EID) &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetSignalCmd",
              "Set tone signal source = redundant invalid");
#endif

    /* Test response to sending a set tone signal source command using an
     * invalid source
     */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.signalcmd.Payload.ToneSource = -1;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.signalcmd),
            UT_TPID_CFE_TIME_CMD_SET_SIGNAL_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_SIGNAL_ERR_EID) &&
            CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetSignalCmd",
              "Invalid tone source");

    /* Test sending a time tone add delay command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.adddelaycmd.Payload.MicroSeconds = 0;
    CmdBuf.adddelaycmd.Payload.Seconds = 0;
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.adddelaycmd),
            UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC);

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELAY_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetDelayCmd",
              "Set tone add delay");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELAY_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetDelayCmd",
              "Set tone add delay invalid");
#endif

    /* Test sending a time tone subtract delay command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.subdelaycmd),
            UT_TPID_CFE_TIME_CMD_SUB_DELAY_CC);

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELAY_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetDelayCmd",
              "Set tone subtract delay");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELAY_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetDelayCmd",
              "Set subtract delay invalid");
#endif

    /* Test sending a set time command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.settimecmd),
            UT_TPID_CFE_TIME_CMD_SET_TIME_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_TIME_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetTimeCmd",
              "Set time");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_TIME_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetTimeCmd",
              "Set time invalid");
#endif

    /* Test sending a set MET command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setmetcmd),
            UT_TPID_CFE_TIME_CMD_SET_MET_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_MET_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetMETCmd",
              "Set MET");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_MET_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetMETCmd",
              "Set MET invalid");
#endif

    /* Test sending a set STCF command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setstcfcmd),
            UT_TPID_CFE_TIME_CMD_SET_STCF_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_STCF_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetSTCFCmd",
              "Set STCF");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_STCF_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetSTCFCmd",
              "Set STCF invalid");
#endif

    /* Test sending an adjust STCF positive command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.addadjcmd),
            UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELTA_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust positive");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELTA_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust positive invalid");
#endif

    /* Test sending an adjust STCF negative command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.subadjcmd),
            UT_TPID_CFE_TIME_CMD_SUB_ADJUST_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELTA_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust negative");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELTA_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_AdjustCmd",
              "Set STCF adjust negative invalid");
#endif

    /* Test sending an adjust STCF 1 Hz positive command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.add1hzadjcmd),
            UT_TPID_CFE_TIME_CMD_ADD_1HZ_ADJUSTMENT_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_1HZ_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust positive");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_1HZ_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust positive invalid");
#endif

    /* Test sending an adjust STCF 1 Hz negative command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.sub1hzadjcmd),
            UT_TPID_CFE_TIME_CMD_SUB_1HZ_ADJUSTMENT_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_1HZ_EID) &&
              CFE_TIME_TaskData.CommandCounter == 1 &&
              CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust negative");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_1HZ_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_1HzAdjCmd",
              "Set STCF 1Hz adjust negative invalid");
#endif

    /* Test response to sending a tone delay command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.adddelaycmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.adddelaycmd),
            UT_TPID_CFE_TIME_CMD_ADD_DELAY_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELAY_ERR_EID),
              "CFE_TIME_SetDelayCmd",
              "Invalid tone delay");

    /* Test response to sending a set time command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.settimecmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.settimecmd),
            UT_TPID_CFE_TIME_CMD_SET_TIME_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_TIME_ERR_EID),
              "CFE_TIME_SetTimeCmd",
              "Invalid time");

    /* Test response to sending a set MET command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.setmetcmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setmetcmd),
            UT_TPID_CFE_TIME_CMD_SET_MET_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_MET_ERR_EID),
              "CFE_TIME_SetMETCmd",
              "Invalid MET");

    /* Test response to sending a set STCF command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.setstcfcmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.setstcfcmd),
            UT_TPID_CFE_TIME_CMD_SET_STCF_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_STCF_ERR_EID),
              "CFE_TIME_SetSTCFCmd",
              "Invalid STCF");

    /* Test response to sending an adjust STCF command using an invalid time */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.setstcfcmd.Payload.MicroSeconds = 1000001;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.addadjcmd),
            UT_TPID_CFE_TIME_CMD_ADD_ADJUST_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_DELTA_ERR_EID),
              "CFE_TIME_AdjustCmd",
              "Invalid STCF adjust");

    /* Test sending a set leap seconds commands */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_TIME_TaskData.CommandCounter = 0;
    CFE_TIME_TaskData.CommandErrorCounter = 0;
    CmdBuf.leapscmd.Payload.LeapSeconds = 0;
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.leapscmd),
            UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_LEAPS_EID) &&
            CFE_TIME_TaskData.CommandCounter == 1 &&
            CFE_TIME_TaskData.CommandErrorCounter == 0,
              "CFE_TIME_SetLeapSecondsCmd",
              "Set leap seconds");
#else
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_LEAPS_CFG_EID) &&
              CFE_TIME_TaskData.CommandCounter == 0 &&
              CFE_TIME_TaskData.CommandErrorCounter == 1,
              "CFE_TIME_SetLeapSecondsCmd",
              "Set leap seconds invalid");
#endif

    /* Test response to sending an invalid command */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd),
            UT_TPID_CFE_TIME_CMD_INVALID_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_CC_ERR_EID),
              "CFE_TIME_TaskPipe",
              "Invalid command code");

    /* Test response to sending a command using an invalid message ID */
    UT_InitData();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd), 
            UT_TPID_CFE_TIME_INVALID_MID);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_ID_ERR_EID),
              "CFE_TIME_TaskPipe",
              "Invalid message ID");

    /* Test response to sending a command with a bad length */
    UT_InitData();
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd), 
            UT_TPID_CFE_TIME_CMD_SET_LEAP_SECONDS_CC);
    UT_Report(__FILE__, __LINE__,
              UT_EventIsInHistory(CFE_TIME_LEN_ERR_EID),
              "CFE_TIME_TaskPipe",
              "Invalid message ID");

    /* Call the Task Pipe with the 1Hz command. */
    /* In the 1Hz state machine it should call PSP GetTime as part,
        of latching the clock.  This is tested only to see that the latch executed. */
    UT_InitData();
    UT_CallTaskPipe(CFE_TIME_TaskPipe, &CmdBuf.message, sizeof(CmdBuf.cmd), 
            UT_TPID_CFE_TIME_1HZ_CMD);
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_PSP_GetTime)) > 0,
              "CFE_TIME_TaskPipe",
              "Invoke 1Hz state machine via SB");

}

/*
** Test the reset area functionality
*/
void Test_ResetArea(void)
{
    CFE_TIME_Reference_t Reference;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Reset Area\n");
#endif

    /* Test successfully updating the reset area */
    UT_InitData();
    CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_EXISTING;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_ToneSignalSelect_PRIMARY;
    UT_GetResetDataPtr()->TimeResetVars.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_UpdateResetVars(&Reference);
    UT_Report(__FILE__, __LINE__,
              UT_GetResetDataPtr()->TimeResetVars.ClockSignal ==
              CFE_TIME_TaskData.ClockSignal,
              "CFE_TIME_UpdateResetVars",
              "Successful update");

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus ==
              CFE_TIME_RESET_AREA_EXISTING,
              "CFE_TIME_QueryResetVars",
              "Initialize times using an existing reset area; time tone PRI");

    /* Tests existing and good Reset Area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_ToneSignalSelect_REDUNDANT);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus ==
              CFE_TIME_RESET_AREA_EXISTING,
              "CFE_TIME_QueryResetVars",
              "Initialize times using an existing reset area; time tone RED");

    /* Test response to a bad reset area */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_ERROR, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_BAD,
              "CFE_TIME_QueryResetVars",
              "Reset area error");

    /* Test initializing to default time values */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE + 1,
                             CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_NEW,
              "CFE_TIME_QueryResetVars",
              "Initialize to default values");

    /* Test response to a bad clock signal selection parameter */
    UT_InitData();
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE,
                             CFE_TIME_ToneSignalSelect_REDUNDANT+1);
    CFE_TIME_QueryResetVars();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.DataStoreStatus == CFE_TIME_RESET_AREA_NEW,
              "CFE_TIME_QueryResetVars",
              "Bad clock signal selection");

    /* Test response to a reset area error */
    UT_InitData();
    CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_ERROR;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_ToneSignalSelect_PRIMARY;
    UT_GetResetDataPtr()->TimeResetVars.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_UpdateResetVars(&Reference);
    UT_Report(__FILE__, __LINE__,
              UT_GetResetDataPtr()->TimeResetVars.ClockSignal !=
              CFE_TIME_TaskData.ClockSignal,
              "CFE_TIME_UpdateResetVars",
              "Reset area error");

    /* Test failure to get reset area updating the reset area */
    UT_InitData();
    UT_SetStatusBSPResetArea(CFE_PSP_ERROR, 0, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_TIME_TaskData.DataStoreStatus = CFE_TIME_RESET_AREA_EXISTING;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_ToneSignalSelect_PRIMARY;
    UT_GetResetDataPtr()->TimeResetVars.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_UpdateResetVars(&Reference);
    UT_Report(__FILE__, __LINE__,
              UT_GetResetDataPtr()->TimeResetVars.ClockSignal !=
              CFE_TIME_TaskData.ClockSignal,
              "CFE_TIME_UpdateResetVars",
              "Get reset area fail");
}

/*
** Test time state functions
*/
void Test_State(void)
{
    uint16 flag;
    int16  ExpState;
    CFE_TIME_Reference_t Reference;
    volatile CFE_TIME_ReferenceState_t *RefState;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Time State\n");
#endif

    /* Test determining if the clock state is valid with the server state
     * in "no flywheel"
     */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_SetState_WAS_SET;
    Reference.ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_FlywheelState_NO_FLY;
    ExpState = CFE_TIME_ClockState_VALID;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == ExpState,
              "CFE_TIME_CalculateState",
              "Valid time state; server state - no flywheel");

    /* Test determining if the clock state is valid with the server state
     * in "flywheel"
     */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_SetState_WAS_SET;
    Reference.ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_FlywheelState_IS_FLY;
#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    ExpState = CFE_TIME_ClockState_FLYWHEEL;
#else
    ExpState = CFE_TIME_ClockState_VALID;
#endif
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == ExpState,
              "CFE_TIME_CalculateState",
              "Valid time state; server state - flywheel");

    /* Test determining if the clock state = flywheel */
    UT_InitData();
    Reference.ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == CFE_TIME_ClockState_FLYWHEEL,
              "CFE_TIME_CalculateState",
              "Flywheel time state");

    /* Test determining if the clock state = invalid */
    UT_InitData();
    Reference.ClockSetState = CFE_TIME_ClockState_INVALID;
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_CalculateState(&Reference) == CFE_TIME_ClockState_INVALID,
              "CFE_TIME_CalculateState",
              "Invalid time state");

    /* Test alternate flag values */
    UT_InitData();

    /* set up initial state */
    CFE_TIME_SetState(CFE_TIME_ClockState_VALID);
    CFE_TIME_SetState(CFE_TIME_ClockState_FLYWHEEL);
    flag = CFE_TIME_GetClockInfo();

    /* Add server flag depending on configuration */
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockSetState = CFE_TIME_SetState_NOT_SET;
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    RefState->DelayDirection = CFE_TIME_AdjustDirection_SUBTRACT;


    /*
     * Atomicity Test Case: Check the flags _BEFORE_ calling FinishReferenceUpdate().
     * The function should return the same flags as the previous call, even though
     * the global data has been updated with new values.
     */
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_GetClockInfo() == flag,
              "CFE_TIME_GetStateFlags",
              "State data atomic update before finish");

    /* Now finish the update and the flags should be different */
    CFE_TIME_FinishReferenceUpdate(RefState);

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_GetClockInfo() != flag,
              "CFE_TIME_GetStateFlags",
              "State data atomic update after finish");

    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.ClockSignal = CFE_TIME_ToneSignalSelect_REDUNDANT;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.Forced2Fly = false;
    CFE_TIME_TaskData.OneTimeDirection = CFE_TIME_AdjustDirection_SUBTRACT;
    CFE_TIME_TaskData.OneHzDirection = CFE_TIME_AdjustDirection_SUBTRACT;

    flag = CFE_TIME_GetClockInfo();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_GetClockInfo() == flag,
              "CFE_TIME_GetStateFlags",
              "State data with alternate flags");

    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_INTERNAL;
}

/*
** Test getting reference data (time at "tone")
*/
void Test_GetReference(void)
{
    CFE_TIME_Reference_t Reference;
    volatile CFE_TIME_ReferenceState_t *RefState;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Get Reference\n");
#endif

    /* Test with local clock rollover */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds = 20;
    RefState->AtToneMET.Subseconds = 0;
    RefState->AtToneDelay.Seconds = 0;
    RefState->AtToneDelay.Subseconds = 0;
    RefState->AtToneLatch.Seconds = 10;
    RefState->AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 1000;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + MaxLocalClock - AtToneLatch +
     *              BSP_Time [+ or - AtToneDelay]
     */
    UT_Report(__FILE__, __LINE__,
              Reference.CurrentMET.Seconds == 1010 &&
              Reference.CurrentMET.Subseconds == 0,
              "CFE_TIME_GetReference",
              "Local clock < latch at tone time");

    /* Test without local clock rollover */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds = 20;
    RefState->AtToneMET.Subseconds = 0;
    RefState->AtToneDelay.Seconds = 0;
    RefState->AtToneDelay.Subseconds = 0;
    RefState->AtToneLatch.Seconds = 10;
    RefState->AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(15, 0);
    CFE_TIME_GetReference(&Reference);
    /* CurrentMET = AtToneMET + BSP_Time - AtToneLatch [+ or - AtToneDelay]
     */
    UT_Report(__FILE__, __LINE__,
              Reference.CurrentMET.Seconds == 25 &&
              Reference.CurrentMET.Subseconds == 0,
              "CFE_TIME_GetReference",
              "Local clock > latch at tone time");
}

/*
** Test send tone, and validate tone and data packet functions
*/
void Test_Tone(void)
{
    CFE_TIME_SysTime_t time1;
    CFE_TIME_SysTime_t time2;
    volatile CFE_TIME_ReferenceState_t *RefState;
    uint32 VersionSave;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    int seconds = 7654321;
    int virtualSeconds = 1234567;
#endif

    uint32 MinElapsed = CFE_TIME_TaskData.MinElapsed;
    uint32 MaxElapsed = CFE_TIME_TaskData.MaxElapsed;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Tone\n");
#endif

    /* Test time at the tone in flywheel mode */
    UT_InitData();
    CFE_TIME_SetState(CFE_TIME_ClockState_FLYWHEEL);

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    UT_SetBSP_Time(seconds, 0);
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneMET.Seconds = 0; /* 20.00000 */
    RefState->AtToneMET.Subseconds = 0;
    RefState->AtToneDelay.Seconds = 0; /* 0.00000 */
    RefState->AtToneDelay.Subseconds = 0;
    RefState->AtToneLatch.Seconds = 0; /* 10.00000 */
    RefState->AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.VirtualMET = virtualSeconds;
    CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds = 0;
    CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds = seconds;
    CFE_TIME_FinishReferenceUpdate(RefState);

    CFE_TIME_ToneSend();

 #if (CFE_MISSION_TIME_AT_TONE_WILL_BE == true)
    seconds++;
 #endif

#ifdef CFE_PLATFORM_TIME_CFG_BIGENDIAN
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds == CFE_MAKE_BIG32(seconds) &&
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds == CFE_MAKE_BIG32(0), /* yes, I know, 0 is 0 in all endians */
              "CFE_TIME_ToneSend",
              "Send tone, flywheel ON");
#else /* !CFE_PLATFORM_TIME_CFG_BIGENDIAN */
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds == seconds &&
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds == 0,
              "CFE_TIME_ToneSend",
              "Send tone, flywheel ON");
#endif /* CFE_PLATFORM_TIME_CFG_BIGENDIAN */

    /* Test time at the tone when not in flywheel mode */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);

    CFE_TIME_ToneSend();

 #if (CFE_MISSION_TIME_AT_TONE_WILL_BE == true)
    virtualSeconds++;
 #endif

#ifdef CFE_PLATFORM_TIME_CFG_BIGENDIAN

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds ==
                  CFE_MAKE_BIG32(virtualSeconds) &&
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds == CFE_MAKE_BIG32(0), /* yes, I know, 0 is 0 in all endians */
              "CFE_TIME_ToneSend",
              "Send tone, flywheel OFF");

#else /* !CFE_PLATFORM_TIME_CFG_BIGENDIAN */

    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Seconds ==
                  virtualSeconds &&
              CFE_TIME_TaskData.ToneDataCmd.Payload.AtToneMET.Subseconds == 0,
              "CFE_TIME_ToneSend",
              "Send tone, flywheel OFF");

#endif /* CFE_PLATFORM_TIME_CFG_BIGENDIAN */

#else
    /* Added to prevent a missing test */
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ToneSend",
              "*Not tested* Send tone, flywheel ON");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ToneSend",
              "*Not tested* Send tone, flywheel OFF"); /* Added to prevent a missing test # */
#endif

    time1.Seconds = 10;
    time1.Subseconds = 0;
    time2.Seconds = 10;
    time2.Subseconds = 100;
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * false.  This test also serves to set the internal PrevTime variables
     * to known values
     */
    UT_InitData();
    VersionSave = CFE_TIME_TaskData.LastVersionCounter; /* Verifies 'ForcedToFly' path */
    CFE_TIME_TaskData.ToneMatchCounter = 0;
    CFE_TIME_TaskData.Forced2Fly = false; /* Exercises '!ForcedToFly' path */
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.LastVersionCounter > VersionSave &&
              CFE_TIME_TaskData.ToneMatchCounter == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 < time 2, Forced2Fly false");

    /* Test tone validation when time 1 equals the previous time 1 value */
    UT_InitData();
    CFE_TIME_TaskData.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time1, time1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrorCounter == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 same as previous time 1");

    /* Test tone validation when time 2 equals the previous time 2 value */
    UT_InitData();
    CFE_TIME_TaskData.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrorCounter == 1,
              "CFE_TIME_ToneVerify",
              "Time 2 same as previous time 2");

    /* Test tone validation with time 1 > time 2 value (clock rollover) */
    UT_InitData();
    time1.Seconds = 20;
    time2.Seconds = 0;
    time1.Subseconds = 0;
    time2.Subseconds = 100;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 20; /* 1000.00000 */
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_TaskData.ToneMatchCounter = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchCounter == 1,
              "CFE_TIME_ToneVerify",
              "Time 1 > time 2 (clock rollover)");

    /* Test tone validation when time between packet and tone is out of
     * limits (seconds)
     */
    UT_InitData();
    time1.Seconds = 10;
    time2.Seconds = 11;
    time1.Subseconds = 0;
    time2.Subseconds = 0;
    CFE_TIME_TaskData.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time2, time1);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrorCounter == 1,
              "CFE_TIME_ToneVerify",
              "Elapsed time out of limits (seconds)");

    /* Test tone validation when time between packet and tone is out of
     * limits (subseconds low)
     */
    UT_InitData();
    time1.Seconds = 12;
    time2.Seconds = 12;
    time1.Subseconds = 0;
    time2.Subseconds = 10;
    CFE_TIME_TaskData.MinElapsed = 20;
    CFE_TIME_TaskData.MaxElapsed = 30;
    CFE_TIME_TaskData.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrorCounter == 1,
              "CFE_TIME_ToneVerify",
              "Elapsed time out of limits (subseconds low)");

    /* Test tone validation when time between packet and tone is out of
     * limits (subseconds high)
     */
    UT_InitData();
    time1.Seconds = 13;
    time2.Seconds = 13;
    time1.Subseconds = 0;
    time2.Subseconds = 40;
    CFE_TIME_TaskData.MinElapsed = 20;
    CFE_TIME_TaskData.MaxElapsed = 30;
    CFE_TIME_TaskData.ToneMatchErrorCounter = 0;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneMatchErrorCounter == 1,
              "CFE_TIME_ToneVerify",
              "Elapsed time out of limits (subseconds high)");

    CFE_TIME_TaskData.MinElapsed = MinElapsed;
    CFE_TIME_TaskData.MaxElapsed = MaxElapsed;

    time1.Seconds = 10;
    time1.Subseconds = 0;
    time2.Seconds = 10;
    time2.Subseconds = 100;

    /* Test tone validation when time 1 < time 2 and Forced2Fly is set to
     * false and the clock source is external
     */
    UT_InitData();
    VersionSave = CFE_TIME_TaskData.LastVersionCounter; /* Verifies 'ForcedToFly' path */
    CFE_TIME_TaskData.ToneMatchCounter = 0;
    CFE_TIME_TaskData.Forced2Fly = false; /* Exercises '!ForcedToFly' path */
    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_EXTERNAL;
    CFE_TIME_TaskData.VirtualMET = 5;
    CFE_TIME_TaskData.PendingMET.Seconds = CFE_TIME_TaskData.VirtualMET;
    CFE_TIME_ToneVerify(time1, time2);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.LastVersionCounter > VersionSave &&
              CFE_TIME_TaskData.ToneMatchCounter == 1 &&
              CFE_TIME_TaskData.VirtualMET == 5,
              "CFE_TIME_ToneVerify",
              "Time 1 < time 2, Forced2Fly false, Clock EXTERN");

    CFE_TIME_TaskData.ClockSource = CFE_TIME_SourceSelect_INTERNAL;

#if (CFE_PLATFORM_TIME_CFG_CLIENT == true)
    /* Test tone update using an invalid pending state */
    UT_InitData();
    CFE_TIME_TaskData.PendingState = CFE_TIME_ClockState_INVALID;
    CFE_TIME_TaskData.ClockSetState  = CFE_TIME_SetState_WAS_SET;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_ToneUpdate();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ClockSetState == CFE_TIME_SetState_NOT_SET &&
              CFE_TIME_TaskData.ServerFlyState == CFE_TIME_FlywheelState_NO_FLY,
              "CFE_TIME_ToneUpdate",
              "Invalid pending state");

    /* Test tone update using FLYWHEEL as the pending state */
    UT_InitData();
    CFE_TIME_TaskData.PendingState = CFE_TIME_ClockState_FLYWHEEL;
    CFE_TIME_TaskData.ServerFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_ToneUpdate();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ServerFlyState == CFE_TIME_FlywheelState_IS_FLY,
              "CFE_TIME_ToneUpdate",
              "Pending state is FLYWHEEL");

#else
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ToneUpdate",
              "*Not tested* Invalid pending state");

    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_ToneUpdate",
              "*Not tested* Pending state is FLYWHEEL");
#endif
}

/*
** Test the 1Hz STCF adjustment, local interrupt service request and task
** (non-tone), and tone task functions
*/
void Test_1Hz(void)
{
    uint16             Arg;
    uint16             i;
    uint32             delSec = 3;
    CFE_TIME_SysTime_t time1;
    CFE_TIME_SysTime_t time2;
    volatile CFE_TIME_ReferenceState_t *RefState;

#if (CFE_PLATFORM_TIME_CFG_SERVER == true)
    /* Test 1Hz STCF adjustment in positive direction */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneSTCF.Seconds = 20;
    RefState->AtToneSTCF.Subseconds = 60;
    CFE_TIME_FinishReferenceUpdate(RefState);
    time1.Seconds = 10;
    time1.Subseconds = 30;
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_AdjustDirection_ADD);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UT_Report(__FILE__, __LINE__,
              RefState->AtToneSTCF.Seconds == 30 &&
              RefState->AtToneSTCF.Subseconds == 90,
              "CFE_TIME_Set1HzAdj",
              "Positive adjustment");

    /* Test 1Hz STCF adjustment in negative direction */
    UT_InitData();
    CFE_TIME_Set1HzAdj(time1, CFE_TIME_AdjustDirection_SUBTRACT);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UT_Report(__FILE__, __LINE__,
              RefState->AtToneSTCF.Seconds == 20 &&
              RefState->AtToneSTCF.Subseconds == 60,
              "CFE_TIME_Set1HzAdj",
              "Negative adjustment");
#else
    /* These prevent missing tests when CFE_PLATFORM_TIME_CFG_SERVER is false */
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Positive adjustment");
    UT_Report(__FILE__, __LINE__,
              true,
              "CFE_TIME_Local1HzISR",
              "(*Not tested*) Negative adjustment");
#endif

    /* Test local 1Hz interrupt when enough time has elapsed (seconds) since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.AutoStartFly = false;
    RefState->AtToneLatch.Seconds = 1;
    RefState->AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 1;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.MaxLocalClock.Seconds = CFE_PLATFORM_TIME_CFG_LATCH_FLY - 1;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_Local1HzStateMachine();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.AutoStartFly == true,
              "CFE_TIME_Local1HzISR",
              "Auto start flywheel (seconds)");

    /* Test local 1Hz interrupt when enough time has elapsed since receiving a
     * time update to automatically update the MET
     */
    UT_InitData();
    time1.Seconds = 11;
    time2.Seconds = 1;
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->AtToneDelay.Seconds = 0;
    RefState->AtToneDelay.Subseconds = 0;
    RefState->AtToneMET.Seconds = time1.Seconds;
    RefState->AtToneMET.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    RefState->AtToneLatch.Seconds = time2.Seconds;
    RefState->AtToneLatch.Subseconds = 0;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.MaxLocalClock.Seconds = CFE_PLATFORM_TIME_CFG_LATCH_FLY + delSec;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    RefState->ClockFlyState = CFE_TIME_FlywheelState_IS_FLY;
    CFE_TIME_FinishReferenceUpdate(RefState);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UT_Report(__FILE__, __LINE__,
              RefState->AtToneMET.Seconds == time1.Seconds +
                                                     CFE_PLATFORM_TIME_CFG_LATCH_FLY +
                                                     delSec - time2.Seconds &&
              RefState->AtToneLatch.Seconds == 0,
              "CFE_TIME_Local1HzISR",
              "Auto update MET");

    /* Test the tone signal ISR when the tone doesn't occur ~1 second after
     * the previous one
     */
    UT_InitData();
    CFE_TIME_TaskData.IsToneGood = true;
    UT_SetBSP_Time(0, 0);
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 1;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == false,
              "CFE_TIME_Tone1HzISR",
              "Invalid tone signal interrupt");

    /* Test the tone signal ISR call to the application synch callback
     * function by verifying the number of callbacks made matches the number
     * of registered callbacks
     */
    UT_InitData();
    UT_SetBSP_Time(1, 0);
    /* Set up the sync callback table with callbacks for 3 apps */
    for (i = 0; i < (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])); i++)
    {
        if (i < 3)
        {
            CFE_TIME_TaskData.SynchCallback[i].Ptr = &ut_time_MyCallbackFunc;
        }
        else
        {
            CFE_TIME_TaskData.SynchCallback[i].Ptr = NULL;
        }
    }
    ut_time_CallbackCalled = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              ut_time_CallbackCalled == 3,
              "CFE_TIME_Tone1HzISR",
              "Proper number of callbacks made");

    /* Test the local 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_TaskData.AutoStartFly = true;
    CFE_TIME_TaskData.LocalTaskCounter = 0;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 2, OS_ERROR);
    CFE_TIME_Local1HzTask();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.LocalTaskCounter == 1,
              "CFE_TIME_Local1HzTask",
              "Semaphore creation pass, then fail");

    /* Test the tone 1Hz task where the binary semaphore take fails on the
     * second call
     */
    UT_InitData();
    CFE_TIME_TaskData.ToneTaskCounter = 0;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 2, OS_ERROR);
    CFE_TIME_Tone1HzTask();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.ToneTaskCounter == 1,
              "CFE_TIME_Tone1HzTask",
              "Semaphore creation pass, then fail");

    /* Test the tone 1Hz task with the tone signal over the time limit */
    UT_InitData();
    UT_SetBSP_Time(1, CFE_TIME_Sub2MicroSecs(CFE_TIME_TaskData.ToneOverLimit));
    CFE_TIME_TaskData.IsToneGood = true;
    CFE_TIME_TaskData.MaxLocalClock.Seconds = 0;
    CFE_TIME_TaskData.MaxLocalClock.Subseconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == false,
              "CFE_TIME_Tone1HzISR",
              "Invalid tone signal interrupt; tolerance over limit");

    /* Test the tone 1Hz task with the tone signal within the time limits */
    UT_InitData();
    UT_SetBSP_Time(0, CFE_TIME_Sub2MicroSecs(CFE_TIME_TaskData.ToneUnderLimit) + 1);
    CFE_TIME_TaskData.IsToneGood = false;
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == true,
              "CFE_TIME_Tone1HzISR",
              "Valid tone signal interrupt, tolerance in limits");

    /* Test the tone 1Hz task with the tone signal under the time limit */
    UT_InitData();
    UT_SetBSP_Time(0, CFE_TIME_Sub2MicroSecs(CFE_TIME_TaskData.ToneUnderLimit) - 1);
    CFE_TIME_TaskData.IsToneGood = true;
    CFE_TIME_TaskData.ToneSignalLatch.Seconds = 0;
    CFE_TIME_TaskData.ToneSignalLatch.Subseconds = 0;
    CFE_TIME_Tone1HzISR();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.IsToneGood == false,
              "CFE_TIME_Tone1HzISR",
              "Valid tone signal interrupt, tolerance under limits");

    /* Test local 1Hz interrupt when enough time has elapsed (subseconds) since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.AutoStartFly = false;
    RefState->AtToneLatch.Seconds = 1;
    RefState->AtToneLatch.Subseconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 1;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzStateMachine();
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.AutoStartFly == true,
              "CFE_TIME_Local1HzISR",
              "Auto start flywheel (subseconds)");

    /* Test local 1Hz interrupt when enough time has not elapsed since
     * receiving a time update to automatically change the state to flywheel
     */
    UT_InitData();
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(1, 0);
    CFE_TIME_Local1HzStateMachine();
    RefState = CFE_TIME_GetReferenceState();
    UT_Report(__FILE__, __LINE__,
              RefState->ClockFlyState == CFE_TIME_FlywheelState_NO_FLY,
              "CFE_TIME_Local1HzISR",
              "Do not auto start flywheel");

    /* Test the local 1Hz task where auto start flywheel is disabled */
    UT_InitData();
    CFE_TIME_TaskData.LocalTaskCounter = 0;
    CFE_TIME_TaskData.AutoStartFly = false;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTake), 2, OS_ERROR);
    CFE_TIME_Local1HzTask();
    RefState = CFE_TIME_GetReferenceState();
    UT_Report(__FILE__, __LINE__,
              RefState->ClockFlyState == CFE_TIME_FlywheelState_NO_FLY &&
              !UT_EventIsInHistory(CFE_TIME_FLY_ON_EID),
              "CFE_TIME_Local1HzTask",
              "Do not auto start flywheel");

    /* Test the CFE_TIME_Local1HzTimerCallback function */
    UT_InitData();
    CFE_TIME_TaskData.LocalIntCounter = 1;
    RefState = CFE_TIME_StartReferenceUpdate();
    RefState->ClockFlyState = CFE_TIME_FlywheelState_NO_FLY;
    CFE_TIME_TaskData.OneHzAdjust.Seconds = 0;
    CFE_TIME_TaskData.OneHzAdjust.Subseconds = 0;
    CFE_TIME_FinishReferenceUpdate(RefState);
    UT_SetBSP_Time(0, 0);
    CFE_TIME_Local1HzTimerCallback(123, &Arg);
    UT_Report(__FILE__, __LINE__,
              CFE_TIME_TaskData.LocalIntCounter == 2,
              "CFE_TIME_Local1HzTimerCallback",
              "Pass through to CFE_TIME_Local1HzISR");
}

/*
** Test unregistering synchronization callback function
*/
void Test_UnregisterSynchCallback(void)
{
    uint32  i = 0;
    int32   Result;

    ut_time_CallbackCalled = 0;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Unregister Callback\n");
#endif

    /* Unregister callback function one too many times to test both valid and
     * invalid cases
     */
    UT_InitData();


    /* Set up the sync callback table with callbacks for 3 apps */
    for (i = 0; i < (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])); i++)
    {
        if (i < 3)
        {
            CFE_TIME_TaskData.SynchCallback[i].Ptr = &ut_time_MyCallbackFunc;
        }
        else
        {
            CFE_TIME_TaskData.SynchCallback[i].Ptr = NULL;
        }
    }

    /* App ID 4 should not have a callback */
    UT_SetAppID(4);

    Result = CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
              Result == CFE_TIME_CALLBACK_NOT_REGISTERED,
              "CFE_TIME_UnregisterSynchCallback",
              "Unregistered result with no callback");

    /*
     * One callback per application is allowed; the first should succeed,
     * the second should fail.
     */
    /* App ID 2 should have a callback */
    UT_SetAppID(2);

    Result = CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
              Result == CFE_SUCCESS,
              "CFE_TIME_UnregisterSynchCallback",
              "Successfully unregister callback");

    Result = CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
              Result == CFE_TIME_CALLBACK_NOT_REGISTERED,
              "CFE_TIME_UnregisterSynchCallback",
              "Unregistered result after successful unregister");

    /* Test unregistering the callback function with a bad application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, -1);
    Result = CFE_TIME_UnregisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_Report(__FILE__, __LINE__,
              Result == -1,
              "CFE_TIME_UnregisterSynchCallback",
              "Bad application ID");

    /* Test tone notification with an invalid time synch application */
    UT_InitData();
    CFE_TIME_TaskData.IsToneGood = true;
    for (i = 0; i < (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])); i++)
    {
        CFE_TIME_TaskData.SynchCallback[i].Ptr = NULL;
    }
    CFE_TIME_NotifyTimeSynchApps();
    UT_Report(__FILE__, __LINE__,
              ut_time_CallbackCalled == 0,
              "CFE_TIME_NotifyTimeSynchApps",
              "Invalid time synch application");
}

/*
** Test function to free resources associated with an application
*/
void Test_CleanUpApp(void)
{
    uint16   i;
    uint16 Count;
    int32  Status = CFE_SUCCESS;
    uint32 TestAppId;

#ifdef UT_VERBOSE
    UT_Text("Begin Test Cleanup App\n");
#endif

    UT_InitData();
    CFE_ES_GetAppID(&TestAppId);

    /* Clear out the sync callback table */
    for (i = 0; i < (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])); i++)
    {
        CFE_TIME_TaskData.SynchCallback[i].Ptr = NULL;
    }

    /* Add callbacks for 3 apps into callback registry table */
    UT_SetAppID(1);
    CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_SetAppID(2);
    CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);
    UT_SetAppID(3);
    CFE_TIME_RegisterSynchCallback(&ut_time_MyCallbackFunc);

    /* Clean up an app which did not have a callback */
    TestAppId = 4;
    UT_SetAppID(TestAppId);
    Status = CFE_TIME_CleanUpApp(TestAppId);
    UT_Report(__FILE__, __LINE__,
              Status == CFE_SUCCESS,
              "CFE_TIME_CleanUpApp",
              "Successful result");

    Count = 0;
    for (i = 0; i < (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])); i++)
    {
        if (CFE_TIME_TaskData.SynchCallback[i].Ptr != NULL)
        {
            ++Count;
        }
    }

    /* should not have affected the callback table */
    UT_Report(__FILE__, __LINE__,
              Count == 3,
              "CFE_TIME_CleanUpApp",
              "No Sync Callback entry cleared");

    /* Clean up an app which did have a callback */
    TestAppId = 2;
    UT_SetAppID(TestAppId);
    Status = CFE_TIME_CleanUpApp(TestAppId);
    UT_Report(__FILE__, __LINE__,
              Status == CFE_SUCCESS,
              "CFE_TIME_CleanUpApp",
              "Successful result");

    Count = 0;
    for (i = 0; i < (sizeof(CFE_TIME_TaskData.SynchCallback) / sizeof(CFE_TIME_TaskData.SynchCallback[0])); i++)
    {
        if (CFE_TIME_TaskData.SynchCallback[i].Ptr != NULL)
        {
            ++Count;
        }
    }

    UT_Report(__FILE__, __LINE__,
              Count == 2,
              "CFE_TIME_CleanUpApp",
              "Sync Callback entry cleared");


    /* Test response to a bad application ID -
     * This is effectively a no-op but here for coverage */
    UT_SetAppID(CFE_PLATFORM_ES_MAX_APPLICATIONS);
    Status = CFE_TIME_CleanUpApp(CFE_PLATFORM_ES_MAX_APPLICATIONS);
    UT_Report(__FILE__, __LINE__,
              Status == CFE_TIME_CALLBACK_NOT_REGISTERED,
              "CFE_TIME_CleanUpApp",
              "Bad application ID");
}

