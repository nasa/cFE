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

/*
 * Message header unit tests
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "test_cfe_msg_init.h"
#include "test_cfe_msg_ccsdspri.h"
#include "test_cfe_msg_ccsdsext.h"
#include "test_cfe_msg_verify.h"
#include "test_cfe_msg_msgid_shared.h"
#include "test_cfe_msg_msgid.h"
#include "test_cfe_msg_fc.h"
#include "test_cfe_msg_checksum.h"
#include "test_cfe_msg_time.h"

/*
 * Functions
 */
void UtTest_Setup(void)
{
    UT_Init("msg");
    UtPrintf("Message header coverage test...");

    UT_ADD_TEST(Test_MSG_Init);
    UT_ADD_TEST(Test_MSG_OriginationAction);
    Test_MSG_CCSDSPri();
    Test_MSG_CCSDSExt();
    Test_MSG_MsgId_Shared();
    UT_ADD_TEST(Test_MSG_VerificationAction);
    UT_ADD_TEST(Test_MSG_MsgId);
    UT_ADD_TEST(Test_MSG_Checksum);
    UT_ADD_TEST(Test_MSG_FcnCode);
    UT_ADD_TEST(Test_MSG_Time);
}
