/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** File:
**    fss_UT.c
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

const char *FS_SYSLOG_MSGS[] =
{
        NULL,
        "FS SharedData Mutex Take Err Stat=0x%x,App=%lu,Function=%s\n",
        "FS SharedData Mutex Give Err Stat=0x%x,App=%lu,Function=%s\n"
};

/* counts the number of times UT_FS_OnEvent() was invoked (below) */
uint32 UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_MAX];


/* UT helper stub compatible with background file write DataGetter */
bool UT_FS_DataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    UT_GetDataBuffer(UT_KEY(UT_FS_DataGetter), Buffer, BufSize, NULL);
    return UT_DEFAULT_IMPL(UT_FS_DataGetter);
}

/* UT helper stub compatible with background file write OnEvent */
void UT_FS_OnEvent(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum, size_t BlockSize, size_t Position)
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
    UT_ADD_TEST(Test_CFE_FS_ByteSwapCFEHeader);
    UT_ADD_TEST(Test_CFE_FS_ByteSwapUint32);
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
    UT_Report(__FILE__, __LINE__,
    		  Hdr.SubType == 123,
              "CFE_FS_InitHeader",
              "Initialize header - successful");
}

/*
** Test FS API read header function
*/
void Test_CFE_FS_ReadHeader(void)
{
    osal_id_t FileDes = OS_OBJECT_ID_UNDEFINED;
    CFE_FS_Header_t Hdr;

    UtPrintf("Begin Test Read Header");

    /* Test reading the header with a lseek failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ReadHeader(&Hdr, FileDes) == OS_ERROR,
              "CFE_FS_ReadHeader",
              "Header read lseek failed");

    /* Test successfully reading the header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_lseek), 1, OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_read), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ReadHeader(&Hdr, FileDes) != sizeof(CFE_FS_Header_t),
              "CFE_FS_ReadHeader",
              "Header read - successful");
}

/*
** Test FS API write header function
*/
void Test_CFE_FS_WriteHeader(void)
{
    osal_id_t FileDes = OS_OBJECT_ID_UNDEFINED;
    CFE_FS_Header_t Hdr;

    UtPrintf("Begin Test Write Header");

    /* Test writing the header with a lseek failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_WriteHeader(FileDes, &Hdr) == OS_ERROR,
              "CFE_FS_WriteHeader",
              "Header write lseek failed");

    /* Test successfully writing the header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_lseek), 1, OS_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, OS_SUCCESS);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_WriteHeader(FileDes, &Hdr) == OS_SUCCESS,
              "CFE_FS_WriteHeader",
              "Header write - successful");
}

/*
** Test FS API set time stamp function
*/
void Test_CFE_FS_SetTimestamp(void)
{
    osal_id_t FileDes = OS_OBJECT_ID_UNDEFINED;
    CFE_TIME_SysTime_t NewTimestamp = {0, 0};

    UtPrintf("Begin Test Set Time Stamp");

    /* Test setting the time stamp with a lseek failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_ERROR,
              "CFE_FS_SetTimestamp",
              "Failed to lseek time fields");

    /* Test setting the time stamp with a seconds write failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) != OS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Failed to write seconds");

    /* Test setting the time stamp with a subeconds write failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, 0);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Failed to write subseconds");

    /* Test successfully setting the time stamp */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_SetTimestamp(FileDes, NewTimestamp) == OS_SUCCESS,
              "CFE_FS_SetTimestamp",
              "Write time stamp - successful");
}

/*
** Test FS API byte swap cFE header function
*/
void Test_CFE_FS_ByteSwapCFEHeader(void)
{
    CFE_FS_Header_t Hdr;

    UtPrintf("Begin Test Byte Swap cFE Header");

    UT_InitData();
    Hdr.ContentType = 0x11223344;
    Hdr.SubType = 0x22334455;
    Hdr.Length = 0x33445566;
    Hdr.SpacecraftID = 0x44556677;
    Hdr.ProcessorID = 0x55667788;
    Hdr.ApplicationID = 0x66778899;
    Hdr.TimeSeconds = 0x778899aa;
    Hdr.TimeSubSeconds = 0x8899aabb;

    /* Test byte-swapping the header values */
    CFE_FS_ByteSwapCFEHeader(&Hdr);
    UT_Report(__FILE__, __LINE__,
              Hdr.ContentType == 0x44332211 && Hdr.SubType == 0x55443322 &&
              Hdr.Length == 0x66554433 && Hdr.SpacecraftID == 0x77665544 &&
              Hdr.ProcessorID == 0x88776655 &&
              Hdr.ApplicationID == 0x99887766 &&
              Hdr.TimeSeconds == 0xaa998877 &&
              Hdr.TimeSubSeconds == 0xbbaa9988,
              "CFE_FS_ByteSwapUint32",
              "Byte swap header - successful");
}

/*
** Test FS API byte swap uint32 function
*/
void Test_CFE_FS_ByteSwapUint32(void)
{
    uint32 test = 0x11223344;
    uint32 *testptr = &test;

    UtPrintf("Begin Test Byte Swap uint32");

    /* Test byte-swapping a uint32 value */
    UT_InitData();
    CFE_FS_ByteSwapUint32(testptr);
    UT_Report(__FILE__, __LINE__,
              test == 0x44332211, "CFE_FS_ByteSwapUint32",
              "Byte swap - successful");
}

/*
** Test FS API write extract file name from path function
*/
void Test_CFE_FS_ExtractFileNameFromPath(void)
{
    char Original[OS_MAX_PATH_LEN];
    char FileName[OS_MAX_PATH_LEN];
    char LongFileName[OS_MAX_PATH_LEN + 7];
    int j;

    UtPrintf("Begin Test Extract File Name from Path");

    /* Test extracting the file name from a path/file name that's
     * missing the path
     */
    UT_InitData();
    strncpy(Original, "/cf/appslibrary.gz", sizeof(Original) - 1);
    Original[sizeof(Original) - 1] = '\0';
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath("name", FileName) ==
                  CFE_FS_INVALID_PATH,
              "CFE_FS_ExtractFilenameFromPath",
              "Missing file path");

    /* Test extracting the file name from a path that's null */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(NULL, FileName) ==
                  CFE_FS_BAD_ARGUMENT,
              "CFE_FS_ExtractFilenameFromPath",
              "Null name");

    /* Test extracting the file name from a path/file name that's too long */
    UT_InitData();
    strcpy(LongFileName, "ut/");

    for (j = 0; j < OS_MAX_PATH_LEN; j++)
    {
        strcat(LongFileName, "a");
    }

    strcat(LongFileName, ".gz");

    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(LongFileName, FileName) ==
                CFE_FS_FNAME_TOO_LONG,
              "CFE_FS_ExtractFilenameFromPath",
              "Path/file name too long");

    /* Test successfully extracting the file name from a path/file name */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(Original, FileName) ==
                CFE_SUCCESS,
              "CFE_FS_ExtractFilenameFromPath",
              "Extract path name - successful");

    /* Test extracting the file name from a file name that's null */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_ExtractFilenameFromPath(Original, NULL) ==
                  CFE_FS_BAD_ARGUMENT,
              "CFE_FS_ExtractFilenameFromPath",
              "Null file name");
}

/*
** Tests for FS private functions (cfe_fs_priv.c)
*/
void Test_CFE_FS_Private(void)
{
    UtPrintf("Begin Test Private");

    /* Test successful FS initialization */
    UT_InitData();
    UT_Report(__FILE__, __LINE__,
              CFE_FS_EarlyInit() == CFE_SUCCESS,
              "CFE_FS_EarlyInit",
              "FS initialization - successful");

    /* Test FS initialization with a mutex creation failure */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemCreate), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_FS_EarlyInit() == -1,
              "CFE_FS_EarlyInit",
              "Mutex creation failure");

    /* Test successful locking of shared data */
    UT_InitData();
    CFE_FS_LockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0,
              "CFE_FS_LockSharedData",
              "Lock shared data - successful");

    /* Test locking of shared data with a mutex take error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, -1);
    CFE_FS_LockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              UT_SyslogIsInHistory(FS_SYSLOG_MSGS[1]) &&
              UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1,
              "CFE_FS_LockSharedData",
              "Shared data mutex take error");

    /* Test successful unlocking of shared data */
    UT_InitData();
    CFE_FS_UnlockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 0,
              "CFE_FS_UnlockSharedData",
              "Unlock shared data - successful");

    /* Test unlocking of shared data with a mutex give error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, -1);
    CFE_FS_UnlockSharedData("FunctionName");
    UT_Report(__FILE__, __LINE__,
              UT_SyslogIsInHistory(FS_SYSLOG_MSGS[2]) &&
              UT_GetStubCount(UT_KEY(CFE_ES_WriteToSysLog)) == 1,
              "CFE_FS_UnlockSharedData",
              "SharedData mutex give error");

    UtPrintf("End Test Private\n");
}

void Test_CFE_FS_BackgroundFileDump(void)
{
    /*
     * Test routine for:
     * bool CFE_FS_RunBackgroundFileDump(uint32 ElapsedTime, void *Arg)
     */
    CFE_FS_FileWriteMetaData_t State;
    uint32 MyBuffer[2];
    int32 Status;

    memset(UT_FS_FileWriteEventCount, 0, sizeof(UT_FS_FileWriteEventCount));
    memset(&State, 0, sizeof(State));
    memset(&CFE_FS_Global.FileDump, 0, sizeof(CFE_FS_Global.FileDump));

    /* Nominal with nothing pending - should accumulate credit */
    UtAssert_True(!CFE_FS_RunBackgroundFileDump(1, NULL), "CFE_FS_RunBackgroundFileDump() nothing pending, short delay");
    UtAssert_True(CFE_FS_Global.FileDump.Current.Credit > 0, "Credit accumulating (%lu)", (unsigned long)CFE_FS_Global.FileDump.Current.Credit);
    UtAssert_True(CFE_FS_Global.FileDump.Current.Credit < CFE_FS_BACKGROUND_MAX_CREDIT, "Credit not max (%lu)", (unsigned long)CFE_FS_Global.FileDump.Current.Credit);

    UtAssert_True(!CFE_FS_RunBackgroundFileDump(100000, NULL), "CFE_FS_RunBackgroundFileDump() nothing pending, long delay");
    UtAssert_True(CFE_FS_Global.FileDump.Current.Credit == CFE_FS_BACKGROUND_MAX_CREDIT, "Credit at max (%lu)", (unsigned long)CFE_FS_Global.FileDump.Current.Credit);

    Status = CFE_FS_BackgroundFileDumpRequest(NULL);
    UtAssert_True(Status == CFE_FS_BAD_ARGUMENT, "CFE_FS_BackgroundFileDumpRequest(NULL) (%lu) == CFE_FS_BAD_ARGUMENT", (unsigned long)Status);
    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_FS_BAD_ARGUMENT, "CFE_FS_BackgroundFileDumpRequest(&State) (%lu) == CFE_FS_BAD_ARGUMENT", (unsigned long)Status);
    UtAssert_True(!CFE_FS_BackgroundFileDumpIsPending(&State), "!CFE_FS_BackgroundFileDumpIsPending(&State)");
    UtAssert_STUB_COUNT(CFE_ES_BackgroundWakeup, 0); /* confirm CFE_ES_BackgroundWakeup() was not invoked */

    /* Set the data except file name and description */
    State.FileSubType = 2;
    State.GetData = UT_FS_DataGetter;
    State.OnEvent = UT_FS_OnEvent;
    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_FS_INVALID_PATH, "CFE_FS_BackgroundFileDumpRequest(&State) (%lu) == CFE_FS_INVALID_PATH", (unsigned long)Status);
    UtAssert_True(!CFE_FS_BackgroundFileDumpIsPending(&State), "!CFE_FS_BackgroundFileDumpIsPending(&State)");
    UtAssert_STUB_COUNT(CFE_ES_BackgroundWakeup, 0); /* confirm CFE_ES_BackgroundWakeup() was not invoked */

    /* Set up remainder of fields, so entry is valid */
    strncpy(State.FileName, "/ram/UT.bin", sizeof(State.FileName));
    strncpy(State.Description, "UT", sizeof(State.Description));

    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_SUCCESS, "CFE_FS_BackgroundFileDumpRequest() (%lu) == CFE_SUCCESS", (unsigned long)Status);
    UtAssert_True(CFE_FS_BackgroundFileDumpIsPending(&State), "CFE_FS_BackgroundFileDumpIsPending(&State)");
    UtAssert_STUB_COUNT(CFE_ES_BackgroundWakeup, 1); /* confirm CFE_ES_BackgroundWakeup() was invoked */

    /* 
     * Set up a fixed data buffer which will be written, 
     * this will write sizeof(MyBuffer) until credit is gone
     */
    MyBuffer[0] = 10;
    MyBuffer[1] = 20;
    UT_SetDataBuffer(UT_KEY(UT_FS_DataGetter),MyBuffer,sizeof(MyBuffer), false);
    UtAssert_True(CFE_FS_RunBackgroundFileDump(1, NULL), "CFE_FS_RunBackgroundFileDump() request pending nominal");
    UtAssert_STUB_COUNT(OS_OpenCreate, 1); /* confirm OS_open() was invoked */
    UtAssert_True(CFE_FS_Global.FileDump.Current.Credit <= 0, "Credit exhausted (%lu)", (unsigned long)CFE_FS_Global.FileDump.Current.Credit);
    UtAssert_STUB_COUNT(OS_close, 0); /* confirm OS_close() was not invoked */

    UT_SetDeferredRetcode(UT_KEY(UT_FS_DataGetter), 2, true); /* return EOF */
    UtAssert_True(!CFE_FS_RunBackgroundFileDump(100, NULL), "CFE_FS_RunBackgroundFileDump() request pending EOF");
    UtAssert_STUB_COUNT(OS_OpenCreate, 1); /* confirm OS_open() was not invoked again */
    UtAssert_STUB_COUNT(OS_close, 1); /* confirm OS_close() was invoked */
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount); /* request was completed */
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_COMPLETE], 1); /* complete event was sent */
    UtAssert_True(!CFE_FS_BackgroundFileDumpIsPending(&State), "!CFE_FS_BackgroundFileDumpIsPending(&State)");

    UT_ResetState(UT_KEY(UT_FS_DataGetter));


    /* Error opening file */
    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_SUCCESS, "CFE_FS_BackgroundFileDumpRequest() (%lu) == CFE_SUCCESS", (unsigned long)Status);
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);

    UtAssert_True(CFE_FS_RunBackgroundFileDump(100, NULL), "CFE_FS_RunBackgroundFileDump() request pending, file open error");
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_CREATE_ERROR], 1); /* create error event was sent */
    UtAssert_True(!CFE_FS_BackgroundFileDumpIsPending(&State), "!CFE_FS_BackgroundFileDumpIsPending(&State)");
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount); /* request was completed */

    /* Error writing header */
    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_SUCCESS, "CFE_FS_BackgroundFileDumpRequest() (%lu) == CFE_SUCCESS", (unsigned long)Status);
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, OS_ERROR);

    UtAssert_True(CFE_FS_RunBackgroundFileDump(100, NULL), "CFE_FS_RunBackgroundFileDump() request pending, file write header error");
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR], 1); /* header error event was sent */
    UtAssert_True(!CFE_FS_BackgroundFileDumpIsPending(&State), "!CFE_FS_BackgroundFileDumpIsPending(&State)");
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount); /* request was completed */

    /* Error writing data */
    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_SUCCESS, "CFE_FS_BackgroundFileDumpRequest() (%lu) == CFE_SUCCESS", (unsigned long)Status);
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, OS_ERROR);
    UT_SetDataBuffer(UT_KEY(UT_FS_DataGetter),MyBuffer,sizeof(MyBuffer), false);
    UtAssert_True(CFE_FS_RunBackgroundFileDump(100, NULL), "CFE_FS_RunBackgroundFileDump() request pending, file write data error");
    UtAssert_UINT32_EQ(UT_FS_FileWriteEventCount[CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR], 1); /* record error event was sent */
    UtAssert_True(!CFE_FS_BackgroundFileDumpIsPending(&State), "!CFE_FS_BackgroundFileDumpIsPending(&State)");
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.CompleteCount, CFE_FS_Global.FileDump.RequestCount); /* request was completed */

    UT_ResetState(UT_KEY(UT_FS_DataGetter));

    /* Request multiple file dumps, check queing logic */
    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_SUCCESS, "CFE_FS_BackgroundFileDumpRequest() (%lu) == CFE_SUCCESS", (unsigned long)Status);
    Status = CFE_FS_BackgroundFileDumpRequest(&State);
    UtAssert_True(Status == CFE_STATUS_REQUEST_ALREADY_PENDING, "CFE_FS_BackgroundFileDumpRequest() (%lu) == CFE_STATUS_REQUEST_ALREADY_PENDING", (unsigned long)Status);

    do
    {
        State.IsPending = false; /* UT hack to fill queue - Force not pending.  Real code should not do this. */
        Status = CFE_FS_BackgroundFileDumpRequest(&State);
    }
    while (Status == CFE_SUCCESS);

    UtAssert_True(Status == CFE_STATUS_REQUEST_ALREADY_PENDING, "CFE_FS_BackgroundFileDumpRequest() (%lu) == CFE_STATUS_REQUEST_ALREADY_PENDING", (unsigned long)Status);
    UtAssert_UINT32_EQ(CFE_FS_Global.FileDump.RequestCount, (CFE_FS_Global.FileDump.CompleteCount + CFE_FS_MAX_BACKGROUND_FILE_WRITES - 1));

    /* Confirm null arg handling in CFE_FS_BackgroundFileDumpIsPending() */
    UtAssert_True(!CFE_FS_BackgroundFileDumpIsPending(NULL), "!CFE_FS_BackgroundFileDumpIsPending(NULL)");
}
