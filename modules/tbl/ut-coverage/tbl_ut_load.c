/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 * @file
 *  Unit test routines for table load
 */

/*
** Includes
*/
#include "tbl_UT.h"
#include "tbl_ut_helpers.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_config.h"

/*
 * A string to pass in for file names (no need to actually exist in UT)
 * Just keep it short - under OS_MAX_FILE_LEN
 */
#define UT_TBL_SRC_FILENAME "utsrc.dat"

/* A buffer to hold table data */
UT_Table1_t UT_TBL_TableData;

/*
 * Function to test all error cases of the CFE_TBL_Load() API call
 * All of these test conditions have something wrong and should be rejected.
 * Importantly - nothing should change the state of the underlying table
 */
void UT_TBL_Load_Error_Suite(CFE_TBL_Handle_t SubjectHandle, bool WasModified, bool WasLoaded)
{
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    uint32                      IdValue;
    CFE_Config_ArrayValue_t     UTAV = {1, &IdValue};

    /* Initially set up a "good" standard (FS) file header data - modified later for failure checks. */
    memset(&StdFileHeader, 0, sizeof(StdFileHeader));
    UT_TBL_SetName(StdFileHeader.Description, sizeof(StdFileHeader.Description), "Test description");
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;

    /* Set local ptrs to refer to the subject table */
    AccessDescPtr = UT_TBL_AccDescFromExtHandle(SubjectHandle);
    RegRecPtr     = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);

    /* None of the tests here should change the state flags, so confirm state before doing anything */
    /* If either of these test fail then it is a setup issue */
    UtAssert_UINT8_EQ(WasModified, CFE_TBL_RegRecIsModified(RegRecPtr));
    UtAssert_UINT8_EQ(WasLoaded, CFE_TBL_RegRecIsTableLoaded(RegRecPtr));

    /* Test response to a null source data pointer */
    UT_InitData_TBL();
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_ADDRESS, NULL), CFE_TBL_BAD_ARGUMENT);

    /* Test attempt to load a table with a bad handle */
    UT_InitData_TBL();
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_TBL_BAD_TABLE_HANDLE, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData),
                      CFE_TBL_ERR_INVALID_HANDLE);

    /* Test response to a table with no access (mimic the request coming from a different App) */
    UT_InitData_TBL();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_HANDLE_ACCESS_ERR_EID);
    UT_SetAppID(UT_TBL_APPID_1);

    /* Test attempt to load a file that has incompatible data for the
     * specified table */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t), "mismatch");
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a file where the std header cannot be read */
    UT_InitData_TBL();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME), CFE_TBL_ERR_NO_STD_HEADER);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_STD_HDR_ERR_EID);

    /* Test attempt to load a file where the tbl header cannot be read */
    UT_InitData_TBL();
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME), CFE_TBL_ERR_NO_TBL_HEADER);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_TBL_HDR_ERR_EID);

    /* Test attempt to load a file where the file is the wrong type */
    UT_InitData_TBL();
    StdFileHeader.ContentType = ~CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME), CFE_TBL_ERR_BAD_CONTENT_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_TYPE_ERR_EID);

    /* Test attempt to load a file where the file is the wrong subtype */
    UT_InitData_TBL();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = ~CFE_FS_SubType_TBL_IMG;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME), CFE_TBL_ERR_BAD_SUBTYPE_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_SUBTYPE_ERR_EID);

    /* Test attempt to load a file where the processor id is wrong */
    UT_InitData_TBL();
    IdValue = 0x123;
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    StdFileHeader.ContentType  = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType      = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.ProcessorID  = ~IdValue;
    StdFileHeader.SpacecraftID = IdValue;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME), CFE_TBL_ERR_BAD_PROCESSOR_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PROCESSOR_ID_ERR_EID);

    /* Test attempt to load a file where the processor id is wrong */
    UT_InitData_TBL();
    IdValue = 0x123;
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    StdFileHeader.ContentType  = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType      = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.ProcessorID  = IdValue;
    StdFileHeader.SpacecraftID = ~IdValue;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME),
                      CFE_TBL_ERR_BAD_SPACECRAFT_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SPACECRAFT_ID_ERR_EID);

    /* Test attempt to load from an illegal source type (not a file or
     * from memory)
     */
    UT_InitData_TBL();
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, (CFE_TBL_SrcEnum_t)99, &UT_TBL_TableData),
                      CFE_TBL_ERR_ILLEGAL_SRC_TYPE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Now confirm that the flags did not change from the start - Keep this last */
    UtAssert_UINT8_EQ(WasModified, CFE_TBL_RegRecIsModified(RegRecPtr));
    UtAssert_UINT8_EQ(WasLoaded, CFE_TBL_RegRecIsTableLoaded(RegRecPtr));
}

/*
** Function to test loading a specified table with data from the
** specified source - this is for the CFE_TBL_Load() API call
*/
void UT_TBL_Basic_Load_Suite(CFE_TBL_Handle_t SubjectHandle)
{
    /*
     * The purpose of this function is to exercise all the various error paths
     * that might occur during loading.
     *
     * It is put into a subroutine here so it can be more easily applied to tables
     * that have different configurations without copying all the test cases over again.
     *
     * In some cases, the table config does affect what is permissable/not permissable
     */
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    memset(&UT_TBL_TableData, 0, sizeof(UT_TBL_TableData));

    /* Initially set up a "good" standard (FS) file header data - modified later for failure checks. */
    memset(&StdFileHeader, 0, sizeof(StdFileHeader));
    UT_TBL_SetName(StdFileHeader.Description, sizeof(StdFileHeader.Description), "Test description");
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;

    /* Set local ptrs to refer to the subject table */
    AccessDescPtr = UT_TBL_AccDescFromExtHandle(SubjectHandle);
    RegRecPtr     = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);

    UtPrintf("Begin Basic Test Load tests on handle=0x%lx", CFE_TBL_HandleID_AsInt(AccessDescPtr->HandleId));

    /* start with the error battery BEFORE loading */
    UT_TBL_Load_Error_Suite(SubjectHandle, false, false);

    /* Test attempt to perform partial INITIAL load (not allowed) */
    /* This is a valid load setup, it is only due to the fact that the table
     * has not had a full load yet that this request is not allowed */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 1, sizeof(UT_Table1_t) - 1, CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME), CFE_TBL_ERR_PARTIAL_LOAD);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PARTIAL_LOAD_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    UtAssert_BOOL_FALSE(CFE_TBL_RegRecIsTableLoaded(RegRecPtr));

    /* Now set the header up for a full load so it will be successful.
     * NOTE: after this, the table is now loaded once and different rules will apply */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* repeat the error battery AFTER loading */
    /* Note internally there is a lot of switching based on CFE_TBL_RegRecIsTableLoaded(),
     * But these error cases should behave the same - this is intended to verify that. */
    UT_TBL_Load_Error_Suite(SubjectHandle, false, true);

    /* Test attempt to perform partial load with table already loaded
     * This is identical to the partial setup from earlier, except
     * now its been loaded once, so the partial load is allowed */
    UT_InitData_TBL();
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    UT_TBL_SetupHeader(&TblFileHeader, 1, sizeof(UT_Table1_t) - 1, CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_FILE, UT_TBL_SRC_FILENAME));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test performing a Load from memory */
    UT_InitData_TBL();
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test failure of validation function on table load using a negative
     * return code
     */
    UT_InitData_TBL();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1234);
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), -1234);
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test failure of validation function on table load using a positive
     * return code
     */
    UT_InitData_TBL();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    UtAssert_INT32_EQ(CFE_TBL_Load(SubjectHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData),
                      CFE_STATUS_VALIDATION_FAILURE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_VAL_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(2);
}

/*
** Test the function that processes load table file to buffer command
** message
*/
void Test_CFE_TBL_LoadCmd(void)
{
    CFE_TBL_File_Hdr_t      TblFileHeader;
    CFE_FS_Header_t         StdFileHeader;
    uint8                   LoadBuffer[sizeof(UT_Table1_t)];
    CFE_TBL_LoadCmd_t       LoadCmd;
    CFE_ES_AppId_t          AppID;
    CFE_TBL_LoadBuff_t *    LoadBuffPtr;
    CFE_TBL_RegistryRec_t * RegRec0Ptr;
    uint32                  IdValue;
    CFE_Config_ArrayValue_t UTAV = {1, &IdValue};

    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Load Command");

    /* Clear all local structures */
    memset(&TblFileHeader, 0, sizeof(TblFileHeader));
    memset(&StdFileHeader, 0, sizeof(StdFileHeader));
    memset(&LoadCmd, 0, sizeof(LoadCmd));

    UT_TBL_SetupSingleReg(&RegRec0Ptr, NULL, CFE_TBL_OPT_DEFAULT);

    IdValue = 0x123;
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);

    /* Set up the headers */
    UT_TBL_SetName(StdFileHeader.Description, sizeof(StdFileHeader.Description), "FS header description");
    StdFileHeader.ContentType  = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType      = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.SpacecraftID = IdValue;
    StdFileHeader.ProcessorID  = IdValue;

    /* Test response to inability to open file */
    UT_InitData_TBL();
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test response to inability to find the table in the registry */
    UT_InitData_TBL();
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "notfound");
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_SUCH_TABLE_ERR_EID);

    /* The rest of the tests will use registry 0, note empty name matches */
    RegRec0Ptr->OwnerAppId = AppID;

    /* Test attempt to load a dump only table */
    UT_InitData_TBL();
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_TBL_Config(RegRec0Ptr)->DumpOnly = true;
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    UT_TBL_Config(RegRec0Ptr)->DumpOnly = false;

    /* Test attempt to load a table with a load already pending */
    UT_InitData_TBL();
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_TBL_SetupLoadBuff(RegRec0Ptr, false, 0);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where the file isn't dump only and passes table checks, get a
     * working buffer, and there is an extra byte (more data than header
     * indicates)
     * Works because OS_read stub assumes success (returns dummy extra byte even though it isn't in the buffer)
     */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_TBL_Config(RegRec0Ptr)->Size = sizeof(UT_Table1_t);

    LoadBuffPtr            = UT_TBL_SetupLoadBuff(RegRec0Ptr, false, 0);
    LoadBuffPtr->BufferPtr = &LoadBuffer;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with no extra byte => successful load */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test with differing amount of data from header's claim */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, 0);
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with no working buffers available */
    UT_InitData_TBL();
    UT_TBL_SetActiveBufferAddr(RegRec0Ptr, 0, LoadBuffPtr->BufferPtr);
    UT_TBL_Status(RegRec0Ptr)->NextBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Config(RegRec0Ptr)->DoubleBuffered = false;
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNextEx), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_WORK_BUFFERS_ERR_EID);

    /* Test with interal CFE_TBL_GetWorkingBuffer error (memcpy with matching address */
    UT_InitData_TBL();
    LoadBuffPtr = UT_TBL_SetupLoadBuff(RegRec0Ptr, false, 0);
    UT_TBL_SetActiveBufferAddr(RegRec0Ptr, 0, LoadBuffPtr->BufferPtr);
    UT_TBL_SetLoadBuffFree(LoadBuffPtr);
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with table header indicating data beyond size of the table */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_TBL_Config(RegRec0Ptr)->Size = sizeof(UT_Table1_t) - 1;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with table header indicating no data in the file */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 0, CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where file has partial load for uninitialized table and offset
     * is non-zero
     */
    UT_InitData_TBL();
    /* reset bufferids back to initial state (an unloaded table) */
    UT_TBL_Status(RegRec0Ptr)->ActiveBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Status(RegRec0Ptr)->PrevBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Config(RegRec0Ptr)->Size           = sizeof(UT_Table1_t);
    UT_TBL_SetupHeader(&TblFileHeader, 1, 1, CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PARTIAL_LOAD_ERR_EID);

    /* Test where file has partial load for uninitialized table and offset
     * is zero
     */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 1, CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_TBL_Config(RegRec0Ptr)->Size = sizeof(UT_Table1_t);
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test response to inability to read the file header */
    UT_InitData_TBL();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where file has partial load for initialized table and offset
     * is non-zero
     */
    UT_InitData_TBL();
    UT_TBL_InitActiveBuffer(RegRec0Ptr, 0);
    UT_TBL_SetupHeader(&TblFileHeader, 1, 0, CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where file has partial load for initialized table and offset
     * is non-zero
     */
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&TblFileHeader, 1, 1, CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
}

/*
** Function to test loading a specified table with data from the
** specified source - this is for the CFE_TBL_Load() API call
*/
void Test_CFE_TBL_Load1(void)
{
    CFE_TBL_Handle_t            TblHandle;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_Handle_t            ShareHandle;
    void *                      DataPtr;

    UT_InitData_TBL();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DEFAULT);
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;
    TblHandle                                   = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Sanity check -- Nothing in the setup should have generated any events */
    CFE_UtAssert_EVENTCOUNT(0);

    /* Perform full battery focusing on the single-buffered table w/default opts */
    UT_TBL_Basic_Load_Suite(TblHandle);

    /* Test sharing semantics with single-buffered tables.  When a single buffered
     * table is shared, it locks out loading until the shared address is released */
    UT_InitData_TBL();
    UT_SetAppID(UT_TBL_APPID_2); /* sharing has to be done from a different app */
    CFE_TBL_Share(&ShareHandle, CFE_TBL_RegRecGetName(RegRecPtr));
    CFE_TBL_GetAddress(&DataPtr, ShareHandle); /* so it will be "locked" */
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), CFE_TBL_INFO_TABLE_LOCKED);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* normally this would update the table, but it will not be able to */
    UtAssert_INT32_EQ(CFE_TBL_Manage(TblHandle), CFE_TBL_INFO_TABLE_LOCKED);

    /* now un-share the table so things get unlocked */
    UT_SetAppID(UT_TBL_APPID_2); /* sharing has to be done from a different app */
    CFE_TBL_ReleaseAddress(ShareHandle);
    CFE_TBL_Unregister(ShareHandle);
    UT_SetAppID(UT_TBL_APPID_1);

    /* Now subsequent loads should be rejected, because the previous load is not yet completed */
    UT_InitData_TBL();
    UtAssert_INT32_EQ(CFE_TBL_GetStatus(TblHandle), CFE_TBL_INFO_UPDATE_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), CFE_TBL_ERR_LOAD_IN_PROGRESS);
    /* This completes the load */
    UtAssert_INT32_EQ(CFE_TBL_Manage(TblHandle), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_SUCCESS_INF_EID);

    /* Now it should be loadable again */
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData));
}

void Test_CFE_TBL_Load2(void)
{
    CFE_TBL_Handle_t            TblHandle;
    CFE_TBL_Handle_t            ShareHandle;
    void *                      DataPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    UT_InitData_TBL();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DBL_BUFFER);
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;
    TblHandle                                   = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Sanity check -- Nothing in the setup should have generated any events */
    CFE_UtAssert_EVENTCOUNT(0);

    /* Perform full battery focusing on the single-buffered table w/default opts */
    UT_TBL_Basic_Load_Suite(TblHandle);

    /* Follow-on testing that is specific for double buffered tables -
     * The load checks if the inactive buffer is _not_ in use (via another access descriptor,
     * i.e. a shared table) before loading something into it.  This sequence may also be
     * somewhat repeated in the CFE_TBL_GetAddress/CFE_TBL_ReleaseAddress tests, but this
     * sequence focuses on the CFE_TBL_Load() paths/outputs during this process */

    /* a. Test setup part 1 - another app gets a shared handle to the double buffered table */
    UT_InitData_TBL();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&ShareHandle, CFE_TBL_RegRecGetName(RegRecPtr)));
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test setup part 2 - it gets the address of the table - this locks the buffer */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&DataPtr, ShareHandle), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test part 1 - the first load should be OK, it uses the inactive buffer as intended */
    UT_InitData_TBL();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* d. Perform test part 2 - the second load should not work, it is blocked due to the lock */
    UT_InitData_TBL();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), CFE_TBL_ERR_NO_BUFFER_AVAIL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_WORK_BUFFERS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* e. Test cleanup */
    UT_InitData_TBL();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(ShareHandle));
    CFE_UtAssert_EVENTCOUNT(0);
}

void Test_CFE_TBL_Load3(void)
{
    CFE_TBL_Handle_t            TblHandle;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    /* register a dump-only table */
    UT_InitData_TBL();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DUMP_ONLY);
    TblHandle = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Sanity check -- Nothing in the setup should have generated any events */
    CFE_UtAssert_EVENTCOUNT(0);

    /* As dump only and user defined tables are more limited, the full battery of tests
     * is not needed -- these cannot be loaded via CFE_TBL_Load at all, so
     * the objective here is to make sure it is properly rejected */
    UT_InitData_TBL();
    UtAssert_INT32_EQ(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a dump-only table with the table already loaded */
    UT_InitData_TBL();
    UT_TBL_SetActiveBufferAddr(RegRecPtr, 0, &UT_TBL_TableData);
    UtAssert_INT32_EQ(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
}

void Test_CFE_TBL_Load4(void)
{
    CFE_TBL_Handle_t            TblHandle;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    /* register a user defined table */
    UT_InitData_TBL();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_USR_DEF_ADDR);
    TblHandle = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Sanity check -- Nothing in the setup should have generated any events */
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test specifying a table address for a user defined table */
    /* Perform test - loading from file is not allowed */
    UT_InitData_TBL();
    UtAssert_INT32_EQ(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_FILE, "abc"), CFE_TBL_ERR_ILLEGAL_SRC_TYPE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Perform test - success */
    UT_InitData_TBL();
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* loading a second time is not valid */
    UT_InitData_TBL();
    UtAssert_INT32_EQ(CFE_TBL_Load(TblHandle, CFE_TBL_SRC_ADDRESS, &UT_TBL_TableData), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
}

void Test_CFE_TBL_TableLoadCodec(void)
{
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_LoadBuff_t *        SharedBufferPtr;
    CFE_TBL_LoadBuff_t *        LocalBufferPtr;
    CFE_ResourceId_t            PendingId;
    CFE_TBL_TxnState_t          Txn;

    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT);
    CFE_TBL_TxnStartFromHandle(&Txn, CFE_TBL_AccDescGetHandle(AccDescPtr), CFE_TBL_TxnContext_UNDEFINED);

    /* To get full coverage on the Encode/Decode routines we need to directly invoke with the various combos */
    /* Force the working buffer to be a shared buff */
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    LocalBufferPtr  = CFE_TBL_GetActiveBuffer(RegRecPtr);
    PendingId       = CFE_TBL_FindNextSharedBufferId();
    SharedBufferPtr = CFE_TBL_LocateLoadBufferByID(CFE_TBL_LOADBUFFID_C(PendingId));
    CFE_TBL_LoadBuffSetUsed(SharedBufferPtr, PendingId, CFE_TBL_RegRecGetID(RegRecPtr));
    CFE_TBL_LoadBuffSetContentSize(SharedBufferPtr, sizeof(UT_Table1_t));

    CFE_UtAssert_SUCCESS(CFE_TBL_DecodeInputData(&Txn, SharedBufferPtr, LocalBufferPtr));
    CFE_UtAssert_SUCCESS(CFE_TBL_EncodeOutputData(&Txn, LocalBufferPtr, SharedBufferPtr));
}

void Test_CFE_TBL_TableLoadCommon(void)
{
    char                        Filename[OS_MAX_PATH_LEN];
    CFE_TBL_CombinedFileHdr_t   FileHeader;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_TxnState_t          Txn;
    osal_id_t                   FileDescriptor;
    uint32                      IdValue;
    CFE_Config_ArrayValue_t     UTAV = {1, &IdValue};

    IdValue = 0x123;

    FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    memset(&Txn, 0, sizeof(Txn));
    memset(&FileHeader, 0, sizeof(FileHeader));
    strncpy(Filename, "ut", sizeof(Filename));

    UT_TBL_SetName(FileHeader.Std.Description, sizeof(FileHeader.Std.Description), "FS header description");
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;

    UtPrintf("Begin Test Table Load Common Impl");

    /* Bad argument - NULL filename */
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, NULL), CFE_TBL_ERR_ACCESS);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_ACCESS_ERR_EID);

    /* Test when the transaction object is not initialized */
    CFE_TBL_TxnInit(&Txn, false);
    UT_InitData_TBL();
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t), "n/a");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_INVALID_HANDLE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_NO_SUCH_TABLE_ERR_EID);

    /* Test CFE_TBL_LoadFinish with an invalid transaction - it should do nothing */
    CFE_TBL_TxnInit(&Txn, false);
    UtAssert_INT32_EQ(CFE_TBL_LoadFinish(&Txn, CFE_STATUS_INCORRECT_STATE), CFE_STATUS_INCORRECT_STATE);

    /* Now set up the transaction to point to a valid registry entry for the rest of tests */
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DBL_BUFFER);
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;
    CFE_TBL_TxnStartFromHandle(&Txn, AccDescPtr->HandleId, CFE_TBL_TxnContext_UNDEFINED);

    /* Test CFE_TBL_ReadHeaders response to a failure reading the standard cFE
     * file header
     */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1, "ut_cfe_tbl.UT_Table1");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_TBL_SetName(Filename, sizeof(Filename), "MyTestInputFilename");
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_NO_STD_HEADER);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to a bad magic number in cFE
     * standard header
     */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID - 1;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1, "ut_cfe_tbl.UT_Table1");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_CONTENT_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to a wrong cFE file subtype */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG - 1;
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1, "ut_cfe_tbl.UT_Table1");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_SUBTYPE_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_SUBTYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to a failure reading the cFE
     * table header
     */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1, "ut_cfe_tbl.UT_Table1");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, sizeof(CFE_TBL_File_Hdr_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_NO_TBL_HEADER);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_TBL_HDR_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to an invalid spacecraft ID */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    FileHeader.Std.ContentType  = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType      = CFE_FS_SubType_TBL_IMG;
    FileHeader.Std.SpacecraftID = ~IdValue;
    FileHeader.Std.ProcessorID  = IdValue;
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1, "ut_cfe_tbl.UT_Table1");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_TBL_SetName(Filename, sizeof(Filename), "MyTestInputFilename");
    Filename[sizeof(Filename) - 1] = '\0';
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_SPACECRAFT_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_SPACECRAFT_ID_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to an invalid processor ID */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    FileHeader.Std.ContentType  = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType      = CFE_FS_SubType_TBL_IMG;
    FileHeader.Std.SpacecraftID = IdValue;
    FileHeader.Std.ProcessorID  = ~IdValue;
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1, "ut_cfe_tbl.UT_Table1");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_TBL_SetName(Filename, sizeof(Filename), "MyTestInputFilename");
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_PROCESSOR_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_PROCESSOR_ID_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_TxnLoadFromFile response to a file name that is too long */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_FS_ERR_PATH_TOO_LONG);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_FILENAME_TOO_LONG);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_LOAD_FILENAME_LONG_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile response to the file being for the wrong table */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t), "mismatch");
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile response to a partial file, where the table is not loaded yet */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 1, sizeof(UT_Table1_t) - 1, CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_PARTIAL_LOAD);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_PARTIAL_LOAD_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile response to a file that's content is too large
     * This is a normal (non-parital) set up - starting at offset 0 - where the number of bytes in
     * the table header is simply greater than the size of the registered table  */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) + 4, CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_FILE_TOO_LARGE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID);

    /* Now set up an active buffer, so it appears as if the table is loaded */
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);

    /* Test CFE_TBL_TxnLoadFromFile response to a file that's content is too large
     * This is a partial load set up - starting at nonzero offset.  The number of bytes
     * is less than the size of the registered table, but the (offset + numbytes) makes it
     * too big (thus a different case than the earlier TOO_LARGE test) */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 4, sizeof(UT_Table1_t) - 2, CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_FILE_TOO_LARGE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID);

    /* Test CFE_TBL_SetMetaDataFromFileHeader where no load is in progress */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    CFE_TBL_RegRecClearLoadInProgress(RegRecPtr);
    UtAssert_VOIDCALL(CFE_TBL_SetMetaDataFromFileHeader(&Txn, Filename, &FileHeader.Std));

    /* For the next group of tests, set up initial conditions where the table
     * is not loaded yet, but there is a load in progress (which would be the initial load) */
    CFE_TBL_LoadBuffSetFree(CFE_TBL_GetActiveBuffer(RegRecPtr));
    UT_TBL_Status(RegRecPtr)->ActiveBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);

    /* Test CFE_TBL_TxnLoadFromFile response to the file content being
     * incomplete
     */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, sizeof(UT_Table1_t) - 1);
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_LOAD_INCOMPLETE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_INCOMPLETE_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile with failure of data OS_read */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, -1);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_LOAD_INCOMPLETE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_INCOMPLETE_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile with failure of extra byte OS_read */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_TxnLoadFromFile(&Txn, Filename));

    /* Test CFE_TBL_TxnLoadFromFile response to an OS open error */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t), CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_ACCESS);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_ACCESS_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile response to a file too short warning */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1, CFE_TBL_RegRecGetName(RegRecPtr));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_WARN_SHORT_FILE);

    /* Test CFE_TBL_TxnLoadFromFile response to an invalid header length */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_NO_STD_HEADER);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_STD_HDR_ERR_EID);

    /* Test CFE_TBL_LoadContentFromFile response to a file that would overrun the buffer */
    UT_InitData_TBL();
    CFE_TBL_TxnClearEvents(&Txn);
    UtAssert_INT32_EQ(CFE_TBL_LoadContentFromFile(&Txn, FileDescriptor, CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE,
                                                  CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE),
                      CFE_TBL_ERR_FILE_TOO_LARGE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_TOO_BIG_ERR_EID);

    /* Test CFE_TBL_ValidateLoadInProgress response where there is no load in progress */
    UT_InitData_TBL();
    CFE_TBL_RegRecClearLoadInProgress(RegRecPtr);
    UtAssert_INT32_EQ(CFE_TBL_ValidateLoadInProgress(&Txn, CFE_SUCCESS), CFE_TBL_ERR_NEVER_LOADED);
}
