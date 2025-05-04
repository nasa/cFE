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
 * \file
 *   Functional test of Message ID APIs
 *
 *   Demonstration....
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestMsgId(void)
{
    UtPrintf("Testing: CFE_MSG_SetMsgId, CFE_MSG_GetMsgId");
    CFE_MSG_Message_t msg;
    CFE_SB_MsgId_t    msgid;
    CFE_SB_MsgId_t    expectedmsgid = CFE_SB_ValueToMsgId(1);

    UtAssert_INT32_EQ(CFE_MSG_SetMsgId(&msg, expectedmsgid), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    CFE_Assert_MSGID_EQ(msgid, expectedmsgid);

    UtAssert_INT32_EQ(CFE_MSG_SetMsgId(NULL, msgid), CFE_MSG_BAD_ARGUMENT);

    /*
     * The purpose of this test is to attempt to set a MsgId beyond the set of values that can
     * be stored in the MSG header. However the criteria for being "storable" depends on the
     * actual MSG implementation, and may be different in other implementations.  Currently both
     * "v1" and "v2" implementations do define a specific highest MsgId value, but another
     * implementation might not have a highest number concept at all.
     *
     * By passing the value of -1, when converted to a an unsigned value (either 16 or 32 bit)
     * it should translate to a MsgId value with all bits being set.  In theory, at least some of
     * those bits will be not mappable to the packet header bits, and it should therefore elicit
     * the CFE_MSG_BAD_ARGUMENT response.
     */
    UtAssert_INT32_EQ(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(-1)), CFE_MSG_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(NULL, &msgid), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_GetMsgId(&msg, NULL), CFE_MSG_BAD_ARGUMENT);
}

void TestGetTypeFromMsgId(void)
{
    UtPrintf("Testing: CFE_MSG_GetTypeFromMsgId");
    CFE_SB_MsgId_t msgid = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_Type_t msgtype;
    int32          status;

    /*
     * Response not verified because msgid 0 could be out of range based on implementation and
     * the msg to type relationship is also implementation defined, black box test just calls the routine
     * to confirm things don't "break" with full range values and the implementation exists.
     */

    status = CFE_MSG_GetTypeFromMsgId(msgid, &msgtype);
    UtAssert_True(status == CFE_SUCCESS || status == CFE_MSG_BAD_ARGUMENT, "CFE_MSG_GetTypeFromMsgId() == (%ld)",
                  (long)status);

    memset(&msgid, 0xFF, sizeof(msgid));
    status = CFE_MSG_GetTypeFromMsgId(msgid, &msgtype);
    UtAssert_True(status == CFE_SUCCESS || status == CFE_MSG_BAD_ARGUMENT, "CFE_MSG_GetTypeFromMsgId() == (%ld)",
                  (long)status);

    UtAssert_INT32_EQ(CFE_MSG_GetTypeFromMsgId(msgid, NULL), CFE_MSG_BAD_ARGUMENT);
}

void MessageIdTestSetup(void)
{
    UtTest_Add(TestMsgId, NULL, NULL, "Test Set/Get Message ID");
    UtTest_Add(TestGetTypeFromMsgId, NULL, NULL, "Test Get Type From Message ID");
}
