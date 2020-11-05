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
 * Test SBR direct message map implementation
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "private/cfe_sbr.h"
#include "cfe_sbr_priv.h"
#include <stdlib.h>

void Test_SBR_Map_Direct(void)
{

    CFE_SB_MsgId_Atom_t msgidx;
    CFE_SBR_RouteId_t   routeid;
    CFE_SB_MsgId_t      msgid;
    uint32              count;
    uint32              i;

    UtPrintf("Invalid msg checks");
    ASSERT_EQ(CFE_SBR_SetRouteId(CFE_SB_ValueToMsgId(0), CFE_SBR_ValueToRouteId(0)), 0);
    ASSERT_EQ(CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_ValueToMsgId(0))), false);

    UtPrintf("Initialize map");
    CFE_SBR_Init_Map();

    /* Force valid msgid responses */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);

    UtPrintf("Check that all entries are set invalid");
    count = 0;
    for (msgidx = 0; msgidx <= CFE_PLATFORM_SB_HIGHEST_VALID_MSGID; msgidx++)
    {
        if (!CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_ValueToMsgId(msgidx))))
        {
            count++;
        }
    }
    ASSERT_EQ(count, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID + 1);

    UtPrintf("Set/Get a range of ids ");
    routeid = CFE_SBR_ValueToRouteId(CFE_PLATFORM_SB_MAX_MSG_IDS + 1);
    msgid   = CFE_SB_ValueToMsgId(0);
    ASSERT_EQ(CFE_SBR_SetRouteId(msgid, routeid), 0);
    ASSERT_EQ(CFE_SBR_GetRouteId(msgid).RouteId, routeid.RouteId);

    routeid = CFE_SBR_ValueToRouteId(0);
    msgid   = CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    ASSERT_EQ(CFE_SBR_SetRouteId(msgid, routeid), 0);
    ASSERT_EQ(CFE_SBR_GetRouteId(msgid).RouteId, routeid.RouteId);

    UtPrintf("Check there is now 1 valid entry in map");
    count = 0;
    for (msgidx = 0; msgidx <= CFE_PLATFORM_SB_HIGHEST_VALID_MSGID; msgidx++)
    {
        if (!CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_ValueToMsgId(msgidx))))
        {
            count++;
        }
    }
    ASSERT_EQ(count, CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);

    UtPrintf("Set back to invalid and check again");
    routeid = CFE_SBR_INVALID_ROUTE_ID;
    ASSERT_EQ(CFE_SBR_SetRouteId(msgid, routeid), 0);
    ASSERT_EQ(CFE_SBR_GetRouteId(msgid).RouteId, routeid.RouteId);
    ASSERT_EQ(CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(msgid)), false);

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
