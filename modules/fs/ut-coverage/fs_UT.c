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

/*
** File:
**    fs_UT.c
**
** Purpose:
**    File Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/

/*
** Includes
*/
#include "fs_UT.h"

#include "target_config.h"

const char *FS_SYSLOG_MSGS[] = {NULL, "%s: SharedData Mutex Take Err Stat=%ld,App=%lu,Function=%s\n",
                                "%s: SharedData Mutex Give Err Stat=%ld,App=%lu,Function=%s\n"};

/* counts the number of times UT_FS_OnEvent() was invoked (below) */
uint32 UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_MAX];

/* UT helper stub compatible with background file write DataGetter */
bool UT_FS_DataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    UT_GetDataBuffer(UT_KEY(UT_FS_DataGetter), Buffer, BufSize, NULL);
    return UT_DEFAULT_IMPL(UT_FS_DataGetter);
}

/* UT helper stub compatible with background file write OnEvent */
void UT_FS_OnEvent(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum, size_t BlockSize,
                   size_t Position)
{
    ++UT_FS_FileWriteEventCount[Event];
    UT_DEFAULT_IMPL(UT_FS_OnEvent);
}

/*
** Functions
*/
void UtTest_Setup(void)
{
    /* Initialize unit test */
    UT_Init("fs");
    UtPrintf("cFE FS Unit Test Output File\n\n");

    /* Perform tests */
    UT_ADD_TEST(Test_CFE_FS_InitHeader);
    UT_ADD_TEST(Test_CFE_FS_ReadHeader);
    UT_ADD_TEST(Test_CFE_FS_WriteHeader);
    UT_ADD_TEST(Test_CFE_FS_SetTimestamp);
    UT_ADD_TEST(Test_CFE_FS_DefaultFileStrings);
    UT_ADD_TEST(Test_CFE_FS_ByteSwapCFEHeader);
    UT_ADD_TEST(Test_CFE_FS_ByteSwapUint32);
    UT_ADD_TEST(Test_CFE_FS_ParseInputFileNameEx);
    UT_ADD_TEST(Test_CFE_FS_ExtractFileNameFromPath);
    UT_ADD_TEST(Test_CFE_FS_Private);

    UT_ADD_TEST(Test_CFE_FS_BackgroundFileDump);
}

/*
** Test FS API initialize header function
*/
void Test_CFE_FS_InitHeader(void)
{
    CFE_FS_Header_t Hdr;

    /* Test initializing the header */
    UT_InitData();
    CFE_FS_InitHeader(&Hdr, "description", 123);
    UtAssert_INT32_EQ(Hdr.SubType, 123);

    /* Test calling with NULL pointer argument (no return codes) */
    CFE_FS_InitHeader(NULL, "description", 123);
    CFE_FS_InitHeader(&Hdr, NULL, 123);
}

/*
** Test FS API read header function
*/
void Test_CFE_FS_ReadHeader(void)
{
    osal_id_t       FileDes = OS_OBJECT_ID_UNDEFINED;
    CFE_FS_Header_t Hdr;

    UtPrintf("Begin Test Read Header");

    /* Test reading the header with a lseek failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), OS_ERROR);
    UtAssert_INT32_EQ(CFE_FS_ReadHeader(&Hdr, FileDes), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test reading header with OS_read failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_read), OS_ERROR);
    UtAssert_INT32_EQ(CFE_FS_ReadHeader(&Hdr, FileDes), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test partial success with reading header */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_read), 1);
    UtAssert_INT32_EQ(CFE_FS_ReadHeader(&Hdr, FileDes), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test successfully reading the header */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_FS_ReadHeader(&Hdr, FileDes), sizeof(Hdr));

    /* Test calling with NULL pointer argument */
    UtAssert_INT32_EQ(CFE_FS_ReadHeader(NULL, FileDes), CFE_FS_BAD_ARGUMENT);
}

/*
** Test FS API write header function
*/
void Test_CFE_FS_WriteHeader(void)
{
    osal_id_t       FileDes = OS_OBJECT_ID_UNDEFINED;
    CFE_FS_Header_t Hdr;

    UtPrintf("Begin Test Write Header");

    /* Test writing the header with a lseek failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), OS_ERROR);
    UtAssert_INT32_EQ(CFE_FS_WriteHeader(FileDes, &Hdr), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test successfully writing the header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_lseek), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, OS_SUCCESS);
    UtAssert_INT32_EQ(CFE_FS_WriteHeader(FileDes, &Hdr), CFE_SUCCESS);

    /* Test calling with NULL pointer argument */
    UtAssert_INT32_EQ(CFE_FS_WriteHeader(FileDes, NULL), CFE_FS_BAD_ARGUMENT);
}

/*
** Test FS API set time stamp function
*/
void Test_CFE_FS_SetTimestamp(void)
{
    osal_id_t          FileDes      = OS_OBJECT_ID_UNDEFINED;
    CFE_TIME_SysTime_t NewTimestamp = {0, 0};

    UtPrintf("Begin Test Set Time Stamp");

    /* Test setting the time stamp with a lseek failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), OS_ERROR);
    UtAssert_INT32_EQ(CFE_FS_SetTimestamp(FileDes, NewTimestamp), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test setting the time stamp with a seconds write failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    UtAssert_INT32_EQ(CFE_FS_SetTimestamp(FileDes, NewTimestamp), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test setting the time stamp with a subseconds write failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, 0);
    UtAssert_INT32_EQ(CFE_FS_SetTimestamp(FileDes, NewTimestamp), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test successfully setting the time stamp */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_FS_SetTimestamp(FileDes, NewTimestamp));
}

/*
** Test FS API byte swap cFE header function
*/
void Test_CFE_FS_ByteSwapCFEHeader(void)
{
    CFE_FS_Header_t Hdr;

    UtPrintf("Begin Test Byte Swap cFE Header");

    UT_InitData();
    Hdr.ContentType    = 0x11223344;
    Hdr.SubType        = 0x22334455;
    Hdr.Length         = 0x33445566;
    Hdr.SpacecraftID   = 0x44556677;
    Hdr.ProcessorID    = 0x55667788;
    Hdr.ApplicationID  = 0x66778899;
    Hdr.TimeSeconds    = 0x778899aa;
    Hdr.TimeSubSeconds = 0x8899aabb;

    /* Test byte-swapping the header values */
    CFE_FS_ByteSwapCFEHeader(&Hdr);

    UtAssert_UINT32_EQ(Hdr.ContentType, 0x44332211);
    UtAssert_UINT32_EQ(Hdr.SubType, 0x55443322);
    UtAssert_UINT32_EQ(Hdr.Length, 0x66554433);
    UtAssert_UINT32_EQ(Hdr.SpacecraftID, 0x77665544);
    UtAssert_UINT32_EQ(Hdr.ProcessorID, 0x88776655);
    UtAssert_UINT32_EQ(Hdr.ApplicationID, 0x99887766);
    UtAssert_UINT32_EQ(Hdr.TimeSeconds, 0xaa998877);
    UtAssert_UINT32_EQ(Hdr.TimeSubSeconds, 0xbbaa9988);
}

/*
** Test FS API byte swap uint32 function
*/
void Test_CFE_FS_ByteSwapUint32(void)
{
    uint32  test    = 0x11223344;
    uint32 *testptr = &test;

    UtPrintf("Begin Test Byte Swap uint32");

    /* Test byte-swapping a uint32 value */
    UT_InitData();
    CFE_FS_ByteSwapUint32(testptr);
    UtAssert_UINT32_EQ(test, 0x44332211);
}

/*
** Test CFE_FS_ParseInputFileNameEx function
*/
void Test_CFE_FS_ParseInputFileNameEx(void)
{
    /*
     * Test case for:
     * int32 CFE_FS_ParseInputFileNameEx(char *OutputBuffer, const char *InputName, size_t OutputBufSize,
     *          const char *DefaultPath, const char *DefaultExtension)
     */

    const char TEST_INPUT_FULLY_QUALIFIED[] = "/path/to/file.log";
    const char TEST_INPUT_NO_PATH[]         = "file.log";
    const char TEST_INPUT_NO_EXTENSION[]    = "/path/to/file";
    const char TEST_INPUT_BASENAME[]        = "file";
    const char TEST_XTRA_SEPARATOR_PATH[]   = "//xtra//sep///file.log";
    const char TEST_XTRA_SEPARATOR_EXT[]    = "/xtra/sep/file..log";
    const char TEST_NO_SEPARATOR[]          = "nosep";
    const char TEST_DEFAULT_INPUT[]         = "/dpath_in/dfile_in.dext_in";
    const char TEST_DEFAULT_PATH[]          = "/dflpath";
    const char TEST_DEFAULT_EXTENSION[]     = ".dflext";

    char OutBuffer[64];

    /* nominal with fully-qualified input */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_FULLY_QUALIFIED, sizeof(OutBuffer),
                                                     sizeof(TEST_INPUT_FULLY_QUALIFIED), TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, TEST_INPUT_FULLY_QUALIFIED, "Fully-qualified pass thru -> %s", OutBuffer);

    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_XTRA_SEPARATOR_EXT, sizeof(OutBuffer),
                                                     sizeof(TEST_XTRA_SEPARATOR_EXT) - 5, TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_STRINGBUF_EQ(OutBuffer, sizeof(OutBuffer), TEST_XTRA_SEPARATOR_EXT, sizeof(TEST_XTRA_SEPARATOR_EXT) - 5);

    /* Same but as a default input, rather than in the buffer */
    /* tests 3 variations of "no input" - null ptr, zero size, empty string */
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, NULL, sizeof(OutBuffer), 1, TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, TEST_DEFAULT_INPUT, "Fully-qualified pass thru -> %s", OutBuffer);
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, "", sizeof(OutBuffer), 0, TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, TEST_DEFAULT_INPUT, "Fully-qualified pass thru -> %s", OutBuffer);
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, "", sizeof(OutBuffer), 1, TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, TEST_DEFAULT_INPUT, "Fully-qualified pass thru -> %s", OutBuffer);

    /* nominal with no path input */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_NO_PATH, sizeof(OutBuffer),
                                                     sizeof(TEST_INPUT_NO_PATH), TEST_DEFAULT_INPUT, TEST_DEFAULT_PATH,
                                                     TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, "/dflpath/file.log", "No Path input -> %s", OutBuffer);

    /* nominal with no path input - should remove duplicate path separators */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_XTRA_SEPARATOR_PATH, sizeof(OutBuffer),
                                                     sizeof(TEST_XTRA_SEPARATOR_PATH), TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, "/xtra/sep/file.log", "No Path input, extra separators -> %s", OutBuffer);

    /* nominal with no extension input */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_NO_EXTENSION, sizeof(OutBuffer),
                                                     sizeof(TEST_INPUT_NO_EXTENSION), TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, "/path/to/file.dflext", "No Extension input -> %s", OutBuffer);

    /* nominal with no extension input, no separator (should be added) */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_NO_EXTENSION, sizeof(OutBuffer),
                                                     sizeof(TEST_INPUT_NO_EXTENSION), TEST_DEFAULT_INPUT,
                                                     TEST_DEFAULT_PATH, TEST_NO_SEPARATOR));
    UtAssert_StrCmp(OutBuffer, "/path/to/file.nosep", "No Extension input, no separator -> %s", OutBuffer);

    /* nominal with neither path nor extension input */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_BASENAME, sizeof(OutBuffer),
                                                     sizeof(TEST_INPUT_BASENAME), TEST_DEFAULT_INPUT, TEST_DEFAULT_PATH,
                                                     TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, "/dflpath/file.dflext", "No Path nor Extension input -> %s", OutBuffer);

    /* Bad arguments for buffer pointer/size */
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(NULL, TEST_INPUT_BASENAME, sizeof(OutBuffer),
                                                  sizeof(TEST_INPUT_BASENAME), NULL, NULL, NULL),
                      CFE_FS_BAD_ARGUMENT);
    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_BASENAME, 0, sizeof(TEST_INPUT_BASENAME), NULL, NULL, NULL),
        CFE_FS_BAD_ARGUMENT);

    /* Bad arguments for input */
    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(OutBuffer, NULL, sizeof(OutBuffer), 0, NULL, NULL, TEST_DEFAULT_EXTENSION),
        CFE_FS_INVALID_PATH);

    /* Cases where the file name itself is actually an empty string */
    UtAssert_INT32_EQ(
        CFE_FS_ParseInputFileNameEx(OutBuffer, "", sizeof(OutBuffer), 10, NULL, NULL, TEST_DEFAULT_EXTENSION),
        CFE_FS_INVALID_PATH);
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutBuffer, "/path/", sizeof(OutBuffer), 10, NULL, TEST_DEFAULT_PATH,
                                                  TEST_DEFAULT_EXTENSION),
                      CFE_FS_INVALID_PATH);

    /* if the default path/extension is null it is just ignored, not an error. */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_BASENAME, sizeof(OutBuffer),
                                                     sizeof(TEST_INPUT_BASENAME), NULL, NULL, TEST_DEFAULT_EXTENSION));
    /* If no path this still adds a leading /  */
    UtAssert_StrCmp(OutBuffer, "/file.dflext", "No Path nor default -> %s", OutBuffer);
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_BASENAME, sizeof(OutBuffer),
                                                     sizeof(TEST_INPUT_BASENAME), NULL, TEST_DEFAULT_PATH, NULL));
    UtAssert_StrCmp(OutBuffer, "/dflpath/file", "No Extension nor default -> %s", OutBuffer);

    /* test corner case for termination where result fits exactly, including NUL (should work) */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_NO_PATH, 18, sizeof(TEST_INPUT_NO_PATH),
                                                     NULL, TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, "/dflpath/file.log", "Exact Length input -> %s", OutBuffer);
    UtAssert_INT32_EQ(OutBuffer[18], 0x7F); /* Confirm character after buffer was not touched */

    /* test corner case for termination where result itself fits but cannot fit NUL char (should be error) */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_NO_PATH, 17, sizeof(TEST_INPUT_NO_PATH), NULL,
                                                  TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION),
                      CFE_FS_FNAME_TOO_LONG);
    UtAssert_INT32_EQ(OutBuffer[17], 0x7F); /* Confirm character after buffer was not touched */

    /* test corner case for termination where result can ONLY fit NUL char (result should be terminated) */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_NO_PATH, 1, sizeof(TEST_INPUT_NO_PATH), NULL,
                                                  TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION),
                      CFE_FS_FNAME_TOO_LONG);
    UtAssert_INT32_EQ(OutBuffer[0], 0);
    UtAssert_INT32_EQ(OutBuffer[1], 0x7F);

    /* test case for where input is not terminated */
    /* Test w/input truncated via size argument (not null char) */
    UtAssert_INT32_EQ(CFE_FS_ParseInputFileNameEx(OutBuffer, TEST_INPUT_FULLY_QUALIFIED, sizeof(OutBuffer),
                                                  sizeof(TEST_INPUT_FULLY_QUALIFIED) - 9, TEST_DEFAULT_INPUT,
                                                  TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION),
                      CFE_FS_INVALID_PATH);

    /* only the specified number of chars should be used from input */
    memset(OutBuffer, 0x7F, sizeof(OutBuffer));
    CFE_UtAssert_SUCCESS(CFE_FS_ParseInputFileNameEx(OutBuffer, "abcdefgh", sizeof(OutBuffer), 4, NULL,
                                                     TEST_DEFAULT_PATH, TEST_DEFAULT_EXTENSION));
    UtAssert_StrCmp(OutBuffer, "/dflpath/abcd.dflext", "Non terminated input -> %s", OutBuffer);

    /* For coverage, also invoke the simplified CFE_FS_ParseInputFileName() */
    /* no more error paths here, as all real logic is in CFE_FS_ParseInputFileNameEx() */
    CFE_UtAssert_SUCCESS(
        CFE_FS_ParseInputFileName(OutBuffer, TEST_INPUT_NO_EXTENSION, sizeof(OutBuffer), CFE_FS_FileCategory_TEXT_LOG));
    UtAssert_StrCmp(OutBuffer, "/path/to/file.log", "Simplified API -> %s", OutBuffer);
}

/*
** Test FS API that gets defaults for file system info
*/
void Test_CFE_FS_DefaultFileStrings(void)
{
    /*
     * Test case for:
     * const char *CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_t FileCategory)
     * const char *CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_t FileCategory)
     *
     * Note that some of these depend on platform-specific and/or user-configurable
     * items, so the exact string outputs can vary.  In general this just confirms
     * that the returned pointer address, not the actual string content.
     */

    UtAssert_NULL(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_UNKNOWN));
    UtAssert_ADDRESS_EQ(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_DYNAMIC_MODULE),
                        GLOBAL_CONFIGDATA.Default_ModuleExtension);
    UtAssert_NULL(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));
    UtAssert_NOT_NULL(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_TEXT_LOG));
    UtAssert_NOT_NULL(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_SCRIPT));
    UtAssert_NOT_NULL(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_TEMP));
    UtAssert_NULL(CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_MAX));

    UtAssert_NULL(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_UNKNOWN));
    UtAssert_ADDRESS_EQ(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_DYNAMIC_MODULE),
                        GLOBAL_CFE_CONFIGDATA.NonvolMountPoint);
    UtAssert_ADDRESS_EQ(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                        GLOBAL_CFE_CONFIGDATA.RamdiskMountPoint);
    UtAssert_ADDRESS_EQ(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_TEXT_LOG),
                        GLOBAL_CFE_CONFIGDATA.RamdiskMountPoint);
    UtAssert_ADDRESS_EQ(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_SCRIPT),
                        GLOBAL_CFE_CONFIGDATA.NonvolMountPoint);
    UtAssert_ADDRESS_EQ(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_TEMP), GLOBAL_CFE_CONFIGDATA.RamdiskMountPoint);
    UtAssert_NULL(CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_MAX));
}

/*
** Test FS API write extract file name from path function
*/
void Test_CFE_FS_ExtractFileNameFromPath(void)
{
    char Original[OS_MAX_PATH_LEN];
    char FileName[OS_MAX_PATH_LEN];
    char LongFileName[OS_MAX_PATH_LEN + 7];
    int  j;

    UtPrintf("Begin Test Extract File Name from Path");

    /* Test extracting the file name from a path/file name that's
     * missing the path
     */
    UT_InitData();
    strncpy(Original, "/cf/appslibrary.gz", sizeof(Original) - 1);
    Original[sizeof(Original) - 1] = '\0';
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath("name", FileName), CFE_FS_INVALID_PATH);

    /* Test extracting the file name from a path that's null */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(NULL, FileName), CFE_FS_BAD_ARGUMENT);

    /* Test extracting the file name from a path/file name that's too long */
    UT_InitData();
    strcpy(LongFileName, "ut/");

    for (j = 0; j < OS_MAX_PATH_LEN; j++)
    {
        strcat(LongFileName, "a");
    }

    strcat(LongFileName, ".gz");

    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(LongFileName, FileName), CFE_FS_FNAME_TOO_LONG);

    /* Test successfully extracting the file name from a path/file name */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_FS_ExtractFilenameFromPath(Original, FileName));

    /* Test extracting the file name from a file name that's null */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_FS_ExtractFilenameFromPath(Original, NULL), CFE_FS_BAD_ARGUMENT);
}

/*
** Tests for FS private functions (cfe_fs_priv.c)
*/
void Test_CFE_FS_Private(void)
{
    UtPrintf("Begin Test Private");

    /* Test successful FS initialization */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_FS_EarlyInit());

    /* Test FS initialization with a mutex creation failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_FS_EarlyInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test successful locking of shared data */
    UT_InitData();
    CFE_FS_LockSharedData("FunctionName");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 0);

    /* Test locking of shared data with a mutex take error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, -1);
    CFE_FS_LockSharedData("FunctionName");
    CFE_UtAssert_SYSLOG(FS_SYSLOG_MSGS[1]);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);

    /* Test successful unlocking of shared data */
    UT_InitData();
    CFE_FS_UnlockSharedData("FunctionName");
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 0);

    /* Test unlocking of shared data with a mutex give error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, -1);
    CFE_FS_UnlockSharedData("FunctionName");
    CFE_UtAssert_SYSLOG(FS_SYSLOG_MSGS[2]);
    UtAssert_STUB_COUNT(CFE_ES_WriteToSysLog, 1);

    UtPrintf("End Test Private\n");
}

void Test_CFE_FS_BackgroundFileDump(void)
{
    /*
     * Test routine for:
     * bool CFE_FS_RunBackgroundFileDump(uint32 ElapsedTime, void *Arg)
     */
    CFE_FS_FileWriteMetaData_t State;
    uint32                     MyBuffer[2];
    uint32                     i;

    memset(UT_FS_FileWriteEventCount, 0, sizeof(UT_FS_FileWriteEventCount));
    memset(&State, 0, sizeof(State));
    memset(&CFE_FS_Global.FileDump, 0, sizeof(CFE_FS_Global.FileDump));

    /* Nominal with nothing pending - should accumulate credit */
    UtAssert_BOOL_FALSE(CFE_FS_RunBackgroundFileDump(1, NULL));
    UtAssert_INT32_GTEQ(CFE_FS_Global.FileDump.Current.Credit, 1);
    UtAssert_INT32_LTEQ(CFE_FS_Global.FileDump.Current.Credit, CFE_FS_BACKGROUND_MAX_CREDIT);

    UtAssert_BOOL_FALSE(CFE_FS_RunBackgroundFileDump(100000, NULL));
    UtAssert_INT32_EQ(CFE_FS_Global.FileDump.Current.Credit, CFE_FS_BACKGROUND_MAX_CREDIT);

    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(NULL), CFE_FS_BAD_ARGUMENT);

    /* Set each of the callback args individually NULL */
    State.GetData = UT_FS_DataGetter;
    State.OnEvent = NULL;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&State), CFE_FS_BAD_ARGUMENT);

    State.GetData = NULL;
    State.OnEvent = UT_FS_OnEvent;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&State), CFE_FS_BAD_ARGUMENT);

    UtAssert_BOOL_FALSE(CFE_FS_BackgroundFileDumpIsPending(&State));
    UtAssert_STUB_COUNT(CFE_ES_BackgroundWakeup, 0); /* confirm CFE_ES_BackgroundWakeup() was not invoked */

    /* Set the data except file name and description */
    State.FileSubType = 2;
    State.GetData     = UT_FS_DataGetter;
    State.OnEvent     = UT_FS_OnEvent;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&State), CFE_FS_INVALID_PATH);
    UtAssert_BOOL_FALSE(CFE_FS_BackgroundFileDumpIsPending(&State));
    UtAssert_STUB_COUNT(CFE_ES_BackgroundWakeup, 0); /* confirm CFE_ES_BackgroundWakeup() was not invoked */

    /* Set up remainder of fields, so entry is valid */
    strncpy(State.FileName, "/ram/UT.bin", sizeof(State.FileName));
    strncpy(State.Description, "UT", sizeof(State.Description));

    CFE_UtAssert_SUCCESS(CFE_FS_BackgroundFileDumpRequest(&State));
    UtAssert_BOOL_TRUE(CFE_FS_BackgroundFileDumpIsPending(&State));
    UtAssert_STUB_COUNT(CFE_ES_BackgroundWakeup, 1); /* confirm CFE_ES_BackgroundWakeup() was invoked */

    /*
     * Set up a fixed data buffer which will be written,
     * this will write sizeof(MyBuffer) until credit is gone
     */
    MyBuffer[0] = 10;
    MyBuffer[1] = 20;
    UT_SetDataBuffer(UT_KEY(UT_FS_DataGetter), MyBuffer, sizeof(MyBuffer), false);
    UtAssert_BOOL_TRUE(CFE_FS_RunBackgroundFileDump(1, NULL));
    UtAssert_STUB_COUNT(OS_OpenCreate, 1); /* confirm OS_open() was invoked */
    UtAssert_INT32_LTEQ(CFE_FS_Global.FileDump.Current.Credit, 0);
    UtAssert_STUB_COUNT(OS_close, 0); /* confirm OS_close() was not invoked */

    UT_SetDeferredRetcode(UT_KEY(UT_FS_DataGetter), 2, true); /* return EOF */
    UtAssert_BOOL_FALSE(CFE_FS_RunBackgroundFileDump(100, NULL));
    UtAssert_STUB_COUNT(OS_OpenCreate, 1); /* confirm OS_open() was not invoked again */
    UtAssert_STUB_COUNT(OS_close, 1);      /* confirm OS_close() was invoked */
    /* No more pending requests */
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount);
    /* complete event was sent */
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_COMPLETE], 1);
    UtAssert_BOOL_FALSE(CFE_FS_BackgroundFileDumpIsPending(&State));

    UT_ResetState(UT_KEY(UT_FS_DataGetter));

    /* Error opening file */
    CFE_UtAssert_SETUP(CFE_FS_BackgroundFileDumpRequest(&State));
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);

    UtAssert_BOOL_TRUE(CFE_FS_RunBackgroundFileDump(100, NULL));
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_CREATE_ERROR],
                       1); /* create error event was sent */
    UtAssert_BOOL_FALSE(CFE_FS_BackgroundFileDumpIsPending(&State));
    /* No more pending requests */
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount);

    /* Error writing header */
    CFE_UtAssert_SETUP(CFE_FS_BackgroundFileDumpRequest(&State));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, OS_ERROR);

    UtAssert_BOOL_TRUE(CFE_FS_RunBackgroundFileDump(100, NULL));
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR],
                       1); /* header error event was sent */
    UtAssert_BOOL_FALSE(CFE_FS_BackgroundFileDumpIsPending(&State));
    /* No more pending requests */
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount);

    /* Error writing data */
    CFE_UtAssert_SETUP(CFE_FS_BackgroundFileDumpRequest(&State));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(UT_FS_DataGetter), MyBuffer, sizeof(MyBuffer), false);
    UtAssert_BOOL_TRUE(CFE_FS_RunBackgroundFileDump(100, NULL));
    /* record error event was sent */
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR], 1);
    UtAssert_BOOL_FALSE(CFE_FS_BackgroundFileDumpIsPending(&State));
    /* No more pending requests */
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount);

    UT_ResetState(UT_KEY(UT_FS_DataGetter));

    /* Request multiple file dumps, check queuing logic */
    CFE_UtAssert_SETUP(CFE_FS_BackgroundFileDumpRequest(&State));
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&State), CFE_STATUS_REQUEST_ALREADY_PENDING);

    for (i = 0; i < (CFE_FS_MAX_BACKGROUND_FILE_WRITES - 2); ++i)
    {
        State.IsPending = false; /* UT hack to fill queue - Force not pending.  Real code should not do this. */
        CFE_UtAssert_SETUP(CFE_FS_BackgroundFileDumpRequest(&State));
    }

    State.IsPending = false;
    UtAssert_INT32_EQ(CFE_FS_BackgroundFileDumpRequest(&State), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.RequestCount,
                       (CFE_FS_Global.FileDump.CompleteCount + CFE_FS_MAX_BACKGROUND_FILE_WRITES - 1));

    /* Confirm null arg handling in CFE_FS_BackgroundFileDumpIsPending() */
    UtAssert_BOOL_FALSE(CFE_FS_BackgroundFileDumpIsPending(NULL));

    /* this catches the branch where Meta->IsPending is false */
    UtAssert_BOOL_TRUE(CFE_FS_RunBackgroundFileDump(100, NULL));
    UtAssert_BOOL_TRUE(CFE_FS_RunBackgroundFileDump(100, NULL));
    UtAssert_BOOL_TRUE(CFE_FS_RunBackgroundFileDump(100, NULL));
    UtAssert_BOOL_FALSE(CFE_FS_RunBackgroundFileDump(100, NULL));

    CFE_UtAssert_SETUP(CFE_FS_BackgroundFileDumpRequest(&State));
    UT_SetDeferredRetcode(UT_KEY(UT_FS_DataGetter), 2, true); /* avoid infinite loop */
    UtAssert_BOOL_FALSE(CFE_FS_RunBackgroundFileDump(100, NULL));
}
