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
 * Test MsgId V1 accessors
 */

#include "utassert.h"
#include "ut_support.h"
#include "cfe_msg.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "test_cfe_msg_msgid.h"
#include "cfe_error.h"
#include <string.h>

#define TEST_MAX_APID 0x7FF

void Test_MSG_MsgId(void)
{
    CFE_MSG_Message_t msg;
    CFE_SB_MsgId_t    msgid = CFE_SB_ValueToMsgId(1);
    CFE_MSG_Type_t    type;
    CFE_MSG_ApId_t    apid;
    bool              hassec;

    UtPrintf("Bad parameter tests, Null pointers and invalid (max valid + 1)");
    memset(&msg, 0, sizeof(msg));
    ASSERT_EQ(CFE_MSG_GetMsgId(NULL, &msgid), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid), 1);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, NULL), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(Test_MSG_NotZero(&msg), 0);
    ASSERT_EQ(CFE_MSG_SetMsgId(NULL, msgid), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, CFE_SB_INVALID_MSG_ID), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(Test_MSG_NotZero(&msg), 0);
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(Test_MSG_NotZero(&msg), 0);
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, 0xFFFF), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(Test_MSG_NotZero(&msg), 0);

    UtPrintf("Set msg to all F's, set msgid to 0 and verify");
    memset(&msg, 0xFF, sizeof(msg));
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid), 0xFFFF);
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, 0), CFE_SUCCESS);
    UT_DisplayPkt(&msg, sizeof(msg));
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid), 0);
    ASSERT_EQ(Test_MSG_NotF(&msg), MSG_HDRVER_FLAG | MSG_APID_FLAG | MSG_TYPE_FLAG | MSG_HASSEC_FLAG);

    UtPrintf("Set msg to all 0, set msgid to max and verify");
    memset(&msg, 0, sizeof(msg));
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid), 0);
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID), CFE_SUCCESS);
    UT_DisplayPkt(&msg, sizeof(msg));
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid), CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    ASSERT_EQ(Test_MSG_NotZero(&msg), MSG_APID_FLAG | MSG_TYPE_FLAG | MSG_HASSEC_FLAG);
    ASSERT_EQ(CFE_MSG_GetApId(&msg, &apid), CFE_SUCCESS);
    ASSERT_EQ(apid, TEST_MAX_APID);
    ASSERT_EQ(CFE_MSG_GetType(&msg, &type), CFE_SUCCESS);
    ASSERT_EQ(type, CFE_MSG_Type_Cmd);
    ASSERT_EQ(CFE_MSG_GetHasSecondaryHeader(&msg, &hassec), CFE_SUCCESS);
    ASSERT_EQ(hassec, true);

    UtPrintf("Set ApId msgid bits only and verify");
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(TEST_MAX_APID)), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(Test_MSG_NotZero(&msg), MSG_APID_FLAG);
    ASSERT_EQ(CFE_MSG_GetApId(&msg, &apid), CFE_SUCCESS);
    ASSERT_EQ(apid, TEST_MAX_APID);

    UtPrintf("Set has secondary header bit only and verify");
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(0x0800)), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(Test_MSG_NotZero(&msg), MSG_HASSEC_FLAG);
    ASSERT_EQ(CFE_MSG_GetHasSecondaryHeader(&msg, &hassec), CFE_SUCCESS);
    ASSERT_EQ(hassec, true);

    UtPrintf("Set type msgid bit only and verify");
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(0x1000)), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(Test_MSG_NotZero(&msg), MSG_TYPE_FLAG);
    ASSERT_EQ(CFE_MSG_GetType(&msg, &type), CFE_SUCCESS);
    ASSERT_EQ(type, CFE_MSG_Type_Cmd);
}
