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
** File: cfe_assert_main.c
**
** Purpose:
**   Implementation of the CFE assert (UT assert wrapper) functions.
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
 * Allows the test reports to be redirected to another destination
 */
void CFE_Assert_RegisterCallback(CFE_Assert_StatusCallback_t Callback)
{
    CFE_Assert_Global.StatusCallback = Callback;
}

/*
 * Initialization Function for this library
 */
int32 CFE_Assert_LibInit(CFE_ES_LibId_t LibId)
{
    int32 status;

    memset(&CFE_Assert_Global, 0, sizeof(CFE_Assert_Global));

    UtTest_EarlyInit();
    UT_BSP_Setup();

    status = OS_MutSemCreate(&CFE_Assert_Global.AccessMutex, "CFE_Assert", 0);
    if (status != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): OS_MutSemCreate failed, rc=%d\n", __func__, (int)status);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /*
     * Start a test case for all startup logic.
     *
     * Test libs may use assert statements within their init function and these
     * will be reported as a "startup" test case.
     */
    UtAssert_BeginTest("CFE-STARTUP");

    return CFE_SUCCESS;
}
