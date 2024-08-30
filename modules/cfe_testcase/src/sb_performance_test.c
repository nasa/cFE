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

/**
 * @file
 *
 * Functional test of SB transmit/receive API performance
 *
 * The intent of this test is to perform a set of SB message transfers
 * at a sufficiently high rate / volume such that the performance of
 * the implementation can be characterized.  Note that this
 * cannot (currently) measure the performance directly, it merely implements a
 * scenario that allows the performance to be measured by an external test
 * harness.
 */

#include "cfe_test.h"
#include "cfe_msgids.h"
#include "cfe_test_msgids.h"
#include "cfe_test_msg.h"

/* Number of messages to send during test */
uint32_t UT_BulkTestDuration = 1000;

/* Number of SB messages sent before yielding CPU (has to be power of 2 minus 1)*/
static uint32_t UT_CpuYieldMask = 1024 - 1;

/* State structure for multicore test - shared between threads */
typedef struct UT_BulkMultiCoreSharedState
{
    volatile bool XmitFinished;
    volatile bool RecvFinished;

    CFE_ES_TaskId_t TaskIdXmit;
    CFE_ES_TaskId_t TaskIdRecv;
    CFE_SB_PipeId_t PipeId;
    osal_id_t       SyncSem;
    uint32          SendCount;
    uint32          RecvCount;
    OS_time_t       StartTime;
    OS_time_t       EndTime;

} UT_BulkMultiCoreSharedState_t;

UT_BulkMultiCoreSharedState_t BulkCmd;
UT_BulkMultiCoreSharedState_t BulkTlm;

/*
 * This test procedure should be agnostic to specific MID values, but it should
 * not overlap/interfere with real MIDs used by other apps.
 */
static CFE_SB_MsgId_t CFE_FT_CMD_MSGID;
static CFE_SB_MsgId_t CFE_FT_TLM_MSGID;

void TestBulkTransferSingle(void)
{
    CFE_SB_PipeId_t                    PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                    PipeId2 = CFE_SB_INVALID_PIPE;
    CFE_TEST_TestCmdMessage32_t        CmdMsg;
    CFE_TEST_TestTlmMessage32_t        TlmMsg;
    CFE_SB_Buffer_t *                  MsgBuf;
    const CFE_TEST_TestCmdMessage32_t *CmdPtr;
    const CFE_TEST_TestTlmMessage32_t *TlmPtr;
    uint32                             SendCount;
    OS_time_t                          StartTime;
    OS_time_t                          ElapsedTime;
    int64                              AvgRate;
    uint32_t                           PrintMask;

    memset(&CmdMsg, 0, sizeof(CmdMsg));
    memset(&TlmMsg, 0, sizeof(TlmMsg));

    PrintMask = UT_BulkTestDuration >> 4;
    PrintMask |= PrintMask >> 1;
    PrintMask |= PrintMask >> 2;
    PrintMask |= PrintMask >> 4;
    PrintMask |= PrintMask >> 8;
    PrintMask |= PrintMask >> 16;

    UtPrintf("Testing: Single Threaded Bulk SB Transmit/Receive");

    /* Setup, create a pipe and subscribe (one cmd, one tlm) */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 5, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 5, "TestPipe2"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId1, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_TLM_MSGID, PipeId2, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);

    /* Initialize the message content */
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(CmdMsg.CommandHeader), CFE_FT_CMD_MSGID, sizeof(CmdMsg)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(TlmMsg.TelemetryHeader), CFE_FT_TLM_MSGID, sizeof(TlmMsg)), CFE_SUCCESS);

    CFE_PSP_GetTime(&StartTime);

    for (SendCount = 0; SendCount < UT_BulkTestDuration; ++SendCount)
    {
        CmdMsg.Payload.Value = SendCount;
        TlmMsg.Payload.Value = ~SendCount;

        /* In order to not "flood" with test results, this should be silent unless a failure occurs */
        CFE_Assert_STATUS_STORE(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        CFE_Assert_STATUS_STORE(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmMsg.TelemetryHeader), true));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        CFE_Assert_STATUS_STORE(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_POLL));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        /* As above, to avoid flooding of test cases, only report mismatch here */
        CmdPtr = (const void *)MsgBuf;
        if (CmdPtr->Payload.Value != CmdMsg.Payload.Value)
        {
            UtAssert_UINT32_EQ(CmdPtr->Payload.Value, CmdMsg.Payload.Value);
            break;
        }

        CFE_Assert_STATUS_STORE(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, CFE_SB_POLL));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        TlmPtr = (const void *)MsgBuf;
        if (TlmPtr->Payload.Value != TlmMsg.Payload.Value)
        {
            UtAssert_UINT32_EQ(TlmPtr->Payload.Value, TlmMsg.Payload.Value);
            break;
        }

        /* report progress periodically */
        if ((SendCount & PrintMask) == 0)
        {
            UtPrintf("Success after %lu messages", (unsigned long)SendCount);
        }
    }

    CFE_PSP_GetTime(&ElapsedTime);
    ElapsedTime = OS_TimeSubtract(ElapsedTime, StartTime);

    UtAssert_MIR("Elapsed time for SB bulk message test: %lu usec",
                 (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));

    /* Note that in this test, each iteration sends and recvs TWO messages */
    AvgRate = OS_TimeGetTotalMilliseconds(ElapsedTime);
    AvgRate = ((int64)SendCount * 20000) / AvgRate;

    UtAssert_MIR("Message Rate: %ld.%01ld messages/sec", (long)(AvgRate / 10), (long)(AvgRate % 10));

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId2), CFE_SUCCESS);
}

void RunSingleCmdSendRecv(void)
{
    CFE_TEST_TestCmdMessage32_t        CmdMsg;
    CFE_SB_Buffer_t *                  MsgBuf;
    const CFE_TEST_TestCmdMessage32_t *CmdPtr;

    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(CmdMsg.CommandHeader), CFE_FT_CMD_MSGID, sizeof(CmdMsg)), CFE_SUCCESS);

    CFE_PSP_GetTime(&BulkCmd.StartTime);

    while (BulkCmd.SendCount < UT_BulkTestDuration)
    {
        CmdMsg.Payload.Value = BulkCmd.SendCount;

        /* In order to not "flood" with test results, this should be silent unless a failure occurs */
        CFE_Assert_STATUS_STORE(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        ++BulkCmd.SendCount;

        CFE_Assert_STATUS_STORE(CFE_SB_ReceiveBuffer(&MsgBuf, BulkCmd.PipeId, CFE_SB_POLL));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        ++BulkCmd.RecvCount;

        /* As above, to avoid flooding of test cases, only report mismatch here */
        CmdPtr = (const void *)MsgBuf;
        if (CmdPtr->Payload.Value != CmdMsg.Payload.Value)
        {
            UtAssert_UINT32_EQ(CmdPtr->Payload.Value, CmdMsg.Payload.Value);
            break;
        }

        /* Only yield CPU once in a while to avoid slowing down the test with too many context switches */
        if ((BulkCmd.SendCount & UT_CpuYieldMask) == 0)
        {
            /* Yield cpu to other task with same priority */
            OS_TaskDelay(0);
        }
    }

    CFE_PSP_GetTime(&BulkCmd.EndTime);

    BulkCmd.XmitFinished = true;
    BulkCmd.RecvFinished = true;
}

void RunSingleTlmSendRecv(void)
{
    CFE_TEST_TestTlmMessage32_t        TlmMsg;
    CFE_SB_Buffer_t *                  MsgBuf;
    const CFE_TEST_TestTlmMessage32_t *TlmPtr;

    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(TlmMsg.TelemetryHeader), CFE_FT_TLM_MSGID, sizeof(TlmMsg)), CFE_SUCCESS);

    CFE_PSP_GetTime(&BulkTlm.StartTime);

    while (BulkTlm.SendCount < UT_BulkTestDuration)
    {
        TlmMsg.Payload.Value = BulkTlm.SendCount;

        /* In order to not "flood" with test results, this should be silent unless a failure occurs */
        CFE_Assert_STATUS_STORE(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmMsg.TelemetryHeader), true));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        ++BulkTlm.SendCount;

        CFE_Assert_STATUS_STORE(CFE_SB_ReceiveBuffer(&MsgBuf, BulkTlm.PipeId, CFE_SB_POLL));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        ++BulkTlm.RecvCount;

        /* As above, to avoid flooding of test cases, only report mismatch here */
        TlmPtr = (const void *)MsgBuf;
        if (TlmPtr->Payload.Value != TlmMsg.Payload.Value)
        {
            UtAssert_UINT32_EQ(TlmPtr->Payload.Value, TlmMsg.Payload.Value);
            break;
        }

        /* Only yield CPU once in a while to avoid slowing down the test with too many context switches */
        if ((BulkTlm.SendCount & UT_CpuYieldMask) == 0)
        {
            /* Yield cpu to other task with same priority */
            OS_TaskDelay(0);
        }
    }

    CFE_PSP_GetTime(&BulkTlm.EndTime);

    BulkTlm.XmitFinished = true;
    BulkTlm.RecvFinished = true;
}

OS_time_t UT_GetTotalElapsedTime(void)
{
    OS_time_t StartTime;
    OS_time_t EndTime;

    /*
     * Computing the average message rate is the total number of messages passed in ALL threads,
     * divided by the time elapsed by ALL threads, which is the difference between the first
     * thread to start and the last thread to finish.
     *
     * Note that the two threads have equal priority and no blocking/yielding in either, so it is
     * possible based on OS scheduler that one thread runs entirely to completion before the second
     * thread even starts running.
     */
    if (OS_TimeGetTotalNanoseconds(OS_TimeSubtract(BulkCmd.StartTime, BulkTlm.StartTime)) < 0)
    {
        /* This means BulkCmd started first */
        StartTime = BulkCmd.StartTime;
    }
    else
    {
        /* This means BulkTlm started first */
        StartTime = BulkTlm.StartTime;
    }

    if (OS_TimeGetTotalNanoseconds(OS_TimeSubtract(BulkCmd.EndTime, BulkTlm.EndTime)) < 0)
    {
        /* This means BulkTlm ended last */
        EndTime = BulkTlm.EndTime;
    }
    else
    {
        /* This means BulkCmd ended last */
        EndTime = BulkCmd.EndTime;
    }

    return OS_TimeSubtract(EndTime, StartTime);
}

void TestBulkTransferMulti2(void)
{
    OS_time_t ElapsedTime;
    int64     AvgRate;

    UtPrintf("Testing: 2 Thread Bulk SB Transmit/Receive without Sync Sem");
    memset(&BulkCmd, 0, sizeof(BulkCmd));
    memset(&BulkTlm, 0, sizeof(BulkCmd));

    /* Setup, create a pipe and subscribe (one cmd, one tlm) */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&BulkCmd.PipeId, 5, "CmdPipe"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&BulkTlm.PipeId, 5, "TlmPipe"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, BulkCmd.PipeId, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_TLM_MSGID, BulkTlm.PipeId, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&BulkCmd.TaskIdXmit, "CmdXmit", RunSingleCmdSendRecv, NULL, 32768, 150, 0),
                      CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&BulkTlm.TaskIdXmit, "TlmXmit", RunSingleTlmSendRecv, NULL, 32768, 150, 0),
                      CFE_SUCCESS);

    do
    {
        OS_TaskDelay(1000);
        UtPrintf("Counts => %lu/%lu CMD, %lu/%lu TLM", (unsigned long)BulkCmd.SendCount,
                 (unsigned long)BulkCmd.RecvCount, (unsigned long)BulkTlm.SendCount, (unsigned long)BulkTlm.RecvCount);
    } while (!BulkCmd.XmitFinished || !BulkCmd.RecvFinished || !BulkTlm.XmitFinished || !BulkTlm.RecvFinished);

    ElapsedTime = OS_TimeSubtract(BulkCmd.EndTime, BulkCmd.StartTime);
    UtAssert_MIR("Elapsed time for SB bulk CMD thread: %lu usec",
                 (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));

    ElapsedTime = OS_TimeSubtract(BulkTlm.EndTime, BulkTlm.StartTime);
    UtAssert_MIR("Elapsed time for SB bulk TLM thread: %lu usec",
                 (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));

    ElapsedTime = UT_GetTotalElapsedTime();
    UtAssert_MIR("Elapsed time for all threads: %lu usec", (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));

    /* The message rate is the total number of messages processed over the total time elapsed */
    AvgRate = OS_TimeGetTotalMilliseconds(ElapsedTime);
    if (AvgRate > 0)
    {
        AvgRate = ((int64)(BulkCmd.RecvCount + BulkTlm.RecvCount) * 10000) / AvgRate;
        UtAssert_MIR("Message Rate: %ld.%01ld messages/sec", (long)(AvgRate / 10), (long)(AvgRate % 10));
    }
    else
    {
        /* If the entire test took less than a millisecond, then there is a config error */
        UtAssert_Failed("Test configuration error: Executed too fast, needs more cycles!");
    }

    UtAssert_INT32_EQ(CFE_SB_DeletePipe(BulkCmd.PipeId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(BulkTlm.PipeId), CFE_SUCCESS);
}

void UT_CommandTransmitterTask(void)
{
    CFE_SB_Buffer_t *            BufPtr;
    CFE_TEST_TestCmdMessage32_t *CmdMsgPtr;

    CFE_PSP_GetTime(&BulkCmd.StartTime);

    for (BulkCmd.SendCount = 0; BulkCmd.SendCount < UT_BulkTestDuration; ++BulkCmd.SendCount)
    {
        CFE_Assert_STATUS_STORE(OS_CountSemTake(BulkCmd.SyncSem));
        if (!CFE_Assert_STATUS_SILENTCHECK(OS_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(OS_SUCCESS);
            break;
        }

        BufPtr = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestCmdMessage32_t));

        CmdMsgPtr = (void *)&BufPtr->Msg;

        /* Initialize the message content */
        CFE_MSG_Init(CFE_MSG_PTR(CmdMsgPtr->CommandHeader), CFE_FT_CMD_MSGID, sizeof(*CmdMsgPtr));

        CmdMsgPtr->Payload.Value = BulkCmd.SendCount;

        CFE_Assert_STATUS_STORE(CFE_SB_TransmitBuffer(BufPtr, true));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);
            break;
        }

        /* Only yield CPU once in a while to avoid slowing down the test with too many context switches */
        if ((BulkCmd.SendCount & UT_CpuYieldMask) == 0)
        {
            /* Yield cpu to other task with same priority */
            OS_TaskDelay(0);
        }
    }

    BulkCmd.XmitFinished = true;
}

void UT_TelemetryTransmitterTask(void)
{
    CFE_SB_Buffer_t *            BufPtr;
    CFE_TEST_TestTlmMessage32_t *TlmMsgPtr;

    CFE_PSP_GetTime(&BulkTlm.StartTime);

    for (BulkTlm.SendCount = 0; BulkTlm.SendCount < UT_BulkTestDuration; ++BulkTlm.SendCount)
    {
        CFE_Assert_STATUS_STORE(OS_CountSemTake(BulkTlm.SyncSem));
        if (!CFE_Assert_STATUS_SILENTCHECK(OS_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(OS_SUCCESS);
            break;
        }

        BufPtr = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestTlmMessage32_t));

        TlmMsgPtr = (void *)&BufPtr->Msg;

        /* Initialize the message content */
        CFE_MSG_Init(CFE_MSG_PTR(TlmMsgPtr->TelemetryHeader), CFE_FT_TLM_MSGID, sizeof(*TlmMsgPtr));

        TlmMsgPtr->Payload.Value = BulkTlm.SendCount;

        CFE_Assert_STATUS_STORE(CFE_SB_TransmitBuffer(BufPtr, true));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);
            break;
        }

        /* Only yield CPU once in a while to avoid slowing down the test with too many context switches */
        if ((BulkTlm.SendCount & UT_CpuYieldMask) == 0)
        {
            /* Yield cpu to other task with same priority */
            OS_TaskDelay(0);
        }
    }

    BulkTlm.XmitFinished = true;
}

void UT_CommandReceiverTask(void)
{
    CFE_SB_Buffer_t *                  MsgBuf;
    const CFE_TEST_TestCmdMessage32_t *CmdPtr;

    for (BulkCmd.RecvCount = 0; BulkCmd.RecvCount < UT_BulkTestDuration; ++BulkCmd.RecvCount)
    {
        CFE_Assert_STATUS_STORE(CFE_SB_ReceiveBuffer(&MsgBuf, BulkCmd.PipeId, 5000));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);
            break;
        }

        /* As above, to avoid flooding of test cases, only report mismatch here */
        CmdPtr = (const void *)MsgBuf;
        if (CmdPtr->Payload.Value != BulkCmd.RecvCount)
        {
            UtAssert_UINT32_EQ(CmdPtr->Payload.Value, BulkCmd.RecvCount);
            break;
        }

        CFE_Assert_STATUS_STORE(OS_CountSemGive(BulkCmd.SyncSem));
        if (!CFE_Assert_STATUS_SILENTCHECK(OS_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(OS_SUCCESS);
            break;
        }
    }

    CFE_PSP_GetTime(&BulkCmd.EndTime);
    BulkCmd.RecvFinished = true;
}

void UT_TelemetryReceiverTask(void)
{
    CFE_SB_Buffer_t *                  MsgBuf;
    const CFE_TEST_TestTlmMessage32_t *TlmPtr;

    for (BulkTlm.RecvCount = 0; BulkTlm.RecvCount < UT_BulkTestDuration; ++BulkTlm.RecvCount)
    {
        CFE_Assert_STATUS_STORE(CFE_SB_ReceiveBuffer(&MsgBuf, BulkTlm.PipeId, 5000));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(CFE_SUCCESS);
            break;
        }

        /* As above, to avoid flooding of test cases, only report mismatch here */
        TlmPtr = (const void *)MsgBuf;
        if (TlmPtr->Payload.Value != BulkTlm.RecvCount)
        {
            UtAssert_UINT32_EQ(TlmPtr->Payload.Value, BulkTlm.RecvCount);
            break;
        }

        CFE_Assert_STATUS_STORE(OS_CountSemGive(BulkTlm.SyncSem));
        if (!CFE_Assert_STATUS_SILENTCHECK(OS_SUCCESS))
        {
            CFE_Assert_STATUS_MUST_BE(OS_SUCCESS);
            break;
        }
    }

    CFE_PSP_GetTime(&BulkTlm.EndTime);
    BulkTlm.RecvFinished = true;
}

void TestBulkTransferMulti4(void)
{
    uint32    i;
    OS_time_t ElapsedTime;
    int64     AvgRate;

    UtPrintf("Testing: 4 Thread Bulk SB Transmit/Receive with Sync Sem");
    memset(&BulkCmd, 0, sizeof(BulkCmd));
    memset(&BulkTlm, 0, sizeof(BulkCmd));

    /* Setup, create a pipe and subscribe (one cmd, one tlm) */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&BulkCmd.PipeId, 10, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&BulkTlm.PipeId, 10, "TestPipe2"), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, BulkCmd.PipeId, CFE_SB_DEFAULT_QOS, 8), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_TLM_MSGID, BulkTlm.PipeId, CFE_SB_DEFAULT_QOS, 8), CFE_SUCCESS);

    UtAssert_INT32_EQ(OS_CountSemCreate(&BulkCmd.SyncSem, "CmdSem", 0, 0), OS_SUCCESS);
    UtAssert_INT32_EQ(OS_CountSemCreate(&BulkTlm.SyncSem, "TlmSem", 0, 0), OS_SUCCESS);

    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&BulkCmd.TaskIdXmit, "CmdXmit", UT_CommandTransmitterTask, NULL, 32768, 150, 0),
        CFE_SUCCESS);
    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&BulkTlm.TaskIdXmit, "TlmXmit", UT_TelemetryTransmitterTask, NULL, 32768, 150, 0),
        CFE_SUCCESS);
    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&BulkCmd.TaskIdRecv, "CmdRecv", UT_CommandReceiverTask, NULL, 32768, 100, 0),
        CFE_SUCCESS);
    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&BulkTlm.TaskIdRecv, "TlmRecv", UT_TelemetryReceiverTask, NULL, 32768, 100, 0),
        CFE_SUCCESS);

    /* Let all tasks start and pend on sem */
    OS_TaskDelay(500);

    /* Give sem several times each to get a pipeline going, but do not exceed msglim of 8 */
    for (i = 0; i < 8; ++i)
    {
        UtAssert_INT32_EQ(OS_CountSemGive(BulkCmd.SyncSem), OS_SUCCESS);
        UtAssert_INT32_EQ(OS_CountSemGive(BulkTlm.SyncSem), OS_SUCCESS);
    }

    do
    {
        OS_TaskDelay(1000);
        UtPrintf("Counts => %lu/%lu CMD, %lu/%lu TLM", (unsigned long)BulkCmd.SendCount,
                 (unsigned long)BulkCmd.RecvCount, (unsigned long)BulkTlm.SendCount, (unsigned long)BulkTlm.RecvCount);
    } while (!BulkCmd.XmitFinished || !BulkCmd.RecvFinished || !BulkTlm.XmitFinished || !BulkTlm.RecvFinished);

    ElapsedTime = OS_TimeSubtract(BulkCmd.EndTime, BulkCmd.StartTime);
    UtAssert_MIR("Elapsed time for SB bulk CMD thread: %lu usec",
                 (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));

    ElapsedTime = OS_TimeSubtract(BulkTlm.EndTime, BulkCmd.StartTime);
    UtAssert_MIR("Elapsed time for SB bulk TLM thread: %lu usec",
                 (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));

    ElapsedTime = UT_GetTotalElapsedTime();
    UtAssert_MIR("Elapsed time for all threads: %lu usec", (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));

    /* The message rate is the total number of messages processed over the total time elapsed */
    AvgRate = OS_TimeGetTotalMilliseconds(ElapsedTime);
    if (AvgRate > 0)
    {
        AvgRate = ((int64)(BulkCmd.RecvCount + BulkTlm.RecvCount) * 10000) / AvgRate;
        UtAssert_MIR("Message Rate: %ld.%01ld messages/sec", (long)(AvgRate / 10), (long)(AvgRate % 10));
    }
    else
    {
        /* If the entire test took less than a millisecond, then there is a config error */
        UtAssert_Failed("Test configuration error: Executed too fast, needs more cycles!");
    }

    /* Child tasks should have self-exited... */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(BulkCmd.PipeId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(BulkTlm.PipeId), CFE_SUCCESS);
    UtAssert_INT32_EQ(OS_CountSemDelete(BulkCmd.SyncSem), OS_SUCCESS);
    UtAssert_INT32_EQ(OS_CountSemDelete(BulkTlm.SyncSem), OS_SUCCESS);
}

void SBPerformanceTestSetup(void)
{
    long      i;
    OS_time_t clock_start;
    OS_time_t clock_now;

    i = 0;
    CFE_PSP_GetTime(&clock_start);
    do
    {
        ++i;
        CFE_PSP_GetTime(&clock_now);
    } while (OS_TimeGetTotalMilliseconds(OS_TimeSubtract(clock_now, clock_start)) < 100);

    UtAssert_MIR("Local CPU speed loop count=%ld", i);

    while (i > 0)
    {
        i /= 5;
        UT_BulkTestDuration *= 2;
    }

    UtAssert_MIR("Configured to execute %lu message transfers", (unsigned long)UT_BulkTestDuration);

    /*
     * This test procedure should be agnostic to specific MID values, but it should
     * not overlap/interfere with real MIDs used by other apps.
     */
    CFE_FT_CMD_MSGID = CFE_SB_ValueToMsgId(CFE_TEST_CMD_MID);
    CFE_FT_TLM_MSGID = CFE_SB_ValueToMsgId(CFE_TEST_HK_TLM_MID);

    UtTest_Add(TestBulkTransferSingle, NULL, NULL, "Single Thread Bulk Transfer");
    UtTest_Add(TestBulkTransferMulti2, NULL, NULL, "2 Thread Bulk Transfer");
    UtTest_Add(TestBulkTransferMulti4, NULL, NULL, "4 Thread Bulk Transfer");
}
