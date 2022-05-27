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
 *   Functional test of FS File Utility APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestFileCategory(void)
{
    UtPrintf("Testing: CFE_FS_GetDefaultMountPoint, CFE_FS_GetDefaultExtension");

    CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_UNKNOWN);
    CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_UNKNOWN);

    CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_DYNAMIC_MODULE);
    CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_DYNAMIC_MODULE);

    CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP);
    CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP);

    CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_TEXT_LOG);
    CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_TEXT_LOG);

    CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_SCRIPT);
    CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_SCRIPT);

    CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_TEMP);
    CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_TEMP);

    CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_MAX);
    CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_MAX);
}

void TestInputFile(void)
{
    char       NameBuf[OS_MAX_PATH_LEN];
    char       OutNameBuf[OS_MAX_PATH_LEN];
    const char Name[]         = "FileName";
    char       InNameBuf[]    = "BufferName";
    const char Path[]         = "/func";
    const char Ext[]          = ".test";
    const char ExpectedName[] = "/func/FileName.test";
    const char ExpectedBuf[]  = "/func/BufferName.test";

    UtPrintf("Testing: CFE_FS_ParseInputFileName, CFE_FS_ParseInputFileNameEX");

    UtAssert_INT32_EQ(CFE_FS_ParseInputFileName(NameBuf, Name, sizeof(NameBuf), CFE_FS_FileCategory_SCRIPT),
                      CFE_SUCCESS);
    UtAssert_NOT_NULL(strstr(NameBuf, Name));

    UtAssert_INT32_EQ(CFE_FS_ParseInputFileName(NULL, Name, sizeof(NameBuf), CFE_FS_FileCategory_SCRIPT),
                      CFE_FS_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileName(NameBuf, NULL, sizeof(NameBuf), CFE_FS_FileCategory_SCRIPT),
                      CFE_FS_INVALID_PATH);
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileName(NameBuf, Name, 0, CFE_FS_FileCategory_SCRIPT), CFE_FS_BAD_ARGUMENT);

    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(OutNameBuf, InNameBuf, sizeof(OutNameBuf), sizeof(InNameBuf), Name, Path, Ext),
        CFE_SUCCESS);
    UtAssert_StrCmp(ExpectedBuf, OutNameBuf, "Parse Input EX: %s", OutNameBuf);
    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(OutNameBuf, NULL, sizeof(OutNameBuf), sizeof(InNameBuf), Name, Path, Ext),
        CFE_SUCCESS);
    UtAssert_StrCmp(ExpectedName, OutNameBuf, "Parse Input EX: %s", OutNameBuf);
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutNameBuf, InNameBuf, sizeof(OutNameBuf), 0, Name, Path, Ext),
                      CFE_SUCCESS);
    UtAssert_StrCmp(ExpectedName, OutNameBuf, "Parse Input EX: %s", OutNameBuf);
    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(OutNameBuf, InNameBuf, sizeof(OutNameBuf), sizeof(InNameBuf), NULL, Path, Ext),
        CFE_SUCCESS);
    UtAssert_StrCmp(ExpectedBuf, OutNameBuf, "Parse Input EX: %s", OutNameBuf);

    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(NULL, InNameBuf, sizeof(OutNameBuf), sizeof(InNameBuf), Name, Path, Ext),
        CFE_FS_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutNameBuf, InNameBuf, 0, sizeof(InNameBuf), Name, Path, Ext),
                      CFE_FS_BAD_ARGUMENT);
    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(OutNameBuf, NULL, sizeof(OutNameBuf), sizeof(InNameBuf), NULL, Path, Ext),
        CFE_FS_INVALID_PATH);
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutNameBuf, InNameBuf, sizeof(OutNameBuf), 0, NULL, Path, Ext),
                      CFE_FS_INVALID_PATH);

    /* A short output buffer that is too small to fit the result */
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutNameBuf, InNameBuf, 8, sizeof(InNameBuf), Name, Path, Ext),
                      CFE_FS_FNAME_TOO_LONG);
}

void TestFileName(void)
{
    char       Path[OS_MAX_PATH_LEN + 4];
    char       Name[OS_MAX_FILE_NAME];
    const char ExpectedName[] = "FileName.test";

    memset(Name, 0, sizeof(Name));

    UtPrintf("Testing: CFE_FS_ExtractFilenameFromPath");

    snprintf(Path, sizeof(Path), "/func/FileName.test");
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(Path, Name), CFE_SUCCESS);
    UtAssert_StrCmp(Name, ExpectedName, "Extract Filename: %s", Name);

    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(NULL, Name), CFE_FS_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(Path, NULL), CFE_FS_BAD_ARGUMENT);

    memset(Path, 'x', sizeof(Path) - 1);
    Path[sizeof(Path) - 1] = 0;
    Path[0]                = '/';
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(Path, Name), CFE_FS_FNAME_TOO_LONG);

    Path[0]                   = 'x';
    Path[OS_MAX_PATH_LEN - 1] = 0;
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(Path, Name), CFE_FS_INVALID_PATH);
}

/* FT helper stub compatible with background file write DataGetter */
bool FS_DataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    *Buffer  = NULL;
    *BufSize = 0;
    return true;
}

/* FT helper stub compatible with background file write OnEvent */
void FS_OnEvent(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum, size_t BlockSize,
                size_t Position)
{
    OS_TaskDelay(100);
}

void TestFileDump(void)
{
    int32 count;
    int32 MaxWait = 20;

    memset(&CFE_FT_Global.FuncTestState, 0, sizeof(CFE_FT_Global.FuncTestState));
    CFE_FT_Global.FuncTestState.FileSubType = 2;
    strncpy(CFE_FT_Global.FuncTestState.Description, "FT", sizeof(CFE_FT_Global.FuncTestState.Description));

    UtPrintf("Testing: CFE_FS_BackgroundFileDumpRequest, CFE_FS_BackgroundFileDumpIsPending");

    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpIsPending(&CFE_FT_Global.FuncTestState), false);

    /* With an empty "FileName" field, it should fail path validation */
    CFE_FT_Global.FuncTestState.GetData = FS_DataGetter;
    CFE_FT_Global.FuncTestState.OnEvent = FS_OnEvent;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&CFE_FT_Global.FuncTestState), CFE_FS_INVALID_PATH);
    strncpy(CFE_FT_Global.FuncTestState.FileName, "/ram/FT.bin", sizeof(CFE_FT_Global.FuncTestState.FileName));

    /* With an empty "GetData" field, it should fail validation */
    CFE_FT_Global.FuncTestState.GetData = NULL;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&CFE_FT_Global.FuncTestState), CFE_FS_BAD_ARGUMENT);
    CFE_FT_Global.FuncTestState.GetData = FS_DataGetter;

    /* With an empty "OnEvent" field, it should fail validation */
    CFE_FT_Global.FuncTestState.OnEvent = NULL;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&CFE_FT_Global.FuncTestState), CFE_FS_BAD_ARGUMENT);
    CFE_FT_Global.FuncTestState.OnEvent = FS_OnEvent;

    /* This should work */
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&CFE_FT_Global.FuncTestState), CFE_SUCCESS);

    /* Duplicate request should get rejected */
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&CFE_FT_Global.FuncTestState),
                      CFE_STATUS_REQUEST_ALREADY_PENDING);

    /* Wait for background task to complete */
    count = 0;
    while (CFE_FS_BackgroundFileDumpIsPending(&CFE_FT_Global.FuncTestState) && count < MaxWait)
    {
        OS_TaskDelay(100);
        count++;
    }

    UtAssert_INT32_LT(count, MaxWait);

    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(NULL), CFE_FS_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpIsPending(NULL), false);
}

void FSUtilTestSetup(void)
{
    UtTest_Add(TestFileCategory, NULL, NULL, "Test File Category");
    UtTest_Add(TestInputFile, NULL, NULL, "Test Input File");
    UtTest_Add(TestFileName, NULL, NULL, "Test File Name");
    UtTest_Add(TestFileDump, NULL, NULL, "Test File Dump");
}
