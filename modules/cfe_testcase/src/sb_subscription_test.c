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
 * Functional test of SB subscription APIs
 * CFE_SB_Subscribe - Subscribe to a message on the software bus with default parameters.
 * CFE_SB_SubscribeEx - Subscribe to a message on the software bus.
 * CFE_SB_SubscribeLocal - Subscribe to a message while keeping the request local to a cpu.
 * CFE_SB_Unsubscribe - Remove a subscription to a message on the software bus.
 * CFE_SB_UnsubscribeLocal - Remove a subscription to a message on the software bus on the current CPU.
 */

#include "cfe_test.h"
#include "cfe_msgids.h"
#include "cfe_test_msgids.h"

/*
 * This test procedure should be agnostic to specific MID values, but it should
 * not overlap/interfere with real MIDs used by other apps.
 */
static CFE_SB_MsgId_t CFE_FT_CMD_MSGID;
static CFE_SB_MsgId_t CFE_FT_TLM_MSGID;

void TestSubscribeUnsubscribe(void)
{
    CFE_SB_PipeId_t PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2 = CFE_SB_INVALID_PIPE;

    UtPrintf("Testing: CFE_SB_Subscribe, CFE_SB_Unsubscribe");

    /* Setup, create some pipes */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 3, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 3, "TestPipe2"), CFE_SUCCESS);

    /* Subscribe - Confirm Bad MsgId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_SB_INVALID_MSG_ID, PipeId1), CFE_SB_BAD_ARGUMENT);

    /* Subscribe - Confirm Bad PipeId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_CMD_MSGID, CFE_SB_INVALID_PIPE), CFE_SB_BAD_ARGUMENT);

    /* Subscribe - Nominal */
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_CMD_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_TLM_MSGID, PipeId2), CFE_SUCCESS);

    /* Subscribe - Duplicate */
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_CMD_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_TLM_MSGID, PipeId2), CFE_SUCCESS);

    /* Subscribe - Nominal 2 */
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_TLM_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_CMD_MSGID, PipeId2), CFE_SUCCESS);

    /* Unsubscribe - Confirm Bad MsgId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_SB_INVALID_MSG_ID, PipeId1), CFE_SB_BAD_ARGUMENT);

    /* Unsubscribe - Confirm Bad PipeId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_CMD_MSGID, CFE_SB_INVALID_PIPE), CFE_SB_BAD_ARGUMENT);

    /* Unsubscribe - Nominal */
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_CMD_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_TLM_MSGID, PipeId2), CFE_SUCCESS);

    /* Unsubscribe - Already unsubscribed */
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_CMD_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_TLM_MSGID, PipeId2), CFE_SUCCESS);

    /* Unsubscribe - Nominal 2 */
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_TLM_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_Unsubscribe(CFE_FT_CMD_MSGID, PipeId2), CFE_SUCCESS);

    /* Teardown - delete the pipes. */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId2), CFE_SUCCESS);
}

void TestSubscribeUnsubscribeLocal(void)
{
    CFE_SB_PipeId_t PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2 = CFE_SB_INVALID_PIPE;

    UtPrintf("Testing: CFE_SB_SubscribeLocal, CFE_SB_UnsubscribeLocal");

    /* Setup, create some pipes */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 3, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 3, "TestPipe2"), CFE_SUCCESS);

    /* Subscribe - Confirm Bad MsgId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_SB_INVALID_MSG_ID, PipeId1, 2), CFE_SB_BAD_ARGUMENT);

    /* Subscribe - Confirm Bad PipeId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_CMD_MSGID, CFE_SB_INVALID_PIPE, 2), CFE_SB_BAD_ARGUMENT);

    /* Subscribe - Nominal */
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_CMD_MSGID, PipeId1, 2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_TLM_MSGID, PipeId2, 2), CFE_SUCCESS);

    /* Subscribe - Duplicate */
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_CMD_MSGID, PipeId1, 2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_TLM_MSGID, PipeId2, 2), CFE_SUCCESS);

    /* Subscribe - Nominal 2 */
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_TLM_MSGID, PipeId1, 2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_CMD_MSGID, PipeId2, 2), CFE_SUCCESS);

    /* Unsubscribe - Confirm Bad MsgId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_SB_INVALID_MSG_ID, PipeId1), CFE_SB_BAD_ARGUMENT);

    /* Unsubscribe - Confirm Bad PipeId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_FT_CMD_MSGID, CFE_SB_INVALID_PIPE), CFE_SB_BAD_ARGUMENT);

    /* Unsubscribe - Nominal */
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_FT_CMD_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_FT_TLM_MSGID, PipeId2), CFE_SUCCESS);

    /* Unsubscribe - Already unsubscribed */
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_FT_CMD_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_FT_TLM_MSGID, PipeId2), CFE_SUCCESS);

    /* Unsubscribe - Nominal 2 */
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_FT_TLM_MSGID, PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_UnsubscribeLocal(CFE_FT_CMD_MSGID, PipeId2), CFE_SUCCESS);

    /* Teardown - delete the pipes. */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId2), CFE_SUCCESS);
}

void TestSubscribeEx(void)
{
    CFE_SB_PipeId_t PipeId1 = CFE_SB_INVALID_PIPE;
    CFE_SB_PipeId_t PipeId2 = CFE_SB_INVALID_PIPE;

    /*
     * NOTE: CFE_SB_SubscribeEx adds a "Quality" parameter for Qos, which is
     * not utilized in the current SB implementation (only "default" is available).
     *
     * Otherwise, it is basically the same as the regular "CFE_SB_Subscribe" call.
     */
    UtPrintf("Testing: CFE_SB_SubscribeEx");

    /* Setup, create some pipes */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId1, 3, "TestPipe1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId2, 3, "TestPipe2"), CFE_SUCCESS);

    /*
     * For completeness, repeat the bad arg rejection tests, even though internally
     * it is likely identical (all going through CFE_SB_SubscribeFull).  However
     * this implementation detail should not be assumed in a black box test environment.
     */

    /* Subscribe - Confirm Bad MsgId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_SB_INVALID_MSG_ID, PipeId1, CFE_SB_DEFAULT_QOS, 2), CFE_SB_BAD_ARGUMENT);

    /* Subscribe - Confirm Bad PipeId Arg Rejection */
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, CFE_SB_INVALID_PIPE, CFE_SB_DEFAULT_QOS, 2),
                      CFE_SB_BAD_ARGUMENT);

    /* Subscribe - Nominal */
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId1, CFE_SB_DEFAULT_QOS, 2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_TLM_MSGID, PipeId2, CFE_SB_DEFAULT_QOS, 2), CFE_SUCCESS);

    /* Teardown - delete the pipes (this also unsubscribes automatically) */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId2), CFE_SUCCESS);
}

void TestSBMaxSubscriptions(void)
{
    CFE_Status_t    Status;
    uint32          NumSubs;
    CFE_SB_MsgId_t  TestMsgId;
    CFE_SB_PipeId_t PipeId = CFE_SB_INVALID_PIPE;

    UtPrintf("Testing: CFE_SB_Subscribe, max routing table limit");

    /* Setup, create a pipe */
    UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId, 2, "TestPipe"), CFE_SUCCESS);

    /*
     * Test max subscriptions - note this depends on the actual number of _other_ apps
     * that are running and have made other subscriptions.  Therefore it is not predictable
     * exactly how many iterations of this loop will succeed, but it should be fewer than
     * CFE_PLATFORM_SB_MAX_MSG_IDS.
     *
     * NOTE: after this loop, it may not be possible to add any more routes at all, until
     * a processor reset is done, because routes are never actually deleted for sequence number persistence.
     */
    NumSubs = 0;
    while (NumSubs <= CFE_PLATFORM_SB_MAX_MSG_IDS)
    {
        /* fabricate a msgid to subscribe to (this may overlap real msgids) */
        TestMsgId = CFE_SB_ValueToMsgId(1 + NumSubs);

        Status = CFE_SB_Subscribe(TestMsgId, PipeId);
        if (Status != CFE_SUCCESS)
        {
            break;
        }

        ++NumSubs;
    }

    /* Check that the number of subscriptions was within range */
    UtAssert_NONZERO(NumSubs);
    UtAssert_UINT32_LTEQ(NumSubs, CFE_PLATFORM_SB_MAX_MSG_IDS);

    /* The last pass should have returned CFE_SB_MAX_MSGS_MET */
    UtAssert_True(Status == CFE_SB_MAX_MSGS_MET,
                  "CFE_SB_Subscribe(TestMsgId, PipeId1) (%ld) == CFE_SB_MAX_MSGS_MET (%ld)", (long)Status,
                  (long)CFE_SB_MAX_MSGS_MET);

    /* Note this should also remove any subscriptions from the above loop */
    UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId), CFE_SUCCESS);
}

/* This is a different flavor of the subscription limit - a single msgid can only
 * have up to CFE_PLATFORM_SB_MAX_DEST_PER_PKT destinations */
void TestSBMaxDestinations(void)
{
    CFE_SB_PipeId_t PipeId[CFE_PLATFORM_SB_MAX_DEST_PER_PKT + 1];
    char            PipeName[CFE_MISSION_MAX_API_LEN];
    uint32          NumDests;

    UtPrintf("Testing: CFE_SB_Subscribe, max destination limit");

    NumDests = 0;
    while (NumDests <= CFE_PLATFORM_SB_MAX_DEST_PER_PKT)
    {
        snprintf(PipeName, sizeof(PipeName), "TestPipe%u", (unsigned int)NumDests);
        if (!UtAssert_INT32_EQ(CFE_SB_CreatePipe(&PipeId[NumDests], 2, PipeName), CFE_SUCCESS))
        {
            break;
        }

        if (NumDests == CFE_PLATFORM_SB_MAX_DEST_PER_PKT)
        {
            /* All 3 variations of subscribe can be checked here, they should all fail identically */
            UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_CMD_MSGID, PipeId[NumDests]), CFE_SB_MAX_DESTS_MET);
            UtAssert_INT32_EQ(CFE_SB_SubscribeEx(CFE_FT_CMD_MSGID, PipeId[NumDests], CFE_SB_DEFAULT_QOS, 2),
                              CFE_SB_MAX_DESTS_MET);
            UtAssert_INT32_EQ(CFE_SB_SubscribeLocal(CFE_FT_CMD_MSGID, PipeId[NumDests], 2), CFE_SB_MAX_DESTS_MET);
        }
        else
        {
            UtAssert_INT32_EQ(CFE_SB_Subscribe(CFE_FT_CMD_MSGID, PipeId[NumDests]), CFE_SUCCESS);
        }

        ++NumDests;
    }

    while (NumDests > 0)
    {
        --NumDests;

        /* Note this should also remove any subscriptions from the above loop */
        UtAssert_INT32_EQ(CFE_SB_DeletePipe(PipeId[NumDests]), CFE_SUCCESS);
    }
}

void SBSubscriptionTestSetup(void)
{
    CFE_FT_CMD_MSGID = CFE_SB_ValueToMsgId(CFE_TEST_CMD_MID);
    CFE_FT_TLM_MSGID = CFE_SB_ValueToMsgId(CFE_TEST_HK_TLM_MID);

    UtTest_Add(TestSubscribeUnsubscribe, NULL, NULL, "Test SB Subscribe/Unsubscribe");
    UtTest_Add(TestSubscribeUnsubscribeLocal, NULL, NULL, "Test SB SubscribeLocal/UnsubscribeLocal");
    UtTest_Add(TestSubscribeEx, NULL, NULL, "Test SB SubscribeEx");
    UtTest_Add(TestSBMaxDestinations, NULL, NULL, "Test SB Max Destinations");

    /*
     * NOTE: The TestSBMaxSubscriptions() is not included/added by default, as it will fill the
     * routing table and make it not possible to add new routes until the system is reset.
     *
     * The test can be optionally enabled by the user and should pass, if this is not a concern.
     */
}
