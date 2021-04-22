/*************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: cfe_testrunner_main.c
**
** Purpose:
**   Implementation of the CFE testrunner (UT testrunner wrapper) functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe.h"

#include "cfe_assert_priv.h"

#include "uttest.h"
#include "utbsp.h"

/*
 * The maximum amount of time that the application will delay to
 * wait for other apps to complete startup before running the tests.
 *
 * The value is in milliseconds.  Normally this shouldn't be more than
 * a second or two for apps to all reach their respective main loop(s).
 */
#define CFE_TESTRUNNER_MAX_STARTUP_WAIT 30000

/*
 * Small Extra delay before starting tests.
 *
 * This is not strictly necessary, but it does give a bit of time for other apps
 * to settle their final syslog writes/events such that they will not be intermixed
 * with test messages in the syslog.
 *
 * The value is in milliseconds.
 */
#define CFE_TESTRUNNER_START_DELAY 1000

/*
 * This uses the message type as the event ID, because these are already
 * sequential integers, and there is no need to redefine - this app has no other events.
 */
static CFE_EVS_BinFilter_t CFE_TR_EventFilters[] = {
    {UTASSERT_CASETYPE_ABORT, CFE_EVS_NO_FILTER}, {UTASSERT_CASETYPE_FAILURE, CFE_EVS_NO_FILTER},
    {UTASSERT_CASETYPE_TSF, CFE_EVS_NO_FILTER},   {UTASSERT_CASETYPE_TTF, CFE_EVS_NO_FILTER},
    {UTASSERT_CASETYPE_MIR, CFE_EVS_NO_FILTER},   {UTASSERT_CASETYPE_NA, CFE_EVS_NO_FILTER},
    {UTASSERT_CASETYPE_BEGIN, CFE_EVS_NO_FILTER}, {UTASSERT_CASETYPE_END, CFE_EVS_NO_FILTER},
    {UTASSERT_CASETYPE_INFO, CFE_EVS_NO_FILTER},  {UTASSERT_CASETYPE_PASS, CFE_EVS_NO_FILTER},
    {UTASSERT_CASETYPE_DEBUG, CFE_EVS_NO_FILTER},
};

void CFE_Assert_StatusReport(uint8 MessageType, const char *Prefix, const char *OutputMessage)
{
    uint16 EventType;

    switch (MessageType)
    {
        case UTASSERT_CASETYPE_ABORT:
            EventType = CFE_EVS_EventType_CRITICAL;
            break;
        case UTASSERT_CASETYPE_FAILURE:
        case UTASSERT_CASETYPE_TSF:
        case UTASSERT_CASETYPE_TTF:
            EventType = CFE_EVS_EventType_ERROR;
            break;
        case UTASSERT_CASETYPE_BEGIN:
        case UTASSERT_CASETYPE_END:
        case UTASSERT_CASETYPE_INFO:
        case UTASSERT_CASETYPE_MIR:
            EventType = CFE_EVS_EventType_INFORMATION;
            break;
        case UTASSERT_CASETYPE_NA:
        case UTASSERT_CASETYPE_PASS:
        case UTASSERT_CASETYPE_DEBUG:
        default:
            EventType = CFE_EVS_EventType_DEBUG;
            break;
    }

    CFE_EVS_SendEvent(MessageType, EventType, "[%5s] %s", Prefix, OutputMessage);
}

int32 CFE_Assert_RegisterTest(const char *TestName)
{
    int32 rc;
    char  SetupSegmentName[64];

    rc = CFE_EVS_Register(CFE_TR_EventFilters, sizeof(CFE_TR_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                          CFE_EVS_EventFilter_BINARY);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Error from CFE_EVS_Register: %08lx\n", (unsigned long)rc);
        return rc;
    }

    /*
     * Delay until the system reaches "operational" state -- this is when all libs have initialized
     * and all apps have reached their RunLoop.
     *
     * If already operational then this should return immediately.  This may be the case if/when
     * test apps are started via ES command.
     */
    rc = CFE_ES_WaitForSystemState(CFE_ES_SystemState_OPERATIONAL, CFE_TESTRUNNER_MAX_STARTUP_WAIT);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Error while waiting for OPERATIONAL state: %08lx\n", (unsigned long)rc);
        return rc;
    }

    /*
     * Acquire the mutex.  This is needed because UtAssert and its data structures are not thread-safe.
     * Only one test app should use UtAssert facilities at a given time.
     */
    rc = OS_MutSemTake(CFE_Assert_Global.AccessMutex);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): Error from OS_MutSemTake(): %d\n", __func__, (int)rc);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /*
     * After acquiring mutex, record the fact that this app now "owns" the assert functions
     */
    rc = CFE_ES_GetAppID(&CFE_Assert_Global.OwnerAppId);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): Error from CFE_ES_GetAppId(): %08x\n", __func__, (unsigned int)rc);
        OS_MutSemGive(CFE_Assert_Global.AccessMutex);
        return rc;
    }

    /*
     * This means the system is operational and at least one app needs to run tests.
     * Update library state accordingly.  The first test app that gets to this point
     * will handle this.
     */
    if (CFE_Assert_Global.LibState != CFE_Assert_State_ACTIVE)
    {
        UtAssert_EndTest();
        CFE_Assert_Global.LibState = CFE_Assert_State_ACTIVE;

        OS_TaskDelay(CFE_TESTRUNNER_START_DELAY);
    }

    /*
     * This resets the underlying UtAssert test list to a clean slate
     *
     * NOTE: this is not the ideal API here; UtAssert was originally designed to
     * run one set of tests and then exit.  In this environment it might be used
     * repeatedly by different apps.  The "EarlyInit" will wipe and reset the
     * internal global, which works, but is heavy handed and doesn't account for
     * the possibility that tests have been added without running.
     */
    UtTest_EarlyInit();

    strncpy(CFE_Assert_Global.CurrentTestName, TestName, sizeof(CFE_Assert_Global.CurrentTestName) - 1);
    CFE_Assert_Global.CurrentTestName[sizeof(CFE_Assert_Global.CurrentTestName) - 1] = 0;

    /* Use the local status report function for the remainder of tests */
    CFE_Assert_RegisterCallback(CFE_Assert_StatusReport);

    /* Start a test group in case UtAssert is used during setup phase */
    snprintf(SetupSegmentName, sizeof(SetupSegmentName), "%s TEST SETUP", TestName);
    UtAssert_BeginTest(SetupSegmentName);

    return CFE_SUCCESS;
}

void CFE_Assert_ExecuteTest(void)
{
    int32          rc;
    CFE_ES_AppId_t AppId;

    /*
     * Sanity check - This should only be called from the same app after CFE_Assert_RegisterTest()
     */
    rc = CFE_ES_GetAppID(&AppId);
    if (rc != CFE_SUCCESS || !CFE_RESOURCEID_TEST_EQUAL(AppId, CFE_Assert_Global.OwnerAppId))
    {
        CFE_ES_WriteToSysLog("%s(): Invalid calling context, CFE_ES_GetAppId() rc=%08x AppId=%lx, OwnerAppId=%lx\n",
                             __func__, (unsigned int)rc, CFE_RESOURCEID_TO_ULONG(AppId),
                             CFE_RESOURCEID_TO_ULONG(CFE_Assert_Global.OwnerAppId));
        return;
    }

    UtAssert_EndTest();

    OS_TaskDelay(CFE_TESTRUNNER_START_DELAY);

    /* Run all registered test cases. */
    UtTest_Run();

    /* unregister the callback and unset the appid */
    CFE_Assert_RegisterCallback(NULL);

    /* Release the access mutex so the next test program can run */
    rc = OS_MutSemGive(CFE_Assert_Global.AccessMutex);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): Error from OS_MutSemGive(): %d\n", __func__, (int)rc);
    }
}
