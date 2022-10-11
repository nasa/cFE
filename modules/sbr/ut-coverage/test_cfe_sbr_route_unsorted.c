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
 * Test SBR unsorted route implementation
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "cfe_sbr.h"
#include "cfe_sbr_priv.h"

/* Callback function for testing */
void Test_SBR_Callback(CFE_SBR_RouteId_t RouteId, void *ArgPtr)
{
    uint32 *count = ArgPtr;

    (*count)++;
}

void Test_SBR_Route_Unsort_General(void)
{
    CFE_SBR_RouteId_t  routeid;
    CFE_SB_MsgId_t     msgid;
    uint32             collisions;
    uint32             count;
    CFE_SBR_Throttle_t throttle;

    UtPrintf("Initialize map and route");
    CFE_SBR_Init();

    UtPrintf("Invalid msg checks");
    UtAssert_BOOL_FALSE(CFE_SBR_IsValidRouteId(CFE_SBR_AddRoute(CFE_SB_INVALID_MSG_ID, NULL)));
    UtAssert_BOOL_FALSE(CFE_SBR_IsValidRouteId(CFE_SBR_AddRoute(CFE_SB_INVALID_MSG_ID, &collisions)));
    UtAssert_INT32_EQ(collisions, 0);

    /*
     * Force valid msgid responses
     * Note from here on msgids must be in the valid range since validation is forced true
     * and if the underlying map implementation is direct it needs to be a valid array index
     */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);

    UtPrintf("Callback test with no routes");
    count = 0;
    CFE_SBR_ForEachRouteId(Test_SBR_Callback, &count, NULL);
    UtAssert_INT32_EQ(count, 0);

    UtPrintf("Add maximum message id value");
    msgid   = CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    routeid = CFE_SBR_AddRoute(msgid, &collisions);
    UtAssert_INT32_EQ(collisions, 0);
    UtAssert_BOOL_TRUE(CFE_SBR_IsValidRouteId(routeid));

    UtPrintf("Callback test with one route");
    count = 0;
    CFE_SBR_ForEachRouteId(Test_SBR_Callback, &count, NULL);
    UtAssert_INT32_EQ(count, 1);

    UtPrintf("Fill routing table");
    count = 0;
    while (CFE_SBR_IsValidRouteId(CFE_SBR_AddRoute(CFE_SB_ValueToMsgId(count), NULL)))
    {
        count++;
    }

    /* Check for expected count indicating full routing table */
    UtAssert_INT32_EQ(count + 1, CFE_PLATFORM_SB_MAX_MSG_IDS);

    /* Try one more for good luck */
    UtAssert_BOOL_FALSE(CFE_SBR_IsValidRouteId(CFE_SBR_AddRoute(CFE_SB_ValueToMsgId(count), NULL)));

    /* Check that maximum msgid is still in the table */
    UtAssert_INT32_EQ(CFE_SB_MsgIdToValue(CFE_SBR_GetMsgId(routeid)), CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
    UtAssert_INT32_EQ(CFE_SBR_GetRouteId(msgid).RouteId, routeid.RouteId);

    UtPrintf("Callback test with full route");
    count = 0;
    CFE_SBR_ForEachRouteId(Test_SBR_Callback, &count, NULL);
    UtAssert_INT32_EQ(count, CFE_PLATFORM_SB_MAX_MSG_IDS);

    UtPrintf("Callback test throttled");
    throttle.MaxLoop    = CFE_PLATFORM_SB_MAX_MSG_IDS - 1;
    throttle.StartIndex = 0;
    count               = 0;
    CFE_SBR_ForEachRouteId(Test_SBR_Callback, &count, &throttle);
    UtAssert_UINT32_EQ(count, CFE_PLATFORM_SB_MAX_MSG_IDS - 1);
    count               = 0;
    throttle.StartIndex = throttle.NextIndex;
    CFE_SBR_ForEachRouteId(Test_SBR_Callback, &count, &throttle);
    UtAssert_INT32_EQ(count, 1);
}

void Test_SBR_Route_Unsort_GetSet(void)
{
    CFE_SB_RouteId_Atom_t   routeidx;
    CFE_SB_MsgId_t          msgid[3];
    CFE_SBR_RouteId_t       routeid[3];
    CFE_SB_DestinationD_t   dest[2];
    CFE_MSG_SequenceCount_t seqcntexpected[] = {1, 2};
    uint32                  count;
    uint32                  i;

    UtPrintf("Invalid route ID checks");
    routeid[0] = CFE_SBR_INVALID_ROUTE_ID;
    routeid[1] = CFE_SBR_ValueToRouteId(CFE_PLATFORM_SB_MAX_MSG_IDS);
    for (i = 0; i < 2; i++)
    {
        UtAssert_BOOL_TRUE(CFE_SB_MsgId_Equal(CFE_SBR_GetMsgId(routeid[i]), CFE_SB_INVALID_MSG_ID));
        UtAssert_ADDRESS_EQ(CFE_SBR_GetDestListHeadPtr(routeid[i]), NULL);
        UtAssert_INT32_EQ(CFE_SBR_GetSequenceCounter(routeid[i]), 0);
        UtAssert_VOIDCALL(CFE_SBR_SetDestListHeadPtr(routeid[i], NULL));
        UtAssert_VOIDCALL(CFE_SBR_IncrementSequenceCounter(routeid[i]));
    }

    /*
     * Force valid msgid responses
     * Note from here on msgids must be in the valid range since validation is forced true
     * and if the underlying map implementation is direct it needs to be a valid array index
     */
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_IsValidMsgId), true);

    UtPrintf("Initialize map and route");
    CFE_SBR_Init();

    UtPrintf("Confirm values initialized for all routes");
    count = 0;
    for (routeidx = 0; routeidx < CFE_PLATFORM_SB_MAX_MSG_IDS; routeidx++)
    {
        routeid[0] = CFE_SBR_ValueToRouteId(routeidx);
        if (!CFE_SB_MsgId_Equal(CFE_SBR_GetMsgId(routeid[0]), CFE_SB_INVALID_MSG_ID) ||
            (CFE_SBR_GetDestListHeadPtr(routeid[0]) != NULL) || (CFE_SBR_GetSequenceCounter(routeid[0]) != 0))
        {
            count++;
        }
    }
    UtAssert_INT32_EQ(count, 0);

    UtPrintf("Add routes and initialize values for testing");
    msgid[0] = CFE_SB_INVALID_MSG_ID;
    msgid[1] = CFE_SB_ValueToMsgId(1);
    msgid[2] = CFE_SB_ValueToMsgId(CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);

    /* Add routes */
    for (i = 0; i < 3; i++)
    {
        routeid[i] = CFE_SBR_AddRoute(msgid[i], NULL);
    }

    /* Check the msgid matches and increment a sequence counter */
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), seqcntexpected[0]);
    for (i = 0; i < 3; i++)
    {
        UtAssert_BOOL_TRUE(CFE_SB_MsgId_Equal(msgid[i], CFE_SBR_GetMsgId(routeid[i])));
        CFE_SBR_IncrementSequenceCounter(routeid[0]);
    }
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 3);

    /* Increment route 1 once and set dest pointers */
    UT_SetDefaultReturnValue(UT_KEY(CFE_MSG_GetNextSequenceCount), seqcntexpected[1]);
    CFE_SBR_IncrementSequenceCounter(routeid[1]);
    UtAssert_STUB_COUNT(CFE_MSG_GetNextSequenceCount, 4);
    CFE_SBR_SetDestListHeadPtr(routeid[1], &dest[1]);
    CFE_SBR_SetDestListHeadPtr(routeid[2], &dest[0]);

    UtPrintf("Verify remaining set values");
    UtAssert_UINT32_EQ(CFE_SBR_GetSequenceCounter(routeid[0]), seqcntexpected[0]);
    UtAssert_UINT32_EQ(CFE_SBR_GetSequenceCounter(routeid[1]), seqcntexpected[1]);
    UtAssert_INT32_EQ(CFE_SBR_GetSequenceCounter(routeid[2]), 0);
    UtAssert_ADDRESS_EQ(CFE_SBR_GetDestListHeadPtr(routeid[0]), NULL);
    UtAssert_ADDRESS_EQ(CFE_SBR_GetDestListHeadPtr(routeid[1]), &dest[1]);
    UtAssert_ADDRESS_EQ(CFE_SBR_GetDestListHeadPtr(routeid[2]), &dest[0]);
}

/* Main unit test routine */
void UtTest_Setup(void)
{
    UT_Init("route_unsort");
    UtPrintf("Software Bus Routing unsorted coverage test...");

    UT_ADD_TEST(Test_SBR_Route_Unsort_General);
    UT_ADD_TEST(Test_SBR_Route_Unsort_GetSet);
}
