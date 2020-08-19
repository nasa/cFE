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

#include <cfe.h>

#include "cfe_testrunner.h"

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
#define CFE_TESTRUNNER_START_DELAY 4000

/*
 * Entry point for this application
 */
void CFE_TestRunner_AppMain(void)
{
    int32  rc;
    uint32 RunStatus;

    /*
    ** Register the app with Executive services
    */
    rc = CFE_ES_RegisterApp();
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Error in CFE_ES_RegisterApp(): %08lx\n", (unsigned long)rc);
        return;
    }

    /*
     * Delay until the system reaches "operational" state -- this is when all libs have initialized
     * and all apps have reached their RunLoop.
     */
    rc = CFE_ES_WaitForSystemState(CFE_ES_SystemState_OPERATIONAL, CFE_TESTRUNNER_MAX_STARTUP_WAIT);
    if (rc != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("Error while waiting for OPERATIONAL state: %08lx\n", (unsigned long)rc);
        return;
    }

    /*
     * Startup Phase has ended.
     */
    UtAssert_EndTest();

    /*
     * Note - in a normal app this would be a while loop,
     * but is just an "if" right now as it only runs once.
     *
     * A future enhancement to this app might be to create an SB pipe
     * and execute tests based on the receipt of a command to do so.
     *
     * For now, it just works in a one-shot mode to run tests that were
     * registered during startup, then it self-exits.
     */
    RunStatus = CFE_ES_RunStatus_APP_RUN;
    if (CFE_ES_RunLoop(&RunStatus))
    {
        OS_TaskDelay(CFE_TESTRUNNER_START_DELAY);

        /*
         * Run all registered test cases.
         */
        UtTest_Run();

        /*
         * Exit the main task.
         */
        RunStatus = CFE_ES_RunStatus_APP_EXIT;
    }

    CFE_ES_ExitApp(RunStatus);
}
