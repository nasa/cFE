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
 *   Functional test of Miscellaneous Resource Id APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"
/* Needed for CFE_RESOURCEID_MAKE_BASE macro */
#include "cfe_resourceid_basevalue.h"

bool TestReturnFalse(CFE_ResourceId_t CheckId)
{
    return false;
}

bool TestReturnTrue(CFE_ResourceId_t CheckId)
{
    return true;
}

void TestToFromInteger(void)
{
    UtPrintf("Testing: CFE_ResourceId_ToInteger, CFE_ResourceId_FromInteger, CFE_ResourceId_Equal");

    /* Test integer -> resource ID -> integer */
    uint32           Id1         = 1;
    uint32           Id2         = 999;
    CFE_ResourceId_t ResourceId1 = CFE_ResourceId_FromInteger(Id1);
    CFE_ResourceId_t ResourceId2 = CFE_ResourceId_FromInteger(Id2);
    UtAssert_UINT32_EQ(CFE_ResourceId_ToInteger(ResourceId1), Id1);
    UtAssert_UINT32_EQ(CFE_ResourceId_ToInteger(ResourceId2), Id2);

    /* Test resource ID -> integer -> resource ID */
    Id1 = CFE_ResourceId_ToInteger(ResourceId1);
    Id2 = CFE_ResourceId_ToInteger(ResourceId2);
    CFE_Assert_RESOURCEID_EQ((CFE_RESOURCEID_BASE_TYPE)CFE_RESOURCEID_WRAP(CFE_ResourceId_FromInteger(Id1)),
                             (CFE_RESOURCEID_BASE_TYPE)CFE_RESOURCEID_WRAP(ResourceId1));
    CFE_Assert_RESOURCEID_EQ((CFE_RESOURCEID_BASE_TYPE)CFE_RESOURCEID_WRAP(CFE_ResourceId_FromInteger(Id2)),
                             (CFE_RESOURCEID_BASE_TYPE)CFE_RESOURCEID_WRAP(ResourceId2));
}

void TestIsDefined(void)
{
    UtPrintf("Testing: CFE_ResourceId_IsDefined");
    CFE_ResourceId_t ResourceId1 = CFE_ResourceId_FromInteger(1);
    CFE_ResourceId_t ResourceId2 = CFE_ResourceId_FromInteger(999);
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(ResourceId1));
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(ResourceId2));
    UtAssert_BOOL_FALSE(CFE_ResourceId_IsDefined(CFE_RESOURCEID_UNDEFINED));
}

void TestGetBaseSerial(void)
{
    UtPrintf("Testing: CFE_ResourceId_GetBase, CFE_ResourceId_GetSerial");
    const char *LibName     = "ASSERT_LIB";
    const char *CounterName = "TEST_COUNTER";
    int8        Pool[1024];
    size_t      CDSBlockSize = 10;
    const char *CDSName      = "TEST_CDS";
    union
    {
        CFE_ES_AppId_t   AppId;
        CFE_ResourceId_t ResourceID;
    } AppIdBuf;
    union
    {
        CFE_ES_LibId_t   LibId;
        CFE_ResourceId_t ResourceID;
    } LibIdBuf;
    union
    {
        CFE_ES_TaskId_t  TaskId;
        CFE_ResourceId_t ResourceID;
    } TaskIdBuf;
    union
    {
        CFE_ES_CounterId_t CounterId;
        CFE_ResourceId_t   ResourceID;
    } CounterIdBuf;
    union
    {
        CFE_ES_MemHandle_t PoolId;
        CFE_ResourceId_t   ResourceID;
    } PoolIdBuf;
    union
    {
        CFE_ES_CDSHandle_t CDSHandleId;
        CFE_ResourceId_t   ResourceID;
    } CDSHandleIdBuf;

    memset(&AppIdBuf, 0, sizeof(AppIdBuf));
    memset(&LibIdBuf, 0, sizeof(LibIdBuf));
    memset(&TaskIdBuf, 0, sizeof(TaskIdBuf));
    memset(&CounterIdBuf, 0, sizeof(CounterIdBuf));
    memset(&PoolIdBuf, 0, sizeof(PoolIdBuf));
    memset(&CDSHandleIdBuf, 0, sizeof(CDSHandleIdBuf));

    /* Referenced from cfe_core_resourceid_basevalues.h */
    int TASKID_BASE     = CFE_RESOURCEID_MAKE_BASE(OS_OBJECT_TYPE_OS_TASK);
    int APPID_BASE      = CFE_RESOURCEID_MAKE_BASE(OS_OBJECT_TYPE_USER + 1);
    int LIBID_BASE      = CFE_RESOURCEID_MAKE_BASE(OS_OBJECT_TYPE_USER + 2);
    int COUNTID_BASE    = CFE_RESOURCEID_MAKE_BASE(OS_OBJECT_TYPE_USER + 3);
    int POOLID_BASE     = CFE_RESOURCEID_MAKE_BASE(OS_OBJECT_TYPE_USER + 4);
    int CDSBLOCKID_BASE = CFE_RESOURCEID_MAKE_BASE(OS_OBJECT_TYPE_USER + 5);

    /* App ID */
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppIdBuf.AppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ResourceId_GetBase(AppIdBuf.ResourceID), APPID_BASE);
    UtAssert_INT32_EQ(CFE_ResourceId_GetSerial(AppIdBuf.ResourceID),
                      CFE_RESOURCEID_UNWRAP(AppIdBuf.ResourceID) - APPID_BASE);
    /* Lib ID */
    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&LibIdBuf.LibId, LibName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ResourceId_GetBase(LibIdBuf.ResourceID), LIBID_BASE);
    UtAssert_INT32_EQ(CFE_ResourceId_GetSerial(LibIdBuf.ResourceID),
                      CFE_RESOURCEID_UNWRAP(LibIdBuf.ResourceID) - LIBID_BASE);
    /* Task ID */
    UtAssert_INT32_EQ(CFE_ES_GetTaskID(&TaskIdBuf.TaskId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ResourceId_GetBase(TaskIdBuf.ResourceID), TASKID_BASE);
    UtAssert_INT32_EQ(CFE_ResourceId_GetSerial(TaskIdBuf.ResourceID),
                      CFE_RESOURCEID_UNWRAP(TaskIdBuf.ResourceID) - TASKID_BASE);
    /* Counter ID */
    UtAssert_UINT32_EQ(CFE_ES_RegisterGenCounter(&CounterIdBuf.CounterId, CounterName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ResourceId_GetBase(CounterIdBuf.ResourceID), COUNTID_BASE);
    UtAssert_INT32_EQ(CFE_ResourceId_GetSerial(CounterIdBuf.ResourceID),
                      CFE_RESOURCEID_UNWRAP(CounterIdBuf.ResourceID) - COUNTID_BASE);
    UtAssert_INT32_EQ(CFE_ES_DeleteGenCounter(CounterIdBuf.CounterId), CFE_SUCCESS);
    /* Pool ID */
    UtAssert_INT32_EQ(CFE_ES_PoolCreate(&PoolIdBuf.PoolId, Pool, sizeof(Pool)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ResourceId_GetBase(PoolIdBuf.ResourceID), POOLID_BASE);
    UtAssert_INT32_EQ(CFE_ResourceId_GetSerial(PoolIdBuf.ResourceID),
                      CFE_RESOURCEID_UNWRAP(PoolIdBuf.ResourceID) - POOLID_BASE);
    UtAssert_INT32_EQ(CFE_ES_PoolDelete(PoolIdBuf.PoolId), CFE_SUCCESS);
    /* CDS Block Id */
    CFE_Assert_STATUS_OK(CFE_ES_RegisterCDS(&CDSHandleIdBuf.CDSHandleId, CDSBlockSize, CDSName));
    UtAssert_INT32_EQ(CFE_ResourceId_GetBase(CDSHandleIdBuf.ResourceID), CDSBLOCKID_BASE);
    UtAssert_INT32_EQ(CFE_ResourceId_GetSerial(CDSHandleIdBuf.ResourceID),
                      CFE_RESOURCEID_UNWRAP(CDSHandleIdBuf.ResourceID) - CDSBLOCKID_BASE);
}

void TestFindNext(void)
{
    UtPrintf("Testing: CFE_ResourceId_FindNext");
    union
    {
        CFE_ES_AppId_t   AppId;
        CFE_ResourceId_t ResourceID;
    } AppIdBuf;
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppIdBuf.AppId), CFE_SUCCESS);
    /*
     * The FindNext API requires a callback function to determine if a resource id is already in use. This test provides
     * two local callback functions TestReturnFalse and TestReturnTrue to test the basic functionality of this API.
     */
    UtAssert_INT32_EQ(CFE_ResourceId_ToInteger(CFE_ResourceId_FindNext(
                          AppIdBuf.ResourceID, CFE_PLATFORM_ES_MAX_APPLICATIONS, TestReturnFalse)),
                      CFE_RESOURCEID_UNWRAP(AppIdBuf.ResourceID) + 1);
    /* Pretend every application id is used */
    AppIdBuf.ResourceID =
        CFE_ResourceId_FindNext(AppIdBuf.ResourceID, CFE_PLATFORM_ES_MAX_APPLICATIONS, TestReturnTrue);
    /*
     * Why does this macro not accept a resource ID
     * The following line won't compile
     * CFE_Assert_RESOURCEID_UNDEFINED(AppIdBuf.ResourceId);
     */
    CFE_Assert_RESOURCEID_UNDEFINED(AppIdBuf.AppId);

    /* maximum number of applications is 0 */
    AppIdBuf.ResourceID = CFE_ResourceId_FindNext(AppIdBuf.ResourceID, 0, TestReturnFalse);
    CFE_Assert_RESOURCEID_UNDEFINED(AppIdBuf.AppId);
}

void TestToIndex(void)
{
    UtPrintf("Testing: CFE_ResourceId_ToIndex");
    union
    {
        CFE_ES_AppId_t   AppId;
        CFE_ResourceId_t ResourceID;
    } AppIdBuf;
    uint32 idx;
    int    APPID_BASE = CFE_RESOURCEID_MAKE_BASE(OS_OBJECT_TYPE_USER + 1);
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppIdBuf.AppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(AppIdBuf.ResourceID, APPID_BASE, CFE_PLATFORM_ES_MAX_APPLICATIONS, NULL),
                      CFE_ES_BAD_ARGUMENT);
    /* pretend maximum number of applications is 0 */
    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(AppIdBuf.ResourceID, APPID_BASE, 0, &idx),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ResourceId_ToIndex(AppIdBuf.ResourceID, APPID_BASE, CFE_PLATFORM_ES_MAX_APPLICATIONS, &idx),
                      CFE_SUCCESS);
    UtAssert_INT32_LTEQ(idx, CFE_PLATFORM_ES_MAX_APPLICATIONS);
}

void ResourceIdMiscTestSetup(void)
{
    UtTest_Add(TestToFromInteger, NULL, NULL, "Test Resource Id to Integer");
    UtTest_Add(TestIsDefined, NULL, NULL, "Test Resource Id is Defined");
    UtTest_Add(TestGetBaseSerial, NULL, NULL, "Test Resource Id Get Base");
    UtTest_Add(TestFindNext, NULL, NULL, "Test Resource Id Find Next");
    UtTest_Add(TestToIndex, NULL, NULL, "Test Resource Id to Index");
}
