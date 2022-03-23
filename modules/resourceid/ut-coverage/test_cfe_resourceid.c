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
 * Includes
 */
#include "cfe.h"
#include "cfe_resourceid.h"
#include "cfe_resourceid_basevalue.h"
#include "utassert.h"
#include "utstubs.h"
#include "uttest.h"

#define UT_RESOURCEID_BASE_OFFSET 37
#define UT_RESOURCEID_TEST_SLOTS  149 /* oddball for test purposes */

static bool UT_ResourceId_CheckIdSlotUsed(CFE_ResourceId_t Id)
{
    return UT_DEFAULT_IMPL(UT_ResourceId_CheckIdSlotUsed) != 0;
}

void TestResourceID(void)
{
    /*
     * Test cases for generic resource ID functions which are
     * not sufficiently covered by other app/lib tests.
     */
    CFE_ResourceId_t Id;
    CFE_ResourceId_t LastId;
    int32            status;
    uint32           RefBase;
    uint32           Count;
    uint32           TestBase;
    uint32           TestSerial;
    uint32           RefSerial;
    uint32           TestIndex;
    uint32           RefIndex;

    /* Call CFE_ResourceId_FindNext() using an invalid resource type */
    UT_SetDefaultReturnValue(UT_KEY(UT_ResourceId_CheckIdSlotUsed), 1);
    Id = CFE_ResourceId_FindNext(CFE_RESOURCEID_UNDEFINED, 5, UT_ResourceId_CheckIdSlotUsed);
    UtAssert_True(!CFE_ResourceId_IsDefined(Id), "CFE_ResourceId_FindNext() on undefined resource type");

    /* Verify that CFE_ResourceId_FindNext() does not repeat until CFE_RESOURCEID_MAX is reached */
    UT_SetDefaultReturnValue(UT_KEY(UT_ResourceId_CheckIdSlotUsed), 0);
    RefBase   = CFE_RESOURCEID_MAKE_BASE(UT_RESOURCEID_BASE_OFFSET);
    LastId    = CFE_ResourceId_FromInteger(RefBase);
    RefIndex  = 1;
    RefSerial = 1;

    /*
     * In this case it is useful/relevant to call CFE_ResourceId_FindNext() thousands
     * of times, in order to exercise and verify the wrap capability.  That is, when the
     * serial number reaches CFE_RESOURCEID_MAX, it should wrap back around to the
     * beginning again.
     *
     * Note in this loop only _failures_ are asserted, to de-clutter the log -
     * otherwise thousands of success cases will be recorded.
     */
    Count = CFE_RESOURCEID_MAX - 1;
    while (Count > 0)
    {
        Id = CFE_ResourceId_FindNext(LastId, UT_RESOURCEID_TEST_SLOTS, UT_ResourceId_CheckIdSlotUsed);
        if (CFE_ResourceId_ToInteger(Id) - CFE_ResourceId_ToInteger(LastId) != 1)
        {
            /* Numbers should be incrementing by 1 each time, never decreasing */
            UtAssert_Failed("ID increment error: got=%lx, previous=%lx", CFE_ResourceId_ToInteger(Id),
                            CFE_ResourceId_ToInteger(LastId));
            break;
        }

        TestBase = CFE_ResourceId_GetBase(Id);
        if (TestBase != RefBase)
        {
            UtAssert_Failed("ID base changed: id=%lx, expected=%lx, got=%lx", CFE_ResourceId_ToInteger(Id),
                            (unsigned long)RefBase, (unsigned long)TestBase);
        }
        TestSerial = CFE_ResourceId_GetSerial(Id);
        if (TestSerial != RefSerial)
        {
            UtAssert_Failed("ID serial jump: id=%lx, previous=%lx, got=%lx", CFE_ResourceId_ToInteger(Id),
                            (unsigned long)RefSerial, (unsigned long)TestSerial);
        }

        status = CFE_ResourceId_ToIndex(Id, RefBase, UT_RESOURCEID_TEST_SLOTS, &TestIndex);
        if (status != CFE_SUCCESS)
        {
            UtAssert_Failed("CFE_ResourceId_ToIndex() failed: id=%lx, rc=%lx", CFE_ResourceId_ToInteger(Id),
                            (unsigned long)status);
        }

        if (TestIndex != RefIndex)
        {
            UtAssert_Failed("ID index mismatch: id=%lx, expected=%lu, got=%lu", CFE_ResourceId_ToInteger(Id),
                            (unsigned long)RefIndex, (unsigned long)TestIndex);
        }

        LastId = Id;
        --Count;

        /* Adjust to next index value */
        ++RefIndex;
        if (RefIndex >= UT_RESOURCEID_TEST_SLOTS)
        {
            RefIndex = 0;
        }
        ++RefSerial;
    }

    UtAssert_True(Count == 0, "CFE_ResourceId_FindNext() allocated all resource ID space");

    /* Now verify that CFE_ResourceId_FindNext() recycles the first item again */
    Id = CFE_ResourceId_FindNext(LastId, UT_RESOURCEID_TEST_SLOTS, UT_ResourceId_CheckIdSlotUsed);
    UtAssert_True(CFE_ResourceId_IsDefined(Id), "CFE_ResourceId_FindNext() after wrap");
    UtAssert_True(CFE_ResourceId_ToInteger(Id) < (RefBase + UT_RESOURCEID_TEST_SLOTS),
                  "CFE_ResourceId_FindNext() wrap ID");

    /*
     * Confirm outputs are as expected after wrapping around -
     * indices should be sequential
     */
    UtAssert_UINT32_EQ(CFE_ResourceId_GetBase(Id), RefBase);

    TestSerial = CFE_ResourceId_GetSerial(Id);
    UtAssert_True(TestSerial < UT_RESOURCEID_TEST_SLOTS, "ID serial after wrap: id=%lx, previous=%lx, got=%lx",
                  CFE_ResourceId_ToInteger(Id), (unsigned long)RefSerial, (unsigned long)TestSerial);

    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(Id, RefBase, UT_RESOURCEID_TEST_SLOTS, &TestIndex), CFE_SUCCESS);
    UtAssert_True(TestIndex == RefIndex, "ID index after wrap: id=%lx, expected=%lu, got=%lu",
                  CFE_ResourceId_ToInteger(Id), (unsigned long)RefIndex, (unsigned long)TestIndex);

    /*
     * Now check that CFE_ResourceId_FindNext() adheres to the CheckFunc.
     * Have it search through 4 entries to find one available on the 5th slot.
     */
    UT_SetDefaultReturnValue(UT_KEY(UT_ResourceId_CheckIdSlotUsed), true);
    UT_SetDeferredRetcode(UT_KEY(UT_ResourceId_CheckIdSlotUsed), 5, false);
    RefIndex  = (RefIndex + 4) % UT_RESOURCEID_TEST_SLOTS; /* expected */
    RefSerial = TestSerial + 4;

    Id         = CFE_ResourceId_FindNext(LastId, UT_RESOURCEID_TEST_SLOTS, UT_ResourceId_CheckIdSlotUsed);
    TestSerial = CFE_ResourceId_GetSerial(Id);
    UtAssert_True(TestSerial == RefSerial, "ID serial after search: id=%lx, previous=%lx, got=%lx",
                  CFE_ResourceId_ToInteger(Id), (unsigned long)RefSerial, (unsigned long)TestSerial);
    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(Id, RefBase, UT_RESOURCEID_TEST_SLOTS, &TestIndex), CFE_SUCCESS);
    UtAssert_True(TestIndex == RefIndex, "ID index after search: id=%lx, expected=%lu, got=%lu",
                  CFE_ResourceId_ToInteger(Id), (unsigned long)RefIndex, (unsigned long)TestIndex);

    /* For valid Id check other invalid inputs */
    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(Id, RefBase, 1, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(Id, RefBase, 0, &TestIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(Id, ~RefBase, 1, &TestIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* Validate off-nominal inputs */
    Id = CFE_ResourceId_FindNext(CFE_RESOURCEID_UNDEFINED, 0, UT_ResourceId_CheckIdSlotUsed);
    UtAssert_True(CFE_ResourceId_Equal(Id, CFE_RESOURCEID_UNDEFINED), "CFE_ResourceId_FindNext() bad input: id=%lx",
                  CFE_ResourceId_ToInteger(Id));

    Id = CFE_ResourceId_FindNext(LastId, 0, NULL);
    UtAssert_True(CFE_ResourceId_Equal(Id, CFE_RESOURCEID_UNDEFINED), "CFE_ResourceId_FindNext() bad input: id=%lx",
                  CFE_ResourceId_ToInteger(Id));
}

void UtTest_Setup(void)
{
    UtTest_Add(TestResourceID, NULL, NULL, "Resource ID");
}
