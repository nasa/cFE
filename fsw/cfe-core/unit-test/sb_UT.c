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
#include "cfe_msg_api.h"

/*
 * A method to add an SB "Subtest"
 *
 * As SB has a lot of test cases it is helpful to group them.
 * This allows each test routine to reported consistently as "GroupName.TestName"
 *
 * This also implicitly includes a call to SB_ResetUnitTest() as a setup function,
 * so the test routines do _not_ need to do this explictly on every test case.
 */
#define SB_UT_ADD_SUBTEST(Func)    UT_AddSubTest(Func, SB_ResetUnitTest, NULL, __func__, #Func)

/*
 * MSG ID constants for unit testing:
 * Unit test cases should not directly use integer MsgId values
 *
 * The following constants are of the CFE_SB_MsgId_t type
 */

const CFE_SB_MsgId_t SB_UT_CMD_MID  = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE);
const CFE_SB_MsgId_t SB_UT_TLM_MID  = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE);

const CFE_SB_MsgId_t SB_UT_CMD_MID1 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE+1);
const CFE_SB_MsgId_t SB_UT_CMD_MID2 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE+2);
const CFE_SB_MsgId_t SB_UT_CMD_MID3 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE+3);
const CFE_SB_MsgId_t SB_UT_CMD_MID4 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE+4);
const CFE_SB_MsgId_t SB_UT_CMD_MID5 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE+5);
const CFE_SB_MsgId_t SB_UT_CMD_MID6 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_CMD_MID_VALUE_BASE+6);

const CFE_SB_MsgId_t SB_UT_TLM_MID1 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE+1);
const CFE_SB_MsgId_t SB_UT_TLM_MID2 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE+2);
const CFE_SB_MsgId_t SB_UT_TLM_MID3 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE+3);
const CFE_SB_MsgId_t SB_UT_TLM_MID4 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE+4);
const CFE_SB_MsgId_t SB_UT_TLM_MID5 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE+5);
const CFE_SB_MsgId_t SB_UT_TLM_MID6 = CFE_SB_MSGID_WRAP_VALUE(SB_UT_TLM_MID_VALUE_BASE+6);

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
const CFE_SB_MsgId_t SB_UT_INTERMEDIATE_VALID_MID = CFE_SB_MSGID_WRAP_VALUE(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID / 2 + 1);

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

/*
 * Helper function to "corrupt" a resource ID value in a consistent/predicatble way,
 * which can also be un-done easily.
 */
uint32 UT_SB_ResourceID_Modify(uint32 InitialID, int32 Modifier)
{
    uint32 NewValue = InitialID + Modifier;
    return (NewValue);
}

/*
** Functions
*/
void UtTest_Setup(void)
{
    UT_Init("sb");
    UT_Text("cFE SB Unit Test Output File\n\n");

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
    Test_SendMsg_API();
    Test_RcvMsg_API();
    SB_UT_ADD_SUBTEST(Test_CleanupApp_API);
    Test_SB_Utils();

    Test_SB_SpecialCases();

} /* end main */

/*
** Reset variable values and sockets prior to a test
*/
void SB_ResetUnitTest(void)
{
    /* If any sockets were left open then report and close them */
    UT_CheckForOpenSockets();
    UT_InitData();
    CFE_SB_EarlyInit();
} /* end SB_ResetUnitTest */

/*
** Function for calling SB application initialization
** test functions
*/
void Test_SB_AppInit(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_ESRegFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_EVSRegFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_EVSSendEvtFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_CrPipeFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_Sub1Fail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_Sub2Fail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_GetPoolFail);
    SB_UT_ADD_SUBTEST(Test_SB_AppInit_PutPoolFail);
} /* end Test_SB_AppInit */

/*
** Test task init with ES_RegisterApp returning error
*/
void Test_SB_AppInit_ESRegFail(void)
{
    int32 ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterApp), 1, ForcedRtnVal);
	ASSERT_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    EVTCNT(0);

} /* end Test_SB_AppInit_ESRegFail */

/*
** Test task init with EVS_Register returning error
*/
void Test_SB_AppInit_EVSRegFail(void)
{
    int32 ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, ForcedRtnVal);
    ASSERT_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    EVTCNT(0);

} /* end Test_SB_AppInit_EVSRegFail */

/*
** Test task init with EVS_SendEvent returning error on task init event
*/
void Test_SB_AppInit_EVSSendEvtFail(void)
{
    int32 ForcedRtnVal = -1;
    CFE_ES_TaskInfo_t TestTaskInfo;

    /* To get coverage on CFE_SB_GetAppTskName(), this ensures that the
     * path with different app/task names is followed on at least one event.
     */
    memset(&TestTaskInfo, 0, sizeof(TestTaskInfo));
    strncpy((char*)TestTaskInfo.TaskName, "test", sizeof(TestTaskInfo.TaskName)-1);
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetTaskInfo), &TestTaskInfo, sizeof(TestTaskInfo), false);

    /* There are three events prior to init, pipe created (1) and subscription
     * rcvd (2). Fourth is SB initialized, but it is the first to use SendEvent.
     * (The others use SendEventWithAppID which is a different counter).
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, ForcedRtnVal);
    ASSERT_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    EVTCNT(7);

    TEARDOWN(CFE_SB_DeletePipe(CFE_SB.CmdPipe));

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
    ASSERT_EQ(CFE_SB_AppInit(), CFE_SB_PIPE_CR_ERR);

    EVTCNT(1);

    EVTSENT(CFE_SB_CR_PIPE_ERR_EID);

} /* end Test_SB_AppInit_CrPipeFail */

/*
** Test task init with a failure on first subscription request
*/
void Test_SB_AppInit_Sub1Fail(void)
{
    CFE_SB_PipeId_t PipeId = 0;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, -1);
    ASSERT_EQ(CFE_SB_AppInit(), CFE_SB_BUF_ALOC_ERR);

    EVTCNT(3);

    EVTSENT(CFE_SB_DEST_BLK_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_AppInit_Sub1Fail */

/*
** Test task init with a failure on second subscription request
*/
void Test_SB_AppInit_Sub2Fail(void)
{
    CFE_SB_PipeId_t PipeId = 0;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 2, -1);
    ASSERT_EQ(CFE_SB_AppInit(), CFE_SB_BUF_ALOC_ERR);

    EVTCNT(5);

    EVTSENT(CFE_SB_DEST_BLK_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_AppInit_Sub2Fail */

/*
** Test task init with a GetPool failure
*/
void Test_SB_AppInit_GetPoolFail(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 4, ForcedRtnVal);

    ASSERT_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    EVTCNT(7);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_AppInit_GetPoolFail */

/*
** Test task init with a PutPool failure
*/
void Test_SB_AppInit_PutPoolFail(void)
{
    CFE_SB_PipeId_t PipeId = 0;
    int32           ForcedRtnVal = -1;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, ForcedRtnVal);

    ASSERT_EQ(CFE_SB_AppInit(), ForcedRtnVal);

    EVTCNT(7);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_AppInit_PutPoolFail */

/*
** Function for calling SB main task test functions
*/
void Test_SB_MainRoutine(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_Main_RcvErr);
    SB_UT_ADD_SUBTEST(Test_SB_Main_InitErr);
} /* end Test_SB_MainRoutine */

/*
** Test main task with a packet receive error
*/
void Test_SB_Main_RcvErr(void)
{
    CFE_SB_PipeId_t PipeId = 0;

    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, -1);
    CFE_SB_TaskMain();

    EVTCNT(10);

    EVTSENT(CFE_SB_INIT_EID);

    EVTSENT(CFE_SB_Q_RD_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Main_RcvErr */

/*
** Test main task with an application initialization error
*/
void Test_SB_Main_InitErr(void)
{
    CFE_SB_PipeId_t PipeId = 0;

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    CFE_SB_TaskMain();

    EVTCNT(7);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Main_InitErr */

/*
** Function for calling SB command test functions
*/
void Test_SB_Cmds(void)
{
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_Noop);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RstCtrs);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_Stats);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoDef);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoSpec);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoCreateFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoHdrFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_RoutingInfoWriteFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoDef);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoSpec);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoCreateFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoHdrFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_PipeInfoWriteFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoDef);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoSpec);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoCreateFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoHdrFail);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_MapInfoWriteFail);
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
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_SubRptUnexpCmdCode);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_BadCmdLength);
    SB_UT_ADD_SUBTEST(Test_SB_Cmds_UnexpMsgId);
} /* end Test_SB_Cmds */

/*
** Test no-op command
*/
void Test_SB_Cmds_Noop(void)
{
    CFE_SB_CmdHdr_t   NoParamCmd;
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_NOOP_CC;
    CFE_SB_MsgId_t    MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size = sizeof(NoParamCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_CMD0_RCVD_EID);

} /* end Test_SB_Cmds_Noop */

/*
** Test reset counters command
*/
void Test_SB_Cmds_RstCtrs(void)
{
    CFE_SB_CmdHdr_t   NoParamCmd;
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_RESET_COUNTERS_CC;
    CFE_SB_MsgId_t    MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t    Size = sizeof(NoParamCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_CMD1_RCVD_EID);

} /* Test_SB_Cmds_RstCtrs */

/*
** Test send SB stats command
*/
void Test_SB_Cmds_Stats(void)
{
    CFE_SB_CmdHdr_t   NoParamCmd;
    CFE_MSG_FcnCode_t FcnCode;
    CFE_SB_MsgId_t    MsgId;
    CFE_MSG_Size_t    Size;

    /* For internal SendMsg call */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_STATS_TLM_MID);
    Size = sizeof(CFE_SB.StatTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For Generic command processing */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    Size = sizeof(NoParamCmd);
    FcnCode = CFE_SB_SEND_SB_STATS_CC;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();

    /* No subs event and command processing event */
    EVTCNT(2);

    EVTSENT(CFE_SB_SND_STATS_EID);

} /* end Test_SB_Cmds_Stats */

/*
** Test send routing information command using the default file name
*/
void Test_SB_Cmds_RoutingInfoDef(void)
{
    CFE_SB_PipeId_t           PipeId = 0;
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_MSG_FcnCode_t         FcnCode = CFE_SB_SEND_ROUTING_INFO_CC;
    CFE_SB_MsgId_t            MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t            Size = sizeof(WriteFileCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy((char *)WriteFileCmd.Payload.Filename, "", sizeof(WriteFileCmd.Payload.Filename));

    /* Make some routing info by calling CFE_SB_AppInit */
    SETUP(CFE_SB_AppInit());

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(12);

    EVTSENT(CFE_SB_INIT_EID);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    EVTSENT(CFE_SB_INIT_EID);

    EVTSENT(CFE_SB_SND_RTG_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Cmds_RoutingInfoDef */

/*
** Test send routing information command using a specified file name
*/
void Test_SB_Cmds_RoutingInfoSpec(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_MSG_FcnCode_t         FcnCode = CFE_SB_SEND_ROUTING_INFO_CC;
    CFE_SB_MsgId_t            MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t            Size = sizeof(WriteFileCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy((char *)WriteFileCmd.Payload.Filename, "RoutingTstFile", sizeof(WriteFileCmd.Payload.Filename));
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_SND_RTG_EID);

} /* end Test_SB_Cmds_RoutingInfoSpec */

/*
**  Test send routing information command with a file creation failure
*/
void Test_SB_Cmds_RoutingInfoCreateFail(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_MSG_FcnCode_t         FcnCode = CFE_SB_SEND_ROUTING_INFO_CC;
    CFE_SB_MsgId_t            MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t            Size = sizeof(WriteFileCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy((char *)WriteFileCmd.Payload.Filename, "RoutingTstFile", sizeof(WriteFileCmd.Payload.Filename));
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;

    /* Make function CFE_SB_SendRtgInfo return CFE_SB_FILE_IO_ERR */
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_SND_RTG_ERR1_EID);

} /* end Test_SB_Cmds_RoutingInfoCreateFail */

/*
** Test send routing information command with a file header write failure
*/
void Test_SB_Cmds_RoutingInfoHdrFail(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);

    ASSERT_EQ(CFE_SB_SendRtgInfo("RoutingTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(1);

    EVTSENT(CFE_SB_FILEWRITE_ERR_EID);

} /* end Test_SB_Cmds_RoutingInfoHdrFail */

/*
** Test send routing information command with a file write failure on
** the second write
*/
void Test_SB_Cmds_RoutingInfoWriteFail(void)
{
    CFE_SB_PipeId_t PipeId = 0;

    /* Make some routing info by calling CFE_SB_AppInit */
    SETUP(CFE_SB_AppInit());

    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, -1);

    ASSERT_EQ(CFE_SB_SendRtgInfo("RoutingTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(11);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    EVTSENT(CFE_SB_INIT_EID);

    EVTSENT(CFE_SB_FILEWRITE_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Cmds_RoutingInfoWriteFail */

/*
** Test send pipe information command using the default file name
*/
void Test_SB_Cmds_PipeInfoDef(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_SB_PipeId_t           PipeId1;
    CFE_SB_PipeId_t           PipeId2;
    CFE_SB_PipeId_t           PipeId3;
    uint16                    PipeDepth = 10;
    CFE_MSG_FcnCode_t         FcnCode = CFE_SB_SEND_PIPE_INFO_CC;
    CFE_SB_MsgId_t            MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t            Size = sizeof(WriteFileCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy((char *)WriteFileCmd.Payload.Filename, "", sizeof(WriteFileCmd.Payload.Filename));

    /* Create some pipe info */
    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(4);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);
    EVTSENT(CFE_SB_SND_RTG_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    TEARDOWN(CFE_SB_DeletePipe(PipeId3));

} /* end Test_SB_Cmds_PipeInfoDef */

/*
** Test send pipe information command using a specified file name
*/
void Test_SB_Cmds_PipeInfoSpec(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_MSG_FcnCode_t         FcnCode = CFE_SB_SEND_PIPE_INFO_CC;
    CFE_SB_MsgId_t            MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t            Size = sizeof(WriteFileCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy((char *)WriteFileCmd.Payload.Filename, "PipeTstFile", sizeof(WriteFileCmd.Payload.Filename));
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_SND_RTG_EID);

} /* end Test_SB_Cmds_PipeInfoSpec */

/*
** Test send pipe information command with a file creation failure
*/
void Test_SB_Cmds_PipeInfoCreateFail(void)
{
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    ASSERT_EQ(CFE_SB_SendPipeInfo("PipeTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(1);

    EVTSENT(CFE_SB_SND_RTG_ERR1_EID);

} /* end Test_SB_Cmds_PipeInfoCreateFail */

/*
** Test send pipe information command with a file header write failure
*/
void Test_SB_Cmds_PipeInfoHdrFail(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);
    ASSERT_EQ(CFE_SB_SendPipeInfo("PipeTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(1);

    EVTSENT(CFE_SB_FILEWRITE_ERR_EID);

} /* end Test_SB_Cmds_PipeInfoHdrFail */

/*
** Test send pipe information command with a file write failure on
** the second write
*/
void Test_SB_Cmds_PipeInfoWriteFail(void)
{
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_PipeId_t PipeId3;
    uint16          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, -1);

    ASSERT_EQ(CFE_SB_SendPipeInfo("PipeTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(4);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    EVTSENT(CFE_SB_FILEWRITE_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    TEARDOWN(CFE_SB_DeletePipe(PipeId3));

} /* end Test_SB_Cmds_PipeInfoWriteFail */

/*
** Test send map information command using the default file name
*/
void Test_SB_Cmds_MapInfoDef(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_SB_PipeId_t           PipeId1;
    CFE_SB_PipeId_t           PipeId2;
    CFE_SB_PipeId_t           PipeId3;
    CFE_SB_MsgId_t            MsgId0 = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t            MsgId1 = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t            MsgId2 = SB_UT_TLM_MID3;
    CFE_SB_MsgId_t            MsgId3 = SB_UT_TLM_MID4;
    CFE_SB_MsgId_t            MsgId4 = SB_UT_TLM_MID5;
    CFE_SB_MsgId_t            MsgId5 = SB_UT_TLM_MID6;
    uint16                    PipeDepth = 10;
    CFE_MSG_FcnCode_t         FcnCode = CFE_SB_SEND_MAP_INFO_CC;
    CFE_SB_MsgId_t            MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t            Size = sizeof(WriteFileCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy((char *)WriteFileCmd.Payload.Filename, "", sizeof(WriteFileCmd.Payload.Filename));

    /* Create some map info */
    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId2));
    SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId2, PipeId3));
    SETUP(CFE_SB_Subscribe(MsgId3, PipeId3));
    SETUP(CFE_SB_Subscribe(MsgId4, PipeId3));
    SETUP(CFE_SB_Subscribe(MsgId5, PipeId2));
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(18);

    EVTSENT(CFE_SB_SND_RTG_EID);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    TEARDOWN(CFE_SB_DeletePipe(PipeId3));

} /* end Test_SB_Cmds_MapInfoDef */

/*
** Test send map information command using a specified file name
*/
void Test_SB_Cmds_MapInfoSpec(void)
{
    CFE_SB_WriteFileInfoCmd_t WriteFileCmd;
    CFE_MSG_FcnCode_t         FcnCode = CFE_SB_SEND_MAP_INFO_CC;
    CFE_SB_MsgId_t            MsgId = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t            Size = sizeof(WriteFileCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    strncpy((char *)WriteFileCmd.Payload.Filename, "MapTstFile", sizeof(WriteFileCmd.Payload.Filename));
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &WriteFileCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_SND_RTG_EID);

} /* end Test_SB_Cmds_MapInfoSpec */

/*
** Test send map information command with a file creation failure
*/
void Test_SB_Cmds_MapInfoCreateFail(void)
{
    UT_SetForceFail(UT_KEY(OS_OpenCreate), OS_ERROR);
    ASSERT_EQ(CFE_SB_SendMapInfo("MapTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(1);

    EVTSENT(CFE_SB_SND_RTG_ERR1_EID);

} /* end Test_SB_Cmds_MapInfoCreateFail */

/*
** Test send map information command with a file header write failure
*/
void Test_SB_Cmds_MapInfoHdrFail(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);
    ASSERT_EQ(CFE_SB_SendMapInfo("MapTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(1);

    EVTSENT(CFE_SB_FILEWRITE_ERR_EID);

} /* end Test_SB_Cmds_MapInfoHdrFail */

/*
** Test send map information command with a file write failure on
** the second write
*/
void Test_SB_Cmds_MapInfoWriteFail(void)
{
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_PipeId_t PipeId3;
    CFE_SB_MsgId_t  MsgId0 = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t  MsgId1 = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t  MsgId2 = SB_UT_TLM_MID3;
    CFE_SB_MsgId_t  MsgId3 = SB_UT_TLM_MID4;
    CFE_SB_MsgId_t  MsgId4 = SB_UT_TLM_MID5;
    CFE_SB_MsgId_t  MsgId5 = SB_UT_TLM_MID6;
    uint16          PipeDepth = 10;

    /* Create some map info */
    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_CreatePipe(&PipeId3, PipeDepth, "TestPipe3"));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId2));
    SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId2, PipeId3));
    SETUP(CFE_SB_Subscribe(MsgId3, PipeId3));
    SETUP(CFE_SB_Subscribe(MsgId4, PipeId3));
    SETUP(CFE_SB_Subscribe(MsgId5, PipeId2));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, -1);

    ASSERT_EQ(CFE_SB_SendMapInfo("MapTstFile"), CFE_SB_FILE_IO_ERR);

    EVTCNT(18);

    EVTSENT(CFE_SB_FILEWRITE_ERR_EID);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));
    TEARDOWN(CFE_SB_DeletePipe(PipeId3));

} /* end Test_SB_Cmds_MapInfoWriteFail */

/*
** Test command to enable a specific route using a valid route
*/
void Test_SB_Cmds_EnRouteValParam(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId;
    CFE_SB_MsgId_t     MsgId = SB_UT_TLM_MID;
    uint16             PipeDepth = 5;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "EnRouteTestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    EnDisRouteCmd.Payload.MsgId = MsgId;
    EnDisRouteCmd.Payload.Pipe = PipeId;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(4);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    EVTSENT(CFE_SB_ENBL_RTE2_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Cmds_EnRouteValParam */

/*
** Test command to enable a specific route using a non-existent route
*/
void Test_SB_Cmds_EnRouteNonExist(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId1;
    CFE_SB_PipeId_t    PipeId2;
    CFE_SB_MsgId_t     MsgId = SB_UT_TLM_MID;
    uint16             PipeDepth = 5;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "EnRouteTestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "EnRouteTestPipe2"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId1));
    EnDisRouteCmd.Payload.MsgId = MsgId;
    EnDisRouteCmd.Payload.Pipe = PipeId2;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(5);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);
    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    EVTSENT(CFE_SB_ENBL_RTE1_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_SB_Cmds_EnRouteNonExist */

/*
** Test command to enable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_EnRouteInvParam(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnDisRouteCmd.Payload.MsgId = SB_UT_LAST_VALID_MID;
    EnDisRouteCmd.Payload.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_ENBL_RTE3_EID);

} /* end Test_SB_Cmds_EnRouteInvParam */

/*
** Test command to enable a specific route using an invalid message ID
*/
void Test_SB_Cmds_EnRouteInvParam2(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnDisRouteCmd.Payload.MsgId = CFE_SB_INVALID_MSG_ID;
    EnDisRouteCmd.Payload.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_ENBL_RTE3_EID);

} /* end Test_SB_Cmds_EnRouteInvParam2 */

/*
** Test command to enable a specific route using a message ID greater
** than the maximum allowed
*/
void Test_SB_Cmds_EnRouteInvParam3(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnDisRouteCmd.Payload.MsgId = SB_UT_ALTERNATE_INVALID_MID;
    EnDisRouteCmd.Payload.Pipe = 0;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_ENBL_RTE3_EID);

} /* end Test_SB_Cmds_EnRouteInvParam3 */

/*
**  Test command to disable a specific route using a valid route
*/
void Test_SB_Cmds_DisRouteValParam(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId;
    CFE_SB_MsgId_t     MsgId = SB_UT_TLM_MID;
    uint16             PipeDepth = 5;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "DisRouteTestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    EnDisRouteCmd.Payload.MsgId = MsgId;
    EnDisRouteCmd.Payload.Pipe = PipeId;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(4);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    EVTSENT(CFE_SB_DSBL_RTE2_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_Cmds_DisRouteValParam */

/*
** Test command to disable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_DisRouteNonExist(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_SB_PipeId_t    PipeId1, PipeId2;
    CFE_SB_MsgId_t     MsgId = SB_UT_TLM_MID;
    uint16             PipeDepth = 5;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "DisRouteTestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "DisRouteTestPipe2"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId1));
    EnDisRouteCmd.Payload.MsgId = MsgId;
    EnDisRouteCmd.Payload.Pipe = PipeId2;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(5);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);
    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    EVTSENT(CFE_SB_DSBL_RTE1_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_SB_Cmds_DisRouteNonExist */

/*
** Test command to disable a specific route using an invalid pipe ID
*/
void Test_SB_Cmds_DisRouteInvParam(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnDisRouteCmd.Payload.MsgId = SB_UT_LAST_VALID_MID;
    EnDisRouteCmd.Payload.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_DSBL_RTE3_EID);

} /* end Test_SB_Cmds_DisRouteInvParam */

/*
** Test command to disable a specific route using an invalid message ID
*/
void Test_SB_Cmds_DisRouteInvParam2(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnDisRouteCmd.Payload.MsgId = CFE_SB_INVALID_MSG_ID;
    EnDisRouteCmd.Payload.Pipe = 3;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_DSBL_RTE3_EID);

} /* end Test_SB_Cmds_DisRouteInvParam2 */

/*
** Test command to disable a specific route using a message ID greater
** than the maximum allowed
*/
void Test_SB_Cmds_DisRouteInvParam3(void)
{
    CFE_SB_RouteCmd_t  EnDisRouteCmd;
    CFE_MSG_FcnCode_t  FcnCode = CFE_SB_DISABLE_ROUTE_CC;
    CFE_SB_MsgId_t     MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t     Size = sizeof(EnDisRouteCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    EnDisRouteCmd.Payload.MsgId = SB_UT_ALTERNATE_INVALID_MID;
    EnDisRouteCmd.Payload.Pipe = 0;
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnDisRouteCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_DSBL_RTE3_EID);

} /* end Test_SB_Cmds_DisRouteInvParam3 */

/*
** Test send housekeeping information command
*/
void Test_SB_Cmds_SendHK(void)
{
    CFE_SB_CmdHdr_t NoParamCmd;
    CFE_SB_MsgId_t  MsgIdCmd;
    CFE_MSG_Size_t  Size;

    /* For internal SendMsg call */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID);
    Size = sizeof(CFE_SB.HKTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For HK command processing */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;

    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_SEND_NO_SUBS_EID);

} /* end Test_SB_Cmds_SendHK */

/*
** Test command to build and send a SB packet containing a complete
** list of current subscriptions
*/
void Test_SB_Cmds_SendPrevSubs(void)
{
    CFE_SB_SendPrevSubs_t NoParamCmd;
    CFE_SB_PipeId_t       PipeId1;
    CFE_SB_PipeId_t       PipeId2;
    CFE_SB_MsgId_t        MsgId = SB_UT_BARE_TLM_MID3;
    uint16                MsgLim = 4;
    uint16                PipeDepth = 50;
    int32                 i;
    int32                 NumEvts;
    CFE_MSG_FcnCode_t     FcnCode = CFE_SB_SEND_PREV_SUBS_CC;
    CFE_SB_MsgId_t        MsgIdCmd;
    CFE_MSG_Size_t        Size;

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    NumEvts = 2; /* one for each pipe create */

    /* Two full pkts to be sent plus five entries in a partial pkt, skipping MSGID 0x0D */
    for (i = 0; i < CFE_SB_SUB_ENTRIES_PER_PKT * 2 + 5; i++)
    {
        /* Skip subscribing to ALLSUBS mid. This is the one we are testing.
         * MsgID for this in CCSDS v.1 was 0x180d so this MID did not appear in the
         * SB sub list. This results in multiple NO_SUBS_EID sent which we are not 
         * testing here so it is irrelevent
         * For CCSDS v.2 CFE_SB_ALLSUBS_TLM_MID (0x0d) now appears in the
         * SB subscription list and thus we must skip adding 0x0D to the list
         * as we were going from MSGID 0-45 (0x00-0x2D)
         * */
        if (i != CFE_SB_ALLSUBS_TLM_MID)
        {
            NumEvts += 2;
            SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId1));
        }
    }

    SETUP(CFE_SB_SubscribeLocal(MsgId, PipeId2, MsgLim));

    /* For 3 internal SendMsg calls */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size = sizeof(CFE_SB.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Set buffers for gets */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    Size = sizeof(CFE_SB_SendPrevSubs_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_ProcessCmdPipePkt();

    NumEvts += 8;  /* +2 for the subscribe, +6 for the SEND_PREV_SUBS_CC */
    EVTCNT(NumEvts);

    /* Round out the number to three full pkts in order to test branch path
     * coverage, MSGID 0x0D was skipped in previous subscription loop
     */
    for (; i < CFE_SB_SUB_ENTRIES_PER_PKT * 3; i++)
    {
        SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId1));
        NumEvts += 2;
    }

    SETUP(CFE_SB_SubscribeLocal(MsgId, PipeId2, MsgLim));

    /* For 3 internal SendMsg calls */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size = sizeof(CFE_SB.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Set buffers for gets */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    Size = sizeof(CFE_SB_SendPrevSubs_t);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB_ProcessCmdPipePkt();

    NumEvts += 8;  /* +2 for the subscribe, +6 for the SEND_PREV_SUBS_CC */

    EVTCNT(NumEvts);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);
    EVTSENT(CFE_SB_SEND_NO_SUBS_EID);
    EVTSENT(CFE_SB_FULL_SUB_PKT_EID);
    EVTSENT(CFE_SB_PART_SUB_PKT_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_SB_Cmds_SendPrevSubs */

/*
** Test command to enable subscription reporting
*/
void Test_SB_Cmds_SubRptOn(void)
{
    CFE_SB_CmdHdr_t   NoParamCmd;
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_ENABLE_SUB_REPORTING_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    CFE_MSG_Size_t    Size = sizeof(NoParamCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(0);

} /* end Test_SB_Cmds_SubRptOn */

/*
** Test command to disable subscription reporting
*/
void Test_SB_Cmds_SubRptOff(void)
{
    CFE_SB_CmdHdr_t   NoParamCmd;
    CFE_MSG_FcnCode_t FcnCode = CFE_SB_DISABLE_SUB_REPORTING_CC;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);
    CFE_MSG_Size_t    Size = sizeof(NoParamCmd);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(0);

} /* end Test_SB_Cmds_SubRptOff */

/*
** Test command handler response to an invalid command code
*/
void Test_SB_Cmds_CmdUnexpCmdCode(void)
{
    CFE_SB_CmdHdr_t   NoParamCmd;
    CFE_MSG_FcnCode_t FcnCode = 99;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Use a command code known to be invalid */
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();
    EVTCNT(1);
    EVTSENT(CFE_SB_BAD_CMD_CODE_EID);

} /* end Test_SB_Cmds_UnexpCmdCode */

/*
** Test command handler response to an invalid command code
*/
void Test_SB_Cmds_SubRptUnexpCmdCode(void)
{
    CFE_SB_CmdHdr_t   NoParamCmd;
    CFE_MSG_FcnCode_t FcnCode = 99;
    CFE_SB_MsgId_t    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_SUB_RPT_CTRL_MID);

    /* Second GetFcnCode is for reporting */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    /* Use a command code known to be invalid */
    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_BAD_CMD_CODE_EID);

} /* end Test_SB_Cmds_UnexpCmdCode */

/*
** Test command handler response to an incorrect length
*/
void Test_SB_Cmds_BadCmdLength(void)
{
    /*
     * Just choosing "EnableRoute" command here as it has a non-empty payload
     */
    CFE_SB_EnableRoute_t EnableRouteCmd;
    CFE_MSG_FcnCode_t    FcnCode = CFE_SB_ENABLE_ROUTE_CC;
    CFE_SB_MsgId_t       MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_CMD_MID);
    CFE_MSG_Size_t       Size = sizeof(EnableRouteCmd) - 1;

    /* Second GetMsgId and GetFcnCode are for reporting */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &EnableRouteCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_LEN_ERR_EID);

} /* end Test_SB_Cmds_BadCmdLength */

/*
** Test command handler response to an invalid message ID
*/
void Test_SB_Cmds_UnexpMsgId(void)
{
    CFE_SB_MsgId_t  MsgIdCmd = SB_UT_TLM_MID;
    CFE_SB_CmdHdr_t NoParamCmd;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB_ProcessCmdPipePkt();

    EVTCNT(1);

    EVTSENT(CFE_SB_BAD_MSGID_EID);

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
    UT_Report(__FILE__, __LINE__,
              CFE_SB_EarlyInit() == OS_ERR_NO_FREE_IDS,
              "CFE_SB_EarlyInit", "Sem Create error logic");
} /* end Test_SB_EarlyInit_SemCreateError */

/*
** Test early initialization response to a pool create ex failure
*/
void Test_SB_EarlyInit_PoolCreateError(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PoolCreateEx), 1, CFE_ES_BAD_ARGUMENT);
    UT_Report(__FILE__, __LINE__,
              CFE_SB_EarlyInit() == CFE_ES_BAD_ARGUMENT,
              "CFE_SB_EarlyInit", "PoolCreateEx error logic");
} /* end Test_SB_EarlyInit_PoolCreateError */

/*
** Test successful early initialization
*/
void Test_SB_EarlyInit_NoErrors(void)
{
    CFE_SB_EarlyInit();
    UT_Report(__FILE__, __LINE__,
              CFE_SB_EarlyInit() == CFE_SUCCESS,
              "CFE_SB_EarlyInit", "No errors test");
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
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetTaskInfo), 1, CFE_ES_ERR_APPID);

    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS); /* Avoids creating socket */
    ASSERT_EQ(CFE_SB_CreatePipe(NULL, PipeDepth, "TestPipe"), CFE_SB_BAD_ARGUMENT);

    EVTCNT(1);

    EVTSENT(CFE_SB_CR_PIPE_BAD_ARG_EID);

} /* end Test_CreatePipe_NullPtr */

/*
** Test create pipe response to valid pipe depths
*/
void Test_CreatePipe_ValPipeDepth(void)
{
    CFE_SB_PipeId_t PipeIdReturned[3];

    ASSERT(CFE_SB_CreatePipe(&PipeIdReturned[0], 99, "TestPipe99"));
    ASSERT(CFE_SB_CreatePipe(&PipeIdReturned[1], 255, "TestPipe255"));
    ASSERT(CFE_SB_CreatePipe(&PipeIdReturned[2],
             CFE_PLATFORM_SB_MAX_PIPE_DEPTH, "TestPipeMaxDepth"));

    EVTCNT(3);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[0]));
    TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[1]));
    TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[2]));

} /* end Test_CreatePipe_ValPipeDepth */

/*
** Test create pipe response to invalid pipe depths
*/
void Test_CreatePipe_InvalPipeDepth(void)
{
    CFE_SB_PipeId_t PipeIdReturned[3];

    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS); /* Avoid creating socket */
    ASSERT_EQ(CFE_SB_CreatePipe(&PipeIdReturned[0], 0, "TestPipe1"), CFE_SB_BAD_ARGUMENT);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS);
    ASSERT_EQ(CFE_SB_CreatePipe(&PipeIdReturned[1],
             CFE_PLATFORM_SB_MAX_PIPE_DEPTH + 1, "TestPipeMaxDepPlus1"), CFE_SB_BAD_ARGUMENT);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 1, OS_SUCCESS);
    ASSERT_EQ(CFE_SB_CreatePipe(&PipeIdReturned[2], 0xffff, "TestPipeffff"), CFE_SB_BAD_ARGUMENT);

    EVTCNT(3);

    EVTSENT(CFE_SB_CR_PIPE_BAD_ARG_EID);

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

    /* Create maximum number of pipes + 1. Only one 'create pipe' failure
     * expected
     */
    for (i = 0; i < (CFE_PLATFORM_SB_MAX_PIPES + 1); i++)
    {
        snprintf(PipeName, OS_MAX_API_NAME, "TestPipe%ld", (long) i);

        if (i < CFE_PLATFORM_SB_MAX_PIPES)
        {
        	SETUP(CFE_SB_CreatePipe(&PipeIdReturned[i], PipeDepth, &PipeName[0]));
        }
        else
        {
        	ASSERT_EQ(CFE_SB_CreatePipe(&PipeIdReturned[i], PipeDepth, &PipeName[0]), CFE_SB_MAX_PIPES_MET);
        }
    }

    EVTSENT(CFE_SB_MAX_PIPES_MET_EID);

    /* Clean up */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_PIPES; i++)
    {
        TEARDOWN(CFE_SB_DeletePipe(PipeIdReturned[i]));
    }

} /* end Test_CreatePipe_MaxPipes */

/*
** Test create pipe response to duplicate pipe names
*/
void Test_CreatePipe_SamePipeName(void)
{
    CFE_SB_PipeId_t FirstPipeId = -1;
    CFE_SB_PipeId_t PipeId = -1;
    uint16 PipeDepth = 1;
    char PipeName[] = "Test_CFE_SB";

    /* Need to make sure OS_QueueCreate() stub fails on second call   */
    /* to mimick the failure expected when passing the same pipe name */
    UT_SetDeferredRetcode(UT_KEY(OS_QueueCreate), 2, OS_ERR_NAME_TAKEN);

    /* First call to CFE_SB_CreatePipe() should succeed */
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName));

    FirstPipeId = PipeId;

    /* Second call to CFE_SB_CreatePipe with same PipeName should fail */
    ASSERT_EQ(CFE_SB_CreatePipe(&PipeId, PipeDepth, PipeName), CFE_SB_PIPE_CR_ERR);

    ASSERT_EQ(PipeId, FirstPipeId);

    EVTCNT(2);

    EVTSENT(CFE_SB_CR_PIPE_NAME_TAKEN_EID);

    /* Call to CFE_SB_DeletePipe with the first pipe id created should work fine */
    TEARDOWN(CFE_SB_DeletePipe(PipeId));

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

    SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));
    ASSERT(CFE_SB_DeletePipe(PipedId));

    EVTCNT(2);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);
    EVTSENT(CFE_SB_PIPE_DELETED_EID);

} /* end Test_DeletePipe_NoSubs */

/*
** Test delete pipe response to deleting a pipe with subscriptions
*/
void Test_DeletePipe_WithSubs(void)
{
    CFE_SB_PipeId_t PipedId;
    CFE_SB_MsgId_t  MsgId0 = SB_UT_CMD_MID1;
    CFE_SB_MsgId_t  MsgId1 = SB_UT_CMD_MID2;
    CFE_SB_MsgId_t  MsgId2 = SB_UT_CMD_MID3;
    CFE_SB_MsgId_t  MsgId3 = SB_UT_CMD_MID4;
    uint16          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId0, PipedId));
    SETUP(CFE_SB_Subscribe(MsgId1, PipedId));
    SETUP(CFE_SB_Subscribe(MsgId2, PipedId));
    SETUP(CFE_SB_Subscribe(MsgId3, PipedId));
    ASSERT(CFE_SB_DeletePipe(PipedId));

    EVTCNT(14);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);
    EVTSENT(CFE_SB_PIPE_DELETED_EID);

} /* end Test_DeletePipe_WithSubs */

/*
** Test delete pipe response to an invalid pipe ID
*/
void Test_DeletePipe_InvalidPipeId(void)
{
    CFE_SB_PipeId_t PipeId = 30;

    ASSERT_EQ(CFE_SB_DeletePipe(PipeId), CFE_SB_BAD_ARGUMENT);

    EVTCNT(1);

    EVTSENT(CFE_SB_DEL_PIPE_ERR1_EID);

} /* end Test_DeletePipe_InvalidPipeId */

/*
** Test delete pipe response to an invalid pipe owner
*/
void Test_DeletePipe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipedId;
    uint32          RealOwner;
    uint16          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));

    /* Change owner of pipe through memory corruption */
    RealOwner = CFE_SB.PipeTbl[PipedId].AppId;

    /* Choose a value that is sure not to be owner */
    CFE_SB.PipeTbl[PipedId].AppId = UT_SB_ResourceID_Modify(RealOwner, 1);
    ASSERT_EQ(CFE_SB_DeletePipe(PipedId), CFE_SB_BAD_ARGUMENT);

    EVTCNT(2);

    EVTSENT(CFE_SB_DEL_PIPE_ERR2_EID);

    /* Restore owner id and delete pipe since test is complete */
    CFE_SB.PipeTbl[PipedId].AppId = RealOwner;
    TEARDOWN(CFE_SB_DeletePipe(PipedId));

} /* end Test_DeletePipe_InvalidPipeId */

/*
** Test successful pipe delete with app ID
*/
void Test_DeletePipe_WithAppid(void)
{
    CFE_SB_PipeId_t PipedId;
    uint32			AppId = 0;
    uint16          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipedId, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID1, PipedId));
    SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID2, PipedId));
    SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID3, PipedId));
    SETUP(CFE_SB_Subscribe(SB_UT_CMD_MID4, PipedId));

    ASSERT(CFE_SB_DeletePipeWithAppId(PipedId, AppId));

    EVTCNT(14);

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
    ASSERT_EQ(CFE_SB_GetPipeName(NULL, OS_MAX_API_NAME, 0), CFE_SB_BAD_ARGUMENT);

    EVTSENT(CFE_SB_GETPIPENAME_NULL_PTR_EID);

} /* end Test_GetPipeName_NullPtr */

/*
** Call to GetPipeName with an invalid pipe id.
*/
void Test_GetPipeName_InvalidId(void)
{
    char PipeName[OS_MAX_API_NAME];

    UT_SetDeferredRetcode(UT_KEY(OS_QueueGetInfo), 1, OS_ERROR);

    ASSERT_EQ(CFE_SB_GetPipeName(PipeName, OS_MAX_API_NAME, 0), CFE_SB_BAD_ARGUMENT);

    EVTSENT(CFE_SB_GETPIPENAME_ID_ERR_EID);

} /* end Test_GetPipeName_InvalidId */

/*
** Successful call to GetPipeName
*/
void Test_GetPipeName(void)
{
    char PipeName[OS_MAX_API_NAME];
    CFE_SB_PipeId_t PipeId = 0;

    OS_queue_prop_t queue_info = {
        "TestPipe1"
    };

    SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe1"));

    UT_SetDataBuffer(
        UT_KEY(OS_QueueGetInfo),
        &queue_info, sizeof(queue_info),
        false);

    ASSERT(CFE_SB_GetPipeName(PipeName, OS_MAX_API_NAME, PipeId));

    EVTSENT(CFE_SB_GETPIPENAME_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

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
    CFE_SB_PipeId_t PipeIDOut = 0;

	ASSERT_EQ(CFE_SB_GetPipeIdByName(&PipeIDOut, NULL), CFE_SB_BAD_ARGUMENT);

	EVTSENT(CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID);

    ASSERT_EQ(CFE_SB_GetPipeIdByName(NULL, "invalid"), CFE_SB_BAD_ARGUMENT);

    EVTSENT(CFE_SB_GETPIPEIDBYNAME_NULL_ERR_EID);

} /* end Test_GetPipeIdByName_NullPtrs */

/*
** Call to GetPipeId with an invalid pipe name.
*/
void Test_GetPipeIdByName_InvalidName(void)
{
    CFE_SB_PipeId_t PipeIdOut = 0;

	ASSERT_EQ(CFE_SB_GetPipeIdByName(&PipeIdOut, "invalid"), CFE_SB_BAD_ARGUMENT);

    EVTSENT(CFE_SB_GETPIPEIDBYNAME_NAME_ERR_EID);

} /* end Test_GetPipeIdByName_InvalidName */

/*
** Successful call to GetPipeIdByName
*/
void Test_GetPipeIdByName(void)
{
    CFE_SB_PipeId_t PipeId = 0, PipeIdOut = 0;

    SETUP(CFE_SB_CreatePipe(&PipeId, 4, "TestPipe1"));

    UT_SetDataBuffer(
        UT_KEY(OS_QueueGetIdByName),
        &(CFE_SB.PipeTbl[0].SysQueueId),
        sizeof(CFE_SB.PipeTbl[0].SysQueueId),
        false);

    ASSERT(CFE_SB_GetPipeIdByName(&PipeIdOut, "TestPipe1"));

    EVTSENT(CFE_SB_GETPIPEIDBYNAME_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_GetPipeIdByName */

/*
** Try setting pipe options on an invalid pipe ID
*/
void Test_SetPipeOpts_BadID(void)
{
	ASSERT_EQ(CFE_SB_SetPipeOpts(CFE_PLATFORM_SB_MAX_PIPES, 0), CFE_SB_BAD_ARGUMENT);

    EVTSENT(CFE_SB_SETPIPEOPTS_ID_ERR_EID);

} /* end Test_SetPipeOpts_BadID */

/*
** Try setting pipe options when not pipe owner
*/
void Test_SetPipeOpts_NotOwner(void)
{
    CFE_SB_PipeId_t PipeID = 0;
    uint8 PipeTblIdx = 0;
    uint32 OrigOwner = 0;

    SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    PipeTblIdx = CFE_SB_GetPipeIdx(PipeID);

    OrigOwner = CFE_SB.PipeTbl[PipeTblIdx].AppId;
    CFE_SB.PipeTbl[PipeTblIdx].AppId = 0xFFFFFFFF;
    ASSERT_EQ(CFE_SB_SetPipeOpts(PipeID, 0), CFE_SB_BAD_ARGUMENT);

    CFE_SB.PipeTbl[PipeTblIdx].AppId = OrigOwner;

    EVTSENT(CFE_SB_SETPIPEOPTS_OWNER_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeID));

} /* end Test_SetPipeOpts_NotOwner */

/*
** Set pipe options
*/
void Test_SetPipeOpts(void)
{
    CFE_SB_PipeId_t PipeID = 0;

    SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    ASSERT(CFE_SB_SetPipeOpts(PipeID, 0));

    EVTSENT(CFE_SB_SETPIPEOPTS_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeID));

} /* end Test_SetPipeOpts */

/*
** Try getting pipe options on an invalid pipe ID
*/
void Test_GetPipeOpts_BadID(void)
{
    uint8 Opts = 0;

    ASSERT_EQ(CFE_SB_GetPipeOpts(CFE_PLATFORM_SB_MAX_PIPES, &Opts), CFE_SB_BAD_ARGUMENT);

    EVTSENT(CFE_SB_GETPIPEOPTS_ID_ERR_EID);

} /* end Test_GetPipeOpts_BadID */

/*
** Try getting pipe options with a bad pointer
*/
void Test_GetPipeOpts_BadPtr(void)
{
    CFE_SB_PipeId_t PipeID = 0;

    SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    ASSERT_EQ(CFE_SB_GetPipeOpts(PipeID, NULL), CFE_SB_BAD_ARGUMENT);

    EVTSENT(CFE_SB_GETPIPEOPTS_PTR_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeID));

} /* end Test_GetPipeOpts_BadPtr */

/*
** Successful call to GetPipeOpts
*/
void Test_GetPipeOpts(void)
{
    CFE_SB_PipeId_t PipeID = 0;
    uint8 Opts = 0;

    SETUP(CFE_SB_CreatePipe(&PipeID, 4, "TestPipe1"));

    ASSERT(CFE_SB_GetPipeOpts(PipeID, &Opts));

    EVTSENT(CFE_SB_GETPIPEOPTS_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeID));

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
    SB_UT_ADD_SUBTEST(Test_Subscribe_FindGlobalMsgIdCnt);
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
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    CFE_SB_Qos_t    Quality = {0, 0};
    uint16          PipeDepth = 10;
    uint16          MsgLim = 8;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    ASSERT(CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim));

    EVTCNT(3);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);
    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_SubscribeEx */

/*
** Test message subscription response to an invalid pipe ID
*/
void Test_Subscribe_InvalidPipeId(void)
{
    CFE_SB_PipeId_t PipeId = 2;
    CFE_SB_MsgId_t  MsgId = SB_UT_ALTERNATE_INVALID_MID;

    ASSERT_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    EVTCNT(2);

    EVTSENT(CFE_SB_SUB_INV_PIPE_EID);

} /* end Test_Subscribe_InvalidPipeId */

/*
** Test message subscription response to an invalid message ID
*/
void Test_Subscribe_InvalidMsgId(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_ALTERNATE_INVALID_MID;
    uint16          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    ASSERT_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    EVTCNT(3);

    EVTSENT(CFE_SB_SUB_ARG_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_InvalidMsgId */

/*
** Test message subscription response to the maximum message limit
*/
void Test_Subscribe_MaxMsgLim(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    CFE_SB_Qos_t    Quality = {0, 0};
    uint16          PipeDepth = 10;
    uint16          MsgLim;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    MsgLim = 0xffff;

    ASSERT(CFE_SB_SubscribeEx(MsgId, PipeId, Quality, MsgLim));

    EVTCNT(3);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_MaxMsgLim */

/*
** Test message subscription response to a duplicate subscription
*/
void Test_Subscribe_DuplicateSubscription(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    uint16          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    ASSERT(CFE_SB_Subscribe(MsgId, PipeId));
    ASSERT(CFE_SB_Subscribe(MsgId, PipeId));

    EVTCNT(5);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_DuplicateSubscription */

/*
** Test API to locally subscribe to a message
*/
void Test_Subscribe_LocalSubscription(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    uint16          PipeDepth = 10;
    uint16          MsgLim = 4;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    ASSERT(CFE_SB_SubscribeLocal(MsgId, PipeId, MsgLim));

    EVTCNT(3);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

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
        snprintf(PipeName, OS_MAX_API_NAME, "TestPipe%ld", (long) i);
        SETUP(CFE_SB_CreatePipe(&PipeId[i], PipeDepth, &PipeName[0]));
    }

    /* Do subscriptions */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        if (i < CFE_PLATFORM_SB_MAX_DEST_PER_PKT)
        {
            SETUP(CFE_SB_Subscribe(MsgId, i));
        }
        else
        {
            ASSERT_EQ(CFE_SB_Subscribe(MsgId, i), CFE_SB_MAX_DESTS_MET);
        }
    }

    EVTCNT(3 * (CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1));

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    /* Delete pipes */
    for (i = 0; i < CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1; i++)
    {
        TEARDOWN(CFE_SB_DeletePipe(PipeId[i]));
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

    SETUP(CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0"));
    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));

    for (i = 0; i < CFE_PLATFORM_SB_MAX_MSG_IDS + 1; i++)
    {
        if (i < CFE_PLATFORM_SB_MAX_MSG_IDS)
        {
            SETUP(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId2));
        }
        else
        {
            ASSERT_EQ(CFE_SB_Subscribe(CFE_SB_ValueToMsgId(i), PipeId2), CFE_SB_MAX_MSGS_MET);
        }
    }

    EVTSENT(CFE_SB_MAX_MSGS_MET_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId0));
    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_Subscribe_MaxMsgIdCount */

/*
** Test obtaining the list of current message subscriptions
*/
void Test_Subscribe_SendPrevSubs(void)
{
    CFE_SB_PipeId_t PipeId0;
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_MsgId_t  MsgId0 = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t  MsgId1 = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t  MsgId2 = SB_UT_TLM_MID3;
    uint16          PipeDepth = 50;
    CFE_SB_SendPrevSubs_t SendPrevSubsMsg;
    CFE_SB_MsgId_t  MsgIdCmd;
    CFE_MSG_Size_t  Size;

    /* note that the message is not currently used or required,
     * but creating one and initializing it for completeness, in
     * case that changes in the future */
    memset(&SendPrevSubsMsg, 0, sizeof(SendPrevSubsMsg));
    SETUP(CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0"));
    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId0));
    SETUP(CFE_SB_Subscribe(MsgId1, PipeId0));
    SETUP(CFE_SB_Subscribe(MsgId2, PipeId0));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId2, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId2));

    /* Set the last list header pointer to NULL to get branch path coverage */
    CFE_SB.RoutingTbl[2].ListHeadPtr = NULL;

    /* For internal SendMsg call */
    MsgIdCmd = CFE_SB_ValueToMsgId(CFE_SB_ALLSUBS_TLM_MID);
    Size = sizeof(CFE_SB.PrevSubMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdCmd, sizeof(MsgIdCmd), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    ASSERT(CFE_SB_SendPrevSubsCmd(&SendPrevSubsMsg));

    EVTCNT(19);

    EVTSENT(CFE_SB_PART_SUB_PKT_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId0));
    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_Subscribe_SendPrevSubs */

/*
** Test function to get a count of the global message ids in use
*/
void Test_Subscribe_FindGlobalMsgIdCnt(void)
{
    CFE_SB_PipeId_t PipeId0;
    CFE_SB_PipeId_t PipeId1;
    CFE_SB_PipeId_t PipeId2;
    CFE_SB_MsgId_t  MsgId0 = SB_UT_TLM_MID1;
    CFE_SB_MsgId_t  MsgId1 = SB_UT_TLM_MID2;
    CFE_SB_MsgId_t  MsgId2 = SB_UT_TLM_MID3;
    uint16          PipeDepth = 50;
    uint16          MsgLim = 4;

    SETUP(CFE_SB_CreatePipe(&PipeId0, PipeDepth, "TestPipe0"));
    SETUP(CFE_SB_CreatePipe(&PipeId1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&PipeId2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId0));
    SETUP(CFE_SB_Subscribe(MsgId1, PipeId0));
    SETUP(CFE_SB_Subscribe(MsgId2, PipeId0));
    SETUP(CFE_SB_Subscribe(MsgId0, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId1, PipeId1));
    SETUP(CFE_SB_Subscribe(MsgId2, PipeId1));
    SETUP(CFE_SB_SubscribeLocal(MsgId0, PipeId2, MsgLim));

    /* Set the last list head pointer to NULL for branch path coverage */
    CFE_SB.RoutingTbl[2].ListHeadPtr = NULL;

    ASSERT_EQ(CFE_SB_FindGlobalMsgIdCnt(), 2); /* 2 unique msg ids; the third is set to skip */

    EVTCNT(17);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId0));
    TEARDOWN(CFE_SB_DeletePipe(PipeId1));
    TEARDOWN(CFE_SB_DeletePipe(PipeId2));

} /* end Test_Subscribe_FindGlobalMsgIdCnt */

/*
** Test message subscription response to nonexistent pipe
*/
void Test_Subscribe_PipeNonexistent(void)
{
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    CFE_SB_PipeId_t PipeId = 55;

    ASSERT_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    EVTCNT(2);

    EVTSENT(CFE_SB_SUB_INV_PIPE_EID);

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

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    /* Enable subscription reporting */
	CFE_SB_SetSubscriptionReporting(CFE_SB_ENABLE);

    /* For internal SendMsg call that will report subscription */
    MsgIdRpt = CFE_SB_ValueToMsgId(CFE_SB_ONESUB_TLM_MID);
    Size = sizeof(CFE_SB.SubRprtMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgIdRpt, sizeof(MsgIdRpt), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

	/* Subscribe to message: GLOBAL */
	SETUP(CFE_SB_Subscribe(MsgId, PipeId));

	/* Unsubscribe so that a local subscription can be tested */
	SETUP(CFE_SB_Unsubscribe(MsgId, PipeId));

	/* Subscribe to message: LOCAL */
	ASSERT(CFE_SB_SubscribeFull(MsgId, PipeId, Quality, CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT, CFE_SB_LOCAL));

	EVTCNT(8);

	EVTSENT(CFE_SB_SUBSCRIPTION_RPT_EID);

	/* Disable subscription reporting */
	CFE_SB_SetSubscriptionReporting(CFE_SB_DISABLE);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_SubscriptionReporting */

/*
** Test message subscription response to an invalid pipe owner
*/
void Test_Subscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    uint16          PipeDepth = 10;
    uint32          RealOwner;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    /* Change owner of pipe through memory corruption */
    RealOwner = CFE_SB.PipeTbl[PipeId].AppId;

    /* Choose a value that is sure not to be owner */
    CFE_SB.PipeTbl[PipeId].AppId = UT_SB_ResourceID_Modify(RealOwner, 1);
    CFE_SB_Subscribe(MsgId, PipeId);

    EVTCNT(3);

    EVTSENT(CFE_SB_SUB_INV_CALLER_EID);

    /* Restore owner id and delete pipe since test is complete */
    CFE_SB.PipeTbl[PipeId].AppId = RealOwner;
    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_Subscribe_InvalidPipeOwner */

/*
** Function for calling SB unsubscribe API test functions
*/
void Test_Unsubscribe_API(void)
{
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_Basic);
    SB_UT_ADD_SUBTEST(Test_Unsubscribe_Local);
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
    CFE_SB_MsgId_t  MsgId = SB_UT_INTERMEDIATE_VALID_MID;
    uint16          PipeDepth = 50;

    SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    ASSERT(CFE_SB_Unsubscribe(MsgId, TestPipe));

    EVTCNT(4);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    /* Check unsubscribe after unsubscribe produces event */
    UT_ClearEventHistory();
    ASSERT(CFE_SB_Unsubscribe(MsgId, TestPipe));
    EVTCNT(2);
    EVTSENT(CFE_SB_UNSUB_NO_SUBS_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe));

} /* end Test_Unsubscribe_Basic */

/*
** Test CFE internal API used to locally unsubscribe to a message (successful)
*/
void Test_Unsubscribe_Local(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;

    SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    ASSERT(CFE_SB_UnsubscribeLocal(SB_UT_LAST_VALID_MID, TestPipe));

    EVTCNT(5);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe));

} /* end Test_Unsubscribe_Local */

/*
** Test message unsubscription response to an invalid message ID
*/
void Test_Unsubscribe_InvalParam(void)
{
    CFE_SB_PipeId_t TestPipe;
    uint32          CallerId = 0xFFFFFFFF;
    uint16          PipeDepth = 50;
    CFE_SB_PipeId_t SavedPipeId;

    SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));

    /* Perform test using a bad message ID */
    ASSERT_EQ(CFE_SB_Unsubscribe(SB_UT_ALTERNATE_INVALID_MID, TestPipe), CFE_SB_BAD_ARGUMENT);

    /* Get the caller's Application ID */
    ASSERT(CFE_ES_GetAppID(&CallerId));

    /* Perform test using a bad scope value */
    ASSERT_EQ(CFE_SB_UnsubscribeFull(SB_UT_FIRST_VALID_MID, TestPipe, CFE_SB_LOCAL + 1, CallerId), CFE_SB_BAD_ARGUMENT);

    /* Perform test using an invalid pipe ID for branch path coverage.
     * This situation cannot happen in normal circumstances since the
     * bad pipe ID is caught by CFE_SB_GetPipeIdx() before it gets to
     * CFE_SB_ValidatePipeId()
     */
    SavedPipeId = CFE_SB.PipeTbl[0].PipeId;
    CFE_SB.PipeTbl[0].PipeId = CFE_PLATFORM_SB_MAX_PIPES;
    CFE_SB.PipeTbl[0].InUse = 1;
    ASSERT_EQ(CFE_SB_Unsubscribe(SB_UT_FIRST_VALID_MID, CFE_PLATFORM_SB_MAX_PIPES), CFE_SB_BAD_ARGUMENT);

    /* We must restore the old value so CFE_SB_DeletePipe() works */
    CFE_SB.PipeTbl[0].PipeId = SavedPipeId;

    EVTCNT(4);

    EVTSENT(CFE_SB_UNSUB_ARG_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe));

} /* end Test_Unsubscribe_InvalParam */

/*
** Test message unsubscription response to a message ID that is not subscribed
*/
void Test_Unsubscribe_NoMatch(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    CFE_SB_MsgRouteIdx_t Idx;
    uint16          PipeDepth = 50;

    SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    ASSERT(CFE_SB_Unsubscribe(SB_UT_TLM_MID1, TestPipe));

    /* Get index into routing table */
    Idx = CFE_SB_GetRoutingTblIdx(CFE_SB_ConvertMsgIdtoMsgKey(MsgId));
    CFE_SB.RoutingTbl[CFE_SB_RouteIdxToValue(Idx)].ListHeadPtr->PipeId = 1;
    CFE_SB.RoutingTbl[CFE_SB_RouteIdxToValue(Idx)].ListHeadPtr->Next = NULL;
    ASSERT(CFE_SB_Unsubscribe(MsgId, TestPipe));

    EVTCNT(7);

    EVTSENT(CFE_SB_UNSUB_NO_SUBS_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe));

} /* end Test_Unsubscribe_NoMatch */

/*
** Test message unsubscription response to an invalid pipe ID
*/
void Test_Unsubscribe_InvalidPipe(void)
{
    CFE_SB_PipeId_t TestPipe;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    uint16          PipeDepth = 50;

    SETUP(CFE_SB_CreatePipe(&TestPipe, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe));

    ASSERT_EQ(CFE_SB_Unsubscribe(MsgId, TestPipe + 1), CFE_SB_BAD_ARGUMENT);

    EVTCNT(4);

    EVTSENT(CFE_SB_UNSUB_INV_PIPE_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe));

} /* end Test_Unsubscribe_InvalidPipe */

/*
** Test message unsubscription response to an invalid pipe owner
*/
void Test_Unsubscribe_InvalidPipeOwner(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;
    uint32          RealOwner;
    uint16          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    /* Change owner of pipe through memory corruption */
    RealOwner = CFE_SB.PipeTbl[PipeId].AppId;

    /* Choose a value that is sure not be owner */
    CFE_SB.PipeTbl[PipeId].AppId = UT_SB_ResourceID_Modify(RealOwner, 1);
    ASSERT_EQ(CFE_SB_Unsubscribe(MsgId, PipeId), CFE_SB_BAD_ARGUMENT);

    EVTCNT(4);

    EVTSENT(CFE_SB_UNSUB_INV_CALLER_EID);

    CFE_SB.PipeTbl[PipeId].AppId = RealOwner;

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

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

    SETUP(CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_CreatePipe(&TestPipe3, PipeDepth, "TestPipe3"));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe1));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe2));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe3));

    ASSERT(CFE_SB_Unsubscribe(MsgId, TestPipe1));

    EVTCNT(10);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe1));
    TEARDOWN(CFE_SB_DeletePipe(TestPipe2));
    TEARDOWN(CFE_SB_DeletePipe(TestPipe3));

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

    SETUP(CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_CreatePipe(&TestPipe3, PipeDepth, "TestPipe3"));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe1));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe2));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe3));

    ASSERT(CFE_SB_Unsubscribe(MsgId, TestPipe2));

    EVTCNT(10);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe1));
    TEARDOWN(CFE_SB_DeletePipe(TestPipe2));
    TEARDOWN(CFE_SB_DeletePipe(TestPipe3));

} /* end Test_Unsubscribe_MiddleDestWithMany */

/*
** Test message unsubscription by verifying the message destination pointer no
** longer points to the pipe
*/
void Test_Unsubscribe_GetDestPtr(void)
{
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    CFE_SB_PipeId_t TestPipe1;
    CFE_SB_PipeId_t TestPipe2;
    uint16          PipeDepth = 50;

    SETUP(CFE_SB_CreatePipe(&TestPipe1, PipeDepth, "TestPipe1"));
    SETUP(CFE_SB_CreatePipe(&TestPipe2, PipeDepth, "TestPipe2"));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe1));
    SETUP(CFE_SB_Subscribe(MsgId, TestPipe2));
    SETUP(CFE_SB_Unsubscribe(MsgId, TestPipe2));

    ASSERT_TRUE(CFE_SB_GetDestPtr(CFE_SB_ConvertMsgIdtoMsgKey(MsgId), TestPipe2) == NULL);

    EVTCNT(7);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(TestPipe1));
    TEARDOWN(CFE_SB_DeletePipe(TestPipe2));

} /* end Test_Unsubscribe_GetDestPtr */

/*
** Function for calling SB send message API test functions
*/
void Test_SendMsg_API(void)
{
    SB_UT_ADD_SUBTEST(Test_SendMsg_NullPtr);
    SB_UT_ADD_SUBTEST(Test_SendMsg_NoSubscribers);
    SB_UT_ADD_SUBTEST(Test_SendMsg_MaxMsgSizePlusOne);
    SB_UT_ADD_SUBTEST(Test_SendMsg_BasicSend);
    SB_UT_ADD_SUBTEST(Test_SendMsg_SequenceCount);
    SB_UT_ADD_SUBTEST(Test_SendMsg_QueuePutError);
    SB_UT_ADD_SUBTEST(Test_SendMsg_PipeFull);
    SB_UT_ADD_SUBTEST(Test_SendMsg_MsgLimitExceeded);
    SB_UT_ADD_SUBTEST(Test_SendMsg_GetPoolBufErr);
    SB_UT_ADD_SUBTEST(Test_SendMsg_ZeroCopyGetPtr);
    SB_UT_ADD_SUBTEST(Test_SendMsg_ZeroCopySend);
    SB_UT_ADD_SUBTEST(Test_SendMsg_ZeroCopyPass);
    SB_UT_ADD_SUBTEST(Test_SendMsg_ZeroCopyReleasePtr);
    SB_UT_ADD_SUBTEST(Test_SendMsg_DisabledDestination);
    SB_UT_ADD_SUBTEST(Test_SendMsg_SendWithMetadata);
    SB_UT_ADD_SUBTEST(Test_SendMsg_MaxMsgSizePlusOne_ZeroCopy);
    SB_UT_ADD_SUBTEST(Test_SendMsg_NoSubscribers_ZeroCopy);
} /* end Test_SendMsg_API */

/*
** Test response to sending a null message on the software bus
*/
void Test_SendMsg_NullPtr(void)
{
    ASSERT_EQ(CFE_SB_SendMsg(NULL), CFE_SB_BAD_ARGUMENT);

    EVTCNT(1);

    EVTSENT(CFE_SB_SEND_BAD_ARG_EID);

} /* end Test_SendMsg_NullPtr */

/*
** Test response to sending a message which has no subscribers
*/
void Test_SendMsg_NoSubscribers(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(1);

    EVTSENT(CFE_SB_SEND_NO_SUBS_EID);

} /* end Test_SendMsg_NoSubscribers */

/*
** Test response to sending a message with the message size larger than allowed
*/
void Test_SendMsg_MaxMsgSizePlusOne(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_MSG_Size_t   Size = CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    ASSERT_EQ(CFE_SB_SendMsg(TlmPktPtr), CFE_SB_MSG_TOO_BIG);

    EVTCNT(1);

    EVTSENT(CFE_SB_MSG_TOO_BIG_EID);

} /* end Test_SendMsg_MaxMsgSizePlusOne */

/*
** Test successfully sending a message on the software bus
*/
void Test_SendMsg_BasicSend(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;


    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(3);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_BasicSend */

/* Sequence count hook */
static int32 UT_CheckSetSequenceCount(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    CFE_MSG_SequenceCount_t *SeqCntPtr = UserObj;

    *SeqCntPtr = UT_Hook_GetArgValueByName(Context, "SeqCnt", CFE_MSG_SequenceCount_t);

    return StubRetcode;
}

/*
** Test successful send/receive for packet sequence count
*/
void Test_SendMsg_SequenceCount(void)
{
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_MsgId_t          MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t        TlmPkt;
    CFE_SB_MsgPtr_t         TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_MSG_Size_t          Size = sizeof(TlmPkt);
    CFE_MSG_Type_t          Type = CFE_MSG_Type_Tlm;
    uint32                  PipeDepth = 10;
    CFE_MSG_SequenceCount_t SeqCnt;

    /* Set up hook for checking CFE_MSG_SetSequenceCount calls */
    UT_SetHookFunction(UT_KEY(CFE_MSG_SetSequenceCount), UT_CheckSetSequenceCount, &SeqCnt);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "SeqCntTestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);
    SETUP(CFE_SB_SendMsg(TlmPktPtr));
    ASSERT_EQ(UT_GetStubCount(UT_KEY(CFE_MSG_SetSequenceCount)), 1);
    ASSERT_EQ(SeqCnt, 1);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);
    ASSERT(CFE_SB_PassMsg(TlmPktPtr));

    /* Assert sequence count wasn't set */
    ASSERT_EQ(UT_GetStubCount(UT_KEY(CFE_MSG_SetSequenceCount)), 1);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));
    ASSERT_EQ(SeqCnt, 2);
    ASSERT_EQ(UT_GetStubCount(UT_KEY(CFE_MSG_SetSequenceCount)), 2);

    EVTCNT(3);
    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    SETUP(CFE_SB_Unsubscribe(MsgId, PipeId)); /* should have no subscribers now */

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);
    SETUP(CFE_SB_SendMsg(TlmPktPtr)); /* increment to 3 */
    ASSERT_EQ(UT_GetStubCount(UT_KEY(CFE_MSG_SetSequenceCount)), 3);

    SETUP(CFE_SB_Subscribe(MsgId, PipeId)); /* resubscribe so we can receive a msg */

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);
    SETUP(CFE_SB_SendMsg(TlmPktPtr)); /* increment to 4 */
    ASSERT_EQ(SeqCnt, 4);
    ASSERT_EQ(UT_GetStubCount(UT_KEY(CFE_MSG_SetSequenceCount)), 4);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_SequenceCount */

/*
** Test send message response to a socket queue 'put' error
*/
void Test_SendMsg_QueuePutError(void)
{
    CFE_SB_PipeId_t  PipeId4Error;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;

    SETUP(CFE_SB_CreatePipe(&PipeId4Error, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId4Error));
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(5);

    EVTSENT(CFE_SB_Q_WR_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId4Error));

} /* end Test_SendMsg_QueuePutError */

/*
** Test send message response when the socket queue is full
*/
void Test_SendMsg_PipeFull(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 1;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "PipeFullTestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* This send should pass */
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    /* Tell the QueuePut stub to return OS_QUEUE_FULL on its next call */
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_QUEUE_FULL);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* Pipe overflow causes SendMsg to return CFE_SUCCESS */
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(5);

    EVTSENT(CFE_SB_Q_FULL_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_PipeFull */

/*
** Test send message response to too many messages sent to the pipe
*/
void Test_SendMsg_MsgLimitExceeded(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 5;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "MsgLimTestPipe"));

    /* Set maximum allowed messages on the pipe at one time to 1 */
    CFE_SB_SubscribeEx(MsgId, PipeId, CFE_SB_Default_Qos, 1);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* First send should pass */
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* This send should produce a MsgId to Pipe Limit Exceeded message, but
     * return success
     */
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(5);

    EVTSENT(CFE_SB_MSGID_LIM_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_MsgLimitExceeded */

/*
** Test send message response to a buffer descriptor allocation failure
*/
void Test_SendMsg_GetPoolBufErr(void)
{
    CFE_SB_PipeId_t  PipeId;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    PipeDepth = 1;
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "GetPoolErrPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* Have GetPoolBuf stub return error on its next call (buf descriptor
     * allocation failed)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    ASSERT_EQ(CFE_SB_SendMsg(TlmPktPtr), CFE_SB_BUF_ALOC_ERR);

    EVTCNT(4);

    EVTSENT(CFE_SB_GET_BUF_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_GetPoolBufErr */

/*
** Test getting a pointer to a buffer for zero copy mode with buffer
** allocation failures
*/
void Test_SendMsg_ZeroCopyGetPtr(void)
{
    CFE_SB_ZeroCopyHandle_t ZeroCpyBufHndl;
    uint16                  MsgSize = 10;

    /* Have GetPoolBuf stub return error on its next call (buf descriptor
     * allocation failed)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    ASSERT_TRUE((cpuaddr) CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl) == (cpuaddr) NULL);

    /* Have GetPoolBuf stub return error on its second call (actual buffer
     * allocation failed)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 2, CFE_ES_ERR_MEM_BLOCK_SIZE);
    ASSERT_TRUE((cpuaddr) CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl) == (cpuaddr) NULL);

    /* Have GetPoolBuf stub return error on its second call (null buffer
     * returned and error returning the memory to the buffer)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 2, -1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, 0);
    ASSERT_TRUE((cpuaddr) CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl) == (cpuaddr) NULL);

    EVTCNT(0);

    /* Increase the peak memory and buffers in use above the expected values in
     * order to exercise branch paths
     */
    CFE_SB.StatTlmMsg.Payload.MemInUse = 0;
    CFE_SB.StatTlmMsg.Payload.PeakMemInUse = sizeof(CFE_SB_ZeroCopyD_t) + MsgSize +
                                              sizeof(CFE_SB_BufferD_t) + 1;
    CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse =
      CFE_SB.StatTlmMsg.Payload.SBBuffersInUse + 2;
    ASSERT_TRUE((cpuaddr) CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl) != (cpuaddr) NULL);

    ASSERT_EQ(CFE_SB.StatTlmMsg.Payload.PeakMemInUse, sizeof(CFE_SB_ZeroCopyD_t) + MsgSize +
        sizeof(CFE_SB_BufferD_t) + 1);
    ASSERT_EQ(CFE_SB.StatTlmMsg.Payload.MemInUse, sizeof(CFE_SB_ZeroCopyD_t) + MsgSize +
        sizeof(CFE_SB_BufferD_t));

    ASSERT_EQ(CFE_SB.StatTlmMsg.Payload.PeakSBBuffersInUse, CFE_SB.StatTlmMsg.Payload.SBBuffersInUse + 1);

    EVTCNT(0);

} /* end Test_SendMsg_ZeroCopyGetPtr */

/*
** Test successfully sending a message in zero copy mode (telemetry source
** sequence count increments)
*/
void Test_SendMsg_ZeroCopySend(void)
{
    CFE_SB_MsgPtr_t         PtrToMsg;
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_MsgId_t          MsgId = SB_UT_TLM_MID;
    CFE_SB_MsgPtr_t         ZeroCpyMsgPtr = NULL;
    uint32                  PipeDepth = 10;
    CFE_SB_ZeroCopyHandle_t ZeroCpyBufHndl = 0;
    CFE_MSG_SequenceCount_t SeqCnt;
    CFE_MSG_Size_t          Size = sizeof(SB_UT_Test_Tlm_t);
    CFE_MSG_Type_t          Type = CFE_MSG_Type_Tlm;

    /* Set up hook for checking CFE_MSG_SetSequenceCount calls */
    UT_SetHookFunction(UT_KEY(CFE_MSG_SetSequenceCount), UT_CheckSetSequenceCount, &SeqCnt);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "ZeroCpyTestPipe"));

    SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    ZeroCpyMsgPtr = CFE_SB_ZeroCopyGetPtr(sizeof(SB_UT_Test_Tlm_t),&ZeroCpyBufHndl);
    if (ZeroCpyMsgPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }

    /* Test response to a get pool information error */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBufInfo), 1, -1);
    ASSERT_EQ(CFE_SB_ZeroCopySend(ZeroCpyMsgPtr, ZeroCpyBufHndl), CFE_SB_BUFFER_INVALID);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* Test a successful zero copy send */
    ASSERT(CFE_SB_ZeroCopySend(ZeroCpyMsgPtr, ZeroCpyBufHndl));

    ASSERT(CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER));

    ASSERT_TRUE(PtrToMsg != NULL);

    ASSERT_EQ(SeqCnt, 1);

    EVTCNT(3);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_ZeroCopySend */

/*
** Test successfully sending a message in zero copy mode (telemetry source
** sequence count is unchanged)
*/
void Test_SendMsg_ZeroCopyPass(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t          MsgId = SB_UT_TLM_MID;
    CFE_SB_MsgPtr_t         ZeroCpyMsgPtr = NULL;
    uint32                  PipeDepth = 10;
    CFE_SB_ZeroCopyHandle_t ZeroCpyBufHndl = 0;
    CFE_MSG_SequenceCount_t SeqCnt = 22;
    CFE_MSG_Size_t          Size = sizeof(SB_UT_Test_Tlm_t);
    CFE_MSG_Type_t          Type = CFE_MSG_Type_Tlm;

    /* Set up hook for checking CFE_MSG_SetSequenceCount calls */
    UT_SetHookFunction(UT_KEY(CFE_MSG_SetSequenceCount), UT_CheckSetSequenceCount, &SeqCnt);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "ZeroCpyPassTestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    ZeroCpyMsgPtr = CFE_SB_ZeroCopyGetPtr(sizeof(SB_UT_Test_Tlm_t),
                                          &ZeroCpyBufHndl);

    if (ZeroCpyMsgPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }

    /* Test response to a get pool information error */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBufInfo), 1, -1);
    ASSERT_EQ(CFE_SB_ZeroCopyPass(ZeroCpyMsgPtr, ZeroCpyBufHndl), CFE_SB_BUFFER_INVALID);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* Test a successful zero copy pass */
    ASSERT(CFE_SB_ZeroCopyPass(ZeroCpyMsgPtr, ZeroCpyBufHndl));
    ASSERT(CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER));

    ASSERT_TRUE(PtrToMsg != NULL);
    ASSERT_EQ(SeqCnt, 22);

    EVTCNT(3);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_ZeroCopyPass */

/*
** Test releasing a pointer to a buffer for zero copy mode
*/
void Test_SendMsg_ZeroCopyReleasePtr(void)
{
    CFE_SB_MsgPtr_t         ZeroCpyMsgPtr1 = NULL;
    CFE_SB_MsgPtr_t         ZeroCpyMsgPtr2 = NULL;
    CFE_SB_MsgPtr_t         ZeroCpyMsgPtr3 = NULL;
    CFE_SB_ZeroCopyHandle_t ZeroCpyBufHndl1 = 0;
    CFE_SB_ZeroCopyHandle_t ZeroCpyBufHndl2 = 0;
    CFE_SB_ZeroCopyHandle_t ZeroCpyBufHndl3 = 0;
    uint16                  MsgSize = 10;

    ZeroCpyMsgPtr1 = CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl1);
    ZeroCpyMsgPtr2 = CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl2);
    ZeroCpyMsgPtr3 = CFE_SB_ZeroCopyGetPtr(MsgSize, &ZeroCpyBufHndl3);
    SETUP(CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr2, ZeroCpyBufHndl2));

    /* Test response to an invalid buffer */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBufInfo), 1, -1);
    ASSERT_EQ(CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr2, ZeroCpyBufHndl2), CFE_SB_BUFFER_INVALID);

    /* Test response to a null message pointer */
    ASSERT_EQ(CFE_SB_ZeroCopyReleasePtr(NULL, ZeroCpyBufHndl2), CFE_SB_BUFFER_INVALID);

    /* Test response to an invalid message pointer */
    ASSERT_EQ(CFE_SB_ZeroCopyReleasePtr((CFE_SB_Msg_t *) 0x1234,
                                       ZeroCpyBufHndl2), CFE_SB_BUFFER_INVALID);

    /* Test path when return the descriptor to the pool fails in
     * CFE_SB_ZeroCopyReleaseDesc
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    ASSERT(CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr2, ZeroCpyBufHndl2));

    /* Test path when return the buffer to the pool fails in
     * CFE_SB_ZeroCopyReleasePtr
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 2, -1);
    ASSERT(CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr2, ZeroCpyBufHndl2));

    /* Test successful release of the second buffer */
    ASSERT(CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr3, ZeroCpyBufHndl3));

    /* Test successful release of the third buffer */
    ASSERT(CFE_SB_ZeroCopyReleasePtr(ZeroCpyMsgPtr1, ZeroCpyBufHndl1));

    EVTCNT(0);

} /* end Test_SendMsg_ZeroCopyReleasePtr */

/*
** Test send message response with the destination disabled
*/
void Test_SendMsg_DisabledDestination(void)
{
    CFE_SB_PipeId_t       PipeId;
    CFE_SB_MsgId_t        MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t      TlmPkt;
    CFE_SB_MsgPtr_t       TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_SB_DestinationD_t *DestPtr;
    int32                 PipeDepth;
    CFE_MSG_Type_t        Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t        Size = sizeof(TlmPkt);

    PipeDepth = 2;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    DestPtr = CFE_SB_GetDestPtr(CFE_SB_ConvertMsgIdtoMsgKey(MsgId), PipeId);
    DestPtr->Active = CFE_SB_INACTIVE;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(3);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_DisabledDestination */

/*
** Test successfully sending a message with the metadata
*/
void Test_SendMsg_SendWithMetadata(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_TLM_MID;

    struct
    {
        /* To exercise the CFE_SB_SEND_ZEROCOPY feature,
         * the packet data must already be enclosed in a
         * buffer descriptor
         */
        CFE_SB_BufferD_t  BufDesc;
        SB_UT_Test_Tlm_t  TlmPkt;
    } TlmPktBufDesc;

    CFE_SB_MsgPtr_t TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPktBufDesc.TlmPkt;
    int32           PipeDepth;
    CFE_MSG_Type_t  Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t  Size = sizeof(TlmPktBufDesc.TlmPkt);

    memset(&TlmPktBufDesc, 0, sizeof(TlmPktBufDesc));
    PipeDepth = 2;
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    ASSERT(CFE_SB_SendMsgFull(TlmPktPtr, CFE_SB_DO_NOT_INCREMENT,
                                CFE_SB_SEND_ZEROCOPY));

    EVTCNT(3);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SendMsg_SendWithMetadata */

/*
** Test response to sending a message with the message size larger than allowed
** and ZeroCopy is set
*/
void Test_SendMsg_MaxMsgSizePlusOne_ZeroCopy(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1;

    TlmPktPtr = CFE_SB_ZeroCopyGetPtr(sizeof(SB_UT_Test_Tlm_t),
    		                          (CFE_SB_ZeroCopyHandle_t *) &TlmPkt);
    if (TlmPktPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }
    else
    {
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

        ASSERT_EQ(CFE_SB_SendMsgFull(TlmPktPtr, CFE_SB_INCREMENT_TLM,
                CFE_SB_SEND_ZEROCOPY), CFE_SB_MSG_TOO_BIG);

    }

    EVTCNT(1);

    EVTSENT(CFE_SB_MSG_TOO_BIG_EID);

} /* end Test_SendMsg_MaxMsgSizePlusOne_ZeroCopy */

/*
** Test response to sending a message which has no subscribers and ZeroCopy is
** set
*/
void Test_SendMsg_NoSubscribers_ZeroCopy(void)
{
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    TlmPktPtr = CFE_SB_ZeroCopyGetPtr(sizeof(SB_UT_Test_Tlm_t),
    		                          (CFE_SB_ZeroCopyHandle_t *) &TlmPkt);
    if (TlmPktPtr == NULL)
    {
        UtAssert_Failed("Unexpected NULL pointer returned from ZeroCopyGetPtr");
    }
    else
    {
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
        UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

        ASSERT(CFE_SB_SendMsgFull(TlmPktPtr, CFE_SB_INCREMENT_TLM,
                                    CFE_SB_SEND_ZEROCOPY));
    }

    EVTCNT(1);

    EVTSENT(CFE_SB_SEND_NO_SUBS_EID);

} /* end Test_SendMsg_NoSubscribers_ZeroCopy */

/*
** Function for calling SB receive message API test functions
*/
void Test_RcvMsg_API(void)
{
    SB_UT_ADD_SUBTEST(Test_RcvMsg_InvalidPipeId);
    SB_UT_ADD_SUBTEST(Test_RcvMsg_InvalidTimeout);
    SB_UT_ADD_SUBTEST(Test_RcvMsg_Poll);
    SB_UT_ADD_SUBTEST(Test_RcvMsg_Timeout);
    SB_UT_ADD_SUBTEST(Test_RcvMsg_PipeReadError);
    SB_UT_ADD_SUBTEST(Test_RcvMsg_PendForever);
    SB_UT_ADD_SUBTEST(Test_RcvMsg_InvalidBufferPtr);
} /* end Test_RcvMsg_API */

/*
** Test receiving a message from the software bus with an invalid pipe ID
*/
void Test_RcvMsg_InvalidPipeId(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t InvalidPipeId = 20;

    CFE_SB.PipeTbl[InvalidPipeId].InUse = CFE_SB_NOT_IN_USE;

    ASSERT_EQ(CFE_SB_RcvMsg(&PtrToMsg, InvalidPipeId, CFE_SB_POLL), CFE_SB_BAD_ARGUMENT);


    EVTCNT(1);

    EVTSENT(CFE_SB_BAD_PIPEID_EID);

} /* end Test_RcvMsg_InvalidPipeId */

/*
** Test receiving a message response to invalid timeout value (< -1)
*/
void Test_RcvMsg_InvalidTimeout(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;
    int32           TimeOut = -5;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe"));

    ASSERT_EQ(CFE_SB_RcvMsg(&PtrToMsg, PipeId, TimeOut), CFE_SB_BAD_ARGUMENT);

    EVTCNT(2);

    EVTSENT(CFE_SB_RCV_BAD_ARG_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_RcvMsg_InvalidTimeout */

/*
** Test receiving a message response when there is no message on the queue
*/
void Test_RcvMsg_Poll(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe"));

    ASSERT_EQ(CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_POLL), CFE_SB_NO_MESSAGE);

    EVTCNT(1);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_RcvMsg_Poll */

/*
** Test receiving a message response to a timeout
*/
void Test_RcvMsg_Timeout(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;
    int32           TimeOut = 200;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe"));
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, OS_QUEUE_TIMEOUT);

    ASSERT_EQ(CFE_SB_RcvMsg(&PtrToMsg, PipeId, TimeOut), CFE_SB_TIME_OUT);

    EVTCNT(1);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_RcvMsg_Timeout */

/*
** Test receiving a message response to a pipe read error
*/
void Test_RcvMsg_PipeReadError(void)
{
    CFE_SB_MsgPtr_t PtrToMsg;
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe"));
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGet), 1, OS_ERROR);
    ASSERT_EQ(CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_PIPE_RD_ERR);

    EVTCNT(3);

    EVTSENT(CFE_SB_Q_RD_ERR_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_RcvMsg_PipeReadError */

/*
** Test receiving a message response to a "pend forever" timeout
*/
void Test_RcvMsg_PendForever(void)
{
    CFE_SB_MsgPtr_t  PtrToMsg;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    CFE_SB_PipeD_t   *PipeDscPtr;
    uint32           PipeDepth = 10;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe"));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    SETUP(CFE_SB_SendMsg(TlmPktPtr));

    ASSERT(CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER));
  
    ASSERT_TRUE(PtrToMsg != NULL);

    EVTCNT(3);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    PipeDscPtr = CFE_SB_GetPipePtr(PipeId);
    PipeDscPtr->ToTrashBuff = PipeDscPtr->CurrentBuff;
    PipeDscPtr->CurrentBuff = NULL;
    
    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_RcvMsg_PendForever */

/*
** Test releasing zero copy buffers for all pipes owned by a given app ID
*/
void Test_CleanupApp_API(void)
{
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_ZeroCopyHandle_t ZeroCpyBufHndl = 0;
    uint16                  PipeDepth = 50;
    uint32     AppID;

    UT_SetAppID(1);
    CFE_ES_GetAppID(&AppID);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    CFE_SB_ZeroCopyGetPtr(PipeDepth, &ZeroCpyBufHndl);
    CFE_SB_ZeroCopyGetPtr(PipeDepth, &ZeroCpyBufHndl);

    /* Set second application ID to provide complete branch path coverage */
    CFE_SB.PipeTbl[1].InUse = CFE_SB_IN_USE;
    CFE_SB.PipeTbl[1].AppId = AppID;

    ASSERT_TRUE(CFE_SB.ZeroCopyTail != NULL);

    /* Attempt with a bad application ID first in order to get full branch path
     * coverage in CFE_SB_ZeroCopyReleaseAppId
     */
    CFE_SB_CleanUpApp(0);

    /* Attempt again with a valid application ID */
    CFE_SB_CleanUpApp(AppID);

    ASSERT_TRUE(CFE_SB.ZeroCopyTail == NULL);

    EVTCNT(3);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);

    EVTSENT(CFE_SB_PIPE_DELETED_EID);

} /* end Test_CleanupApp_API */

/*
** Test receiving a message response to invalid buffer pointer (null)
*/
void Test_RcvMsg_InvalidBufferPtr(void)
{
    CFE_SB_PipeId_t PipeId;
    uint32          PipeDepth = 10;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe"));

    ASSERT_EQ(CFE_SB_RcvMsg(NULL, PipeId, CFE_SB_PEND_FOREVER), CFE_SB_BAD_ARGUMENT);

    EVTCNT(2);

    EVTSENT(CFE_SB_RCV_BAD_ARG_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_RcvMsg_InvalidBufferPtr */

/*
** Test SB Utility APIs
*/
void Test_SB_Utils(void)
{
    SB_UT_ADD_SUBTEST(Test_CFE_SB_InitMsg);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_MsgHdrSize);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_GetUserData);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_SetGetMsgId);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_SetGetUserDataLength);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_SetGetTotalMsgLength);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_SetGetMsgTime);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_SetGetCmdCode);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_ChecksumUtils);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_ValidateMsgId);
} /* end Test_SB_Utils */

/*
**
*/
void Test_CFE_SB_InitMsg(void)
{
    CFE_MSG_Message_t msg;

    /* Pass through function */
    CFE_SB_InitMsg(&msg, CFE_SB_ValueToMsgId(0), 0, false);

} /* end Test_CFE_SB_InitMsg */

/*
** Test getting the size of a command/telemetry message header
*/
void Test_CFE_SB_MsgHdrSize(void)
{
    CFE_SB_Msg_t           msg;
    bool                   hassec;
    CFE_MSG_Type_t         type;

    /* No secondary */
    hassec = false;
    type = CFE_MSG_Type_Invalid;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    ASSERT_EQ(CFE_SB_MsgHdrSize(&msg), sizeof(CCSDS_SpacePacket_t));

    /* Has secondary, tlm type */
    hassec = true;
    type = CFE_MSG_Type_Tlm;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    ASSERT_EQ(CFE_SB_MsgHdrSize(&msg), sizeof(CFE_SB_TlmHdr_t));

    /* Has secondary, cmd type */
    type = CFE_MSG_Type_Cmd;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    ASSERT_EQ(CFE_SB_MsgHdrSize(&msg), sizeof(CFE_SB_CmdHdr_t));

    /* Has secondary, invalid type */
    type = CFE_MSG_Type_Invalid;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    ASSERT_EQ(CFE_SB_MsgHdrSize(&msg), 0);

} /* end Test_CFE_SB_MsgHdrSize */

/*
** Test getting a pointer to the user data portion of a message
*/
void Test_CFE_SB_GetUserData(void)
{
    CFE_SB_Msg_t           msg;
    uint8                  *ExpAdrReturned;
    bool                   hassec;
    CFE_MSG_Type_t         type = CFE_MSG_Type_Invalid;

    /* No secondary */
    hassec = false;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);

    /* Expected return */
    ExpAdrReturned = (uint8 *)&msg + sizeof(CCSDS_SpacePacket_t);

    ASSERT_TRUE(CFE_SB_GetUserData(&msg) == ExpAdrReturned);

} /* end Test_CFE_SB_GetUserData */

/*
** Test setting and getting the message ID of a message
*/
void Test_CFE_SB_SetGetMsgId(void)
{
    CFE_MSG_Message_t msg;
    CFE_SB_MsgId_t    msgid = 5;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &msgid, sizeof(msgid), false);

    /* Pass through functions */
    ASSERT_EQ(CFE_SB_GetMsgId(&msg), msgid);
    CFE_SB_SetMsgId(&msg, CFE_SB_ValueToMsgId(0));

} /* end Test_CFE_SB_SetGetMsgId */

/*
** Test setting and getting the user data length of a message
*/
void Test_CFE_SB_SetGetUserDataLength(void)
{
    CFE_MSG_Message_t msg;
    CFE_MSG_Size_t    size = 10;
    CFE_MSG_Type_t    type = CFE_MSG_Type_Invalid;
    bool              hassec = false;

    /* Pass through functions */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);
    ASSERT_EQ(CFE_SB_GetUserDataLength(&msg), size - sizeof(CCSDS_SpacePacket_t));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    CFE_SB_SetUserDataLength(&msg, 0);

} /* end Util_CFE_SB_SetGetUserDataLength */

/*
** Test setting and getting the total message size
*/
void Test_CFE_SB_SetGetTotalMsgLength(void)
{
    CFE_MSG_Message_t msg;
    CFE_MSG_Size_t    size = 6;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &size, sizeof(size), false);

    /* Pass through functions */
    ASSERT_EQ(CFE_SB_GetTotalMsgLength(&msg), size);
    CFE_SB_SetTotalMsgLength(&msg, 0);

} /* end Test_CFE_SB_SetGetTotalMsgLength */

void Test_CFE_SB_SetGetMsgTime(void)
{
    CFE_MSG_Message_t msg;
    CFE_TIME_SysTime_t time = {0xb, 0xa0000};
    CFE_TIME_SysTime_t actual;

    /* Set up buffer */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgTime), &time, sizeof(time), false);

    /* Pass through functions */
    actual = CFE_SB_GetMsgTime(&msg);
    ASSERT_TRUE(actual.Seconds == time.Seconds);
    ASSERT_TRUE(actual.Subseconds == time.Subseconds);
    CFE_SB_SetMsgTime(&msg, time);
    CFE_SB_TimeStampMsg(&msg);

} /* end Test_CFE_SB_SetGetMsgTime */

void Test_CFE_SB_SetGetCmdCode(void)
{
    CFE_MSG_Message_t msg;
    CFE_MSG_FcnCode_t fcncode = 7;

    /* Set up buffer */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &fcncode, sizeof(fcncode), false);

    /* Pass through functions */
    ASSERT_EQ(CFE_SB_GetCmdCode(&msg), fcncode);
    CFE_SB_SetCmdCode(&msg, 0);

} /* end Test_CFE_SB_SetGetCmdCode */

/*
** Test checksum utils
*/
void Test_CFE_SB_ChecksumUtils(void)
{
    CFE_SB_CmdHdr_t        SBCmd;
    CFE_SB_Msg_t          *SBCmdPtr = (CFE_SB_Msg_t *) &SBCmd;
    bool                   hassec;
    CFE_MSG_Type_t         type;
    bool                   validcksum;

    /* Has secondary, tlm type */
    hassec = true;
    type = CFE_MSG_Type_Tlm;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    ASSERT_EQ(CFE_SB_GetChecksum(SBCmdPtr), 0);

    /* No secondary, cmd type */
    hassec = false;
    type = CFE_MSG_Type_Cmd;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    ASSERT_EQ(CFE_SB_GetChecksum(SBCmdPtr), 0);

    /* Valid, 0 */
    hassec = true;
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    memset(&SBCmd, 1, sizeof(SBCmd));
    ASSERT_EQ(CFE_SB_GetChecksum(SBCmdPtr), 1);

    /* Valid, 0xFF */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetHasSecondaryHeader), &hassec, sizeof(hassec), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetType), &type, sizeof(type), false);
    memset(&SBCmd, 0xFF, sizeof(SBCmd));
    ASSERT_EQ(CFE_SB_GetChecksum(SBCmdPtr), 0xFF);

    /* Execute passthrough functions */
    validcksum = true;
    CFE_SB_GenerateChecksum(SBCmdPtr);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_ValidateChecksum), &validcksum, sizeof(validcksum), false);
    ASSERT_EQ(CFE_SB_ValidateChecksum(SBCmdPtr), validcksum);

} /* end Test_CFE_SB_ChecksumUtils */

/*
** Test validating a msg id
*/
void Test_CFE_SB_ValidateMsgId(void)
{
    CFE_SB_MsgId_t        MsgId;

    /* Validate Msg Id */
    MsgId = SB_UT_LAST_VALID_MID;
    ASSERT_EQ(CFE_SB_ValidateMsgId(MsgId), CFE_SUCCESS);

    /* Test for invalid msg id */
    MsgId = SB_UT_ALTERNATE_INVALID_MID;
    ASSERT_EQ(CFE_SB_ValidateMsgId(MsgId), CFE_SB_FAILED);

}

/*
** Function for calling SB special test cases functions
*/
void Test_SB_SpecialCases(void)
{
    SB_UT_ADD_SUBTEST(Test_OS_MutSem_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_ReqToSendEvent_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_PutDestBlk_ErrLogic);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_GetPipeIdx);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_Buffers);
    SB_UT_ADD_SUBTEST(Test_CFE_SB_BadPipeInfo);
    SB_UT_ADD_SUBTEST(Test_SB_SendMsgPaths_Nominal);
    SB_UT_ADD_SUBTEST(Test_SB_SendMsgPaths_LimitErr);
    SB_UT_ADD_SUBTEST(Test_SB_SendMsgPaths_FullErr);
    SB_UT_ADD_SUBTEST(Test_SB_SendMsgPaths_WriteErr);
    SB_UT_ADD_SUBTEST(Test_SB_SendMsgPaths_IgnoreOpt);
    SB_UT_ADD_SUBTEST(Test_RcvMsg_UnsubResubPath);
    SB_UT_ADD_SUBTEST(Test_MessageString);
    SB_UT_ADD_SUBTEST(Test_SB_IdxPushPop);
} /* end Test_SB_SpecialCases */

/*
** Test msg key idx push pop
*/
void Test_SB_IdxPushPop()
{
    int32           i;
    CFE_SB_MsgRouteIdx_t Idx;

    CFE_SB_InitIdxStack();

    for (i = 0; i < CFE_PLATFORM_SB_MAX_MSG_IDS; i++)
    {
        /* Subscribe to maximum number of messages */
        Idx = CFE_SB_RouteIdxPop_Unsync();
        ASSERT_EQ(CFE_SB_RouteIdxToValue(Idx), i);
    }


    Idx = CFE_SB_RouteIdxPop_Unsync();
    ASSERT_EQ(CFE_SB_RouteIdxToValue(Idx), CFE_SB_RouteIdxToValue(CFE_SB_INVALID_ROUTE_IDX));

    for (i = 0; i < CFE_PLATFORM_SB_MAX_MSG_IDS; i++)
    {
        /* Un-subscribe from all messages */
        CFE_SB_RouteIdxPush_Unsync(CFE_SB_ValueToRouteIdx(i));
    }

    CFE_SB_RouteIdxPush_Unsync(CFE_SB_ValueToRouteIdx(i));


} /* end Test_SB_IdxPushPop */

/*
** Test pipe creation with semaphore take and give failures
*/
void Test_OS_MutSem_ErrLogic(void)
{
    CFE_SB_PipeId_t PipeId;
    CFE_SB_MsgId_t  MsgId = SB_UT_CMD_MID;
    uint16          PipeDepth = 50;

    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_SEM_FAILURE);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 2, OS_SEM_FAILURE);
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    ASSERT(CFE_SB_Subscribe(MsgId, PipeId));

    EVTCNT(3);

    EVTSENT(CFE_SB_PIPE_ADDED_EID);
    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_OS_MutSemTake_ErrLogic */

/*
** Test successful recursive event prevention
*/
void Test_ReqToSendEvent_ErrLogic(void)
{
    uint32 TaskId;
    uint32 Bit = 5;

    /* Clear task bits, then call function, which should set the bit for
     * the specified task
     */
    CFE_ES_GetTaskID(&TaskId);
    CFE_SB.StopRecurseFlags[0] = 0x0000;
    ASSERT_EQ(CFE_SB_RequestToSendEvent(TaskId, Bit), CFE_SB_GRANTED);

    /* Call the function a second time; the result should indicate that the
     * bit is already set
     */
    ASSERT_EQ(CFE_SB_RequestToSendEvent(TaskId, Bit), CFE_SB_DENIED);

    EVTCNT(0);

} /* end Test_ReqToSendEvent_ErrLogic */

/*
** Test getting a destination descriptor to the SB memory pool using a null
** destination pointer
*/
void Test_PutDestBlk_ErrLogic(void)
{
    ASSERT_EQ(CFE_SB_PutDestinationBlk(NULL), CFE_SB_BAD_ARGUMENT);

    EVTCNT(0);

} /* end Test_PutDestBlk_ErrLogic */

/*
** Test internal function to get the pipe table index for the given pipe ID
*/
void Test_CFE_SB_GetPipeIdx(void)
{
    CFE_SB.PipeTbl[0].PipeId = 0;
    CFE_SB.PipeTbl[0].InUse = CFE_SB_NOT_IN_USE;
    ASSERT_EQ(CFE_SB_GetPipeIdx(0), CFE_SB_INVALID_PIPE);

    EVTCNT(0);

} /* end Test_CFE_SB_GetPipeIdx */

/*
** Test functions that involve a buffer in the SB buffer pool
*/
void Test_CFE_SB_Buffers(void)
{
    int32 ExpRtn;

    CFE_SB_BufferD_t *bd;

    CFE_SB.StatTlmMsg.Payload.MemInUse = 0;
    CFE_SB.StatTlmMsg.Payload.PeakMemInUse = sizeof(CFE_SB_BufferD_t) * 4;
    bd = CFE_SB_GetBufferFromPool(SB_UT_FIRST_VALID_MID, 0);

    ASSERT_EQ(CFE_SB.StatTlmMsg.Payload.PeakMemInUse, sizeof(CFE_SB_BufferD_t) * 4);

    EVTCNT(0);

    ExpRtn = CFE_SB.StatTlmMsg.Payload.SBBuffersInUse;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    CFE_SB_ReturnBufferToPool(bd);
    ASSERT_EQ(CFE_SB.StatTlmMsg.Payload.SBBuffersInUse, ExpRtn);

    EVTCNT(0);

    bd->UseCount = 0;
    CFE_SB_DecrBufUseCnt(bd);
    ASSERT_EQ(bd->UseCount, 0);

    EVTCNT(0);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, -1);
    CFE_SB.StatTlmMsg.Payload.MemInUse = 0;
    CFE_SB_PutDestinationBlk((CFE_SB_DestinationD_t *) bd);

    ASSERT_EQ(CFE_SB.StatTlmMsg.Payload.MemInUse, 0);

    EVTCNT(0);

} /* end Test_CFE_SB_Buffers */

/*
** Test internal function to get the pipe table index for the given pipe ID
*/
void Test_CFE_SB_BadPipeInfo(void)
{
    CFE_SB_PipeId_t PipeId;
    uint16          PipeDepth = 10;
    CFE_SB_Qos_t    CFE_SB_Default_Qos;
    uint32          AppID;

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe1"));

    /* Set the pipe ID to an erroneous value and attempt to delete the pipe */
    CFE_SB.PipeTbl[0].PipeId = 1;
    CFE_SB.PipeTbl[0].InUse = 1;
    CFE_ES_GetAppID(&AppID);
    ASSERT_EQ(CFE_SB_DeletePipeFull(0, AppID), CFE_SB_BAD_ARGUMENT);

    EVTCNT(2);

    /* Reset the pipe ID and delete the pipe */
    CFE_SB.PipeTbl[0].PipeId = 0;

    ASSERT_EQ(CFE_SB_SubscribeFull(SB_UT_FIRST_VALID_MID ,0, CFE_SB_Default_Qos,
                                  CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT, 2), CFE_SB_BAD_ARGUMENT);

    EVTCNT(4);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));


} /* end Test_CFE_SB_BadPipeInfo */
/*
** Test send housekeeping information command
*/

void Test_SB_SendMsgPaths_Nominal(void)
{
    CFE_SB_CmdHdr_t  NoParamCmd;
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Size_t   Size;
    CFE_MSG_Type_t   Type;

    /* For internal SendMsg call */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID);
    Size = sizeof(CFE_SB.HKTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For HK command processing */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    CFE_SB.CmdPipePktPtr = (CFE_SB_MsgPtr_t) &NoParamCmd;
    CFE_SB.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_SEND_NO_SUBS_EID_BIT);
    CFE_SB_ProcessCmdPipePkt();

    ASSERT_TRUE(!UT_EventIsInHistory(CFE_SB_SEND_NO_SUBS_EID));

    CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter = 0;
    CFE_SB.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_GET_BUF_ERR_EID_BIT);

    /* For internal SendMsg call */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_HK_TLM_MID);
    Size = sizeof(CFE_SB.HKTlmMsg);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);

    /* For HK command processing */
    MsgId = CFE_SB_ValueToMsgId(CFE_SB_SEND_HK_MID);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);

    CFE_SB.MsgMap[CFE_SB_MsgKeyToValue(CFE_SB_ConvertMsgIdtoMsgKey(CFE_SB_HK_TLM_MID))] = CFE_SB_INVALID_ROUTE_IDX;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    CFE_SB_ProcessCmdPipePkt();
    ASSERT_EQ(CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter, 0);

    ASSERT_TRUE(!UT_EventIsInHistory(CFE_SB_GET_BUF_ERR_EID));

    EVTCNT(0);

    CFE_SB.StopRecurseFlags[1] = 0;

    /* Create a message ID with the command bit set and disable reporting */
    MsgId = SB_UT_CMD_MID;
    Size = sizeof(TlmPkt);
    Type = CFE_MSG_Type_Cmd;
    CFE_SB.SenderReporting = 0;
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));

    /* Will fail because of deferred CFE_ES_GetPoolBuf failure return */
    ASSERT_EQ(CFE_SB_Subscribe(MsgId, PipeId), CFE_SB_BUF_ALOC_ERR);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(3);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));
} /* end Test_SB_SendMsgPaths */

void Test_SB_SendMsgPaths_LimitErr(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    /* Test inhibiting sending a "message ID limit error" message */
    MsgId = SB_UT_TLM_MID;
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "MsgLimTestPipe"));

    /* Set maximum allowed messages on the pipe at one time to 1 */
    SETUP(CFE_SB_SubscribeEx(MsgId, PipeId, CFE_SB_Default_Qos, 1));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* First send should pass */
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    CFE_SB.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_MSGID_LIM_ERR_EID_BIT);
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));
    CFE_SB.StopRecurseFlags[1] = 0;

    ASSERT_TRUE(!UT_EventIsInHistory(CFE_SB_MSGID_LIM_ERR_EID));

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_SendMsgPaths */


void Test_SB_SendMsgPaths_FullErr(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    /* Test inhibiting sending a "pipe full" message */
    MsgId = SB_UT_TLM_MID;
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "PipeFullTestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* This send should pass */
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* Tell the QueuePut stub to return OS_QUEUE_FULL on its next call */
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_QUEUE_FULL);
    CFE_SB.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_Q_FULL_ERR_EID_BIT);
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));
    CFE_SB.StopRecurseFlags[1] = 0;

    ASSERT_TRUE(!UT_EventIsInHistory(CFE_SB_Q_FULL_ERR_EID_BIT));

    EVTCNT(3);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));
} /* end Test_SB_SendMsgPaths */

void Test_SB_SendMsgPaths_WriteErr(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    /* Test inhibiting sending a "pipe write error" message */
    MsgId = SB_UT_TLM_MID;
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "TestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDeferredRetcode(UT_KEY(OS_QueuePut), 1, OS_ERROR);

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    CFE_SB.StopRecurseFlags[1] |= CFE_BIT(CFE_SB_Q_WR_ERR_EID_BIT);
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));
    CFE_SB.StopRecurseFlags[1] = 0;

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    EVTCNT(3);

    ASSERT_TRUE(!UT_EventIsInHistory(CFE_SB_Q_WR_ERR_EID));

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_SendMsgPaths */

void Test_SB_SendMsgPaths_IgnoreOpt(void)
{
    CFE_SB_MsgId_t   MsgId;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    int32            PipeDepth = 2;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    /* Setup Test skipping sending to a pipe when the pipe option is set to ignore */
    MsgId = SB_UT_TLM_MID;
    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "SkipPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    SETUP(CFE_SB_SetPipeOpts(PipeId, CFE_SB_PIPEOPTS_IGNOREMINE));

    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);

    /* Test skipping this pipe and the send should pass */
    ASSERT(CFE_SB_SendMsg(TlmPktPtr));

    TEARDOWN(CFE_SB_SetPipeOpts(PipeId, 0));
    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_SB_SendMsgPaths */


/*
** Test receiving a message response to a unsubscribing to message, then
** resubscribing to it while it's in the pipe
*/
void Test_RcvMsg_UnsubResubPath(void)
{
    CFE_SB_MsgPtr_t  PtrToMsg;
    CFE_SB_MsgId_t   MsgId = SB_UT_TLM_MID;
    CFE_SB_PipeId_t  PipeId;
    SB_UT_Test_Tlm_t TlmPkt;
    CFE_SB_MsgPtr_t  TlmPktPtr = (CFE_SB_MsgPtr_t) &TlmPkt;
    uint32           PipeDepth = 10;
    CFE_MSG_Type_t   Type = CFE_MSG_Type_Tlm;
    CFE_MSG_Size_t   Size = sizeof(TlmPkt);

    SETUP(CFE_SB_CreatePipe(&PipeId, PipeDepth, "RcvMsgTestPipe"));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetSize), &Size, sizeof(Size), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetTypeFromMsgId), &Type, sizeof(Type), false);
    SETUP(CFE_SB_SendMsg(TlmPktPtr));

    SETUP(CFE_SB_Unsubscribe(MsgId, PipeId));
    SETUP(CFE_SB_Subscribe(MsgId, PipeId));
    ASSERT(CFE_SB_RcvMsg(&PtrToMsg, PipeId, CFE_SB_PEND_FOREVER));

    ASSERT_TRUE(PtrToMsg != NULL);

    EVTCNT(6);

    EVTSENT(CFE_SB_SUBSCRIPTION_RCVD_EID);

    TEARDOWN(CFE_SB_DeletePipe(PipeId));

} /* end Test_RcvMsg_UnsubResubPath */

/*
** Test the paths through the MessageStringSet and MessageStringGet functions
*/
void Test_MessageString(void)
{
	const char *SrcString = "abcdefg";
	char DestString[20];
	char *DestStringPtr = DestString;
	const char *DefString = "default";

    /* Test setting string where the destination size > source string size */
	CFE_SB_MessageStringSet(DestStringPtr, SrcString, sizeof(DestString),
			                strlen(SrcString));
    UT_Report(__FILE__, __LINE__,
              strcmp(DestString, SrcString) == 0, "CFE_SB_MessageStringSet",
              "Destination size > source string size");

	/* Test setting string where the source string is empty */
	CFE_SB_MessageStringSet(DestStringPtr, "", sizeof(DestString),
			                strlen(SrcString));
    UT_Report(__FILE__, __LINE__,
    		  strcmp(DestString, SrcString) != 0, "CFE_SB_MessageStringSet",
              "Empty source string");

	/* Test setting string where the destination size < source string size */
	CFE_SB_MessageStringSet(DestStringPtr, SrcString, strlen(SrcString) - 1,
			                strlen(SrcString));
    UT_Report(__FILE__, __LINE__,
    		  strncmp(DestString, SrcString, strlen(SrcString) - 1) == 0,
    		  "CFE_SB_MessageStringSet",
              "Destination size < source string size");

	/* Test getting string where the destination size > source string size */
	CFE_SB_MessageStringGet(DestStringPtr, SrcString, DefString,
			                sizeof(DestString), strlen(SrcString));
    UT_Report(__FILE__, __LINE__,
    		  strcmp(DestString, SrcString) == 0, "CFE_SB_MessageStringGet",
              "Destination size > source string size");

	/* Test getting string where the destination size is zero */
    DestString[0] = '\0';
	CFE_SB_MessageStringGet(DestStringPtr, SrcString, DefString, 0,
			                strlen(SrcString));
    UT_Report(__FILE__, __LINE__,
            strcmp(DestString, SrcString) != 0, "CFE_SB_MessageStringGet",
              "Destination size = 0");

	/* Test getting string where the default string is NULL */
	CFE_SB_MessageStringGet(DestStringPtr, SrcString, NULL,
			                sizeof(DestString), 0);
    UT_Report(__FILE__, __LINE__,
    		  strcmp(DefString, SrcString) != 0, "CFE_SB_MessageStringGet",
              "Default string = NULL");

	/* Test getting string where the source string size is zero */
	CFE_SB_MessageStringGet(DestStringPtr, SrcString, DefString,
			                sizeof(DestString), 0);
    UT_Report(__FILE__, __LINE__,
    		strcmp(DestString, SrcString) != 0, "CFE_SB_MessageStringGet",
              "Source string size = 0");

	/* Test getting string where the destination size < source string size */
    DestString[0] = '\0';
	CFE_SB_MessageStringGet(DestStringPtr, SrcString, DefString,
			                strlen(SrcString) - 1, strlen(SrcString));
    UT_Report(__FILE__, __LINE__,
    		  strncmp(DestString, SrcString, strlen(DestString)) == 0,
    		  "CFE_SB_MessageStringGet",
              "Destination size < source string size");
} /* end Test_MessageString */
