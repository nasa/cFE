/**
 * \file
 *   Functional test of ES Performance APIs
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestPerfLogEntry(void)
{
    UtAssert_VOIDCALL(CFE_ES_PerfLogEntry(0));
    UtAssert_VOIDCALL(CFE_ES_PerfLogEntry(CFE_MISSION_ES_PERF_MAX_IDS - 1));
    UtAssert_VOIDCALL(CFE_ES_PerfLogEntry(CFE_MISSION_ES_PERF_MAX_IDS));
}

void TestPerfLogExit(void)
{
    UtAssert_VOIDCALL(CFE_ES_PerfLogExit(0));
    UtAssert_VOIDCALL(CFE_ES_PerfLogExit(CFE_MISSION_ES_PERF_MAX_IDS - 1));
    UtAssert_VOIDCALL(CFE_ES_PerfLogExit(CFE_MISSION_ES_PERF_MAX_IDS));
}

void TestPerfLogAdd(void)
{
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(0, 0));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(0, 1));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(0, 0xFFFFFFFF));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS - 1, 0));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS - 1, 1));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS - 1, 0xFFFFFFFF));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS, 0));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS, 1));
    UtAssert_VOIDCALL(CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS, 0xFFFFFFFF));
}

/* These commands should trigger and stop perf data (based on value commanded in functional test workflow) */
void TestPerfLogTrigger(void)
{
    UtAssert_VOIDCALL(CFE_ES_PerfLogEntry(126));
    UtAssert_VOIDCALL(CFE_ES_PerfLogExit(126));
}

void ESPerfTestSetup(void)
{
    UtTest_Add(TestPerfLogEntry, NULL, NULL, "Test PerfLogEntry");
    UtTest_Add(TestPerfLogExit, NULL, NULL, "Test PerfLogExit");
    UtTest_Add(TestPerfLogAdd, NULL, NULL, "Test PerfLogAdd");
    UtTest_Add(TestPerfLogTrigger, NULL, NULL, "Test Perf Trigger");
}
