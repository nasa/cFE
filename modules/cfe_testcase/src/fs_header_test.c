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
**   Functional test of basic FS Header APIs
**
**   Demonstration of how to register and use the UT assert functions.
**
*************************************************************************/

/*
 * Includes
 */

#include "cfe_test.h"

#define OS_TEST_HEADER_FILENAME "/drive0/header_test.txt"
char *fsAddrPtr = NULL;

static osal_id_t setup_file(void)
{
    osal_id_t id;
    OS_mkfs(fsAddrPtr, "/ramdev1", "RAM", 512, 20);
    OS_mount("/ramdev1", "/drive0");
    UtAssert_INT32_EQ(OS_OpenCreate(&id, OS_TEST_HEADER_FILENAME, OS_FILE_FLAG_CREATE, OS_READ_WRITE), OS_SUCCESS);
    return id;
}

void TestCreateHeader(void)
{
    CFE_FS_Header_t Header;
    const char *    TestDescription = "TEST_HEADER";
    osal_id_t       fd              = setup_file();

    UtPrintf("Testing: CFE_FS_InitHeader, CFE_FS_WriteHeader");

    CFE_FS_InitHeader(&Header, TestDescription, CFE_FS_SubType_ES_ERLOG);
    UtAssert_INT32_EQ(CFE_FS_WriteHeader(fd, &Header), sizeof(CFE_FS_Header_t));

    OS_close(fd);
    OS_remove(OS_TEST_HEADER_FILENAME);
}

void TestReadHeader(void)
{
    CFE_FS_Header_t Header;
    CFE_FS_Header_t ReadHeader;
    const char *    TestDescription = "TEST_HEADER";
    osal_id_t       fd              = setup_file();

    UtPrintf("Testing: CFE_FS_ReadHeader");

    CFE_FS_InitHeader(&Header, TestDescription, CFE_FS_SubType_ES_ERLOG);
    UtAssert_INT32_EQ(CFE_FS_WriteHeader(fd, &Header), sizeof(CFE_FS_Header_t));
    UtAssert_INT32_EQ(CFE_FS_ReadHeader(&ReadHeader, fd), sizeof(CFE_FS_Header_t));

    UtAssert_INT32_EQ(Header.ContentType, ReadHeader.ContentType);
    UtAssert_INT32_EQ(Header.SubType, ReadHeader.SubType);
    UtAssert_StrCmp(TestDescription, ReadHeader.Description, "ReadHeader.Description = %s", ReadHeader.Description);

    OS_close(fd);
    OS_remove(OS_TEST_HEADER_FILENAME);
}

void TestTimeStamp(void)
{
    CFE_FS_Header_t    Header;
    CFE_FS_Header_t    ReadHeader;
    const char *       TestDescription = "TEST_HEADER";
    CFE_TIME_SysTime_t NewTimestamp    = {10, 10};
    osal_id_t          fd              = setup_file();

    UtPrintf("Testing: CFE_FS_SetTimestamp");

    CFE_FS_InitHeader(&Header, TestDescription, CFE_FS_SubType_ES_ERLOG);
    UtAssert_INT32_EQ(CFE_FS_WriteHeader(fd, &Header), sizeof(CFE_FS_Header_t));
    UtAssert_INT32_EQ(CFE_FS_SetTimestamp(fd, NewTimestamp), OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_FS_ReadHeader(&ReadHeader, fd), sizeof(CFE_FS_Header_t));

    UtAssert_INT32_EQ(10, ReadHeader.TimeSeconds);
    UtAssert_INT32_EQ(10, ReadHeader.TimeSubSeconds);

    OS_close(fd);
    OS_remove(OS_TEST_HEADER_FILENAME);
}

void FSHeaderTestSetup(void)
{
    UtTest_Add(TestCreateHeader, NULL, NULL, "Test Create Header");
    UtTest_Add(TestReadHeader, NULL, NULL, "Test Read Header");
    UtTest_Add(TestTimeStamp, NULL, NULL, "Test Time Stamp");
}
