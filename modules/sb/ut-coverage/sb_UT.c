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

/*
** Includes
*/
#include "sb_UT.h"
#include "cfe_msg.h"
#include "cfe_core_resourceid_basevalues.h"

/*
 * A method to add an SB "Subtest"
 *
 * As SB has a lot of test cases it is helpful to group them.
 * This allows each test routine to reported consistently as "GroupName.TestName"
 *
 * This also implicitly includes a call to SB_ResetUnitTest() as a setup function,
 * so the test routines do _not_ need to do this explicitly on every test case.
 */
#define SB_UT_ADD_SUBTEST(Func) UtTest_AddSubTest(Func, SB_ResetUnitTest, NULL, __func__, #Func)

/*
 * MSG ID constants for unit testing:
 * Unit test cases should not directly use integer MsgId values
 *
 * The following constants are of the CFE_SB_MsgId_t type
 */

const CFE_SB_MsgId_t SB_UT_CMD_MID = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE);
const CFE_SB_MsgId_t SB_UT_TLM_MID = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE);

const CFE_SB_MsgId_t SB_UT_CMD_MID1 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 1);
const CFE_SB_MsgId_t SB_UT_CMD_MID2 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 2);
const CFE_SB_MsgId_t SB_UT_CMD_MID3 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 3);
const CFE_SB_MsgId_t SB_UT_CMD_MID4 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 4);
const CFE_SB_MsgId_t SB_UT_CMD_MID5 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 5);
const CFE_SB_MsgId_t SB_UT_CMD_MID6 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE + 6);

const CFE_SB_MsgId_t SB_UT_TLM_MID1 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 1);
const CFE_SB_MsgId_t SB_UT_TLM_MID2 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 2);
const CFE_SB_MsgId_t SB_UT_TLM_MID3 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 3);
const CFE_SB_MsgId_t SB_UT_TLM_MID4 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 4);
const CFE_SB_MsgId_t SB_UT_TLM_MID5 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 5);
const CFE_SB_MsgId_t SB_UT_TLM_MID6 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE + 6);

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define SB_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_SB_##intf##_MID)

#define SB_UT_MSG_DISPATCH(intf, cmd)       SB_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_SB_##cmd)
#define SB_UT_CC_DISPATCH(intf, cc, cmd)    SB_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define SB_UT_ERROR_DISPATCH(intf, cc, err) SB_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_NOOP_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_NOOP_CC,NoopCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_RESET_COUNTERS_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_RESET_COUNTERS_CC,ResetCountersCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_SEND_SB_STATS_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_SEND_SB_STATS_CC,SendSbStatsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_ROUTING_INFO_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_WRITE_ROUTING_INFO_CC,WriteRoutingInfoCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_PIPE_INFO_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_WRITE_PIPE_INFO_CC,WritePipeInfoCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_WRITE_MAP_INFO_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_WRITE_MAP_INFO_CC,WriteMapInfoCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_ENABLE_ROUTE_CC,EnableRouteCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC =
    { SB_UT_CC_DISPATCH(CMD,CFE_SB_DISABLE_ROUTE_CC,DisableRouteCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SEND_HK =
    { SB_UT_MSG_DISPATCH(SEND_HK,SendHkCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_SEND_PREV_SUBS_CC =
    { SB_UT_CC_DISPATCH(SUB_RPT_CTRL,CFE_SB_SEND_PREV_SUBS_CC,SendPrevSubsCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_ENABLE_SUB_REPORTING_CC =
    { SB_UT_CC_DISPATCH(SUB_RPT_CTRL,CFE_SB_ENABLE_SUB_REPORTING_CC,DisableSubReportingCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTL_DISABLE_SUB_REPORTING_CC =
    { SB_UT_CC_DISPATCH(SUB_RPT_CTRL,CFE_SB_DISABLE_SUB_REPORTING_CC,EnableSubReportingCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_CMD_BAD_FCNCODE =
    { SB_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_SUB_RPT_CTRL_BAD_FCNCODE =
    { SB_UT_ERROR_DISPATCH(SUB_RPT_CTRL, -1, CFE_STATUS_BAD_COMMAND_CODE) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_SB_BAD_MSGID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
/* clang-format on */

/*
 * A MsgId value which still qualifies as valid
 *
 * This is a  "borderline" value to test the limits of the validity checking
 * The specific value depends on how MsgId is actually defined internally
 */
const CFE_SB_MsgId_t SB_UT_LAST_VALID_MID = CFE_SB_MSGID_WRAP_VALUE(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);

/*
 * A MsgId value which still qualifies as valid
 *
 * This is a  "borderline" value to test the limits of the validity checking
 * The specific value depends on how MsgId is actually defined internally
 */
const CFE_SB_MsgId_t SB_UT_FIRST_VALID_MID = CFE_SB_MSGID_WRAP_VALUE(1);

/*
 * A MsgId value which is in the middle of the valid range
 *
 * The specific value depends on how MsgId is actually defined internally
 */
const CFE_SB_MsgId_t SB_UT_INTERMEDIATE_VALID_MID =
    CFE_SB_MSGID_WRAP_VALUE(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID / 2 + 1);

/*
 * A MsgId value which is not valid but also not equal to CFE_SB_INVALID_MSG_ID
 * Like CFE_SB_INVALID_MSG_ID, this should also _not_ pass the validity check.
 */
const CFE_SB_MsgId_t SB_UT_ALTERNATE_INVALID_MID = CFE_SB_MSGID_WRAP_VALUE(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1);

/*
 * A MsgId value which is valid per CCSDS but does not have the secondary header bit set
 */
const CFE_SB_MsgId_t SB_UT_BARE_CMD_MID3 = CFE_SB_MSGID_WRAP_VALUE(0x1003);
const CFE_SB_MsgId_t SB_UT_BARE_TLM_MID3 = CFE_SB_MSGID_WRAP_VALUE(0x0003);

#define SB_UT_PIPEID_0                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 0))
#define SB_UT_PIPEID_1                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 1))
#define SB_UT_PIPEID_2                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 2))
#define SB_UT_PIPEID_3                 CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + 3))
#define SB_UT_ALTERNATE_INVALID_PIPEID CFE_SB_PIPEID_C(CFE_ResourceId_FromInteger(0xDEADBEEF))

/*
 * Helper function to manufacture a fake pipe ID value that will validate
 */
CFE_ResourceId_t UT_SB_MakePipeIdForIndex(uint32 ArrayIdx)
{
    return CFE_ResourceId_FromInteger(CFE_SB_PIPEID_BASE + ArrayIdx);
}

/*
 * Helper function to "corrupt" a resource ID value in a consistent/predictable way,
 * which can also be un-done easily.
 */
CFE_ES_AppId_t UT_SB_AppID_Modify(CFE_ES_AppId_t InitialID, int32 Modifier)
{
    CFE_ES_AppId_t OutValue;
    uint32         InValue;

    InValue = CFE_RESOURCEID_TO_ULONG(InitialID);
    InValue += Modifier;

    /* Underneath the wrapper(s) the IDs are 32-bit integer values, so it can be copied */
    memcpy(&OutValue, &InValue, sizeof(OutValue));

    return OutValue;
}

/*
** Functions
*/
void UtTest_Setup(void)
{
    UT_Init("sb");
    UtPrintf("cFE SB Unit Test Output File\n\n");

    Test_SB_AppInit();
    Test_SB_MainRoutine();
    Test_SB_Cmds();
    Test_SB_EarlyInit();
    Test_CreatePipe_API();
    Test_DeletePipe_API();
    Test_PipeOpts_API();
    Test_GetPipeName_API();
    Test_GetPipeIdByName_API();
    Test_Subscribe_API();
    Test_Unsubscribe_API();
    Test_TransmitMsg_API();
    Test_ReceiveBuffer_API();
    SB_UT_ADD_SUBTEST(Test_CleanupApp_API);
    Test_SB_Utils();

    Test_SB_SpecialCases();
}

/*
** Reset variable values and sockets prior to a test
*/
void SB_ResetUnitTest(void)
{
    UT_InitData();
    CFE_SB_EarlyInit();
}

/*
** Function for calling SB application initialization
** test functions
*/
void Test_SB_AppInit(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_EVSRegFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_EVSSendEvtFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_CrPipeFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_Sub1Fail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_Sub2Fail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_Sub3Fail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_GetPoolFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_PutPoolFail);
}

/*
** Test task init with EVS_Register returning error
*/
void Test_SB_AppInit_EVSRegFail(void)
{
    int32 ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, ForcedRtnVal);
    UtAssert_INT32_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test task init with EVS_SendEvent returning error on task init event
*/
void Test_SB_AppInit_EVSSendEvtFail(void)
{
    int32             ForcedRtnVal = -1;
    CFE_ES_TaskInfo_t TestTaskInfo;

    /* To get coverage on CFE_SB_GetAppTskName(), this ensures that the
     * path with different app/task names is followed on at least one event.
     */
    memset(&TestTaskInfo, 0, sizeof(TestTaskInfo));
    strncpy(TestTaskInfo.TaskName, "test", sizeof(TestTaskInfo.TaskName) - 1);
    TestTaskInfo.TaskName[sizeof(TestTaskInfo.TaskName) - 1] = '\0';
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetTaskInfo), &TestTaskInfo, sizeof(TestTaskInfo), false);

    /* There are three events prior to init, pipe created (1) and subscription
     * rcvd (2). Fourth is SB initialized, but it is the first to use SendEvent.
     * (The others use SendEventWithAppID which is a different counter).
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, ForcedRtnVal);
    UtAssert_INT32_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Test task init with pipe create failure
*/
void Test_SB_AppInit_CrPipeFail(void)
{
    /* To fail the pipe create, force the OS_QueueCreate call to return some
     * type of error code.
     */
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_SB_AppInit(), CFE_SB_PIPE_CR_ERR);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_CR_PIPE_ERR_EID);
}

/*
** Test task init with a failure on first subscription request
*/
void Test_SB_AppInit_Sub1Fail(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, -1);
    UtAssert_INT32_EQ(CFE_SB_AppInit(), CFE_SB_BUF_ALOC_ERR);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_DEST_BLK_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Test task init with a failure on second subscription request
*/
void Test_SB_AppInit_Sub2Fail(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 2, -1);
    UtAssert_INT32_EQ(CFE_SB_AppInit(), CFE_SB_BUF_ALOC_ERR);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_DEST_BLK_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Test task init with a failure on second subscription request
*/
void Test_SB_AppInit_Sub3Fail(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 3, -1);
    UtAssert_INT32_EQ(CFE_SB_AppInit(), CFE_SB_BUF_ALOC_ERR);

    CFE_UtAssert_EVENTCOUNT(5);

    CFE_UtAssert_EVENTSENT(CFE_SB_DEST_BLK_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Test task init with a GetPool failure
*/
void Test_SB_AppInit_GetPoolFail(void)
{
    int32 ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 4, ForcedRtnVal);

    UtAssert_INT32_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Test task init with a PutPool failure
*/
void Test_SB_AppInit_PutPoolFail(void)
{
    int32 ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, ForcedRtnVal);

    UtAssert_INT32_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Function for calling SB main task test functions
*/
void Test_SB_MainRoutine(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_Main_RcvErr);
    SB_UT_ADD_SUBTEST(Test_SB_Main_InitErr);
    SB_UT_ADD_SUBTEST(Test_SB_Main_Nominal);
}

/*
** Test main task with a packet receive error
*/
void Test_SB_Main_RcvErr(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, -1);
    CFE_SB_TaskMain();

    CFE_UtAssert_EVENTCOUNT(7);

    CFE_UtAssert_EVENTSENT(CFE_SB_INIT_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_RD_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

static void SB_UT_PipeGetHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void *  data        = UT_Hook_GetArgValueByName(Context, "data", void *);
    size_t *size_copied = UT_Hook_GetArgValueByName(Context, "size_copied", size_t *);
    int32   status;

    if (!UT_Stub_GetInt32StatusCode(Context, &status))
    {
        *((void **)data) = UserObj;
        *size_copied     = sizeof(void *);

        status = OS_SUCCESS;
        UT_Stub_SetReturnValue(FuncKey, status);
    }
}

/*
** Test main task nominal path
*/
void Test_SB_Main_Nominal(void)
{
    union
    {
        CFE_SB_BufferD_t Desc;
        uint8            Data[sizeof(CFE_SB_NoopCmd_t) + offsetof(CFE_SB_BufferD_t, Content)];
    } Buffer;
    CFE_MSG_Size_t Size = sizeof(CFE_SB_NoopCmd_t);

    memset(&Buffer, 0, sizeof(Buffer));
    Buffer.Desc.UseCount      = 1;
    Buffer.Desc.AllocatedSize = sizeof(Buffer);
    Buffer.Desc.ContentSize   = Size;
    CFE_SB_TrackingListReset(&Buffer.Desc.Link);
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), SB_UT_PipeGetHandler, &Buffer);
    /* This still needs to error-out to avoid an infinite loop */
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 2, OS_QUEUE_EMPTY);

    /* For the first pass it will call CFE_SB_ProcessCmdPacket which needs to decode it */
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_CMD_NOOP_CC, Size, false);

    CFE_SB_TaskMain();

    CFE_UtAssert_EVENTCOUNT(6);

    CFE_UtAssert_EVENTSENT(CFE_SB_INIT_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_CMD0_RCVD_EID);

    /* remove the handler so the pipe can be deleted */
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), NULL, NULL);
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Test main task with an application initialization error
*/
void Test_SB_Main_InitErr(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    CFE_SB_TaskMain();

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Function for calling SB command test functions
*/
void Test_SB_Cmds(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_Noop);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RstCtrs);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_Stats);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_BackgroundFileWriteEvents);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoDef);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoAlreadyPending);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoDataGetter);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoDef);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoAlreadyPending);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoDataGetter);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoDef);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoAlreadyPending);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoDataGetter);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_EnRouteValParam);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_EnRouteNonExist);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_EnRouteInvParam);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_EnRouteInvParam2);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_EnRouteInvParam3);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_DisRouteValParam);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_DisRouteNonExist);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_DisRouteInvParam);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_DisRouteInvParam2);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_DisRouteInvParam3);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_SendHK);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_SendPrevSubs);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_SubRptOn);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_SubRptOff);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_CmdUnexpCmdCode);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_BadCmdLength);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_UnexpMsgId);
}

/*
** Test no-op command
*/
void Test_SB_Cmds_Noop(void)
{
    union
    {
        CFE_SB_Buffer_t  SBBuf;
        CFE_SB_NoopCmd_t Cmd;
    } Noop;

    memset(&Noop, 0, sizeof(Noop));

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(Noop.SBBuf), sizeof(Noop.Cmd), UT_TPID_CFE_SB_CMD_NOOP_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_CMD0_RCVD_EID);

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(Noop.SBBuf), 0, UT_TPID_CFE_SB_CMD_NOOP_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);
}

/*
** Test reset counters command
*/
void Test_SB_Cmds_RstCtrs(void)
{
    union
    {
        CFE_SB_Buffer_t           SBBuf;
        CFE_SB_ResetCountersCmd_t Cmd;
    } ResetCounters;

    memset(&ResetCounters, 0, sizeof(ResetCounters));

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(ResetCounters.SBBuf), sizeof(ResetCounters.Cmd),
                    UT_TPID_CFE_SB_CMD_RESET_COUNTERS_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_CMD1_RCVD_EID);

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(ResetCounters.SBBuf), 0,
                    UT_TPID_CFE_SB_CMD_RESET_COUNTERS_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);
} /* Test_SB_Cmds_RstCtrs */

/*
** Test send SB stats command
*/
void Test_SB_Cmds_Stats(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_SB_SendSbStatsCmd_t Cmd;
    } SendSbStats;
    CFE_SB_MsgId_t  MsgId;
    CFE_MSG_Size_t  Size;
    CFE_SB_PipeId_t PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId3 = CFE_SB_INVALID_PIPE;

    memset(&SendSbStats, 0, sizeof(SendSbStats));

    /* Make the pipe table non-empty so the stats command writes something */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, 4, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, 4, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, 4, "TestPipe3"));

    /* Generic command processing - The dispatch must be set up FIRST */
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_CMD_SEND_SB_STATS_CC, sizeof(SendSbStats.Cmd), false);

    /* For internal TransmitMsg call */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_STATS_TLM_MID);
    Size  = sizeof(CFE_SB_Global.StatTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_SB_ProcessCmdPipePkt(&SendSbStats.SBBuf);

    /* No subs event and command processing event */
    CFE_UtAssert_EVENTCOUNT(5);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_STATS_EID);

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(SendSbStats.SBBuf), 0, UT_TPID_CFE_SB_CMD_SEND_SB_STATS_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));
}

/*
** Test write routing information command using the default file name
*/
void Test_SB_Cmds_RoutingInfoDef(void)
{
    union
    {
        CFE_SB_Buffer_t              SBBuf;
        CFE_SB_WriteRoutingInfoCmd_t Cmd;
    } WriteRoutingInfo;

    memset(&WriteRoutingInfo, 0, sizeof(WriteRoutingInfo));

    /* Make some routing info by calling CFE_SB_AppInit */
    CFE_UtAssert_SETUP(CFE_SB_AppInit());

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteRoutingInfo.SBBuf), sizeof(WriteRoutingInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_ROUTING_INFO_CC);

    CFE_UtAssert_EVENTCOUNT(5);

    CFE_UtAssert_EVENTSENT(CFE_SB_INIT_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Also test with a bad file name - should generate CFE_SB_SND_RTG_ERR1_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteRoutingInfo.SBBuf), sizeof(WriteRoutingInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_ROUTING_INFO_CC);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteRoutingInfo.SBBuf), 0,
                    UT_TPID_CFE_SB_CMD_WRITE_ROUTING_INFO_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));
}

/*
** Test write routing information command with request already pending
*/
void Test_SB_Cmds_RoutingInfoAlreadyPending(void)
{
    union
    {
        CFE_SB_Buffer_t              SBBuf;
        CFE_SB_WriteRoutingInfoCmd_t Cmd;
    } WriteRoutingInfo;

    memset(&WriteRoutingInfo, 0, sizeof(WriteRoutingInfo));

    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);

    strncpy(WriteRoutingInfo.Cmd.Payload.Filename, "RoutingTstFile", sizeof(WriteRoutingInfo.Cmd.Payload.Filename) - 1);
    WriteRoutingInfo.Cmd.Payload.Filename[sizeof(WriteRoutingInfo.Cmd.Payload.Filename) - 1] = '\0';

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteRoutingInfo.SBBuf), sizeof(WriteRoutingInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_ROUTING_INFO_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);
}

/*
**  Test send routing information command data getter
*/
void Test_SB_Cmds_RoutingInfoDataGetter(void)
{
    CFE_SB_PipeId_t                  PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                  PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                  PipeId3   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t                   MsgId0    = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t                   MsgId1    = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t                   MsgId2    = SB_UT_TLM_MID3;
    CFE_SB_MsgId_t                   MsgId3    = SB_UT_TLM_MID4;
    CFE_SB_MsgId_t                   MsgId4    = SB_UT_TLM_MID5;
    CFE_SB_MsgId_t                   MsgId5    = SB_UT_TLM_MID6;
    uint16                           PipeDepth = 10;
    uint16                           i;
    void *                           LocalBuffer;
    size_t                           LocalBufSize;
    CFE_SB_BackgroundFileStateInfo_t State;
    CFE_SB_PipeD_t *                 PipeDscPtr;
    CFE_SB_DestinationD_t *          DestPtr;
    CFE_SB_DestinationD_t *          DestPtrSave;

    /* Create some map info */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId2));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId2, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId3, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId4, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId5, PipeId2));

    memset(&State, 0, sizeof(State));
    LocalBuffer  = NULL;
    LocalBufSize = 0;

    /* Loop through records including one unused */
    for (i = 0; i < 5; i++)
    {
        UtAssert_BOOL_FALSE(CFE_SB_WriteRouteInfoDataGetter(&State, i, &LocalBuffer, &LocalBufSize));
        UtAssert_NOT_NULL(LocalBuffer);
        UtAssert_NONZERO(LocalBufSize);
    }

    /* Last record should return TRUE but valid entries */
    UtAssert_BOOL_TRUE(CFE_SB_WriteRouteInfoDataGetter(&State, i, &LocalBuffer, &LocalBufSize));
    UtAssert_NOT_NULL(LocalBuffer);
    UtAssert_NONZERO(LocalBufSize);

    UtAssert_BOOL_TRUE(
        CFE_SB_WriteRouteInfoDataGetter(&State, CFE_PLATFORM_SB_MAX_MSG_IDS, &LocalBuffer, &LocalBufSize));
    UtAssert_ZERO(LocalBufSize);

    /* Hit max destination limit by corrupting head pointer, point to self (bug avoidance code) */
    DestPtr       = CFE_SBR_GetDestListHeadPtr(CFE_SBR_ValueToRouteId(1));
    DestPtrSave   = DestPtr->Next;
    DestPtr->Next = DestPtr;
    UtAssert_VOIDCALL(CFE_SB_CollectRouteInfo(CFE_SBR_ValueToRouteId(1), &State));
    DestPtr->Next = DestPtrSave;

    /* Hit invalid PipeId case by corrupting PipeId (bug avoidance code) */
    PipeDscPtr         = CFE_SB_LocatePipeDescByID(PipeId1);
    PipeDscPtr->PipeId = CFE_SB_INVALID_PIPE;
    UtAssert_VOIDCALL(CFE_SB_CollectRouteInfo(CFE_SBR_ValueToRouteId(0), &State));
    PipeDscPtr->PipeId = PipeId1;

    /* Hit invalid MsgId for route by passing in invalid route (bug avoidance code) */
    UtAssert_VOIDCALL(CFE_SB_CollectRouteInfo(CFE_SBR_ValueToRouteId(CFE_PLATFORM_SB_MAX_MSG_IDS), &State));

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));
}

/*
** Test send pipe information command default / nominal path
*/
void Test_SB_Cmds_PipeInfoDef(void)
{
    union
    {
        CFE_SB_Buffer_t           SBBuf;
        CFE_SB_WritePipeInfoCmd_t Cmd;
    } WritePipeInfo;
    CFE_SB_PipeId_t PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId3   = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth = 10;

    memset(&WritePipeInfo, 0, sizeof(WritePipeInfo));

    /* Create some pipe info */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WritePipeInfo.SBBuf), sizeof(WritePipeInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_PIPE_INFO_CC);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    /* Also test with a bad file name - should generate CFE_SB_SND_RTG_ERR1_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WritePipeInfo.SBBuf), sizeof(WritePipeInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_PIPE_INFO_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WritePipeInfo.SBBuf), 0,
                    UT_TPID_CFE_SB_CMD_WRITE_PIPE_INFO_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));
}

/*
** Test write pipe information command when already pending
*/
void Test_SB_Cmds_PipeInfoAlreadyPending(void)
{
    union
    {
        CFE_SB_Buffer_t           SBBuf;
        CFE_SB_WritePipeInfoCmd_t Cmd;
    } WritePipeInfo;

    memset(&WritePipeInfo, 0, sizeof(WritePipeInfo));

    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);

    strncpy(WritePipeInfo.Cmd.Payload.Filename, "PipeTstFile", sizeof(WritePipeInfo.Cmd.Payload.Filename) - 1);
    WritePipeInfo.Cmd.Payload.Filename[sizeof(WritePipeInfo.Cmd.Payload.Filename) - 1] = '\0';

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WritePipeInfo.SBBuf), sizeof(WritePipeInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_PIPE_INFO_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);
}

/*
** Test write pipe information data getter
*/
void Test_SB_Cmds_PipeInfoDataGetter(void)
{
    CFE_SB_PipeId_t                  PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                  PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                  PipeId3   = CFE_SB_INVALID_PIPE;
    uint16                           PipeDepth = 10;
    void *                           LocalBuffer;
    size_t                           LocalBufSize;
    CFE_SB_BackgroundFileStateInfo_t State;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));

    memset(&State, 0, sizeof(State));
    LocalBuffer  = NULL;
    LocalBufSize = 0;

    /* Note that CFE_SB_CreatePipe() fills entry 1 first, so entry 0 is unused */
    UtAssert_BOOL_FALSE(CFE_SB_WritePipeInfoDataGetter(&State, 0, &LocalBuffer, &LocalBufSize));
    UtAssert_ZERO(LocalBufSize);

    UtAssert_BOOL_FALSE(CFE_SB_WritePipeInfoDataGetter(&State, 1, &LocalBuffer, &LocalBufSize));
    UtAssert_NOT_NULL(LocalBuffer);
    UtAssert_NONZERO(LocalBufSize);

    UtAssert_BOOL_TRUE(
        CFE_SB_WritePipeInfoDataGetter(&State, CFE_PLATFORM_SB_MAX_PIPES - 1, &LocalBuffer, &LocalBufSize));
    UtAssert_ZERO(LocalBufSize);

    UtAssert_BOOL_TRUE(CFE_SB_WritePipeInfoDataGetter(&State, CFE_PLATFORM_SB_MAX_PIPES, &LocalBuffer, &LocalBufSize));
    UtAssert_ZERO(LocalBufSize);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));
}

/*
** Test background file write event generator
*/
void Test_SB_Cmds_BackgroundFileWriteEvents(void)
{
    CFE_SB_BackgroundFileStateInfo_t State;

    memset(&State, 0, sizeof(State));

    UT_ClearEventHistory();
    CFE_SB_BackgroundFileEventHandler(&State, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0, 1000);
    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_EID);

    UT_ClearEventHistory();
    CFE_SB_BackgroundFileEventHandler(&State, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR, CFE_SUCCESS, 10, 10, 1000);
    CFE_UtAssert_EVENTSENT(CFE_SB_FILEWRITE_ERR_EID);

    UT_ClearEventHistory();
    CFE_SB_BackgroundFileEventHandler(&State, CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR, CFE_SUCCESS, 10, 10, 1000);
    CFE_UtAssert_EVENTSENT(CFE_SB_FILEWRITE_ERR_EID);

    UT_ClearEventHistory();
    CFE_SB_BackgroundFileEventHandler(&State, CFE_FS_FileWriteEvent_CREATE_ERROR, OS_ERROR, 10, 0, 0);
    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

    UT_ClearEventHistory();
    CFE_SB_BackgroundFileEventHandler(&State, CFE_FS_FileWriteEvent_UNDEFINED, OS_ERROR, 0, 0, 0);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test write pipe information data getter for background file write
*/
void Test_SB_Cmds_MapInfoDataGetter(void)
{
    CFE_SB_PipeId_t                  PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                  PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                  PipeId3   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t                   MsgId0    = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t                   MsgId1    = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t                   MsgId2    = SB_UT_TLM_MID3;
    CFE_SB_MsgId_t                   MsgId3    = SB_UT_TLM_MID4;
    CFE_SB_MsgId_t                   MsgId4    = SB_UT_TLM_MID5;
    CFE_SB_MsgId_t                   MsgId5    = SB_UT_TLM_MID6;
    uint16                           PipeDepth = 10;
    void *                           LocalBuffer;
    size_t                           LocalBufSize;
    CFE_SB_BackgroundFileStateInfo_t State;

    /* Create some map info */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId2));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId2, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId3, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId4, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId5, PipeId2));

    memset(&State, 0, sizeof(State));
    LocalBuffer  = NULL;
    LocalBufSize = 0;

    UtAssert_BOOL_FALSE(CFE_SB_WriteMsgMapInfoDataGetter(&State, 0, &LocalBuffer, &LocalBufSize));
    UtAssert_NOT_NULL(LocalBuffer);
    UtAssert_NONZERO(LocalBufSize);

    UtAssert_BOOL_TRUE(
        CFE_SB_WriteMsgMapInfoDataGetter(&State, CFE_PLATFORM_SB_MAX_MSG_IDS, &LocalBuffer, &LocalBufSize));
    UtAssert_NULL(LocalBuffer);
    UtAssert_ZERO(LocalBufSize);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));
}

/*
** Test write map information command nominal path
*/
void Test_SB_Cmds_MapInfoDef(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_WriteMapInfoCmd_t Cmd;
    } WriteMapInfo;
    CFE_SB_PipeId_t PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId3   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId0    = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t  MsgId1    = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t  MsgId2    = SB_UT_TLM_MID3;
    CFE_SB_MsgId_t  MsgId3    = SB_UT_TLM_MID4;
    CFE_SB_MsgId_t  MsgId4    = SB_UT_TLM_MID5;
    CFE_SB_MsgId_t  MsgId5    = SB_UT_TLM_MID6;
    uint16          PipeDepth = 10;

    memset(&WriteMapInfo, 0, sizeof(WriteMapInfo));

    /* Create some map info */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId2));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId2, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId3, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId4, PipeId3));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId5, PipeId2));

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteMapInfo.SBBuf), sizeof(WriteMapInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_MAP_INFO_CC);

    CFE_UtAssert_EVENTCOUNT(10);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Also test with a bad file name - should generate CFE_SB_SND_RTG_ERR1_EID */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteMapInfo.SBBuf), sizeof(WriteMapInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_MAP_INFO_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

    /* Bad Size */
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteMapInfo.SBBuf), 0, UT_TPID_CFE_SB_CMD_WRITE_MAP_INFO_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));
}

/*
** Test write map information command when already pending
*/
void Test_SB_Cmds_MapInfoAlreadyPending(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_WriteMapInfoCmd_t Cmd;
    } WriteMapInfo;

    memset(&WriteMapInfo, 0, sizeof(WriteMapInfo));

    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);

    strncpy(WriteMapInfo.Cmd.Payload.Filename, "MapTstFile", sizeof(WriteMapInfo.Cmd.Payload.Filename) - 1);
    WriteMapInfo.Cmd.Payload.Filename[sizeof(WriteMapInfo.Cmd.Payload.Filename) - 1] = '\0';

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(WriteMapInfo.SBBuf), sizeof(WriteMapInfo.Cmd),
                    UT_TPID_CFE_SB_CMD_WRITE_MAP_INFO_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);
}

/*
** Test command to enable a specific route using a valid route
*/
void Test_SB_Cmds_EnRouteValParam(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_SB_EnableRouteCmd_t Cmd;
    } EnableRoute;
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 5;

    memset(&EnableRoute, 0, sizeof(EnableRoute));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "EnRouteTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    EnableRoute.Cmd.Payload.MsgId = MsgId;
    EnableRoute.Cmd.Payload.Pipe  = PipeId;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableRoute.SBBuf), sizeof(EnableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE2_EID);

    /* Bad Size */
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableRoute.SBBuf), 0, UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test command to enable a specific route using a non-existent route
*/
void Test_SB_Cmds_EnRouteNonExist(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_SB_EnableRouteCmd_t Cmd;
    } EnableRoute;
    CFE_SB_PipeId_t PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 5;

    memset(&EnableRoute, 0, sizeof(EnableRoute));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "EnRouteTestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "EnRouteTestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId1));
    EnableRoute.Cmd.Payload.MsgId = MsgId;
    EnableRoute.Cmd.Payload.Pipe  = PipeId2;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableRoute.SBBuf), sizeof(EnableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE1_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
}

/*
** Test command to enable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_EnRouteInvParam(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_SB_EnableRouteCmd_t Cmd;
    } EnableRoute;

    memset(&EnableRoute, 0, sizeof(EnableRoute));

    EnableRoute.Cmd.Payload.MsgId = SB_UT_LAST_VALID_MID;
    EnableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableRoute.SBBuf), sizeof(EnableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE3_EID);
}

/*
** Test command to enable a specific route using an invalid message ID
*/
void Test_SB_Cmds_EnRouteInvParam2(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_SB_EnableRouteCmd_t Cmd;
    } EnableRoute;

    memset(&EnableRoute, 0, sizeof(EnableRoute));

    EnableRoute.Cmd.Payload.MsgId = CFE_SB_INVALID_MSG_ID;
    EnableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableRoute.SBBuf), sizeof(EnableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE3_EID);
}

/*
** Test command to enable a specific route using a message ID greater
** than the maximum allowed
*/
void Test_SB_Cmds_EnRouteInvParam3(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_SB_EnableRouteCmd_t Cmd;
    } EnableRoute;

    memset(&EnableRoute, 0, sizeof(EnableRoute));

    EnableRoute.Cmd.Payload.MsgId = SB_UT_ALTERNATE_INVALID_MID;
    EnableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_0;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableRoute.SBBuf), sizeof(EnableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE3_EID);
}

/*
**  Test command to disable a specific route using a valid route
*/
void Test_SB_Cmds_DisRouteValParam(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_DisableRouteCmd_t Cmd;
    } DisableRoute;
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 5;

    memset(&DisableRoute, 0, sizeof(DisableRoute));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "DisRouteTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    DisableRoute.Cmd.Payload.MsgId = MsgId;
    DisableRoute.Cmd.Payload.Pipe  = PipeId;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableRoute.SBBuf), sizeof(DisableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE2_EID);

    /* Bad Size */
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableRoute.SBBuf), 0, UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test command to disable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_DisRouteNonExist(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_DisableRouteCmd_t Cmd;
    } DisableRoute;
    CFE_SB_PipeId_t PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 5;

    memset(&DisableRoute, 0, sizeof(DisableRoute));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "DisRouteTestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "DisRouteTestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId1));
    DisableRoute.Cmd.Payload.MsgId = MsgId;
    DisableRoute.Cmd.Payload.Pipe  = PipeId2;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableRoute.SBBuf), sizeof(DisableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE1_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
}

/*
** Test command to disable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_DisRouteInvParam(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_DisableRouteCmd_t Cmd;
    } DisableRoute;

    memset(&DisableRoute, 0, sizeof(DisableRoute));

    DisableRoute.Cmd.Payload.MsgId = SB_UT_LAST_VALID_MID;
    DisableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableRoute.SBBuf), sizeof(DisableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE3_EID);
}

/*
** Test command to disable a specific route using an invalid message ID
*/
void Test_SB_Cmds_DisRouteInvParam2(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_DisableRouteCmd_t Cmd;
    } DisableRoute;

    memset(&DisableRoute, 0, sizeof(DisableRoute));

    DisableRoute.Cmd.Payload.MsgId = CFE_SB_INVALID_MSG_ID;
    DisableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableRoute.SBBuf), sizeof(DisableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE3_EID);
}

/*
** Test command to disable a specific route using a message ID greater
** than the maximum allowed
*/
void Test_SB_Cmds_DisRouteInvParam3(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_DisableRouteCmd_t Cmd;
    } DisableRoute;

    memset(&DisableRoute, 0, sizeof(DisableRoute));

    DisableRoute.Cmd.Payload.MsgId = SB_UT_ALTERNATE_INVALID_MID;
    DisableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_0;

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableRoute.SBBuf), sizeof(DisableRoute.Cmd),
                    UT_TPID_CFE_SB_CMD_DISABLE_ROUTE_CC);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE3_EID);
}

/*
** Test send housekeeping information command
*/
void Test_SB_Cmds_SendHK(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_MSG_CommandHeader_t Cmd;
    } Housekeeping;
    CFE_SB_MsgId_t MsgIdCmd;
    CFE_MSG_Size_t Size;

    memset(&Housekeeping, 0, sizeof(Housekeeping));

    /* For HK command processing - set up FIRST */
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_SEND_HK, sizeof(Housekeeping.Cmd), false);

    /* For internal TransmitMsg call */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID);
    Size     = sizeof(CFE_SB_Global.HKTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_SB_ProcessCmdPipePkt(&Housekeeping.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_NO_SUBS_EID);
}

/*
** Test command to build and send a SB packet containing a complete
** list of current subscriptions
*/
void Test_SB_Cmds_SendPrevSubs(void)
{
    union
    {
        CFE_SB_Buffer_t          SBBuf;
        CFE_SB_SendPrevSubsCmd_t Cmd;
    } SendPrevSubs;
    CFE_SB_PipeId_t PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_BARE_TLM_MID3;
    uint16          MsgLim    = 4;
    uint16          PipeDepth = 50;
    int32           i;
    int32           NumEvts;
    CFE_MSG_Size_t  Size;
    CFE_SB_MsgId_t  MsgIdCmd;

    memset(&SendPrevSubs, 0, sizeof(SendPrevSubs));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    NumEvts = 2; /* one for each pipe create */

    /* Two full pkts to be sent plus five entries in a partial pkt, skipping MSGID 0x0D */
    for (i = 1; i < (CFE_SB_SUB_ENTRIES_PER_PKT * 2) + 6; i++)
    {
        /* Skip subscribing to ALLSUBS mid. This is the one we are testing.
         * MsgID for this in CCSDS v.1 was 0x180d so this MID did not appear in the
         * SB sub list. This results in multiple NO_SUBS_EID sent which we are not
         * testing here so it is irrelevant
         * For CCSDS v.2 CFE_SB_ALLSUBS_TLM_MID (0x0d) now appears in the
         * SB subscription list and thus we must skip adding 0x0D to the list
         * as we were going from MSGID 1-46 (0x01-0x2E)
         * */
        if (i != CFE_SB_ALLSUBS_TLM_MID)
        {
            NumEvts += 1;
            CFE_UtAssert_SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId1));
        }
    }

    CFE_UtAssert_SETUP(CFE_SB_SubscribeLocal(MsgId, PipeId2, MsgLim));

    /* Set up for dispatch FIRST */
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_SUB_RPT_CTL_SEND_PREV_SUBS_CC, sizeof(SendPrevSubs.Cmd), false);

    /* For 3 internal TransmitMsg calls */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size     = sizeof(CFE_SB_Global.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_SB_ProcessCmdPipePkt(&SendPrevSubs.SBBuf);

    NumEvts += 7; /* +1 for the subscribe, +6 for the SEND_PREV_SUBS_CC */

    /* Event count is only exact if there were no collisions */
    if (UT_EventIsInHistory(CFE_SB_HASHCOLLISION_EID))
    {
        UtAssert_UINT32_GTEQ(UT_GetNumEventsSent(), NumEvts + 1);
    }
    else
    {
        CFE_UtAssert_EVENTCOUNT(NumEvts);
    }

    /* Round out the number to three full pkts in order to test branch path
     * coverage, MSGID 0x0D was skipped in previous subscription loop
     */
    for (; i < (CFE_SB_SUB_ENTRIES_PER_PKT * 3) + 1; i++)
    {
        CFE_UtAssert_SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId1));
        NumEvts += 1;
    }

    /* Additional unique local route to hit while loop destptr == NULL branch */
    CFE_UtAssert_SETUP(CFE_SB_SubscribeLocal(CFE_SB_ValueToMsgId(i), PipeId2, MsgLim));

    /* Set up for dispatch FIRST */
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_SUB_RPT_CTL_SEND_PREV_SUBS_CC, sizeof(SendPrevSubs.Cmd), false);

    /* For 3 internal TransmitMsg calls */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size     = sizeof(CFE_SB_Global.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_SB_ProcessCmdPipePkt(&SendPrevSubs.SBBuf);

    NumEvts += 7; /* +1 for the local subscribe, +6 for the SEND_PREV_SUBS_CC */

    /* Event count is only exact if there were no collisions */
    if (UT_EventIsInHistory(CFE_SB_HASHCOLLISION_EID))
    {
        UtAssert_UINT32_GTEQ(UT_GetNumEventsSent(), NumEvts + 1);
    }
    else
    {
        CFE_UtAssert_EVENTCOUNT(NumEvts);
    }

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_NO_SUBS_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_FULL_SUB_PKT_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_PART_SUB_PKT_EID);

    /* Test error paths in the CFE_SB_SEND_PREV_SUBS_CC handling */
    UT_ClearEventHistory();

    /* Bad Size */
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(SendPrevSubs.SBBuf), 0,
                    UT_TPID_CFE_SB_SUB_RPT_CTL_SEND_PREV_SUBS_CC);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
}

/*
** Test command to enable subscription reporting
*/
void Test_SB_Cmds_SubRptOn(void)
{
    union
    {
        CFE_SB_Buffer_t                SBBuf;
        CFE_SB_EnableSubReportingCmd_t Cmd;
    } EnableSubReporting;

    memset(&EnableSubReporting, 0, sizeof(EnableSubReporting));

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableSubReporting.SBBuf), sizeof(EnableSubReporting.Cmd),
                    UT_TPID_CFE_SB_SUB_RPT_CTL_ENABLE_SUB_REPORTING_CC);

    CFE_UtAssert_EVENTCOUNT(0);

    /* Test error paths in the CFE_SB_ENABLE_SUB_REPORTING_CC handling */
    UT_ClearEventHistory();

    /* Bad Size */
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(EnableSubReporting.SBBuf), 0,
                    UT_TPID_CFE_SB_SUB_RPT_CTL_ENABLE_SUB_REPORTING_CC);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);
}

/*
** Test command to disable subscription reporting
*/
void Test_SB_Cmds_SubRptOff(void)
{
    union
    {
        CFE_SB_Buffer_t                 SBBuf;
        CFE_SB_DisableSubReportingCmd_t Cmd;
    } DisableSubReporting;

    memset(&DisableSubReporting, 0, sizeof(DisableSubReporting));

    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableSubReporting.SBBuf), sizeof(DisableSubReporting.Cmd),
                    UT_TPID_CFE_SB_SUB_RPT_CTL_DISABLE_SUB_REPORTING_CC);

    CFE_UtAssert_EVENTCOUNT(0);

    /* Test error paths in the CFE_SB_DISABLE_SUB_REPORTING_CC handling */
    UT_ClearEventHistory();

    /* Bad Size */
    UT_CallTaskPipe(CFE_SB_ProcessCmdPipePkt, CFE_MSG_PTR(DisableSubReporting.SBBuf), 0,
                    UT_TPID_CFE_SB_SUB_RPT_CTL_DISABLE_SUB_REPORTING_CC);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);
}

/*
** Test command handler response to an invalid command code
*/
void Test_SB_Cmds_CmdUnexpCmdCode(void)
{
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_CMD_BAD_FCNCODE, sizeof(CFE_MSG_CommandHeader_t), true);
    CFE_SB_ProcessCmdPipePkt((CFE_SB_Buffer_t *)NULL);
    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_SB_BAD_CMD_CODE_EID);

    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_SUB_RPT_CTRL_BAD_FCNCODE, sizeof(CFE_MSG_CommandHeader_t), true);
    CFE_SB_ProcessCmdPipePkt((CFE_SB_Buffer_t *)NULL);
}

/*
** Test command handler response to an incorrect length
*/
void Test_SB_Cmds_BadCmdLength(void)
{
    /*
     * Just choosing "EnableRoute" command here as it has a non-empty payload
     */

    /*
     * Note the length validation failure paths have already been checked
     * in the other command processor tests.  The only thing unique about
     * this dedicated test is that it does so with a non-zero size
     */
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_CMD_ENABLE_ROUTE_CC, sizeof(CFE_SB_EnableRouteCmd_t) - 1, true);
    CFE_SB_ProcessCmdPipePkt(NULL);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);
}

/*
** Test command handler response to an invalid message ID
*/
void Test_SB_Cmds_UnexpMsgId(void)
{
    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_SB_BAD_MSGID, sizeof(CFE_MSG_CommandHeader_t), true);
    CFE_SB_ProcessCmdPipePkt(NULL);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_BAD_MSGID_EID);
}

/*
** Function for calling early initialization tests
*/
void Test_SB_EarlyInit(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_EarlyInit_SemCreateError);
    SB_UT_ADD_SUBTEST(Test_SB_EarlyInit_PoolCreateError);
    SB_UT_ADD_SUBTEST(Test_SB_EarlyInit_NoErrors);
}

/*
** Test early initialization response to a semaphore create failure
*/
void Test_SB_EarlyInit_SemCreateError(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERR_NO_FREE_IDS);
    UtAssert_INT32_EQ(CFE_SB_EarlyInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
}

/*
** Test early initialization response to a pool create ex failure
*/
void Test_SB_EarlyInit_PoolCreateError(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PoolCreateEx), 1, CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_EarlyInit(), CFE_ES_BAD_ARGUMENT);
}

/*
** Test successful early initialization
*/
void Test_SB_EarlyInit_NoErrors(void)
{
    /* Initialize global to nonzero to confirm resets */
    memset(&CFE_SB_Global, 0xFF, sizeof(CFE_SB_Global));
    CFE_SB_EarlyInit();
    CFE_UtAssert_SUCCESS(CFE_SB_EarlyInit());

    /* Confirm reset of values to cover reset requirements that are challenging operationaly */
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.CommandErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.SubscribeErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.PipeOptsErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.DuplicateSubscriptionsCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.GetPipeIdByNameErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.PipeOverflowErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.MsgLimitErrorCounter);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.MsgIdsInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.PipesInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.PeakPipesInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.MemInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.SubscriptionsInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.PeakSubscriptionsInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.PeakSBBuffersInUse);
}

/*
** Function for calling SB create pipe API test functions
*/
void Test_CreatePipe_API(void)
{
    SB_UT_ADD_SUBTEST(Test_CreatePipe_NullPtr);
    SB_UT_ADD_SUBTEST(Test_CreatePipe_ValPipeDepth);
    SB_UT_ADD_SUBTEST(Test_CreatePipe_InvalPipeDepth);
    SB_UT_ADD_SUBTEST(Test_CreatePipe_MaxPipes);
    SB_UT_ADD_SUBTEST(Test_CreatePipe_SamePipeName);
    SB_UT_ADD_SUBTEST(Test_CreatePipe_EmptyPipeName);
    SB_UT_ADD_SUBTEST(Test_CreatePipe_PipeName_NullPtr);
}

/*
** Test create pipe response to a null pipe ID pointer
*/
void Test_CreatePipe_NullPtr(void)
{
    uint16 PipeDepth = 10;

    /* This provides completion of code coverage in CFE_SB_GetAppTskName() */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetTaskInfo), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS); /* Avoids creating socket */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(NULL, PipeDepth, "TestPipe"), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_CR_PIPE_BAD_ARG_EID);
}

/*
** Test create pipe response to valid pipe depths
*/
void Test_CreatePipe_ValPipeDepth(void)
{
    CFE_SB_PipeId_t PipeIdReturned[2] = {CFE_SB_INVALID_PIPE, CFE_SB_INVALID_PIPE};

    CFE_UtAssert_SUCCESS(CFE_SB_CreatePipe(&PipeIdReturned[0], 1, "TestPipeMin"));
    CFE_UtAssert_SUCCESS(CFE_SB_CreatePipe(&PipeIdReturned[1], OS_QUEUE_MAX_DEPTH, "TestPipeMax"));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[0]));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[1]));
}

/*
** Test create pipe response to invalid pipe depths
*/
void Test_CreatePipe_InvalPipeDepth(void)
{
    CFE_SB_PipeId_t PipeIdReturned[3];

    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS); /* Avoid creating socket */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeIdReturned[0], 0, "TestPipe1"), CFE_SB_BAD_ARGUMENT);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeIdReturned[1], OS_QUEUE_MAX_DEPTH + 1, "TestPipeMaxDepPlus1"),
                      CFE_SB_BAD_ARGUMENT);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeIdReturned[2], 0xffff, "TestPipeffff"), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_CR_PIPE_BAD_ARG_EID);
}

/*
** Test create pipe response to too many pipes
*/
void Test_CreatePipe_MaxPipes(void)
{
    CFE_SB_PipeId_t PipeIdReturned[CFE_PLATFORM_SB_MAX_PIPES + 1];
    uint16          PipeDepth = 50;
    int32           i;
    char            PipeName[OS_MAX_API_NAME];

    /* Set up for OSAL to return OS_ERR_NO_FREE_IDS from OS_QueueCreate() */
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_ERR_NO_FREE_IDS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeIdReturned[0], PipeDepth, "NoFree"), CFE_SB_PIPE_CR_ERR);
    CFE_UtAssert_EVENTSENT(CFE_SB_CR_PIPE_NO_FREE_EID);
    /* confirm that CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter was incremented */
    UtAssert_INT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter, 1);
    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakPipesInUse, 0);
    UT_ClearEventHistory();

    /* Create maximum number of pipes + 1. Only one 'create pipe' failure
     * expected
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_ToIndex), 1 + CFE_PLATFORM_SB_MAX_PIPES, -1);
    for (i = 0; i < (CFE_PLATFORM_SB_MAX_PIPES + 1); i++)
    {
        snprintf(PipeName, sizeof(PipeName), "TestPipe%ld", (long)i);

        if (i < CFE_PLATFORM_SB_MAX_PIPES)
        {
            CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeIdReturned[i], PipeDepth, PipeName));
        }
        else
        {
            UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeIdReturned[i], PipeDepth, PipeName), CFE_SB_MAX_PIPES_MET);
        }
    }

    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakPipesInUse, CFE_PLATFORM_SB_MAX_PIPES);
    CFE_UtAssert_EVENTSENT(CFE_SB_MAX_PIPES_MET_EID);

    /* Clean up */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_PIPES; i++)
    {
        CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[i]));
    }

    /* Creating another pipe should work again, but should _NOT_ increment PeakPipes */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeIdReturned[0], PipeDepth, PipeName), CFE_SUCCESS);
    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakPipesInUse, CFE_PLATFORM_SB_MAX_PIPES);
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[0]));

    /*
     * Also validate the CFE_SB_CheckPipeDescSlotUsed() helper function in this test,
     * as it is used to determine when max pipes has been hit.
     */
    CFE_SB_Global.PipeTbl[1].PipeId = CFE_SB_PIPEID_C(UT_SB_MakePipeIdForIndex(1));
    CFE_SB_Global.PipeTbl[2].PipeId = CFE_SB_INVALID_PIPE;
    UtAssert_BOOL_TRUE(CFE_SB_CheckPipeDescSlotUsed(UT_SB_MakePipeIdForIndex(1)));
    UtAssert_BOOL_FALSE(CFE_SB_CheckPipeDescSlotUsed(UT_SB_MakePipeIdForIndex(2)));
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_ToIndex), 1, -1);
    UtAssert_BOOL_TRUE(CFE_SB_CheckPipeDescSlotUsed(UT_SB_MakePipeIdForIndex(1)));
}

/*
** Test create pipe response to duplicate pipe names
*/
void Test_CreatePipe_SamePipeName(void)
{
    CFE_SB_PipeId_t FirstPipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId      = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth   = 1;
    char            PipeName[]  = "Test_CFE_SB";

    /* Need to make sure OS_QueueCreate() stub fails on second call   */
    /* to mimic the failure expected when passing the same pipe name */
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 2, OS_ERR_NAME_TAKEN);

    /* First call to CFE_SB_CreatePipe() should succeed */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName));

    FirstPipeId = PipeId;

    /* Second call to CFE_SB_CreatePipe with same PipeName should fail */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName), CFE_SB_PIPE_CR_ERR);

    CFE_UtAssert_RESOURCEID_EQ(PipeId, FirstPipeId);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_CR_PIPE_NAME_TAKEN_EID);

    /* Call to CFE_SB_DeletePipe with the first pipe id created should work fine */
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test create pipe response to empty pipe name
*/
void Test_CreatePipe_EmptyPipeName(void)
{
    CFE_SB_PipeId_t PipeId     = SB_UT_PIPEID_0;
    uint16          PipeDepth  = 1;
    char            PipeName[] = "";

    /* Call to CFE_SB_CreatePipe with empty PipeName should fail */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName), CFE_SB_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter, 1);
}

/*

** Test create pipe response to a null pipe name pointer.
*/
void Test_CreatePipe_PipeName_NullPtr(void)
{
    CFE_SB_PipeId_t PipeId    = SB_UT_PIPEID_0;
    uint16          PipeDepth = 1;

    /* To fail the pipe create, force the OS_QueueCreate call to return some
     * type of error code.
     */
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_ERR_NO_FREE_IDS);

    /* Call to CFE_SB_CreatePipe with empty PipeName should fail */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, NULL), CFE_SB_PIPE_CR_ERR);

    UtAssert_INT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.CreatePipeErrorCounter, 1);
}

/*

** Function for calling SB delete pipe API test functions
*/
void Test_DeletePipe_API(void)
{
    SB_UT_ADD_SUBTEST(Test_DeletePipe_NoSubs);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_WithSubs);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_InvalidPipeId);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_InvalidPipeOwner);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_WithAppid);
}

/*
** Test delete pipe response to deleting a pipe with no subscriptions
*/
void Test_DeletePipe_NoSubs(void)
{
    CFE_SB_PipeId_t PipedId   = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SUCCESS(CFE_SB_DeletePipe(PipedId));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_DELETED_EID);
}

/*
** Test delete pipe response to deleting a pipe with subscriptions
*/
void Test_DeletePipe_WithSubs(void)
{
    CFE_SB_PipeId_t PipedId   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId0    = SB_UT_CMD_MID1;
    CFE_SB_MsgId_t  MsgId1    = SB_UT_CMD_MID2;
    CFE_SB_MsgId_t  MsgId2    = SB_UT_CMD_MID3;
    CFE_SB_MsgId_t  MsgId3    = SB_UT_CMD_MID4;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipedId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId1, PipedId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId2, PipedId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId3, PipedId));
    CFE_UtAssert_SUCCESS(CFE_SB_DeletePipe(PipedId));

    CFE_UtAssert_EVENTCOUNT(6);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_DELETED_EID);
}

/*
** Test delete pipe response to an invalid pipe ID
*/
void Test_DeletePipe_InvalidPipeId(void)
{
    CFE_SB_PipeId_t PipeId = SB_UT_ALTERNATE_INVALID_PIPEID;

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DEL_PIPE_ERR1_EID);
}

/*
** Test delete pipe response to an invalid pipe owner
*/
void Test_DeletePipe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipedId = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeD_t *PipeDscPtr;
    CFE_ES_AppId_t  RealOwner;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));

    /* Change owner of pipe through memory corruption */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipedId);
    RealOwner  = PipeDscPtr->AppId;

    /* Choose a value that is sure not to be owner */
    PipeDscPtr->AppId = UT_SB_AppID_Modify(RealOwner, 1);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipedId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_DEL_PIPE_ERR2_EID);

    /* Restore owner id and delete pipe since test is complete */
    PipeDscPtr->AppId = RealOwner;
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipedId));
}

/*
** Test successful pipe delete with app ID
*/
void Test_DeletePipe_WithAppid(void)
{
    CFE_SB_PipeId_t PipedId = CFE_SB_INVALID_PIPE;
    CFE_ES_AppId_t  AppId;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID1, PipedId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID2, PipedId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID3, PipedId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID4, PipedId));
    CFE_UtAssert_SETUP(CFE_ES_GetAppID(&AppId));

    CFE_UtAssert_SUCCESS(CFE_SB_DeletePipeWithAppId(PipedId, AppId));

    CFE_UtAssert_EVENTCOUNT(6);
}

/*
** Function for calling SB set pipe opts API test functions
*/
void Test_PipeOpts_API(void)
{
    SB_UT_ADD_SUBTEST(Test_SetPipeOpts_BadID);
    SB_UT_ADD_SUBTEST(Test_SetPipeOpts_NotOwner);
    SB_UT_ADD_SUBTEST(Test_SetPipeOpts);
    SB_UT_ADD_SUBTEST(Test_GetPipeOpts_BadID);
    SB_UT_ADD_SUBTEST(Test_GetPipeOpts_BadPtr);
    SB_UT_ADD_SUBTEST(Test_GetPipeOpts);
}

/*
** Function for calling SB get pipe name by id API test functions
*/
void Test_GetPipeName_API(void)
{
    SB_UT_ADD_SUBTEST(Test_GetPipeName_NullPtr);
    SB_UT_ADD_SUBTEST(Test_GetPipeName_InvalidId);
    SB_UT_ADD_SUBTEST(Test_GetPipeName);
}

/*
** Call GetPipeName with null pointer.
*/
void Test_GetPipeName_NullPtr(void)
{
    CFE_SB_PipeId_t PipeId = CFE_SB_INVALID_PIPE;
    char            NameBuf[10];

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe"));
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(NULL, OS_MAX_API_NAME, PipeId), CFE_SB_BAD_ARGUMENT);
    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_NULL_PTR_EID);
    UT_ClearEventHistory();

    UtAssert_INT32_EQ(CFE_SB_GetPipeName(NameBuf, 0, PipeId), CFE_SB_BAD_ARGUMENT);
    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_NULL_PTR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Call to GetPipeName with an invalid pipe id.
*/
void Test_GetPipeName_InvalidId(void)
{
    char            PipeName[OS_MAX_API_NAME];
    CFE_SB_PipeId_t PipeId = CFE_SB_INVALID_PIPE;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe"));

    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_ID_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Successful call to GetPipeName
*/
void Test_GetPipeName(void)
{
    char            PipeName[OS_MAX_API_NAME];
    CFE_SB_PipeId_t PipeId = SB_UT_PIPEID_0;

    OS_queue_prop_t queue_info = {"TestPipe1"};

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe1"));

    UT_SetDataBuffer(UT_KEY(OS_QueueGetInfo), &queue_info, sizeof(queue_info), false);

    CFE_UtAssert_SUCCESS(CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId));

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Function for calling SB get pipe id by name API test functions
*/
void Test_GetPipeIdByName_API(void)
{
    SB_UT_ADD_SUBTEST(Test_GetPipeIdByName_NullPtrs);
    SB_UT_ADD_SUBTEST(Test_GetPipeIdByName_InvalidName);
    SB_UT_ADD_SUBTEST(Test_GetPipeIdByName);
}

/*
** Call GetPipeIdByName with null pointers.
*/
void Test_GetPipeIdByName_NullPtrs(void)
{
    CFE_SB_PipeId_t PipeIDOut;

    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIDOut, NULL), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID);

    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(NULL, "invalid"), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID);
}

/*
** Call to GetPipeId with an invalid pipe name.
*/
void Test_GetPipeIdByName_InvalidName(void)
{
    CFE_SB_PipeId_t PipeIdOut;
    osal_id_t       OtherQueueId;

    UT_SetDeferredRetcode(UT_KEY(OS_QueueGetIdByName), 1, OS_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIdOut, "invalid"), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID);
    UT_ClearEventHistory();

    /* Set up a test case where OS_QueueGetIdByName() succeeds but returns an ID that
       is not an SB pipe (i.e. if it was a queue belonging to some app other than SB) */
    CFE_UtAssert_SETUP(OS_QueueCreate(&OtherQueueId, "nonsb", 4, 4, 0));
    UT_SetDataBuffer(UT_KEY(OS_QueueGetIdByName), &OtherQueueId, sizeof(OtherQueueId), false);

    UtAssert_INT32_EQ(CFE_SB_GetPipeIdByName(&PipeIdOut, "nonsb"), CFE_SB_BAD_ARGUMENT);
    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID);
    CFE_UtAssert_TEARDOWN(OS_QueueDelete(OtherQueueId));
}

/*
** Successful call to GetPipeIdByName
*/
void Test_GetPipeIdByName(void)
{
    CFE_SB_PipeId_t PipeId = SB_UT_PIPEID_0;
    CFE_SB_PipeId_t PipeIdOut;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe1"));

    UT_SetDataBuffer(UT_KEY(OS_QueueGetIdByName), &(CFE_SB_Global.PipeTbl[0].SysQueueId),
                     sizeof(CFE_SB_Global.PipeTbl[0].SysQueueId), false);

    CFE_UtAssert_SUCCESS(CFE_SB_GetPipeIdByName(&PipeIdOut, "TestPipe1"));
    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEIDBYNAME_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Try setting pipe options on an invalid pipe ID
*/
void Test_SetPipeOpts_BadID(void)
{
    UtAssert_INT32_EQ(CFE_SB_SetPipeOpts(SB_UT_ALTERNATE_INVALID_PIPEID, 0), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_SETPIPEOPTS_ID_ERR_EID);
}

/*
** Try setting pipe options when not pipe owner
*/
void Test_SetPipeOpts_NotOwner(void)
{
    CFE_SB_PipeId_t PipeID = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeD_t *PipeDscPtr;
    CFE_ES_AppId_t  OrigOwner;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeID);

    OrigOwner         = PipeDscPtr->AppId;
    PipeDscPtr->AppId = UT_SB_AppID_Modify(OrigOwner, 1);

    UtAssert_INT32_EQ(CFE_SB_SetPipeOpts(PipeID, 0), CFE_SB_BAD_ARGUMENT);

    PipeDscPtr->AppId = OrigOwner;

    CFE_UtAssert_EVENTSENT(CFE_SB_SETPIPEOPTS_OWNER_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeID));
}

/*
** Set pipe options
*/
void Test_SetPipeOpts(void)
{
    CFE_SB_PipeId_t PipeID = CFE_SB_INVALID_PIPE;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    CFE_UtAssert_SUCCESS(CFE_SB_SetPipeOpts(PipeID, 0));

    CFE_UtAssert_EVENTSENT(CFE_SB_SETPIPEOPTS_EID);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_SB_SetPipeOpts(PipeID, 0), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeID));
}

/*
** Try getting pipe options on an invalid pipe ID
*/
void Test_GetPipeOpts_BadID(void)
{
    uint8 Opts = 0;

    UtAssert_INT32_EQ(CFE_SB_GetPipeOpts(SB_UT_ALTERNATE_INVALID_PIPEID, &Opts), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEOPTS_ID_ERR_EID);
}

/*
** Try getting pipe options with a bad pointer
*/
void Test_GetPipeOpts_BadPtr(void)
{
    CFE_SB_PipeId_t PipeID = CFE_SB_INVALID_PIPE;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    UtAssert_INT32_EQ(CFE_SB_GetPipeOpts(PipeID, NULL), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEOPTS_PTR_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeID));
}

/*
** Successful call to GetPipeOpts
*/
void Test_GetPipeOpts(void)
{
    CFE_SB_PipeId_t PipeID = CFE_SB_INVALID_PIPE;
    uint8           Opts   = 0;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    CFE_UtAssert_SUCCESS(CFE_SB_GetPipeOpts(PipeID, &Opts));

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEOPTS_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeID));
}

/*
** Function for calling SB subscribe API test functions
*/
void Test_Subscribe_API(void)
{
    SB_UT_ADD_SUBTEST(Test_Subscribe_SubscribeEx);
    SB_UT_ADD_SUBTEST(Test_Subscribe_InvalidPipeId);
    SB_UT_ADD_SUBTEST(Test_Subscribe_InvalidMsgId);
    SB_UT_ADD_SUBTEST(Test_Subscribe_MaxMsgLim);
    SB_UT_ADD_SUBTEST(Test_Subscribe_DuplicateSubscription);
    SB_UT_ADD_SUBTEST(Test_Subscribe_LocalSubscription);
    SB_UT_ADD_SUBTEST(Test_Subscribe_MaxDestCount);
    SB_UT_ADD_SUBTEST(Test_Subscribe_MaxMsgIdCount);
    SB_UT_ADD_SUBTEST(Test_Subscribe_SendPrevSubs);
    SB_UT_ADD_SUBTEST(Test_Subscribe_PipeNonexistent);
    SB_UT_ADD_SUBTEST(Test_Subscribe_SubscriptionReporting);
    SB_UT_ADD_SUBTEST(Test_Subscribe_InvalidPipeOwner);
}

/*
** Test API to globally subscribe to a message
*/
void Test_Subscribe_SubscribeEx(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_CMD_MID;
    CFE_SB_Qos_t    Quality   = {0, 0};
    uint16          PipeDepth = 10;
    uint16          MsgLim    = 8;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    CFE_UtAssert_SUCCESS(CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test message subscription response to an invalid pipe ID
*/
void Test_Subscribe_InvalidPipeId(void)
{
    CFE_SB_PipeId_t PipeId = SB_UT_PIPEID_2;
    CFE_SB_MsgId_t  MsgId  = SB_UT_ALTERNATE_INVALID_MID;

    UtAssert_INT32_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUB_INV_PIPE_EID);
}

/*
** Test message subscription response to an invalid message ID
*/
void Test_Subscribe_InvalidMsgId(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_ALTERNATE_INVALID_MID;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    UtAssert_INT32_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUB_ARG_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test message subscription response to the maximum message limit
*/
void Test_Subscribe_MaxMsgLim(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_CMD_MID;
    CFE_SB_Qos_t    Quality   = {0, 0};
    uint16          PipeDepth = 10;
    uint16          MsgLim;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    MsgLim = 0xffff;

    CFE_UtAssert_SUCCESS(CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test message subscription response to a duplicate subscription
*/
void Test_Subscribe_DuplicateSubscription(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_CMD_MID;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    CFE_UtAssert_SUCCESS(CFE_SB_Subscribe(MsgId, PipeId));
    CFE_UtAssert_SUCCESS(CFE_SB_Subscribe(MsgId, PipeId));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_DUP_SUBSCRIP_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test API to locally subscribe to a message
*/
void Test_Subscribe_LocalSubscription(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 10;
    uint16          MsgLim    = 4;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    CFE_UtAssert_SUCCESS(CFE_SB_SubscribeLocal(MsgId, PipeId, MsgLim));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Test_Subscribe_LocalSubscription with message
     * limit greater than CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT
     */
    CFE_UtAssert_SUCCESS(CFE_SB_Unsubscribe(MsgId, PipeId));
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(MsgId, PipeId, UINT16_MAX), CFE_SUCCESS);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test message subscription response to reaching the maximum destination count
*/
void Test_Subscribe_MaxDestCount(void)
{
    CFE_SB_PipeId_t PipeId[CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1];
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    char            PipeName[OS_MAX_API_NAME];
    uint16          PipeDepth = 50;
    int32           i;

    /* Create pipes */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        snprintf(PipeName, sizeof(PipeName), "TestPipe%ld", (long)i);
        CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId[i], PipeDepth, PipeName));
    }

    /* Do subscriptions */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        if (i < CFE_PLATFORM_SB_MAX_DEST_PER_PKT)
        {
            CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId[i]));
        }
        else
        {
            UtAssert_INT32_EQ(CFE_SB_Subscribe(MsgId, PipeId[i]), CFE_SB_MAX_DESTS_MET);
        }
    }

    CFE_UtAssert_EVENTCOUNT((2 * CFE_PLATFORM_SB_MAX_DEST_PER_PKT) + 3);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_MAX_DESTS_MET_EID);

    /* Delete pipes */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId[i]));
    }
}

/*
** Test message subscription response to reaching the maximum message ID count
*/
void Test_Subscribe_MaxMsgIdCount(void)
{
    CFE_SB_PipeId_t PipeId0   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2   = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth = 50;
    int32           i;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));

    /* For code coverage set PeakMsgIdsInUse so it won't always increment */
    CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse = 1;

    for (i = 0; i < CFE_PLATFORM_SB_MAX_MSG_IDS + 1; i++)
    {
        if (i < CFE_PLATFORM_SB_MAX_MSG_IDS)
        {
            CFE_UtAssert_SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(1 + i), PipeId2));
        }
        else
        {
            UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(1 + i), PipeId2), CFE_SB_MAX_MSGS_MET);
        }
    }

    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse, CFE_PLATFORM_SB_MAX_MSG_IDS);
    CFE_UtAssert_EVENTSENT(CFE_SB_MAX_MSGS_MET_EID);

    CFE_UtAssert_SUCCESS(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(1), PipeId1));
    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse, CFE_PLATFORM_SB_MAX_MSG_IDS);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId0));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
}

/*
** Test obtaining the list of current message subscriptions
*/
void Test_Subscribe_SendPrevSubs(void)
{
    CFE_SB_PipeId_t          PipeId0   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t          PipeId1   = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t          PipeId2   = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t           MsgId0    = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t           MsgId1    = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t           MsgId2    = SB_UT_TLM_MID3;
    uint16                   PipeDepth = 50;
    CFE_SB_SendPrevSubsCmd_t SendPrevSubsMsg;
    CFE_SB_MsgId_t           MsgIdCmd;
    CFE_MSG_Size_t           Size;

    /* note that the message is not currently used or required,
     * but creating one and initializing it for completeness, in
     * case that changes in the future */
    memset(&SendPrevSubsMsg, 0, sizeof(SendPrevSubsMsg));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId0));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId1, PipeId0));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId2, PipeId0));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId2, PipeId1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId0, PipeId2));

    /* For internal TransmitMsg call */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size     = sizeof(CFE_SB_Global.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_UtAssert_SUCCESS(CFE_SB_SendPrevSubsCmd(&SendPrevSubsMsg));

    CFE_UtAssert_EVENTCOUNT(12);

    CFE_UtAssert_EVENTSENT(CFE_SB_PART_SUB_PKT_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId0));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
}

/*
** Test message subscription response to nonexistent pipe
*/
void Test_Subscribe_PipeNonexistent(void)
{
    CFE_SB_MsgId_t  MsgId  = SB_UT_CMD_MID;
    CFE_SB_PipeId_t PipeId = SB_UT_ALTERNATE_INVALID_PIPEID;

    UtAssert_INT32_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUB_INV_PIPE_EID);
}

/*
** Test enabling and disabling subscription reporting
*/
void Test_Subscribe_SubscriptionReporting(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    CFE_SB_Qos_t    Quality   = CFE_SB_DEFAULT_QOS;
    uint16          PipeDepth = 10;
    CFE_SB_MsgId_t  MsgIdRpt;
    CFE_MSG_Size_t  Size;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    /* Enable subscription reporting */
    CFE_SB_SetSubscriptionReporting(CFE_SB_ENABLE);

    /* For internal TransmitMsg call that will report subscription */
    MsgIdRpt = CFE_SB_ValueToMsgId(CFE_SB_ONESUB_TLM_MID);
    Size     = sizeof(CFE_SB_SingleSubscriptionTlm_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdRpt, sizeof(MsgIdRpt), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Subscribe to message: GLOBAL */
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    /* Unsubscribe so that a local subscription can be tested */
    CFE_UtAssert_SETUP(CFE_SB_Unsubscribe(MsgId, PipeId));

    /* Subscribe to message: LOCAL */
    CFE_UtAssert_SUCCESS(
        CFE_SB_SubscribeFull(MsgId, PipeId, Quality, CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT, CFE_SB_MSG_LOCAL));

    CFE_UtAssert_EVENTCOUNT(6);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RPT_EID);

    /* Disable subscription reporting */
    CFE_SB_SetSubscriptionReporting(CFE_SB_DISABLE);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test message subscription response to an invalid pipe owner
*/
void Test_Subscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeD_t *PipeDscPtr;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 10;
    CFE_ES_AppId_t  RealOwner;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    /* Change owner of pipe through memory corruption */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    RealOwner  = PipeDscPtr->AppId;

    /* Choose a value that is sure not to be owner */
    PipeDscPtr->AppId = UT_SB_AppID_Modify(RealOwner, 1);
    CFE_SB_Subscribe(MsgId, PipeId);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUB_INV_CALLER_EID);

    /* Restore owner id and delete pipe since test is complete */
    PipeDscPtr->AppId = RealOwner;
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Function for calling SB unsubscribe API test functions
*/
void Test_Unsubscribe_API(void)
{
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_Basic);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_Local);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_AppId);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_InvalParam);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_NoMatch);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_InvalidPipe);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_InvalidPipeOwner);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_FirstDestWithMany);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_MiddleDestWithMany);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_GetDestPtr);
}

/*
** Test API used to unsubscribe to a message (successful)
*/
void Test_Unsubscribe_Basic(void)
{
    CFE_SB_PipeId_t TestPipe  = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_INTERMEDIATE_VALID_MID;
    uint16          PipeDepth = 50;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    CFE_UtAssert_SUCCESS(CFE_SB_Unsubscribe(MsgId, TestPipe));

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Check unsubscribe after unsubscribe produces event */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_SB_Unsubscribe(MsgId, TestPipe));
    CFE_UtAssert_EVENTCOUNT(2);
    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_NO_SUBS_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));
}

/*
** Test API used to unsubscribe to a message with a specific AppId
*/
void Test_Unsubscribe_AppId(void)
{
    CFE_SB_PipeId_t TestPipe  = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;
    CFE_ES_AppId_t  CallerId;

    CFE_UtAssert_SETUP(CFE_ES_GetAppID(&CallerId));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    CFE_UtAssert_SUCCESS(CFE_SB_UnsubscribeWithAppId(MsgId, TestPipe, CallerId));

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));
}

/*
** Test CFE internal API used to locally unsubscribe to a message (successful)
*/
void Test_Unsubscribe_Local(void)
{
    CFE_SB_PipeId_t TestPipe  = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    CFE_UtAssert_SUCCESS(CFE_SB_UnsubscribeLocal(SB_UT_LAST_VALID_MID, TestPipe));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));
}

/*
** Test message unsubscription response to an invalid message ID
*/
void Test_Unsubscribe_InvalParam(void)
{
    CFE_SB_PipeId_t TestPipe = CFE_SB_INVALID_PIPE;
    CFE_ES_AppId_t  CallerId;
    CFE_SB_PipeD_t *PipeDscPtr;
    uint16          PipeDepth = 50;
    CFE_SB_PipeId_t SavedPipeId;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));

    /* Perform test using a bad message ID */
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(SB_UT_ALTERNATE_INVALID_MID, TestPipe), CFE_SB_BAD_ARGUMENT);

    /* Get the caller's Application ID */
    CFE_UtAssert_SUCCESS(CFE_ES_GetAppID(&CallerId));

    /* Perform test using a bad scope value */
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeFull(SB_UT_FIRST_VALID_MID, TestPipe, CFE_SB_MSG_LOCAL + 1, CallerId),
                      CFE_SB_BAD_ARGUMENT);

    /* Perform test using an invalid pipe ID for branch path coverage.
     * This situation cannot happen in normal circumstances since the
     * bad pipe ID is caught by CFE_SB_GetPipeIdx() before it gets to
     * CFE_SB_ValidatePipeId()
     */
    PipeDscPtr         = CFE_SB_LocatePipeDescByID(TestPipe);
    SavedPipeId        = CFE_SB_PipeDescGetID(PipeDscPtr);
    PipeDscPtr->PipeId = SB_UT_ALTERNATE_INVALID_PIPEID;
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(SB_UT_FIRST_VALID_MID, TestPipe), CFE_SB_BAD_ARGUMENT);

    /* We must restore the old value so CFE_SB_DeletePipe() works */
    PipeDscPtr->PipeId = SavedPipeId;

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_ARG_ERR_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_INV_PIPE_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));
}

/*
** Test message unsubscription response to a message ID that is not subscribed
*/
void Test_Unsubscribe_NoMatch(void)
{
    CFE_SB_PipeId_t TestPipe  = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;

    /* Create pipe, subscribe, unsubscribe */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe));
    CFE_UtAssert_SETUP(CFE_SB_Unsubscribe(MsgId, TestPipe));
    UT_ClearEventHistory();

    /* Check that unsubscribe to msgid that was never subscribed reports error */
    CFE_UtAssert_SUCCESS(CFE_SB_Unsubscribe(SB_UT_TLM_MID1, TestPipe));
    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_NO_SUBS_EID);
    UT_ClearEventHistory();

    /* Check that repeated unsubscribe to msgid that was subscribed reports error */
    CFE_UtAssert_SUCCESS(CFE_SB_Unsubscribe(MsgId, TestPipe));
    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_NO_SUBS_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));
}

/*
** Test message unsubscription response to an invalid pipe ID
*/
void Test_Unsubscribe_InvalidPipe(void)
{
    CFE_SB_PipeId_t TestPipe  = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(MsgId, SB_UT_ALTERNATE_INVALID_PIPEID), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_INV_PIPE_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));
}

/*
** Test message unsubscription response to an invalid pipe owner
*/
void Test_Unsubscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId  = SB_UT_TLM_MID;
    CFE_SB_PipeD_t *PipeDscPtr;
    CFE_ES_AppId_t  RealOwner;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    /* Change owner of pipe through memory corruption */
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    RealOwner  = PipeDscPtr->AppId;

    /* Choose a value that is sure not be owner */
    PipeDscPtr->AppId = UT_SB_AppID_Modify(RealOwner, 1);
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_INV_CALLER_EID);

    PipeDscPtr->AppId = RealOwner;

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test message unsubscription response to the first pipe destination when
** the message is subscribed to by multiple pipes
*/
void Test_Unsubscribe_FirstDestWithMany(void)
{
    CFE_SB_MsgId_t  MsgId     = SB_UT_CMD_MID;
    CFE_SB_PipeId_t TestPipe1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t TestPipe2 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t TestPipe3 = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth = 50;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe3, PipeDepth, "TestPipe3"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe2));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe3));

    CFE_UtAssert_SUCCESS(CFE_SB_Unsubscribe(MsgId, TestPipe1));

    CFE_UtAssert_EVENTCOUNT(7);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_REMOVED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe3));
}

/*
** Test message unsubscription response to a middle pipe destination when
** the message is subscribed to by multiple pipes
*/
void Test_Unsubscribe_MiddleDestWithMany(void)
{
    CFE_SB_MsgId_t  MsgId     = SB_UT_CMD_MID;
    CFE_SB_PipeId_t TestPipe1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t TestPipe2 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t TestPipe3 = CFE_SB_INVALID_PIPE;
    uint16          PipeDepth = 50;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe3, PipeDepth, "TestPipe3"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe2));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe3));

    CFE_UtAssert_SUCCESS(CFE_SB_Unsubscribe(MsgId, TestPipe2));

    CFE_UtAssert_EVENTCOUNT(7);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_REMOVED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe3));
}

/*
** Test message unsubscription by verifying the message destination pointer no
** longer points to the pipe
*/
void Test_Unsubscribe_GetDestPtr(void)
{
    CFE_SB_MsgId_t    MsgId     = SB_UT_CMD_MID;
    CFE_SB_PipeId_t   TestPipe1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t   TestPipe2 = CFE_SB_INVALID_PIPE;
    uint16            PipeDepth = 50;
    CFE_SBR_RouteId_t RouteId;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe1));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe2));
    CFE_UtAssert_SETUP(CFE_SB_Unsubscribe(MsgId, TestPipe2));

    /* For now just get route id and use it, will need update when stubbed */
    RouteId = CFE_SBR_GetRouteId(MsgId);
    UtAssert_NULL(CFE_SB_GetDestPtr(RouteId, TestPipe2));

    CFE_UtAssert_EVENTCOUNT(5);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_REMOVED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe2));
}

void Test_TransmitTxn_Init(void)
{
    /* Test case for:
     * void CFE_SB_TransmitTxn_Init(CFE_SB_TransmitTxn_State_t *TxnPtr, const void *RefMemPtr);
     */
    CFE_SB_TransmitTxn_State_t TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;
    uint32                     MyData;

    /* Call to ensure coverage */
    memset(&Txn, 0xEE, sizeof(Txn));
    MyData = 123;
    UtAssert_ADDRESS_EQ(Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &MyData), &TxnBuf);

    /* This should have cleared everything */
    UtAssert_ADDRESS_EQ(Txn->RefMemPtr, &MyData);
    UtAssert_ZERO(Txn->NumPipes);
    UtAssert_ZERO(Txn->NumPipeErrs);
    UtAssert_ZERO(Txn->TransactionEventId);
    UtAssert_ZERO(Txn->TimeoutMode);
    UtAssert_ZERO(Txn->ContentSize);
    UtAssert_BOOL_FALSE(Txn->IsEndpoint);
}

void Test_MessageTxn_SetEventAndStatus(void)
{
    /* Test function for:
     * void CFE_SB_MessageTxn_SetEventAndStatus(CFE_SB_MessageTxn_State_t *TxnPtr,
     *               uint16 EventId, CFE_Status_t Status)
     */
    CFE_SB_MessageTxn_State_t Txn;

    /*
     * although this function is used throughout the other tests, this is needed to target
     * certain branches/paths that don't get executed.  Namely - this should only store
     * the first error/event that occurs.
     */

    /* Set only an event ID first, then attempt to set both */
    memset(&Txn, 0, sizeof(Txn));

    UtAssert_BOOL_TRUE(CFE_SB_MessageTxn_IsOK(&Txn));

    CFE_SB_MessageTxn_SetEventAndStatus(&Txn, CFE_SB_MSG_TOO_BIG_EID, CFE_SUCCESS);
    UtAssert_UINT16_EQ(Txn.TransactionEventId, CFE_SB_MSG_TOO_BIG_EID);
    UtAssert_INT32_EQ(Txn.Status, CFE_SUCCESS);
    UtAssert_BOOL_FALSE(CFE_SB_MessageTxn_IsOK(&Txn));

    CFE_SB_MessageTxn_SetEventAndStatus(&Txn, CFE_SB_SEND_INV_MSGID_EID, CFE_SB_BAD_ARGUMENT);
    UtAssert_UINT16_EQ(Txn.TransactionEventId, CFE_SB_MSG_TOO_BIG_EID);
    UtAssert_INT32_EQ(Txn.Status, CFE_SB_BAD_ARGUMENT);
    UtAssert_BOOL_FALSE(CFE_SB_MessageTxn_IsOK(&Txn));

    /* Set only a status code first, then attempt to set both */
    memset(&Txn, 0, sizeof(Txn));

    CFE_SB_MessageTxn_SetEventAndStatus(&Txn, 0, CFE_SB_BUF_ALOC_ERR);
    UtAssert_UINT16_EQ(Txn.TransactionEventId, 0);
    UtAssert_INT32_EQ(Txn.Status, CFE_SB_BUF_ALOC_ERR);
    UtAssert_BOOL_FALSE(CFE_SB_MessageTxn_IsOK(&Txn));

    CFE_SB_MessageTxn_SetEventAndStatus(&Txn, CFE_SB_SEND_INV_MSGID_EID, CFE_SB_BAD_ARGUMENT);
    UtAssert_UINT16_EQ(Txn.TransactionEventId, CFE_SB_SEND_INV_MSGID_EID);
    UtAssert_INT32_EQ(Txn.Status, CFE_SB_BUF_ALOC_ERR);
    UtAssert_BOOL_FALSE(CFE_SB_MessageTxn_IsOK(&Txn));
}

void Test_MessageTxn_Timeout(void)
{
    /* Test case for:
     * void CFE_SB_MessageTxn_SetTimeout(CFE_SB_MessageTxn_State_t *TxnPtr, int32 Timeout);
     * int32 CFE_SB_MessageTxn_GetOsTimeout(const CFE_SB_MessageTxn_State_t *TxnPtr);
     */
    CFE_SB_MessageTxn_State_t Txn;
    OS_time_t                 TestTime;

    memset(&Txn, 0, sizeof(Txn));

    UtAssert_VOIDCALL(CFE_SB_MessageTxn_SetTimeout(&Txn, CFE_SB_POLL));
    UtAssert_INT32_EQ(CFE_SB_MessageTxn_GetOsTimeout(&Txn), OS_CHECK);

    UtAssert_VOIDCALL(CFE_SB_MessageTxn_SetTimeout(&Txn, CFE_SB_PEND_FOREVER));
    UtAssert_INT32_EQ(CFE_SB_MessageTxn_GetOsTimeout(&Txn), OS_PEND);

    TestTime = OS_TimeFromTotalSeconds(1000000000);
    UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &TestTime, sizeof(TestTime), false);
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_SetTimeout(&Txn, 100));

    UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &TestTime, sizeof(TestTime), false);
    UtAssert_INT32_EQ(CFE_SB_MessageTxn_GetOsTimeout(&Txn), 100);

    TestTime = OS_TimeAdd(TestTime, OS_TimeFromTotalMilliseconds(10));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &TestTime, sizeof(TestTime), false);
    UtAssert_INT32_EQ(CFE_SB_MessageTxn_GetOsTimeout(&Txn), 90);

    TestTime = OS_TimeAdd(TestTime, OS_TimeFromTotalSeconds(1));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_GetTime), &TestTime, sizeof(TestTime), false);
    UtAssert_INT32_EQ(CFE_SB_MessageTxn_GetOsTimeout(&Txn), OS_CHECK);

    UtAssert_VOIDCALL(CFE_SB_MessageTxn_SetTimeout(&Txn, -1000));
    UtAssert_INT32_EQ(Txn.Status, CFE_SB_BAD_ARGUMENT);
}

void Test_MessageTxn_SetupFromMsg(void)
{
    /* Test case for:
     * CFE_Status_t CFE_SB_TransmitTxn_SetupFromMsg(CFE_SB_MessageTxn_State_t *TxnPtr, const CFE_MSG_Message_t
     * *MsgPtr);
     */
    CFE_SB_MessageTxn_State_t Txn;
    CFE_MSG_Message_t         Msg;
    CFE_SB_MsgId_t            MsgId;
    CFE_MSG_Size_t            MsgSize;

    memset(&Msg, 0, sizeof(Msg));

    /* Transaction already failed case (no-op) */
    memset(&Txn, 0, sizeof(Txn));
    Txn.Status = -20;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_SetupFromMsg(&Txn, &Msg));
    UtAssert_INT32_EQ(Txn.Status, -20);

    /* CFE_MSG_GetMsgId() fail case */
    memset(&Txn, 0, sizeof(Txn));
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_GetMsgId), 1, -10);
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_SetupFromMsg(&Txn, &Msg));
    UtAssert_INT32_EQ(Txn.Status, -10);
    UtAssert_UINT32_EQ(Txn.TransactionEventId, CFE_SB_SEND_BAD_ARG_EID);

    /* Invalid MsgId case */
    memset(&Txn, 0, sizeof(Txn));
    MsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_SetupFromMsg(&Txn, &Msg));
    UtAssert_INT32_EQ(Txn.Status, CFE_SB_BAD_ARGUMENT);
    UtAssert_UINT32_EQ(Txn.TransactionEventId, CFE_SB_SEND_INV_MSGID_EID);

    /* CFE_MSG_GetSize() fail case */
    memset(&Txn, 0, sizeof(Txn));
    MsgId = SB_UT_TLM_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_GetSize), 1, -11);
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_SetupFromMsg(&Txn, &Msg));
    UtAssert_INT32_EQ(Txn.Status, -11);
    UtAssert_UINT32_EQ(Txn.TransactionEventId, CFE_SB_SEND_BAD_ARG_EID);

    /* Message too big case */
    memset(&Txn, 0, sizeof(Txn));
    MsgId = SB_UT_TLM_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    MsgSize = 1 + CFE_MISSION_SB_MAX_SB_MSG_SIZE;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_SetupFromMsg(&Txn, &Msg));
    UtAssert_INT32_EQ(Txn.Status, CFE_SB_MSG_TOO_BIG);
    UtAssert_UINT32_EQ(Txn.TransactionEventId, CFE_SB_MSG_TOO_BIG_EID);

    /* Nominal case */
    memset(&Txn, 0, sizeof(Txn));
    MsgId = SB_UT_TLM_MID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    MsgSize = sizeof(Msg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &MsgSize, sizeof(MsgSize), false);

    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_SetupFromMsg(&Txn, &Msg));
    UtAssert_INT32_EQ(Txn.Status, CFE_SUCCESS);
    UtAssert_ZERO(Txn.TransactionEventId);
    CFE_UtAssert_MSGID_EQ(Txn.RoutingMsgId, SB_UT_TLM_MID);
    UtAssert_UINT32_EQ(Txn.ContentSize, sizeof(Msg));
}

void Test_TransmitTxn_FindDestinations(void)
{
    /* Test case for:
     * void CFE_SB_TransmitTxn_FindDestinations(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_BufferD_t *BufDscPtr);
     */

    CFE_SB_TransmitTxn_State_t TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;
    CFE_SB_BufferD_t           BufDsc;
    CFE_SB_PipeD_t *           PipeDscPtr;
    CFE_SB_PipeId_t            PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t             MsgId  = SB_UT_TLM_MID;
    CFE_SBR_RouteId_t          RouteId;
    CFE_SB_DestinationD_t *    DestPtr;

    memset(&BufDsc, 0, sizeof(BufDsc));
    CFE_SB_TrackingListReset(&BufDsc.Link); /* so tracking list ops work */

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 2, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_SubscribeFull(MsgId, PipeId, CFE_SB_DEFAULT_QOS, 2, CFE_SB_MSG_GLOBAL));
    PipeDscPtr                 = CFE_SB_LocatePipeDescByID(PipeId);
    RouteId                    = CFE_SBR_GetRouteId(MsgId);
    DestPtr                    = CFE_SB_GetDestPtr(RouteId, PipeId);
    Txn                        = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    PipeDscPtr->PeakQueueDepth = 1;

    CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter = 0;

    /* No subscriber case */
    Txn->RoutingMsgId = CFE_SB_INVALID_MSG_ID;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_UINT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter, 1);
    UtAssert_UINT32_EQ(Txn->TransactionEventId, CFE_SB_SEND_NO_SUBS_EID);
    UtAssert_UINT32_EQ(BufDsc.UseCount, 0);

    /* Nominal Case 1 */
    memset(&BufDsc, 0, sizeof(BufDsc));
    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    CFE_SB_TrackingListReset(&BufDsc.Link); /* so tracking list ops work */
    Txn->RoutingMsgId = MsgId;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_UINT32_EQ(Txn->NumPipes, 1);
    CFE_UtAssert_RESOURCEID_EQ(Txn->PipeSet[0].PipeId, PipeId);
    UtAssert_UINT32_EQ(Txn->PipeSet[0].PendingEventId, 0);
    UtAssert_UINT32_EQ(BufDsc.UseCount, 1);
    UtAssert_UINT32_EQ(DestPtr->BuffCount, 1);
    UtAssert_UINT32_EQ(PipeDscPtr->CurrentQueueDepth, 1);
    UtAssert_UINT32_EQ(PipeDscPtr->PeakQueueDepth, 1);
    UtAssert_UINT32_EQ(PipeDscPtr->SendErrors, 0);
    UtAssert_UINT32_EQ(Txn->NumPipeErrs, 0);

    /* Nominal Case 2 */
    memset(&BufDsc, 0, sizeof(BufDsc));
    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    CFE_SB_TrackingListReset(&BufDsc.Link); /* so tracking list ops work */
    Txn->RoutingMsgId = MsgId;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_UINT32_EQ(Txn->NumPipes, 1);
    CFE_UtAssert_RESOURCEID_EQ(Txn->PipeSet[0].PipeId, PipeId);
    UtAssert_UINT32_EQ(Txn->PipeSet[0].PendingEventId, 0);
    UtAssert_UINT32_EQ(BufDsc.UseCount, 1);
    UtAssert_UINT32_EQ(DestPtr->BuffCount, 2);
    UtAssert_UINT32_EQ(PipeDscPtr->CurrentQueueDepth, 2);
    UtAssert_UINT32_EQ(PipeDscPtr->PeakQueueDepth, 2);
    UtAssert_UINT32_EQ(PipeDscPtr->SendErrors, 0);
    UtAssert_UINT32_EQ(Txn->NumPipeErrs, 0);

    /* MsgLim Error Case */
    memset(&BufDsc, 0, sizeof(BufDsc));
    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    CFE_SB_TrackingListReset(&BufDsc.Link); /* so tracking list ops work */
    Txn->RoutingMsgId = MsgId;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_UINT32_EQ(Txn->NumPipes, 1);
    CFE_UtAssert_RESOURCEID_EQ(Txn->PipeSet[0].PipeId, PipeId);
    UtAssert_UINT32_EQ(Txn->PipeSet[0].PendingEventId, CFE_SB_MSGID_LIM_ERR_EID);
    UtAssert_ZERO(BufDsc.UseCount);
    UtAssert_UINT32_EQ(DestPtr->BuffCount, 2);
    UtAssert_UINT32_EQ(PipeDscPtr->CurrentQueueDepth, 2);
    UtAssert_UINT32_EQ(PipeDscPtr->PeakQueueDepth, 2);
    UtAssert_UINT32_EQ(PipeDscPtr->SendErrors, 1);
    UtAssert_UINT32_EQ(Txn->NumPipeErrs, 1);
    DestPtr->BuffCount = 0;

    /* Destination Inactive Case */
    memset(&BufDsc, 0, sizeof(BufDsc));
    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    CFE_SB_TrackingListReset(&BufDsc.Link); /* so tracking list ops work */
    Txn->RoutingMsgId = MsgId;
    DestPtr->Active   = CFE_SB_INACTIVE;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_ZERO(Txn->NumPipes);
    UtAssert_UINT32_EQ(BufDsc.UseCount, 0);
    UtAssert_UINT32_EQ(DestPtr->BuffCount, 0);
    DestPtr->Active = CFE_SB_ACTIVE;

    /* Pipe "Ignore Mine" Option Case w/Matching AppID */
    Txn               = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    Txn->RoutingMsgId = MsgId;
    PipeDscPtr->Opts |= CFE_SB_PIPEOPTS_IGNOREMINE;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_ZERO(Txn->NumPipes);
    UtAssert_UINT32_EQ(BufDsc.UseCount, 0);
    UtAssert_UINT32_EQ(DestPtr->BuffCount, 0);

    /* Pipe "Ignore Mine" Option Case w/Non-Matching AppID */
    Txn               = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    Txn->RoutingMsgId = MsgId;
    PipeDscPtr->AppId = CFE_ES_APPID_UNDEFINED;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_UINT32_EQ(Txn->NumPipes, 1);
    UtAssert_UINT32_EQ(BufDsc.UseCount, 1);
    UtAssert_UINT32_EQ(DestPtr->BuffCount, 1);
    CFE_ES_GetAppID(&PipeDscPtr->AppId);
    PipeDscPtr->Opts &= ~CFE_SB_PIPEOPTS_IGNOREMINE;

    /* DestPtr List too long - this emulates a hypothetical bug in SBR allowing list to grow too long */
    /* Hack to make it infinite length */
    DestPtr->Next     = DestPtr;
    Txn               = CFE_SB_TransmitTxn_Init(&TxnBuf, &BufDsc.Content);
    Txn->RoutingMsgId = MsgId;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_FindDestinations(Txn, &BufDsc));
    UtAssert_UINT32_EQ(Txn->NumPipes, CFE_PLATFORM_SB_MAX_DEST_PER_PKT);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

void Test_TransmitTxn_PipeHandler(void)
{
    /* Test function for:
     * bool CFE_SB_TransmitTxn_PipeHandler(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_PipeSetEntry_t *ContextPtr, void
     * *Arg);
     */

    CFE_SB_TransmitTxn_State_t TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;
    CFE_SB_PipeId_t            PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t             MsgId  = SB_UT_TLM_MID;
    CFE_SB_BufferD_t           SBBufD;

    memset(&SBBufD, 0, sizeof(SBBufD));
    memset(&TxnBuf, 0, sizeof(TxnBuf));
    CFE_SB_TrackingListReset(&SBBufD.Link); /* so tracking list ops work */
    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &SBBufD.Content);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 3, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    SBBufD.DestRouteId = CFE_SBR_GetRouteId(MsgId);

    CFE_SB_LocatePipeDescByID(PipeId)->CurrentQueueDepth     = 1;
    CFE_SB_GetDestPtr(SBBufD.DestRouteId, PipeId)->BuffCount = 1;

    Txn->NumPipes                  = 6;
    Txn->NumPipeErrs               = 1;
    Txn->PipeSet[0].PipeId         = PipeId;
    Txn->PipeSet[1].PipeId         = PipeId;
    Txn->PipeSet[2].PipeId         = PipeId;
    Txn->PipeSet[2].PendingEventId = 1;
    Txn->PipeSet[3].PipeId         = PipeId;
    Txn->PipeSet[4].PipeId         = PipeId;
    Txn->PipeSet[5].PipeId         = SB_UT_ALTERNATE_INVALID_PIPEID;

    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 2, OS_QUEUE_FULL);
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);

    UtAssert_VOIDCALL(CFE_SB_MessageTxn_ProcessPipes(CFE_SB_TransmitTxn_PipeHandler, Txn, &SBBufD));
    UtAssert_STUB_COUNT(OS_QueuePut, 5);
    UtAssert_UINT32_EQ(Txn->NumPipeErrs, 5);
    UtAssert_UINT32_EQ(Txn->PipeSet[0].PendingEventId, 0);
    UtAssert_UINT32_EQ(Txn->PipeSet[1].PendingEventId, CFE_SB_Q_FULL_ERR_EID);
    UtAssert_UINT32_EQ(Txn->PipeSet[2].PendingEventId, 1);
    UtAssert_UINT32_EQ(Txn->PipeSet[3].PendingEventId, CFE_SB_Q_WR_ERR_EID);
    UtAssert_UINT32_EQ(Txn->PipeSet[4].PendingEventId, CFE_SB_Q_WR_ERR_EID);
    UtAssert_UINT32_EQ(Txn->PipeSet[5].PendingEventId, CFE_SB_Q_WR_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

void Test_TransmitTxn_Execute(void)
{
    /* Test case for:
     * CFE_Status_t CFE_SB_TransmitTxn_Execute(CFE_SB_MessageTxn_State_t *TxnPtr, CFE_SB_Buffer_t *BufPtr);
     */
    CFE_SB_BufferD_t           SBBufD;
    CFE_SB_TransmitTxn_State_t TxnBuf;
    CFE_SB_MessageTxn_State_t *Txn;
    CFE_ES_AppId_t             MyAppId;
    CFE_SB_PipeId_t            PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t             MsgId  = SB_UT_TLM_MID;

    memset(&SBBufD, 0, sizeof(SBBufD));
    memset(&TxnBuf, 0, sizeof(TxnBuf));
    CFE_SB_TrackingListReset(&SBBufD.Link); /* so tracking list ops work */
    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &SBBufD.Content);
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 2, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_ES_GetAppID(&MyAppId));

    /* no subs - this should still keep status as SUCCESS but trigger CFE_SB_SEND_NO_SUBS_EID event */
    SBBufD.AppId = MyAppId;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_Execute(Txn, &SBBufD.Content));
    UtAssert_STUB_COUNT(OS_QueuePut, 0);
    UtAssert_UINT32_EQ(Txn->TransactionEventId, CFE_SB_SEND_NO_SUBS_EID);
    UtAssert_INT32_EQ(Txn->Status, CFE_SUCCESS);
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(SBBufD.AppId));

    /* add a subscriber - nominal case */
    CFE_UtAssert_SETUP(CFE_SB_SubscribeFull(MsgId, PipeId, CFE_SB_DEFAULT_QOS, 2, CFE_SB_MSG_GLOBAL));
    Txn               = CFE_SB_TransmitTxn_Init(&TxnBuf, &SBBufD.Content);
    Txn->RoutingMsgId = MsgId;
    SBBufD.AppId      = MyAppId;
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_Execute(Txn, &SBBufD.Content));
    UtAssert_BOOL_TRUE(CFE_SB_MessageTxn_IsOK(Txn));
    UtAssert_STUB_COUNT(OS_QueuePut, 1);
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(SBBufD.AppId));

    /* error writing to the pipe - this also still keeps status as SUCCESS but trigger CFE_SB_Q_WR_ERR_EID */
    Txn               = CFE_SB_TransmitTxn_Init(&TxnBuf, &SBBufD.Content);
    Txn->RoutingMsgId = MsgId;
    SBBufD.AppId      = MyAppId;
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_Execute(Txn, &SBBufD.Content));
    UtAssert_BOOL_TRUE(CFE_SB_MessageTxn_IsOK(Txn));
    UtAssert_UINT32_EQ(Txn->PipeSet[0].PendingEventId, CFE_SB_Q_WR_ERR_EID);
    UtAssert_STUB_COUNT(OS_QueuePut, 2);
    UtAssert_BOOL_FALSE(CFE_RESOURCEID_TEST_DEFINED(SBBufD.AppId));

    /* Validation fail */
    Txn = CFE_SB_TransmitTxn_Init(&TxnBuf, &SBBufD.Content);
    UtAssert_VOIDCALL(CFE_SB_TransmitTxn_Execute(Txn, &SBBufD.Content));
    UtAssert_BOOL_FALSE(CFE_SB_MessageTxn_IsOK(Txn));
    UtAssert_INT32_EQ(Txn->Status, CFE_SB_BUFFER_INVALID);
    UtAssert_STUB_COUNT(OS_QueuePut, 2);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

void Test_MessageTxn_GetEventDetails(void)
{
    /* Test case for:
     * void CFE_SB_MessageTxn_GetEventDetails(const CFE_SB_MessageTxn_State_t *TxnPtr, CFE_ES_TaskId_t TskId, uint16
     * EventId,char *EvtMsg, size_t EvtMsgSize, const CFE_SB_PipeSetEntry_t *ContextPtr,CFE_EVS_EventType_Enum_t
     * *EventType, int32 *ReqBit);
     */

    CFE_SB_MessageTxn_State_t Txn;
    CFE_SB_PipeSetEntry_t     Entry;
    CFE_ES_TaskId_t           MyTskId;
    char                      EvtMsg[64];
    CFE_EVS_EventType_Enum_t  EvType;
    int32                     EvReqBit;
    CFE_SB_PipeId_t           PipeId;

    CFE_UtAssert_SETUP(CFE_ES_GetTaskID(&MyTskId));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 3, "TestPipe"));
    CFE_SB_MessageTxn_Init(&Txn, &Entry, 1, &PipeId);
    EvReqBit     = 0;
    EvType       = 0;
    Entry.PipeId = PipeId;

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(
        CFE_SB_MessageTxn_GetEventDetails(&Txn, NULL, 0, MyTskId, EvtMsg, sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_ZERO(EvtMsg[0]);
    UtAssert_ZERO(EvReqBit);

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, NULL, CFE_SB_SEND_BAD_ARG_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_SEND_BAD_ARG_EID_BIT);

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, NULL, CFE_SB_SEND_INV_MSGID_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_SEND_INV_MSGID_EID_BIT);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, NULL, CFE_SB_MSG_TOO_BIG_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_MSG_TOO_BIG_EID_BIT);

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, NULL, CFE_SB_SEND_NO_SUBS_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_SEND_NO_SUBS_EID_BIT);

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, &Entry, CFE_SB_GET_BUF_ERR_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_GET_BUF_ERR_EID_BIT);

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, &Entry, CFE_SB_MSGID_LIM_ERR_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_MSGID_LIM_ERR_EID_BIT);

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, &Entry, CFE_SB_Q_FULL_ERR_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_Q_FULL_ERR_EID_BIT);

    memset(EvtMsg, 0xAA, sizeof(EvtMsg));
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_GetEventDetails(&Txn, NULL, CFE_SB_Q_WR_ERR_EID, MyTskId, EvtMsg,
                                                        sizeof(EvtMsg), &EvType, &EvReqBit));
    UtAssert_NONZERO(EvtMsg[0]);
    UtAssert_NOT_NULL(memchr(EvtMsg, 0, sizeof(EvtMsg)));
    UtAssert_UINT32_EQ(EvReqBit, CFE_SB_Q_WR_ERR_EID_BIT);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

void Test_MessageTxn_ReportSingleEvent(void)
{
    /* Test case for:
     * bool CFE_SB_MessageTxn_ReportSingleEvent(uint16 EventId, const CFE_SB_MessageTxn_State_t *TxnPtr, const
     * CFE_SB_PipeSetEntry_t *ContextPtr);
     */
    CFE_SB_PipeSetEntry_t     Entry;
    CFE_SB_MessageTxn_State_t Txn;
    CFE_SB_PipeId_t           PipeId;
    CFE_ES_TaskId_t           MyTskId;

    memset(&Txn, 0, sizeof(Txn));
    memset(&Entry, 0, sizeof(Entry));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 3, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_ES_GetTaskID(&MyTskId));
    Txn.NumPipes = 1;
    Txn.PipeSet  = &Entry;
    Entry.PipeId = PipeId;

    /* Event ID == 0 is reserved for no event, nothing to report */
    UT_ClearEventHistory();
    UtAssert_BOOL_FALSE(CFE_SB_MessageTxn_ReportSingleEvent(&Txn, NULL, 0));
    CFE_UtAssert_EVENTCOUNT(0);

    /* No subscribers should be an information event (not an error) */
    UT_ClearEventHistory();
    UtAssert_BOOL_FALSE(CFE_SB_MessageTxn_ReportSingleEvent(&Txn, NULL, CFE_SB_SEND_NO_SUBS_EID));
    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_NO_SUBS_EID);

    /* Queue write failure should be an error */
    /* Note that with context != NULL, it looks up the pipe name, which generates a debug event */
    UT_ClearEventHistory();
    UtAssert_BOOL_TRUE(CFE_SB_MessageTxn_ReportSingleEvent(&Txn, &Entry, CFE_SB_Q_WR_ERR_EID));
    CFE_UtAssert_EVENTCOUNT(2);
    CFE_UtAssert_EVENTSENT(CFE_SB_Q_WR_ERR_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_EID);

    /* Check that the loop-avoidance works */
    CFE_SB_RequestToSendEvent(MyTskId, CFE_SB_MSG_TOO_BIG_EID_BIT);
    UT_ClearEventHistory();
    UtAssert_BOOL_TRUE(CFE_SB_MessageTxn_ReportSingleEvent(&Txn, NULL, CFE_SB_MSG_TOO_BIG_EID));
    CFE_UtAssert_EVENTCOUNT(0);
    CFE_SB_FinishSendEvent(MyTskId, CFE_SB_MSG_TOO_BIG_EID_BIT);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

void Test_MessageTxn_ReportEvents(void)
{
    /* Test case for:
     * void CFE_SB_MessageTxn_ReportEvents(const CFE_SB_MessageTxn_State_t *TxnPtr);
     */
    CFE_SB_PipeSetEntry_t     PipeSetEntry;
    CFE_SB_MessageTxn_State_t Txn;

    memset(&Txn, 0, sizeof(Txn));
    Txn.PipeSet  = &PipeSetEntry;
    Txn.NumPipes = 1;
    Txn.MaxPipes = 1;

    /* nominal */
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_ReportEvents(&Txn));
    UtAssert_ZERO(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter);

    /* with an event at the transaction level, known to be an error */
    Txn.TransactionEventId = CFE_SB_MSG_TOO_BIG_EID;
    Txn.IsTransmit         = true;
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_ReportEvents(&Txn));
    UtAssert_UINT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter, 1);

    /* with some undefined/unknown event at the transaction level, not an error */
    Txn.TransactionEventId = 0xFFFF;
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_ReportEvents(&Txn));
    UtAssert_UINT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter, 1);

    /* with an event at the pipe level, known to be an error */
    Txn.TransactionEventId        = 0;
    Txn.PipeSet[0].PendingEventId = CFE_SB_Q_FULL_ERR_EID;
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_ReportEvents(&Txn));
    UtAssert_UINT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter, 2);

    /* with some undefined/unknown event at the pipe level, not an error */
    Txn.PipeSet[0].PendingEventId = 0xFFFF;
    UtAssert_VOIDCALL(CFE_SB_MessageTxn_ReportEvents(&Txn));
    UtAssert_UINT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter, 2);
}

/*
** Function for calling SB send message API test functions
*/
void Test_TransmitMsg_API(void)
{
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_NullPtr);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_NoSubscribers);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_MaxMsgSizePlusOne);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_BasicSend);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_UpdateHeader);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_QueuePutError);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_PipeFull);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_MsgLimitExceeded);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_GetPoolBufErr);
    SB_UT_ADD_SUBTEST(Test_TransmitBuffer_IncrementSeqCnt);
    SB_UT_ADD_SUBTEST(Test_TransmitBuffer_NoIncrement);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_ZeroCopyBufferValidate);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_DisabledDestination);

    SB_UT_ADD_SUBTEST(Test_MessageTxn_SetEventAndStatus);
    SB_UT_ADD_SUBTEST(Test_MessageTxn_SetupFromMsg);
    SB_UT_ADD_SUBTEST(Test_MessageTxn_Timeout);
    SB_UT_ADD_SUBTEST(Test_MessageTxn_GetEventDetails);
    SB_UT_ADD_SUBTEST(Test_MessageTxn_ReportSingleEvent);
    SB_UT_ADD_SUBTEST(Test_MessageTxn_ReportEvents);

    SB_UT_ADD_SUBTEST(Test_TransmitTxn_Init);
    SB_UT_ADD_SUBTEST(Test_TransmitTxn_FindDestinations);
    SB_UT_ADD_SUBTEST(Test_TransmitTxn_PipeHandler);
    SB_UT_ADD_SUBTEST(Test_TransmitTxn_Execute);

    SB_UT_ADD_SUBTEST(Test_AllocateMessageBuffer);
    SB_UT_ADD_SUBTEST(Test_ReleaseMessageBuffer);
}

/*
** Test response to sending a null message on the software bus
*/
void Test_TransmitMsg_NullPtr(void)
{
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(NULL, true), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_BAD_ARG_EID);
}

/*
** Test response to sending a message which has no subscribers
*/
void Test_TransmitMsg_NoSubscribers(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_NO_SUBS_EID);
}

/*
** Test response to sending a message with the message size larger than allowed
*/
void Test_TransmitMsg_MaxMsgSizePlusOne(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_MSG_Size_t   Size = CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter = 0;

    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true), CFE_SB_MSG_TOO_BIG);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_MSG_TOO_BIG_EID);
    UtAssert_INT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter, 1);
}

/*
** Test successfully sending a message on the software bus
*/
void Test_TransmitMsg_BasicSend(void)
{
    CFE_SB_PipeId_t  PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t   MsgId  = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/* Set sequence count hook */
static int32 UT_CheckSetSequenceCount(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                      const UT_StubContext_t *Context)
{
    CFE_MSG_SequenceCount_t *SeqCntPtr = UserObj;

    *SeqCntPtr = UT_Hook_GetArgValueByName(Context, "SeqCnt", CFE_MSG_SequenceCount_t);

    return StubRetcode;
}

/*
** Test successful send with header update
*/
void Test_TransmitMsg_UpdateHeader(void)
{
    CFE_SB_PipeId_t         PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t          MsgId  = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t        TlmPkt;
    CFE_MSG_Size_t          Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t          Type      = CFE_MSG_Type_Tlm;
    uint32                  PipeDepth = 10;
    CFE_MSG_SequenceCount_t SeqCnt;
    CFE_MSG_SequenceCount_t SeqCntExpected;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    /* Set up hook for checking CFE_MSG_SetSequenceCount calls */
    UT_SetHookFunction(UT_KEY(CFE_MSG_SetSequenceCount), UT_CheckSetSequenceCount, &SeqCnt);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "SeqCntTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    /* Note the Sequence count value doesn't really matter, just set unique to confirm use */
    SeqCntExpected = 1;
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), SeqCntExpected);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 1);
    UtAssert_STUB_COUNT(CFE_MSG_OriginationAction, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 1);
    UtAssert_INT32_EQ(SeqCnt, SeqCntExpected);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), false));

    /* Assert sequence count wasn't set */
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 1);
    UtAssert_STUB_COUNT(CFE_MSG_OriginationAction, 1);

    SeqCntExpected = 2;
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), SeqCntExpected);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));
    UtAssert_INT32_EQ(SeqCnt, SeqCntExpected);
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 2);
    UtAssert_STUB_COUNT(CFE_MSG_OriginationAction, 2);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 2);

    CFE_UtAssert_EVENTCOUNT(2);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_SETUP(CFE_SB_Unsubscribe(MsgId, PipeId)); /* should have no subscribers now */

    SeqCntExpected = 3;
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), SeqCntExpected);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true)); /* increment to 3 */
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 3);
    UtAssert_STUB_COUNT(CFE_MSG_OriginationAction, 3);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 3);

    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId)); /* resubscribe so we can receive a msg */

    SeqCntExpected = 4;
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), SeqCntExpected);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true)); /* increment to 4 */
    UtAssert_INT32_EQ(SeqCnt, SeqCntExpected);
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 4);
    UtAssert_STUB_COUNT(CFE_MSG_OriginationAction, 4);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test send message response to a socket queue 'put' error
*/
void Test_TransmitMsg_QueuePutError(void)
{
    CFE_SB_PipeId_t  PipeId4Error = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t   MsgId        = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId4Error, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId4Error));
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_WR_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId4Error));
}

/*
** Test send message response when the socket queue is full
*/
void Test_TransmitMsg_PipeFull(void)
{
    CFE_SB_PipeId_t  PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t   MsgId  = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 1;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "PipeFullTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* This send should pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    /* Tell the QueuePut stub to return OS_QUEUE_FULL on its next call */
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_QUEUE_FULL);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* Pipe overflow causes TransmitMsg to return CFE_SUCCESS */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_FULL_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test send message response to too many messages sent to the pipe
*/
void Test_TransmitMsg_MsgLimitExceeded(void)
{
    CFE_SB_PipeId_t  PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t   MsgId  = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 5;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "MsgLimTestPipe"));

    /* Set maximum allowed messages on the pipe at one time to 1 */
    CFE_SB_SubscribeEx(MsgId, PipeId, CFE_SB_DEFAULT_QOS, 1);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* First send should pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* This send should produce a MsgId to Pipe Limit Exceeded message, but
     * return success
     */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_MSGID_LIM_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test send message response to a buffer descriptor allocation failure
*/
void Test_TransmitMsg_GetPoolBufErr(void)
{
    CFE_SB_PipeId_t  PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t   MsgId  = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    PipeDepth = 1;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "GetPoolErrPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Have GetPoolBuf stub return error on its next call (buf descriptor
     * allocation failed)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true), CFE_SB_BUF_ALOC_ERR);

    CFE_UtAssert_EVENTCOUNT(3);
    CFE_UtAssert_EVENTSENT(CFE_SB_GET_BUF_ERR_EID);

    /* Repeat buf descriptor allocation failed with event denied */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_TaskID_ToIndex), 1, -1);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true), CFE_SB_BUF_ALOC_ERR);

    /* Confirm no additional events sent */
    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test getting a pointer to a buffer for zero copy mode with buffer
** allocation failures
*/
void Test_AllocateMessageBuffer(void)
{
    size_t MsgSize = 10;
    uint32 MemUse;

    /* Attempt to allocate a message buffer greater than the max size */
    UtAssert_NULL(CFE_SB_AllocateMessageBuffer(CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1));

    /* Attempt to allocate a message buffer from a non-CFE context */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_NULL(CFE_SB_AllocateMessageBuffer(MsgSize));

    /* Have GetPoolBuf stub return error on its next call (buf descriptor
     * allocation failed)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_NULL(CFE_SB_AllocateMessageBuffer(MsgSize));

    CFE_UtAssert_EVENTCOUNT(0);

    /* Increase the peak memory and buffers in use above the expected values in
     * order to exercise branch paths
     */

    /* predict memory use for a given descriptor (this needs to match what impl does) */
    MemUse                                              = MsgSize + offsetof(CFE_SB_BufferD_t, Content);
    CFE_SB_Global.StatTlmMsg.Payload.MemInUse           = 0;
    CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse       = MemUse + 10;
    CFE_SB_Global.StatTlmMsg.Payload.PeakSBBuffersInUse = CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse + 2;
    UtAssert_NOT_NULL(CFE_SB_AllocateMessageBuffer(MsgSize));

    UtAssert_INT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse, MemUse + 10); /* unchanged */
    UtAssert_INT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.MemInUse, MemUse);          /* predicted value */

    UtAssert_INT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakSBBuffersInUse,
                      CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse + 1);

    CFE_UtAssert_EVENTCOUNT(0);
}

void Test_TransmitMsg_ZeroCopyBufferValidate(void)
{
    CFE_SB_Buffer_t * SendPtr;
    CFE_SB_BufferD_t  BadZeroCpyBuf;
    CFE_SB_BufferD_t *BufDscPtr;

    /* Create a real/valid Zero Copy handle via the API */
    SendPtr = CFE_SB_AllocateMessageBuffer(sizeof(SB_UT_Test_Tlm_t));
    if (SendPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }

    /* Create an invalid Zero Copy handle that is not NULL but refers to a
     * descriptor which is NOT from CFE_SB_AllocateMessageBuffer(). */
    memset(&BadZeroCpyBuf, 0, sizeof(BadZeroCpyBuf));

    /* Non-null buffer pointer but Non Zero-Copy => CFE_SB_BUFFER_INVALID */
    UtAssert_INT32_EQ(CFE_SB_ZeroCopyBufferValidate(&BadZeroCpyBuf.Content, &BufDscPtr), CFE_SB_BUFFER_INVALID);

    /* Good buffer pointer + Good Handle => SUCCESS */
    CFE_UtAssert_SUCCESS(CFE_SB_ZeroCopyBufferValidate(SendPtr, &BufDscPtr));

    /* Confirm that the computed pointer was correct */
    UtAssert_ADDRESS_EQ(&BufDscPtr->Content, SendPtr);

    /* Clean-up */
    CFE_SB_ReleaseMessageBuffer(SendPtr);
}

/*
** Test successfully sending a message in zero copy mode (telemetry source
** sequence count increments)
*/
void Test_TransmitBuffer_IncrementSeqCnt(void)
{
    CFE_SB_Buffer_t *       SendPtr    = NULL;
    CFE_SB_Buffer_t *       ReceivePtr = NULL;
    CFE_SB_PipeId_t         PipeId     = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t          MsgId      = SB_UT_TLM_MID;
    uint32                  PipeDepth  = 10;
    CFE_MSG_SequenceCount_t SeqCnt;
    CFE_MSG_Size_t          Size = sizeof(SB_UT_Test_Tlm_t);
    CFE_MSG_Type_t          Type = CFE_MSG_Type_Tlm;

    /* Set up hook for checking CFE_MSG_SetSequenceCount calls */
    UT_SetHookFunction(UT_KEY(CFE_MSG_SetSequenceCount), UT_CheckSetSequenceCount, &SeqCnt);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "ZeroCpyTestPipe"));

    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    /* Create a real/valid Zero Copy handle via the API */
    SendPtr = CFE_SB_AllocateMessageBuffer(sizeof(SB_UT_Test_Tlm_t));
    if (SendPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }

    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), 1);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* Test a successful zero copy send */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitBuffer(SendPtr, true));

    CFE_UtAssert_SUCCESS(CFE_SB_ReceiveBuffer(&ReceivePtr, PipeId, CFE_SB_PEND_FOREVER));

    UtAssert_ADDRESS_EQ(SendPtr, ReceivePtr);

    UtAssert_INT32_EQ(SeqCnt, 1);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Test again with message validation failure */
    SendPtr = CFE_SB_AllocateMessageBuffer(sizeof(SB_UT_Test_Tlm_t));
    if (SendPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }

    MsgId = CFE_SB_INVALID_MSG_ID;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(SendPtr, true), CFE_SB_BAD_ARGUMENT);
    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_INV_MSGID_EID);
    CFE_UtAssert_SUCCESS(CFE_SB_ReleaseMessageBuffer(SendPtr));

    /* Test an unsuccessful zero copy send */
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(NULL, true), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test successfully sending a message in zero copy mode (telemetry source
** sequence count is unchanged)
*/
void Test_TransmitBuffer_NoIncrement(void)
{
    CFE_SB_Buffer_t *       SendPtr    = NULL;
    CFE_SB_Buffer_t *       ReceivePtr = NULL;
    CFE_SB_PipeId_t         PipeId     = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t          MsgId      = SB_UT_TLM_MID;
    uint32                  PipeDepth  = 10;
    CFE_MSG_SequenceCount_t SeqCnt     = 22;
    CFE_MSG_Size_t          Size       = sizeof(SB_UT_Test_Tlm_t);
    CFE_MSG_Type_t          Type       = CFE_MSG_Type_Tlm;

    /* Set up hook for checking CFE_MSG_SetSequenceCount calls */
    UT_SetHookFunction(UT_KEY(CFE_MSG_SetSequenceCount), UT_CheckSetSequenceCount, &SeqCnt);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "ZeroCpyPassTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    SendPtr = CFE_SB_AllocateMessageBuffer(sizeof(SB_UT_Test_Tlm_t));

    if (SendPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* Test a successful zero copy pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitBuffer(SendPtr, false));
    CFE_UtAssert_SUCCESS(CFE_SB_ReceiveBuffer(&ReceivePtr, PipeId, CFE_SB_PEND_FOREVER));

    UtAssert_ADDRESS_EQ(SendPtr, ReceivePtr);
    UtAssert_INT32_EQ(SeqCnt, 22);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test releasing a pointer to a buffer for zero copy mode
*/
void Test_ReleaseMessageBuffer(void)
{
    CFE_SB_Buffer_t *ZeroCpyMsgPtr1 = NULL;
    CFE_SB_Buffer_t *ZeroCpyMsgPtr2 = NULL;
    CFE_SB_Buffer_t *ZeroCpyMsgPtr3 = NULL;
    CFE_SB_BufferD_t BadBufferDesc;
    size_t           MsgSize = 10;

    ZeroCpyMsgPtr1 = CFE_SB_AllocateMessageBuffer(MsgSize);
    ZeroCpyMsgPtr2 = CFE_SB_AllocateMessageBuffer(MsgSize);
    ZeroCpyMsgPtr3 = CFE_SB_AllocateMessageBuffer(MsgSize);
    CFE_UtAssert_SETUP(CFE_SB_ReleaseMessageBuffer(ZeroCpyMsgPtr2));

    /* Test response to an invalid buffer (has been released already) */
    UtAssert_INT32_EQ(CFE_SB_ReleaseMessageBuffer(ZeroCpyMsgPtr2), CFE_SB_BUFFER_INVALID);

    /* Test response to a null message pointer */
    UtAssert_INT32_EQ(CFE_SB_ReleaseMessageBuffer(NULL), CFE_SB_BAD_ARGUMENT);

    /* Test response to an invalid message pointer */
    memset(&BadBufferDesc, 0, sizeof(BadBufferDesc));
    UtAssert_INT32_EQ(CFE_SB_ReleaseMessageBuffer(&BadBufferDesc.Content), CFE_SB_BUFFER_INVALID);

    /* Test successful release of the second buffer */
    CFE_UtAssert_SUCCESS(CFE_SB_ReleaseMessageBuffer(ZeroCpyMsgPtr3));

    /* Test successful release of the third buffer */
    CFE_UtAssert_SUCCESS(CFE_SB_ReleaseMessageBuffer(ZeroCpyMsgPtr1));

    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test send message response with the destination disabled
*/
void Test_TransmitMsg_DisabledDestination(void)
{
    CFE_SB_PipeId_t        PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t         MsgId  = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t       TlmPkt;
    CFE_SB_DestinationD_t *DestPtr;
    int32                  PipeDepth;
    CFE_MSG_Type_t         Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t         Size = sizeof(TlmPkt);
    CFE_SBR_RouteId_t      RouteId;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    PipeDepth = 2;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    RouteId         = CFE_SBR_GetRouteId(MsgId);
    DestPtr         = CFE_SB_GetDestPtr(RouteId, PipeId);
    DestPtr->Active = CFE_SB_INACTIVE;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

void UT_CFE_MSG_Verify_CustomHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *IsAcceptable = UT_Hook_GetArgValueByName(Context, "IsAcceptable", bool *);

    /* Return alternating false/true (this is needed to avoid an endless loop) */
    *IsAcceptable = (UT_GetStubCount(FuncKey) & 1) == 0;
}

/*
** Function for calling SB receive message API test functions
*/
void Test_ReceiveBuffer_API(void)
{
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_InvalidPipeId);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_InvalidTimeout);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_Poll);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_Timeout);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_PipeReadError);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_PendForever);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_InvalidBufferPtr);
}

static void SB_UT_PipeIdModifyHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_BufferD_t **     OutData     = UT_Hook_GetArgValueByName(Context, "data", void *);
    size_t *                size_copied = UT_Hook_GetArgValueByName(Context, "size_copied", size_t *);
    int32                   status;
    static CFE_SB_BufferD_t LocalBuf;
    CFE_SB_PipeD_t *        PipeDscPtr = UserObj;

    memset(&LocalBuf, 0, sizeof(LocalBuf));
    CFE_SB_TrackingListReset(&LocalBuf.Link);
    LocalBuf.UseCount = 1;
    *OutData          = &LocalBuf;
    *size_copied      = sizeof(*OutData);
    status            = OS_SUCCESS;
    UT_Stub_SetReturnValue(FuncKey, status);

    /* Modify the PipeID so it fails to match */
    PipeDscPtr->PipeId = CFE_SB_INVALID_PIPE;
}

/* Special handler to hit OS_QueueGet error casses */
static void SB_UT_QueueGetHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    size_t **data        = (size_t **)UT_Hook_GetArgValueByName(Context, "data", void *);
    size_t * size_copied = UT_Hook_GetArgValueByName(Context, "size_copied", size_t *);
    int32    status      = OS_SUCCESS;

    *data = UserObj;

    if (*data == NULL)
    {
        *size_copied = sizeof(CFE_SB_BufferD_t *);
    }
    else
    {
        *size_copied = *((size_t *)UserObj);
    }
    UT_Stub_SetReturnValue(FuncKey, status);
}

/*
** Test receiving a message from the software bus with an invalid pipe ID
*/
void Test_ReceiveBuffer_InvalidPipeId(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  InvalidPipeId = SB_UT_ALTERNATE_INVALID_PIPEID;
    CFE_SB_PipeId_t  PipeId        = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeD_t * PipeDscPtr;

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, InvalidPipeId, CFE_SB_POLL), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_SB_BAD_PIPEID_EID);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter, 1);
    UT_ClearEventHistory();

    /*
     * The internal call to OS_QueueGet is done with the SB global unlocked, as other tasks must be able
     * to access the SB global while another task is pending on a receive.  Because of this lock/unlock/relock
     * pattern, the CFE_SB_ReceiveBuffer() function validates the pipeID twice, after each time it obtains the
     * global SB lock.  The second check is to cover the corner case that the pipeID was deleted or reconfigured
     * while it was pending.
     *
     * This case is harder to test for, need to set up a handler function that changes the pipe in between these
     * two checks, such that the first check passes, but the second (identical) check fails.
     */

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 4, "RcvTestPipe"));
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), SB_UT_PipeIdModifyHandler, PipeDscPtr);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_POLL), CFE_SB_PIPE_RD_ERR);
    CFE_UtAssert_EVENTSENT(CFE_SB_BAD_PIPEID_EID);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter, 1);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter, 1);
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), NULL, NULL);

    /* restore the PipeID so it can be deleted */
    PipeDscPtr->PipeId = PipeId;

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test receiving a message response to invalid timeout value (< -1)
*/
void Test_ReceiveBuffer_InvalidTimeout(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId    = CFE_SB_INVALID_PIPE;
    uint32           PipeDepth = 10;
    int32            TimeOut   = -5;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, TimeOut), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_RCV_BAD_ARG_EID);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter, 1);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter, 0);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test receiving a message response when there is no message on the queue
*/
void Test_ReceiveBuffer_Poll(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId    = CFE_SB_INVALID_PIPE;
    uint32           PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_POLL), CFE_SB_NO_MESSAGE);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter, 0);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter, 0);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test receiving a message response to a timeout
*/
void Test_ReceiveBuffer_Timeout(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId    = CFE_SB_INVALID_PIPE;
    uint32           PipeDepth = 10;
    int32            TimeOut   = 200;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, OS_QUEUE_TIMEOUT);

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, TimeOut), CFE_SB_TIME_OUT);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter, 0);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter, 0);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test receiving a message response to a pipe read error
*/
void Test_ReceiveBuffer_PipeReadError(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId    = CFE_SB_INVALID_PIPE;
    uint32           PipeDepth = 10;
    size_t           Data      = 1;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_PIPE_RD_ERR);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_RD_ERR_EID);

    /* Set handler to exercise error conditions for OS_QueueGet */
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), SB_UT_QueueGetHandler, &Data);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_PIPE_RD_ERR);
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), SB_UT_QueueGetHandler, NULL);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_PIPE_RD_ERR);
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), NULL, NULL);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter, 0);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter, 3);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test receiving a message response to a "pend forever" timeout
*/
void Test_ReceiveBuffer_PendForever(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_MsgId_t   MsgId  = SB_UT_TLM_MID;
    CFE_SB_PipeId_t  PipeId = CFE_SB_INVALID_PIPE;
    SB_UT_Test_Tlm_t TlmPkt;
    uint32           PipeDepth = 10;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_SB_PipeD_t * PipeDscPtr;

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_SUCCESS(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER));

    UtAssert_NOT_NULL(SBBufPtr);
    UtAssert_ADDRESS_EQ(&PipeDscPtr->LastBuffer->Content, SBBufPtr);

    CFE_UtAssert_EVENTCOUNT(2);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Ensure that calling a second time with no message clears the LastBuffer reference */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_NO_MESSAGE);
    UtAssert_NULL(PipeDscPtr->LastBuffer);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgReceiveErrorCounter, 0);
    UtAssert_UINT8_EQ(CFE_SB_Global.HKTlmMsg.Payload.InternalErrorCounter, 0);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test releasing zero copy buffers for all pipes owned by a given app ID
*/
void Test_CleanupApp_API(void)
{
    CFE_SB_PipeId_t PipeId;
    uint16          PipeDepth = 10;
    CFE_ES_AppId_t  AppID;
    CFE_ES_AppId_t  AppID2;

    /*
     * Reset global descriptor list
     */
    CFE_SB_InitBuffers();

    CFE_ES_GetAppID(&AppID);
    AppID2 = CFE_ES_APPID_C(CFE_ResourceId_FromInteger(2));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_SB_AllocateMessageBuffer(10);

    /* Mimic a different app ID getting a buffer */
    UT_SetAppID(AppID2);
    CFE_SB_AllocateMessageBuffer(10);

    /* Original app gets a second buffer */
    UT_SetAppID(AppID);
    CFE_SB_AllocateMessageBuffer(10);

    /* Set second application ID to provide complete branch path coverage */
    CFE_SB_Global.PipeTbl[1].PipeId = SB_UT_PIPEID_1;
    CFE_SB_Global.PipeTbl[1].AppId  = AppID;

    /* Attempt with a bad application ID first in order to get full branch path
     * coverage in CFE_SB_ZeroCopyReleaseAppId
     */
    CFE_SB_CleanUpApp(CFE_ES_APPID_UNDEFINED);

    /* This should have freed no buffers  */
    UtAssert_STUB_COUNT(CFE_ES_PutPoolBuf, 0);

    /* Attempt again with a valid application ID */
    CFE_SB_CleanUpApp(AppID);

    /* This should have freed 2 out of the 3 buffers -
     * the ones which were gotten by this app. */
    UtAssert_STUB_COUNT(CFE_ES_PutPoolBuf, 2);

    /* Clean up the second App */
    CFE_SB_CleanUpApp(AppID2);

    /* This should have freed the last buffer */
    UtAssert_STUB_COUNT(CFE_ES_PutPoolBuf, 3);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_DELETED_EID);
}

/*
** Test receiving a message response to invalid buffer pointer (null)
*/
void Test_ReceiveBuffer_InvalidBufferPtr(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    uint32          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(NULL, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_RCV_BAD_ARG_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test SB Utility APIs
*/
void Test_SB_Utils(void)
{
    SB_UT_ADD_SUBTEST(Test_CFE_SB_MsgHdrSize);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_GetUserData);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_SetGetUserDataLength);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_ZeroCopyReleaseAppId);

    SB_UT_ADD_SUBTEST(Test_CFE_SB_ValidateMsgId);

    /* TopicID <-> MsgID conversion utilities implemented in msg_id_util.c */
    SB_UT_ADD_SUBTEST(Test_CFE_SB_CmdTopicIdToMsgId);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_TlmTopicIdToMsgId);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_GlobalCmdTopicIdToMsgId);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_GlobalTlmTopicIdToMsgId);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_LocalCmdTopicIdToMsgId);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_LocalTlmTopicIdToMsgId);
}

static void UT_SB_Setup_MsgHdrSize(bool HasSec, CFE_MSG_Type_t MsgType)
{
    UT_ResetState(UT_KEY(CFE_MSG_GetHasSecondaryHeader));
    UT_ResetState(UT_KEY(CFE_MSG_GetType));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &HasSec, sizeof(HasSec), true);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &MsgType, sizeof(MsgType), true);
}

static void UT_SB_Check_MsgHdrSize(void *BasePtr, bool HasSec, CFE_MSG_Type_t MsgType, size_t ExpectedPayloadOffset)
{
    UT_SB_Setup_MsgHdrSize(HasSec, MsgType);

    UtAssert_EQ(size_t, CFE_SB_MsgHdrSize(BasePtr), ExpectedPayloadOffset);
}

/*
** Test getting the size of a command/telemetry message header
*/
void Test_CFE_SB_MsgHdrSize(void)
{
    union
    {
        CFE_MSG_Message_t         BaseObject;
        CFE_MSG_CommandHeader_t   CommandHeader;
        CFE_MSG_TelemetryHeader_t TelemetryHeader;
    } msg;

    memset(&msg, 0, sizeof(msg));

    /* No secondary */
    UT_SB_Check_MsgHdrSize(&msg, false, CFE_MSG_Type_Invalid, sizeof(msg.BaseObject));

    /* Has secondary, tlm type */
    UT_SB_Check_MsgHdrSize(&msg, true, CFE_MSG_Type_Tlm, sizeof(msg.TelemetryHeader));

    /* Has secondary, cmd type */
    UT_SB_Check_MsgHdrSize(&msg, true, CFE_MSG_Type_Cmd, sizeof(msg.CommandHeader));

    /* Has secondary, invalid type */
    UT_SB_Check_MsgHdrSize(&msg, true, CFE_MSG_Type_Invalid, 0);

    /* Bad message argument (NULL) */
    UT_SB_Check_MsgHdrSize(NULL, true, CFE_MSG_Type_Invalid, 0);
}

static void UT_SB_Check_GetUserData(void *BasePtr, bool HasSec, CFE_MSG_Type_t MsgType, size_t ExpectedPayloadOffset)
{
    const uint8_t *ExpectedPayloadPtr = (const uint8_t *)BasePtr + ExpectedPayloadOffset;

    UT_SB_Setup_MsgHdrSize(HasSec, MsgType);

    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(BasePtr), ExpectedPayloadPtr);
}

/*
** Test getting a pointer to the user data portion of a message
*/
void Test_CFE_SB_GetUserData(void)
{
    CFE_MSG_Message_t msg;
    struct cmd_uint8
    {
        CFE_MSG_CommandHeader_t CommandHeader;
        uint8                   payload;
    } cmd_uint8;
    struct cmd_uint16
    {
        CFE_MSG_CommandHeader_t CommandHeader;
        uint16                  payload;
    } cmd_uint16;
    struct cmd_uint32
    {
        CFE_MSG_CommandHeader_t CommandHeader;
        uint32                  payload;
    } cmd_uint32;
    struct cmd_uint64
    {
        CFE_MSG_CommandHeader_t CommandHeader;
        uint64                  payload;
    } cmd_uint64;
    struct tlm_uint8
    {
        CFE_MSG_TelemetryHeader_t TelemetryHeader;
        uint8                     payload;
    } tlm_uint8;
    struct tlm_uint16
    {
        CFE_MSG_TelemetryHeader_t TelemetryHeader;
        uint16                    payload;
    } tlm_uint16;
    struct tlm_uint32
    {
        CFE_MSG_TelemetryHeader_t TelemetryHeader;
        uint32                    payload;
    } tlm_uint32;
    struct tlm_uint64
    {
        CFE_MSG_TelemetryHeader_t TelemetryHeader;
        uint64                    payload;
    } tlm_uint64;

    memset(&msg, 0, sizeof(msg));
    memset(&cmd_uint8, 0, sizeof(cmd_uint8));
    memset(&cmd_uint16, 0, sizeof(cmd_uint16));
    memset(&cmd_uint32, 0, sizeof(cmd_uint32));
    memset(&cmd_uint64, 0, sizeof(cmd_uint64));
    memset(&tlm_uint8, 0, sizeof(tlm_uint8));
    memset(&tlm_uint16, 0, sizeof(tlm_uint16));
    memset(&tlm_uint32, 0, sizeof(tlm_uint32));
    memset(&tlm_uint64, 0, sizeof(tlm_uint64));

    /* No secondary */
    UT_SB_Check_GetUserData(&msg, false, CFE_MSG_Type_Invalid, sizeof(CFE_MSG_Message_t));

    /* Commands */
    UT_SB_Check_GetUserData(&cmd_uint8, true, CFE_MSG_Type_Cmd, offsetof(struct cmd_uint8, payload));
    UT_SB_Check_GetUserData(&cmd_uint16, true, CFE_MSG_Type_Cmd, offsetof(struct cmd_uint16, payload));
    UT_SB_Check_GetUserData(&cmd_uint32, true, CFE_MSG_Type_Cmd, offsetof(struct cmd_uint32, payload));
    UT_SB_Check_GetUserData(&cmd_uint64, true, CFE_MSG_Type_Cmd, offsetof(struct cmd_uint64, payload));

    /* Telemetry */
    UT_SB_Check_GetUserData(&tlm_uint8, true, CFE_MSG_Type_Tlm, offsetof(struct tlm_uint8, payload));
    UT_SB_Check_GetUserData(&tlm_uint16, true, CFE_MSG_Type_Tlm, offsetof(struct tlm_uint16, payload));
    UT_SB_Check_GetUserData(&tlm_uint32, true, CFE_MSG_Type_Tlm, offsetof(struct tlm_uint32, payload));
    UT_SB_Check_GetUserData(&tlm_uint64, true, CFE_MSG_Type_Tlm, offsetof(struct tlm_uint64, payload));

    /* Bad inputs */
    UtAssert_NULL(CFE_SB_GetUserData(NULL));
}

/*
** Test setting and getting the user data length of a message
*/
void Test_CFE_SB_SetGetUserDataLength(void)
{
    CFE_MSG_Message_t msg;
    CFE_MSG_Size_t    size   = 10;
    CFE_MSG_Type_t    type   = CFE_MSG_Type_Invalid;
    bool              hassec = false;

    memset(&msg, 0, sizeof(msg));

    /* Pass through functions */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UtAssert_INT32_EQ(CFE_SB_GetUserDataLength(&msg), size - sizeof(CFE_MSG_Message_t));

    /*
     * Note, this function currently has a type mismatch - it attempts to
     * return CFE_SB_BAD_ARGUMENT if passed a NULL MsgPtr value, but that
     * will be implicitly converted to size_t, and the result is
     * platform-defined.
     *
     * The function is called for coverage, but the result is not verified
     * due to this mismatch.
     */
    UtAssert_VOIDCALL(CFE_SB_GetUserDataLength(NULL));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_VOIDCALL(CFE_SB_SetUserDataLength(&msg, 0));
    UtAssert_VOIDCALL(CFE_SB_SetUserDataLength(NULL, 0));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_VOIDCALL(CFE_SB_SetUserDataLength(&msg, CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1));
}

/*
** Test validating a msg id
*/
void Test_CFE_SB_ValidateMsgId(void)
{
    CFE_SB_MsgId_t MsgId;

    /* Validate Msg Id */
    MsgId = SB_UT_LAST_VALID_MID;
    CFE_UtAssert_SUCCESS(CFE_SB_ValidateMsgId(MsgId));

    /* Test for invalid msg id */
    MsgId = SB_UT_ALTERNATE_INVALID_MID;
    UtAssert_INT32_EQ(CFE_SB_ValidateMsgId(MsgId), CFE_SB_FAILED);
}

void Test_CFE_SB_CmdTopicIdToMsgId(void)
{
    CFE_SB_MsgId_Atom_t MsgIdVal1;
    CFE_SB_MsgId_Atom_t MsgIdVal2;
    bool ReturnCode1 = false;
    bool ReturnCode2 = false;

    MsgIdVal1 = CFE_SB_CmdTopicIdToMsgId(1, 1);
    ReturnCode1 = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal1));

    MsgIdVal2 = CFE_SB_CmdTopicIdToMsgId(1, 0);
    ReturnCode2 = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal2));

    if(MsgIdVal1 == 0 || MsgIdVal2 == 0)
    {
        /* Macro does not exist. Topic ID has not been implemented. */
        UtAssert_MIR("Topic ID has not been implemented. ");
    }
    else
    {
        UtAssert_BOOL_TRUE(ReturnCode1);
        UtAssert_BOOL_TRUE(ReturnCode2);
    }
}

void Test_CFE_SB_TlmTopicIdToMsgId(void)
{
    CFE_SB_MsgId_Atom_t MsgIdVal1;
    CFE_SB_MsgId_Atom_t MsgIdVal2;
    bool ReturnCode1 = false;
    bool ReturnCode2 = false;

    MsgIdVal1 = CFE_SB_TlmTopicIdToMsgId(1, 1);
    ReturnCode1 = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal1));

    MsgIdVal2 = CFE_SB_TlmTopicIdToMsgId(1, 0);
    ReturnCode2 = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal2));

    if(MsgIdVal1 == 0 || MsgIdVal2 == 0)
    {
        /* Macro does not exist. Topic ID has not been implemented. */
        UtAssert_MIR("Topic ID has not been implemented. ");
    }
    else
    {
        UtAssert_BOOL_TRUE(ReturnCode1);
        UtAssert_BOOL_TRUE(ReturnCode2);
    }
}

void Test_CFE_SB_GlobalCmdTopicIdToMsgId(void)
{
    CFE_SB_MsgId_Atom_t MsgIdVal;
    bool ReturnCode = false;

    MsgIdVal = CFE_SB_GlobalCmdTopicIdToMsgId(2);
    ReturnCode = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal));
    if(MsgIdVal == 0)
    {
        /* Macro does not exist. Topic ID has not been implemented. */
        UtAssert_MIR("Topic ID has not been implemented. ");
    }
    else
    {
        UtAssert_BOOL_TRUE(ReturnCode);
    }
}

void Test_CFE_SB_GlobalTlmTopicIdToMsgId(void)
{
    CFE_SB_MsgId_Atom_t MsgIdVal;
    bool ReturnCode = false;

    MsgIdVal = CFE_SB_GlobalTlmTopicIdToMsgId(2);
    ReturnCode = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal));
    if(MsgIdVal == 0)
    {
        /* Macro does not exist. Topic ID has not been implemented. */
        UtAssert_MIR("Topic ID has not been implemented. ");
    }
    else
    {
        UtAssert_BOOL_TRUE(ReturnCode);
    }
}

void Test_CFE_SB_LocalCmdTopicIdToMsgId(void)
{
    CFE_SB_MsgId_Atom_t MsgIdVal;
    bool ReturnCode = false;

    MsgIdVal = CFE_SB_LocalCmdTopicIdToMsgId(3);
    ReturnCode = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal));
    if(MsgIdVal == 0)
    {
        /* Macro does not exist. Topic ID has not been implemented. */
        UtAssert_MIR("Topic ID has not been implemented. ");
    }
    else
    {
        UtAssert_BOOL_TRUE(ReturnCode);
    }
}

void Test_CFE_SB_LocalTlmTopicIdToMsgId(void)
{
    CFE_SB_MsgId_Atom_t MsgIdVal;
    bool ReturnCode = false;

    MsgIdVal = CFE_SB_LocalTlmTopicIdToMsgId(3);
    ReturnCode = CFE_SB_IsValidMsgId(CFE_SB_ValueToMsgId(MsgIdVal));
    if(MsgIdVal == 0)
    {
        /* Macro does not exist. Topic ID has not been implemented. */
        UtAssert_MIR("Topic ID has not been implemented. ");
    }
    else
    {
        UtAssert_BOOL_TRUE(ReturnCode);
    }
}

/*
** Test validating a msg id
*/
void Test_CFE_SB_ZeroCopyReleaseAppId(void)
{
    CFE_SB_BufferD_t BufDesc;
    CFE_ES_AppId_t   SelfId;

    CFE_UtAssert_SETUP(CFE_ES_GetAppID(&SelfId));

    memset(&BufDesc, 0, sizeof(BufDesc));
    BufDesc.UseCount = 3;
    BufDesc.AppId    = SelfId;
    CFE_SB_TrackingListReset(&CFE_SB_Global.ZeroCopyList);
    CFE_SB_TrackingListReset(&BufDesc.Link);
    CFE_SB_TrackingListAdd(&CFE_SB_Global.ZeroCopyList, &BufDesc.Link);

    /* confirm that CFE_SB_ZeroCopyReleaseAppId() decrements the buffer use count properly */
    CFE_UtAssert_SUCCESS(CFE_SB_ZeroCopyReleaseAppId(SelfId));
    UtAssert_UINT32_EQ(BufDesc.UseCount, 2);

    /* Invoke CFE_SB_TrackingListIsEnd() with NULL args */
    /* Note this can only happen if init was somehow skipped, but the FSW checks for it */
    memset(&CFE_SB_Global.ZeroCopyList, 0, sizeof(CFE_SB_Global.ZeroCopyList));
    CFE_UtAssert_SUCCESS(CFE_SB_ZeroCopyReleaseAppId(SelfId));
}

/*
** Function for calling SB special test cases functions
*/
void Test_SB_SpecialCases(void)
{
    SB_UT_ADD_SUBTEST(Test_UseCount_Rollover_Prevention);
    SB_UT_ADD_SUBTEST(Test_OS_MutSem_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_ReqToSendEvent_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_PutDestBlk_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_Buffers);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_BadPipeInfo);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_UnsubResubPath);
    SB_UT_ADD_SUBTEST(Test_MessageString);
}

/*
 * Test the use count rollover prevetion
 */
void Test_UseCount_Rollover_Prevention(void)
{
    CFE_SB_BufferD_t bd;
    uint16           usecount_expected = 0x7FFF;

    /* Note this is just for coverage, limit can not be reached in nomal ops (would be a bug) */
    bd.UseCount = usecount_expected;
    UtAssert_VOIDCALL(CFE_SB_IncrBufUseCnt(&bd));
    UtAssert_UINT32_EQ(bd.UseCount, usecount_expected);
}

/*
** Test pipe creation with semaphore take and give failures
*/
void Test_OS_MutSem_ErrLogic(void)
{
    CFE_SB_PipeId_t PipeId    = CFE_SB_INVALID_PIPE;
    CFE_SB_MsgId_t  MsgId     = SB_UT_CMD_MID;
    uint16          PipeDepth = 50;

    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SEM_FAILURE);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 2, OS_SEM_FAILURE);
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    CFE_UtAssert_SUCCESS(CFE_SB_Subscribe(MsgId, PipeId));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test successful recursive event prevention
*/
void Test_ReqToSendEvent_ErrLogic(void)
{
    CFE_ES_TaskId_t TaskId;
    uint32          Bit = 5;

    /* Clear task bits, then call function, which should set the bit for
     * the specified task
     */
    CFE_ES_GetTaskID(&TaskId);
    CFE_SB_Global.StopRecurseFlags[0] = 0x0000;
    UtAssert_INT32_EQ(CFE_SB_RequestToSendEvent(TaskId, Bit), CFE_SB_GRANTED);

    /* Call the function a second time; the result should indicate that the
     * bit is already set
     */
    UtAssert_INT32_EQ(CFE_SB_RequestToSendEvent(TaskId, Bit), CFE_SB_DENIED);

    /* if called from an invalid task ID, should be denied */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_TaskID_ToIndex), -1);
    UtAssert_INT32_EQ(CFE_SB_RequestToSendEvent(TaskId, Bit), CFE_SB_DENIED);
    UtAssert_VOIDCALL(CFE_SB_FinishSendEvent(TaskId, Bit)); /* should be no-op */
    UT_ClearDefaultReturnValue(UT_KEY(CFE_ES_TaskID_ToIndex));

    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test getting a destination descriptor to the SB memory pool using a null
** destination pointer
*/
void Test_PutDestBlk_ErrLogic(void)
{
    UtAssert_INT32_EQ(CFE_SB_PutDestinationBlk(NULL), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test functions that involve a buffer in the SB buffer pool
*/
void Test_CFE_SB_Buffers(void)
{
    int32 ExpRtn;

    CFE_SB_BufferD_t *     bd;
    CFE_SB_DestinationD_t *destptr;

    CFE_SB_Global.StatTlmMsg.Payload.MemInUse     = 0;
    CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse = sizeof(CFE_SB_BufferD_t) * 4;
    bd                                            = CFE_SB_GetBufferFromPool(0);

    UtAssert_INT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakMemInUse, sizeof(CFE_SB_BufferD_t) * 4);

    CFE_UtAssert_EVENTCOUNT(0);

    /*
     * If returning to the pool fails SB still isn't going to use the buffer anymore,
     * so it shouldn't be tracked as "in use" - it is lost.
     */
    ExpRtn = CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse - 1;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    CFE_SB_ReturnBufferToPool(bd);
    UtAssert_INT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.SBBuffersInUse, ExpRtn);

    CFE_UtAssert_EVENTCOUNT(0);

    bd->UseCount = 0;
    CFE_SB_DecrBufUseCnt(bd);
    UtAssert_INT32_EQ(bd->UseCount, 0);
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.MemInUse);

    CFE_UtAssert_EVENTCOUNT(0);

    destptr = CFE_SB_GetDestinationBlk();
    UtAssert_NOT_NULL(destptr);
    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.MemInUse, sizeof(*destptr));

    /*
     * historical behavior has CFE_SB_PutDestinationBlk() return SUCCESS even if the underlying call fails,
     * but the MemInUse should remain the same
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_SB_PutDestinationBlk(destptr));
    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.MemInUse, sizeof(*destptr));

    /* normal case should reduce MemInUse */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, sizeof(*destptr));
    CFE_UtAssert_SUCCESS(CFE_SB_PutDestinationBlk(destptr));
    UtAssert_ZERO(CFE_SB_Global.StatTlmMsg.Payload.MemInUse);

    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test internal function to get the pipe table index for the given pipe ID
*/
void Test_CFE_SB_BadPipeInfo(void)
{
    CFE_SB_PipeId_t PipeId = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeD_t *PipeDscPtr;
    uint16          PipeDepth = 10;
    CFE_ES_AppId_t  AppID;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe1"));

    /* Set the pipe ID to an erroneous value and attempt to delete the pipe */
    PipeDscPtr         = CFE_SB_LocatePipeDescByID(PipeId);
    PipeDscPtr->PipeId = SB_UT_PIPEID_1;
    CFE_ES_GetAppID(&AppID);
    UtAssert_INT32_EQ(CFE_SB_DeletePipeFull(SB_UT_PIPEID_0, AppID), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    /* Reset the pipe ID and delete the pipe */
    PipeDscPtr->PipeId = PipeId;

    UtAssert_INT32_EQ(
        CFE_SB_SubscribeFull(SB_UT_FIRST_VALID_MID, PipeId, CFE_SB_DEFAULT_QOS, CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT, 2),
        CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test receiving a message response to an unsubscribing to message, then
** resubscribing to it while it's in the pipe
*/
void Test_ReceiveBuffer_UnsubResubPath(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_MsgId_t   MsgId  = SB_UT_TLM_MID;
    CFE_SB_PipeId_t  PipeId = CFE_SB_INVALID_PIPE;
    SB_UT_Test_Tlm_t TlmPkt;
    uint32           PipeDepth = 10;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);

    memset(&TlmPkt, 0, sizeof(TlmPkt));

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmPkt.TelemetryHeader), true));

    CFE_UtAssert_SETUP(CFE_SB_Unsubscribe(MsgId, PipeId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    CFE_UtAssert_SUCCESS(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER));

    UtAssert_NOT_NULL(SBBufPtr);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_REMOVED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
}

/*
** Test the paths through the MessageStringSet and MessageStringGet functions
*/
void Test_MessageString(void)
{
    char       SrcString[12];
    char       DestString[12];
    const char DefString[] = "default";

    strcpy(SrcString, "abcdefg");
    memset(DestString, 'q', sizeof(DestString));

    /* Test setting string with invalid inputs */
    UtAssert_INT32_EQ(CFE_SB_MessageStringSet(NULL, SrcString, sizeof(DestString), sizeof(SrcString)),
                      CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_MessageStringSet(DestString, NULL, sizeof(DestString), sizeof(SrcString)),
                      CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(NULL, SrcString, DefString, sizeof(DestString), sizeof(SrcString)),
                      CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(DestString, SrcString, DefString, 0, sizeof(SrcString)),
                      CFE_SB_BAD_ARGUMENT);

    /* Test setting string where the destination size > source string size */
    CFE_SB_MessageStringSet(DestString, SrcString, sizeof(DestString), sizeof(SrcString));
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), SrcString, sizeof(SrcString));

    /* Test setting string where the source string is empty */
    memset(SrcString, 0, sizeof(SrcString));
    memset(DestString, 'q', sizeof(DestString));
    CFE_SB_MessageStringSet(DestString, SrcString, sizeof(DestString), sizeof(SrcString));
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), SrcString, sizeof(SrcString));
    SrcString[0] = 'a';
    CFE_SB_MessageStringSet(DestString, SrcString, sizeof(DestString), 1);
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), SrcString, sizeof(SrcString));

    /* Test setting string where the destination size < source string size */
    strcpy(SrcString, "abcdefg");
    memset(DestString, 'q', sizeof(DestString));
    CFE_SB_MessageStringSet(DestString, SrcString, 4, sizeof(SrcString));
    UtAssert_STRINGBUF_EQ(DestString, 4, SrcString, 4);

    /* Test getting string where the destination size > source string size */
    strcpy(SrcString, "abcdefg");
    memset(DestString, 'q', sizeof(DestString));
    CFE_SB_MessageStringGet(DestString, SrcString, DefString, sizeof(DestString), 5);
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), SrcString, 5);

    /* Test getting string where the destination size is zero */
    memset(DestString, 'q', sizeof(DestString));
    memset(SrcString, 'k', sizeof(SrcString));
    CFE_SB_MessageStringGet(DestString, SrcString, DefString, 0, sizeof(SrcString));
    memset(SrcString, 'q', sizeof(SrcString));
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), SrcString, sizeof(SrcString));

    /* Test getting string where the default string is NULL */
    memset(DestString, 'q', sizeof(DestString));
    CFE_SB_MessageStringGet(DestString, SrcString, NULL, sizeof(DestString), 0);
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), "", 0);

    /* Test getting string where the source string size is zero */
    CFE_SB_MessageStringGet(DestString, SrcString, DefString, sizeof(DestString), 0);
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), DefString, sizeof(DefString));
    CFE_SB_MessageStringGet(DestString, "", DefString, sizeof(DestString), 1);
    UtAssert_STRINGBUF_EQ(DestString, sizeof(DestString), DefString, sizeof(DefString));

    /* Test getting string where the destination size < source string size */
    strcpy(SrcString, "abcdefg");
    memset(DestString, 'q', sizeof(DestString));
    CFE_SB_MessageStringGet(DestString, SrcString, DefString, 4, sizeof(SrcString));
    UtAssert_STRINGBUF_EQ(DestString, 4, SrcString, 3);
}
