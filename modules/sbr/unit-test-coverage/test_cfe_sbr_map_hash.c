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

/*
 * Defines
 */

/* Unhash magic number */
#define CFE_SBR_UNHASH_MAGIC (0x119de1f3)

/******************************************************************************
 * Local helper to unhash
 */
CFE_SB_MsgId_t Test_SBR_Unhash(CFE_SB_MsgId_Atom_t Hash)
{

    Hash = ((Hash >> 16) ^ Hash) * CFE_SBR_UNHASH_MAGIC;
    Hash = ((Hash >> 16) ^ Hash) * CFE_SBR_UNHASH_MAGIC;
    Hash = (Hash >> 16) ^ Hash;

    return CFE_SB_ValueToMsgId(Hash);
}

void Test_SBR_Map_Hash(void)
{

    CFE_SB_MsgId_Atom_t msgidx;
    CFE_SBR_RouteId_t   routeid[3];
    CFE_SB_MsgId_t      msgid[3];
    uint32              count;
    uint32              collisions;

    UtPrintf("Invalid msg checks");
    ASSERT_EQ(CFE_SBR_SetRouteId(CFE_SB_ValueToMsgId(0), CFE_SBR_ValueToRouteId(0)), 0);
    ASSERT_EQ(CFE_SBR_IsValidRouteId(CFE_SBR_GetRouteId(CFE_SB_ValueToMsgId(0))), false);

    UtPrintf("Initialize routing and map");
    CFE_SBR_Init();

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

    /* Note AddRoute required for hash logic to work since it depends on MsgId in routing table */
    UtPrintf("Add routes and check with a rollover and a skip");
    msgid[0]   = CFE_SB_ValueToMsgId(0);
    msgid[1]   = Test_SBR_Unhash(0xFFFFFFFF);
    msgid[2]   = Test_SBR_Unhash(0x7FFFFFFF);
    routeid[0] = CFE_SBR_AddRoute(msgid[0], &collisions);
    ASSERT_EQ(collisions, 0);
    routeid[1] = CFE_SBR_AddRoute(msgid[1], &collisions);
    ASSERT_EQ(collisions, 0);
    routeid[2] = CFE_SBR_AddRoute(msgid[2], &collisions);
    ASSERT_EQ(collisions, 2);

    ASSERT_EQ(CFE_SBR_RouteIdToValue(CFE_SBR_GetRouteId(msgid[0])), CFE_SBR_RouteIdToValue(routeid[0]));
    ASSERT_EQ(CFE_SBR_RouteIdToValue(CFE_SBR_GetRouteId(msgid[1])), CFE_SBR_RouteIdToValue(routeid[1]));
    ASSERT_EQ(CFE_SBR_RouteIdToValue(CFE_SBR_GetRouteId(msgid[2])), CFE_SBR_RouteIdToValue(routeid[2]));

    /* Performance check, 0xFFFFFF on 3.2GHz linux box is around 8-9 seconds */
    count = 0;
    for (msgidx = 0; msgidx <= 0xFFFF; msgidx++)
    {
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
    UT_Init("map_hash");
    UtPrintf("Software Bus Routing hash map coverage test...");

    UT_ADD_TEST(Test_SBR_Map_Hash);
}
