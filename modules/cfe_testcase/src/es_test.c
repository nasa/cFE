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
** File: es_test.c
**
** Purpose:
**   Functional test of basic ES APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void ES_Test_AppId(void)
{
    CFE_ES_ResourceID_t AppId;
    char   AppNameBuf[OS_MAX_API_NAME + 4];

    UtAssert_INT32_EQ(CFE_ES_GetAppID(&AppId), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetAppName(AppNameBuf, AppId, sizeof(AppNameBuf)), CFE_SUCCESS);
    UtAssert_StrCmp(AppNameBuf, "TESTRUN_APP", "CFE_ES_GetAppName() returned TESTRUN_APP");
}
