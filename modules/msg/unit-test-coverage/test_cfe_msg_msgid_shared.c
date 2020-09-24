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
 * Test MsgId shared accessors
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "cfe_msg_api.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "test_cfe_msg_msgid_shared.h"
#include "cfe_error.h"
#include <string.h>

void Test_MSG_GetTypeFromMsgId(void)
{
    CFE_MSG_Message_t msg;
    CFE_SB_MsgId_t    msgid  = CFE_SB_ValueToMsgId(0);
    CFE_MSG_Type_t    actual = CFE_MSG_Type_Invalid;

    UtPrintf("Bad parameter tests, Null pointer");
    memset(&msg, 0, sizeof(msg));
    ASSERT_EQ(CFE_MSG_GetTypeFromMsgId(msgid, NULL), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(Test_MSG_NotZero(&msg), 0);

    UtPrintf("Set to all F's, test cmd and tlm");
    memset(&msg, 0xFF, sizeof(msg));
    ASSERT_EQ(CFE_MSG_SetMsgId(&msg, CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID)), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_SetType(&msg, CFE_MSG_Type_Tlm), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetTypeFromMsgId(msgid, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, CFE_MSG_Type_Tlm);

    ASSERT_EQ(CFE_MSG_SetType(&msg, CFE_MSG_Type_Cmd), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetTypeFromMsgId(msgid, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, CFE_MSG_Type_Cmd);

    UtPrintf("Set to all 0, test cmd and tlm");
    ASSERT_EQ(CFE_MSG_SetType(&msg, CFE_MSG_Type_Cmd), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetTypeFromMsgId(msgid, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, CFE_MSG_Type_Cmd);

    ASSERT_EQ(CFE_MSG_SetType(&msg, CFE_MSG_Type_Tlm), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetTypeFromMsgId(msgid, &actual), CFE_SUCCESS);
    ASSERT_EQ(actual, CFE_MSG_Type_Tlm);
}

/*
 * Test MSG MsgId Shared
 */
void Test_MSG_MsgId_Shared(void)
{
    MSG_UT_ADD_SUBTEST(Test_MSG_GetTypeFromMsgId);
}
