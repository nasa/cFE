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
 * Functional test of SB transmit/receive APIs
 * CFE_SB_TransmitMsg - Transmit a message.
 * CFE_SB_ReceiveBuffer - Receive a message from a software bus pipe.
 * CFE_SB_AllocateMessageBuffer - Get a buffer pointer to use for "zero copy" SB sends.
 * CFE_SB_ReleaseMessageBuffer - Release an unused "zero copy" buffer pointer.
 * CFE_SB_TransmitBuffer - Transmit a buffer.
 */

#include "cfe_test.h"
#include "cfe_msgids.h"
#include "cfe_test_msgids.h"
#include "cfe_test_msg.h"

#define CFE_FT_STRINGBUF_SIZE 12

/* A message intended to be (overall) larger than the CFE_MISSION_SB_MAX_SB_MSG_SIZE */
typedef union
{
    CFE_MSG_Message_t         Hdr;
    CFE_MSG_CommandHeader_t   CommandHeader;
    CFE_MSG_TelemetryHeader_t TelemetryHeader;
    uint8                     MaxSize[CFE_MISSION_SB_MAX_SB_MSG_SIZE + 16];
    char                      StringBuffer[CFE_FT_STRINGBUF_SIZE];
} CFE_FT_TestBigMessage_t;

/*
 * This test procedure should be agnostic to specific MID values, but it should
 * not overlap/interfere with real MIDs used by other apps.
 */
static CFE_SB_MsgId_t CFE_FT_CMD_MSGID;
static CFE_SB_MsgId_t CFE_FT_TLM_MSGID;

static CFE_FT_TestBigMessage_t CFE_FT_BigMsg;

void TestBasicTransmitRecv(void)
{
    CFE_SB_PipeId_t                    PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                    PipeId2 = CFE_SB_INVALID_PIPE;
    CFE_TEST_TestCmdMessage64_t        CmdMsg;
    CFE_TEST_TestTlmMessage64_t        TlmMsg;
    CFE_SB_MsgId_t                     MsgId;
    CFE_MSG_SequenceCount_t            Seq1, Seq2;
    CFE_SB_Buffer_t *                  MsgBuf;
    const CFE_TEST_TestCmdMessage64_t *CmdPtr;
    const CFE_TEST_TestTlmMessage64_t *TlmPtr;

    memset(&CmdMsg, 0, sizeof(CmdMsg));
    memset(&TlmMsg, 0, sizeof(TlmMsg));

    UtPrintf("Testing: CFE_SB_TransmitMsg");

    /* Setup, create a pipe and subscribe (one cmd, one tlm) */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 5, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 5, "TestPipe2"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId1, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_TLM_MSGID, PipeId2, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);

    /* Initialize the message content */
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(CmdMsg.CommandHeader), CFE_FT_CMD_MSGID, sizeof(CmdMsg)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(TlmMsg.TelemetryHeader), CFE_FT_TLM_MSGID, sizeof(TlmMsg)), CFE_SUCCESS);

    CFE_MSG_SetSequenceCount(CFE_MSG_PTR(CmdMsg.CommandHeader), 11);
    CFE_MSG_SetSequenceCount(CFE_MSG_PTR(TlmMsg.TelemetryHeader), 21);

    /* Sending with sequence update should ignore the sequence in the msg struct */
    CmdMsg.Payload.Value = 0x0c0ffee;
    TlmMsg.Payload.Value = 0x0d00d1e;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmMsg.TelemetryHeader), true), CFE_SUCCESS);

    CmdMsg.Payload.Value = 0x1c0ffee;
    TlmMsg.Payload.Value = 0x1d00d1e;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmMsg.TelemetryHeader), true), CFE_SUCCESS);

    /* Sending without sequence update should use the sequence in the msg struct */
    CmdMsg.Payload.Value = 0x2c0ffee;
    TlmMsg.Payload.Value = 0x2d00d1e;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), false), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmMsg.TelemetryHeader), false), CFE_SUCCESS);

    /* Sending again should trigger MsgLimit errors on the pipe, however the call still returns CFE_SUCCESS */
    CmdMsg.Payload.Value = 0x3c0ffee;
    TlmMsg.Payload.Value = 0x3d00d1e;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(TlmMsg.TelemetryHeader), true), CFE_SUCCESS);

    /* Attempt to send a msg which does not have a valid msgid  */
    memset(&CFE_FT_BigMsg, 0xFF, sizeof(CFE_FT_BigMsg));
    CFE_MSG_SetSize(&CFE_FT_BigMsg.Hdr, sizeof(CFE_MSG_Message_t) + 4);

    CFE_Assert_STATUS_STORE(CFE_SB_TransmitMsg(&CFE_FT_BigMsg.Hdr, true));
    if (!CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS))
    {
        CFE_Assert_STATUS_MUST_BE(CFE_SB_BAD_ARGUMENT);
    }

    /* Attempt to send a msg which is too big */
    CFE_MSG_SetSize(&CFE_FT_BigMsg.Hdr, sizeof(CFE_FT_BigMsg));
    CFE_MSG_SetMsgId(&CFE_FT_BigMsg.Hdr, CFE_FT_CMD_MSGID);
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(&CFE_FT_BigMsg.Hdr, true), CFE_SB_MSG_TOO_BIG);

    /* Attempt to send a msg which is NULL */
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(NULL, true), CFE_SB_BAD_ARGUMENT);

    UtPrintf("Testing: CFE_SB_ReceiveBuffer");

    /* off nominal / bad arguments */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, CFE_SB_INVALID_PIPE, 100), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(NULL, PipeId1, 100), CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, -100), CFE_SB_BAD_ARGUMENT);

    /*
     * Note, the CFE_SB_TransmitMsg now adheres to the "IsOrigination" flag.
     * Thus, the sequence numbers should come back with the value from the Route (1-2)
     * rather than the value the message was filled with initially.
     *
     * Note this also utilizes the CFE_SB_PEND_FOREVER flag - if working correctly,
     * there should be a message in the queue, so it should not block.
     */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_PEND_FOREVER), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &Seq1), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0x0c0ffee);
    UtAssert_UINT32_EQ(Seq1, 1);

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_PEND_FOREVER), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &Seq1), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0x1c0ffee);
    UtAssert_UINT32_EQ(Seq1, 2);

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_PEND_FOREVER), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &Seq1), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0x2c0ffee);
    UtAssert_UINT32_EQ(Seq1, 11);

    /* Final should not be in the pipe, should have been rejected due to MsgLim */
    /* Must not use CFE_SB_PEND_FOREVER here, as this will cause the test to block */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, 100), CFE_SB_TIME_OUT);

    /*
     * For TLM, the CFE_SB_TransmitMsg obeys the "IncrementSequence" flag.
     * Thus, first message gets the reference point, next message should be one more.
     */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, 100), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &Seq1), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_TLM_MSGID);
    TlmPtr = (const CFE_TEST_TestTlmMessage64_t *)MsgBuf;
    UtAssert_UINT32_EQ(TlmPtr->Payload.Value, 0x0d00d1e);

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, 100), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &Seq2), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_TLM_MSGID);
    TlmPtr = (const CFE_TEST_TestTlmMessage64_t *)MsgBuf;
    UtAssert_UINT32_EQ(TlmPtr->Payload.Value, 0x1d00d1e);
    UtAssert_UINT32_EQ(Seq2, CFE_MSG_GetNextSequenceCount(Seq1));

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, 100), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &Seq2), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_TLM_MSGID);
    TlmPtr = (const CFE_TEST_TestTlmMessage64_t *)MsgBuf;
    UtAssert_UINT32_EQ(TlmPtr->Payload.Value, 0x2d00d1e);
    UtAssert_UINT32_EQ(Seq2, 21);

    /* Final should not be in the pipe, should have been rejected due to MsgLim */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, 100), CFE_SB_TIME_OUT);

    /* Cleanup */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId2), CFE_SUCCESS);
}

/*
 * Test distribution/broadcasting features (MsgLimit/PipeDepth enforcement, etc)
 *
 * Important to verify that although some receive pipes may have errors/limits, it should not affect
 * the transmit side nor should it affect delivery to pipes that do not have limit errors.
 */
void TestMsgBroadcast(void)
{
    CFE_SB_PipeId_t                    PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                    PipeId2 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                    PipeId3 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t                    PipeId4 = CFE_SB_INVALID_PIPE;
    CFE_TEST_TestCmdMessage64_t        CmdMsg;
    CFE_SB_MsgId_t                     MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_SB_Buffer_t *                  MsgBuf1;
    CFE_SB_Buffer_t *                  MsgBuf2;
    CFE_SB_Buffer_t *                  MsgBuf3;
    CFE_SB_Buffer_t *                  MsgBuf4;
    const CFE_TEST_TestCmdMessage64_t *CmdPtr;

    memset(&CmdMsg, 0, sizeof(CmdMsg));

    UtPrintf("Testing: MsgLimit enforcement");

    /* Setup - subscribe same MsgId to multiple different pipes with different limits */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 3, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 3, "TestPipe2"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId3, 3, "TestPipe3"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId4, 5, "TestPipe4"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId1, CFE_SB_DEFAULT_QOS, 1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId2, CFE_SB_DEFAULT_QOS, 2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId3, CFE_SB_DEFAULT_QOS, 4), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId4, CFE_SB_DEFAULT_QOS, 6), CFE_SUCCESS);

    /* Initialize the message content */
    UtAssert_INT32_EQ(CFE_MSG_Init(CFE_MSG_PTR(CmdMsg.CommandHeader), CFE_FT_CMD_MSGID, sizeof(CmdMsg)), CFE_SUCCESS);

    /* Make unique content in each message. Sending should always be successful. */
    CmdMsg.Payload.Value = 0xbabb1e00;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);
    CmdMsg.Payload.Value = 0xbabb1e01;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);
    CmdMsg.Payload.Value = 0xbabb1e02;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);
    CmdMsg.Payload.Value = 0xbabb1e03;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);

    /* Now receive 1st message from Pipes, actual msg should appear on all (no limit violations here) */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf1, PipeId1, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf2, PipeId2, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf3, PipeId3, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf4, PipeId4, CFE_SB_POLL), CFE_SUCCESS);

    /* All pipes should have gotten the same actual buffer */
    UtAssert_ADDRESS_EQ(MsgBuf1, MsgBuf2);
    UtAssert_ADDRESS_EQ(MsgBuf1, MsgBuf3);
    UtAssert_ADDRESS_EQ(MsgBuf1, MsgBuf4);

    /* Confirm content */
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf1->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf1;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0xbabb1e00);

    /* Now receive 2nd message from Pipes, should not appear on PipeId 1 due to MsgLimit */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf1, PipeId1, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf2, PipeId2, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf3, PipeId3, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf4, PipeId4, CFE_SB_POLL), CFE_SUCCESS);

    /* All pipes should have gotten the same actual buffer */
    UtAssert_ADDRESS_EQ(MsgBuf2, MsgBuf3);
    UtAssert_ADDRESS_EQ(MsgBuf2, MsgBuf4);

    /* Confirm content */
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf2->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf2;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0xbabb1e01);

    /* Now receive 3rd message from Pipes, should not appear on PipeId 1 or 2 due to MsgLimit */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf1, PipeId1, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf2, PipeId2, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf3, PipeId3, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf4, PipeId4, CFE_SB_POLL), CFE_SUCCESS);

    /* All pipes should have gotten the same actual buffer */
    UtAssert_ADDRESS_EQ(MsgBuf3, MsgBuf4);

    /* Confirm content */
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf3->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf3;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0xbabb1e02);

    /* Now receive 4th message from Pipes, should only appear on PipeId4 due PipeDepth limit on 3  */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf1, PipeId1, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf2, PipeId2, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf3, PipeId3, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf4, PipeId4, CFE_SB_POLL), CFE_SUCCESS);

    /* Confirm content */
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf4->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf4;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0xbabb1e03);

    UtPrintf("Testing: Unsubscribe single pipe");

    /* Now unsubscribe only one of the pipes, and confirm no messages delivered to that pipe */
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_CMD_MSGID, PipeId2), CFE_SUCCESS);

    /* Send two more messages */
    CmdMsg.Payload.Value = 0xbabb1e04;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);
    CmdMsg.Payload.Value = 0xbabb1e05;
    UtAssert_INT32_EQ(CFE_SB_TransmitMsg(CFE_MSG_PTR(CmdMsg.CommandHeader), true), CFE_SUCCESS);

    /* poll all pipes again, message should appear on all except PipeId2 (Unsubscribed) */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf1, PipeId1, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf2, PipeId2, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf3, PipeId3, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf4, PipeId4, CFE_SB_POLL), CFE_SUCCESS);

    /* All pipes should have gotten the same actual buffer */
    UtAssert_ADDRESS_EQ(MsgBuf1, MsgBuf3);
    UtAssert_ADDRESS_EQ(MsgBuf1, MsgBuf4);

    /* Confirm content */
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf1->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf1;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0xbabb1e04);

    /* poll all pipes again, message should appear on all except PipeId1 (MsgLim) or PipeId2 (Unsubscribed) */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf1, PipeId1, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf2, PipeId2, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf3, PipeId3, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf4, PipeId4, CFE_SB_POLL), CFE_SUCCESS);

    /* All pipes should have gotten the same actual buffer */
    UtAssert_ADDRESS_EQ(MsgBuf3, MsgBuf4);

    /* Confirm content */
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf3->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    CmdPtr = (const CFE_TEST_TestCmdMessage64_t *)MsgBuf3;
    UtAssert_UINT32_EQ(CmdPtr->Payload.Value, 0xbabb1e05);

    /* poll all pipes again, all should be empty now */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf1, PipeId1, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf2, PipeId2, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf3, PipeId3, CFE_SB_POLL), CFE_SB_NO_MESSAGE);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf4, PipeId4, CFE_SB_POLL), CFE_SB_NO_MESSAGE);

    /* Cleanup */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId3), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId4), CFE_SUCCESS);
}

/* This is a variant of the message transmit API that does not copy */
void TestZeroCopyTransmitRecv(void)
{
    CFE_SB_PipeId_t         PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t         PipeId2 = CFE_SB_INVALID_PIPE;
    CFE_SB_Buffer_t *       CmdBuf;
    CFE_SB_Buffer_t *       TlmBuf;
    CFE_SB_Buffer_t *       MsgBuf;
    CFE_SB_MsgId_t          MsgId = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_SequenceCount_t SeqCmd1;
    CFE_MSG_SequenceCount_t SeqTlm1;
    CFE_MSG_SequenceCount_t SeqCmd2;
    CFE_MSG_SequenceCount_t SeqTlm2;

    /* Setup, create a pipe and subscribe (one cmd, one tlm) */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 5, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 5, "TestPipe2"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId1, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_TLM_MSGID, PipeId2, CFE_SB_DEFAULT_QOS, 3), CFE_SUCCESS);

    UtPrintf("Testing: CFE_SB_AllocateMessageBuffer");

    /* Confirm bad size rejection */
    UtAssert_NULL(CFE_SB_AllocateMessageBuffer(CFE_MISSION_SB_MAX_SB_MSG_SIZE + 1));

    /* Nominal */
    UtAssert_NOT_NULL(CmdBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestCmdMessage64_t)));
    UtAssert_NOT_NULL(TlmBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestTlmMessage64_t)));

    UtPrintf("Testing: CFE_SB_ReleaseMessageBuffer");

    /* allocate a buffer but then discard it without sending */
    UtAssert_NOT_NULL(MsgBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_MSG_Message_t) + 4));
    UtAssert_INT32_EQ(CFE_SB_ReleaseMessageBuffer(MsgBuf), CFE_SUCCESS);

    /* Attempt to double-release, should fail validation */
    UtAssert_INT32_EQ(CFE_SB_ReleaseMessageBuffer(MsgBuf), CFE_SB_BUFFER_INVALID);

    /* Other bad input checking */
    UtAssert_INT32_EQ(CFE_SB_ReleaseMessageBuffer(NULL), CFE_SB_BAD_ARGUMENT);

    UtPrintf("Testing: CFE_SB_TransmitBuffer");

    /* Initialize the message content */
    UtAssert_INT32_EQ(CFE_MSG_Init(&CmdBuf->Msg, CFE_FT_CMD_MSGID, sizeof(CFE_TEST_TestCmdMessage64_t)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_Init(&TlmBuf->Msg, CFE_FT_TLM_MSGID, sizeof(CFE_TEST_TestTlmMessage64_t)), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(CmdBuf, true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(TlmBuf, true), CFE_SUCCESS);

    /* Attempt to send a buffer which has been released */
    UtAssert_NOT_NULL(MsgBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_MSG_Message_t) + 4));
    UtAssert_INT32_EQ(CFE_MSG_Init(&MsgBuf->Msg, CFE_FT_CMD_MSGID, sizeof(CFE_MSG_Message_t) + 4), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReleaseMessageBuffer(MsgBuf), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(MsgBuf, true), CFE_SB_BUFFER_INVALID);

    /* Attempt to send a NULL buffer */
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(NULL, true), CFE_SB_BAD_ARGUMENT);

    UtPrintf("Testing: CFE_SB_ReceiveBuffer");

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, 100), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_CMD_MSGID);
    UtAssert_ADDRESS_EQ(MsgBuf, CmdBuf); /* should be the same actual buffer (not a copy) */

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_POLL), CFE_SB_NO_MESSAGE);

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, 100), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&MsgBuf->Msg, &MsgId), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(MsgId, CFE_FT_TLM_MSGID);
    UtAssert_ADDRESS_EQ(MsgBuf, TlmBuf); /* should be the same actual buffer (not a copy) */

    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, CFE_SB_POLL), CFE_SB_NO_MESSAGE);

    /* Attempt to send a msg of maximum size */
    UtAssert_NOT_NULL(CmdBuf = CFE_SB_AllocateMessageBuffer(CFE_MISSION_SB_MAX_SB_MSG_SIZE));

    /* First initialize to indicate its even bigger than the max (should fail to transmit) */
    UtAssert_INT32_EQ(CFE_MSG_Init(&CmdBuf->Msg, CFE_FT_CMD_MSGID, sizeof(CFE_FT_BigMsg)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(CmdBuf, true), CFE_SB_MSG_TOO_BIG);

    /* reducing size should make it work */
    UtAssert_INT32_EQ(CFE_MSG_SetSize(&CmdBuf->Msg, CFE_MISSION_SB_MAX_SB_MSG_SIZE), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(CmdBuf, true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_ADDRESS_EQ(MsgBuf, CmdBuf); /* should be the same actual buffer (not a copy) */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_POLL), CFE_SB_NO_MESSAGE);

    UtPrintf("Testing: CFE_SB_TransmitBuffer sequence number updates");

    /* Send a set of messages with and without sequence number update flag */
    UtAssert_NOT_NULL(CmdBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestCmdMessage64_t)));
    UtAssert_NOT_NULL(TlmBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestTlmMessage64_t)));
    UtAssert_INT32_EQ(CFE_MSG_Init(&CmdBuf->Msg, CFE_FT_CMD_MSGID, sizeof(CFE_TEST_TestCmdMessage64_t)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_Init(&TlmBuf->Msg, CFE_FT_TLM_MSGID, sizeof(CFE_TEST_TestTlmMessage64_t)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&CmdBuf->Msg, 1234), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&TlmBuf->Msg, 5678), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(CmdBuf, true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(TlmBuf, true), CFE_SUCCESS);

    /* Receive and get initial sequence count */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &SeqCmd1), CFE_SUCCESS);
    UtAssert_UINT32_EQ(SeqCmd1, 6); /* NOTE: commands now honor "Update" flag */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &SeqTlm1), CFE_SUCCESS);

    /* Send a second message also with increment = true and confirm value */
    UtAssert_NOT_NULL(CmdBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestCmdMessage64_t)));
    UtAssert_NOT_NULL(TlmBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestTlmMessage64_t)));
    UtAssert_INT32_EQ(CFE_MSG_Init(&CmdBuf->Msg, CFE_FT_CMD_MSGID, sizeof(CFE_TEST_TestCmdMessage64_t)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_Init(&TlmBuf->Msg, CFE_FT_TLM_MSGID, sizeof(CFE_TEST_TestTlmMessage64_t)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&CmdBuf->Msg, 1234), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&TlmBuf->Msg, 5678), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(CmdBuf, true), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(TlmBuf, true), CFE_SUCCESS);

    /* Receive and get current sequence count */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &SeqCmd2), CFE_SUCCESS);
    UtAssert_UINT32_EQ(SeqCmd2, 7); /* NOTE: commands now honor "Update" flag */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &SeqTlm2), CFE_SUCCESS);
    UtAssert_UINT32_EQ(SeqTlm2, CFE_MSG_GetNextSequenceCount(SeqTlm1)); /* should be +1 from the previous */

    /* Send a third message also with increment = false and confirm value */
    UtAssert_NOT_NULL(CmdBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestCmdMessage64_t)));
    UtAssert_NOT_NULL(TlmBuf = CFE_SB_AllocateMessageBuffer(sizeof(CFE_TEST_TestTlmMessage64_t)));
    UtAssert_INT32_EQ(CFE_MSG_Init(&CmdBuf->Msg, CFE_FT_CMD_MSGID, sizeof(CFE_TEST_TestCmdMessage64_t)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_Init(&TlmBuf->Msg, CFE_FT_TLM_MSGID, sizeof(CFE_TEST_TestTlmMessage64_t)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&CmdBuf->Msg, 1234), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_SetSequenceCount(&TlmBuf->Msg, 5678), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(CmdBuf, false), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_TransmitBuffer(TlmBuf, false), CFE_SUCCESS);

    /* Receive and get sequence count, should NOT be incremented from previous */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId1, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &SeqCmd1), CFE_SUCCESS);
    UtAssert_UINT32_EQ(SeqCmd1, 1234); /* should match initialized value */
    UtAssert_INT32_EQ(CFE_SB_ReceiveBuffer(&MsgBuf, PipeId2, CFE_SB_POLL), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetSequenceCount(&MsgBuf->Msg, &SeqTlm1), CFE_SUCCESS);
    UtAssert_UINT32_EQ(SeqTlm1, 5678); /* should match initialized value */

    /* Cleanup */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId2), CFE_SUCCESS);
}

void TestMessageUserDataAccess(void)
{
    CFE_TEST_TestCmdMessage64_t CmdMsg;
    CFE_TEST_TestTlmMessage64_t TlmMsg;

    /* Test CFE_SB_GetUserData */
    UtAssert_INT32_EQ(
        CFE_MSG_Init(CFE_MSG_PTR(CmdMsg.CommandHeader), CFE_FT_CMD_MSGID, sizeof(CFE_TEST_TestCmdMessage64_t)),
        CFE_SUCCESS);
    UtAssert_INT32_EQ(
        CFE_MSG_Init(CFE_MSG_PTR(TlmMsg.TelemetryHeader), CFE_FT_TLM_MSGID, sizeof(CFE_TEST_TestTlmMessage64_t)),
        CFE_SUCCESS);

    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(CFE_MSG_PTR(CmdMsg.CommandHeader)), &CmdMsg.Payload);
    UtAssert_ADDRESS_EQ(CFE_SB_GetUserData(CFE_MSG_PTR(TlmMsg.TelemetryHeader)), &TlmMsg.Payload);

    UtAssert_UINT32_EQ(CFE_SB_GetUserDataLength(CFE_MSG_PTR(CmdMsg.CommandHeader)), sizeof(CmdMsg.Payload));
    UtAssert_UINT32_EQ(CFE_SB_GetUserDataLength(CFE_MSG_PTR(TlmMsg.TelemetryHeader)), sizeof(TlmMsg.Payload));

    /* The size should be settable to something less, if necessary */
    CFE_SB_SetUserDataLength(CFE_MSG_PTR(CmdMsg.CommandHeader), sizeof(CmdMsg.Payload) - 2);
    CFE_SB_SetUserDataLength(CFE_MSG_PTR(TlmMsg.TelemetryHeader), sizeof(TlmMsg.Payload) - 2);
    UtAssert_UINT32_EQ(CFE_SB_GetUserDataLength(CFE_MSG_PTR(CmdMsg.CommandHeader)), sizeof(CmdMsg.Payload) - 2);
    UtAssert_UINT32_EQ(CFE_SB_GetUserDataLength(CFE_MSG_PTR(TlmMsg.TelemetryHeader)), sizeof(TlmMsg.Payload) - 2);

    /* Bad inputs */
    UtAssert_NULL(CFE_SB_GetUserData(NULL));
    UtAssert_NULL(CFE_SB_GetUserData(NULL));
    UtAssert_ZERO(CFE_SB_GetUserDataLength(NULL));
    UtAssert_ZERO(CFE_SB_GetUserDataLength(NULL));
}

void TestMiscMessageUtils(void)
{
    char       TestString[CFE_FT_STRINGBUF_SIZE + 4];
    const char RefString1[] = "abc";
    const char RefString2[] = "defg";

    memset(&CFE_FT_BigMsg, 'x', sizeof(CFE_FT_BigMsg));
    memset(TestString, 'y', sizeof(TestString));

    /* nominal CFE_SB_MessageStringGet */
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(TestString, CFE_FT_BigMsg.StringBuffer, RefString1, sizeof(TestString),
                                              sizeof(CFE_FT_BigMsg.StringBuffer)),
                      CFE_FT_STRINGBUF_SIZE);

    /* The result should be null terminated, even if the input was not */
    UtAssert_ZERO(TestString[CFE_FT_STRINGBUF_SIZE]);
    UtAssert_STRINGBUF_EQ(TestString, sizeof(TestString), CFE_FT_BigMsg.StringBuffer,
                          sizeof(CFE_FT_BigMsg.StringBuffer));

    /* No default */
    memset(&CFE_FT_BigMsg, 'w', sizeof(CFE_FT_BigMsg));
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(TestString, CFE_FT_BigMsg.StringBuffer, NULL, sizeof(TestString),
                                              sizeof(CFE_FT_BigMsg.StringBuffer)),
                      CFE_FT_STRINGBUF_SIZE);
    UtAssert_STRINGBUF_EQ(TestString, sizeof(TestString), CFE_FT_BigMsg.StringBuffer,
                          sizeof(CFE_FT_BigMsg.StringBuffer));
    UtAssert_ZERO(TestString[CFE_FT_STRINGBUF_SIZE]);
    UtAssert_STRINGBUF_EQ(TestString, sizeof(TestString), CFE_FT_BigMsg.StringBuffer,
                          sizeof(CFE_FT_BigMsg.StringBuffer));

    /* Check if the input is empty */
    memset(&CFE_FT_BigMsg, 0, sizeof(CFE_FT_BigMsg));
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(TestString, CFE_FT_BigMsg.StringBuffer, RefString1, sizeof(TestString),
                                              sizeof(CFE_FT_BigMsg.StringBuffer)),
                      sizeof(RefString1) - 1);
    UtAssert_STRINGBUF_EQ(TestString, sizeof(TestString), RefString1, sizeof(RefString1));
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(TestString, NULL, RefString2, sizeof(TestString), 0),
                      sizeof(RefString2) - 1);
    UtAssert_STRINGBUF_EQ(TestString, sizeof(TestString), RefString2, sizeof(RefString2));

    /* Neither source nor default */
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(TestString, NULL, NULL, sizeof(TestString), 0), CFE_SUCCESS);
    UtAssert_STRINGBUF_EQ(TestString, sizeof(TestString), "", 1);

    /* bad inputs */
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(NULL, CFE_FT_BigMsg.StringBuffer, RefString1, sizeof(TestString),
                                              sizeof(CFE_FT_BigMsg.StringBuffer)),
                      CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_MessageStringGet(TestString, CFE_FT_BigMsg.StringBuffer, RefString1, 0,
                                              sizeof(CFE_FT_BigMsg.StringBuffer)),
                      CFE_SB_BAD_ARGUMENT);

    /* nominal CFE_SB_MessageStringSet */
    memset(TestString, 'z', sizeof(TestString));
    UtAssert_INT32_EQ(CFE_SB_MessageStringSet(CFE_FT_BigMsg.StringBuffer, TestString,
                                              sizeof(CFE_FT_BigMsg.StringBuffer), sizeof(TestString)),
                      CFE_FT_STRINGBUF_SIZE);
    UtAssert_STRINGBUF_EQ(TestString, CFE_FT_STRINGBUF_SIZE, CFE_FT_BigMsg.StringBuffer, CFE_FT_STRINGBUF_SIZE);

    /* The result should NOT be null terminated (fixed size msg string does not need it) */
    UtAssert_INT32_EQ(CFE_FT_BigMsg.StringBuffer[sizeof(CFE_FT_BigMsg.StringBuffer) - 1], 'z');
    /* Should not have overwritten anything beyond the fixed buffer */
    UtAssert_ZERO(CFE_FT_BigMsg.MaxSize[sizeof(CFE_FT_BigMsg.StringBuffer)]);

    /* bad inputs */
    UtAssert_INT32_EQ(CFE_SB_MessageStringSet(NULL, TestString, sizeof(CFE_FT_BigMsg.StringBuffer), sizeof(TestString)),
                      CFE_SB_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_SB_MessageStringSet(CFE_FT_BigMsg.StringBuffer, NULL, sizeof(CFE_FT_BigMsg.StringBuffer),
                                              sizeof(TestString)),
                      CFE_SB_BAD_ARGUMENT);
}

void SBSendRecvTestSetup(void)
{
    CFE_FT_CMD_MSGID = CFE_SB_ValueToMsgId(CFE_TEST_CMD_MID);
    CFE_FT_TLM_MSGID = CFE_SB_ValueToMsgId(CFE_TEST_HK_TLM_MID);

    UtTest_Add(TestBasicTransmitRecv, NULL, NULL, "Test Basic Transmit/Receive");
    UtTest_Add(TestZeroCopyTransmitRecv, NULL, NULL, "Test Zero Copy Transmit/Receive");
    UtTest_Add(TestMsgBroadcast, NULL, NULL, "Test Msg Broadcast");
    UtTest_Add(TestMiscMessageUtils, NULL, NULL, "Test Miscellaneous Message Utility APIs");
    UtTest_Add(TestMessageUserDataAccess, NULL, NULL, "Test Message UserData APIs");
}
