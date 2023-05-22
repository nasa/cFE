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
 * Test message verify
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "cfe_msg.h"
#include "test_cfe_msg_verify.h"
#include "cfe_error.h"
#include "cfe_msg_defaults.h"

#include <string.h>

/*
 * Test MSG Verify
 */
void Test_MSG_Verify(void)
{
    union
    {
        CFE_MSG_Message_t         msg;
        CFE_MSG_CommandHeader_t   cmd;
        CFE_MSG_TelemetryHeader_t tlm;
    } LocalBuf;
    bool Result;

    memset(&LocalBuf, 0, sizeof(LocalBuf));
    Result = false;

    /* bad buffer */
    UtAssert_INT32_EQ(CFE_MSG_Verify(NULL, &Result), CFE_MSG_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_MSG_Verify(&LocalBuf.msg, NULL), CFE_MSG_BAD_ARGUMENT);

    /* nominal */
    Result = false;
    CFE_UtAssert_SUCCESS(CFE_MSG_Verify(&LocalBuf.msg, &Result));
    UtAssert_BOOL_TRUE(Result);
}
