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
** File: es_info_test.c
**
** Purpose:
**   Functional test of FS File Utility APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

void TestFileCategory(void)
{
    UtPrintf("Testing: CFE_FS_GetDefaultMountPoint, CFE_FS_GetDefaultExtension");

    UtAssert_NULL(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_UNKNOWN));
    UtAssert_NULL(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_UNKNOWN));
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
}

void TestFileName(void)
{
    const char Path[] = "/func/FileName.test";
    char       Name[OS_MAX_FILE_NAME];
    const char ExpectedName[] = "FileName.test";

    UtPrintf("Testing: CFE_FS_ExtractFilenameFromPath");

    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(Path, Name), CFE_SUCCESS);
    UtAssert_StrCmp(Name, ExpectedName, "Extract Filename: %s", Name);

    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(NULL, Name), CFE_FS_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(Path, NULL), CFE_FS_BAD_ARGUMENT);
}

/* FT helper stub compatible with background file write DataGetter */
bool FS_DataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    return true;
}

/* FT helper stub compatible with background file write OnEvent */
void FS_OnEvent(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum, size_t BlockSize,
                size_t Position)
{
}

void TestFileDump(void)
{
    CFE_FS_FileWriteMetaData_t State;
    memset(&State, 0, sizeof(State));
    State.FileSubType = 2;
    State.GetData     = FS_DataGetter;
    State.OnEvent     = FS_OnEvent;
    strncpy(State.FileName, "/ram/FT.bin", sizeof(State.FileName));
    strncpy(State.Description, "FT", sizeof(State.Description));

    UtPrintf("Testing: CFE_FS_BackgroundFileDumpRequest, CFE_FS_BackgroundFileDumpIsPending");

    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpIsPending(&State), false);
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&State), CFE_SUCCESS);

    State.IsPending = true;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpIsPending(&State), true);

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
