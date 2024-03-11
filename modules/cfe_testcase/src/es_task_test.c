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
 *   Functional test of ES Child Tasks APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TaskFunction(void)
{
    while (CFE_FT_Global.Count < 200)
    {
        CFE_FT_Global.Count += 1;
        OS_TaskDelay(100);
    }
}

/* A task function that verifies the behavior of other APIs when those are called from a child task */
void TaskFunctionCheckChildTaskContext(void)
{
    CFE_ES_TaskId_t  TaskId;
    CFE_ES_AppId_t   AppId;
    CFE_ES_AppInfo_t AppInfo;

    /* extra startup delay before first assert, to make sure parent task has reached its wait loop */
    OS_TaskDelay(100);

    /* If invoked from the context of a child task, this should return an error */
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, "Test", TaskFunction, OSAL_TASK_STACK_ALLOCATE, 4096, 150, 0),
                      CFE_ES_ERR_CHILD_TASK_CREATE);

    /* Likewise attempting to delete the main task of the app from a child task should fail */
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppInfo(&AppInfo, AppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(AppInfo.MainTaskId), CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK);

    UtAssert_True(true, "CFE_ES_ExitChildTask() called");
    CFE_ES_ExitChildTask();
}

/* A task function that verifies the behavior of other APIs when those are called from a non-CFE app task */
void TaskFunctionCheckNonAppContext(void)
{
    CFE_ES_TaskId_t TaskId;
    CFE_ES_AppId_t  AppId;

    /* extra startup delay before first assert, to make sure parent task has reached its wait loop */
    OS_TaskDelay(100);

    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppId), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetTaskID(&TaskId), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&TaskId, "TaskName", TaskFunction, CFE_ES_TASK_STACK_ALLOCATE, 4096, 200, 0),
        CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UtAssert_True(true, "OS_TaskExit() called");
    OS_TaskExit();
}

void TaskExitFunction(void)
{
    while (CFE_FT_Global.Count < 200)
    {
        CFE_FT_Global.Count += 1;
        CFE_ES_ExitChildTask();
    }
}

#define UT_LOCAL_STACK_SIZE 4096
static unsigned long UT_LOCAL_STACK[UT_LOCAL_STACK_SIZE];

void TestCheckStackPointer(void)
{
    int32   LocalVar;
    cpuaddr VarAddress;
    cpuaddr StackAddress;

    OS_TaskDelay(10);

    VarAddress   = (cpuaddr)&LocalVar;
    StackAddress = (cpuaddr)UT_LOCAL_STACK;

    UtAssert_GT(cpuaddr, VarAddress, StackAddress);
    UtAssert_LT(cpuaddr, VarAddress, StackAddress + sizeof(UT_LOCAL_STACK));

    CFE_ES_ExitChildTask();
}

void TestCreateChildWithStack(void)
{
    CFE_ES_TaskId_t            TaskId = CFE_ES_TASKID_UNDEFINED;
    int32                      RetryCount;
    const char *               TaskName     = "CHILD_W_STACK";
    CFE_ES_StackPointer_t      StackPointer = UT_LOCAL_STACK;
    size_t                     StackSize    = sizeof(UT_LOCAL_STACK);
    CFE_ES_TaskPriority_Atom_t Priority     = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags        = 0;
    char                       TaskNameBuf[16];

    UtPrintf("Testing: CFE_ES_CreateChildTask with user-specified stack");

    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&TaskId, TaskName, TestCheckStackPointer, StackPointer, StackSize, Priority, Flags),
        CFE_SUCCESS);

    /* wait for task to exit itself */
    RetryCount = 0;
    while (RetryCount < 10)
    {
        /*
         * poll until CFE_ES_GetTaskName() returns an error, then the task has exited
         *
         * NOTE: this intentionally does not Assert the status here, because the child task is
         * also doing asserts at the time this loop is running.  Once the child task finishes,
         * it is OK to do asserts from this task again
         */
        if (CFE_Assert_STATUS_STORE(CFE_ES_GetTaskName(TaskNameBuf, TaskId, sizeof(TaskNameBuf))) != CFE_SUCCESS)
        {
            break;
        }
        OS_TaskDelay(100);
        ++RetryCount;
    }

    /* Retroactively confirm that the previous call to CFE_ES_GetTaskName() returned RESOURCEID_NOT_VALID */
    CFE_Assert_STATUS_MUST_BE(CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void TestCreateChild(void)
{
    UtPrintf("Testing: CFE_ES_CreateChildTask");

    CFE_ES_TaskId_t            TaskId        = CFE_ES_TASKID_UNDEFINED;
    CFE_ES_TaskId_t            TaskId2       = CFE_ES_TASKID_UNDEFINED;
    const char *               TaskName      = "CHILD_TASK_1";
    CFE_ES_StackPointer_t      StackPointer  = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize     = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority      = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags         = 0;
    uint32                     Index         = 0;
    int32                      ExpectedCount = 5;
    int32                      RetryCount;
    char                       TaskNameBuf[16];
    osal_id_t                  OtherTaskId = OS_OBJECT_ID_UNDEFINED;
    OS_task_prop_t             task_prop;

    CFE_FT_Global.Count = 0;
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_SUCCESS);
    while (CFE_FT_Global.Count != ExpectedCount && Index < 100)
    {
        OS_TaskDelay(10);
        Index++;
    }

    UtAssert_INT32_GT(CFE_FT_Global.Count, ExpectedCount - 1);
    UtAssert_INT32_LT(CFE_FT_Global.Count, ExpectedCount + 1);

    /* Create task with same name - note the name conflict is detected by OSAL, not CFE here, and the error code is
     * translated */
    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&TaskId2, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
        CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(TaskId), CFE_SUCCESS);

    /* Also Confirm behavior of child task create/delete when called from a child task */
    CFE_FT_Global.Count = 0;
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunctionCheckChildTaskContext, StackPointer,
                                             StackSize, Priority, Flags),
                      CFE_SUCCESS);

    /* wait for task to exit itself */
    RetryCount = 0;
    while (RetryCount < 10)
    {
        /*
         * poll until CFE_ES_GetTaskName() returns an error, then the task has exited
         *
         * NOTE: this intentionally does not Assert the status here, because the child task is
         * also doing asserts at the time this loop is running.  Once the child task finishes,
         * it is OK to do asserts from this task again
         */
        if (CFE_Assert_STATUS_STORE(CFE_ES_GetTaskName(TaskNameBuf, TaskId, sizeof(TaskNameBuf))) != CFE_SUCCESS)
        {
            break;
        }
        OS_TaskDelay(100);
        ++RetryCount;
    }

    /* Retroactively confirm that the previous call to CFE_ES_GetTaskName() returned RESOURCEID_NOT_VALID */
    CFE_Assert_STATUS_MUST_BE(CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* Now do the same but instead of a CFE child task, make an OSAL task that is not associated with a CFE app */
    UtAssert_INT32_EQ(OS_TaskCreate(&OtherTaskId, "NonCfe", TaskFunctionCheckNonAppContext, OSAL_TASK_STACK_ALLOCATE,
                                    4096, OSAL_PRIORITY_C(200), 0),
                      OS_SUCCESS);

    /* wait for task to exit itself */
    RetryCount = 0;
    while (RetryCount < 10)
    {
        /*
         * poll until OS_TaskGetInfo() returns an error, then the task has exited
         */
        if (OS_TaskGetInfo(OtherTaskId, &task_prop) != OS_SUCCESS)
        {
            break;
        }

        OS_TaskDelay(100);
        ++RetryCount;
    }

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(NULL, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, NULL, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, NULL, StackPointer, StackSize, Priority, Flags),
                      CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, 0, Priority, Flags),
                      CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
}

void TestChildTaskName(void)
{
    UtPrintf("Testing: CFE_ES_GetTaskIDByName, CFE_ES_GetTaskName");

    CFE_ES_TaskId_t            TaskId     = CFE_ES_TASKID_UNDEFINED;
    const char                 TaskName[] = "CHILD_TASK_1";
    CFE_ES_TaskId_t            TaskIdByName;
    char                       TaskNameBuf[OS_MAX_API_NAME + 4];
    CFE_ES_StackPointer_t      StackPointer = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize    = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority     = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags        = 0;

    memset(TaskNameBuf, 0, sizeof(TaskNameBuf));

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskIdByName, TaskName), CFE_SUCCESS);
    CFE_Assert_RESOURCEID_EQ(TaskIdByName, TaskId);

    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, TaskId, sizeof(TaskNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(TaskNameBuf, TaskName, "CFE_ES_GetTaskName() = %s", TaskNameBuf);

    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(NULL, TaskName), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskIdByName, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskIdByName, "INVALID_NAME"), CFE_ES_ERR_NAME_NOT_FOUND);
    CFE_Assert_RESOURCEID_UNDEFINED(TaskIdByName);

    UtAssert_INT32_EQ(CFE_ES_GetTaskName(NULL, TaskId, sizeof(TaskNameBuf)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, CFE_ES_TASKID_UNDEFINED, sizeof(TaskNameBuf)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, TaskId, 0), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(TaskNameBuf, TaskId, sizeof(TaskName) - 1), CFE_ES_BAD_ARGUMENT);

    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(TaskId), CFE_SUCCESS);
}

void TestChildTaskDelete(void)
{
    UtPrintf("Testing: CFE_ES_DeleteChildTask");

    CFE_ES_TaskId_t            TaskId        = CFE_ES_TASKID_UNDEFINED;
    const char *               TaskName      = "CHILD_TASK_1";
    CFE_ES_StackPointer_t      StackPointer  = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize     = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority      = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags         = 0;
    int                        ExpectedCount = 5;

    CFE_FT_Global.Count = 0;

    UtAssert_INT32_EQ(CFE_ES_CreateChildTask(&TaskId, TaskName, TaskFunction, StackPointer, StackSize, Priority, Flags),
                      CFE_SUCCESS);
    OS_TaskDelay(500);

    UtAssert_True(ExpectedCount >= CFE_FT_Global.Count - 1 && ExpectedCount <= CFE_FT_Global.Count + 1,
                  "countCopy (%d) == count (%d)", (int)ExpectedCount, (int)CFE_FT_Global.Count);

    ExpectedCount = CFE_FT_Global.Count;

    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(TaskId), CFE_SUCCESS);

    OS_TaskDelay(500);

    UtAssert_True(ExpectedCount == CFE_FT_Global.Count || ExpectedCount == CFE_FT_Global.Count + 1,
                  "ExpectedCount (%d) == count (%d)", (int)ExpectedCount, (int)CFE_FT_Global.Count);

    UtAssert_INT32_EQ(CFE_ES_DeleteChildTask(CFE_ES_TASKID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void TestExitChild(void)
{
    UtPrintf("Testing: CFE_ES_ExitChildTask");

    CFE_ES_TaskId_t            TaskId;
    const char *               TaskName      = "CHILD_TASK_1";
    CFE_ES_StackPointer_t      StackPointer  = CFE_ES_TASK_STACK_ALLOCATE;
    size_t                     StackSize     = CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE;
    CFE_ES_TaskPriority_Atom_t Priority      = CFE_PLATFORM_ES_PERF_CHILD_PRIORITY;
    uint32                     Flags         = 0;
    int                        ExpectedCount = 1;

    UtAssert_INT32_EQ(
        CFE_ES_CreateChildTask(&TaskId, TaskName, TaskExitFunction, StackPointer, StackSize, Priority, Flags),
        CFE_SUCCESS);
    OS_TaskDelay(500);
    UtAssert_INT32_EQ(ExpectedCount, 1);

    /*
     * Invoking CFE_ES_ExitChildTask() from the context of a main task should _not_ actually exit.
     * as this is a void function there is no return code to check here.  The fact that the test
     * continues after this call is evidence that the test passed (i.e. it did not actually end the task).
     */
    UtAssert_VOIDCALL(CFE_ES_ExitChildTask());

    /* If this message is printed, then the test passed.  If the test fails this will not be reached. */
    UtAssert_True(true, "CFE_ES_ExitChildTask() called from main task (ignored; main task did not exit)");
}

void ESTaskTestSetup(void)
{
    UtTest_Add(TestCreateChild, NULL, NULL, "Test Create Child");
    UtTest_Add(TestChildTaskName, NULL, NULL, "Test Child Task Name");
    UtTest_Add(TestChildTaskDelete, NULL, NULL, "Test Child Tasks Delete");
    UtTest_Add(TestExitChild, NULL, NULL, "Test Exit Child");

    /*
     * NOTE: The custom stack does not work on RTEMS, test is disabled on that platform
     * for the time being (custom stack may be deprecated in future CFE release).
     */
#ifndef _RTEMS_OS_
    UtTest_Add(TestCreateChildWithStack, NULL, NULL, "Test Child with Custom Stack");
#endif
}
