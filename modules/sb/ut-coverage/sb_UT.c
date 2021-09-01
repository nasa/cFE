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
const CFE_SB_MsgId_t SB_UT_FIRST_VALID_MID = CFE_SB_MSGID_WRAP_VALUE(0);

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
    CFE_ES_AppId_t TempValue = InitialID;

    /* Underneath the wrapper(s) the IDs are 32-bit integer values, so it can be cast */
    *((uint32 *)&TempValue) += Modifier;

    return TempValue;
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

} /* end main */

/*
** Reset variable values and sockets prior to a test
*/
void SB_ResetUnitTest(void)
{
    UT_InitData();
    CFE_SB_EarlyInit();
} /* end SB_ResetUnitTest */

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
} /* end Test_SB_AppInit */

/*
** Test task init with EVS_Register returning error
*/
void Test_SB_AppInit_EVSRegFail(void)
{
    int32 ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, ForcedRtnVal);
    UtAssert_INT32_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    CFE_UtAssert_EVENTCOUNT(0);

} /* end Test_SB_AppInit_EVSRegFail */

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

} /* end Test_SB_AppInit_EVSSendEvtFail */

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

} /* end Test_SB_AppInit_CrPipeFail */

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

} /* end Test_SB_AppInit_Sub1Fail */

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

} /* end Test_SB_AppInit_Sub2Fail */

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

} /* end Test_SB_AppInit_Sub3Fail */

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

} /* end Test_SB_AppInit_GetPoolFail */

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

} /* end Test_SB_AppInit_PutPoolFail */

/*
** Function for calling SB main task test functions
*/
void Test_SB_MainRoutine(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_Main_RcvErr);
    SB_UT_ADD_SUBTEST(Test_SB_Main_InitErr);
    SB_UT_ADD_SUBTEST(Test_SB_Main_Nominal);
} /* end Test_SB_MainRoutine */

/*
** Test main task with a packet receive error
*/
void Test_SB_Main_RcvErr(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, -1);
    CFE_SB_TaskMain();

    CFE_UtAssert_EVENTCOUNT(6);

    CFE_UtAssert_EVENTSENT(CFE_SB_INIT_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_RD_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));

} /* end Test_SB_Main_RcvErr */

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
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_NOOP_CC;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size    = sizeof(CFE_SB_NoopCmd_t);

    memset(&Buffer, 0, sizeof(Buffer));
    Buffer.Desc.UseCount      = 1;
    Buffer.Desc.AllocatedSize = sizeof(Buffer);
    Buffer.Desc.ContentSize   = Size;
    CFE_SB_TrackingListReset(&Buffer.Desc.Link);
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), SB_UT_PipeGetHandler, &Buffer);
    /* This still needs to error-out to avoid an infinite loop */
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 2, OS_QUEUE_EMPTY);

    /* For the first pass it will call CFE_SB_ProcessCmdPacket which needs to decode it */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_TaskMain();

    CFE_UtAssert_EVENTCOUNT(6);

    CFE_UtAssert_EVENTSENT(CFE_SB_INIT_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_CMD0_RCVD_EID);

    /* remove the handler so the pipe can be deleted */
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), NULL, NULL);
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));

} /* end Test_SB_Main_RcvErr */

/*
** Test main task with an application initialization error
*/
void Test_SB_Main_InitErr(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    CFE_SB_TaskMain();

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));

} /* end Test_SB_Main_InitErr */

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
} /* end Test_SB_Cmds */

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
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_NOOP_CC;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size    = sizeof(Noop.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&Noop.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_CMD0_RCVD_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&Noop.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

} /* end Test_SB_Cmds_Noop */

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
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_RESET_COUNTERS_CC;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size    = sizeof(ResetCounters.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&ResetCounters.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_CMD1_RCVD_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&ResetCounters.SBBuf);
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
    CFE_MSG_FcnCode_t FcnCode;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    Size;
    CFE_SB_PipeId_t   PipeId1;
    CFE_SB_PipeId_t   PipeId2;
    CFE_SB_PipeId_t   PipeId3;

    /* Make the pipe table non-empty so the stats command writes something */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, 4, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, 4, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, 4, "TestPipe3"));

    /* For internal TransmitMsg call */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_STATS_TLM_MID);
    Size  = sizeof(CFE_SB_Global.StatTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For Generic command processing */
    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    Size    = sizeof(SendSbStats.Cmd);
    FcnCode = CFE_SB_SEND_SB_STATS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_ProcessCmdPipePkt(&SendSbStats.SBBuf);

    /* No subs event and command processing event */
    CFE_UtAssert_EVENTCOUNT(5);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_STATS_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&SendSbStats.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));

} /* end Test_SB_Cmds_Stats */

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
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_WRITE_ROUTING_INFO_CC;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size    = sizeof(WriteRoutingInfo.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    WriteRoutingInfo.Cmd.Payload.Filename[0] = '\0';

    /* Make some routing info by calling CFE_SB_AppInit */
    CFE_UtAssert_SETUP(CFE_SB_AppInit());

    CFE_SB_ProcessCmdPipePkt(&WriteRoutingInfo.SBBuf);

    CFE_UtAssert_EVENTCOUNT(5);

    CFE_UtAssert_EVENTSENT(CFE_SB_INIT_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Also test with a bad file name - should generate CFE_SB_SND_RTG_ERR1_EID */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    CFE_SB_ProcessCmdPipePkt(&WriteRoutingInfo.SBBuf);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&WriteRoutingInfo.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(CFE_SB_Global.CmdPipe));

} /* end Test_SB_Cmds_RoutingInfoDef */

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
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_WRITE_ROUTING_INFO_CC;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size    = sizeof(WriteRoutingInfo.Cmd);

    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy(WriteRoutingInfo.Cmd.Payload.Filename, "RoutingTstFile", sizeof(WriteRoutingInfo.Cmd.Payload.Filename) - 1);
    WriteRoutingInfo.Cmd.Payload.Filename[sizeof(WriteRoutingInfo.Cmd.Payload.Filename) - 1] = '\0';

    CFE_SB_ProcessCmdPipePkt(&WriteRoutingInfo.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

} /* end Test_SB_Cmds_RoutingInfoSpec */

/*
**  Test send routing information command data getter
*/
void Test_SB_Cmds_RoutingInfoDataGetter(void)
{
    CFE_SB_PipeId_t                  PipeId1;
    CFE_SB_PipeId_t                  PipeId2;
    CFE_SB_PipeId_t                  PipeId3;
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

    UtAssert_BOOL_FALSE(CFE_SB_WriteRouteInfoDataGetter(&State, 0, &LocalBuffer, &LocalBufSize));
    UtAssert_NOT_NULL(LocalBuffer);
    UtAssert_NONZERO(LocalBufSize);

    UtAssert_BOOL_TRUE(
        CFE_SB_WriteRouteInfoDataGetter(&State, CFE_PLATFORM_SB_MAX_MSG_IDS, &LocalBuffer, &LocalBufSize));
    UtAssert_ZERO(LocalBufSize);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));
} /* end Test_SB_Cmds_RoutingInfoDataGetter */

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
    CFE_SB_PipeId_t   PipeId1;
    CFE_SB_PipeId_t   PipeId2;
    CFE_SB_PipeId_t   PipeId3;
    uint16            PipeDepth = 10;
    CFE_MSG_FcnCode_t FcnCode   = CFE_SB_WRITE_PIPE_INFO_CC;
    CFE_SB_MsgId_t    MsgId     = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size      = sizeof(WritePipeInfo.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    WritePipeInfo.Cmd.Payload.Filename[0] = '\0';

    /* Create some pipe info */
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));

    CFE_SB_ProcessCmdPipePkt(&WritePipeInfo.SBBuf);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    /* Also test with a bad file name - should generate CFE_SB_SND_RTG_ERR1_EID */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    CFE_SB_ProcessCmdPipePkt(&WritePipeInfo.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&WritePipeInfo.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));

} /* end Test_SB_Cmds_PipeInfoDef */

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
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_WRITE_PIPE_INFO_CC;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size    = sizeof(WritePipeInfo.Cmd);

    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy(WritePipeInfo.Cmd.Payload.Filename, "PipeTstFile", sizeof(WritePipeInfo.Cmd.Payload.Filename) - 1);
    WritePipeInfo.Cmd.Payload.Filename[sizeof(WritePipeInfo.Cmd.Payload.Filename) - 1] = '\0';

    CFE_SB_ProcessCmdPipePkt(&WritePipeInfo.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

} /* end Test_SB_Cmds_PipeInfoAlreadyPending */

/*
** Test write pipe information data getter
*/
void Test_SB_Cmds_PipeInfoDataGetter(void)
{
    CFE_SB_PipeId_t                  PipeId1;
    CFE_SB_PipeId_t                  PipeId2;
    CFE_SB_PipeId_t                  PipeId3;
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
} /* end Test_SB_Cmds_PipeInfoDataGetter */

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
    CFE_SB_PipeId_t                  PipeId1;
    CFE_SB_PipeId_t                  PipeId2;
    CFE_SB_PipeId_t                  PipeId3;
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
} /* end Test_SB_MapInfoDataGetter */

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
    CFE_SB_PipeId_t   PipeId1;
    CFE_SB_PipeId_t   PipeId2;
    CFE_SB_PipeId_t   PipeId3;
    CFE_SB_MsgId_t    MsgId0    = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t    MsgId1    = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t    MsgId2    = SB_UT_TLM_MID3;
    CFE_SB_MsgId_t    MsgId3    = SB_UT_TLM_MID4;
    CFE_SB_MsgId_t    MsgId4    = SB_UT_TLM_MID5;
    CFE_SB_MsgId_t    MsgId5    = SB_UT_TLM_MID6;
    uint16            PipeDepth = 10;
    CFE_MSG_FcnCode_t FcnCode   = CFE_SB_WRITE_MAP_INFO_CC;
    CFE_SB_MsgId_t    MsgId     = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size      = sizeof(WriteMapInfo.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    WriteMapInfo.Cmd.Payload.Filename[0] = '\0';

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

    CFE_SB_ProcessCmdPipePkt(&WriteMapInfo.SBBuf);

    CFE_UtAssert_EVENTCOUNT(10);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Also test with a bad file name - should generate CFE_SB_SND_RTG_ERR1_EID */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    CFE_SB_ProcessCmdPipePkt(&WriteMapInfo.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&WriteMapInfo.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId3));

} /* end Test_SB_Cmds_MapInfoDef */

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
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_WRITE_MAP_INFO_CC;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size    = sizeof(WriteMapInfo.Cmd);

    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy(WriteMapInfo.Cmd.Payload.Filename, "MapTstFile", sizeof(WriteMapInfo.Cmd.Payload.Filename) - 1);
    WriteMapInfo.Cmd.Payload.Filename[sizeof(WriteMapInfo.Cmd.Payload.Filename) - 1] = '\0';

    CFE_SB_ProcessCmdPipePkt(&WriteMapInfo.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SND_RTG_ERR1_EID);

} /* end Test_SB_Cmds_MapInfoSpec */

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
    CFE_SB_PipeId_t   PipeId;
    CFE_SB_MsgId_t    MsgId     = SB_UT_TLM_MID;
    uint16            PipeDepth = 5;
    CFE_MSG_FcnCode_t FcnCode   = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd  = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size      = sizeof(EnableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "EnRouteTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    EnableRoute.Cmd.Payload.MsgId = MsgId;
    EnableRoute.Cmd.Payload.Pipe  = PipeId;

    CFE_SB_ProcessCmdPipePkt(&EnableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE2_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&EnableRoute.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Cmds_EnRouteValParam */

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
    CFE_SB_PipeId_t   PipeId1;
    CFE_SB_PipeId_t   PipeId2;
    CFE_SB_MsgId_t    MsgId     = SB_UT_TLM_MID;
    uint16            PipeDepth = 5;
    CFE_MSG_FcnCode_t FcnCode   = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd  = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size      = sizeof(EnableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "EnRouteTestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "EnRouteTestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId1));
    EnableRoute.Cmd.Payload.MsgId = MsgId;
    EnableRoute.Cmd.Payload.Pipe  = PipeId2;

    CFE_SB_ProcessCmdPipePkt(&EnableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE1_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_SB_Cmds_EnRouteNonExist */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size     = sizeof(EnableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnableRoute.Cmd.Payload.MsgId = SB_UT_LAST_VALID_MID;
    EnableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    CFE_SB_ProcessCmdPipePkt(&EnableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE3_EID);

} /* end Test_SB_Cmds_EnRouteInvParam */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size     = sizeof(EnableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnableRoute.Cmd.Payload.MsgId = CFE_SB_INVALID_MSG_ID;
    EnableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    CFE_SB_ProcessCmdPipePkt(&EnableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE3_EID);

} /* end Test_SB_Cmds_EnRouteInvParam2 */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size     = sizeof(EnableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnableRoute.Cmd.Payload.MsgId = SB_UT_ALTERNATE_INVALID_MID;
    EnableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_0;

    CFE_SB_ProcessCmdPipePkt(&EnableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_ENBL_RTE3_EID);

} /* end Test_SB_Cmds_EnRouteInvParam3 */

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
    CFE_SB_PipeId_t   PipeId;
    CFE_SB_MsgId_t    MsgId     = SB_UT_TLM_MID;
    uint16            PipeDepth = 5;
    CFE_MSG_FcnCode_t FcnCode   = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd  = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size      = sizeof(DisableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "DisRouteTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    DisableRoute.Cmd.Payload.MsgId = MsgId;
    DisableRoute.Cmd.Payload.Pipe  = PipeId;

    CFE_SB_ProcessCmdPipePkt(&DisableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE2_EID);

    --Size;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&DisableRoute.SBBuf);
    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Cmds_DisRouteValParam */

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
    CFE_SB_PipeId_t   PipeId1, PipeId2;
    CFE_SB_MsgId_t    MsgId     = SB_UT_TLM_MID;
    uint16            PipeDepth = 5;
    CFE_MSG_FcnCode_t FcnCode   = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd  = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size      = sizeof(DisableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "DisRouteTestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "DisRouteTestPipe2"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId1));
    DisableRoute.Cmd.Payload.MsgId = MsgId;
    DisableRoute.Cmd.Payload.Pipe  = PipeId2;

    CFE_SB_ProcessCmdPipePkt(&DisableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE1_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_SB_Cmds_DisRouteNonExist */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size     = sizeof(DisableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    DisableRoute.Cmd.Payload.MsgId = SB_UT_LAST_VALID_MID;
    DisableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    CFE_SB_ProcessCmdPipePkt(&DisableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE3_EID);

} /* end Test_SB_Cmds_DisRouteInvParam */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size     = sizeof(DisableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    DisableRoute.Cmd.Payload.MsgId = CFE_SB_INVALID_MSG_ID;
    DisableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_3;

    CFE_SB_ProcessCmdPipePkt(&DisableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE3_EID);

} /* end Test_SB_Cmds_DisRouteInvParam2 */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size     = sizeof(DisableRoute.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    DisableRoute.Cmd.Payload.MsgId = SB_UT_ALTERNATE_INVALID_MID;
    DisableRoute.Cmd.Payload.Pipe  = SB_UT_PIPEID_0;

    CFE_SB_ProcessCmdPipePkt(&DisableRoute.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DSBL_RTE3_EID);

} /* end Test_SB_Cmds_DisRouteInvParam3 */

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

    /* For internal TransmitMsg call */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID);
    Size     = sizeof(CFE_SB_Global.HKTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For HK command processing */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);

    CFE_SB_ProcessCmdPipePkt(&Housekeeping.SBBuf);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_NO_SUBS_EID);

} /* end Test_SB_Cmds_SendHK */

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
    CFE_SB_PipeId_t   PipeId1;
    CFE_SB_PipeId_t   PipeId2;
    CFE_SB_MsgId_t    MsgId     = SB_UT_BARE_TLM_MID3;
    uint16            MsgLim    = 4;
    uint16            PipeDepth = 50;
    int32             i;
    int32             NumEvts;
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_SEND_PREV_SUBS_CC;
    CFE_SB_MsgId_t    MsgIdCmd;
    CFE_MSG_Size_t    Size;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    NumEvts = 2; /* one for each pipe create */

    /* Two full pkts to be sent plus five entries in a partial pkt, skipping MSGID 0x0D */
    for (i = 0; i < CFE_SB_SUB_ENTRIES_PER_PKT * 2 + 5; i++)
    {
        /* Skip subscribing to ALLSUBS mid. This is the one we are testing.
         * MsgID for this in CCSDS v.1 was 0x180d so this MID did not appear in the
         * SB sub list. This results in multiple NO_SUBS_EID sent which we are not
         * testing here so it is irrelevant
         * For CCSDS v.2 CFE_SB_ALLSUBS_TLM_MID (0x0d) now appears in the
         * SB subscription list and thus we must skip adding 0x0D to the list
         * as we were going from MSGID 0-45 (0x00-0x2D)
         * */
        if (i != CFE_SB_ALLSUBS_TLM_MID)
        {
            NumEvts += 1;
            CFE_UtAssert_SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId1));
        }
    }

    CFE_UtAssert_SETUP(CFE_SB_SubscribeLocal(MsgId, PipeId2, MsgLim));

    /* For 3 internal TransmitMsg calls */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size     = sizeof(CFE_SB_Global.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Set buffers for gets */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    Size     = sizeof(SendPrevSubs.Cmd);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

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
    for (; i < CFE_SB_SUB_ENTRIES_PER_PKT * 3; i++)
    {
        CFE_UtAssert_SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId1));
        NumEvts += 1;
    }

    CFE_UtAssert_SETUP(CFE_SB_SubscribeLocal(MsgId, PipeId2, MsgLim));

    /* For 3 internal TransmitMsg calls */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size     = sizeof(CFE_SB_Global.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Set buffers for gets */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    Size     = sizeof(SendPrevSubs.Cmd);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_ProcessCmdPipePkt(&SendPrevSubs.SBBuf);

    NumEvts += 8; /* +2 for the subscribe, +6 for the SEND_PREV_SUBS_CC */

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
    Size = sizeof(SendPrevSubs.Cmd) - 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&SendPrevSubs.SBBuf);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_SB_Cmds_SendPrevSubs */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_ENABLE_SUB_REPORTING_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    CFE_MSG_Size_t    Size     = sizeof(EnableSubReporting.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_ProcessCmdPipePkt(&EnableSubReporting.SBBuf);

    CFE_UtAssert_EVENTCOUNT(0);

    /* Test error paths in the CFE_SB_ENABLE_SUB_REPORTING_CC handling */
    UT_ClearEventHistory();
    Size = sizeof(EnableSubReporting.Cmd) - 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&EnableSubReporting.SBBuf);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

} /* end Test_SB_Cmds_SubRptOn */

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
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_DISABLE_SUB_REPORTING_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    CFE_MSG_Size_t    Size     = sizeof(DisableSubReporting.Cmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_ProcessCmdPipePkt(&DisableSubReporting.SBBuf);

    CFE_UtAssert_EVENTCOUNT(0);

    /* Test error paths in the CFE_SB_DISABLE_SUB_REPORTING_CC handling */
    UT_ClearEventHistory();
    Size = sizeof(DisableSubReporting.Cmd) - 1;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt(&DisableSubReporting.SBBuf);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

} /* end Test_SB_Cmds_SubRptOff */

/*
** Test command handler response to an invalid command code
*/
void Test_SB_Cmds_CmdUnexpCmdCode(void)
{
    CFE_MSG_FcnCode_t FcnCode  = 99;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Use a command code known to be invalid */
    CFE_SB_ProcessCmdPipePkt((CFE_SB_Buffer_t *)NULL);
    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_SB_BAD_CMD_CODE_EID);

    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB_ProcessCmdPipePkt((CFE_SB_Buffer_t *)NULL);

} /* end Test_SB_Cmds_UnexpCmdCode */

/*
** Test command handler response to an incorrect length
*/
void Test_SB_Cmds_BadCmdLength(void)
{
    /*
     * Just choosing "EnableRoute" command here as it has a non-empty payload
     */
    CFE_MSG_FcnCode_t FcnCode  = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size     = sizeof(CFE_SB_EnableRouteCmd_t) - 1;

    /* Second GetMsgId and GetFcnCode are for reporting */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_ProcessCmdPipePkt((CFE_SB_Buffer_t *)NULL);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_LEN_ERR_EID);

} /* end Test_SB_Cmds_BadCmdLength */

/*
** Test command handler response to an invalid message ID
*/
void Test_SB_Cmds_UnexpMsgId(void)
{
    CFE_SB_MsgId_t MsgIdCmd = SB_UT_TLM_MID;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);

    CFE_SB_ProcessCmdPipePkt((CFE_SB_Buffer_t *)NULL);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_BAD_MSGID_EID);

} /* end Test_SB_Cmds_UnexpMsgId */

/*
** Function for calling early initialization tests
*/
void Test_SB_EarlyInit(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_EarlyInit_SemCreateError);
    SB_UT_ADD_SUBTEST(Test_SB_EarlyInit_PoolCreateError);
    SB_UT_ADD_SUBTEST(Test_SB_EarlyInit_NoErrors);
} /* end Test_SB_EarlyInit */

/*
** Test early initialization response to a semaphore create failure
*/
void Test_SB_EarlyInit_SemCreateError(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERR_NO_FREE_IDS);
    UtAssert_INT32_EQ(CFE_SB_EarlyInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
} /* end Test_SB_EarlyInit_SemCreateError */

/*
** Test early initialization response to a pool create ex failure
*/
void Test_SB_EarlyInit_PoolCreateError(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PoolCreateEx), 1, CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_EarlyInit(), CFE_ES_BAD_ARGUMENT);
} /* end Test_SB_EarlyInit_PoolCreateError */

/*
** Test successful early initialization
*/
void Test_SB_EarlyInit_NoErrors(void)
{
    CFE_SB_EarlyInit();
    CFE_UtAssert_SUCCESS(CFE_SB_EarlyInit());
} /* end Test_SB_EarlyInit_NoErrors */

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
} /* end Test_CreatePipe_API */

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

} /* end Test_CreatePipe_NullPtr */

/*
** Test create pipe response to valid pipe depths
*/
void Test_CreatePipe_ValPipeDepth(void)
{
    CFE_SB_PipeId_t PipeIdReturned[2];

    CFE_UtAssert_SUCCESS(CFE_SB_CreatePipe(&PipeIdReturned[0], 1, "TestPipeMin"));
    CFE_UtAssert_SUCCESS(CFE_SB_CreatePipe(&PipeIdReturned[1], OS_QUEUE_MAX_DEPTH, "TestPipeMax"));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[0]));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[1]));

} /* end Test_CreatePipe_ValPipeDepth */

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

} /* end Test_CreatePipe_InvalPipeDepth */

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

} /* end Test_CreatePipe_MaxPipes */

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
** Function for calling SB delete pipe API test functions
*/
void Test_DeletePipe_API(void)
{
    SB_UT_ADD_SUBTEST(Test_DeletePipe_NoSubs);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_WithSubs);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_InvalidPipeId);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_InvalidPipeOwner);
    SB_UT_ADD_SUBTEST(Test_DeletePipe_WithAppid);
} /* end Test_DeletePipe_API */

/*
** Test delete pipe response to deleting a pipe with no subscriptions
*/
void Test_DeletePipe_NoSubs(void)
{
    CFE_SB_PipeId_t PipedId;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SUCCESS(CFE_SB_DeletePipe(PipedId));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_DELETED_EID);

} /* end Test_DeletePipe_NoSubs */

/*
** Test delete pipe response to deleting a pipe with subscriptions
*/
void Test_DeletePipe_WithSubs(void)
{
    CFE_SB_PipeId_t PipedId;
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

} /* end Test_DeletePipe_WithSubs */

/*
** Test delete pipe response to an invalid pipe ID
*/
void Test_DeletePipe_InvalidPipeId(void)
{
    CFE_SB_PipeId_t PipeId = SB_UT_ALTERNATE_INVALID_PIPEID;

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_DEL_PIPE_ERR1_EID);

} /* end Test_DeletePipe_InvalidPipeId */

/*
** Test delete pipe response to an invalid pipe owner
*/
void Test_DeletePipe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipedId;
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

} /* end Test_DeletePipe_InvalidPipeId */

/*
** Test successful pipe delete with app ID
*/
void Test_DeletePipe_WithAppid(void)
{
    CFE_SB_PipeId_t PipedId;
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

} /* end Test_DeletePipe_WithAppid */

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
} /* end Test_PipeOpts_API */

/*
** Function for calling SB get pipe name by id API test functions
*/
void Test_GetPipeName_API(void)
{
    SB_UT_ADD_SUBTEST(Test_GetPipeName_NullPtr);
    SB_UT_ADD_SUBTEST(Test_GetPipeName_InvalidId);
    SB_UT_ADD_SUBTEST(Test_GetPipeName);
} /* end Test_GetGetPipeName_API */

/*
** Call GetPipeName with null pointer.
*/
void Test_GetPipeName_NullPtr(void)
{
    CFE_SB_PipeId_t PipeId;
    char            NameBuf[10];

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe"));
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(NULL, OS_MAX_API_NAME, PipeId), CFE_SB_BAD_ARGUMENT);
    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_NULL_PTR_EID);
    UT_ClearEventHistory();

    UtAssert_INT32_EQ(CFE_SB_GetPipeName(NameBuf, 0, PipeId), CFE_SB_BAD_ARGUMENT);
    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_NULL_PTR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_GetPipeName_NullPtr */

/*
** Call to GetPipeName with an invalid pipe id.
*/
void Test_GetPipeName_InvalidId(void)
{
    char            PipeName[OS_MAX_API_NAME];
    CFE_SB_PipeId_t PipeId;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe"));

    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_SB_GetPipeName(PipeName, sizeof(PipeName), PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPENAME_ID_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_GetPipeName_InvalidId */

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

} /* end Test_GetPipeName */

/*
** Function for calling SB get pipe id by name API test functions
*/
void Test_GetPipeIdByName_API(void)
{
    SB_UT_ADD_SUBTEST(Test_GetPipeIdByName_NullPtrs);
    SB_UT_ADD_SUBTEST(Test_GetPipeIdByName_InvalidName);
    SB_UT_ADD_SUBTEST(Test_GetPipeIdByName);
} /* end Test_GetPipeIdByName_API */

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

} /* end Test_GetPipeIdByName_NullPtrs */

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

} /* end Test_GetPipeIdByName_InvalidName */

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

} /* end Test_GetPipeIdByName */

/*
** Try setting pipe options on an invalid pipe ID
*/
void Test_SetPipeOpts_BadID(void)
{
    UtAssert_INT32_EQ(CFE_SB_SetPipeOpts(SB_UT_ALTERNATE_INVALID_PIPEID, 0), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_SETPIPEOPTS_ID_ERR_EID);

} /* end Test_SetPipeOpts_BadID */

/*
** Try setting pipe options when not pipe owner
*/
void Test_SetPipeOpts_NotOwner(void)
{
    CFE_SB_PipeId_t PipeID;
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

} /* end Test_SetPipeOpts_NotOwner */

/*
** Set pipe options
*/
void Test_SetPipeOpts(void)
{
    CFE_SB_PipeId_t PipeID;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    CFE_UtAssert_SUCCESS(CFE_SB_SetPipeOpts(PipeID, 0));

    CFE_UtAssert_EVENTSENT(CFE_SB_SETPIPEOPTS_EID);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_SB_SetPipeOpts(PipeID, 0), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeID));

} /* end Test_SetPipeOpts */

/*
** Try getting pipe options on an invalid pipe ID
*/
void Test_GetPipeOpts_BadID(void)
{
    uint8 Opts = 0;

    UtAssert_INT32_EQ(CFE_SB_GetPipeOpts(SB_UT_ALTERNATE_INVALID_PIPEID, &Opts), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEOPTS_ID_ERR_EID);

} /* end Test_GetPipeOpts_BadID */

/*
** Try getting pipe options with a bad pointer
*/
void Test_GetPipeOpts_BadPtr(void)
{
    CFE_SB_PipeId_t PipeID;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    UtAssert_INT32_EQ(CFE_SB_GetPipeOpts(PipeID, NULL), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEOPTS_PTR_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeID));

} /* end Test_GetPipeOpts_BadPtr */

/*
** Successful call to GetPipeOpts
*/
void Test_GetPipeOpts(void)
{
    CFE_SB_PipeId_t PipeID;
    uint8           Opts = 0;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    CFE_UtAssert_SUCCESS(CFE_SB_GetPipeOpts(PipeID, &Opts));

    CFE_UtAssert_EVENTSENT(CFE_SB_GETPIPEOPTS_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeID));

} /* end Test_GetPipeOpts */

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
} /* end Test_Subscribe_API */

/*
** Test API to globally subscribe to a message
*/
void Test_Subscribe_SubscribeEx(void)
{
    CFE_SB_PipeId_t PipeId;
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

} /* end Test_Subscribe_SubscribeEx */

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

} /* end Test_Subscribe_InvalidPipeId */

/*
** Test message subscription response to an invalid message ID
*/
void Test_Subscribe_InvalidMsgId(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId     = SB_UT_ALTERNATE_INVALID_MID;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    UtAssert_INT32_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUB_ARG_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_InvalidMsgId */

/*
** Test message subscription response to the maximum message limit
*/
void Test_Subscribe_MaxMsgLim(void)
{
    CFE_SB_PipeId_t PipeId;
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

} /* end Test_Subscribe_MaxMsgLim */

/*
** Test message subscription response to a duplicate subscription
*/
void Test_Subscribe_DuplicateSubscription(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId     = SB_UT_CMD_MID;
    uint16          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    CFE_UtAssert_SUCCESS(CFE_SB_Subscribe(MsgId, PipeId));
    CFE_UtAssert_SUCCESS(CFE_SB_Subscribe(MsgId, PipeId));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_DUP_SUBSCRIP_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_DuplicateSubscription */

/*
** Test API to locally subscribe to a message
*/
void Test_Subscribe_LocalSubscription(void)
{
    CFE_SB_PipeId_t PipeId;
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

} /* end Test_Subscribe_LocalSubscription */

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

} /* end Test_Subscribe_MaxDestCount */

/*
** Test message subscription response to reaching the maximum message ID count
*/
void Test_Subscribe_MaxMsgIdCount(void)
{
    CFE_SB_PipeId_t PipeId0;
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    uint16          PipeDepth = 50;
    int32           i;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));

    for (i = 0; i < CFE_PLATFORM_SB_MAX_MSG_IDS + 1; i++)
    {
        if (i < CFE_PLATFORM_SB_MAX_MSG_IDS)
        {
            CFE_UtAssert_SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId2));
        }
        else
        {
            UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId2), CFE_SB_MAX_MSGS_MET);
        }
    }

    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse, CFE_PLATFORM_SB_MAX_MSG_IDS);
    CFE_UtAssert_EVENTSENT(CFE_SB_MAX_MSGS_MET_EID);

    CFE_UtAssert_SUCCESS(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(0), PipeId1));
    UtAssert_UINT32_EQ(CFE_SB_Global.StatTlmMsg.Payload.PeakMsgIdsInUse, CFE_PLATFORM_SB_MAX_MSG_IDS);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId0));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_Subscribe_MaxMsgIdCount */

/*
** Test obtaining the list of current message subscriptions
*/
void Test_Subscribe_SendPrevSubs(void)
{
    CFE_SB_PipeId_t          PipeId0;
    CFE_SB_PipeId_t          PipeId1;
    CFE_SB_PipeId_t          PipeId2;
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

} /* end Test_Subscribe_SendPrevSubs */

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

} /* end Test_Subscribe_PipeNonexistent */

/*
** Test enabling and disabling subscription reporting
*/
void Test_Subscribe_SubscriptionReporting(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    CFE_SB_Qos_t    Quality;
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

} /* end Test_Subscribe_SubscriptionReporting */

/*
** Test message subscription response to an invalid pipe owner
*/
void Test_Subscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId;
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

} /* end Test_Subscribe_InvalidPipeOwner */

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
} /* end Test_Unsubscribe_API */

/*
** Test API used to unsubscribe to a message (successful)
*/
void Test_Unsubscribe_Basic(void)
{
    CFE_SB_PipeId_t TestPipe;
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

} /* end Test_Unsubscribe_Basic */

/*
** Test API used to unsubscribe to a message with a specific AppId
*/
void Test_Unsubscribe_AppId(void)
{
    CFE_SB_PipeId_t TestPipe;
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

} /* end Test_Unsubscribe_AppId */

/*
** Test CFE internal API used to locally unsubscribe to a message (successful)
*/
void Test_Unsubscribe_Local(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    CFE_UtAssert_SUCCESS(CFE_SB_UnsubscribeLocal(SB_UT_LAST_VALID_MID, TestPipe));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));

} /* end Test_Unsubscribe_Local */

/*
** Test message unsubscription response to an invalid message ID
*/
void Test_Unsubscribe_InvalParam(void)
{
    CFE_SB_PipeId_t TestPipe;
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

} /* end Test_Unsubscribe_InvalParam */

/*
** Test message unsubscription response to a message ID that is not subscribed
*/
void Test_Unsubscribe_NoMatch(void)
{
    CFE_SB_PipeId_t TestPipe;
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

} /* end Test_Unsubscribe_NoMatch */

/*
** Test message unsubscription response to an invalid pipe ID
*/
void Test_Unsubscribe_InvalidPipe(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId     = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(MsgId, SB_UT_ALTERNATE_INVALID_PIPEID), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_UNSUB_INV_PIPE_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(TestPipe));

} /* end Test_Unsubscribe_InvalidPipe */

/*
** Test message unsubscription response to an invalid pipe owner
*/
void Test_Unsubscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
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

} /* end Test_Unsubscribe_InvalidPipeOwner */

/*
** Test message unsubscription response to the first pipe destination when
** the message is subscribed to by multiple pipes
*/
void Test_Unsubscribe_FirstDestWithMany(void)
{
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    CFE_SB_PipeId_t TestPipe1;
    CFE_SB_PipeId_t TestPipe2;
    CFE_SB_PipeId_t TestPipe3;
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

} /* end Test_Unsubscribe_FirstDestWithMany */

/*
** Test message unsubscription response to a middle pipe destination when
** the message is subscribed to by multiple pipes
*/
void Test_Unsubscribe_MiddleDestWithMany(void)
{
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    CFE_SB_PipeId_t TestPipe1;
    CFE_SB_PipeId_t TestPipe2;
    CFE_SB_PipeId_t TestPipe3;
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

} /* end Test_Unsubscribe_MiddleDestWithMany */

/*
** Test message unsubscription by verifying the message destination pointer no
** longer points to the pipe
*/
void Test_Unsubscribe_GetDestPtr(void)
{
    CFE_SB_MsgId_t    MsgId = SB_UT_CMD_MID;
    CFE_SB_PipeId_t   TestPipe1;
    CFE_SB_PipeId_t   TestPipe2;
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

} /* end Test_Unsubscribe_GetDestPtr */

/*
** Function for calling SB send message API test functions
*/
void Test_TransmitMsg_API(void)
{
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_NullPtr);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_NoSubscribers);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_MaxMsgSizePlusOne);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_BasicSend);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_SequenceCount);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_QueuePutError);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_PipeFull);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_MsgLimitExceeded);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_GetPoolBufErr);
    SB_UT_ADD_SUBTEST(Test_TransmitBuffer_IncrementSeqCnt);
    SB_UT_ADD_SUBTEST(Test_TransmitBuffer_NoIncrement);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_ZeroCopyBufferValidate);
    SB_UT_ADD_SUBTEST(Test_TransmitMsg_DisabledDestination);
    SB_UT_ADD_SUBTEST(Test_BroadcastBufferToRoute);
    SB_UT_ADD_SUBTEST(Test_TransmitMsgValidate_MaxMsgSizePlusOne);
    SB_UT_ADD_SUBTEST(Test_TransmitMsgValidate_NoSubscribers);
    SB_UT_ADD_SUBTEST(Test_TransmitMsgValidate_InvalidMsgId);
    SB_UT_ADD_SUBTEST(Test_AllocateMessageBuffer);
    SB_UT_ADD_SUBTEST(Test_ReleaseMessageBuffer);
} /* end Test_TransmitMsg_API */

/*
** Test response to sending a null message on the software bus
*/
void Test_TransmitMsg_NullPtr(void)
{
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(NULL, true), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_BAD_ARG_EID);

} /* end Test_TransmitMsg_NullPtr */

/*
** Test response to sending a message which has no subscribers
*/
void Test_TransmitMsg_NoSubscribers(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_NO_SUBS_EID);

} /* end Test_TransmitMsg_NoSubscribers */

/*
** Test response to sending a message with the message size larger than allowed
*/
void Test_TransmitMsg_MaxMsgSizePlusOne(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_MSG_Size_t   Size = CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter = 0;

    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true), CFE_SB_MSG_TOO_BIG);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_MSG_TOO_BIG_EID);
    UtAssert_INT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter, 1);

} /* end Test_TransmitMsg_MaxMsgSizePlusOne */

/*
** Test successfully sending a message on the software bus
*/
void Test_TransmitMsg_BasicSend(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_TransmitMsg_BasicSend */

/* Set sequence count hook */
static int32 UT_CheckSetSequenceCount(void *UserObj, int32 StubRetcode, uint32 CallCount,
                                      const UT_StubContext_t *Context)
{
    CFE_MSG_SequenceCount_t *SeqCntPtr = UserObj;

    *SeqCntPtr = UT_Hook_GetArgValueByName(Context, "SeqCnt", CFE_MSG_SequenceCount_t);

    return StubRetcode;
}

/*
** Test successful send/receive for packet sequence count
*/
void Test_TransmitMsg_SequenceCount(void)
{
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_MsgId_t          MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t        TlmPkt;
    CFE_MSG_Size_t          Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t          Type      = CFE_MSG_Type_Tlm;
    uint32                  PipeDepth = 10;
    CFE_MSG_SequenceCount_t SeqCnt;
    CFE_MSG_SequenceCount_t SeqCntExpected;

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

    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 1);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 1);
    UtAssert_INT32_EQ(SeqCnt, SeqCntExpected);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, false));

    /* Assert sequence count wasn't set */
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 1);
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 1);

    SeqCntExpected = 2;
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), SeqCntExpected);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));
    UtAssert_INT32_EQ(SeqCnt, SeqCntExpected);
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 2);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 2);

    CFE_UtAssert_EVENTCOUNT(2);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_SETUP(CFE_SB_Unsubscribe(MsgId, PipeId)); /* should have no subscribers now */

    SeqCntExpected = 3;
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), SeqCntExpected);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true)); /* increment to 3 */
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 3);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 3);

    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId)); /* resubscribe so we can receive a msg */

    SeqCntExpected = 4;
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), SeqCntExpected);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true)); /* increment to 4 */
    UtAssert_INT32_EQ(SeqCnt, SeqCntExpected);
    UtAssert_STUB_COUNT(CFE_MSG_SetSequenceCount, 4);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 4);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_TransmitMsg_SequenceCount */

/*
** Test send message response to a socket queue 'put' error
*/
void Test_TransmitMsg_QueuePutError(void)
{
    CFE_SB_PipeId_t  PipeId4Error;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId4Error, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId4Error));
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_WR_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId4Error));

} /* end Test_TransmitMsg_QueuePutError */

/*
** Test send message response when the socket queue is full
*/
void Test_TransmitMsg_PipeFull(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 1;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "PipeFullTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* This send should pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    /* Tell the QueuePut stub to return OS_QUEUE_FULL on its next call */
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_QUEUE_FULL);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* Pipe overflow causes TransmitMsg to return CFE_SUCCESS */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_FULL_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_TransmitMsg_PipeFull */

/*
** Test send message response to too many messages sent to the pipe
*/
void Test_TransmitMsg_MsgLimitExceeded(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 5;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "MsgLimTestPipe"));

    /* Set maximum allowed messages on the pipe at one time to 1 */
    CFE_SB_SubscribeEx(MsgId, PipeId, CFE_SB_DEFAULT_QOS, 1);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* First send should pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* This send should produce a MsgId to Pipe Limit Exceeded message, but
     * return success
     */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_MSGID_LIM_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_TransmitMsg_MsgLimitExceeded */

/*
** Test send message response to a buffer descriptor allocation failure
*/
void Test_TransmitMsg_GetPoolBufErr(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    PipeDepth = 1;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "GetPoolErrPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Have GetPoolBuf stub return error on its next call (buf descriptor
     * allocation failed)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true), CFE_SB_BUF_ALOC_ERR);

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_EVENTSENT(CFE_SB_GET_BUF_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_TransmitMsg_GetPoolBufErr */

/*
** Test getting a pointer to a buffer for zero copy mode with buffer
** allocation failures
*/
void Test_AllocateMessageBuffer(void)
{
    uint16 MsgSize = 10;
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

} /* end Test_AllocateMessageBuffer */

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

    /* Null Buffer => BAD_ARGUMENT */
    UtAssert_INT32_EQ(CFE_SB_ZeroCopyBufferValidate(NULL, &BufDscPtr), CFE_SB_BAD_ARGUMENT);

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
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_MsgId_t          MsgId     = SB_UT_TLM_MID;
    uint32                  PipeDepth = 10;
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
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_MsgId_t          MsgId     = SB_UT_TLM_MID;
    uint32                  PipeDepth = 10;
    CFE_MSG_SequenceCount_t SeqCnt    = 22;
    CFE_MSG_Size_t          Size      = sizeof(SB_UT_Test_Tlm_t);
    CFE_MSG_Type_t          Type      = CFE_MSG_Type_Tlm;

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
    uint16           MsgSize = 10;

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

} /* end Test_TransmitMsg_ZeroCopyReleasePtr */

/*
** Test send message response with the destination disabled
*/
void Test_TransmitMsg_DisabledDestination(void)
{
    CFE_SB_PipeId_t        PipeId;
    CFE_SB_MsgId_t         MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t       TlmPkt;
    CFE_SB_DestinationD_t *DestPtr;
    int32                  PipeDepth;
    CFE_MSG_Type_t         Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t         Size = sizeof(TlmPkt);
    CFE_SBR_RouteId_t      RouteId;

    PipeDepth = 2;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    RouteId         = CFE_SBR_GetRouteId(MsgId);
    DestPtr         = CFE_SB_GetDestPtr(RouteId, PipeId);
    DestPtr->Active = CFE_SB_INACTIVE;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_TransmitMsg_DisabledDestination */

/*
** Test successful CFE_SB_BroadcastBufferToRoute
*/
void Test_BroadcastBufferToRoute(void)
{
    CFE_SB_PipeId_t   PipeId;
    CFE_SB_MsgId_t    MsgId = SB_UT_TLM_MID;
    CFE_SB_BufferD_t  SBBufD;
    int32             PipeDepth;
    CFE_SBR_RouteId_t RouteId;

    memset(&SBBufD, 0, sizeof(SBBufD));
    SBBufD.MsgId = MsgId;
    CFE_SB_TrackingListReset(&SBBufD.Link);

    PipeDepth = 2;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    RouteId = CFE_SBR_GetRouteId(MsgId);

    /* No return from this function - it handles all errors */
    CFE_SB_BroadcastBufferToRoute(&SBBufD, RouteId);

    CFE_UtAssert_EVENTCOUNT(2);
    UT_ClearEventHistory();

    /* Calling this with invalid route ID is essentially a no-op, called for coverage */
    CFE_SB_BroadcastBufferToRoute(&SBBufD, CFE_SBR_INVALID_ROUTE_ID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_BroadcastBufferToRoute */

/*
** Test response to sending a message with the message size larger than allowed
*/
void Test_TransmitMsgValidate_MaxMsgSizePlusOne(void)
{
    CFE_SB_MsgId_t    MsgId = SB_UT_TLM_MID;
    CFE_SB_MsgId_t    MsgIdRtn;
    SB_UT_Test_Tlm_t  TlmPkt;
    CFE_MSG_Size_t    Size = CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1;
    CFE_MSG_Size_t    SizeRtn;
    CFE_SBR_RouteId_t RouteIdRtn;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    UtAssert_INT32_EQ(CFE_SB_TransmitMsgValidate(&TlmPkt.Hdr.Msg, &MsgIdRtn, &SizeRtn, &RouteIdRtn),
                      CFE_SB_MSG_TOO_BIG);
    CFE_UtAssert_MSGID_EQ(MsgIdRtn, MsgId);
    UtAssert_INT32_EQ(SizeRtn, Size);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_MSG_TOO_BIG_EID);
}

/*
** Test response to sending a message which has no subscribers
*/
void Test_TransmitMsgValidate_NoSubscribers(void)
{
    CFE_SB_MsgId_t    MsgId = SB_UT_TLM_MID;
    CFE_SB_MsgId_t    MsgIdRtn;
    SB_UT_Test_Tlm_t  TlmPkt;
    CFE_MSG_Size_t    Size = sizeof(TlmPkt);
    CFE_MSG_Size_t    SizeRtn;
    CFE_SBR_RouteId_t RouteIdRtn;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsgValidate(&TlmPkt.Hdr.Msg, &MsgIdRtn, &SizeRtn, &RouteIdRtn));
    CFE_UtAssert_MSGID_EQ(MsgIdRtn, MsgId);
    UtAssert_INT32_EQ(SizeRtn, Size);
    UtAssert_BOOL_FALSE(CFE_SBR_IsValidRouteId(RouteIdRtn));

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_NO_SUBS_EID);
}

/*
** Test response to sending a message which has an invalid Msg ID
*/
void Test_TransmitMsgValidate_InvalidMsgId(void)
{
    CFE_SB_MsgId_t    MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_SB_MsgId_t    MsgIdRtn;
    SB_UT_Test_Tlm_t  TlmPkt;
    CFE_MSG_Size_t    SizeRtn;
    CFE_SBR_RouteId_t RouteIdRtn;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    UtAssert_INT32_EQ(CFE_SB_TransmitMsgValidate(&TlmPkt.Hdr.Msg, &MsgIdRtn, &SizeRtn, &RouteIdRtn),
                      CFE_SB_BAD_ARGUMENT);
    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_SB_SEND_INV_MSGID_EID);
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
} /* end Test_ReceiveBuffer_API */

static void SB_UT_PipeIdModifyHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void *                  data        = UT_Hook_GetArgValueByName(Context, "data", void *);
    size_t *                size_copied = UT_Hook_GetArgValueByName(Context, "size_copied", size_t *);
    int32                   status;
    static SB_UT_Test_Tlm_t FakeTlmPkt;
    SB_UT_Test_Tlm_t **     OutData;
    CFE_SB_PipeD_t *        PipeDscPtr = UserObj;

    OutData      = data;
    *OutData     = &FakeTlmPkt;
    *size_copied = sizeof(*OutData);
    status       = OS_SUCCESS;
    UT_Stub_SetReturnValue(FuncKey, status);

    /* Modify the PipeID so it fails to match */
    PipeDscPtr->PipeId = CFE_SB_INVALID_PIPE;
}

/*
** Test receiving a message from the software bus with an invalid pipe ID
*/
void Test_ReceiveBuffer_InvalidPipeId(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  InvalidPipeId = SB_UT_ALTERNATE_INVALID_PIPEID;
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_PipeD_t * PipeDscPtr;

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, InvalidPipeId, CFE_SB_POLL), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_SB_BAD_PIPEID_EID);
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
    UT_SetHandlerFunction(UT_KEY(OS_QueueGet), NULL, NULL);

    /* restore the PipeID so it can be deleted */
    PipeDscPtr->PipeId = PipeId;

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_InvalidPipeId */

/*
** Test receiving a message response to invalid timeout value (< -1)
*/
void Test_ReceiveBuffer_InvalidTimeout(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId;
    uint32           PipeDepth = 10;
    int32            TimeOut   = -5;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, TimeOut), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_RCV_BAD_ARG_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_InvalidTimeout */

/*
** Test receiving a message response when there is no message on the queue
*/
void Test_ReceiveBuffer_Poll(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId;
    uint32           PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_POLL), CFE_SB_NO_MESSAGE);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_Poll */

/*
** Test receiving a message response to a timeout
*/
void Test_ReceiveBuffer_Timeout(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId;
    uint32           PipeDepth = 10;
    int32            TimeOut   = 200;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, OS_QUEUE_TIMEOUT);

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, TimeOut), CFE_SB_TIME_OUT);

    CFE_UtAssert_EVENTCOUNT(1);

    CFE_UtAssert_EVENTSENT(CFE_SB_PIPE_ADDED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_Timeout */

/*
** Test receiving a message response to a pipe read error
*/
void Test_ReceiveBuffer_PipeReadError(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_PipeId_t  PipeId;
    uint32           PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_PIPE_RD_ERR);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_Q_RD_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_PipeReadError */

/*
** Test receiving a message response to a "pend forever" timeout
*/
void Test_ReceiveBuffer_PendForever(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    uint32           PipeDepth = 10;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);
    CFE_SB_PipeD_t * PipeDscPtr;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    PipeDscPtr = CFE_SB_LocatePipeDescByID(PipeId);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_SUCCESS(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER));

    UtAssert_NOT_NULL(SBBufPtr);
    UtAssert_ADDRESS_EQ(&PipeDscPtr->LastBuffer->Content, SBBufPtr);

    CFE_UtAssert_EVENTCOUNT(2);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Ensure that calling a second time with no message clears the LastBuffer reference */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_NO_MESSAGE);
    UtAssert_NULL(PipeDscPtr->LastBuffer);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_PendForever */

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

} /* end Test_CleanupApp_API */

/*
** Test receiving a message response to invalid buffer pointer (null)
*/
void Test_ReceiveBuffer_InvalidBufferPtr(void)
{
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(NULL, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTSENT(CFE_SB_RCV_BAD_ARG_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_InvalidBufferPtr */

/*
** Test SB Utility APIs
*/
void Test_SB_Utils(void)
{
    SB_UT_ADD_SUBTEST(Test_CFE_SB_MsgHdrSize);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_GetUserData);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_SetGetUserDataLength);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_ValidateMsgId);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_ZeroCopyReleaseAppId);
} /* end Test_SB_Utils */

/*
** Test getting the size of a command/telemetry message header
*/
void Test_CFE_SB_MsgHdrSize(void)
{
    CFE_MSG_Message_t msg;
    bool              hassec;
    CFE_MSG_Type_t    type;

    /* No secondary */
    hassec = false;
    type   = CFE_MSG_Type_Invalid;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    CFE_UtAssert_MEMOFFSET_EQ(CFE_SB_MsgHdrSize(&msg), sizeof(CFE_MSG_Message_t));

    /* Has secondary, tlm type */
    hassec = true;
    type   = CFE_MSG_Type_Tlm;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    CFE_UtAssert_MEMOFFSET_EQ(CFE_SB_MsgHdrSize(&msg), sizeof(CFE_MSG_TelemetryHeader_t));

    /* Has secondary, cmd type */
    type = CFE_MSG_Type_Cmd;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    CFE_UtAssert_MEMOFFSET_EQ(CFE_SB_MsgHdrSize(&msg), sizeof(CFE_MSG_CommandHeader_t));

    /* Has secondary, invalid type */
    type = CFE_MSG_Type_Invalid;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    CFE_UtAssert_MEMOFFSET_EQ(CFE_SB_MsgHdrSize(&msg), 0);

    /*
     * Note, this function currently has a type mismatch - it attempts to
     * return CFE_SB_BAD_ARGUMENT if passed a NULL MsgPtr value, but that
     * will be implicitly converted to size_t, and the result is
     * platform-defined.
     *
     * The function is called for coverage, but the result is not verified
     * due to this mismatch.
     */
    UtAssert_VOIDCALL(CFE_SB_MsgHdrSize(NULL));

} /* end Test_CFE_SB_MsgHdrSize */

/*
** Test getting a pointer to the user data portion of a message
*/
void Test_CFE_SB_GetUserData(void)
{
    CFE_MSG_Message_t msg;
    uint8 *           expected;
    bool              hassec;
    CFE_MSG_Type_t    type = CFE_MSG_Type_Invalid;
    struct
    {
        CFE_MSG_CommandHeader_t cmd;
        uint8                   payload;
    } cmd_uint8;
    struct
    {
        CFE_MSG_CommandHeader_t cmd;
        uint16                  payload;
    } cmd_uint16;
    struct
    {
        CFE_MSG_CommandHeader_t cmd;
        uint32                  payload;
    } cmd_uint32;
    struct
    {
        CFE_MSG_CommandHeader_t cmd;
        uint64                  payload;
    } cmd_uint64;
    struct
    {
        CFE_MSG_TelemetryHeader_t tlm;
        uint8                     payload;
    } tlm_uint8;
    struct
    {
        CFE_MSG_TelemetryHeader_t tlm;
        uint16                    payload;
    } tlm_uint16;
    struct
    {
        CFE_MSG_TelemetryHeader_t tlm;
        uint32                    payload;
    } tlm_uint32;
    struct
    {
        CFE_MSG_TelemetryHeader_t tlm;
        uint64                    payload;
    } tlm_uint64;

    /* No secondary */
    hassec = false;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);

    /* Expected return */
    expected = (uint8 *)&msg + sizeof(CCSDS_SpacePacket_t);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&msg), expected);

    /* Commands */
    hassec = true;
    type   = CFE_MSG_Type_Cmd;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&cmd_uint8.cmd.Msg), &(cmd_uint8.payload));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&cmd_uint16.cmd.Msg), &(cmd_uint16.payload));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&cmd_uint32.cmd.Msg), &(cmd_uint32.payload));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&cmd_uint64.cmd.Msg), &(cmd_uint64.payload));

    /* Telemetry */
    type = CFE_MSG_Type_Tlm;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&tlm_uint8.tlm.Msg), &(tlm_uint8.payload));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&tlm_uint16.tlm.Msg), &(tlm_uint16.payload));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&tlm_uint32.tlm.Msg), &(tlm_uint32.payload));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(&tlm_uint64.tlm.Msg), &(tlm_uint64.payload));

    /* Bad inputs */
    UtAssert_NULL(CFE_SB_GetUserData(NULL));

} /* end Test_CFE_SB_GetUserData */

/*
** Test setting and getting the user data length of a message
*/
void Test_CFE_SB_SetGetUserDataLength(void)
{
    CFE_MSG_Message_t msg;
    CFE_MSG_Size_t    size   = 10;
    CFE_MSG_Type_t    type   = CFE_MSG_Type_Invalid;
    bool              hassec = false;

    /* Pass through functions */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    UtAssert_INT32_EQ(CFE_SB_GetUserDataLength(&msg), size - sizeof(CCSDS_SpacePacket_t));

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

} /* end Util_CFE_SB_SetGetUserDataLength */

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
    SB_UT_ADD_SUBTEST(Test_OS_MutSem_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_ReqToSendEvent_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_PutDestBlk_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_Buffers);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_BadPipeInfo);
    SB_UT_ADD_SUBTEST(Test_SB_TransmitMsgPaths_Nominal);
    SB_UT_ADD_SUBTEST(Test_SB_TransmitMsgPaths_LimitErr);
    SB_UT_ADD_SUBTEST(Test_SB_TransmitMsgPaths_FullErr);
    SB_UT_ADD_SUBTEST(Test_SB_TransmitMsgPaths_WriteErr);
    SB_UT_ADD_SUBTEST(Test_SB_TransmitMsgPaths_IgnoreOpt);
    SB_UT_ADD_SUBTEST(Test_ReceiveBuffer_UnsubResubPath);
    SB_UT_ADD_SUBTEST(Test_MessageString);
} /* end Test_SB_SpecialCases */

/*
** Test pipe creation with semaphore take and give failures
*/
void Test_OS_MutSem_ErrLogic(void)
{
    CFE_SB_PipeId_t PipeId;
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

} /* end Test_OS_MutSemTake_ErrLogic */

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

} /* end Test_ReqToSendEvent_ErrLogic */

/*
** Test getting a destination descriptor to the SB memory pool using a null
** destination pointer
*/
void Test_PutDestBlk_ErrLogic(void)
{
    UtAssert_INT32_EQ(CFE_SB_PutDestinationBlk(NULL), CFE_SB_BAD_ARGUMENT);

    CFE_UtAssert_EVENTCOUNT(0);

} /* end Test_PutDestBlk_ErrLogic */

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

} /* end Test_CFE_SB_Buffers */

/*
** Test internal function to get the pipe table index for the given pipe ID
*/
void Test_CFE_SB_BadPipeInfo(void)
{
    CFE_SB_PipeId_t PipeId;
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

} /* end Test_CFE_SB_BadPipeInfo */

/*
** Test send housekeeping information command
*/
void Test_SB_TransmitMsgPaths_Nominal(void)
{
    union
    {
        CFE_SB_Buffer_t         SBBuf;
        CFE_MSG_CommandHeader_t Cmd;
    } Housekeeping;
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size;
    CFE_MSG_Type_t   Type;

    /* For internal send message call */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID);
    Size  = sizeof(CFE_SB_Global.HKTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For HK command processing */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    /* Repress sending the no subscriptions event and process request */
    CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter = 0;
    CFE_SB_Global.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_SEND_NO_SUBS_EID_BIT);
    CFE_SB_ProcessCmdPipePkt(&Housekeeping.SBBuf);

    /* The no subs event should not be in history but count should increment */
    CFE_UtAssert_EVENTNOTSENT(CFE_SB_SEND_NO_SUBS_EID);
    UtAssert_INT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.NoSubscribersCounter, 1);

    /* Repress get buffer error */
    CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter = 0;
    CFE_SB_Global.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_GET_BUF_ERR_EID_BIT);

    /* For internal send message call */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID);
    Size  = sizeof(CFE_SB_Global.HKTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For HK command processing */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    CFE_SB_ProcessCmdPipePkt(&Housekeeping.SBBuf);
    UtAssert_INT32_EQ(CFE_SB_Global.HKTlmMsg.Payload.MsgSendErrorCounter, 0);

    CFE_UtAssert_EVENTNOTSENT(CFE_SB_GET_BUF_ERR_EID);

    CFE_UtAssert_EVENTCOUNT(0);

    CFE_SB_Global.StopRecurseFlags[1] = 0;

    /* Create a message ID with the command bit set and disable reporting */
    MsgId = SB_UT_CMD_MID;
    Size  = sizeof(TlmPkt);
    Type  = CFE_MSG_Type_Cmd;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    /* Will fail because of deferred CFE_ES_GetPoolBuf failure return */
    UtAssert_INT32_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BUF_ALOC_ERR);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(3);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
} /* end Test_SB_TransmitMsgPaths */

void Test_SB_TransmitMsgPaths_LimitErr(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);

    /* Test inhibiting sending a "message ID limit error" message */
    MsgId = SB_UT_TLM_MID;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "MsgLimTestPipe"));

    /* Set maximum allowed messages on the pipe at one time to 1 */
    CFE_UtAssert_SETUP(CFE_SB_SubscribeEx(MsgId, PipeId, CFE_SB_DEFAULT_QOS, 1));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* First send should pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_SB_Global.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_MSGID_LIM_ERR_EID_BIT);
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));
    CFE_SB_Global.StopRecurseFlags[1] = 0;

    CFE_UtAssert_EVENTNOTSENT(CFE_SB_MSGID_LIM_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_TransmitMsgPaths */

void Test_SB_TransmitMsgPaths_FullErr(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);

    /* Test inhibiting sending a "pipe full" message */
    MsgId = SB_UT_TLM_MID;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "PipeFullTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* This send should pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* Tell the QueuePut stub to return OS_QUEUE_FULL on its next call */
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_QUEUE_FULL);
    CFE_SB_Global.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_Q_FULL_ERR_EID_BIT);
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));
    CFE_SB_Global.StopRecurseFlags[1] = 0;

    CFE_UtAssert_EVENTNOTSENT(CFE_SB_Q_FULL_ERR_EID_BIT);

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));
} /* end Test_SB_TransmitMsgPaths */

void Test_SB_TransmitMsgPaths_WriteErr(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);

    /* Test inhibiting sending a "pipe write error" message */
    MsgId = SB_UT_TLM_MID;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_SB_Global.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_Q_WR_ERR_EID_BIT);
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));
    CFE_SB_Global.StopRecurseFlags[1] = 0;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_EVENTCOUNT(2);

    CFE_UtAssert_EVENTNOTSENT(CFE_SB_Q_WR_ERR_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_TransmitMsgPaths */

void Test_SB_TransmitMsgPaths_IgnoreOpt(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);

    /* Setup Test skipping sending to a pipe when the pipe option is set to ignore */
    MsgId = SB_UT_TLM_MID;
    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "SkipPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    CFE_UtAssert_SETUP(CFE_SB_SetPipeOpts(PipeId, CFE_SB_PIPEOPTS_IGNOREMINE));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);

    /* Test skipping this pipe and the send should pass */
    CFE_UtAssert_SUCCESS(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_TEARDOWN(CFE_SB_SetPipeOpts(PipeId, 0));
    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_TransmitMsgPaths */

/*
** Test receiving a message response to an unsubscribing to message, then
** resubscribing to it while it's in the pipe
*/
void Test_ReceiveBuffer_UnsubResubPath(void)
{
    CFE_SB_Buffer_t *SBBufPtr;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    uint32           PipeDepth = 10;
    CFE_MSG_Type_t   Type      = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size      = sizeof(TlmPkt);

    CFE_UtAssert_SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvTestPipe"));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &Type, sizeof(Type), false);
    CFE_UtAssert_SETUP(CFE_SB_TransmitMsg(&TlmPkt.Hdr.Msg, true));

    CFE_UtAssert_SETUP(CFE_SB_Unsubscribe(MsgId, PipeId));
    CFE_UtAssert_SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    CFE_UtAssert_SUCCESS(CFE_SB_ReceiveBuffer(&SBBufPtr, PipeId, CFE_SB_PEND_FOREVER));

    UtAssert_NOT_NULL(SBBufPtr);

    CFE_UtAssert_EVENTCOUNT(4);

    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    CFE_UtAssert_EVENTSENT(CFE_SB_SUBSCRIPTION_REMOVED_EID);

    CFE_UtAssert_TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_ReceiveBuffer_UnsubResubPath */

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
} /* end Test_MessageString */
