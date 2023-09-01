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

/* A simple command message */
typedef struct
{
    CFE_MSG_CommandHeader_t CommandHeader;
    uint32                  CmdPayload;
} CFE_FT_TestCmdMessage_t;

/* A simple telemetry message */
typedef struct
{
    CFE_MSG_TelemetryHeader_t TelemetryHeader;
    uint32                    TlmPayload;
} CFE_FT_TestTlmMessage_t;

/*
 * This test procedure should be agnostic to specific MID values, but it should
 * not overlap/interfere with real MIDs used by other apps.
 */
static const CFE_SB_MsgId_t CFE_FT_CMD_MSGID = CFE_SB_MSGID_WRAP_VALUE(CFE_TEST_CMD_MID);
static const CFE_SB_MsgId_t CFE_FT_TLM_MSGID = CFE_SB_MSGID_WRAP_VALUE(CFE_TEST_HK_TLM_MID);

void TestBulkTransmitRecv(void)
{
    CFE_SB_PipeId_t                PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                PipeId2 = CFE_SB_INVALID_PIPE;
    CFE_FT_TestCmdMessage_t        CmdMsg;
    CFE_FT_TestTlmMessage_t        TlmMsg;
    CFE_SB_Buffer_t *              MsgBuf;
    const CFE_FT_TestCmdMessage_t *CmdPtr;
    const CFE_FT_TestTlmMessage_t *TlmPtr;
    uint32                         SendCount;
    OS_time_t                      StartTime;
    OS_time_t                      ElapsedTime;

    memset(&CmdMsg, 0, sizeof(CmdMsg));
    memset(&TlmMsg, 0, sizeof(TlmMsg));

    UtPrintf("Testing: Bulk SB Transmit/Receive");
    CFE_PSP_GetTime(&StartTime);

    /* Setup, create a pipe and subscribe (one cmd, one tlm) */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 5, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 5, "TestPipe2"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId1, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_TLM_MSGID, PipeId2, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);

    /* Initialize the message content */
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(CmdMsg.CommandHeader), CFE_FT_CMD_MSGID, sizeof(CmdMsg)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(TlmMsg.TelemetryHeader), CFE_FT_TLM_MSGID, sizeof(TlmMsg)), CFE_SUCCESS);

    for (SendCount = 0; SendCount < 1000000; ++SendCount)
    {
        CmdMsg.CmdPayload = SendCount;
        TlmMsg.TlmPayload = ~SendCount;

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
        if (CmdPtr->CmdPayload != CmdMsg.CmdPayload)
        {
            UtAssert_UINT32_EQ(CmdPtr->CmdPayload, CmdMsg.CmdPayload);
            break;
        }

        CFE_Assert_STATUS_STORE(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, CFE_SB_POLL));
        if (!CFE_Assert_STATUS_SILENTCHECK(CFE_SUCCESS))
        {
            break;
        }

        TlmPtr = (const void *)MsgBuf;
        if (TlmPtr->TlmPayload != TlmMsg.TlmPayload)
        {
            UtAssert_UINT32_EQ(TlmPtr->TlmPayload, TlmMsg.TlmPayload);
            break;
        }

        /* report progress periodically */
        if ((SendCount % 50000) == 0)
        {
            UtPrintf("Success after %lu messages", (unsigned long)SendCount);
        }
    }

    CFE_PSP_GetTime(&ElapsedTime);
    ElapsedTime = OS_TimeSubtract(ElapsedTime, StartTime);

    UtAssert_MIR("Elapsed time for SB bulk message test: %lu usec",
                 (unsigned long)OS_TimeGetTotalMicroseconds(ElapsedTime));
}

void SBPerformanceTestSetup(void)
{
    UtTest_Add(TestBulkTransmitRecv, NULL, NULL, "Test Bulk SB Transmit/Receive");
}
