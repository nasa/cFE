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
 *   Functional test of basic ES Application Behavior APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestRunCounter(void)
{
    CFE_ES_TaskId_t   TaskId;
    CFE_ES_TaskInfo_t TaskInfo;
    uint32            ExecutionCounter;
    uint32            RunStatus = CFE_ES_RunStatus_APP_RUN;

    UtPrintf("Testing: CFE_ES_RunLoop, CFE_ES_IncrementTaskCounter");

    UtAssert_INT32_EQ(CFE_ES_GetTaskID(&TaskId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&TaskInfo, TaskId), CFE_SUCCESS);
    ExecutionCounter = TaskInfo.ExecutionCounter;

    UtAssert_BOOL_TRUE(CFE_ES_RunLoop(&RunStatus));
    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&TaskInfo, TaskId), CFE_SUCCESS);
    UtAssert_UINT32_EQ(TaskInfo.ExecutionCounter, (ExecutionCounter + 1));

    UtAssert_BOOL_TRUE(CFE_ES_RunLoop(NULL));
    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&TaskInfo, TaskId), CFE_SUCCESS);
    UtAssert_UINT32_EQ(TaskInfo.ExecutionCounter, (ExecutionCounter + 2));

    UtAssert_VOIDCALL(CFE_ES_IncrementTaskCounter());
    UtAssert_INT32_EQ(CFE_ES_GetTaskInfo(&TaskInfo, TaskId), CFE_SUCCESS);
    UtAssert_UINT32_EQ(TaskInfo.ExecutionCounter, (ExecutionCounter + 3));

    RunStatus = CFE_ES_RunStatus_UNDEFINED;
    UtAssert_BOOL_FALSE(CFE_ES_RunLoop(&RunStatus));
}

void TestWaitBehavior(void)
{
    CFE_TIME_SysTime_t start;
    CFE_TIME_SysTime_t end;
    CFE_TIME_SysTime_t TimePassed;
    CFE_TIME_SysTime_t TimeExpected = {8, 0};

    start = CFE_TIME_GetTime();

    /* MinSystemStates of CFE_ES_SystemState_SHUTDOWN and higher not tested because they cause a shutdown */
    UtAssert_INT32_EQ(CFE_ES_WaitForSystemState(CFE_ES_SystemState_UNDEFINED, 10000), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_WaitForSystemState(CFE_ES_SystemState_EARLY_INIT, 10000), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_WaitForSystemState(CFE_ES_SystemState_CORE_STARTUP, 10000), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_WaitForSystemState(CFE_ES_SystemState_CORE_READY, 10000), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_WaitForSystemState(CFE_ES_SystemState_APPS_INIT, 10000), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_WaitForSystemState(CFE_ES_SystemState_OPERATIONAL, 10000), CFE_SUCCESS);
    UtAssert_VOIDCALL(CFE_ES_WaitForStartupSync(10000));

    end        = CFE_TIME_GetTime();
    TimePassed = CFE_TIME_Subtract(end, start);

    UtAssert_UINT32_EQ(CFE_TIME_Compare(TimePassed, TimeExpected), CFE_TIME_A_LT_B);
}

void ESBehaviorestSetup(void)
{
    UtTest_Add(TestRunCounter, NULL, NULL, "Test Run Counter");
    UtTest_Add(TestWaitBehavior, NULL, NULL, "Test Wait Behavior");
}
