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
#include "cfe_msg.h"
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

    CFE_MSG_CommandHeader_t    cmd;
    CFE_MSG_Size_t             size;
    CFE_SB_MsgId_Atom_t        msgidval_exp;
    CFE_SB_MsgId_t             msgid_act;
    CFE_MSG_HeaderVersion_t    hdrver;
    CFE_MSG_ApId_t             apid;
    CFE_MSG_SegmentationFlag_t segflag;
    bool                       hassec;
    bool                       is_v1;

    UtPrintf("Bad parameter tests, Null pointer, invalid size, invalid msgid");
    CFE_UtAssert_EQUAL(CFE_MSG_Init(NULL, CFE_SB_ValueToMsgId(0), sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);
    CFE_UtAssert_EQUAL(CFE_MSG_Init(&cmd.Msg, CFE_SB_ValueToMsgId(0), 0), CFE_MSG_BAD_ARGUMENT);
    CFE_UtAssert_EQUAL(
        CFE_MSG_Init(&cmd.Msg, CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1), sizeof(cmd)),
        CFE_MSG_BAD_ARGUMENT);
    CFE_UtAssert_EQUAL(CFE_MSG_Init(&cmd.Msg, CFE_SB_ValueToMsgId(-1), sizeof(cmd)), CFE_MSG_BAD_ARGUMENT);

    UtPrintf("Set to all F's, msgid value = 0");
    memset(&cmd, 0xFF, sizeof(cmd));
    msgidval_exp = 0;

    CFE_UtAssert_EQUAL(CFE_MSG_Init(&cmd.Msg, CFE_SB_ValueToMsgId(msgidval_exp), sizeof(cmd)), CFE_SUCCESS);
    UT_DisplayPkt(&cmd.Msg, 0);
    CFE_UtAssert_EQUAL(CFE_MSG_GetMsgId(&cmd.Msg, &msgid_act), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(CFE_SB_MsgIdToValue(msgid_act), msgidval_exp);
    CFE_UtAssert_EQUAL(CFE_MSG_GetSize(&cmd.Msg, &size), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(size, sizeof(cmd));
    CFE_UtAssert_EQUAL(CFE_MSG_GetSegmentationFlag(&cmd.Msg, &segflag), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(segflag, CFE_MSG_SegFlag_Unsegmented);

    CFE_UtAssert_EQUAL(CFE_MSG_GetApId(&cmd.Msg, &apid), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(CFE_MSG_GetHeaderVersion(&cmd.Msg, &hdrver), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(CFE_MSG_GetHasSecondaryHeader(&cmd.Msg, &hassec), CFE_SUCCESS);

    /* A zero msgid will set hassec to false for v1 */
    is_v1 = !hassec;

    if (!is_v1)
    {
        CFE_UtAssert_EQUAL(apid, CFE_PLATFORM_DEFAULT_APID & TEST_DEFAULT_APID_MASK);
        CFE_UtAssert_EQUAL(hdrver, CFE_MISSION_CCSDSVER);
    }
    else
    {
        CFE_UtAssert_EQUAL(apid, 0);
        CFE_UtAssert_EQUAL(hdrver, 0);
    }

    /* Confirm the rest of the fields not already explicitly checked */
    CFE_UtAssert_EQUAL(Test_MSG_Pri_NotZero(&cmd.Msg) & ~(MSG_APID_FLAG | MSG_HDRVER_FLAG | MSG_HASSEC_FLAG),
                       MSG_LENGTH_FLAG | MSG_SEGMENT_FLAG);

    UtPrintf("Set to all 0, max msgid value");
    memset(&cmd, 0, sizeof(cmd));
    msgidval_exp = CFE_PLATFORM_SB_HIGHEST_VALID_MSGID;

    CFE_UtAssert_EQUAL(CFE_MSG_Init(&cmd.Msg, CFE_SB_ValueToMsgId(msgidval_exp), sizeof(cmd)), CFE_SUCCESS);
    UT_DisplayPkt(&cmd.Msg, 0);
    CFE_UtAssert_EQUAL(CFE_MSG_GetMsgId(&cmd.Msg, &msgid_act), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(CFE_SB_MsgIdToValue(msgid_act), msgidval_exp);
    CFE_UtAssert_EQUAL(CFE_MSG_GetSize(&cmd.Msg, &size), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(size, sizeof(cmd));
    CFE_UtAssert_EQUAL(CFE_MSG_GetSegmentationFlag(&cmd.Msg, &segflag), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(segflag, CFE_MSG_SegFlag_Unsegmented);

    CFE_UtAssert_EQUAL(CFE_MSG_GetApId(&cmd.Msg, &apid), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(CFE_MSG_GetHeaderVersion(&cmd.Msg, &hdrver), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(CFE_MSG_GetHasSecondaryHeader(&cmd.Msg, &hassec), CFE_SUCCESS);
    CFE_UtAssert_EQUAL(hassec, true);
    if (!is_v1)
    {
        CFE_UtAssert_EQUAL(apid & TEST_DEFAULT_APID_MASK, CFE_PLATFORM_DEFAULT_APID & TEST_DEFAULT_APID_MASK);
        CFE_UtAssert_EQUAL(hdrver, CFE_MISSION_CCSDSVER);
    }
    else
    {
        CFE_UtAssert_EQUAL(apid, 0x7FF);
        CFE_UtAssert_EQUAL(hdrver, 0);
    }

    CFE_UtAssert_EQUAL(Test_MSG_Pri_NotZero(&cmd.Msg) & ~MSG_HDRVER_FLAG,
                       MSG_APID_FLAG | MSG_HASSEC_FLAG | MSG_TYPE_FLAG | MSG_LENGTH_FLAG | MSG_SEGMENT_FLAG);
}
