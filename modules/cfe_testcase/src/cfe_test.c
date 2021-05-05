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
** File: cfe_test.c
**
** Purpose:
**   Initialization routine for CFE functional test
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_assert.h"
#include "cfe_test.h"

/*
 * Test main function
 * Register this test routine with CFE Assert
 */
void CFE_TestMain(void)
{
    /*
     * Register this test app with CFE assert
     *
     * Note this also waits for the appropriate overall system
     * state and gets ownership of the UtAssert subsystem
     */
    CFE_Assert_RegisterTest("CFE API");

    /*
     * Register test cases in UtAssert
     */
    ESInfoTestSetup();
    ESTaskTestSetup();
    ESCDSTestSetup();
    FSHeaderTestSetup();
    TimeCurrentTestSetup();

    /*
     * Execute the tests
     *
     * Note this also releases ownership of the UtAssert subsystem when complete
     */
    CFE_Assert_ExecuteTest();

    /* Nothing more for this app to do */
    CFE_ES_ExitApp(CFE_ES_RunStatus_APP_EXIT);
}
