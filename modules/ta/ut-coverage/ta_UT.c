/************************************************************************
 * cFE Task Affinity (TA) Unit Test
 ************************************************************************/

#include "ta_UT.h"
#include "ta_ut_helpers.h"

void TA_ResetUnitTest(void)
{
    UT_InitData_TA();
    memset(&CFE_TA_Global, 0, sizeof(CFE_TA_Global));
    memset(CFE_TA_CpuTaskAffinity, 0, sizeof(CFE_TA_CpuTaskAffinity));
}

void UtTest_Setup(void)
{
    UtPrintf("cFE TA Unit Test Output File\n\n");
    Test_Init();
    Test_Main();
    Test_Cmds();
    Test_QueryAll();
    Test_StartupAffinity();
    Test_Events();
}

void Test_Init(void)
{
    TA_UT_ADD_SUBTEST(Test_TA_EarlyInit_NullTable);
    TA_UT_ADD_SUBTEST(Test_TA_EarlyInit_WithTable);
    TA_UT_ADD_SUBTEST(Test_TA_EarlyInit_MaxTasks);
    TA_UT_ADD_SUBTEST(Test_TA_EarlyInit_Error);
    TA_UT_ADD_SUBTEST(Test_TA_TaskInit);
    TA_UT_ADD_SUBTEST(Test_TA_TaskInit_Errors);
    TA_UT_ADD_SUBTEST(Test_TA_CleanUpApp);
    TA_UT_ADD_SUBTEST(Test_TA_EarlyInit_HandlerError);
}

void Test_Main(void)
{
    TA_UT_ADD_SUBTEST(Test_TA_Main_InitFail);
    TA_UT_ADD_SUBTEST(Test_TA_Main_RcvError);
    TA_UT_ADD_SUBTEST(Test_TA_Main_Nominal);
}

void Test_Cmds(void)
{
    TA_UT_ADD_SUBTEST(Test_TA_TaskPipe_InvalidMsgId);
    TA_UT_ADD_SUBTEST(Test_TA_TaskPipe_HK);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_BadCC);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_BadLength_Noop);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_BadLength_Reset);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_BadLength_QueryAll);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_BadLength_SetAffinity);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_BadLength_GetAffinity);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_QueryAll);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_SetAffinity);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_GetAffinity);
    TA_UT_ADD_SUBTEST(Test_TA_NoopCmd);
    TA_UT_ADD_SUBTEST(Test_TA_ResetCountersCmd);
    TA_UT_ADD_SUBTEST(Test_TA_Dispatch_CommandError);
    TA_UT_ADD_SUBTEST(Test_TA_SetTaskAffinityCmd_Nominal);
    TA_UT_ADD_SUBTEST(Test_TA_SetTaskAffinityCmd_NotFound);
    TA_UT_ADD_SUBTEST(Test_TA_SetTaskAffinityCmd_SetError);
    TA_UT_ADD_SUBTEST(Test_TA_SetTaskAffinityCmd_GetError);
    TA_UT_ADD_SUBTEST(Test_TA_GetTaskAffinityCmd_NominalSame);
    TA_UT_ADD_SUBTEST(Test_TA_GetTaskAffinityCmd_NominalDiff);
    TA_UT_ADD_SUBTEST(Test_TA_GetTaskAffinityCmd_NotFound);
    TA_UT_ADD_SUBTEST(Test_TA_GetTaskAffinityCmd_GetError);
    TA_UT_ADD_SUBTEST(Test_TA_SetTaskAffinityCmd_TableFull);
    TA_UT_ADD_SUBTEST(Test_TA_GetTaskAffinityCmd_TableFull);
    TA_UT_ADD_SUBTEST(Test_TA_CpuGet_MaxCores);
    TA_UT_ADD_SUBTEST(Test_TA_ProcessCmd_ResetCounters);
    TA_UT_ADD_SUBTEST(Test_TA_GetTaskAffinityCmd_IdNotFound);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_IdNotFound);
    TA_UT_ADD_SUBTEST(Test_TA_SetTaskAffinityCmd_IdNotFound);
    TA_UT_ADD_SUBTEST(Test_TA_CpuSet_LoopLimit_MaxBits);
    TA_UT_ADD_SUBTEST(Test_TA_CpuSet_LoopLimit_MaxCpus);
    TA_UT_ADD_SUBTEST(Test_TA_CpuSet_LoopLimit_AvailCores);
}

void Test_QueryAll(void)
{
    TA_UT_ADD_SUBTEST(Test_TA_QueryAll_Nominal);
    TA_UT_ADD_SUBTEST(Test_TA_QueryAll_ParseError);
    TA_UT_ADD_SUBTEST(Test_TA_QueryAll_OpenError);
    TA_UT_ADD_SUBTEST(Test_TA_QueryAll_WriteHdrError);
    TA_UT_ADD_SUBTEST(Test_TA_QueryAll_WriteDataError);
}

void Test_StartupAffinity(void)
{
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_AddSuccess);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_AddGetError);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_OsDefaultSuccess);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_OsDefaultGetError);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_MaskSuccess);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_MaskSetError);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_MaskGetError);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_InvalidType);
    TA_UT_ADD_SUBTEST(Test_TA_SetStartupAffinity_TableFull);
}

void Test_Events(void)
{
    TA_UT_ADD_SUBTEST(Test_TA_OS_EventHandler_Nominal);
    TA_UT_ADD_SUBTEST(Test_TA_OS_EventHandler_OtherEvent);
    TA_UT_ADD_SUBTEST(Test_TA_OS_EventHandler_AllOtherEvents);
    TA_UT_ADD_SUBTEST(Test_TA_OS_EventHandler_GetNameError);
}

/* --------------------------------------------------------- */
/* INIT TESTS                                                */
/* --------------------------------------------------------- */
void Test_TA_EarlyInit_NullTable(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_RegisterEventHandler), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_EarlyInit(), CFE_SUCCESS);
    UtAssert_True(CFE_TA_CpuTaskAffinity[0].TaskName[0] == '\0', "Loop bypassed for NULL table");
}

void Test_TA_EarlyInit_WithTable(void)
{
    static CFE_TA_CpuTaskAffinity_t mock_table[2];
    memset(mock_table, 0, sizeof(mock_table));

    /* Set up a valid task in slot 0, and leave slot 1 empty to break the loop */
    strncpy(mock_table[0].TaskName, "MOCK_TASK", sizeof(mock_table[0].TaskName));

    /* Feed the ENTIRE array into the data buffer so the mock returns a pointer to it */
    UT_SetDataBuffer(UT_KEY(CFE_Config_GetObjPointer), mock_table, sizeof(mock_table), false);
    UT_SetDeferredRetcode(UT_KEY(OS_RegisterEventHandler), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_EarlyInit(), CFE_SUCCESS);
    UtAssert_True(strcmp(CFE_TA_CpuTaskAffinity[0].TaskName, "MOCK_TASK") == 0, "TaskName matched");
    UtAssert_True(CFE_TA_CpuTaskAffinity[1].TaskName[0] == '\0', "Loop broke cleanly");
}

void Test_TA_EarlyInit_MaxTasks(void)
{
    static CFE_TA_CpuTaskAffinity_t mock_table[OS_MAX_TASKS];
    uint32                          i;

    memset(mock_table, 0, sizeof(mock_table));
    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        strncpy(mock_table[i].TaskName, "TASK", sizeof(mock_table[i].TaskName));
    }

    /* Feed the ENTIRE full array into the data buffer */
    UT_SetDataBuffer(UT_KEY(CFE_Config_GetObjPointer), mock_table, sizeof(mock_table), false);
    UT_SetDeferredRetcode(UT_KEY(OS_RegisterEventHandler), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_EarlyInit(), CFE_SUCCESS);
    UtAssert_True(strcmp(CFE_TA_CpuTaskAffinity[OS_MAX_TASKS - 1].TaskName, "TASK") == 0, "Max tasks copied");
}

void Test_TA_EarlyInit_Error(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_RegisterEventHandler), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_EarlyInit(), CFE_SUCCESS);
}

void Test_TA_TaskInit(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityCoresConfigured), 1, 4);
    UtAssert_INT32_EQ(CFE_TA_TaskInit(), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CFE_TA_DiagInfo.CoresConfigured, 4);
}

void Test_TA_TaskInit_Errors(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_MSG_Init), 1, CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TA_TaskInit(), CFE_MSG_BAD_ARGUMENT);

    TA_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_APP_REGISTER);
    UtAssert_INT32_EQ(CFE_TA_TaskInit(), CFE_ES_ERR_APP_REGISTER);

    TA_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_EVS_APP_NOT_REGISTERED);
    UtAssert_INT32_EQ(CFE_TA_TaskInit(), CFE_EVS_APP_NOT_REGISTERED);

    TA_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SB_PIPE_CR_ERR);
    UtAssert_INT32_EQ(CFE_TA_TaskInit(), CFE_SB_PIPE_CR_ERR);

    TA_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TA_TaskInit(), CFE_SB_BAD_ARGUMENT);

    TA_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TA_TaskInit(), CFE_SB_BAD_ARGUMENT);
}

void Test_TA_CleanUpApp(void)
{
    UtAssert_INT32_EQ(CFE_TA_CleanUpApp(0), CFE_SUCCESS);
}

void Test_TA_EarlyInit_HandlerError(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_RegisterEventHandler), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_EarlyInit(), CFE_SUCCESS);
}

/* --------------------------------------------------------- */
/* MAIN TESTS                                                */
/* --------------------------------------------------------- */
void Test_TA_Main_InitFail(void)
{
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_APP_REGISTER);
    UtAssert_VOIDCALL(CFE_TA_TaskMain());
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 2);
}

void Test_TA_Main_RcvError(void)
{
    /* Provide an UNKNOWN MsgId so the dispatcher safely ignores the empty packet */
    CFE_SB_MsgId_t DummyId = CFE_SB_ValueToMsgId(0x1234);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &DummyId, sizeof(DummyId), false);

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 1, CFE_SB_PIPE_RD_ERR);

    UtAssert_VOIDCALL(CFE_TA_TaskMain());
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
}

void Test_TA_Main_Nominal(void)
{
    CFE_SB_Buffer_t  Buf;
    CFE_SB_Buffer_t *BufPtrs[2] = { &Buf, NULL };

    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, CFE_SUCCESS);

    UT_SetupBasicMsgDispatch(&UT_TPID_CFE_TA_CMD_NOOP_CC, sizeof(CFE_TA_NoopCmd_t), true);
    UT_SetDataBuffer(UT_KEY(CFE_SB_ReceiveBuffer), BufPtrs, sizeof(BufPtrs), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_ReceiveBuffer), 2, CFE_SB_PIPE_RD_ERR);

    UtAssert_VOIDCALL(CFE_TA_TaskMain());
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);
}

/* --------------------------------------------------------- */
/* CMDS AND DISPATCH TESTS                                   */
/* --------------------------------------------------------- */

void Test_TA_TaskPipe_InvalidMsgId(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_TA_DoDispatchCheckEvents(&Buf, sizeof(Buf), UT_TPID_CFE_TA_INVALID_MID, &UT_TA_EventBuf);
    UtAssert_UINT32_EQ(UT_TA_EventBuf.EventID, CFE_TA_ERR_MSGID_EID);
}

void Test_TA_TaskPipe_HK(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_CallTaskPipe(CFE_TA_TaskPipe, &Buf.Msg, sizeof(CFE_TA_SendHkCmd_t), UT_TPID_CFE_TA_SEND_HK);
    UtAssert_STUB_COUNT(CFE_SB_TransmitMsg, 1);
}

void Test_TA_ProcessCmd_BadCC(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_TA_DoDispatchCheckEvents(&Buf, sizeof(Buf), UT_TPID_CFE_TA_CMD_INVALID_CC, &UT_TA_EventBuf);
    UtAssert_UINT32_EQ(UT_TA_EventBuf.EventID, CFE_TA_ERR_CC_EID);
}

void Test_TA_ProcessCmd_BadLength_Noop(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_TA_DoDispatchCheckEvents(&Buf, 0, UT_TPID_CFE_TA_CMD_NOOP_CC, &UT_TA_EventBuf);
    UtAssert_UINT32_EQ(UT_TA_EventBuf.EventID, CFE_TA_LEN_ERR_EID);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_ProcessCmd_BadLength_Reset(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_TA_DoDispatchCheckEvents(&Buf, 0, UT_TPID_CFE_TA_CMD_RESET_COUNTERS_CC, &UT_TA_EventBuf);
    UtAssert_UINT32_EQ(UT_TA_EventBuf.EventID, CFE_TA_LEN_ERR_EID);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_ProcessCmd_BadLength_QueryAll(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_TA_DoDispatchCheckEvents(&Buf, 0, UT_TPID_CFE_TA_CMD_QUERY_ALL_TASKS_CC, &UT_TA_EventBuf);
    UtAssert_UINT32_EQ(UT_TA_EventBuf.EventID, CFE_TA_LEN_ERR_EID);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_ProcessCmd_BadLength_SetAffinity(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_TA_DoDispatchCheckEvents(&Buf, 0, UT_TPID_CFE_TA_CMD_SET_TASK_AFFINITY_CC, &UT_TA_EventBuf);
    UtAssert_UINT32_EQ(UT_TA_EventBuf.EventID, CFE_TA_LEN_ERR_EID);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_ProcessCmd_BadLength_GetAffinity(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_TA_DoDispatchCheckEvents(&Buf, 0, UT_TPID_CFE_TA_CMD_GET_TASK_AFFINITY_CC, &UT_TA_EventBuf);
    UtAssert_UINT32_EQ(UT_TA_EventBuf.EventID, CFE_TA_LEN_ERR_EID);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_ProcessCmd_QueryAll(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_TA_TaskPipe,
                    &Buf.Msg,
                    sizeof(CFE_TA_QueryAllTasksCmd_t),
                    UT_TPID_CFE_TA_CMD_QUERY_ALL_TASKS_CC);
}

void Test_TA_ProcessCmd_SetAffinity(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_CallTaskPipe(CFE_TA_TaskPipe,
                    &Buf.Msg,
                    sizeof(CFE_TA_SetTaskAffinityCmd_t),
                    UT_TPID_CFE_TA_CMD_SET_TASK_AFFINITY_CC);
}

void Test_TA_ProcessCmd_GetAffinity(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_CallTaskPipe(CFE_TA_TaskPipe,
                    &Buf.Msg,
                    sizeof(CFE_TA_GetTaskAffinityCmd_t),
                    UT_TPID_CFE_TA_CMD_GET_TASK_AFFINITY_CC);
}

void Test_TA_Dispatch_CommandError(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_TA_TaskPipe,
                    &Buf.Msg,
                    sizeof(CFE_TA_GetTaskAffinityCmd_t),
                    UT_TPID_CFE_TA_CMD_GET_TASK_AFFINITY_CC);
}

void Test_TA_ProcessCmd_ResetCounters(void)
{
    CFE_SB_Buffer_t Buf;
    memset(&Buf, 0, sizeof(Buf));

    CFE_TA_Global.CommandCounter      = 5;
    CFE_TA_Global.CommandErrorCounter = 3;

    UT_CallTaskPipe(CFE_TA_TaskPipe, &Buf.Msg, sizeof(CFE_TA_ResetCountersCmd_t), UT_TPID_CFE_TA_CMD_RESET_COUNTERS_CC);
}

void Test_TA_NoopCmd(void)
{
    CFE_TA_NoopCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    UtAssert_INT32_EQ(CFE_TA_NoopCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandCounter, 1);
}

void Test_TA_ResetCountersCmd(void)
{
    CFE_TA_ResetCountersCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    CFE_TA_Global.CommandCounter      = 5;
    CFE_TA_Global.CommandErrorCounter = 3;
    UtAssert_INT32_EQ(CFE_TA_ResetCountersCmd(&cmd), CFE_STATUS_NO_COUNTER_INCREMENT);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandCounter, 0);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 0);
}

/* --------------------------------------------------------- */
/* QUERY ALL TASKS TESTS                                     */
/* --------------------------------------------------------- */
void Test_TA_QueryAll_Nominal(void)
{
    CFE_TA_QueryAllTasksCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, sizeof(CFE_FS_Header_t));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, sizeof(CFE_TA_DiagInfo_t));
    UtAssert_INT32_EQ(CFE_TA_QueryAllTasksCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 0);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandCounter, 1);
}

void Test_TA_QueryAll_ParseError(void)
{
    CFE_TA_QueryAllTasksCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UtAssert_INT32_EQ(CFE_TA_QueryAllTasksCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_QueryAll_OpenError(void)
{
    CFE_TA_QueryAllTasksCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_QueryAllTasksCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_QueryAll_WriteHdrError(void)
{
    CFE_TA_QueryAllTasksCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, 0);
    UtAssert_INT32_EQ(CFE_TA_QueryAllTasksCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_QueryAll_WriteDataError(void)
{
    CFE_TA_QueryAllTasksCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, sizeof(CFE_FS_Header_t));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, 0);
    UtAssert_INT32_EQ(CFE_TA_QueryAllTasksCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

/* --------------------------------------------------------- */
/* SET TASK AFFINITY TESTS                                   */
/* --------------------------------------------------------- */
void Test_TA_SetTaskAffinityCmd_Nominal(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    memset(cmd.Payload.AffinityMask, 0, sizeof(cmd.Payload.AffinityMask));
    cmd.Payload.AffinityMask[0] = 0x03; /* Sets bits 0 and 1 */

    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);
    OS_CPUSET_ZERO(&CFE_TA_CpuTaskAffinity[0].CpuMask);

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskAffinityCoresConfigured), 4);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandCounter, 1);
}

void Test_TA_SetTaskAffinityCmd_NotFound(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MISSING", sizeof(cmd.Payload.TaskName));
    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_TA_APP_TASK_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_SetTaskAffinityCmd_SetError(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_TA_APP_SET_TASK_AFFINITY_FAILED);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_SetTaskAffinityCmd_GetError(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_TA_APP_GET_TASK_AFFINITY_FAILED);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_SetTaskAffinityCmd_TableFull(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    int                         i;

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));

    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        strncpy(CFE_TA_CpuTaskAffinity[i].TaskName, "OTHER_TASK", CFE_MISSION_MAX_API_LEN);
    }

    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_SUCCESS);
}

void Test_TA_SetTaskAffinityCmd_IdNotFound(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERROR);

    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_TA_APP_SET_TASK_AFFINITY_FAILED);
}

/* --------------------------------------------------------- */
/* GET TASK AFFINITY TESTS                                   */
/* --------------------------------------------------------- */
void Test_TA_GetTaskAffinityCmd_NominalDiff(void)
{
    CFE_TA_GetTaskAffinityCmd_t cmd;
    OS_cpuset_t                 mock_cpuset;

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    OS_CPUSET_ZERO(&CFE_TA_CpuTaskAffinity[0].CpuFromOS);
    OS_CPUSET_SET(2, &CFE_TA_CpuTaskAffinity[0].CpuFromOS); /* Differs from mock */

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskAffinityCoresConfigured), 4);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);

    /* Mock OS_TaskAffinityGetAffinity returning core 0 */
    OS_CPUSET_ZERO(&mock_cpuset);
    OS_CPUSET_SET(0, &mock_cpuset);
    UT_SetDataBuffer(UT_KEY(OS_TaskAffinityGetAffinity), &mock_cpuset, sizeof(mock_cpuset), false);

    UtAssert_INT32_EQ(CFE_TA_GetTaskAffinityCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandCounter, 1);
}

void Test_TA_GetTaskAffinityCmd_NominalSame(void)
{
    CFE_TA_GetTaskAffinityCmd_t cmd;
    OS_cpuset_t                 mock_cpuset;

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    OS_CPUSET_ZERO(&CFE_TA_CpuTaskAffinity[0].CpuFromOS);
    OS_CPUSET_SET(0, &CFE_TA_CpuTaskAffinity[0].CpuFromOS);

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskAffinityCoresConfigured), 4);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);

    /* Mock OS_TaskAffinityGetAffinity returning the exact same set */
    OS_CPUSET_ZERO(&mock_cpuset);
    OS_CPUSET_SET(0, &mock_cpuset);
    UT_SetDataBuffer(UT_KEY(OS_TaskAffinityGetAffinity), &mock_cpuset, sizeof(mock_cpuset), false);

    UtAssert_INT32_EQ(CFE_TA_GetTaskAffinityCmd(&cmd), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandCounter, 1);
}

void Test_TA_GetTaskAffinityCmd_NotFound(void)
{
    CFE_TA_GetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MISSING", sizeof(cmd.Payload.TaskName));
    UtAssert_INT32_EQ(CFE_TA_GetTaskAffinityCmd(&cmd), CFE_TA_APP_TASK_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_GetTaskAffinityCmd_GetError(void)
{
    CFE_TA_GetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_GetTaskAffinityCmd(&cmd), CFE_TA_APP_GET_TASK_AFFINITY_FAILED);
    UtAssert_INT32_EQ(CFE_TA_Global.CommandErrorCounter, 1);
}

void Test_TA_GetTaskAffinityCmd_TableFull(void)
{
    CFE_TA_GetTaskAffinityCmd_t cmd;
    int                         i;

    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));

    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        strncpy(CFE_TA_CpuTaskAffinity[i].TaskName, "OTHER_TASK", CFE_MISSION_MAX_API_LEN);
    }

    UtAssert_INT32_EQ(CFE_TA_GetTaskAffinityCmd(&cmd), CFE_SUCCESS);
}

void Test_TA_GetTaskAffinityCmd_IdNotFound(void)
{
    CFE_TA_GetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERROR);

    UtAssert_INT32_EQ(CFE_TA_GetTaskAffinityCmd(&cmd), CFE_TA_APP_GET_TASK_AFFINITY_FAILED);
}

/* --------------------------------------------------------- */
/* STARTUP AFFINITY TESTS                                    */
/* --------------------------------------------------------- */
void Test_TA_SetStartupAffinity_AddSuccess(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("NEW_TASK"), CFE_SUCCESS);
}

void Test_TA_SetStartupAffinity_AddGetError(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("NEW_TASK"), CFE_TA_APP_GET_TASK_AFFINITY_FAILED);
}

void Test_TA_SetStartupAffinity_OsDefaultSuccess(void)
{
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "DEF_TASK", CFE_MISSION_MAX_API_LEN);
    CFE_TA_CpuTaskAffinity[0].Type = CFE_TA_OsDefault;

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("DEF_TASK"), CFE_SUCCESS);
}

void Test_TA_SetStartupAffinity_OsDefaultGetError(void)
{
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "DEF_TASK", CFE_MISSION_MAX_API_LEN);
    CFE_TA_CpuTaskAffinity[0].Type = CFE_TA_OsDefault;

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("DEF_TASK"), CFE_TA_APP_GET_TASK_AFFINITY_FAILED);
}

void Test_TA_SetStartupAffinity_MaskSuccess(void)
{
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MSK_TASK", CFE_MISSION_MAX_API_LEN);
    CFE_TA_CpuTaskAffinity[0].Type = CFE_TA_Mask;

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("MSK_TASK"), CFE_SUCCESS);
}

void Test_TA_SetStartupAffinity_MaskSetError(void)
{
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MSK_TASK", CFE_MISSION_MAX_API_LEN);
    CFE_TA_CpuTaskAffinity[0].Type = CFE_TA_Mask;

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("MSK_TASK"), CFE_TA_APP_SET_TASK_AFFINITY_FAILED);
}

void Test_TA_SetStartupAffinity_MaskGetError(void)
{
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MSK_TASK", CFE_MISSION_MAX_API_LEN);
    CFE_TA_CpuTaskAffinity[0].Type = CFE_TA_Mask;

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("MSK_TASK"), CFE_TA_APP_GET_TASK_AFFINITY_FAILED);
}

void Test_TA_SetStartupAffinity_InvalidType(void)
{
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "INV_TASK", CFE_MISSION_MAX_API_LEN);
    CFE_TA_CpuTaskAffinity[0].Type = 99; /* Invalid */

    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("INV_TASK"), CFE_TA_APP_INVALID_TASK_AFFINITY_TYPE);
}

void Test_TA_SetStartupAffinity_TableFull(void)
{
    int i;
    for (i = 0; i < OS_MAX_TASKS; i++)
    {
        strncpy(CFE_TA_CpuTaskAffinity[i].TaskName, "FILLER", CFE_MISSION_MAX_API_LEN);
    }
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("NEW_TASK"), CFE_SUCCESS);
}

void Test_TA_SetStartupAffinity_IdNotFound(void)
{
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_SetStartupAffinity("MY_TASK"), OS_ERROR);
}

/* --------------------------------------------------------- */
/* CFE_TA_Cpu_Set Loop Limit Branches                        */
/* --------------------------------------------------------- */
void Test_TA_CpuSet_LoopLimit_MaxBits(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    memset(cmd.Payload.AffinityMask, 0xFF, sizeof(cmd.Payload.AffinityMask));

    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskAffinityCoresConfigured), CFE_MISSION_TA_MAX_NUM_CPUS + 10);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_SUCCESS);
}

void Test_TA_CpuSet_LoopLimit_MaxCpus(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    memset(cmd.Payload.AffinityMask, 0xFF, sizeof(cmd.Payload.AffinityMask));

    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskAffinityCoresConfigured), OS_MAX_CPUS + 10);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_SUCCESS);
}

void Test_TA_CpuSet_LoopLimit_AvailCores(void)
{
    CFE_TA_SetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    memset(cmd.Payload.AffinityMask, 0xFF, sizeof(cmd.Payload.AffinityMask));

    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskAffinityCoresConfigured), 2);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinitySetAffinity), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_SetTaskAffinityCmd(&cmd), CFE_SUCCESS);
}

void Test_TA_CpuGet_MaxCores(void)
{
    CFE_TA_GetTaskAffinityCmd_t cmd;
    memset(&cmd, 0, sizeof(cmd));
    strncpy(cmd.Payload.TaskName, "MY_TASK", sizeof(cmd.Payload.TaskName));
    strncpy(CFE_TA_CpuTaskAffinity[0].TaskName, "MY_TASK", CFE_MISSION_MAX_API_LEN);

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskAffinityCoresConfigured), 64);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskAffinityGetAffinity), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_GetTaskAffinityCmd(&cmd), CFE_SUCCESS);
}

/* --------------------------------------------------------- */
/* EVENT HANDLER TESTS                                       */
/* --------------------------------------------------------- */
void Test_TA_OS_EventHandler_Nominal(void)
{
    osal_id_t mock_task_id     = { 123 };
    char      mock_task_name[] = "TEST_TASK";
    UT_SetDataBuffer(UT_KEY(OS_GetResourceName), mock_task_name, sizeof(mock_task_name), false);
    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_SUCCESS);

    UtAssert_INT32_EQ(CFE_TA_OS_EventHandler(OS_EVENT_TASK_STARTUP, mock_task_id, NULL), OS_SUCCESS);
}

void Test_TA_OS_EventHandler_OtherEvent(void)
{
    osal_id_t mock_task_id = { 123 };
    UtAssert_INT32_EQ(CFE_TA_OS_EventHandler(OS_EVENT_RESOURCE_CREATED, mock_task_id, NULL), OS_SUCCESS);
}

void Test_TA_OS_EventHandler_AllOtherEvents(void)
{
    osal_id_t mock_task_id = { 123 };
    UtAssert_INT32_EQ(CFE_TA_OS_EventHandler(OS_EVENT_RESOURCE_ALLOCATED, mock_task_id, NULL), OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_OS_EventHandler(OS_EVENT_RESOURCE_CREATED, mock_task_id, NULL), OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_OS_EventHandler(OS_EVENT_RESOURCE_DELETED, mock_task_id, NULL), OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_TA_OS_EventHandler((OS_Event_t)9999, mock_task_id, NULL), OS_SUCCESS);
}

void Test_TA_OS_EventHandler_GetNameError(void)
{
    osal_id_t mock_task_id = { 123 };
    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TA_OS_EventHandler(OS_EVENT_TASK_STARTUP, mock_task_id, NULL), OS_SUCCESS);
}
