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
 *   Implementation of the CFE assert (UT assert wrapper) functions.
 */

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
 * Opens a log file to "tee" the test output to
 */
int32 CFE_Assert_OpenLogFile(const char *Filename)
{
    int32  OsStatus;
    char * Ext;
    size_t NameLen;

    strncpy(CFE_Assert_Global.LogFileFinal, Filename, sizeof(CFE_Assert_Global.LogFileFinal) - 1);
    CFE_Assert_Global.LogFileFinal[sizeof(CFE_Assert_Global.LogFileFinal) - 1] = 0;

    strncpy(CFE_Assert_Global.LogFileTemp, Filename, sizeof(CFE_Assert_Global.LogFileTemp) - 1);
    CFE_Assert_Global.LogFileTemp[sizeof(CFE_Assert_Global.LogFileTemp) - 1] = 0;

    Ext = strrchr(CFE_Assert_Global.LogFileTemp, '.');
    if (Ext == NULL)
    {
        NameLen = strlen(CFE_Assert_Global.LogFileTemp);
    }
    else
    {
        NameLen = Ext - CFE_Assert_Global.LogFileTemp;
    }

    /* Use a ".tmp" file while actively writing, will rename at the end */
    if (NameLen > (sizeof(CFE_Assert_Global.LogFileTemp) - 5))
    {
        NameLen = sizeof(CFE_Assert_Global.LogFileTemp) - 5;
    }
    strcpy(&CFE_Assert_Global.LogFileTemp[NameLen], ".tmp");

    OsStatus = OS_OpenCreate(&CFE_Assert_Global.LogFileDesc, CFE_Assert_Global.LogFileTemp,
                             OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Failed to open %s, rc=%ld\n", __func__, CFE_Assert_Global.LogFileTemp,
                             (long)OsStatus);
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return CFE_SUCCESS;
}

/*
 * Closes the log file
 * This also renames the intermediate log file to its final name
 */
void CFE_Assert_CloseLogFile(void)
{
    if (OS_ObjectIdDefined(CFE_Assert_Global.LogFileDesc))
    {
        OS_close(CFE_Assert_Global.LogFileDesc);
        OS_rename(CFE_Assert_Global.LogFileTemp, CFE_Assert_Global.LogFileFinal);
    }

    CFE_Assert_Global.LogFileDesc     = OS_OBJECT_ID_UNDEFINED;
    CFE_Assert_Global.LogFileTemp[0]  = 0;
    CFE_Assert_Global.LogFileFinal[0] = 0;
}

/*
 * Initialization Function for this library
 */
int32 CFE_Assert_LibInit(CFE_ES_LibId_t LibId)
{
    int32 OsStatus;

    memset(&CFE_Assert_Global, 0, sizeof(CFE_Assert_Global));

    UtTest_EarlyInit();
    UT_BSP_Setup();

    OsStatus = OS_MutSemCreate(&CFE_Assert_Global.AccessMutex, "CFE_Assert", 0);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: OS_MutSemCreate failed, rc=%ld\n", __func__, (long)OsStatus);
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
