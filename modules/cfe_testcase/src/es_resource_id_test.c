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
 *   Functional test of ES Resource ID APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestAppIDToIndex(void)
{
    UtPrintf("Testing: CFE_ES_AppID_ToIndex");
    CFE_ES_AppId_t TestAppId;
    uint32         TestAppIdx;
    uint32         idx;
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&TestAppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_AppID_ToIndex(TestAppId, &TestAppIdx), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_AppID_ToIndex(TestAppId, &idx), CFE_SUCCESS);
    UtAssert_INT32_LTEQ(TestAppIdx, CFE_PLATFORM_ES_MAX_APPLICATIONS);
    UtAssert_UINT32_EQ(idx, TestAppIdx);

    UtAssert_INT32_EQ(CFE_ES_AppID_ToIndex(TestAppId, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_AppID_ToIndex(CFE_ES_APPID_UNDEFINED, &TestAppIdx), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void TestLibIDToIndex(void)
{
    UtPrintf("Testing: CFE_ES_LibID_ToIndex");
    const char *   LibName = "ASSERT_LIB";
    CFE_ES_LibId_t LibId;
    uint32         LibIdx = 0;
    uint32         idx    = 0;

    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&LibId, LibName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_LibID_ToIndex(LibId, &LibIdx), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_LibID_ToIndex(LibId, &idx), CFE_SUCCESS);
    UtAssert_INT32_LTEQ(LibIdx, CFE_PLATFORM_ES_MAX_LIBRARIES);
    UtAssert_UINT32_EQ(idx, LibIdx);

    UtAssert_INT32_EQ(CFE_ES_LibID_ToIndex(LibId, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_LibID_ToIndex(CFE_ES_LIBID_UNDEFINED, &LibIdx), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void TestTaskIDToIndex(void)
{
    CFE_ES_TaskId_t TaskId  = CFE_ES_TASKID_UNDEFINED;
    uint32          TaskIdx = 0;
    uint32          idx     = 0;

    UtPrintf("Testing: CFE_ES_TaskID_ToIndex");

    UtAssert_INT32_EQ(CFE_ES_GetTaskID(&TaskId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_TaskID_ToIndex(TaskId, &TaskIdx), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_TaskID_ToIndex(TaskId, &idx), CFE_SUCCESS);
    UtAssert_INT32_LTEQ(TaskIdx, OS_MAX_TASKS);
    UtAssert_UINT32_EQ(idx, TaskIdx);

    UtAssert_INT32_EQ(CFE_ES_TaskID_ToIndex(TaskId, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_TaskID_ToIndex(CFE_ES_TASKID_UNDEFINED, &TaskIdx), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void TestCounterIDToIndex(void)
{
    UtPrintf("Testing: CFE_ES_CounterID_ToIndex");
    const char *       CounterName = "TEST_COUNTER";
    CFE_ES_CounterId_t CounterId;
    uint32             CounterIdx;
    uint32             idx;
    UtAssert_UINT32_EQ(CFE_ES_RegisterGenCounter(&CounterId, CounterName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_CounterID_ToIndex(CounterId, &CounterIdx), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_CounterID_ToIndex(CounterId, &idx), CFE_SUCCESS);
    UtAssert_INT32_LTEQ(CounterIdx, CFE_PLATFORM_ES_MAX_GEN_COUNTERS);
    UtAssert_UINT32_EQ(idx, CounterIdx);

    UtAssert_INT32_EQ(CFE_ES_CounterID_ToIndex(CounterId, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_CounterID_ToIndex(CFE_ES_COUNTERID_UNDEFINED, &CounterIdx),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* Unregister Counter */
    UtAssert_INT32_EQ(CFE_ES_DeleteGenCounter(CounterId), CFE_SUCCESS);
}

void ESResourceIDTestSetup(void)
{
    UtTest_Add(TestAppIDToIndex, NULL, NULL, "Test Obtaining indices from App ID");
    UtTest_Add(TestLibIDToIndex, NULL, NULL, "Test Obtaining indices from Lib ID");
    UtTest_Add(TestTaskIDToIndex, NULL, NULL, "Test Obtaining indices from Task ID");
    UtTest_Add(TestCounterIDToIndex, NULL, NULL, "Test Obtaining indices from Counter ID");
}
