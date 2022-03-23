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
