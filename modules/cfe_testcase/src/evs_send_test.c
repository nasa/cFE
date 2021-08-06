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
** File: evs_send_test.c
**
** Purpose:
**   Functional test of basic EVS Send Event APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestSendEvent(void)
{
    UtPrintf("Testing: CFE_EVS_SendEvent");

    UtAssert_INT32_EQ(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, "OK Send"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_EVS_SendEvent(0, CFE_EVS_EventType_INFORMATION, NULL), CFE_EVS_INVALID_PARAMETER);
}

void TestSendEventAppID(void)
{
    CFE_ES_AppId_t AppId;

    UtPrintf("Testing: CFE_EVS_SendEventWithAppID");

    CFE_ES_GetAppID(&AppId);

    UtAssert_INT32_EQ(CFE_EVS_SendEventWithAppID(0, CFE_EVS_EventType_INFORMATION, AppId, "OK App ID"), CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_EVS_SendEventWithAppID(0, CFE_EVS_EventType_INFORMATION, AppId, NULL),
                      CFE_EVS_INVALID_PARAMETER);
    UtAssert_INT32_EQ(CFE_EVS_SendEventWithAppID(0, CFE_EVS_EventType_INFORMATION, CFE_ES_APPID_UNDEFINED, "OK"),
                      CFE_EVS_APP_ILLEGAL_APP_ID);
}

void TestSendTimedEvent(void)
{
    CFE_TIME_SysTime_t Time = {1000, 1000};
    UtPrintf("Testing: CFE_EVS_SendTimedEvent");

    UtAssert_INT32_EQ(CFE_EVS_SendTimedEvent(Time, 0, CFE_EVS_EventType_INFORMATION, "OK Time"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_EVS_SendTimedEvent(Time, 0, CFE_EVS_EventType_INFORMATION, NULL), CFE_EVS_INVALID_PARAMETER);
}

void EVSSendTestSetup(void)
{
    UtTest_Add(TestSendEvent, NULL, NULL, "Test Send Event");
    UtTest_Add(TestSendEventAppID, NULL, NULL, "Test Send Event with App ID");
    UtTest_Add(TestSendTimedEvent, NULL, NULL, "Test Send Timed Event");
}
