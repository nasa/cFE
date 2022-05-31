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

/**
 * @file
 *
 * Functional test of ES Generic Counter APIs
 */

#include "cfe_test.h"

void TestCounterCreateDelete(void)
{
    CFE_ES_CounterId_t Ids[CFE_PLATFORM_ES_MAX_GEN_COUNTERS + 1];
    CFE_ES_CounterId_t TestId;
    CFE_ES_CounterId_t CheckId;
    char               CounterName[CFE_MISSION_MAX_API_LEN];
    char               CheckName[CFE_MISSION_MAX_API_LEN];
    CFE_Status_t       Status;
    uint32             NumCounters;
    uint32             Idx;

    UtPrintf("Testing: CFE_ES_RegisterGenCounter");

    snprintf(CounterName, sizeof(CounterName), "ut");

    /* Confirm proper bad argument rejection */
    UtAssert_INT32_EQ(CFE_ES_RegisterGenCounter(&Ids[0], NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_RegisterGenCounter(NULL, CounterName), CFE_ES_BAD_ARGUMENT);

    /* Create up to CFE_PLATFORM_ES_MAX_GEN_COUNTERS and confirm success */
    /* Note that this loop may execute fewer than CFE_PLATFORM_ES_MAX_GEN_COUNTERS times,
     * if another unrelated app has already registered a counter.  Because this test
     * cannot control for those pre-conditions, anything within range is acceptable */
    for (NumCounters = 0; NumCounters <= CFE_PLATFORM_ES_MAX_GEN_COUNTERS; ++NumCounters)
    {
        snprintf(CounterName, sizeof(CounterName), "C%u", (unsigned int)NumCounters);
        CFE_Assert_STATUS_STORE(CFE_ES_RegisterGenCounter(&Ids[NumCounters], CounterName));

        /* When the max limit is reached, should return CFE_ES_NO_RESOURCE_IDS_AVAILABLE */
        if (CFE_Assert_STATUS_MAY_BE(CFE_ES_NO_RESOURCE_IDS_AVAILABLE))
        {
            break;
        }

        /* If max limit not reached, should return CFE_SUCCESS, anything else is a test fail */
        CFE_Assert_STATUS_MAY_BE(CFE_SUCCESS);
    }

    /* If no counters were available skip the rest of the test */
    if (NumCounters == 0)
    {
        UtAssert_MIR("No ES generic counters available for testing, skipping");
        return;
    }

    /* Confirm that the expected number of counters were created */
    UtAssert_UINT32_LTEQ(NumCounters, CFE_PLATFORM_ES_MAX_GEN_COUNTERS);

    /* pick a single counter ID from the middle of the set for more detail testing of support APIs */
    TestId = Ids[NumCounters / 2];
    snprintf(CounterName, sizeof(CounterName), "C%u", (unsigned int)NumCounters / 2);

    UtPrintf("Testing: CFE_ES_CounterID_ToIndex");

    /* Confirm CFE_ES_CounterID_ToIndex works (nominal) */
    Idx = UINT32_MAX;
    UtAssert_INT32_EQ(CFE_ES_CounterID_ToIndex(TestId, &Idx), CFE_SUCCESS);
    UtAssert_UINT32_LT(Idx, CFE_PLATFORM_ES_MAX_GEN_COUNTERS);

    /* Confirm proper rejection of bad args in CFE_ES_CounterID_ToIndex */
    UtAssert_INT32_EQ(CFE_ES_CounterID_ToIndex(CFE_ES_COUNTERID_UNDEFINED, &Idx), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_CounterID_ToIndex(TestId, NULL), CFE_ES_BAD_ARGUMENT);

    UtPrintf("Testing: CFE_ES_GetGenCounterIDByName, CFE_ES_GetGenCounterName");

    /* Confirm conversion To/From Name */
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterIDByName(&CheckId, CounterName), CFE_SUCCESS);
    CFE_Assert_RESOURCEID_EQ(CheckId, TestId);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterName(CheckName, TestId, sizeof(CheckName)), CFE_SUCCESS);
    UtAssert_STRINGBUF_EQ(CheckName, sizeof(CheckName), CounterName, sizeof(CounterName));

    UtAssert_INT32_EQ(CFE_ES_GetGenCounterIDByName(&CheckId, "na"), CFE_ES_ERR_NAME_NOT_FOUND);

    /* Confirm proper rejection of bad args in conversion To/From Name */
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterIDByName(NULL, CounterName), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterIDByName(&CheckId, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterName(CheckName, CFE_ES_COUNTERID_UNDEFINED, sizeof(CounterName)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterName(CheckName, TestId, 0), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterName(NULL, TestId, sizeof(CounterName)), CFE_ES_BAD_ARGUMENT);

    UtPrintf("Testing: CFE_ES_DeleteGenCounter");

    /* Confirm proper rejection of bad args in CFE_ES_DeleteGenCounter (this returns CFE_ES_BAD_ARGUMENT instead) */
    UtAssert_INT32_EQ(CFE_ES_DeleteGenCounter(CFE_ES_COUNTERID_UNDEFINED), CFE_ES_BAD_ARGUMENT);

    /* Delete last counter to test duplicate name rejection (needs a free slot) */
    --NumCounters;
    UtAssert_INT32_EQ(CFE_ES_DeleteGenCounter(Ids[NumCounters]), CFE_SUCCESS);
    snprintf(CounterName, sizeof(CounterName), "C%u", (unsigned int)0);
    UtAssert_INT32_EQ(CFE_ES_RegisterGenCounter(&TestId, CounterName), CFE_ES_ERR_DUPLICATE_NAME);

    /* Delete remainder of counters */
    while (NumCounters > 0)
    {
        Status = CFE_ES_DeleteGenCounter(Ids[NumCounters - 1]);
        if (Status != CFE_SUCCESS)
        {
            break;
        }

        --NumCounters;
    }

    UtAssert_ZERO(NumCounters);
}

void TestCounterGetSet(void)
{
    CFE_ES_CounterId_t TestId;
    uint32             CountVal;

    UtPrintf("Testing: CFE_ES_GetGenCount, CFE_ES_SetGenCount, CFE_ES_IncrementGenCounter");

    /* Setup - create a single counter */
    UtAssert_INT32_EQ(CFE_ES_RegisterGenCounter(&TestId, "ut"), CFE_SUCCESS);

    /* Get and set its count - should be initially 0 */
    CountVal = UINT32_MAX;
    UtAssert_INT32_EQ(CFE_ES_GetGenCount(TestId, &CountVal), CFE_SUCCESS);
    UtAssert_ZERO(CountVal);
    UtAssert_INT32_EQ(CFE_ES_SetGenCount(TestId, 5), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetGenCount(TestId, &CountVal), CFE_SUCCESS);
    UtAssert_UINT32_EQ(CountVal, 5);
    UtAssert_INT32_EQ(CFE_ES_IncrementGenCounter(TestId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetGenCount(TestId, &CountVal), CFE_SUCCESS);
    UtAssert_UINT32_EQ(CountVal, 6);

    /*
     * Confirm bad arg rejection in Get/Set/Increment
     * Note these APIs return CFE_ES_BAD_ARGUMENT rather than
     * CFE_ES_ERR_RESOURCEID_NOT_VALID on a bad ID (historical)
     */
    UtAssert_INT32_EQ(CFE_ES_GetGenCount(TestId, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetGenCount(CFE_ES_COUNTERID_UNDEFINED, &CountVal), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_IncrementGenCounter(CFE_ES_COUNTERID_UNDEFINED), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_SetGenCount(CFE_ES_COUNTERID_UNDEFINED, 0), CFE_ES_BAD_ARGUMENT);

    /* Teardown - delete the counter */
    UtAssert_INT32_EQ(CFE_ES_DeleteGenCounter(TestId), CFE_SUCCESS);
}

void ESCounterTestSetup(void)
{
    UtTest_Add(TestCounterCreateDelete, NULL, NULL, "Test Counter Create/Delete");
    UtTest_Add(TestCounterGetSet, NULL, NULL, "Test Counter Get/Set");
}
