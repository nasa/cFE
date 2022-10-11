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
 * Test SBR direct message map implementation
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "cfe_sbr.h"
#include "cfe_sbr_priv.h"
#include <stdlib.h>

/*
 * Reasonable limit on loops in case CFE_PLATFORM_SB_HIGHEST_VALID_MSGID is large
 * Can be set equal to the configured highest if user requires it
 */
#define CFE_SBR_UT_LIMIT_HIGHEST_MSGID 0x1FFF

void Test_SBR_Map_Direct(void)
{
    CFE_SB_MsgId_Atom_t msgidx;
    CFE_SB_MsgId_Atom_t msgid_limit;
    CFE_SBR_RouteId_t   routeid;
    CFE_SB_MsgId_t      msgid;
    uint32              count;
    uint32              i;

    UtPrintf("Invalid msg checks");
    UtAssert_INT32_EQ(CFE_SBR_SetRouteId(CFE_SB_INVALID_MSG_ID, CFE_SBR_ValueToRouteId(0)), 0);
    UtAssert_BOOL_FALSE(CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_INVALID_MSG_ID)));

    UtPrintf("Initialize map");
    CFE_SBR_Init_Map();

    /* Force valid msgid responses */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);

    /* Limit message id loops */
    if (CFE_PLATFORM_SB_HIGHEST_VALID_MSGID > CFE_SBR_UT_LIMIT_HIGHEST_MSGID)
    {
        msgid_limit = CFE_SBR_UT_LIMIT_HIGHEST_MSGID;
        UtPrintf("Limiting msgid ut loops to 0x%08X of 0x%08X", (unsigned int)msgid_limit,
                 (unsigned int)CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    }
    else
    {
        msgid_limit = CFE_PLATFORM_SB_HIGHEST_VALID_MSGID;
        UtPrintf("Testing full msgid range in ut up to 0x%08X", (unsigned int)msgid_limit);
    }

    UtPrintf("Check that entries are set invalid");
    count = 0;
    for (msgidx = 0; msgidx <= msgid_limit; msgidx++)
    {
        if (!CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_ValueToMsgId(msgidx))))
        {
            count++;
        }
    }
    UtAssert_INT32_EQ(count, msgid_limit + 1);

    UtPrintf("Set/Get a range of ids ");
    routeid = CFE_SBR_ValueToRouteId(CFE_PLATFORM_SB_MAX_MSG_IDS + 1);
    msgid   = CFE_SB_INVALID_MSG_ID;
    UtAssert_INT32_EQ(CFE_SBR_SetRouteId(msgid, routeid), 0);
    UtAssert_INT32_EQ(CFE_SBR_GetRouteId(msgid).RouteId, routeid.RouteId);

    routeid = CFE_SBR_ValueToRouteId(0);
    msgid   = CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    UtAssert_INT32_EQ(CFE_SBR_SetRouteId(msgid, routeid), 0);
    UtAssert_INT32_EQ(CFE_SBR_GetRouteId(msgid).RouteId, routeid.RouteId);

    /* Get number of valid routes in range */
    count = 0;
    for (msgidx = 0; msgidx <= msgid_limit; msgidx++)
    {
        if (CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_ValueToMsgId(msgidx))))
        {
            count++;
        }
    }

    /* Check result based on range checked */
    if (msgid_limit == CFE_PLATFORM_SB_HIGHEST_VALID_MSGID)
    {
        /* Full range, 1 valid */
        UtPrintf("Check there is 1 valid entry in map");
        UtAssert_INT32_EQ(count, 1);
    }
    else
    {
        /* Limited range, up to 1 valid */
        UtPrintf("Up to 1 valid entry in limited range check");
        UtAssert_INT32_LTEQ(count, 1);
    }

    UtPrintf("Set back to invalid and check again");
    routeid = CFE_SBR_INVALID_ROUTE_ID;
    UtAssert_INT32_EQ(CFE_SBR_SetRouteId(msgid, routeid), 0);
    UtAssert_INT32_EQ(CFE_SBR_GetRouteId(msgid).RouteId, routeid.RouteId);
    UtAssert_BOOL_FALSE(CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(msgid)));

    /* Performance check, 0xFFFFFF on 3.2GHz linux box is around 8-9 seconds */
    count = 0;
    for (i = 0; i <= 0xFFFF; i++)
    {
        msgidx = rand() % CFE_PLATFORM_SB_HIGHEST_VALID_MSGID;
        if (CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_ValueToMsgId(msgidx))))
        {
            count++;
        }
    }
    UtPrintf("Valid route id's encountered in performance loop: %u", (unsigned int)count);
}

/* Main unit test routine */
void UtTest_Setup(void)
{
    UT_Init("map_direct");
    UtPrintf("Software Bus Routing direct map coverage test...");

    UT_ADD_TEST(Test_SBR_Map_Direct);
}
