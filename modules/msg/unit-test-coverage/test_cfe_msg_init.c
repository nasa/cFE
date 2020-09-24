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
 * Test message init
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "test_msg_not.h"
#include "test_msg_utils.h"
#include "cfe_msg_api.h"
#include "test_cfe_msg_init.h"
#include "cfe_error.h"
#include "cfe_msg_defaults.h"
#include <string.h>

#define TEST_DEFAULT_APID_MASK 0x780 /* Bits that can be set by default apid if msgid V2 */

/*
 * Test MSG Init
 */
void Test_MSG_Init(void)
{

    CFE_MSG_Message_t          msg;
    CFE_MSG_Size_t             size;
    CFE_SB_MsgId_Atom_t        msgidval_exp;
    CFE_SB_MsgId_t             msgid_act;
    CFE_MSG_HeaderVersion_t    hdrver;
    CFE_MSG_ApId_t             apid;
    CFE_MSG_SegmentationFlag_t segflag;
    unsigned int               flag_exp;
    bool                       hassec;
    bool                       is_v1;

    UtPrintf("Bad parameter tests, Null pointer, invalid size, invalid msgid");
    memset(&msg, 0, sizeof(msg));
    ASSERT_EQ(CFE_MSG_Init(NULL, CFE_SB_ValueToMsgId(0), sizeof(msg), false), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(CFE_MSG_Init(&msg, CFE_SB_ValueToMsgId(0), 0, false), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(CFE_MSG_Init(&msg, CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1), sizeof(msg), false),
              CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(CFE_MSG_Init(&msg, CFE_SB_ValueToMsgId(-1), sizeof(msg), false), CFE_MSG_BAD_ARGUMENT);
    ASSERT_EQ(Test_MSG_NotZero(&msg), 0);

    UtPrintf("Set to all F's, msgid value = 0, and run without clearing");
    memset(&msg, 0xFF, sizeof(msg));
    msgidval_exp = 0;
    ASSERT_EQ(CFE_MSG_Init(&msg, CFE_SB_ValueToMsgId(msgidval_exp), sizeof(msg), false), CFE_SUCCESS);
    Test_MSG_PrintMsg(&msg, 0);

    /* Get msgid version by checking if header version was set */
    ASSERT_EQ(CFE_MSG_GetHeaderVersion(&msg, &hdrver), CFE_SUCCESS);
    is_v1 = (hdrver == 0);

    flag_exp = MSG_TYPE_FLAG | MSG_LENGTH_FLAG | MSG_APID_FLAG;
    if (is_v1)
        flag_exp |= MSG_HDRVER_FLAG | MSG_HASSEC_FLAG;

    ASSERT_EQ(Test_MSG_Pri_NotF(&msg), flag_exp);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid_act), CFE_SUCCESS);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid_act), msgidval_exp);
    ASSERT_EQ(CFE_MSG_GetSize(&msg, &size), CFE_SUCCESS);
    ASSERT_EQ(size, sizeof(msg));

    UtPrintf("Set to all F's, msgid value = 0, and run with clearing");
    memset(&msg, 0xFF, sizeof(msg));
    msgidval_exp = 0;

    ASSERT_EQ(CFE_MSG_Init(&msg, CFE_SB_ValueToMsgId(msgidval_exp), sizeof(msg), true), CFE_SUCCESS);
    Test_MSG_PrintMsg(&msg, 0);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid_act), CFE_SUCCESS);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid_act), msgidval_exp);
    ASSERT_EQ(CFE_MSG_GetSize(&msg, &size), CFE_SUCCESS);
    ASSERT_EQ(size, sizeof(msg));
    ASSERT_EQ(CFE_MSG_GetSegmentationFlag(&msg, &segflag), CFE_SUCCESS);
    ASSERT_EQ(segflag, CFE_MSG_SegFlag_Unsegmented);

    ASSERT_EQ(CFE_MSG_GetApId(&msg, &apid), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetHeaderVersion(&msg, &hdrver), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetHasSecondaryHeader(&msg, &hassec), CFE_SUCCESS);
    if (!is_v1)
    {
        ASSERT_EQ(apid, CFE_PLATFORM_DEFAULT_APID & TEST_DEFAULT_APID_MASK);
        ASSERT_EQ(hdrver, CFE_MISSION_CCSDSVER);
        ASSERT_EQ(hassec, true);
    }
    else
    {
        ASSERT_EQ(apid, 0);
        ASSERT_EQ(hdrver, 0);
        ASSERT_EQ(hassec, false);
    }

    /* Confirm the rest of the fields not already explicitly checked */
    ASSERT_EQ(Test_MSG_Pri_NotZero(&msg) & ~(MSG_APID_FLAG | MSG_HDRVER_FLAG | MSG_HASSEC_FLAG),
              MSG_LENGTH_FLAG | MSG_SEGMENT_FLAG);

    UtPrintf("Set to all 0, max msgid value, and run without clearing");
    memset(&msg, 0, sizeof(msg));
    msgidval_exp = CFE_PLATFORM_SB_HIGHEST_VALID_MSGID;
    ASSERT_EQ(CFE_MSG_Init(&msg, CFE_SB_ValueToMsgId(msgidval_exp), sizeof(msg), false), CFE_SUCCESS);
    Test_MSG_PrintMsg(&msg, 0);
    ASSERT_EQ(CFE_MSG_GetMsgId(&msg, &msgid_act), CFE_SUCCESS);
    ASSERT_EQ(CFE_SB_MsgIdToValue(msgid_act), msgidval_exp);
    ASSERT_EQ(CFE_MSG_GetSize(&msg, &size), CFE_SUCCESS);
    ASSERT_EQ(size, sizeof(msg));

    ASSERT_EQ(CFE_MSG_GetApId(&msg, &apid), CFE_SUCCESS);
    ASSERT_EQ(CFE_MSG_GetHasSecondaryHeader(&msg, &hassec), CFE_SUCCESS);
    if (!is_v1)
    {
        ASSERT_EQ(apid & TEST_DEFAULT_APID_MASK, CFE_PLATFORM_DEFAULT_APID & TEST_DEFAULT_APID_MASK);
        ASSERT_EQ(hassec, false);
    }
    else
    {
        ASSERT_EQ(apid, 0x7FF);
        ASSERT_EQ(hassec, true);
    }

    ASSERT_EQ(Test_MSG_Pri_NotZero(&msg) & ~(MSG_APID_FLAG | MSG_HASSEC_FLAG), MSG_TYPE_FLAG | MSG_LENGTH_FLAG);

    /* Zero (no default) header version check */
    ASSERT_EQ(CFE_MSG_GetHeaderVersion(&msg, &hdrver), CFE_SUCCESS);
    ASSERT_EQ(hdrver, 0);
}
