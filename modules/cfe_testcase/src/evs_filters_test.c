/**
 * \file
 *   Functional test of basic EVS Reset Filters APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestResetFilters(void)
{
    UtPrintf("Testing: CFE_EVS_ResetFilter, CFE_EVS_ResetAllFilters");

    /* Test logic below depends on the test case registering an EID of 1 and not registering 0, and the resets in theory
     * could impact test behavior/management related to events. Although the expectation is either all or none of an EID
     * would be filtered (no use case for a "counting" filter within the test app) so for normal use this is no impact.
     */

    UtAssert_INT32_EQ(CFE_EVS_ResetFilter(1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_EVS_ResetAllFilters(), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_EVS_ResetFilter(0), CFE_EVS_EVT_NOT_REGISTERED);
}

void EVSFiltersTestSetup(void)
{
    UtTest_Add(TestResetFilters, NULL, NULL, "Test Reset Filters");
}
