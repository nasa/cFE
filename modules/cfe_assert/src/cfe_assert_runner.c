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
 *   Implementation of the CFE testrunner (UT testrunner wrapper) functions.
 */

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

bool CFE_Assert_StatusCheck(CFE_Status_t Status, bool ExpectSuccess, UtAssert_CaseType_t CaseType, const char *File,
                            uint32 Line, const char *Text)
{
    bool        Result = (Status >= CFE_SUCCESS);
    const char *MatchText;

    if (ExpectSuccess)
    {
        MatchText = "OK";
    }
    else
    {
        /* expecting non-success; result should be inverted */
        Result    = !Result;
        MatchText = "ERROR";
    }

    return UtAssertEx(Result, CaseType, File, Line, "%s (0x%lx) is %s", Text, (unsigned long)Status, MatchText);
}

CFE_Status_t CFE_Assert_Status_Store(CFE_Status_t Status, const char *File, uint32 Line, const char *Text)
{
    const char *BaseName;

    /* All this needs to do is save the code+text, will assert later */
    CFE_Assert_Global.StoredStatus = Status;
    strncpy(CFE_Assert_Global.StoredText, Text, sizeof(CFE_Assert_Global.StoredText) - 1);
    CFE_Assert_Global.StoredText[sizeof(CFE_Assert_Global.StoredText) - 1] = 0;

    BaseName = strrchr(File, '/');
    if (BaseName == NULL)
    {
        BaseName = File;
    }
    else
    {
        ++BaseName;
    }
    strncpy(CFE_Assert_Global.StoredFile, BaseName, sizeof(CFE_Assert_Global.StoredFile) - 1);
    CFE_Assert_Global.StoredFile[sizeof(CFE_Assert_Global.StoredFile) - 1] = 0;
    CFE_Assert_Global.StoredLine                                           = Line;

    /* Status code is just passed thru so the test case can check it however it needs to */
    return Status;
}

bool CFE_Assert_Status_SilentCheck(CFE_Status_t Status)
{
    return (Status == CFE_Assert_Global.StoredStatus);
}

bool CFE_Assert_Status_DeferredCheck(CFE_Status_t Status, UtAssert_CaseType_t CaseType, const char *File, uint32 Line,
                                     const char *Text)
{
    bool        Result;
    const char *ExtraTag;

    if (CFE_Assert_Global.StoredText[0] == 0)
    {
        /* If no status was stored, then this is a bug in the test program (need to store a result first) */
        UtAssertEx(false, UTASSERT_CASETYPE_FAILURE, File, Line, "TEST BUG: No stored status to assert (%s)", Text);
        Result = false;
    }
    else
    {
        Result = CFE_Assert_Status_SilentCheck(Status);
        if (Result)
        {
            /* no extra tag added to "true" conditions */
            ExtraTag = "";
        }
        else
        {
            /* if condition was false add an exta marker so user does not necessarily need to decode the string */
            ExtraTag = " [false]";
        }

        /* This produces a log message similar to what UtAssert_INT32_EQ would produce.
         * Note the file/line will reflect where the call was made, not where this assertion was done */
        Result = UtAssertEx(Result, CaseType, CFE_Assert_Global.StoredFile, CFE_Assert_Global.StoredLine,
                            "%s (%ld) == %s (%ld)%s", CFE_Assert_Global.StoredText,
                            (long)CFE_Assert_Global.StoredStatus, Text, (long)Status, ExtraTag);
    }

    return Result;
}

void CFE_Assert_StatusReport(uint8 MessageType, const char *Prefix, const char *OutputMessage)
{
    CFE_EVS_EventType_Enum_t EventType;

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
    int32          rc;
    char           SetupSegmentName[64];
    CFE_ES_AppId_t SelfId;

    rc = CFE_EVS_Register(CFE_TR_EventFilters, sizeof(CFE_TR_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                          CFE_EVS_EventFilter_BINARY);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Error from CFE_EVS_Register: %08lx\n", __func__, (unsigned long)rc);
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
        CFE_ES_WriteToSysLog("%s: Error while waiting for OPERATIONAL state: %08lx\n", __func__, (unsigned long)rc);
        return rc;
    }

    rc = CFE_ES_GetAppID(&SelfId);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): Error from CFE_ES_GetAppId(): %08x\n", __func__, (unsigned int)rc);
        return rc;
    }

    /*
     * Acquire the mutex.  This is needed because UtAssert and its data structures are not thread-safe.
     * Only one test app should use UtAssert facilities at a given time.
     */
    UT_BSP_Lock();

    /*
     * Wait here until "OwnerAppId" is available/undefined
     */
    while (CFE_RESOURCEID_TEST_DEFINED(CFE_Assert_Global.OwnerAppId))
    {
        UT_BSP_Unlock();
        OS_TaskDelay(100);
        UT_BSP_Lock();
    }

    /*
     * After acquiring mutex, record the fact that this app now owns the assert functions
     */
    CFE_Assert_Global.OwnerAppId = SelfId;

    UT_BSP_Unlock();

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
        CFE_ES_WriteToSysLog("%s: Invalid calling context, CFE_ES_GetAppId() rc=%08x AppId=%lx, OwnerAppId=%lx\n",
                             __func__, (unsigned int)rc, CFE_RESOURCEID_TO_ULONG(AppId),
                             CFE_RESOURCEID_TO_ULONG(CFE_Assert_Global.OwnerAppId));
        return;
    }

    UtAssert_EndTest();

    OS_TaskDelay(CFE_TESTRUNNER_START_DELAY);

    /* Run all registered test cases. */
    UtTest_Run();

    /* unregister the callback and unset the appid */
    UT_BSP_Lock();
    CFE_Assert_RegisterCallback(NULL);
    CFE_Assert_CloseLogFile();
    CFE_Assert_Global.OwnerAppId = CFE_ES_APPID_UNDEFINED;
    UT_BSP_Unlock();
}
