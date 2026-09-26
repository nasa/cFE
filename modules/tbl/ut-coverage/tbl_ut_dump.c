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

#include "cfe_fs_core_internal.h"
#include "tbl_UT.h"
#include "tbl_ut_helpers.h"

static void UT_TBL_DumpQueue(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_FS_FileWriteMetaData_t *Meta = UT_Hook_GetArgValueByName(Context, "Meta", CFE_FS_FileWriteMetaData_t *);
    int32                       Status;
    UT_Stub_GetInt32StatusCode(Context, &Status);
    if (Status == CFE_SUCCESS)
    {
        Meta->IsPending = true;
    }
    UT_Stub_SetReturnValue(FuncKey, Status);
}

static void UT_TBL_DumpPending(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    const CFE_FS_FileWriteMetaData_t *Meta =
        UT_Hook_GetArgValueByName(Context, "Meta", const CFE_FS_FileWriteMetaData_t *);
    bool Pending = Meta->IsPending;
    UT_Stub_SetReturnValue(FuncKey, Pending);
}

static void UT_TBL_InitDumpTest(void)
{
    UT_InitData_TBL();
    UT_SetHandlerFunction(UT_KEY(CFE_FS_BackgroundFileDumpRequest), UT_TBL_DumpQueue, NULL);
    UT_SetHandlerFunction(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), UT_TBL_DumpPending, NULL);
}

static CFE_TBL_DumpControl_t *UT_TBL_MakeSnapshot(uint32 Index, void *Data, size_t Size)
{
    CFE_TBL_LoadBuff_t    *Buffer = UT_TBL_SetupLoadBuff(NULL, false, Index);
    CFE_TBL_DumpControl_t *Control;
    Buffer->IsValid             = true;
    Buffer->BufferPtr           = Data;
    Buffer->AllocationSize      = Size;
    Buffer->ContentSize         = Size;
    Buffer->FileTime.Seconds    = 123;
    Buffer->FileTime.Subseconds = 456;
    UT_TBL_SetName(Buffer->DataSource, sizeof(Buffer->DataSource), "snapshot.tbl");
    UT_TBL_SetupPendingDump(Index, Buffer, NULL, &Control);
    Control->State        = CFE_TBL_DUMP_PERFORMED;
    Control->SourceBuffId = CFE_TBL_LOADBUFFID_UNDEFINED;
    return Control;
}

void Test_CFE_TBL_DumpToFile(void)
{
    uint8                  Data[3] = { 1, 2, 3 };
    CFE_TBL_DumpControl_t *Control;
    UT_TBL_InitDumpTest();
    Control = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(Control), CFE_SUCCESS);
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 1);
    UtAssert_STUB_COUNT(OS_OpenCreate, 0);
    UtAssert_STUB_COUNT(OS_write, 0);
    UtAssert_INT32_EQ(Control->State, CFE_TBL_DUMP_WRITING);
    UtAssert_BOOL_TRUE(Control->FileWrite.IsPending);
    UtAssert_STRINGBUF_EQ(Control->FileWrite.FileName, -1, "snapshot.tbl", -1);
    UtAssert_BOOL_TRUE(Control->FileWrite.WriteHeader != NULL);
    UtAssert_BOOL_TRUE(Control->FileWrite.GetData != NULL);
    UtAssert_BOOL_TRUE(Control->FileWrite.OnEvent != NULL);
    Control->NextOffset = 2;
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(Control), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 1);
    UtAssert_UINT32_EQ(Control->NextOffset, 2);
    UT_TBL_InitDumpTest();
    Control = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    memset(Control->DumpBufferPtr->DataSource, 'x', sizeof(Control->DumpBufferPtr->DataSource));
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(Control), CFE_FS_FNAME_TOO_LONG);
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 0);
    CFE_UtAssert_EVENTSENT(CFE_TBL_CREATING_DUMP_FILE_ERR_EID);
}

void Test_CFE_TBL_TableDumpExecuteBackground(void)
{
    uint8                  Data[3] = { 1, 2, 3 };
    CFE_TBL_DumpControl_t *Control;
    CFE_TBL_LoadBuffId_t   BufferId;
    UT_TBL_InitDumpTest();
    Control  = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    BufferId = Control->DumpBufferPtr->LoadBufferId;
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_STUB_COUNT(OS_write, 0);
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 1);
    UtAssert_BOOL_TRUE(CFE_TBL_DumpCtrlBlockIsUsed(Control));
    UtAssert_BOOL_TRUE(CFE_TBL_LoadBuffIsMatch(Control->DumpBufferPtr, BufferId));
    if (Control->FileWrite.OnEvent == NULL)
    {
        UtAssert_Failed("Snapshot must install the background callbacks");
        return;
    }
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 1);
    Control->FileWrite.OnEvent(Control, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 1, 0, 100);
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_BOOL_TRUE(CFE_TBL_DumpCtrlBlockIsUsed(Control));
    UtAssert_STRINGBUF_EQ(CFE_TBL_Global.HkPacket.Payload.LastFileDumped, -1, "", -1);
    /* FS releases its pointer only after the completion callback returns. */
    Control->FileWrite.IsPending = false;
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_BOOL_FALSE(CFE_TBL_DumpCtrlBlockIsUsed(Control));
    UtAssert_BOOL_FALSE(CFE_TBL_LoadBuffIsMatch(Control->DumpBufferPtr, BufferId));
    UtAssert_STRINGBUF_EQ(CFE_TBL_Global.HkPacket.Payload.LastFileDumped, -1, "snapshot.tbl", -1);
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 1);

    /* Full FS queues retain the snapshot for a later housekeeping cycle. */
    UT_TBL_InitDumpTest();
    Control  = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    BufferId = Control->DumpBufferPtr->LoadBufferId;
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_STATUS_REQUEST_ALREADY_PENDING);
    CFE_TBL_TableDumpExecuteBackground();
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_INT32_EQ(Control->State, CFE_TBL_DUMP_PERFORMED);
    UtAssert_BOOL_TRUE(CFE_TBL_LoadBuffIsMatch(Control->DumpBufferPtr, BufferId));
    CFE_UtAssert_EVENTCOUNT(0);
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_SUCCESS);
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_INT32_EQ(Control->State, CFE_TBL_DUMP_WRITING);
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 3);

    /* Non-retryable request rejection and failed encoding release their buffers.
     */
    UT_TBL_InitDumpTest();
    Control = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_FS_INVALID_PATH);
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_BOOL_FALSE(CFE_TBL_DumpCtrlBlockIsUsed(Control));
    CFE_UtAssert_EVENTSENT(CFE_TBL_CREATING_DUMP_FILE_ERR_EID);
    UT_TBL_InitDumpTest();
    Control               = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    Control->EncodeStatus = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_BOOL_FALSE(CFE_TBL_DumpCtrlBlockIsUsed(Control));
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 0);
    CFE_UtAssert_EVENTSENT(CFE_TBL_DUMP_ENCODE_FAIL_EID);
}

void Test_CFE_TBL_SnapshotHeaders(void)
{
    uint8                  Data[3] = { 1, 2, 3 };
    CFE_TBL_DumpControl_t *Control;
    CFE_FS_Header_t        Header;
    osal_id_t              Fd = OS_OBJECT_ID_UNDEFINED;
    size_t                 Case;
    UT_TBL_InitDumpTest();
    Control = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    CFE_TBL_WriteSnapshotToFile(Control);
    if (Control->FileWrite.WriteHeader == NULL)
    {
        UtAssert_Failed("Snapshot must install the header writer");
        return;
    }
    UT_SetDataBuffer(UT_KEY(CFE_FS_WriteHeaderFromBuffer), &Header, sizeof(Header), false);
    UT_SetDefaultReturnValue(UT_KEY(OS_lseek), 128);
    UtAssert_INT32_EQ(Control->FileWrite.WriteHeader(Control, Fd), 128);
    UtAssert_UINT32_EQ(Header.TimeSeconds, 123);
    UtAssert_UINT32_EQ(Header.TimeSubSeconds, 456);
    UtAssert_STUB_COUNT(CFE_FS_WriteHeaderFromBuffer, 1);
    UtAssert_STUB_COUNT(OS_write, 1);
    UtAssert_STUB_COUNT(OS_close, 0);

    for (Case = 0; Case < 5; ++Case)
    {
        UT_TBL_InitDumpTest();
        if (Case < 2)
        {
            UT_SetDefaultReturnValue(UT_KEY(CFE_FS_WriteHeaderFromBuffer), Case == 0 ? OS_ERROR : 1);
        }
        else if (Case == 2)
        {
            UT_SetDefaultReturnValue(UT_KEY(OS_write), 1);
        }
        else
        {
            UT_SetDefaultReturnValue(UT_KEY(OS_lseek), Case == 3 ? OS_ERROR : 0);
        }
        UtAssert_INT32_EQ(Control->FileWrite.WriteHeader(Control, Fd), CFE_TBL_ERR_ACCESS);
        CFE_UtAssert_EVENTSENT(Case < 2 ? CFE_TBL_WRITE_CFE_HDR_ERR_EID : CFE_TBL_WRITE_TBL_HDR_ERR_EID);
        UtAssert_STUB_COUNT(OS_close, 0);
    }
}

void Test_CFE_TBL_SnapshotBlocks(void)
{
    uint8        Data[2 * CFE_TBL_DUMP_BLOCK_SIZE + 7];
    const size_t Sizes[] = {
        0, 1, CFE_TBL_DUMP_BLOCK_SIZE - 1, CFE_TBL_DUMP_BLOCK_SIZE, CFE_TBL_DUMP_BLOCK_SIZE + 1, sizeof(Data)
    };
    CFE_TBL_DumpControl_t *Control;
    size_t                 Case, Offset, Length;
    void                  *Buffer;
    bool                   EOFSeen;
    uint32                 Record;
    for (Case = 0; Case < sizeof(Sizes) / sizeof(Sizes[0]); ++Case)
    {
        UT_TBL_InitDumpTest();
        Control = UT_TBL_MakeSnapshot(0, Data, Sizes[Case]);
        CFE_TBL_WriteSnapshotToFile(Control);
        if (Control->FileWrite.GetData == NULL)
        {
            UtAssert_Failed("Snapshot must install the data getter");
            return;
        }
        Offset = 0;
        Record = 0;
        do
        {
            EOFSeen = Control->FileWrite.GetData(Control, Record++, &Buffer, &Length);
            UtAssert_True(Length <= CFE_TBL_DUMP_BLOCK_SIZE, "Records are bounded");
            UtAssert_True(Offset + Length <= Sizes[Case], "Records stay inside snapshot");
            if (Length != 0)
            {
                UtAssert_ADDRESS_EQ(Buffer, Data + Offset);
            }
            else
            {
                UtAssert_NULL(Buffer);
            }
            Offset += Length;
            UtAssert_True(EOFSeen == (Offset == Sizes[Case]), "EOF coincides with the final byte");
        } while (!EOFSeen && Record < 5);
        UtAssert_True(Offset == Sizes[Case], "Every snapshot byte supplied exactly once");
        UtAssert_BOOL_TRUE(Control->FileWrite.GetData(Control, Record, &Buffer, &Length));
        UtAssert_NULL(Buffer);
        UtAssert_ZERO(Length);
    }
}

void Test_CFE_TBL_SnapshotEvents(void)
{
    const CFE_FS_FileWriteEvent_t Events[] = { CFE_FS_FileWriteEvent_CREATE_ERROR,
                                               CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR,
                                               CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR,
                                               CFE_FS_FileWriteEvent_COMPLETE,
                                               CFE_FS_FileWriteEvent_COMPLETE };
    const uint16                  IDs[]    = { CFE_TBL_CREATING_DUMP_FILE_ERR_EID,
                                               CFE_TBL_WRITE_TBL_IMG_ERR_EID,
                                               0,
                                               CFE_TBL_WRITE_DUMP_INF_EID,
                                               CFE_TBL_OVERWRITE_DUMP_INF_EID };
    uint8                         Data     = 0;
    size_t                        Case;
    CFE_TBL_DumpControl_t        *Control;
    for (Case = 0; Case < sizeof(Events) / sizeof(Events[0]); ++Case)
    {
        UT_TBL_InitDumpTest();
        Control = UT_TBL_MakeSnapshot(0, &Data, sizeof(Data));
        UT_SetDefaultReturnValue(UT_KEY(OS_stat), Case == 4 ? OS_SUCCESS : OS_ERROR);
        CFE_TBL_TableDumpExecuteBackground();
        if (Control->FileWrite.OnEvent == NULL)
        {
            UtAssert_Failed("Snapshot must install the event handler");
            return;
        }
        Control->FileWrite.OnEvent(Control, CFE_FS_FileWriteEvent_UNDEFINED, 0, 0, 0, 0);
        CFE_UtAssert_EVENTCOUNT(0);
        Control->FileWrite.OnEvent(Control, Events[Case], OS_ERROR, 1, 4, 128);
        UtAssert_BOOL_TRUE(CFE_TBL_DumpCtrlBlockIsUsed(Control));
        if (IDs[Case] != 0)
        {
            CFE_UtAssert_EVENTSENT(IDs[Case]);
        }
        else
        {
            CFE_UtAssert_EVENTCOUNT(0);
        }
        Control->FileWrite.IsPending = false;
        CFE_TBL_TableDumpExecuteBackground();
        UtAssert_BOOL_FALSE(CFE_TBL_DumpCtrlBlockIsUsed(Control));
        if (Case < 3)
        {
            UtAssert_INT32_EQ(Control->WriteStatus, CFE_TBL_ERR_ACCESS);
        }
        else
        {
            UtAssert_INT32_EQ(Control->WriteStatus, CFE_SUCCESS);
        }
    }
}

void Test_CFE_TBL_SnapshotMultiple(void)
{
    uint8                  Data[2] = { 1, 2 };
    CFE_TBL_DumpControl_t *First;
    CFE_TBL_DumpControl_t *Second;
    UT_TBL_InitDumpTest();
    First  = UT_TBL_MakeSnapshot(0, Data, sizeof(Data));
    Second = UT_TBL_MakeSnapshot(1, Data, sizeof(Data));
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 2);
    if (First->FileWrite.OnEvent == NULL || Second->FileWrite.OnEvent == NULL)
    {
        UtAssert_Failed("Both requests need independent callbacks and metadata");
        return;
    }
    First->FileWrite.OnEvent(First, CFE_FS_FileWriteEvent_COMPLETE, 0, 1, 0, 128);
    First->FileWrite.IsPending = false;
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_BOOL_FALSE(CFE_TBL_DumpCtrlBlockIsUsed(First));
    UtAssert_BOOL_TRUE(CFE_TBL_DumpCtrlBlockIsUsed(Second));
    UtAssert_BOOL_TRUE(Second->FileWrite.IsPending);
    UtAssert_STUB_COUNT(CFE_FS_BackgroundFileDumpRequest, 2);
    Second->FileWrite.OnEvent(Second, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR, OS_ERROR, 0, 2, 128);
    Second->FileWrite.IsPending = false;
    CFE_TBL_TableDumpExecuteBackground();
    UtAssert_BOOL_FALSE(CFE_TBL_DumpCtrlBlockIsUsed(Second));
}
