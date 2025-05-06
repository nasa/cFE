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
**    tbl_UT.c
**
** Purpose:
**    Table Services unit test
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
#include "tbl_UT.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_config.h"

/*
** External global variables
*/

extern CFE_TBL_Global_t CFE_TBL_Global;

/*
** Global variables
*/
CFE_TBL_Handle_t App1TblHandle1;
CFE_TBL_Handle_t App1TblHandle2;
CFE_TBL_Handle_t App2TblHandle1;
CFE_TBL_Handle_t App2TblHandle2;
CFE_TBL_Handle_t ArrayOfHandles[2];

/* Checks the TBL transaction structure for an event (deferred reporting) */
#define UT_TBL_EVENT_PENDING(Txn, Evt) UT_TBL_CheckTxnHasEventImpl(Txn, Evt, __FILE__, __LINE__, #Evt)

#define UT_TBL_APPID_1  CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 1))
#define UT_TBL_APPID_2  CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 2))
#define UT_TBL_APPID_3  CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 3))
#define UT_TBL_APPID_10 CFE_ES_APPID_C(CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE + 10))

#define UT_HANDLE_INDEX(x)     ((x)-CFE_TBL_Global.Handles)
#define UT_CFE_TBL_HANDLE_INVL CFE_TBL_HANDLEID_UNDEFINED
#define UT_CFE_TBL_HANDLE_0    CFE_TBL_HANDLEID_C(CFE_RESOURCEID_WRAP(CFE_TBL_HANDLE_BASE + 0))
#define UT_CFE_TBL_HANDLE_1    CFE_TBL_HANDLEID_C(CFE_RESOURCEID_WRAP(CFE_TBL_HANDLE_BASE + 1))
#define UT_CFE_TBL_HANDLE_2    CFE_TBL_HANDLEID_C(CFE_RESOURCEID_WRAP(CFE_TBL_HANDLE_BASE + 2))
#define UT_CFE_TBL_HANDLE_LAST \
    CFE_TBL_HANDLEID_C(CFE_RESOURCEID_WRAP(CFE_TBL_HANDLE_BASE + CFE_PLATFORM_TBL_MAX_NUM_HANDLES - 1))

#define UT_REGREC_INDEX(x)    ((x)-CFE_TBL_Global.Registry)
#define UT_CFE_TBL_REGID_INVL CFE_TBL_REGID_UNDEFINED
#define UT_CFE_TBL_REGID_0    CFE_TBL_REGID_C(CFE_RESOURCEID_WRAP(CFE_TBL_REGID_BASE + 0))
#define UT_CFE_TBL_REGID_1    CFE_TBL_REGID_C(CFE_RESOURCEID_WRAP(CFE_TBL_REGID_BASE + 1))
#define UT_CFE_TBL_REGID_2    CFE_TBL_REGID_C(CFE_RESOURCEID_WRAP(CFE_TBL_REGID_BASE + 2))
#define UT_CFE_TBL_REGID_LAST \
    CFE_TBL_REGID_C(CFE_RESOURCEID_WRAP(CFE_TBL_REGID_BASE + CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1))
#define UT_CFE_TBL_REGID_INVH CFE_TBL_REGID_C(CFE_RESOURCEID_WRAP(INT32_MAX))

#define UT_CFE_TBL_LOADBUFFID_GLB_BASE (CFE_TBL_LOADBUFFID_BASE)
#define UT_CFE_TBL_LOADBUFFID_REG_BASE (CFE_TBL_LOADBUFFID_BASE + CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS)

#define UT_CFE_TBL_LOADBUFFID_INVL    CFE_TBL_LOADBUFFID_UNDEFINED
#define UT_CFE_TBL_LOADBUFFID_GLB_0   CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_WRAP(UT_CFE_TBL_LOADBUFFID_GLB_BASE + 0))
#define UT_CFE_TBL_LOADBUFFID_GLB_1   CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_WRAP(UT_CFE_TBL_LOADBUFFID_GLB_BASE + 1))
#define UT_CFE_TBL_LOADBUFFID_REG_0_0 CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_WRAP(UT_CFE_TBL_LOADBUFFID_REG_BASE + 0))
#define UT_CFE_TBL_LOADBUFFID_REG_0_1 CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_WRAP(UT_CFE_TBL_LOADBUFFID_REG_BASE + 1))
#define UT_CFE_TBL_LOADBUFFID_REG_1_0 CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_WRAP(UT_CFE_TBL_LOADBUFFID_REG_BASE + 2))
#define UT_CFE_TBL_LOADBUFFID_REG_1_1 CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_WRAP(UT_CFE_TBL_LOADBUFFID_REG_BASE + 3))
#define UT_CFE_TBL_LOADBUFFID_LAST \
    CFE_TBL_LOADBUFFID_C(          \
        CFE_RESOURCEID_WRAP(UT_CFE_TBL_LOADBUFFID_GLB_BASE + CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - 1))
#define UT_CFE_TBL_LOADBUFFID_INVH CFE_TBL_LOADBUFFID_C(CFE_RESOURCEID_WRAP(INT32_MAX))

/* Set up buffer to provide to CFE_ES_GetPoolBuf handler */
#define UT_TBL_LOAD_BUFFER_SIZE \
    (CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS * (CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + sizeof(CFE_ES_PoolAlign_t)))
static union
{
    CFE_ES_PoolAlign_t Align;
    uint8              Bytes[UT_TBL_LOAD_BUFFER_SIZE];
} UT_TBL_LoadBuffer;

void * Tbl1Ptr = NULL;
void * Tbl2Ptr = NULL;
void **ArrayOfPtrsToTblPtrs[2];

/* Normal dispatching registers the MsgID+CC in order to follow a
 * certain path through a series of switch statements */
#define TBL_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TBL_##intf##_MID)

#define TBL_UT_MSG_DISPATCH(intf, cmd)       TBL_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_TBL_##cmd)
#define TBL_UT_CC_DISPATCH(intf, cc, cmd)    TBL_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define TBL_UT_ERROR_DISPATCH(intf, cc, err) TBL_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_MSG_HK =
    { TBL_UT_MSG_DISPATCH(SEND_HK, SendHkCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_NOOP_CC =
    { TBL_UT_CC_DISPATCH(CMD, CFE_TBL_NOOP_CC, NoopCmd) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC =
    { TBL_UT_CC_DISPATCH(CMD, CFE_TBL_RESET_COUNTERS_CC, ResetCountersCmd) };

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_CC =
    { TBL_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */

CFE_TBL_RegistryRec_t Original[CFE_PLATFORM_TBL_MAX_NUM_TABLES];

/*
 * UT helper routines
 */

CFE_TBL_AccessDescriptor_t *UT_TBL_AccDescFromExtHandle(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_HandleId_t HandleId;

    HandleId = CFE_TBL_HANDLE_IMPORT(TblHandle);

    return CFE_TBL_LocateAccDescByHandle(HandleId);
}

CFE_TBL_Handle_t UT_TBL_AccDescToExtHandle(CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    CFE_TBL_Handle_t TblHandle;

    TblHandle = CFE_TBL_HANDLE_EXPORT(AccDescPtr->HandleId);

    return TblHandle;
}

void UT_TBL_SetupTxnFromExtHandle(CFE_TBL_TxnState_t *Txn, CFE_TBL_Handle_t TblHandle, uint32 Context)
{
    CFE_TBL_HandleId_t HandleId;

    HandleId = CFE_TBL_HANDLE_IMPORT(TblHandle);

    CFE_UtAssert_SETUP(CFE_TBL_TxnStartFromHandle(Txn, HandleId, Context));
}

CFE_TBL_TableConfig_t *UT_TBL_Config(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return &RegRecPtr->Config;
}

CFE_TBL_TableStatus_t *UT_TBL_Status(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return &RegRecPtr->Status;
}

void UT_TBL_ForEveryAccDesc(void (*Func)(CFE_TBL_AccessDescriptor_t *))
{
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    uint32                      i;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES; i++)
    {
        AccDescPtr = &CFE_TBL_Global.Handles[i];

        Func(AccDescPtr);
    }
}

void UT_TBL_ForEveryRegRec(void (*Func)(CFE_TBL_RegistryRec_t *))
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    uint32                 i;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[i];

        Func(RegRecPtr);
    }
}

void UT_TBL_ForEveryLoadBuff(void (*Func)(CFE_TBL_LoadBuff_t *))
{
    CFE_TBL_LoadBuff_t *LoadBuffPtr;
    uint32              i;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        LoadBuffPtr = &CFE_TBL_Global.LoadBuffs[i];

        Func(LoadBuffPtr);
    }
}

void UT_TBL_SetupSingleReg(CFE_TBL_RegistryRec_t **RegRecOut, CFE_TBL_AccessDescriptor_t **AccDescOut, uint16 Flags)
{
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_Handle_t            Handle;
    static uint32               Num = 0;
    char                        TableName[16];

    ++Num;
    snprintf(TableName, sizeof(TableName), "ut%03u", (unsigned int)Num);

    CFE_UtAssert_SETUP(CFE_TBL_Register(&Handle, TableName, sizeof(UT_Table1_t), Flags, NULL));

    AccDescPtr = UT_TBL_AccDescFromExtHandle(Handle);
    if (AccDescPtr != NULL)
    {
        RegRecPtr = CFE_TBL_LocateRegRecByID(AccDescPtr->RegIndex);
    }
    else
    {
        RegRecPtr = NULL;
    }

    if (AccDescOut != NULL)
    {
        *AccDescOut = AccDescPtr;
    }

    if (RegRecOut != NULL)
    {
        *RegRecOut = RegRecPtr;
    }
}

void UT_TBL_SetupHeader(CFE_TBL_File_Hdr_t *TblFileHeader, size_t Offset, size_t NumBytes)
{
    TblFileHeader->Offset   = Offset;
    TblFileHeader->NumBytes = NumBytes;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&TblFileHeader->Offset);
        CFE_TBL_ByteSwapUint32(&TblFileHeader->NumBytes);
    }
}

/* Sets up the indicated validation request/result buffer as VALIDATION_PENDING */
void UT_TBL_SetupPendingValidation(uint32 ArrayIndex, bool UseActive, CFE_TBL_RegistryRec_t *RegRecPtr,
                                   CFE_TBL_ValidationResult_t **ValResultOut)
{
    CFE_TBL_ValidationResult_t *ValResultPtr;
    CFE_ResourceId_t            PendingId;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;

    ValResultPtr = &CFE_TBL_Global.ValidationResults[ArrayIndex];
    PendingId    = CFE_ResourceId_FromInteger(CFE_TBL_VALRESULTID_BASE + ArrayIndex);

    memset(ValResultPtr, 0, sizeof(*ValResultPtr));

    ValResultPtr->State = CFE_TBL_VALIDATION_PENDING;

    ValResultPtr->ValId        = CFE_TBL_VALRESULTID_C(PendingId);
    ValResultPtr->ActiveBuffer = UseActive;

    snprintf(ValResultPtr->TableName, sizeof(ValResultPtr->TableName), "ut_cfe_tbl.UT_Table%u",
             (unsigned int)ArrayIndex + 1);

    if (RegRecPtr != NULL)
    {
        if (UseActive)
        {
            RegRecPtr->ValidateActiveId = ValResultPtr->ValId;
        }
        else
        {
            RegRecPtr->ValidateInactiveId = ValResultPtr->ValId;
            CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr);
        }
    }

    if (ValResultOut != NULL)
    {
        *ValResultOut = ValResultPtr;
    }
}

/* Resets the indicated validation request/result buffer to the free/unused state */
void UT_TBL_ResetValidationState(uint32 ArrayIndex)
{
    CFE_TBL_ValidationResult_t *ValResultPtr;
    ValResultPtr = &CFE_TBL_Global.ValidationResults[ArrayIndex];
    memset(ValResultPtr, 0, sizeof(*ValResultPtr));
}

void UT_TBL_SetupPendingDump(uint32 ArrayIndex, CFE_TBL_LoadBuff_t *DumpBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                             CFE_TBL_DumpControl_t **DumpCtrlOut)
{
    CFE_TBL_DumpControl_t *DumpCtrlPtr;
    CFE_ResourceId_t       PendingId;

    DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[ArrayIndex];

    PendingId = CFE_ResourceId_FromInteger(CFE_TBL_DUMPCTRLID_BASE + ArrayIndex);

    memset(DumpCtrlPtr, 0, sizeof(*DumpCtrlPtr));

    DumpCtrlPtr->State   = CFE_TBL_DUMP_PENDING;
    DumpCtrlPtr->BlockId = CFE_TBL_DUMPCTRLID_C(PendingId);
    if (RegRecPtr != NULL)
    {
        DumpCtrlPtr->SourceBuffId = RegRecPtr->Status.ActiveBufferId;
    }
    DumpCtrlPtr->DumpBufferPtr = DumpBufferPtr;

    snprintf(DumpCtrlPtr->TableName, sizeof(DumpCtrlPtr->TableName), "ut_cfe_tbl.UT_Table%u",
             (unsigned int)ArrayIndex + 1);

    if (RegRecPtr != NULL)
    {
        RegRecPtr->DumpControlId = DumpCtrlPtr->BlockId;
        if (DumpBufferPtr != NULL)
        {
            DumpBufferPtr->OwnerRegId = CFE_TBL_RegRecGetID(RegRecPtr);
        }
    }

    if (DumpCtrlOut != NULL)
    {
        *DumpCtrlOut = DumpCtrlPtr;
    }
}

/* Resets the indicated dump control block to the free/unused state */
void UT_TBL_ResetDumpCtrlState(uint32 ArrayIndex)
{
    CFE_TBL_DumpControl_t *DumpCtrlPtr;
    DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[ArrayIndex];
    memset(DumpCtrlPtr, 0, sizeof(*DumpCtrlPtr));
}

/* Simple helper to set a string in a fixed size buffer and ensure null term (repeated logic) */
static void UT_TBL_SetName(char *BufPtr, size_t BufSz, const char *Src)
{
    strncpy(BufPtr, Src, BufSz - 1);
    BufPtr[BufSz - 1] = '\0';
}

static void UT_ArrayConfigHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_Config_ArrayValue_t Val = *((const CFE_Config_ArrayValue_t *)UserObj);
    UT_Stub_SetReturnValue(FuncKey, Val);
}

static void UT_TBL_SetDumpOnly(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    UT_TBL_Config(RegRecPtr)->DumpOnly = true;
}

static void UT_TBL_ClearLoadPending(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    UT_TBL_Status(RegRecPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Status(RegRecPtr)->LoadPending  = false;
}

static void UT_TBL_SetLoadBuffTaken(CFE_TBL_LoadBuff_t *LoadBuffPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                                    CFE_ResourceId_t PendingId)
{
    CFE_TBL_RegId_t RegId;

    if (RegRecPtr != NULL)
    {
        RegId = CFE_TBL_RegRecGetID(RegRecPtr);
    }
    else
    {
        RegId = CFE_TBL_REGID_UNDEFINED;
    }

    if (!CFE_ResourceId_IsDefined(PendingId))
    {
        /* make one up */
        PendingId = CFE_ResourceId_FromInteger(CFE_TBL_LOADBUFFID_BASE + (LoadBuffPtr - CFE_TBL_Global.LoadBuffs));
    }

    CFE_TBL_LoadBuffSetUsed(LoadBuffPtr, PendingId, RegId);
}

static void UT_TBL_SetLoadBuffFree(CFE_TBL_LoadBuff_t *LoadBuffPtr)
{
    CFE_TBL_LoadBuffSetFree(LoadBuffPtr);
}

static void UT_TBL_SetHandleUsed(CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    CFE_ResourceId_t PendingId;

    if (!CFE_TBL_AccDescIsUsed(AccDescPtr))
    {
        /* make up an ID */
        PendingId = CFE_ResourceId_FromInteger(CFE_TBL_HANDLE_BASE + UT_HANDLE_INDEX(AccDescPtr));

        CFE_TBL_AccDescSetUsed(AccDescPtr, PendingId);
        AccDescPtr->RegIndex = UT_CFE_TBL_REGID_0;
    }
}

static void UT_TBL_GlobalDataReset(void)
{
    memset(&CFE_TBL_Global, 0, sizeof(CFE_TBL_Global));

    UT_ResetPoolBufferIndex();
    UtAssert_VOIDCALL(CFE_TBL_EarlyInit());
    UtAssert_VOIDCALL(CFE_TBL_InitData());

    CFE_TBL_Global.TableTaskAppId = UT_TBL_APPID_10;
}

/* Sets up a "load in progress" buffer */
CFE_TBL_LoadBuff_t *UT_TBL_SetupLoadBuff(CFE_TBL_RegistryRec_t *RegRecPtr, bool UseLocalBuffer, uint32 BuffNum)
{
    static uint32        Serial = 0;
    uint32               ReqCategory;
    CFE_TBL_LoadBuffId_t BuffId;
    CFE_TBL_LoadBuff_t * BuffPtr;

    if (UseLocalBuffer)
    {
        BuffPtr     = &RegRecPtr->Buffers[BuffNum];
        ReqCategory = 1 + UT_REGREC_INDEX(RegRecPtr);
    }
    else
    {
        BuffPtr     = &CFE_TBL_Global.LoadBuffs[BuffNum];
        ReqCategory = 0;
    }

    CFE_TBL_LoadBuffSerialCompose(&Serial, ReqCategory, BuffNum);
    UT_TBL_SetLoadBuffTaken(BuffPtr, RegRecPtr, CFE_ResourceId_FromInteger(CFE_TBL_LOADBUFFID_BASE + Serial));
    BuffId = CFE_TBL_LoadBufferGetID(BuffPtr);

    if (RegRecPtr != NULL)
    {
        UT_TBL_Config(RegRecPtr)->DoubleBuffered = UseLocalBuffer;
        UT_TBL_Status(RegRecPtr)->NextBufferId   = BuffId;
    }

    UtAssert_ADDRESS_EQ(CFE_TBL_LocateLoadBufferByID(BuffId), BuffPtr);

    return BuffPtr;
}

CFE_TBL_LoadBuff_t *UT_TBL_InitActiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr, uint32 BuffNum)
{
    static uint32       Serial = 0;
    uint32              ReqCategory;
    CFE_TBL_LoadBuff_t *BuffPtr;

    BuffPtr     = &RegRecPtr->Buffers[BuffNum];
    ReqCategory = 1 + UT_REGREC_INDEX(RegRecPtr);

    CFE_TBL_LoadBuffSerialCompose(&Serial, ReqCategory, BuffNum);
    CFE_TBL_LoadBuffSetUsed(BuffPtr, CFE_ResourceId_FromInteger(CFE_TBL_LOADBUFFID_BASE + Serial),
                            CFE_TBL_RegRecGetID(RegRecPtr));

    CFE_TBL_SetActiveBuffer(RegRecPtr, BuffPtr);

    return BuffPtr;
}

void UT_TBL_SetActiveBufferAddr(CFE_TBL_RegistryRec_t *RegRecPtr, uint32 BuffNum, void *Addr)
{
    CFE_TBL_LoadBuff_t *BuffPtr;

    BuffPtr = UT_TBL_InitActiveBuffer(RegRecPtr, BuffNum);

    if (Addr != NULL)
    {
        BuffPtr->BufferPtr = Addr;
    }
}

static bool UT_TBL_CheckTxnHasEventHelper(const CFE_TBL_TxnEvent_t *Txn, void *Arg)
{
    uint16 EventId = *((const uint16 *)Arg);

    return (Txn->EventId == EventId);
}

void UT_TBL_CheckTxnHasEventImpl(const CFE_TBL_TxnState_t *Txn, uint16 EventId, const char *File, uint32 Line,
                                 const char *EventName)
{
    uint32 Count;

    Count = CFE_TBL_TxnProcessEvents(Txn, UT_TBL_CheckTxnHasEventHelper, &EventId);
    UtAssert_GenericUnsignedCompare(Count, UtAssert_Compare_NEQ, 0, UtAssert_Radix_DECIMAL, File, Line,
                                    "Event Pending: ", EventName, "");
}

#define UT_TBL_ADD_TEST(Func) UtTest_Add(Func, UT_TBL_GlobalDataReset, NULL, #Func)

/*
** Functions
*/
void UtTest_Setup(void)
{
    /* Initialize unit test */
    UT_Init("tbl");
    UtPrintf("cFE TBL Unit Test Output File\n\n");
    UT_InitializeTableRegistryNames();

    /* cfe_tbl_task.c functions */
    UT_ADD_TEST(Test_CFE_TBL_TaskInit);
    UT_ADD_TEST(Test_CFE_TBL_InitData);

    /*
     * Transaction access patterns
     * (do this early because many other APIs depend on these working correctly)
     */
    UT_TBL_ADD_TEST(Test_CFE_TBL_TxnState);
    UT_TBL_ADD_TEST(Test_CFE_TBL_TxnEvents);

    /*
     * Shared resource access patterns
     * (do this early because many other APIs depend on these working correctly)
     */
    UT_TBL_ADD_TEST(Test_CFE_TBL_ResourceID_ValidationResult);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ResourceID_RegistryRecord);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ResourceID_AccessDescriptor);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ResourceID_DumpControl);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ResourceID_LoadBuff);

    /* Test logic that is shared between API and commands */
    UT_TBL_ADD_TEST(Test_CFE_TBL_TableLoadCommon);
    UT_TBL_ADD_TEST(Test_CFE_TBL_TableDumpCommon);

    /* cfe_tbl_task_cmds.c functions */
    /* This should be done first (it initializes working data structures) */
    UT_TBL_ADD_TEST(Test_CFE_TBL_DeleteCDSCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_TlmRegCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_AbortLoadCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ActivateCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_DumpToFile);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ResetCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ValidateCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_NoopCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_GetTblRegData);
    UT_TBL_ADD_TEST(Test_CFE_TBL_GetHkData);
    UT_TBL_ADD_TEST(Test_CFE_TBL_DumpRegCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_DumpCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_LoadCmd);
    UT_TBL_ADD_TEST(Test_CFE_TBL_SendHkCmd);

    /* cfe_tbl_api.c and cfe_tbl_internal.c functions */
    // UT_TBL_ADD_TEST(Test_CFE_TBL_ApiInit);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Register);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Share);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Unregister);
    UT_TBL_ADD_TEST(Test_CFE_TBL_NotifyByMessage);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Load);
    UT_TBL_ADD_TEST(Test_CFE_TBL_GetAddress);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ReleaseAddress);
    UT_TBL_ADD_TEST(Test_CFE_TBL_GetAddresses);
    UT_TBL_ADD_TEST(Test_CFE_TBL_ReleaseAddresses);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Validate);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Manage);
    UT_TBL_ADD_TEST(Test_CFE_TBL_DumpToBuffer);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Update);
    UT_TBL_ADD_TEST(Test_CFE_TBL_GetStatus);
    UT_TBL_ADD_TEST(Test_CFE_TBL_GetInfo);
    UT_TBL_ADD_TEST(Test_CFE_TBL_TblMod);

    /* Miscellaneous cfe_tbl_internal.c tests */
    UT_TBL_ADD_TEST(Test_CFE_TBL_Internal1);
    UT_TBL_ADD_TEST(Test_CFE_TBL_Internal2);
}

void UT_TBL_SetupRegRec(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* This should always use the array index of the reg entry */
    size_t           Idx;
    CFE_ResourceId_t PendingId;

    Idx = UT_REGREC_INDEX(RegRecPtr);

    snprintf(UT_TBL_Config(RegRecPtr)->Name, CFE_TBL_MAX_FULL_NAME_LEN, "i%d", (int)Idx);
    RegRecPtr->OwnerAppId = UT_TBL_APPID_2;

    /* make up an ID */
    PendingId = CFE_ResourceId_FromInteger(CFE_TBL_REGID_BASE + Idx);
    CFE_TBL_RegRecSetUsed(RegRecPtr, PendingId);
}

void UT_TBL_SetupAccDesc(CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    memset(AccDescPtr, 0, sizeof(*AccDescPtr));
}

/*
** Fill the whole table registry with known table names and set an owner ID
*/
void UT_InitializeTableRegistryNames(void)
{
    UT_TBL_ForEveryRegRec(UT_TBL_SetupRegRec);
}

/*
** Initialize table registry values
*/
void UT_ResetTableRegistry(void)
{
    int32 i;

    UT_TBL_ForEveryRegRec(CFE_TBL_InitRegistryRecord);

    /* Initialize the table access descriptors */
    UT_TBL_ForEveryAccDesc(UT_TBL_SetupAccDesc);

    /* Initialize the table validation results records */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS; i++)
    {
        UT_TBL_ResetValidationState(i);
    }

    /* Initialize the dump-only table dump control blocks */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        UT_TBL_ResetDumpCtrlState(i);
    }

    /* Free all shared buffers */
    UT_TBL_ForEveryLoadBuff(UT_TBL_SetLoadBuffFree);

    CFE_TBL_Global.ValidationCounter = 0;

    CFE_TBL_Global.HkTlmTblRegId  = CFE_TBL_REGID_UNDEFINED;
    CFE_TBL_Global.LastTblUpdated = CFE_TBL_REGID_UNDEFINED;
}

/*
** Tests to cover table task initialization functions
*/
void Test_CFE_TBL_TaskInit(void)
{
    uint32 ExitCode;
    union
    {
        CFE_TBL_NoopCmd_t          NoopCmd;
        CFE_TBL_SendHkCmd_t        SendHkCmd;
        CFE_TBL_ResetCountersCmd_t ResetCountersCmd;
        CFE_MSG_Message_t          Msg;
    } CmdBuf;
    CFE_SB_MsgId_t    MsgId   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t FcnCode = 0;

    UtPrintf("Begin Test Task Init");

    memset(&CmdBuf, 0, sizeof(CmdBuf));

    /* Test successful table services main entry point execution */
    UT_InitData();
    ExitCode = 0;
    UT_SetDataBuffer(UT_KEY(CFE_ES_ExitApp), &ExitCode, sizeof(ExitCode), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetFcnCode), &FcnCode, sizeof(FcnCode), false);
    UtAssert_VOIDCALL(CFE_TBL_TaskMain());
    UtAssert_INT32_EQ(ExitCode, CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 1);

    /* Main task initialization failure */
    UT_InitData();
    ExitCode = 0;
    UT_SetDataBuffer(UT_KEY(CFE_ES_ExitApp), &ExitCode, sizeof(ExitCode), false);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);
    UtAssert_VOIDCALL(CFE_TBL_TaskMain());
    UtAssert_INT32_EQ(ExitCode, CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    /* Since stub doesn't actually cause an exit, will get called twice */
    UtAssert_STUB_COUNT(CFE_ES_ExitApp, 2);

    /* Test successful table services core application initialization */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_TaskInit());

    /* Test table services core application initialization response to a pipe
     * creation failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -2);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -2);

    /* Test table services core application initialization response to a
     * housekeeping request subscription error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, -3);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -3);

    /* Test table services core application initialization response to a
     * ground command subscription error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -4);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -4);

    /* Test table services core application initialization response to a
     * send initialization event error
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, -5);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -5);

    /* Test table services core application initialization response to an
     * EVS register failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -6);
    UtAssert_INT32_EQ(CFE_TBL_TaskInit(), -6);

    /* Test command pipe messages handler response to a valid command */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd), UT_TPID_CFE_TBL_CMD_NOOP_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NOOP_INF_EID);

    /* Test command pipe messages handler response to an invalid
     * message length
     */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd) - 1, UT_TPID_CFE_TBL_CMD_NOOP_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LEN_ERR_EID);

    /* Test command pipe messages handler response to an invalid
     * command code
     */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd), UT_TPID_CFE_TBL_CMD_INVALID_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_CC1_ERR_EID);

    /* Test command pipe messages handler response to other errors */
    UT_InitData();
    CFE_TBL_Global.CommandCounter      = 0;
    CFE_TBL_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.NoopCmd), UT_TPID_CFE_TBL_INVALID_MID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_MID_ERR_EID);
    UtAssert_ZERO(CFE_TBL_Global.CommandCounter);
    UtAssert_ZERO(CFE_TBL_Global.CommandErrorCounter);

    /* Test command pipe messages handler response to "message type" message */
    UT_InitData();
    CFE_TBL_Global.CommandCounter      = 0;
    CFE_TBL_Global.CommandErrorCounter = 0;
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.SendHkCmd), UT_TPID_CFE_TBL_MSG_HK);
    UtAssert_ZERO(CFE_TBL_Global.CommandCounter);
    UtAssert_ZERO(CFE_TBL_Global.CommandErrorCounter);

    /* Test command pipe messages handler response to "command type" message */
    UT_InitData();
    UT_CallTaskPipe(CFE_TBL_TaskPipe, CFE_MSG_PTR(CmdBuf), sizeof(CmdBuf.ResetCountersCmd),
                    UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC);
    CFE_UtAssert_EVENTSENT(CFE_TBL_RESET_INF_EID);
    UtAssert_ZERO(CFE_TBL_Global.CommandCounter);
    UtAssert_ZERO(CFE_TBL_Global.CommandErrorCounter);
}

/*
** Test table service application data initialization
*/
void Test_CFE_TBL_InitData(void)
{
    UtPrintf("Begin Test Init Data");

    /* This function has only one possible path with no return code */
    UT_InitData();
    CFE_TBL_InitData();
    UtAssert_STUB_COUNT(CFE_MSG_Init, 3);
}

/*
** Test the delete critical table's CDS command message
*/
void Test_CFE_TBL_DeleteCDSCmd(void)
{
    int                    j, k;
    CFE_TBL_DeleteCDSCmd_t DelCDSCmd;

    UtPrintf("Begin Test Delete CDS Command");

    /* Test successfully finding the table name in the table registry */
    UT_InitData();
    UT_TBL_SetName(DelCDSCmd.Payload.TableName, sizeof(DelCDSCmd.Payload.TableName), "0");
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test case where table is still in the table registry - CDS cannot be deleted yet */
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "ut", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    UT_TBL_SetName(DelCDSCmd.Payload.TableName, sizeof(DelCDSCmd.Payload.TableName), "ut_cfe_tbl.ut");
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_IN_REGISTRY_ERR_EID);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test failure to find table in the critical table registry */
    UT_InitData();
    k = CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES + CFE_PLATFORM_TBL_MAX_NUM_TABLES;

    for (j = CFE_PLATFORM_TBL_MAX_NUM_TABLES; j < k; j++)
    {
        snprintf(CFE_TBL_Global.CritReg[j - CFE_PLATFORM_TBL_MAX_NUM_TABLES].Name, CFE_TBL_MAX_FULL_NAME_LEN, "%d", j);
    }

    UT_TBL_SetName(DelCDSCmd.Payload.TableName, sizeof(DelCDSCmd.Payload.TableName), "-1");
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test finding the table in the critical table registry, but CDS is not
     * tagged as a table
     */
    UT_InitData();
    snprintf(DelCDSCmd.Payload.TableName, sizeof(DelCDSCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES + CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_CDS_WRONG_TYPE_ERR);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test deletion when CDS owning application is still active */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_CDS_OWNER_ACTIVE_ERR);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test deletion where the table cannot be located in the CDS registry */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test deletion error while deleting table from the CDS */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_SUCCESS - 1);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test successful removal of the table from the CDS */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_DeleteCDS), 1, CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_DeleteCDSCmd(&DelCDSCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);
}

/*
** Test the processing telemetry table registry command message function
*/
void Test_CFE_TBL_TlmRegCmd(void)
{
    CFE_TBL_SendRegistryCmd_t TlmRegCmd;
    CFE_TBL_RegistryRec_t *   RegRecPtr;

    UtPrintf("Begin Test Telemetry Registry Command");

    /* Test when table name does exist */
    UT_InitData();
    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);

    /* Registry[0].Name used because it is confirmed to be a registered
     * table name
     */
    UT_TBL_SetName(TlmRegCmd.Payload.TableName, sizeof(TlmRegCmd.Payload.TableName), CFE_TBL_RegRecGetName(RegRecPtr));
    UtAssert_INT32_EQ(CFE_TBL_SendRegistryCmd(&TlmRegCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test when table name does not exist */
    UT_InitData();

    snprintf(TlmRegCmd.Payload.TableName, sizeof(TlmRegCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_SendRegistryCmd(&TlmRegCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
}

/*
** Test the processing abort load command message function
*/
void Test_CFE_TBL_AbortLoadCmd(void)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_AbortLoadCmd_t AbortLdCmd;
    CFE_TBL_LoadBuff_t *   LoadBuffPtr;

    UtPrintf("Begin Test Abort Load Command");
    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);

    /* Test when table name does exist and a table load is in progress */
    UT_InitData();

    /* Entering the if statement with a table name that has to be in
     * the registry
     */
    UT_TBL_SetName(AbortLdCmd.Payload.TableName, sizeof(AbortLdCmd.Payload.TableName),
                   CFE_TBL_RegRecGetName(RegRecPtr));

    UT_TBL_Status(RegRecPtr)->NextBufferId = UT_CFE_TBL_LOADBUFFID_GLB_1;
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test when table name does exist but no table load is in progress */
    UT_InitData();
    UT_TBL_Status(RegRecPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test when table name does exist, a table load is in progress, and the
     * table is dump only
     */
    UT_InitData();
    UT_TBL_Status(RegRecPtr)->NextBufferId = UT_CFE_TBL_LOADBUFFID_GLB_0;
    UT_TBL_Config(RegRecPtr)->DumpOnly     = true;
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test when table name not found in the registry */
    UT_InitData();
    snprintf(AbortLdCmd.Payload.TableName, sizeof(AbortLdCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_AbortLoadCmd(&AbortLdCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test when table is double buffered */
    UT_InitData();
    LoadBuffPtr = UT_TBL_SetupLoadBuff(RegRecPtr, true, 0);
    UtAssert_BOOL_TRUE(CFE_TBL_LoadBuffIsUsed(LoadBuffPtr));
    CFE_TBL_AbortLoad(RegRecPtr);
    UtAssert_BOOL_TRUE(CFE_TBL_LoadBuffIsUsed(LoadBuffPtr));
}

/*
** Test the activate table command message function
*/
void Test_CFE_TBL_ActivateCmd(void)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_ActivateCmd_t  ActivateCmd;
    CFE_TBL_LoadBuff_t *   LoadBuffPtr;

    UtPrintf("Begin Test Activate Command");
    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);

    /* Enter the if statement with a table name that is in the registry */
    UT_TBL_SetName(ActivateCmd.Payload.TableName, sizeof(ActivateCmd.Payload.TableName),
                   CFE_TBL_RegRecGetName(RegRecPtr));

    /* Test when table name exists, but attempts to activate a dump-only
     * table
     */
    UT_InitData();
    UT_TBL_Config(RegRecPtr)->DumpOnly = true;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, and the table is double-buffered
     */
    UT_InitData();
    UT_TBL_Config(RegRecPtr)->DumpOnly = false;
    UT_TBL_SetupLoadBuff(RegRecPtr, true, 0);
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test when table name exists, the table is not a dump-only, a load is in
     * progress, the table isn't double-buffered, and ValidationStatus = true
     */
    UT_InitData();
    LoadBuffPtr               = UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    LoadBuffPtr->Validated    = true;
    RegRecPtr->Notify.Enabled = false;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test when table name exists, the table is not a dump-only, load in
     * progress, and a notification message should be sent
     */
    UT_InitData();
    RegRecPtr->Notify.Enabled = true;
    LoadBuffPtr               = UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    LoadBuffPtr->Validated    = true;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test when table name exists, the table is not a dump-only, no load is in
     * progress, and no notification message should be sent
     */
    UT_InitData();
    RegRecPtr->Notify.Enabled              = false;
    UT_TBL_Status(RegRecPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test when table name exists, the table is not a dump-only, no load in in
     * progress, and a notification message should be sent
     */
    UT_InitData();
    RegRecPtr->Notify.Enabled              = true;
    UT_TBL_Status(RegRecPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Same as above but sending the notification fails, this should trigger an event
     */
    UT_InitData();
    RegRecPtr->Notify.Enabled = true;
    LoadBuffPtr               = UT_TBL_SetupLoadBuff(RegRecPtr, false, 1);
    LoadBuffPtr->Validated    = true;
    UT_SetDefaultReturnValue(UT_KEY(CFE_SB_TransmitMsg), CFE_SB_BUF_ALOC_ERR);
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FAIL_NOTIFY_SEND_ERR_EID);

    /* Test when the table name doesn't exist */
    UT_InitData();
    snprintf(ActivateCmd.Payload.TableName, sizeof(ActivateCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_ActivateCmd(&ActivateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
}

/*
** Test the write table data to a file function
*/
void Test_CFE_TBL_DumpToFile(void)
{
    const char TableData[]    = "dumptest";
    size_t     TblSizeInBytes = sizeof(TableData);

    CFE_TBL_DumpControl_t *DumpCtlPtr;
    CFE_TBL_LoadBuff_t *   BuffPtr;

    BuffPtr = UT_TBL_SetupLoadBuff(NULL, false, 0);
    UT_TBL_SetupPendingDump(0, BuffPtr, NULL, &DumpCtlPtr);

    strncpy(DumpCtlPtr->DumpBufferPtr->DataSource, "filename", sizeof(DumpCtlPtr->DumpBufferPtr->DataSource));
    CFE_TBL_LoadBuffSetContentSize(BuffPtr, sizeof(TableData));
    DumpCtlPtr->State = CFE_TBL_DUMP_PERFORMED;

    UtPrintf("Begin Test Dump to File");

    /* Test with an error creating the dump file */
    UT_InitData();

    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtlPtr), CFE_TBL_ERR_ACCESS);

    /* Test with an error writing the cFE file header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtlPtr), CFE_TBL_ERR_ACCESS);

    /* Test with an error writing the table file header */
    UT_InitData();

    /* Set the count for the FSWriteHdrRtn return code variable to a large
     * enough value to pass through every time
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 6, sizeof(CFE_FS_Header_t));
    UT_SetDeferredRetcode(UT_KEY(OS_write), 1, sizeof(CFE_TBL_File_Hdr_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtlPtr), CFE_TBL_ERR_ACCESS);

    /* Test with an error writing the table to a file */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, TblSizeInBytes - 1);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtlPtr), CFE_TBL_ERR_ACCESS);

    /* Test successful file creation and data dumped */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtlPtr), CFE_SUCCESS);

    /* Test where file already exists so data is overwritten */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtlPtr), CFE_SUCCESS);
}

/*
** Test the processing reset counters command message function
*/
void Test_CFE_TBL_ResetCmd(void)
{
    UtPrintf("Begin Test Reset Command");

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_ResetCountersCmd(NULL), CFE_SUCCESS);
    UtAssert_ZERO(CFE_TBL_Global.CommandCounter);
    UtAssert_ZERO(CFE_TBL_Global.CommandErrorCounter);
}

/*
** Test the validate table command message function
*/
void Test_CFE_TBL_ValidateCmd(void)
{
    uint8                     Buff;
    uint8 *                   BuffPtr = &Buff;
    CFE_TBL_ValidateCmd_t     ValidateCmd;
    CFE_TBL_CallbackFuncPtr_t ValFuncPtr = (CFE_TBL_CallbackFuncPtr_t)((unsigned long)&UT_InitializeTableRegistryNames);
    CFE_TBL_RegistryRec_t *   RegRecPtr;
    CFE_TBL_LoadBuff_t *      LoadBuffPtr;

    UtPrintf("Begin Test Validate Command");
    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);

    /* Test when table name is not found in the registry */
    UT_InitData();
    snprintf(ValidateCmd.Payload.TableName, sizeof(ValidateCmd.Payload.TableName), "%d",
             CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where the active buffer has data, but too many table validations
     * have been requested
     */
    UT_InitData();
    UT_TBL_SetName(ValidateCmd.Payload.TableName, sizeof(ValidateCmd.Payload.TableName),
                   CFE_TBL_RegRecGetName(RegRecPtr));
    ValidateCmd.Payload.ActiveTableFlag = CFE_TBL_BufferSelect_ACTIVE;

    UT_TBL_SetActiveBufferAddr(RegRecPtr, 0, BuffPtr);

    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where the active buffer has data, but there is no validation
     * function pointer
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = NULL;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test where the active buffer has data, the validation function pointer
     * exists, and the active table flag is set
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = ValFuncPtr;
    ValidateCmd.Payload.ActiveTableFlag         = true;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test with the buffer inactive, the table is double-buffered, and the
     * validation function pointer exists
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    UT_TBL_InitActiveBuffer(RegRecPtr, 1);
    ValidateCmd.Payload.ActiveTableFlag         = CFE_TBL_BufferSelect_INACTIVE;
    UT_TBL_Config(RegRecPtr)->DoubleBuffered    = true;
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = ValFuncPtr;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test with the buffer inactive, the table is single-buffered with a
     * load in progress, the validation function pointer exists, and no
     * notification message should be sent
     */
    UT_InitData();
    UT_TBL_ResetValidationState(0);
    RegRecPtr->Notify.Enabled = false;
    LoadBuffPtr               = UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    LoadBuffPtr->BufferPtr    = BuffPtr;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test with the buffer inactive, the table is single-buffered with a
     * load in progress, the validation function pointer exists, and a
     * notification message should be sent
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    UT_TBL_ResetValidationState(0);
    RegRecPtr->Notify.Enabled = true;
    LoadBuffPtr               = UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    LoadBuffPtr->BufferPtr    = BuffPtr;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test where no inactive buffer is present (single-buffered table without
     * load in progress)
     */
    UT_InitData();
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    UT_TBL_Config(RegRecPtr)->DoubleBuffered = false;
    UT_TBL_Status(RegRecPtr)->PrevBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Status(RegRecPtr)->NextBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with an illegal buffer */
    UT_InitData();
    ValidateCmd.Payload.ActiveTableFlag = 0xffff;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCmd(&ValidateCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
}

/*
** Test the processing no-operation command message function
*/
void Test_CFE_TBL_NoopCmd(void)
{
    UtPrintf("Begin Test No-Op Command");

    /* Test run through function (there are no additional paths) */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_NoopCmd(NULL), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);
}

/*
** Test the function which converts table registry entries for tables into
** messages
*/
void Test_CFE_TBL_GetTblRegData(void)
{
    CFE_TBL_RegistryRec_t *RegRecDPtr;
    CFE_TBL_RegistryRec_t *RegRecSPtr;

    UtPrintf("Begin Test Get Table Registry Command");
    UT_TBL_SetupSingleReg(&RegRecDPtr, NULL, CFE_TBL_OPT_DBL_BUFFER);
    UT_TBL_InitActiveBuffer(RegRecDPtr, 0);
    UT_TBL_InitActiveBuffer(RegRecDPtr, 1);
    UT_TBL_SetupSingleReg(&RegRecSPtr, NULL, CFE_TBL_OPT_DEFAULT);
    UT_TBL_InitActiveBuffer(RegRecSPtr, 0);

    /* Test using a double buffered table */
    UT_InitData();
    CFE_TBL_Global.HkTlmTblRegId                           = CFE_TBL_RegRecGetID(RegRecDPtr);
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr = CFE_ES_MEMADDRESS_C(0);
    CFE_TBL_GetTblRegData();
    UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr));

    /* Test using a single buffered table and the buffer is inactive */
    UT_InitData();
    CFE_TBL_Global.HkTlmTblRegId                           = CFE_TBL_RegRecGetID(RegRecSPtr);
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr = CFE_ES_MEMADDRESS_C(0);
    UT_TBL_SetupLoadBuff(RegRecSPtr, false, 1);
    CFE_TBL_GetTblRegData();
    UtAssert_NOT_NULL(CFE_ES_MEMADDRESS_TO_PTR(CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr));

    /* Test with no inactive buffer */
    UT_InitData();
    CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr = CFE_ES_MEMADDRESS_C(0);
    UT_TBL_Status(RegRecSPtr)->NextBufferId                = CFE_TBL_LOADBUFFID_UNDEFINED;
    CFE_TBL_GetTblRegData();
    UtAssert_NULL(CFE_ES_MEMADDRESS_TO_PTR(CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr));

    /* Test when   HkTlmTblRegId does not refer to anything valid */
    /* In this case it is a no-op so there is nothing to check for -- this is for branch coverage */
    CFE_TBL_Global.HkTlmTblRegId = UT_CFE_TBL_REGID_INVL;
    CFE_TBL_GetTblRegData();
}

/*
** Test the function that collects data and stores it in the housekeeping
** message
*/
void Test_CFE_TBL_GetHkData(void)
{
    int32                       ValTableIndex = CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS - 1;
    CFE_ES_AppId_t              AppID;
    CFE_TBL_ValidationResult_t *ValResultPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_LoadBuff_t *        LoadBuffPtr;

    /* Get the AppID being used for UT */
    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Get Housekeeping Data");

    UT_TBL_ForEveryRegRec(UT_TBL_ClearLoadPending);
    UT_TBL_ForEveryLoadBuff(UT_TBL_SetLoadBuffFree);

    /* Test raising the count of load pending tables */
    UT_InitData();
    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);
    UT_TBL_Status(RegRecPtr)->LoadPending = true;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.NumLoadPending, 1);

    /* Test lowering the count of free shared buffers */
    UT_InitData();
    LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_LAST);
    UT_TBL_SetLoadBuffTaken(LoadBuffPtr, RegRecPtr, CFE_RESOURCEID_UNDEFINED);
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.NumFreeSharedBufs, CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - 1);

    /* Test making a ValPtr with result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_Global.SuccessValCounter = 0;
    UT_TBL_SetupPendingValidation(ValTableIndex, false, NULL, &ValResultPtr);
    ValResultPtr->State = CFE_TBL_VALIDATION_PERFORMED;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.SuccessValCounter, 1);

    /* Test with a validation pending, not done yet */
    UT_InitData();
    CFE_TBL_Global.SuccessValCounter = 0;
    UT_TBL_SetupPendingValidation(ValTableIndex, false, NULL, &ValResultPtr);
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.SuccessValCounter, 0);

    /* Test making a ValPtr without result = CFE_SUCCESS */
    UT_InitData();
    CFE_TBL_Global.FailedValCounter = 0;
    UT_TBL_SetupPendingValidation(ValTableIndex, false, NULL, &ValResultPtr);
    ValResultPtr->State  = CFE_TBL_VALIDATION_PERFORMED;
    ValResultPtr->Result = CFE_SUCCESS - 1;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.FailedValCounter, 1);

    /* Test with an invalid registry entry */
    UT_InitData();
    CFE_TBL_Global.LastTblUpdated                          = UT_CFE_TBL_REGID_0;
    RegRecPtr                                              = CFE_TBL_LocateRegRecByID(CFE_TBL_Global.LastTblUpdated);
    RegRecPtr->OwnerAppId                                  = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds = 19283;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds, 19283);

    /* Test with invalid last valid table updated out of range (low) */
    UT_InitData();
    CFE_TBL_Global.LastTblUpdated                          = UT_CFE_TBL_REGID_INVL;
    CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds = 12345;
    CFE_TBL_GetHkData();
    UtAssert_UINT32_EQ(CFE_TBL_Global.HkPacket.Payload.LastUpdateTime.Seconds, 12345);
}

/*
** Test the function that processes dump table registration to file
** command message
*/
void Test_CFE_TBL_DumpRegCmd(void)
{
    int                       q;
    CFE_TBL_DumpRegistryCmd_t DumpRegCmd;
    CFE_ES_AppId_t            AppID;
    size_t                    LocalSize;
    void *                    LocalBuf;

    CFE_TBL_RegistryRec_t *     RegRec0Ptr;
    CFE_TBL_RegistryRec_t *     RegRec1Ptr;
    CFE_TBL_RegistryRec_t *     RegRecLastPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;

    UT_TBL_SetupSingleReg(&RegRec0Ptr, NULL, CFE_TBL_OPT_DEFAULT);
    UT_TBL_SetupSingleReg(&RegRec1Ptr, NULL, CFE_TBL_OPT_DEFAULT);
    RegRecLastPtr = CFE_TBL_LocateRegRecByID(UT_CFE_TBL_REGID_LAST);

    /* Get the AppID being used for UT */
    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Dump Register Command");

    for (q = 0; q < CFE_PLATFORM_TBL_MAX_NUM_TABLES; q++)
    {
        CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Registry[q].AccessList);
    }

    /* Test command using the default dump file name (nominal path) */
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), false);
    UT_TBL_SetName(DumpRegCmd.Payload.DumpFilename, sizeof(DumpRegCmd.Payload.DumpFilename), "X");
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test command with a bad file name */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    UT_ResetState(UT_KEY(CFE_FS_ParseInputFileNameEx));

    /* Test command with the dump file already pending (max requests pending) */
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    UT_ResetState(UT_KEY(CFE_FS_BackgroundFileDumpRequest));

    /* Test command with the dump file already pending (local) */
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), false);
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpRequest), CFE_STATUS_REQUEST_ALREADY_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_DumpRegistryCmd(&DumpRegCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Check event generators */
    UT_ClearEventHistory();
    CFE_TBL_Global.RegDumpState.FileExisted = true;
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0,
                                     1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_OVERWRITE_REG_DUMP_INF_EID);

    UT_ClearEventHistory();
    CFE_TBL_Global.RegDumpState.FileExisted = false;
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0,
                                     1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_REG_DUMP_INF_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR,
                                     CFE_SUCCESS, 10, 10, 1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_TBL_REG_ERR_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR,
                                     CFE_SUCCESS, 10, 10, 1000);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_CFE_HDR_ERR_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_CREATE_ERROR, OS_ERROR, 10, 0,
                                     0);
    CFE_UtAssert_EVENTSENT(CFE_TBL_CREATING_DUMP_FILE_ERR_EID);

    UT_ClearEventHistory();
    CFE_TBL_DumpRegistryEventHandler(&CFE_TBL_Global.RegDumpState, CFE_FS_FileWriteEvent_UNDEFINED, OS_ERROR, 0, 0, 0);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Check dumping a table that has not been loaded yet - this works but sets the time/crc to zero */
    RegRec0Ptr->OwnerAppId = AppID;
    CFE_TBL_HandleLinkInit(&RegRec0Ptr->AccessList);
    LocalBuf  = NULL;
    LocalSize = 0;
    UtAssert_BOOL_FALSE(
        CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, UT_REGREC_INDEX(RegRec0Ptr), &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);
    UtAssert_INT32_EQ(CFE_TBL_Global.RegDumpState.DumpRecord.LoadInProgress, -1); /* no load in progress */
    UtAssert_ZERO(CFE_TBL_Global.RegDumpState.DumpRecord.FileTime.Seconds);
    UtAssert_ZERO(CFE_TBL_Global.RegDumpState.DumpRecord.FileTime.Subseconds);
    UtAssert_ZERO(CFE_TBL_Global.RegDumpState.DumpRecord.Crc);

    /* Test where the table is owned, the file doesn't already exist, and the
     * table is successfully dumped
     */
    UT_InitData();
    UT_TBL_InitActiveBuffer(RegRec0Ptr, 0);
    RegRec1Ptr->OwnerAppId = CFE_TBL_NOT_OWNED;
    UT_TBL_SetupLoadBuff(RegRec0Ptr, true, 1);
    LocalBuf  = NULL;
    LocalSize = 0;
    UtAssert_BOOL_FALSE(
        CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, UT_REGREC_INDEX(RegRec0Ptr), &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);
    UtAssert_INT32_EQ(CFE_TBL_Global.RegDumpState.DumpRecord.LoadInProgress, -2);

    /* When load buffer points at secondary buffer, TLM index should be -3 */
    UT_TBL_InitActiveBuffer(RegRec0Ptr, 1);
    UT_TBL_SetupLoadBuff(RegRec0Ptr, true, 0);
    LocalBuf  = NULL;
    LocalSize = 0;
    UtAssert_BOOL_FALSE(
        CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, UT_REGREC_INDEX(RegRec0Ptr), &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);
    UtAssert_INT32_EQ(CFE_TBL_Global.RegDumpState.DumpRecord.LoadInProgress, -3);

    /* Same but not double buffered */
    UT_InitData();
    RegRec0Ptr->OwnerAppId = AppID;
    CFE_TBL_HandleLinkInit(&RegRec0Ptr->AccessList);
    RegRec1Ptr->OwnerAppId                    = CFE_TBL_NOT_OWNED;
    UT_TBL_Status(RegRec0Ptr)->NextBufferId   = UT_CFE_TBL_LOADBUFFID_LAST;
    UT_TBL_Config(RegRec0Ptr)->DoubleBuffered = false;
    LocalBuf                                  = NULL;
    LocalSize                                 = 0;
    UtAssert_BOOL_FALSE(
        CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, UT_REGREC_INDEX(RegRec0Ptr), &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);
    UtAssert_INT32_EQ(CFE_TBL_Global.RegDumpState.DumpRecord.LoadInProgress,
                      CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - 1);

    /*
     * Condition where the load buffer points to something it doesn't own.
     * (the FSW should never do this; this is to make sure it does not walk off the end of the array)
     */
    UT_InitData();
    UT_TBL_Status(RegRec0Ptr)->NextBufferId = UT_CFE_TBL_LOADBUFFID_INVH;
    LocalBuf                                = NULL;
    LocalSize                               = 0;
    UtAssert_BOOL_FALSE(
        CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, UT_REGREC_INDEX(RegRec0Ptr), &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);
    UtAssert_INT32_EQ(CFE_TBL_Global.RegDumpState.DumpRecord.LoadInProgress, -1);

    /* Hit last entry, no load in progress */
    UT_TBL_SetupRegRec(RegRecLastPtr);
    RegRecLastPtr->OwnerAppId                  = CFE_TBL_NOT_OWNED;
    RegRecLastPtr->AccessList.Next             = UT_CFE_TBL_HANDLE_2;
    RegRecLastPtr->AccessList.Prev             = UT_CFE_TBL_HANDLE_2;
    UT_TBL_Status(RegRecLastPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    AccDescPtr                                 = CFE_TBL_LocateAccDescByHandle(UT_CFE_TBL_HANDLE_2);
    CFE_TBL_HandleLinkInit(&AccDescPtr->Link);
    LocalBuf  = NULL;
    LocalSize = 0;
    UtAssert_BOOL_TRUE(CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1,
                                                  &LocalBuf, &LocalSize));
    UtAssert_NOT_NULL(LocalBuf);
    UtAssert_NONZERO(LocalSize);

    /* Test with record numb beyond EOF (should be ignored, return null) */
    UtAssert_BOOL_TRUE(CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, CFE_PLATFORM_TBL_MAX_NUM_TABLES + 1,
                                                  &LocalBuf, &LocalSize));
    UtAssert_NULL(LocalBuf);
    UtAssert_ZERO(LocalSize);

    /* Test empty registry */
    CFE_TBL_RegRecSetFree(RegRec0Ptr);
    LocalBuf  = NULL;
    LocalSize = 0;
    UtAssert_BOOL_FALSE(
        CFE_TBL_DumpRegistryGetter(&CFE_TBL_Global.RegDumpState, UT_REGREC_INDEX(RegRec0Ptr), &LocalBuf, &LocalSize));
    UtAssert_NULL(LocalBuf);
    UtAssert_ZERO(LocalSize);
}

/*
** Test the function that processes dump table to file command message
*/
void Test_CFE_TBL_DumpCmd(void)
{
    uint8             Buff;
    uint8 *           BuffPtr = &Buff;
    CFE_TBL_DumpCmd_t DumpCmd;
    CFE_ES_AppId_t    AppID;

    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_LoadBuff_t *   LoadBuffPtr;

    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);

    CFE_ES_GetAppID(&AppID);

    UtPrintf("Begin Test Dump Command");

    memset(&DumpCmd, 0, sizeof(DumpCmd));

    /* Test where the table cannot be found in the registry */
    UT_InitData();
    UT_TBL_SetName(DumpCmd.Payload.TableName, sizeof(DumpCmd.Payload.TableName), "notfound");
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_SUCH_TABLE_ERR_EID);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where the active buffer has data, but no dump control buffer is available
     */
    UT_InitData();
    UT_TBL_SetName(DumpCmd.Payload.TableName, sizeof(DumpCmd.Payload.TableName), CFE_TBL_RegRecGetName(RegRecPtr));
    DumpCmd.Payload.ActiveTableFlag = CFE_TBL_BufferSelect_ACTIVE;

    UT_TBL_Config(RegRecPtr)->DumpOnly = true;
    UT_TBL_SetActiveBufferAddr(RegRecPtr, 0, BuffPtr);

    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_TOO_MANY_DUMPS_ERR_EID);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with an active buffer, the pointer is created, validation passes,
     * the table is dump only, no dump is already in progress, and have a
     * working buffer; load in progress, single-buffered
     */
    UT_InitData();

    UT_TBL_SetName(UT_TBL_Config(RegRecPtr)->Name, sizeof(UT_TBL_Config(RegRecPtr)->Name), "DumpCmdTest");
    RegRecPtr->OwnerAppId = AppID;
    UT_TBL_SetName(DumpCmd.Payload.TableName, sizeof(DumpCmd.Payload.TableName), CFE_TBL_RegRecGetName(RegRecPtr));
    DumpCmd.Payload.ActiveTableFlag = CFE_TBL_BufferSelect_ACTIVE;
    UT_TBL_SetActiveBufferAddr(RegRecPtr, 0, BuffPtr);

    UT_TBL_ForEveryRegRec(UT_TBL_SetDumpOnly);

    UT_TBL_SetupPendingDump(2, NULL, NULL, NULL);
    UT_TBL_ResetDumpCtrlState(3);
    RegRecPtr->DumpControlId  = CFE_TBL_NO_DUMP_PENDING;
    RegRecPtr->Notify.Enabled = true;
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SB_INTERNAL_ERR);
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, no dump is already progress, and fails to get a working buffer;
     * no load in progress, TableLoadedOnce = true, single-buffered, no buffer
     * available
     */
    UT_InitData();
    UT_TBL_ResetDumpCtrlState(2);
    RegRecPtr->DumpControlId                 = CFE_TBL_NO_DUMP_PENDING;
    UT_TBL_Status(RegRecPtr)->NextBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Config(RegRecPtr)->DoubleBuffered = false;

    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNextEx), 1, -1);

    RegRecPtr->Notify.Enabled = true;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with an active buffer, a pointer is created, the table is dump
     * only, and no dump fails to find a free dump control block; too many
     * dump only table dumps have been requested
     */
    UT_InitData();
    RegRecPtr->DumpControlId  = CFE_TBL_NO_DUMP_PENDING;
    RegRecPtr->Notify.Enabled = true;
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with an inactive buffer, double-buffered, dump already in progress;
     * dump is already pending
     */
    UT_InitData();
    DumpCmd.Payload.ActiveTableFlag                 = CFE_TBL_BufferSelect_INACTIVE;
    UT_TBL_Config(RegRecPtr)->DoubleBuffered        = true;
    CFE_TBL_GetInactiveBuffer(RegRecPtr)->BufferPtr = BuffPtr;
    RegRecPtr->DumpControlId                        = CFE_TBL_DUMPCTRLID_C(CFE_ResourceId_FromInteger(1));
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with an inactive buffer, single-buffered, pointer created, is a
     * dump only table
     */
    UT_InitData();
    UT_TBL_Config(RegRecPtr)->DumpOnly = false;
    RegRecPtr->DumpControlId           = CFE_TBL_DUMPCTRLID_UNDEFINED;

    LoadBuffPtr            = UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    LoadBuffPtr->BufferPtr = BuffPtr;
    UT_TBL_SetName(DumpCmd.Payload.DumpFilename, sizeof(DumpCmd.Payload.DumpFilename),
                   UT_TBL_Status(RegRecPtr)->LastFileLoaded);
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test with an inactive buffer, single-buffered: No inactive buffer for
     * table due to load in progress
     */
    UT_InitData();
    RegRecPtr->DumpControlId = CFE_TBL_DUMPCTRLID_UNDEFINED;
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    UT_TBL_Config(RegRecPtr)->DoubleBuffered = false;
    UT_TBL_Status(RegRecPtr)->PrevBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Status(RegRecPtr)->NextBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with an inactive buffer, single-buffered: No inactive buffer for
     * table due to user defined address
     */
    UT_InitData();
    UT_TBL_Status(RegRecPtr)->NextBufferId = UT_CFE_TBL_LOADBUFFID_GLB_0;
    UT_TBL_Config(RegRecPtr)->UserDefAddr  = true;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with an illegal buffer parameter */
    UT_InitData();
    DumpCmd.Payload.ActiveTableFlag = CFE_TBL_BufferSelect_ACTIVE + 1;
    UtAssert_INT32_EQ(CFE_TBL_DumpCmd(&DumpCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
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
    UT_InitData();
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test response to inability to find the table in the registry */
    UT_InitData();
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "notfound");
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_SUCH_TABLE_ERR_EID);

    /* The rest of the tests will use registry 0, note empty name matches */
    RegRec0Ptr->OwnerAppId = AppID;

    /* Test attempt to load a dump only table */
    UT_InitData();
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_TBL_Config(RegRec0Ptr)->DumpOnly = true;
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    UT_TBL_Config(RegRec0Ptr)->DumpOnly = false;

    /* Test attempt to load a table with a load already pending */
    UT_InitData();
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_TBL_Status(RegRec0Ptr)->LoadPending = true;
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    UT_TBL_Status(RegRec0Ptr)->LoadPending = false;

    /* Test where the file isn't dump only and passes table checks, get a
     * working buffer, and there is an extra byte (more data than header
     * indicates)
     * Works because OS_read stub assumes success (returns dummy extra byte even though it isn't in the buffer)
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    UT_TBL_Config(RegRec0Ptr)->Size = sizeof(UT_Table1_t);

    LoadBuffPtr            = UT_TBL_SetupLoadBuff(RegRec0Ptr, false, 0);
    LoadBuffPtr->BufferPtr = &LoadBuffer;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with no extra byte => successful load */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandCounter);

    /* Test with differing amount of data from header's claim */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with no working buffers available */
    UT_InitData();
    UT_TBL_SetActiveBufferAddr(RegRec0Ptr, 0, LoadBuffPtr->BufferPtr);
    UT_TBL_Status(RegRec0Ptr)->NextBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Config(RegRec0Ptr)->DoubleBuffered = false;

    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNextEx), 1, -1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_WORK_BUFFERS_ERR_EID);

    /* Test with interal CFE_TBL_GetWorkingBuffer error (memcpy with matching address */
    UT_InitData();
    LoadBuffPtr = UT_TBL_SetupLoadBuff(RegRec0Ptr, false, 0);
    UT_TBL_SetActiveBufferAddr(RegRec0Ptr, 0, LoadBuffPtr->BufferPtr);
    UT_TBL_SetLoadBuffFree(LoadBuffPtr);
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), CFE_TBL_RegRecGetName(RegRec0Ptr));
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with table header indicating data beyond size of the table */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));
    UT_TBL_Config(RegRec0Ptr)->Size = sizeof(UT_Table1_t) - 1;
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test with table header indicating no data in the file */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where file has partial load for uninitialized table and offset
     * is non-zero
     */
    UT_InitData();
    /* reset bufferids back to initial state (an unloaded table) */
    UT_TBL_Status(RegRec0Ptr)->ActiveBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Status(RegRec0Ptr)->PrevBufferId   = CFE_TBL_LOADBUFFID_UNDEFINED;
    UT_TBL_Config(RegRec0Ptr)->Size           = sizeof(UT_Table1_t);
    UT_TBL_SetupHeader(&TblFileHeader, 1, 1);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PARTIAL_LOAD_ERR_EID);

    /* Test where file has partial load for uninitialized table and offset
     * is zero
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 0, 1);
    UT_TBL_Config(RegRec0Ptr)->Size = sizeof(UT_Table1_t);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test response to inability to read the file header */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where file has partial load for initialized table and offset
     * is non-zero
     */
    UT_InitData();
    UT_TBL_InitActiveBuffer(RegRec0Ptr, 0);
    UT_TBL_SetupHeader(&TblFileHeader, 1, 0);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);

    /* Test where file has partial load for initialized table and offset
     * is non-zero
     */
    UT_InitData();
    UT_TBL_SetupHeader(&TblFileHeader, 1, 1);
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_LoadCmd(&LoadCmd), CFE_SUCCESS);
    CFE_UtAssert_COUNTER_INCR(CFE_TBL_Global.CommandErrorCounter);
}

/*
** Test the function that processes housekeeping request message
*/
void Test_CFE_TBL_SendHkCmd(void)
{
    int                    i;
    CFE_TBL_LoadBuff_t     DumpBuff;
    CFE_TBL_LoadBuff_t *   DumpBuffPtr = &DumpBuff;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    uint8                  Buff;
    void *                 BuffPtr = &Buff;
    CFE_TBL_LoadBuffId_t   LoadInProg;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;

    UtPrintf("Begin Test Housekeeping Command");
    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);

    /* Test response to inability to update timestamp in dump file + inability
     * to send Hk packet
     */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    LoadInProg                             = UT_CFE_TBL_LOADBUFFID_GLB_0;
    UT_TBL_Status(RegRecPtr)->NextBufferId = LoadInProg;
    UT_TBL_SetLoadBuffTaken(DumpBuffPtr, RegRecPtr, CFE_RESOURCEID_UNWRAP(LoadInProg));
    DumpBuffPtr->Validated = true;
    DumpBuffPtr->BufferPtr = BuffPtr;
    DumpBuffPtr->FileTime  = CFE_TIME_ZERO_VALUE;
    UT_TBL_SetName(DumpBuffPtr->DataSource, sizeof(DumpBuffPtr->DataSource), "hkSource");
    DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;

    for (i = 1; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        UT_TBL_SetupPendingDump(i, NULL, NULL, NULL);
    }

    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, CFE_SUCCESS - 1);
    CFE_TBL_Global.HkTlmTblRegId = UT_CFE_TBL_REGID_0;
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_SUCCESS);

    for (i = 1; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        UT_TBL_SetupPendingDump(i, NULL, NULL, NULL);
    }

    UT_TBL_Status(RegRecPtr)->NextBufferId = LoadInProg;

    /* Test response to inability to open dump file */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    DumpCtrlPtr->State           = CFE_TBL_DUMP_PERFORMED;
    CFE_TBL_Global.HkTlmTblRegId = UT_CFE_TBL_REGID_0;
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_SUCCESS);

    /* Test response to an invalid table and a dump file create failure */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    CFE_TBL_Global.HkTlmTblRegId = CFE_TBL_NOT_FOUND;
    DumpCtrlPtr->State           = CFE_TBL_DUMP_PERFORMED;
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_SUCCESS);

    /* Test response to a file time stamp failure */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_SetTimestamp), 1, OS_SUCCESS - 1);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_SUCCESS);

    /* Test response to OS_OpenCreate failure */
    UT_InitData();
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 3, -1);
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_SUCCESS);

    /* Test when the table is not owned */
    UT_InitData();
    RegRecPtr->OwnerAppId        = CFE_TBL_NOT_OWNED;
    CFE_TBL_Global.HkTlmTblRegId = UT_CFE_TBL_REGID_INVH;
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_SUCCESS);

    CFE_TBL_Global.HkTlmTblRegId = UT_CFE_TBL_REGID_INVL;
    UtAssert_INT32_EQ(CFE_TBL_SendHkCmd(NULL), CFE_SUCCESS);
}

/*
** Test function that registers a table with cFE to obtain table management
** services
*/
void Test_CFE_TBL_Register(void)
{
    CFE_TBL_Handle_t            TblHandle1;
    CFE_TBL_Handle_t            TblHandle2;
    CFE_TBL_Handle_t            TblHandle3;
    char                        TblName[CFE_MISSION_TBL_MAX_NAME_LENGTH + 2];
    int16                       i;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    UtPrintf("Begin Test Register");

    /* Test response to a null table handle and null table name */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(NULL, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, NULL, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_BAD_ARGUMENT);

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a table name longer than the maximum allowed */
    UT_InitData();

    /* Create a table name longer than allowed */
    for (i = 0; i < CFE_MISSION_TBL_MAX_NAME_LENGTH + 1; i++)
    {
        TblName[i] = 'A';
    }

    TblName[i] = '\0';
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a table name shorter than the minimum allowed */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /*  Test response to a table size of zero */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", 0, CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a table size larger than the maximum allowed */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", (CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + 1),
                                       CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response at single size limit */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_Table1", CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE, CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to a double-buffered table size larger than the
     * maximum allowed
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", (CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1),
                                       CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_INVALID_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response at double size limit */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_Table1", CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE, CFE_TBL_OPT_DBL_BUFFER, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       ((CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK) | CFE_TBL_OPT_DBL_BUFFER),
                                       NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_LOAD_DUMP)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       (CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK), NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       ((CFE_TBL_OPT_USR_DEF_ADDR & ~CFE_TBL_OPT_LD_DMP_MSK) | CFE_TBL_OPT_CRITICAL),
                                       NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_DBL_BUFFER)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_DBL_BUFFER), NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a memory handle error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a memory block size error */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_ES_ERR_MEM_BLOCK_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to a memory block size error (for a second buffer) */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 2, CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_ES_ERR_MEM_BLOCK_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test successfully getting a double buffered table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to register table owned by another application */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle3, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_DUPLICATE_NOT_OWNED);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to register existing table with a different size */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle3, "UT_Table1", sizeof(UT_Table2_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_DUPLICATE_DIFF_SIZE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test cleanup of a shared table */
    /* This tests the condition that a registry entry should not be completely released
     * until all shared access descriptors are also unregistered */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&TblHandle3, "ut_cfe_tbl.UT_Table1"));
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1); /* Restore AppID to proper value */
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_WARN_DUPLICATE);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_BOOL_TRUE(CFE_TBL_HANDLE_EQ(TblHandle1, TblHandle2));

    /* c. Test cleanup: unregister tables */
    UT_ClearEventHistory();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_NOT_NULL(AccessDescPtr = UT_TBL_AccDescFromExtHandle(TblHandle1));
    UtAssert_NOT_NULL(RegRecPtr = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex));
    UtAssert_BOOL_TRUE(CFE_TBL_RegRecIsUsed(RegRecPtr));
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle2));
    /* This should NOT have freed the registry entry yet */
    UtAssert_BOOL_TRUE(CFE_TBL_RegRecIsUsed(RegRecPtr));
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle3));
    /* Now this should have freed the registry entry */
    UtAssert_BOOL_FALSE(CFE_TBL_RegRecIsUsed(RegRecPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a single buffered table */
    /* a. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a single buffered dump-only table */
    /* a. Perform test */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                          (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_DUMP_ONLY), NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a user defined address table */
    /* a. Perform test */
    UT_InitData();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_USR_DEF_ADDR, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table */
    /* a. Perform test */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table that already has an allocated CDS */
    /* a. Perform test */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    CFE_TBL_Global.CritReg[0].TableLoadedOnce = true;
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_INFO_RECOVERED_TBL);
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table that already has an allocated CDS
     * and recovery fails
     */
    /* a. Perform test */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    CFE_TBL_Global.CritReg[0].TableLoadedOnce = false;
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table that already has an allocated CDS but
     * fails recovery
     */
    /* a. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table that already has an allocated CDS but
     * no critical table registry entry
     */
    /* a. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);

    /* Remove all entries from critical table registry */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    }

    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table but no critical table registry entry
     * is free
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_SUCCESS);

    /* Remove all entries from critical table registry */
    /* a. Perform test */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    }

    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to register a critical table when the CDS registry
     * is full
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_NO_RESOURCE_IDS_AVAILABLE);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_WARN_NOT_CRITICAL);
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to register a table when the registry is full */
    /* a. Test setup */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);

    /* b. Perform test */
    UT_ClearEventHistory();
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", i + 1);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_REGISTRY_FULL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* c. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test registering a critical table with no space in the critical table registry */
    /* a. Setup test */
    UT_InitData();
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDSHANDLE_C(CFE_ResourceId_FromInteger(i + 1));
    }
    /* b. Perform test */
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&TblHandle1, "NOTABLE", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Test cleanup: unregister table */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to no available handles */
    /* a. Test setup */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 2, -1);

    /* b. Perform test */
    UT_ClearEventHistory();
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, TblName, sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_TBL_ERR_HANDLES_FULL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       CFE_TBL_OPT_USR_DEF_ADDR | CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to an invalid table option combination
     * (CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_CRITICAL)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                       CFE_TBL_OPT_DUMP_ONLY | CFE_TBL_OPT_CRITICAL, NULL),
                      CFE_TBL_ERR_INVALID_OPTIONS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to register a duplicate table */
    UT_InitData();
    CFE_UtAssert_SETUP(CFE_TBL_Register(&TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL));
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_WARN_DUPLICATE);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_BOOL_TRUE(CFE_TBL_HANDLE_EQ(TblHandle1, TblHandle2));

    /* Test attempt to register a table with an invalid registry index */
    UT_InitData();
    UtAssert_NOT_NULL(AccessDescPtr = UT_TBL_AccDescFromExtHandle(TblHandle1));
    UtAssert_NOT_NULL(RegRecPtr = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex));
    CFE_TBL_AccDescSetFree(AccessDescPtr);
    AccessDescPtr->RegIndex = UT_CFE_TBL_REGID_INVL;
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_TBL_ERR_UNREGISTERED);
    CFE_UtAssert_EVENTSENT(CFE_TBL_REGISTER_ERR_EID);
    UtAssert_BOOL_FALSE(CFE_TBL_HANDLE_IS_VALID(TblHandle2));

    /* Test attempt to register a double buffered table with a pool buffer
     * error */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_SEVERITY_ERROR);
    snprintf(TblName, CFE_MISSION_TBL_MAX_NAME_LENGTH, "UT_Table%d", CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    AccessDescPtr = CFE_TBL_LocateAccDescByHandle(UT_CFE_TBL_HANDLE_0);
    CFE_TBL_AccDescSetFree(AccessDescPtr);
    UtAssert_INT32_EQ(CFE_TBL_Register(&TblHandle2, TblName, sizeof(UT_Table1_t) + 1, CFE_TBL_OPT_DBL_BUFFER, NULL),
                      CFE_SEVERITY_ERROR);
}

/*
** Test function that obtains the handle of table registered by another
** application
*/
void Test_CFE_TBL_Share(void)
{
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr2;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;

    UtPrintf("Begin Test Share");

    /* Test response to a null table handle and null table name */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Share(NULL, "ut_cfe_tbl.UT_Table2"), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Share(&App2TblHandle1, NULL), CFE_TBL_BAD_ARGUMENT);

    /* Test response to an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table2"), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SHARE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response when table name is not in the registry */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Share(&App1TblHandle1, "ut_cfe_tbl.NOT_Table2"), CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SHARE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response when there are no available table handles */
    UT_InitData();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT); /* so an actual table exists to share */
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_Share(&App1TblHandle1, CFE_TBL_RegRecGetName(RegRecPtr)), CFE_TBL_ERR_HANDLES_FULL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SHARE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test unregistering tables where unregister is called by different app (non-owner) */
    /* This is allowed and it does de-register the handle but not the table registry entry */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_10);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(UT_TBL_AccDescToExtHandle(AccDescPtr)));
    UtAssert_BOOL_TRUE(CFE_TBL_RegRecIsUsed(RegRecPtr));
    UtAssert_BOOL_FALSE(CFE_TBL_AccDescIsUsed(AccDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test unregister response to a PutPoolBuf error */
    /* This should still release everything else */
    UT_InitData();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(UT_TBL_AccDescToExtHandle(AccDescPtr)));
    UtAssert_BOOL_FALSE(CFE_TBL_RegRecIsUsed(RegRecPtr));
    UtAssert_BOOL_FALSE(CFE_TBL_AccDescIsUsed(AccDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful first load of a table */
    UT_InitData();
    UT_ResetTableRegistry();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT);
    UT_TBL_SetName(StdFileHeader.Description, sizeof(StdFileHeader.Description), "FS header description");
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), CFE_TBL_RegRecGetName(RegRecPtr));

    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(UT_TBL_AccDescToExtHandle(AccDescPtr), CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test successful share of a table that has not been loaded once */
    UT_InitData();
    UT_TBL_SetupSingleReg(&RegRecPtr2, NULL, CFE_TBL_OPT_DEFAULT);
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, CFE_TBL_RegRecGetName(RegRecPtr2)));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful share of a table that has been loaded once */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle2, CFE_TBL_RegRecGetName(RegRecPtr)));
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Function to test unregistering a previously registered table and freeing
** associated resources
*/
void Test_CFE_TBL_Unregister(void)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;

    UtPrintf("Begin Test Unregister");

    UT_SetAppID(UT_TBL_APPID_1);

    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);
    CFE_UtAssert_SETUP(CFE_TBL_Share(&App1TblHandle1, CFE_TBL_RegRecGetName(RegRecPtr)));
    CFE_UtAssert_SETUP(CFE_TBL_Share(&App2TblHandle2, CFE_TBL_RegRecGetName(RegRecPtr)));

    /* Test response to unregistering a table with an invalid handle */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UNREGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Unregister a shared table to make it unowned */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to unregistering an unowned table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(App2TblHandle2), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UNREGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
}

/*
** Test function that instructs table services to notify calling application
** whenever the specified table requires management
*/
void Test_CFE_TBL_NotifyByMessage(void)
{
    UtPrintf("Begin Test Notify by Message");

    /* Set up notify by message tests */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetPoolBufferIndex();

    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App1TblHandle1, "NBMsg_Tbl", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful notification */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to notification request when the application doesn't
     * own the table handle
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_10);
    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to notification request when the application ID is bad */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_NotifyByMessage(App1TblHandle1, CFE_SB_ValueToMsgId(1), 1, 1),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Function to test loading a specified table with data from the
** specified source
*/
void Test_CFE_TBL_Load(void)
{
    CFE_TBL_Handle_t            DumpOnlyTblHandle;
    CFE_TBL_Handle_t            UserDefTblHandle;
    UT_Table1_t                 TestTable1;
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    void *                      App2TblPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    uint32                      IdValue;
    CFE_Config_ArrayValue_t     UTAV = {1, &IdValue};

    memset(&TestTable1, 0, sizeof(TestTable1));

    UtPrintf("Begin Test Load");

    /* Set up for table load test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetTableRegistry();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to a null source data pointer */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, NULL), CFE_TBL_BAD_ARGUMENT);

    /* Test response to a table with no access */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_HANDLE_ACCESS_ERR_EID);
    UT_SetAppID(UT_TBL_APPID_1);

    /* Test attempt to perform partial INITIAL load */
    UT_InitData();
    UT_TBL_SetName(StdFileHeader.Description, sizeof(StdFileHeader.Description), "Test description");
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table1");
    UT_TBL_SetupHeader(&TblFileHeader, 1, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_PARTIAL_LOAD);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PARTIAL_LOAD_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to perform partial INITIAL load with table already
     * loaded
     */
    UT_InitData();
    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    AccessDescPtr                            = UT_TBL_AccDescFromExtHandle(App1TblHandle1);
    RegRecPtr                                = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_Config(RegRecPtr)->UserDefAddr    = true;
    UT_TBL_Status(RegRecPtr)->ActiveBufferId = UT_CFE_TBL_LOADBUFFID_REG_0_0;
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_PARTIAL_LOAD_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a file that has incompatible data for the
     * specified table
     */
    UT_InitData();
    UT_TBL_SetName(StdFileHeader.Description, sizeof(StdFileHeader.Description), "Test description");
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.NotUT_Table1");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a file where the std header cannot be read */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_NO_STD_HEADER);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_STD_HDR_ERR_EID);

    /* Test attempt to load a file where the tbl header cannot be read */
    UT_InitData();
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_NO_TBL_HEADER);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_TBL_HDR_ERR_EID);

    /* Test attempt to load a file where the file is the wrong type */
    UT_InitData();
    StdFileHeader.ContentType = ~CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_BAD_CONTENT_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_TYPE_ERR_EID);

    /* Test attempt to load a file where the file is the wrong subtype */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = ~CFE_FS_SubType_TBL_IMG;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_BAD_SUBTYPE_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_FILE_SUBTYPE_ERR_EID);

    /* Test attempt to load a file where the processor id is wrong */
    UT_InitData();
    IdValue = 0x123;
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    StdFileHeader.ContentType  = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType      = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.ProcessorID  = ~IdValue;
    StdFileHeader.SpacecraftID = IdValue;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_BAD_PROCESSOR_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_PROCESSOR_ID_ERR_EID);

    /* Test attempt to load a file where the processor id is wrong */
    UT_InitData();
    IdValue = 0x123;
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    StdFileHeader.ContentType  = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType      = CFE_FS_SubType_TBL_IMG;
    StdFileHeader.ProcessorID  = IdValue;
    StdFileHeader.SpacecraftID = ~IdValue;
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_BAD_SPACECRAFT_ID);
    CFE_UtAssert_EVENTSENT(CFE_TBL_SPACECRAFT_ID_ERR_EID);

    /* Set up for double buffer table load test */
    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table2x", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to load a file that has incompatible data for the
     * specified double buffered table that is already loaded
     */
    UT_InitData();
    UT_TBL_SetName(StdFileHeader.Description, sizeof(StdFileHeader.Description), "Test description");
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.NotUT_Table1");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    AccessDescPtr                            = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr                                = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_Config(RegRecPtr)->DoubleBuffered = true;
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test performing a Load from memory */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load from an illegal source type (not a file or
     * from memory)
     */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, (CFE_TBL_SrcEnum_t)99, &TestTable1), CFE_TBL_ERR_ILLEGAL_SRC_TYPE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test failure of validation function on table load using a negative
     * return code
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1234);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), -1234);
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test failure of validation function on table load using a positive
     * return code
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_STATUS_VALIDATION_FAILURE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_VAL_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(2);

    /* Test attempt to load a table with a bad handle */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_TBL_BAD_TABLE_HANDLE, CFE_TBL_SRC_ADDRESS, &TestTable1),
                      CFE_TBL_ERR_INVALID_HANDLE);

    /* Test attempt to load a dump-only table */
    /* a. Test setup */
    UT_InitData();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&DumpOnlyTblHandle, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DUMP_ONLY, NULL));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(DumpOnlyTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a dump-only table with the table already loaded */
    UT_InitData();
    AccessDescPtr                         = UT_TBL_AccDescFromExtHandle(DumpOnlyTblHandle);
    RegRecPtr                             = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_Config(RegRecPtr)->UserDefAddr = true;
    UT_TBL_SetActiveBufferAddr(RegRecPtr, 0, &TestTable1); /* sets "table loaded once" */
    UtAssert_INT32_EQ(CFE_TBL_Load(DumpOnlyTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test specifying a table address for a user defined table */
    /* a. Test setup */
    UT_InitData();
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&UserDefTblHandle, "UT_Table3", sizeof(UT_Table1_t), CFE_TBL_OPT_USR_DEF_ADDR, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Perform test - loading from file is not allowed */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(UserDefTblHandle, CFE_TBL_SRC_FILE, &TestTable1), CFE_TBL_ERR_ILLEGAL_SRC_TYPE);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Perform test - success */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(UserDefTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* loading a second time is not valid */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_Load(UserDefTblHandle, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_DUMP_ONLY);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test attempt to load a locked shared table */
    /* a. Test setup part 1 */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1"));
    CFE_UtAssert_EVENTCOUNT(0);

    /* a. Test setup part 2 */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App2TblPtr, App2TblHandle1), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_INFO_TABLE_LOCKED);
    CFE_UtAssert_EVENTCOUNT(1);

    /* d. Test cleanup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App2TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);
}

void Test_CFE_TBL_TableDumpCommon(void)
{
    char                      Filename[OS_MAX_PATH_LEN];
    osal_id_t                 FileDescriptor;
    CFE_TBL_DumpCtrlId_t      DumpCtrlId;
    CFE_TBL_DumpControl_t *   DumpCtrlPtr;
    CFE_TBL_RegistryRec_t *   RegRecPtr;
    CFE_TBL_LoadBuff_t *      DumpBuffPtr;
    CFE_TBL_TxnState_t        Txn;
    CFE_TBL_CombinedFileHdr_t FileHeader;

    memset(&Txn, 0, sizeof(Txn));
    strncpy(Filename, "ut", sizeof(Filename));

    UtPrintf("Begin Test Table Dump Common Impl");

    /* Test cases focusing on the following APIs:
     * CFE_Status_t CFE_TBL_AllocateDumpCtrlBlock(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t *DumpCtrlIdOut,
     *                                    CFE_TBL_BufferSelect_Enum_t BufferSelect);
     */

    /* Test when the transaction object is not initialized */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_AllocateDumpCtrlBlock(&Txn, &DumpCtrlId, CFE_TBL_BufferSelect_ACTIVE),
                      CFE_TBL_ERR_INVALID_HANDLE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_NO_SUCH_TABLE_ERR_EID);
    UtAssert_BOOL_FALSE(CFE_TBL_DUMPCTRLID_IS_VALID(DumpCtrlId));

    /* Now set up the transaction to point to a valid registry entry for the rest of tests */
    CFE_UtAssert_SETUP(CFE_TBL_Register(&App1TblHandle1, "load", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                        Test_CFE_TBL_ValidationFunc));
    UT_TBL_SetupTxnFromExtHandle(&Txn, App1TblHandle1, CFE_TBL_TxnContext_UNDEFINED);
    RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

    /* Test when the table is not loaded (no active or inactive buffer) */
    UtAssert_INT32_EQ(CFE_TBL_AllocateDumpCtrlBlock(&Txn, &DumpCtrlId, CFE_TBL_BufferSelect_ACTIVE),
                      CFE_TBL_BAD_ARGUMENT);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID);
    UtAssert_BOOL_FALSE(CFE_TBL_DUMPCTRLID_IS_VALID(DumpCtrlId));
    UtAssert_INT32_EQ(CFE_TBL_AllocateDumpCtrlBlock(&Txn, &DumpCtrlId, CFE_TBL_BufferSelect_INACTIVE),
                      CFE_TBL_BAD_ARGUMENT);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID);
    UtAssert_BOOL_FALSE(CFE_TBL_DUMPCTRLID_IS_VALID(DumpCtrlId));

    /* Dump already pending */
    UT_TBL_SetupPendingDump(0, NULL, RegRecPtr, &DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_AllocateDumpCtrlBlock(&Txn, &DumpCtrlId, CFE_TBL_BufferSelect_ACTIVE),
                      CFE_TBL_INFO_DUMP_PENDING);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_DUMP_PENDING_ERR_EID);
    UtAssert_BOOL_FALSE(CFE_TBL_DUMPCTRLID_IS_VALID(DumpCtrlId));
    RegRecPtr->DumpControlId = CFE_TBL_DUMPCTRLID_UNDEFINED;

    /* No available dump control blocks */
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNext), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_AllocateDumpCtrlBlock(&Txn, &DumpCtrlId, CFE_TBL_BufferSelect_ACTIVE),
                      CFE_TBL_ERR_NO_BUFFER_AVAIL);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_TOO_MANY_DUMPS_ERR_EID);
    UtAssert_BOOL_FALSE(CFE_TBL_DUMPCTRLID_IS_VALID(DumpCtrlId));
    UT_ResetState(UT_KEY(CFE_ResourceId_FindNext));

    /* Success / Nominal */
    UtAssert_INT32_EQ(CFE_TBL_AllocateDumpCtrlBlock(&Txn, &DumpCtrlId, CFE_TBL_BufferSelect_ACTIVE), CFE_SUCCESS);
    UtAssert_BOOL_TRUE(CFE_TBL_DUMPCTRLID_IS_VALID(DumpCtrlId));
    DumpCtrlPtr = CFE_TBL_LocateDumpCtrlByID(DumpCtrlId);
    UtAssert_BOOL_TRUE(CFE_TBL_DumpCtrlBlockIsMatch(DumpCtrlPtr, DumpCtrlId));

    /* Test cases focusing on the following APIs:
     * CFE_Status_t CFE_TBL_PrepareDumpSnapshotBuffer(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t DumpCtrlId,
     *                                          const char *DumpFilename);
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNextEx), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_PrepareDumpSnapshotBuffer(&Txn, DumpCtrlId, Filename), CFE_TBL_ERR_NO_BUFFER_AVAIL);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_NO_WORK_BUFFERS_ERR_EID);
    UT_ResetState(UT_KEY(CFE_ResourceId_FindNextEx));

    /* Forcibly drop the dump ctrl buffer to make a stale ID situation */
    DumpCtrlPtr = CFE_TBL_LocateDumpCtrlByID(DumpCtrlId);
    CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_PrepareDumpSnapshotBuffer(&Txn, DumpCtrlId, Filename), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);

    /* Nominal success case, dump only table - should leave it in the pending state */
    UT_TBL_Config(RegRecPtr)->DumpOnly = true;
    UT_TBL_SetupPendingDump(1, NULL, RegRecPtr, &DumpCtrlPtr);
    RegRecPtr->DumpControlId = CFE_TBL_DumpCtrlBlockGetId(DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_PrepareDumpSnapshotBuffer(&Txn, DumpCtrlId, Filename), CFE_SUCCESS);
    UtAssert_UINT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_PENDING);

    /* Nominal success case, normal table, should actually do the dump */
    UT_TBL_Config(RegRecPtr)->DumpOnly = false;
    UT_TBL_SetupPendingDump(0, NULL, RegRecPtr, &DumpCtrlPtr);
    DumpCtrlId               = CFE_TBL_DumpCtrlBlockGetId(DumpCtrlPtr);
    RegRecPtr->DumpControlId = DumpCtrlId;
    UtAssert_INT32_EQ(CFE_TBL_PrepareDumpSnapshotBuffer(&Txn, DumpCtrlId, Filename), CFE_SUCCESS);
    UtAssert_UINT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_PERFORMED);

    /* Test cases focusing on the following APIs:
     * CFE_Status_t CFE_TBL_ExecuteDumpSnapshot(CFE_TBL_DumpControl_t *DumpCtrlPtr);
     */

    /* Normal/Successful copy to buffer */
    DumpBuffPtr = CFE_TBL_AcquireGlobalLoadBuff(CFE_TBL_RegRecGetID(RegRecPtr));
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_ExecuteDumpSnapshot(DumpCtrlPtr), CFE_SUCCESS);
    UtAssert_UINT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_PERFORMED);

    /* Make it so the source buffer ID does not match (e.g. if the table was reloaded in the meantime) */
    UT_TBL_SetupPendingDump(1, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    CFE_TBL_LoadBuffSetFree(CFE_TBL_LocateLoadBufferByID(DumpCtrlPtr->SourceBuffId));
    UtAssert_INT32_EQ(CFE_TBL_ExecuteDumpSnapshot(DumpCtrlPtr), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_UINT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_PENDING); /* unchanged */
    CFE_TBL_LoadBuffSetFree(DumpBuffPtr);

    /* Test cases focusing on the following APIs:
     * int32 CFE_TBL_WriteHeaders(osal_id_t FileDescriptor, const CFE_TBL_File_Hdr_t *TblFileHeaderPtr);
     */
    memset(&FileHeader, 0xCC, sizeof(FileHeader));
    OS_OpenCreate(&FileDescriptor, Filename, 0, OS_WRITE_ONLY);

    /* Failure of CFE_FS_WriteHeader() */
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_WriteHeader), -1);
    UtAssert_INT32_EQ(CFE_TBL_WriteHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_ACCESS);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_WRITE_CFE_HDR_ERR_EID);
    UT_ResetState(UT_KEY(CFE_FS_WriteHeader));

    /* Failure of OS_write() */
    UT_SetDefaultReturnValue(UT_KEY(OS_write), -1);
    UtAssert_INT32_EQ(CFE_TBL_WriteHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_ACCESS);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_WRITE_TBL_HDR_ERR_EID);
    UT_ResetState(UT_KEY(OS_write));

    /* Nominal/success */
    UtAssert_INT32_EQ(CFE_TBL_WriteHeaders(&Txn, FileDescriptor, &FileHeader), CFE_SUCCESS);

    /* Test cases focusing on the following APIs:
     * CFE_Status_t CFE_TBL_WriteSnapshotToFile(const CFE_TBL_DumpControl_t *DumpCtlPtr);
     */

    DumpBuffPtr = CFE_TBL_AcquireGlobalLoadBuff(CFE_TBL_RegRecGetID(RegRecPtr));

    /* Nominal, overwriting a file */
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtrlPtr), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_OVERWRITE_DUMP_INF_EID);

    /* Nominal, creating a new file */
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, -1);
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtrlPtr), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_DUMP_INF_EID);
    UT_ResetState(UT_KEY(OS_OpenCreate));

    /* Unable to open output */
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), -1);
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtrlPtr), CFE_TBL_ERR_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_CREATING_DUMP_FILE_ERR_EID);
    UT_ResetState(UT_KEY(OS_OpenCreate));

    /* Fail to write header (via CFE_TBL_WriteHeaders, events already checked) */
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtrlPtr), CFE_TBL_ERR_ACCESS);
    UT_ResetState(UT_KEY(CFE_FS_WriteHeader));

    /* Fail to write data (headers are first OS_write call, data is second here) */
    UT_SetDeferredRetcode(UT_KEY(OS_write), 2, -1);
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    UtAssert_INT32_EQ(CFE_TBL_WriteSnapshotToFile(DumpCtrlPtr), CFE_TBL_ERR_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_WRITE_TBL_IMG_ERR_EID);
    UT_ResetState(UT_KEY(OS_write));

    /* Test cases focusing on the following APIs:
     * bool CFE_TBL_SendDumpEventHelper(const CFE_TBL_TxnEvent_t *Event, void *Arg);
     * void CFE_TBL_SendTableDumpEvents(CFE_TBL_TxnState_t *Txn);
     *
     * Note - only checking the off-nominal cases here.  The real event ID cases should be
     * covered through other test cases (this will also make it evident if there is an unreachable
     * or unused case in the switch statement).
     */
    UT_ResetState(UT_KEY(CFE_EVS_SendEventWithAppID));
    memset(&Txn, 0, sizeof(Txn));
    CFE_TBL_TxnAddEvent(&Txn, -1, -1, -1);
    CFE_TBL_SendTableDumpEvents(&Txn, NULL);
    UtAssert_STUB_COUNT(CFE_EVS_SendEventWithAppID, 0); /* nothing should have been sent */
}

void Test_CFE_TBL_TableLoadCommon(void)
{
    char                      Filename[OS_MAX_PATH_LEN];
    CFE_TBL_CombinedFileHdr_t FileHeader;
    CFE_TBL_RegistryRec_t *   RegRecPtr;
    CFE_TBL_TxnState_t        Txn;
    osal_id_t                 FileDescriptor;
    uint32                    IdValue;
    CFE_Config_ArrayValue_t   UTAV = {1, &IdValue};

    IdValue = 0x123;

    FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    memset(&Txn, 0, sizeof(Txn));
    memset(&FileHeader, 0, sizeof(FileHeader));
    strncpy(Filename, "ut", sizeof(Filename));

    UT_TBL_SetName(FileHeader.Std.Description, sizeof(FileHeader.Std.Description), "FS header description");
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;

    UtPrintf("Begin Test Table Load Common Impl");

    /* Test when the transaction object is not initialized */
    UT_InitData();
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t));
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_INVALID_HANDLE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_NO_SUCH_TABLE_ERR_EID);

    /* Now set up the transaction to point to a valid registry entry for the rest of tests */
    CFE_UtAssert_SETUP(CFE_TBL_Register(&App1TblHandle1, "load", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                        Test_CFE_TBL_ValidationFunc));
    UT_TBL_SetupTxnFromExtHandle(&Txn, App1TblHandle1, CFE_TBL_TxnContext_UNDEFINED);
    RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

    /* Test CFE_TBL_ReadHeaders response to a failure reading the standard cFE
     * file header
     */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), "ut_cfe_tbl.UT_Table1");
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_TBL_SetName(Filename, sizeof(Filename), "MyTestInputFilename");
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_NO_STD_HEADER);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to a bad magic number in cFE
     * standard header
     */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID - 1;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), "ut_cfe_tbl.UT_Table1");
    FileHeader.Tbl.NumBytes = sizeof(UT_Table1_t) - 1;
    FileHeader.Tbl.Offset   = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&FileHeader.Tbl.NumBytes);
        CFE_TBL_ByteSwapUint32(&FileHeader.Tbl.Offset);
    }

    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_CONTENT_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_TYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to a wrong cFE file subtype */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG - 1;
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), "ut_cfe_tbl.UT_Table1");
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_SUBTYPE_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_SUBTYPE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to a failure reading the cFE
     * table header
     */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    FileHeader.Std.ContentType = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), "ut_cfe_tbl.UT_Table1");
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1);

    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, sizeof(CFE_TBL_File_Hdr_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_NO_TBL_HEADER);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_TBL_HDR_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to an invalid spacecraft ID */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    FileHeader.Std.ContentType  = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType      = CFE_FS_SubType_TBL_IMG;
    FileHeader.Std.SpacecraftID = ~IdValue;
    FileHeader.Std.ProcessorID  = IdValue;
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), "ut_cfe_tbl.UT_Table1");
    FileHeader.Tbl.NumBytes = sizeof(UT_Table1_t) - 1;
    FileHeader.Tbl.Offset   = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&FileHeader.Tbl.NumBytes);
        CFE_TBL_ByteSwapUint32(&FileHeader.Tbl.Offset);
    }

    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_TBL_SetName(Filename, sizeof(Filename), "MyTestInputFilename");
    Filename[sizeof(Filename) - 1] = '\0';
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_SPACECRAFT_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_SPACECRAFT_ID_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_ReadHeaders response to an invalid processor ID */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, &UTAV);
    FileHeader.Std.ContentType  = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.SubType      = CFE_FS_SubType_TBL_IMG;
    FileHeader.Std.SpacecraftID = IdValue;
    FileHeader.Std.ProcessorID  = ~IdValue;
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), "ut_cfe_tbl.UT_Table1");
    FileHeader.Tbl.NumBytes = sizeof(UT_Table1_t) - 1;
    FileHeader.Tbl.Offset   = 0;

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        CFE_TBL_ByteSwapUint32(&FileHeader.Tbl.NumBytes);
        CFE_TBL_ByteSwapUint32(&FileHeader.Tbl.Offset);
    }

    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_TBL_SetName(Filename, sizeof(Filename), "MyTestInputFilename");
    UtAssert_INT32_EQ(CFE_TBL_ReadHeaders(&Txn, FileDescriptor, &FileHeader), CFE_TBL_ERR_BAD_PROCESSOR_ID);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_PROCESSOR_ID_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_TxnLoadFromFile response to the file being for the
     * wrong table
     */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), "mismatch");
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t));
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile response to a file that's content is too large
     * (according to the header)
     */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetName(FileHeader.Tbl.TableName, sizeof(FileHeader.Tbl.TableName), CFE_TBL_RegRecGetName(RegRecPtr));
    UT_TBL_SetupHeader(&FileHeader.Tbl, 1, sizeof(UT_Table1_t));
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_FILE_TOO_LARGE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile response to a file name that is too long */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_FS_ERR_PATH_TOO_LONG);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_FILENAME_TOO_LONG);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_LOAD_FILENAME_LONG_ERR_EID);

    /* Test CFE_TBL_SetMetaDataFromFileHeader where no load is in progress */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    CFE_TBL_RegRecClearLoadInProgress(RegRecPtr);
    UtAssert_VOIDCALL(CFE_TBL_SetMetaDataFromFileHeader(&Txn, Filename, &FileHeader.Std));

    /* For the remainder of tests there needs to be a load buffer */
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);

    /* Test CFE_TBL_TxnLoadFromFile response to the file content being
     * incomplete
     */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t));
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, sizeof(UT_Table1_t) - 1);
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_LOAD_INCOMPLETE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_INCOMPLETE_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile with failure of data OS_read */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t));
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 2, -1);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_LOAD_INCOMPLETE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_INCOMPLETE_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile with failure of extra byte OS_read */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t));
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_TxnLoadFromFile(&Txn, Filename));

    /* Test CFE_TBL_TxnLoadFromFile response to an OS open error */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t));
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_ACCESS);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_ACCESS_ERR_EID);

    /* Test CFE_TBL_TxnLoadFromFile response to a file too short warning */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_TBL_SetupHeader(&FileHeader.Tbl, 0, sizeof(UT_Table1_t) - 1);
    UT_SetReadBuffer(&FileHeader.Tbl, sizeof(FileHeader.Tbl));
    UT_SetReadHeader(&FileHeader.Std, sizeof(FileHeader.Std));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_WARN_SHORT_FILE);

    /* Test CFE_TBL_TxnLoadFromFile response to an invalid header length */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ReadHeader), 1, sizeof(CFE_FS_Header_t) - 1);
    UtAssert_INT32_EQ(CFE_TBL_TxnLoadFromFile(&Txn, Filename), CFE_TBL_ERR_NO_STD_HEADER);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_STD_HDR_ERR_EID);

    /* Test CFE_TBL_LoadContentFromFile response to a file that would overrun the buffer */
    UT_InitData();
    CFE_TBL_TxnClearEvents(&Txn);
    UtAssert_INT32_EQ(CFE_TBL_LoadContentFromFile(&Txn, FileDescriptor, CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE,
                                                  CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE),
                      CFE_TBL_ERR_FILE_TOO_LARGE);
    UT_TBL_EVENT_PENDING(&Txn, CFE_TBL_FILE_TOO_BIG_ERR_EID);

    /* Test CFE_TBL_ValidateLoadInProgress response where there is no load in progress */
    UT_InitData();
    CFE_TBL_RegRecClearLoadInProgress(RegRecPtr);
    UtAssert_INT32_EQ(CFE_TBL_ValidateLoadInProgress(&Txn, CFE_SUCCESS), CFE_TBL_ERR_NEVER_LOADED);

    /* Test CFE_TBL_CompleteInitialLoad response where there is no load in progress */
    UtAssert_INT32_EQ(CFE_TBL_CompleteInitialLoad(&Txn), CFE_TBL_ERR_NEVER_LOADED);
}
/*
** Function to test obtaining the current address of the contents
** of the specified table
*/
void Test_CFE_TBL_GetAddress(void)
{
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    void *                      App3TblPtr;
    void *                      App2TblPtr;

    UtPrintf("Begin Test Get Address");

    /* Set up App1TblHandle1 as the owner handle */
    UT_SetAppID(UT_TBL_APPID_1);
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccDescPtr);

    /* Set up App2TblHandle1 as a shared handle */
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SETUP(CFE_TBL_Share(&App2TblHandle1, CFE_TBL_RegRecGetName(RegRecPtr)));

    /* Test response to a null table pointer */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(NULL, App1TblHandle1), CFE_TBL_BAD_ARGUMENT);

    /* Test attempt to get the address of a table for which the application
     * does not have access
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_3);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App3TblPtr, App2TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to get the address with an invalid application ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App3TblPtr, App2TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to get the address with an invalid handle */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App3TblPtr, CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Attempt to get the address of an unregistered (unowned) table */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App2TblPtr, App2TblHandle1), CFE_TBL_ERR_UNREGISTERED);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Function to test release of a previously obtained pointer to the
** contents of the specified table
*/
void Test_CFE_TBL_ReleaseAddress(void)
{
    UtPrintf("Begin Test Release Address");

    /* Test address release using an invalid application ID */
    /* a. Test setup - register single buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetTableRegistry();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(App1TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that obtains the current addresses of the contents of a
** collection of tables
*/
void Test_CFE_TBL_GetAddresses(void)
{
    UtPrintf("Begin Test Get Addresses");

    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_InitData();

    /* Test getting the addresses of two tables that have not been loaded */
    ArrayOfHandles[0]       = App1TblHandle1;
    ArrayOfHandles[1]       = App1TblHandle2;
    ArrayOfPtrsToTblPtrs[0] = &Tbl1Ptr;
    ArrayOfPtrsToTblPtrs[1] = &Tbl2Ptr;

    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles), CFE_TBL_ERR_NEVER_LOADED);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_NULL(Tbl1Ptr);
    UtAssert_NULL(Tbl2Ptr);

    /* Test getting the addresses of zero tables */
    CFE_UtAssert_SUCCESS(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 0, ArrayOfHandles));

    /* Test response to a null table pointer and null table handle */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(NULL, 2, ArrayOfHandles), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, NULL), CFE_TBL_BAD_ARGUMENT);

    /* Test attempt to get addresses of tables that the application is not
     * allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test attempt to get addresses of tables with a bad app ID */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 2, ArrayOfHandles), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that releases previously obtained pointers to the contents
** of the specified tables
*/
void Test_CFE_TBL_ReleaseAddresses(void)
{
    CFE_FS_Header_t             FileHeader;
    UT_TempFile_t               File;
    CFE_TBL_Handle_t            TblHandle1;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;

    UtPrintf("Begin Test Release Addresses");

    /* Set up App1TblHandle1 as the owner handle */
    UT_SetAppID(UT_TBL_APPID_1);
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT);
    ArrayOfHandles[0] = UT_TBL_AccDescToExtHandle(AccDescPtr);
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT);
    ArrayOfHandles[1] = UT_TBL_AccDescToExtHandle(AccDescPtr);

    /* Test response to a null table handle pointer */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddresses(2, NULL), CFE_TBL_BAD_ARGUMENT);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to releasing two tables that have not been loaded */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddresses(2, ArrayOfHandles), CFE_TBL_ERR_NEVER_LOADED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test releasing 0 then 1 addresses */
    UT_InitData();

    memset(&File, 0, sizeof(File));

    /* a. Configure for successful file read to initialize table */
    UT_TBL_SetName(FileHeader.Description, sizeof(FileHeader.Description), "FS header description");
    FileHeader.ContentType    = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType        = CFE_FS_SubType_TBL_IMG;
    FileHeader.TimeSeconds    = 1704;
    FileHeader.TimeSubSeconds = 104;
    UT_TBL_SetName(File.TblHeader.TableName, sizeof(File.TblHeader.TableName), "ut_cfe_tbl.UT_RELEASE_TEST");
    UT_TBL_SetupHeader(&File.TblHeader, 0, sizeof(UT_Table1_t));

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        File.TblData.TblElement1 = 0x04030201;
        File.TblData.TblElement2 = 0x08070605;
    }
    else
    {
        File.TblData.TblElement1 = 0x01020304;
        File.TblData.TblElement2 = 0x05060708;
    }

    UT_SetReadBuffer(&File, sizeof(File));
    UT_SetReadHeader(&FileHeader, sizeof(FileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);

    /* b. Perform load */
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&TblHandle1, "UT_RELEASE_TEST", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(TblHandle1, CFE_TBL_SRC_FILE, "MyInputFile"));

    /* c. Get addresses */
    ArrayOfHandles[0]       = TblHandle1;
    ArrayOfPtrsToTblPtrs[0] = &Tbl1Ptr;
    CFE_UtAssert_SUCCESS(CFE_TBL_GetAddresses(ArrayOfPtrsToTblPtrs, 1, ArrayOfHandles));
    UT_ClearEventHistory();

    /* d. Release 0 addresses */
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddresses(0, ArrayOfHandles));
    CFE_UtAssert_EVENTCOUNT(0);

    /* e. Release 1 addresses */
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddresses(1, ArrayOfHandles));
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function for validating the table image for a specified table
*/
void Test_CFE_TBL_Validate(void)
{
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_ValidationResult_t *ValResultPtr;

    UtPrintf("Begin Test Validate");
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccDescPtr, CFE_TBL_OPT_DEFAULT);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccDescPtr);

    /* Refer to the test validation function */
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;

    /* Test response to attempt to validate a table that an application is
     * not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_Validate(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to attempt to validate a table when no validation is
     * pending
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Validate(App1TblHandle1), CFE_TBL_INFO_NO_VALIDATION_PENDING);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test failed validation */
    UT_InitData();

    /* a. Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 1); /* Sets NextBufferId appropriately */

    /* b. Perform failed validation */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test validation on table w/user-defined address (this is not a valid combo) */
    UT_InitData();

    /* a. Configure table for validation and set UserDefAddr flag */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    UT_TBL_ClearLoadPending(RegRecPtr);
    UT_TBL_Config(RegRecPtr)->UserDefAddr = true;

    /* b. Perform validation */
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);
    UT_TBL_Config(RegRecPtr)->UserDefAddr = false;

    /* Test case where validation request is stale */
    UT_InitData();

    /* a. Configure table for validation and modify the ID so it will not match */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 1); /* Sets NextBufferId appropriately */
    ValResultPtr->ValId = CFE_TBL_VALRESULTID_C(CFE_ResourceId_FromInteger(1));

    /* b. Perform validation */
    UtAssert_INT32_EQ(CFE_TBL_Validate(App1TblHandle1), CFE_TBL_INFO_NO_VALIDATION_PENDING);
    UtAssert_INT32_EQ(ValResultPtr->Result, CFE_SUCCESS);

    /* Test successful validation */
    UT_InitData();

    /* a. Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 1); /* Sets NextBufferId appropriately */

    /* b. Perform validation */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    UtAssert_INT32_EQ(ValResultPtr->Result, CFE_SUCCESS);

    /* Set up a case where the entry does not have a validation function ptr */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 1); /* Sets NextBufferId appropriately */
    ValResultPtr->Result                        = -1;
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = NULL;

    /* b. Perform failed validation */
    CFE_UtAssert_SUCCESS(CFE_TBL_Validate(App1TblHandle1));
    UtAssert_INT32_EQ(ValResultPtr->Result, CFE_SUCCESS);
}

/*
** Test function for performing standard table maintenance on the
** specified table
*/
void Test_CFE_TBL_Manage(void)
{
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    UT_Table1_t                 TestTable1;
    void *                      App2TblPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_ValidationResult_t *ValResultPtr;
    CFE_TBL_DumpControl_t *     DumpCtrlPtr;
    CFE_TBL_AccessDescriptor_t *IterAccDescPtr;
    CFE_TBL_LoadBuff_t *        LoadBuffPtr;

    memset(&TestTable1, 0, sizeof(TestTable1));

    UtPrintf("Begin Test Manage");
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DEFAULT);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Test response to attempt to manage a table that doesn't need managing */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to attempt to load while a load is in progress */
    UT_InitData();

    /* Refer to the test validation function */
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;

    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr));
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_ADDRESS, &TestTable1), CFE_TBL_ERR_LOAD_IN_PROGRESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_IN_PROGRESS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer; validation function return code is valid
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test response to processing an unsuccessful validation request on
     * inactive buffer ; validation function return code is invalid
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 1);

    /* Test response to processing an unsuccessful validation request;
     * CFE_TBL_Validate does not return CFE_SUCCESS
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Manage(App1TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test response to processing a successful validation request on an
     * inactive buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, false, RegRecPtr, &ValResultPtr);
    ValResultPtr->Result = 1;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test response to processing an unsuccessful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, true, RegRecPtr, &ValResultPtr);
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test response to processing an unsuccessful validation request on
     * an active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, true, RegRecPtr, &ValResultPtr);
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, 1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 1);

    /* Test response to processing a successful validation request on an
     * active buffer
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(0, true, RegRecPtr, &ValResultPtr);
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    ValResultPtr->Result = 1;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle1));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test response to processing an update request on a locked table */
    /* a. Test setup - part 1 */
    UT_InitData();
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DBL_BUFFER);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, CFE_TBL_RegRecGetName(RegRecPtr)));
    CFE_UtAssert_EVENTCOUNT(0);

    /* a. Test setup - part 2 */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&App2TblPtr, App2TblHandle1), CFE_TBL_ERR_NEVER_LOADED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);

    /* Configure table for update */
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 1); /* Sets NextBufferId appropriately */
    UT_TBL_Status(RegRecPtr)->LoadPending = true;

    UtAssert_INT32_EQ(CFE_TBL_Manage(App1TblHandle1), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Repeat call, this is a mismatch where LoadPending is set but NextBufferId is NOT set. */
    UT_TBL_Status(RegRecPtr)->LoadPending  = true;
    UT_TBL_Status(RegRecPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_INT32_EQ(CFE_TBL_Manage(App1TblHandle1), CFE_TBL_INFO_NO_UPDATE_PENDING);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test unlocking a table by releasing the address */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App2TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to processing an update request on a single
     * buffered table
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);

    /* Configure table for Update */
    UT_TBL_Status(RegRecPtr)->LoadPending = true;
    LoadBuffPtr                           = UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    UtAssert_INT32_EQ(CFE_TBL_Manage(App1TblHandle1), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test response to processing an unsuccessful validation request on an
     * inactive buffer (double buffered)
     */
    UT_InitData();

    IterAccDescPtr = UT_TBL_AccDescFromExtHandle(App2TblHandle1);

    UT_TBL_Config(RegRecPtr)->DoubleBuffered = true;

    /* init both buffers, and make it so the older one appears to still be in use */
    LoadBuffPtr = UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    UT_TBL_InitActiveBuffer(RegRecPtr, 1);
    IterAccDescPtr->BufferIndex = CFE_TBL_LoadBufferGetID(LoadBuffPtr);
    IterAccDescPtr->LockFlag    = true;
    UtAssert_INT32_EQ(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr), CFE_TBL_ERR_NO_BUFFER_AVAIL);

    /* same but make it unlocked */
    IterAccDescPtr->LockFlag = false;
    UtAssert_INT32_EQ(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr), CFE_SUCCESS);

    /* Reset the table information for subsequent tests */
    LoadBuffPtr                 = CFE_TBL_GetActiveBuffer(RegRecPtr);
    IterAccDescPtr->BufferIndex = CFE_TBL_LoadBufferGetID(LoadBuffPtr);

    /* Successfully "load" image into inactive buffer for table */
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr));

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    App1TblHandle2                              = UT_TBL_AccDescToExtHandle(AccessDescPtr);
    UT_TBL_Config(RegRecPtr)->ValidationFuncPtr = Test_CFE_TBL_ValidationFunc;
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test successfully processing a validation request on an inactive buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, false, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test processing an unsuccessful validation request on an active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, true, RegRecPtr, &ValResultPtr);

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, -1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, -1);

    /* Test successfully processing a validation request on active buffer
     * (double buffered)
     */
    UT_InitData();

    /* Configure table for validation */
    UT_TBL_SetupPendingValidation(1, true, RegRecPtr, &ValResultPtr);
    ValResultPtr->Result = 1;

    /* Perform validation via manage call */
    UT_SetDeferredRetcode(UT_KEY(Test_CFE_TBL_ValidationFunc), 1, CFE_SUCCESS);
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTSENT(CFE_TBL_VALIDATION_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);
    UtAssert_INT32_EQ(ValResultPtr->Result, 0);

    /* Test successfully processing a table dump request */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr));
    UT_TBL_SetupPendingDump(0, WorkingBufferPtr, RegRecPtr, &DumpCtrlPtr);

    /* Save the name of the desired dump filename, table name, and size for
     * later
     */
    UT_TBL_SetName(WorkingBufferPtr->DataSource, sizeof(WorkingBufferPtr->DataSource), "MyDumpFilename");
    CFE_UtAssert_SUCCESS(CFE_TBL_Manage(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function for dumping to a buffer
*/
void Test_CFE_TBL_DumpToBuffer(void)
{
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_DumpControl_t *     DumpCtrlPtr;
    CFE_TBL_LoadBuff_t *        DumpBuffPtr;

    UtPrintf("Begin Test Dump To Buffer");
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DEFAULT);
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Test response to an attempt to dump the buffer on a table where no dump is pending */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(App1TblHandle1), CFE_TBL_INFO_NO_DUMP_PENDING);

    /* Test successfully dumping to a buffer */
    UT_InitData();
    DumpBuffPtr = CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_GLB_0);
    UT_TBL_SetupPendingDump(0, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    CFE_UtAssert_SUCCESS(CFE_TBL_DumpToBuffer(App1TblHandle1));

    /* Make the dump request stale */
    UT_TBL_SetupPendingDump(1, DumpBuffPtr, RegRecPtr, &DumpCtrlPtr);
    CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);
    UtAssert_BOOL_TRUE(CFE_TBL_DUMPCTRLID_IS_VALID(RegRecPtr->DumpControlId));
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(App1TblHandle1), CFE_TBL_INFO_NO_DUMP_PENDING);
    UtAssert_BOOL_FALSE(CFE_TBL_DUMPCTRLID_IS_VALID(RegRecPtr->DumpControlId));

    /* Test response to an attempt to dump the buffer on a table that the
     * application is not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to an attempt to dump the buffer on a bad table handle */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that updates the contents of a table if an update is pending
*/
void Test_CFE_TBL_Update(void)
{
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    UtPrintf("Begin Test Update");

    UT_SetAppID(UT_TBL_APPID_1);
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DBL_BUFFER);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Test a successful update */
    UT_InitData();

    /* a. Configure table for update */
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr));
    UT_TBL_Status(RegRecPtr)->LoadPending = true;

    /* b. Perform update test */
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_SUCCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test processing an update on a single buffered table without
     * privileges
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_2);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTSENT(CFE_TBL_UPDATE_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test processing an update on a single buffered table when no update
     * is pending
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_TBL_INFO_NO_UPDATE_PENDING);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test processing an update on an application with a bad ID
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test with resource not valid */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetAppID), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Update(App1TblHandle1), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(1);
}

/*
** Test function that obtains the pending action status for specified table
*/
void Test_CFE_TBL_GetStatus(void)
{
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    UtPrintf("Begin Test Get Status");
    UT_TBL_SetupSingleReg(NULL, &AccessDescPtr, CFE_TBL_OPT_DBL_BUFFER);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Test response to successfully getting status of a table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_GetStatus(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to an attempt to get the status on a table that the
     * application is not allowed to see
     */
    UT_InitData();
    UT_SetAppID(CFE_ES_APPID_UNDEFINED);
    UtAssert_INT32_EQ(CFE_TBL_GetStatus(App1TblHandle1), CFE_TBL_ERR_NO_ACCESS);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that obtains characteristics/information for a specified table
*/
void Test_CFE_TBL_GetInfo(void)
{
    CFE_TBL_Info_t              TblInfo;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    UtPrintf("Begin Test Get Info");

    UT_SetAppID(UT_TBL_APPID_1);
    UT_TBL_SetupSingleReg(&RegRecPtr, &AccessDescPtr, CFE_TBL_OPT_DBL_BUFFER);
    App1TblHandle1 = UT_TBL_AccDescToExtHandle(AccessDescPtr);

    /* Test response to a null table info and null table name */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(NULL, CFE_TBL_RegRecGetName(RegRecPtr)), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(&TblInfo, NULL), CFE_TBL_BAD_ARGUMENT);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successfully getting information on a table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_GetInfo(&TblInfo, CFE_TBL_RegRecGetName(RegRecPtr)));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to attempt to get information on a non-existent table */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetInfo(&TblInfo, "ut_cfe_tbl.UT_Table_Not"), CFE_TBL_ERR_INVALID_NAME);
    CFE_UtAssert_EVENTCOUNT(0);
}

/*
** Test function that loads a specified table with data from the
** specified source
*/
void Test_CFE_TBL_TblMod(void)
{
    CFE_FS_Header_t             FileHeader;
    UT_TempFile_t               File;
    uint32                      Index;
    CFE_TBL_Info_t              TblInfo1;
    void *                      TblDataAddr;
    UT_Table1_t *               TblDataPtr;
    char                        MyFilename[OS_MAX_PATH_LEN];
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_HandleId_t          AccessIterator;
    CFE_TBL_AccessDescriptor_t *IterAccDescPtr;
    uint8                       CDS_Data[sizeof(UT_Table1_t)];
    uint32                      ExpectedCrc;
    int                         maxPathLenDiff = (int)CFE_MISSION_MAX_PATH_LEN - (int)OS_MAX_PATH_LEN;

    memset(&TblInfo1, 0, sizeof(TblInfo1));

    UtPrintf("Begin Test Table Modified");

    /* Test adding a TBL API for notifying table services that the table has
     * been updated by the application
     */
    /* a. Test setup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetPoolBufferIndex();

    /* Test setup for CFE_TBL_Modified; register a non critical table */
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Attempt to call CFE_TBL_Modified before loading the table (should do nothing) */
    CFE_UtAssert_SUCCESS(CFE_TBL_Modified(App1TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Perform test */
    UT_ClearEventHistory();

    memset(&File, 0, sizeof(File));

    /* Configure for successful file read to initialize table */
    UT_TBL_SetName(FileHeader.Description, sizeof(FileHeader.Description), "FS header description");
    FileHeader.ContentType    = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType        = CFE_FS_SubType_TBL_IMG;
    FileHeader.TimeSeconds    = 1704;
    FileHeader.TimeSubSeconds = 104;
    UT_TBL_SetName(File.TblHeader.TableName, sizeof(File.TblHeader.TableName), "ut_cfe_tbl.UT_Table1");
    UT_TBL_SetupHeader(&File.TblHeader, 0, sizeof(UT_Table1_t));

    if (UT_Endianess == UT_LITTLE_ENDIAN)
    {
        File.TblData.TblElement1 = 0x04030201;
        File.TblData.TblElement2 = 0x08070605;
    }
    else
    {
        File.TblData.TblElement1 = 0x01020304;
        File.TblData.TblElement2 = 0x05060708;
    }

    UT_SetReadBuffer(&File, sizeof(File));
    UT_SetReadHeader(&FileHeader, sizeof(FileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);

    /* Perform load */
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "MyInputFile"));
    CFE_UtAssert_EVENTCOUNT(1);
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);

    /* Modify the contents of the table */
    CFE_TBL_GetAddress(&TblDataAddr, App1TblHandle1);
    TblDataPtr = TblDataAddr;
    TblDataPtr->TblElement1 ^= 0xFFFFFFFF;
    File.TblData.TblElement1 ^= 0xFFFFFFFF;

    /* Notify Table Services that the table has been modified */
    UT_SetDataBuffer(UT_KEY(CFE_ES_CopyToCDS), CDS_Data, sizeof(CDS_Data), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_Modified(App1TblHandle1));
    CFE_UtAssert_SUCCESS(CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table1"));
    UtAssert_UINT32_EQ(TblInfo1.TimeOfLastUpdate.Seconds, TblInfo1.TimeOfLastUpdate.Subseconds);
    UtAssert_STRINGBUF_EQ(TblInfo1.LastFileLoaded, sizeof(TblInfo1.LastFileLoaded), "MyInputFile(*)", -1);
    UtAssert_MemCmp(CDS_Data, &File.TblData, sizeof(CDS_Data), "Table Data");

    /* Save the previous table's information for a subsequent test */
    AccessDescPtr  = UT_TBL_AccDescFromExtHandle(App1TblHandle1);
    RegRecPtr      = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    AccessIterator = RegRecPtr->AccessList.Next;

    /* Test response to adding a TBL API for notifying table services that
     * the table has been updated by application
     */
    /* a. Test setup */
    UT_InitData();

    /* Register a non critical table */
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App1TblHandle1, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Reset the current table entry pointer to a previous table in order to
     * exercise the path where one of the application IDs don't match
     */
    AccessDescPtr              = UT_TBL_AccDescFromExtHandle(App1TblHandle1);
    RegRecPtr                  = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    IterAccDescPtr             = CFE_TBL_LocateAccDescByHandle(AccessIterator);
    IterAccDescPtr->Link.Next  = RegRecPtr->AccessList.Next;
    IterAccDescPtr->AppId      = UT_TBL_APPID_2;
    RegRecPtr->AccessList.Next = AccessIterator;

    /* Configure for successful file read to initialize table */
    UT_TBL_SetName(FileHeader.Description, sizeof(FileHeader.Description), "FS header description");
    FileHeader.ContentType    = CFE_FS_FILE_CONTENT_ID;
    FileHeader.SubType        = CFE_FS_SubType_TBL_IMG;
    FileHeader.TimeSeconds    = 1704;
    FileHeader.TimeSubSeconds = 104;
    UT_TBL_SetName(File.TblHeader.TableName, sizeof(File.TblHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&File.TblHeader, 0, sizeof(UT_Table1_t));

    File.TblData.TblElement1 = 0x04030201;
    File.TblData.TblElement2 = 0x08070605;
    UT_SetReadBuffer(&File, sizeof(File));
    UT_SetReadHeader(&FileHeader, sizeof(FileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);

    /* Perform load with extra long filename */
    for (Index = 0; Index < sizeof(MyFilename) - 1; Index++)
    {
        MyFilename[Index] = 'a';
    }
    MyFilename[sizeof(MyFilename) - 1] = '\0';

    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, MyFilename));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /*
     * Notify Table Services that the table has been modified. Verify CRC has been
     * calculated and table has been flagged as Updated
     */
    ExpectedCrc = 0x0000F00D;
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CalculateCRC), 1, ExpectedCrc);
    IterAccDescPtr          = CFE_TBL_LocateAccDescByHandle(AccessIterator);
    IterAccDescPtr->Updated = false;
    CFE_UtAssert_SUCCESS(CFE_TBL_Modified(App1TblHandle1));
    UtAssert_BOOL_TRUE(IterAccDescPtr->Updated);
    CFE_UtAssert_SUCCESS(CFE_TBL_GetInfo(&TblInfo1, "ut_cfe_tbl.UT_Table2"));
    UtAssert_INT32_EQ(TblInfo1.TimeOfLastUpdate.Seconds, TblInfo1.TimeOfLastUpdate.Subseconds);
    UtAssert_UINT32_EQ(TblInfo1.Crc, ExpectedCrc);
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblDataAddr, App1TblHandle1), CFE_TBL_INFO_UPDATED);

    /*
     * LastFileLoaded (limited by mission) can be bigger than MyFilename (limited by osal)
     */
    UtAssert_StrnCmp(TblInfo1.LastFileLoaded, MyFilename, sizeof(TblInfo1.LastFileLoaded) - 4, "%s == %s, %ld",
                     TblInfo1.LastFileLoaded, MyFilename, (long)sizeof(TblInfo1.LastFileLoaded) - 4);

    if (maxPathLenDiff >= 0)
    {
        UtAssert_StrCmp(&TblInfo1.LastFileLoaded[sizeof(MyFilename) - 4], "(*)", "%s == (*)",
                        &TblInfo1.LastFileLoaded[sizeof(MyFilename) - 4]);
    }
    else if (maxPathLenDiff > -3)
    {
        int modIndicatorStart = (int)CFE_MISSION_MAX_PATH_LEN - 4 - maxPathLenDiff;
        UtAssert_StrCmp(&TblInfo1.LastFileLoaded[modIndicatorStart], "(*)", "%s == (*)",
                        &TblInfo1.LastFileLoaded[modIndicatorStart]);
    }

    /* Test response to an invalid handle */
    UtAssert_INT32_EQ(CFE_TBL_Modified(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

/*
** Tests for the remaining functions in cfe_tbl_internal.c
*/
void Test_CFE_TBL_Internal1(void)
{
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    UtPrintf("Begin Test Internal 1");

    /* Test setup */
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t), CFE_TBL_OPT_CRITICAL, NULL));

    /* Register a non critical table */
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(
        CFE_TBL_Register(&App2TblHandle1, "UT_Table2", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));

    /* Test setup - register a double buffered table */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table3", sizeof(UT_Table1_t), CFE_TBL_OPT_DBL_BUFFER,
                                          Test_CFE_TBL_ValidationFunc));
    CFE_UtAssert_EVENTNOTSENT(CFE_TBL_REGISTER_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful initial load of double buffered table */
    UT_InitData();
    AccessDescPtr = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr     = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr));
    CFE_UtAssert_EVENTCOUNT(0);
    UtAssert_NOT_NULL(WorkingBufferPtr);
    UtAssert_NULL(CFE_TBL_GetActiveBuffer(RegRecPtr)); /* never loaded */

    /* Check that PrepareNewLoadBuff also sees the table not yet loaded */
    UT_InitData();
    AccessDescPtr = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr     = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UtAssert_NOT_NULL(WorkingBufferPtr = CFE_TBL_PrepareNewLoadBuff(RegRecPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test response to a single buffered table with a mutex sem take
     * failure
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    AccessDescPtr = UT_TBL_AccDescFromExtHandle(App1TblHandle1);
    RegRecPtr     = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_InitActiveBuffer(RegRecPtr, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_TxnRemoveAccessLink response to a failure to put back the
     * memory buffer for a double buffered table
     * Note: CFE_TBL_Unregister() does not propagate this error to the caller,
     * as there is no recourse and the table is still unregistered.  However, it
     * is invoked here for internal coverage paths.
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PutPoolBuf), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Unregister(App1TblHandle2), CFE_SUCCESS);
    CFE_UtAssert_EVENTCOUNT(0);

    /* EarlyInit - Table Registry Mutex Create Failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a work buffer mutex create failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 2, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a memory pool create failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_PoolCreate), 1, CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_ES_BAD_ARGUMENT);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a get pool buffer failure */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_GetPoolBuf), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_EarlyInit(), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response where the CDS already exists but
     * restore fails
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RestoreFromCDS), 1, CFE_ES_CDS_BLOCK_CRC_ERR);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response when no CDS is available */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_NOT_IMPLEMENTED);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_EarlyInit response to a failure to save a critical table
     * registry to the CDS
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Reset, then register tables for subsequent tests */
    /* a. Reset tables */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
}

/*
** Tests for the remaining functions in cfe_tbl_internal.c
*/
void Test_CFE_TBL_Internal2(void)
{
    CFE_TBL_TxnState_t          Txn;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_DumpControl_t *     DumpCtrlPtr;
    CFE_TBL_LoadBuff_t *        LoadBuffPtr;
    int32                       i;
    CFE_TBL_TableConfig_t       TblOpt;
    CFE_FS_Header_t             StdFileHeader;
    CFE_TBL_File_Hdr_t          TblFileHeader;
    void *                      TblPtr;

    UtPrintf("Begin Test Internal 2");

    /* b. Register critical single buffered table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "UT_Table1", sizeof(UT_Table1_t),
                                          CFE_TBL_OPT_DEFAULT | CFE_TBL_OPT_CRITICAL, Test_CFE_TBL_ValidationFunc));

    /* c. Register critical double buffered table */
    UT_InitData();
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle2, "UT_Table2", sizeof(UT_Table1_t),
                                          CFE_TBL_OPT_DBL_BUFFER | CFE_TBL_OPT_CRITICAL, Test_CFE_TBL_ValidationFunc));

    /* d. Perform an initial load on the critical single buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table1");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* e. Update CDS for single buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table1");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle1, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* f. Perform an initial load on the critical double buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* g. Update CDS for double buffered table */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    CFE_TBL_GetAddress(&TblPtr, App1TblHandle2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test CFE_TBL_GetWorkingBuffer response when both double buffered table
     * buffers are locked
     */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"),
                      CFE_TBL_ERR_NO_BUFFER_AVAIL);
    CFE_UtAssert_EVENTSENT(CFE_TBL_NO_WORK_BUFFERS_ERR_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Release buffer for error writing to CDS test */
    /* a. Get table address */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, App1TblHandle2), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Release table address */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Release buffer for error writing to CDS (second time) test */
    /* a. Get table address */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, App1TblHandle2), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Release table address */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Release buffer for failure to find the CDS handle in the CDS
     * registry test
     */
    /* a. Get table address */
    UT_InitData();
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TblPtr, App1TblHandle2), CFE_TBL_INFO_UPDATED);
    CFE_UtAssert_EVENTCOUNT(0);

    /* b. Release table address */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_ReleaseAddress(App1TblHandle2));
    CFE_UtAssert_EVENTCOUNT(0);

    /* c. Perform test */
    UT_ClearEventHistory();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    AccessDescPtr = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr     = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        if (CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.CritReg[i].CDSHandle, RegRecPtr->CDSHandle))
        {
            CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDSHANDLE_C(CFE_RESOURCEID_RESERVED);
        }
    }

    CFE_UtAssert_SUCCESS(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"));
    CFE_UtAssert_EVENTSENT(CFE_TBL_LOAD_SUCCESS_INF_EID);
    CFE_UtAssert_EVENTCOUNT(1);

    /* Test unregistering a shared table */
    /* a. Share table */
    UT_InitData();
    CFE_TBL_Global.CritReg[0].CDSHandle = RegRecPtr->CDSHandle;
    UT_SetAppID(UT_TBL_APPID_2);
    CFE_UtAssert_SUCCESS(CFE_TBL_Share(&App2TblHandle1, "ut_cfe_tbl.UT_Table1"));

    /* b. Perform test */
    UT_ClearEventHistory();
    CFE_UtAssert_SUCCESS(CFE_TBL_Unregister(App2TblHandle1));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test successful application cleanup */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_PutPoolBuf), -1);

    /* set up several pending dumps to check various branches in the loop (should be skipped) */
    /* this should fail initial match checks */
    UT_TBL_SetupPendingDump(1, NULL, NULL, NULL);

    /* also set up another pending dump from a different entry (should be skipped) */
    /* this should fail secondary match checks */
    AccessDescPtr    = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr        = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    WorkingBufferPtr = CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_GLB_0);
    UT_TBL_SetupPendingDump(2, WorkingBufferPtr, RegRecPtr, NULL);
    WorkingBufferPtr             = CFE_TBL_GetActiveBuffer(RegRecPtr);
    WorkingBufferPtr->OwnerRegId = CFE_TBL_REGID_UNDEFINED; /* make the owner invalid for now (for coverage) */

    /* Set up the target dump control block */
    /* this should pass all match checks */
    AccessDescPtr    = UT_TBL_AccDescFromExtHandle(App1TblHandle1);
    RegRecPtr        = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    WorkingBufferPtr = CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_GLB_1);
    UT_TBL_SetupPendingDump(3, WorkingBufferPtr, RegRecPtr, &DumpCtrlPtr);

    UT_TBL_Status(RegRecPtr)->NextBufferId = UT_CFE_TBL_LOADBUFFID_GLB_1;
    UT_TBL_SetLoadBuffTaken(WorkingBufferPtr, RegRecPtr, CFE_RESOURCEID_UNDEFINED);
    CFE_UtAssert_SUCCESS(CFE_TBL_CleanUpApp(UT_TBL_APPID_1));
    UtAssert_INT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_FREE);
    CFE_UtAssert_RESOURCEID_EQ(RegRecPtr->OwnerAppId, CFE_TBL_NOT_OWNED);
    UtAssert_BOOL_FALSE(CFE_TBL_LoadBuffIsUsed(WorkingBufferPtr));
    UtAssert_BOOL_FALSE(CFE_TBL_LOADBUFFID_IS_VALID(UT_TBL_Status(RegRecPtr)->NextBufferId));

    /* Test response to an attempt to use an invalid table handle */
    UT_InitData();
    StdFileHeader.ContentType = CFE_FS_FILE_CONTENT_ID;
    StdFileHeader.SubType     = CFE_FS_SubType_TBL_IMG;
    UT_TBL_SetName(TblFileHeader.TableName, sizeof(TblFileHeader.TableName), "ut_cfe_tbl.UT_Table2");
    UT_TBL_SetupHeader(&TblFileHeader, 0, sizeof(UT_Table1_t));

    UT_SetReadBuffer(&TblFileHeader, sizeof(TblFileHeader));
    UT_SetReadHeader(&StdFileHeader, sizeof(StdFileHeader));
    UT_SetDeferredRetcode(UT_KEY(OS_read), 3, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 2, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_Load(App1TblHandle2, CFE_TBL_SRC_FILE, "TblSrcFileName.dat"), CFE_TBL_ERR_INVALID_HANDLE);

    /* Test CFE_TBL_EarlyInit response where the CDS already exists and
     * restore succeeds
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_RegisterCDSEx), 1, CFE_ES_CDS_ALREADY_EXISTS);
    UT_ResetState(UT_KEY(CFE_ES_GetPoolBuf));
    UT_SetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), &UT_TBL_LoadBuffer, sizeof(UT_TBL_LoadBuffer), false);
    CFE_UtAssert_SUCCESS(CFE_TBL_EarlyInit());
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test starting a transaction where the handle is OK but the underlying registry record is invalid */
    UT_InitData();
    memset(&Txn, 0, sizeof(Txn));
    AccessDescPtr = CFE_TBL_LocateAccDescByHandle(UT_CFE_TBL_HANDLE_2);
    UT_TBL_SetHandleUsed(AccessDescPtr);
    AccessDescPtr->RegIndex = UT_CFE_TBL_REGID_INVL;
    UtAssert_INT32_EQ(CFE_TBL_TxnStartFromHandle(&Txn, AccessDescPtr->HandleId, 0), CFE_TBL_ERR_UNREGISTERED);

    UT_InitData();
    memset(&Txn, 0, sizeof(Txn));
    AccessDescPtr = CFE_TBL_LocateAccDescByHandle(UT_CFE_TBL_HANDLE_2);
    UT_TBL_SetHandleUsed(AccessDescPtr);
    AccessDescPtr->RegIndex = UT_CFE_TBL_REGID_INVH;
    UtAssert_INT32_EQ(CFE_TBL_TxnStartFromHandle(&Txn, AccessDescPtr->HandleId, 0), CFE_TBL_ERR_UNREGISTERED);
    CFE_TBL_AccDescSetFree(AccessDescPtr);

    /* Test CFE_TBL_TxnAllocateRegistryEntry response when the registry entry is
     * not owned but is not at the end of the list
     */
    UT_InitData();
    memset(&Txn, 0, sizeof(Txn));
    RegRecPtr                  = CFE_TBL_LocateRegRecByID(UT_CFE_TBL_REGID_0);
    RegRecPtr->OwnerAppId      = CFE_TBL_NOT_OWNED;
    RegRecPtr->AccessList.Next = UT_CFE_TBL_HANDLE_0;
    CFE_UtAssert_SUCCESS(CFE_TBL_TxnAllocateRegistryEntry(&Txn));
    UtAssert_INT32_EQ(CFE_TBL_REGID_INT(CFE_TBL_TxnRegId(&Txn)), 1);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_LockRegistry response when an error occurs taking the mutex
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_LockRegistry(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UnlockRegistry response when an error occurs giving the
     * mutex
     */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_TBL_UnlockRegistry(), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UpdateInternal response when an inactive buffer is ready to
     * be copied but a load is in progress
     */
    UT_InitData();
    AccessDescPtr                          = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr                              = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_Status(RegRecPtr)->LoadPending  = true;
    UT_TBL_Status(RegRecPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_INT32_EQ(CFE_TBL_UpdateInternal(AccessDescPtr->HandleId, RegRecPtr, AccessDescPtr),
                      CFE_TBL_INFO_NO_UPDATE_PENDING);
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UpdateInternal response when an inactive buffer is ready to
     * be copied but a load is in progress
     */
    UT_InitData();
    AccessDescPtr                         = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr                             = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_Status(RegRecPtr)->LoadPending = true;
    UT_TBL_Config(RegRecPtr)->Critical    = false;
    UT_TBL_SetupLoadBuff(RegRecPtr, true, 0);
    UT_SetDeferredRetcode(UT_KEY(CFE_ES_CopyToCDS), 1, CFE_ES_ERR_RESOURCEID_NOT_VALID);
    CFE_UtAssert_SUCCESS(CFE_TBL_UpdateInternal(AccessDescPtr->HandleId, RegRecPtr, AccessDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UpdateInternal single buffer memcpy when
     * source and dest are not equal
     */
    UT_InitData();
    AccessDescPtr                         = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr                             = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_Status(RegRecPtr)->LoadPending = true;
    UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    CFE_UtAssert_SUCCESS(CFE_TBL_UpdateInternal(AccessDescPtr->HandleId, RegRecPtr, AccessDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test CFE_TBL_UpdateInternal with overlapping memcopy (bug) */
    UT_InitData();
    AccessDescPtr                         = UT_TBL_AccDescFromExtHandle(App1TblHandle2);
    RegRecPtr                             = CFE_TBL_LocateRegRecByID(AccessDescPtr->RegIndex);
    UT_TBL_Status(RegRecPtr)->LoadPending = true;

    LoadBuffPtr = UT_TBL_SetupLoadBuff(RegRecPtr, false, 0);
    UT_TBL_SetActiveBufferAddr(RegRecPtr, 0, LoadBuffPtr->BufferPtr);

    CFE_UtAssert_SUCCESS(CFE_TBL_UpdateInternal(AccessDescPtr->HandleId, RegRecPtr, AccessDescPtr));
    CFE_UtAssert_EVENTCOUNT(0);

    /* Test application cleanup where there are no dumped tables to delete,
     * one unused handle and one used but not owned table
     */
    UT_InitData();
    UT_SetAppID(UT_TBL_APPID_1);
    UT_SetDefaultReturnValue(UT_KEY(CFE_ES_PutPoolBuf), -1);
    RegRecPtr = CFE_TBL_LocateRegRecByID(UT_CFE_TBL_REGID_0);
    UT_TBL_SetupPendingDump(3, NULL, RegRecPtr, &DumpCtrlPtr);

    AccessDescPtr        = CFE_TBL_LocateAccDescByHandle(UT_CFE_TBL_HANDLE_0);
    AccessDescPtr->AppId = UT_TBL_APPID_1;
    UT_TBL_SetHandleUsed(AccessDescPtr);
    AccessDescPtr->RegIndex = UT_CFE_TBL_REGID_0;
    RegRecPtr->OwnerAppId   = UT_TBL_APPID_2;

    AccessDescPtr        = CFE_TBL_LocateAccDescByHandle(UT_CFE_TBL_HANDLE_1);
    AccessDescPtr->AppId = UT_TBL_APPID_2;
    UT_TBL_SetHandleUsed(AccessDescPtr);

    CFE_UtAssert_SUCCESS(CFE_TBL_CleanUpApp(UT_TBL_APPID_1));
    UtAssert_INT32_EQ(DumpCtrlPtr->State, CFE_TBL_DUMP_PENDING);
    CFE_UtAssert_RESOURCEID_EQ(RegRecPtr->OwnerAppId, UT_TBL_APPID_2);

    /* Test CFE_TBL_RestoreTableDataFromCDS() when failed to get a working buffer */
    UT_InitData();

    RegRecPtr = CFE_TBL_LocateRegRecByID(UT_CFE_TBL_REGID_0);

    UT_TBL_Config(RegRecPtr)->DoubleBuffered = false;

    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_FindNextEx), 1, -1);

    UtAssert_INT32_EQ(CFE_TBL_RestoreTableDataFromCDS(RegRecPtr), CFE_TBL_ERR_NO_BUFFER_AVAIL);

    UT_ClearEventHistory();

    UT_InitData();
    memset(&TblOpt, 0, sizeof(TblOpt));

    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize(&TblOpt, 0), CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize(&TblOpt, CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize(&TblOpt, CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE + 1),
                      CFE_TBL_ERR_INVALID_SIZE);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize(&TblOpt, CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1),
                      CFE_TBL_ERR_INVALID_SIZE);

    TblOpt.DoubleBuffered = true;
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize(&TblOpt, CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableSize(&TblOpt, CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE + 1),
                      CFE_TBL_ERR_INVALID_SIZE);
}

/*
 * Tests the resource accessors for Validation Results
 */
void Test_CFE_TBL_ResourceID_ValidationResult(void)
{
    uint32                       Idx;
    CFE_TBL_ValidationResultId_t InvalidResultId;
    CFE_TBL_ValidationResultId_t ValidResultId;
    CFE_ResourceId_t             PendingId;

    UT_InitData();

    InvalidResultId = CFE_TBL_VALRESULTID_UNDEFINED;
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_ValidationResultId_ToIndex(InvalidResultId, &Idx), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateValidationResultByID(InvalidResultId));
    UT_ResetState(UT_KEY(CFE_ResourceId_ToIndex));

    ValidResultId = CFE_TBL_VALRESULTID_C(CFE_ResourceId_FromInteger(CFE_TBL_VALRESULTID_BASE + 1));
    UtAssert_INT32_EQ(CFE_TBL_ValidationResultId_ToIndex(ValidResultId, &Idx), CFE_SUCCESS);

    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextValResultBlock());
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));

    /* The slot should be available right now */
    UtAssert_BOOL_FALSE(CFE_TBL_CheckValidationResultSlotUsed(PendingId));

    /* Make it used and confirm it is reported as not available */
    CFE_TBL_ValidationResultSetUsed(CFE_TBL_LocateValidationResultByID(CFE_TBL_VALRESULTID_C(PendingId)), PendingId);
    UtAssert_BOOL_TRUE(CFE_TBL_CheckValidationResultSlotUsed(PendingId));

    /* Test case where no ID is available */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), -1);
    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextValResultBlock());
    UtAssert_BOOL_FALSE(CFE_ResourceId_IsDefined(PendingId));

    /* A nonexistent slot is always "unavailable" */
    UtAssert_BOOL_TRUE(CFE_TBL_CheckValidationResultSlotUsed(PendingId));
    UT_ResetState(UT_KEY(CFE_ResourceId_FindNext));
}

/*
 * Tests the resource accessors for Table Registry Records
 */
void Test_CFE_TBL_ResourceID_RegistryRecord(void)
{
    uint32                 Idx;
    CFE_TBL_RegId_t        InvalidRegId;
    CFE_TBL_RegId_t        ValidRegId;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    UT_InitData();

    InvalidRegId = UT_CFE_TBL_REGID_INVL;
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_ToIndex), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_INT32_EQ(CFE_TBL_RegId_ToIndex(InvalidRegId, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateRegRecByID(InvalidRegId));

    InvalidRegId = UT_CFE_TBL_REGID_INVH;
    UtAssert_INT32_EQ(CFE_TBL_RegId_ToIndex(InvalidRegId, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* Now with a valid ID */
    ValidRegId = UT_CFE_TBL_REGID_1;
    UT_ResetState(UT_KEY(CFE_ResourceId_ToIndex));
    UtAssert_INT32_EQ(CFE_TBL_RegId_ToIndex(ValidRegId, &Idx), CFE_SUCCESS);
    UtAssert_UINT32_EQ(Idx, 1);
    UtAssert_NOT_NULL(RegRecPtr = CFE_TBL_LocateRegRecByID(ValidRegId));
}

/*
 * Tests the resource accessors for Table Access Descriptors
 */
void Test_CFE_TBL_ResourceID_AccessDescriptor(void)
{
    uint32             Idx;
    CFE_TBL_HandleId_t InvalidHandle;
    CFE_TBL_HandleId_t ValidHandle;

    UT_InitData();

    InvalidHandle = UT_CFE_TBL_HANDLE_INVL;
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_ToIndex), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_INT32_EQ(CFE_TBL_Handle_ToIndex(InvalidHandle, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateAccDescByHandle(InvalidHandle));

    /* Make up something that is neither the "defined" invalid handle ID (i.e. CFE_TBL_BAD_TABLE_HANDLE)
     * nor is it a valid one - it should fail with the same status */
    memset(&InvalidHandle, 0xEE, sizeof(InvalidHandle));
    UtAssert_INT32_EQ(CFE_TBL_Handle_ToIndex(InvalidHandle, &Idx), CFE_TBL_ERR_INVALID_HANDLE);

    /* Now with a valid ID */
    ValidHandle = UT_CFE_TBL_HANDLE_1;
    UT_ResetState(UT_KEY(CFE_ResourceId_ToIndex));
    UtAssert_INT32_EQ(CFE_TBL_Handle_ToIndex(ValidHandle, &Idx), CFE_SUCCESS);
    UtAssert_UINT32_EQ(Idx, 1);
}

/*
 * Tests the resource accessors for Dump Control Blocks
 */
void Test_CFE_TBL_ResourceID_DumpControl(void)
{
    uint32               Idx;
    CFE_TBL_DumpCtrlId_t InvalidBlockId;
    CFE_TBL_DumpCtrlId_t ValidBlockId;
    CFE_ResourceId_t     PendingId;

    UT_InitData();

    InvalidBlockId = CFE_TBL_DUMPCTRLID_UNDEFINED;
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_ToIndex), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_TBL_DumpCtrlId_ToIndex(InvalidBlockId, &Idx), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    /* by definition, looking up the undefined value should always be NULL */
    UtAssert_NULL(CFE_TBL_LocateDumpCtrlByID(InvalidBlockId));
    UT_ResetState(UT_KEY(CFE_ResourceId_ToIndex));

    ValidBlockId = CFE_TBL_DUMPCTRLID_C(CFE_ResourceId_FromInteger(CFE_TBL_DUMPCTRLID_BASE + 1));
    UtAssert_INT32_EQ(CFE_TBL_DumpCtrlId_ToIndex(ValidBlockId, &Idx), CFE_SUCCESS);

    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextDumpCtrlBlock());
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));

    /* The slot should be available right now */
    UtAssert_BOOL_FALSE(CFE_TBL_CheckDumpCtrlSlotUsed(PendingId));

    /* Make it used and confirm it is reported as not available */
    CFE_TBL_DumpCtrlBlockSetUsed(CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DUMPCTRLID_C(PendingId)), PendingId);
    UtAssert_BOOL_TRUE(CFE_TBL_CheckDumpCtrlSlotUsed(PendingId));

    /* Test case where no ID is available */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), -1);
    UtAssert_VOIDCALL(PendingId = CFE_TBL_GetNextDumpCtrlBlock());
    UtAssert_BOOL_FALSE(CFE_ResourceId_IsDefined(PendingId));

    /* A nonexistent slot is always "unavailable" */
    UtAssert_BOOL_TRUE(CFE_TBL_CheckDumpCtrlSlotUsed(PendingId));
    UT_ResetState(UT_KEY(CFE_ResourceId_FindNext));
}

/*
** Test function executed when the contents of a table need to be validated
*/
int32 Test_CFE_TBL_ValidationFunc(void *TblPtr)
{
    return UT_DEFAULT_IMPL(Test_CFE_TBL_ValidationFunc);
}

/*
 * Test various aspects of the Transaction State structure and its associated routines
 * Many of these transaction calls are used under the hood by other tests, but this ensures that certain
 * error cases are triggered, and it also checks the basic sanity of operation for the accessors.
 */
void Test_CFE_TBL_TxnState(void)
{
    CFE_TBL_TxnState_t    Txn;
    CFE_TBL_TableConfig_t Cfg;
    void *                addr;
    char                  Buf[CFE_MISSION_TBL_MAX_NAME_LENGTH + 4];
    char                  FullBuf[CFE_TBL_MAX_FULL_NAME_LEN + 4];

    memset(&Txn, 0, sizeof(Txn));
    UT_InitData();
    UT_ResetTableRegistry();

    /* All of the accessors should return 0 which is the undefined value */
    UtAssert_ZERO(CFE_TBL_TxnHandleAsULong(&Txn));
    UtAssert_ZERO(CFE_TBL_TxnRegIdAsULong(&Txn));
    UtAssert_ZERO(CFE_TBL_TxnAppIdAsULong(&Txn));
    UtAssert_NULL(CFE_TBL_TxnAccDesc(&Txn));
    UtAssert_NULL(CFE_TBL_TxnRegRec(&Txn));
    UtAssert_NOT_NULL(CFE_TBL_TxnAppNameCaller(&Txn));

    /* If the registry is locked, the CFE_TBL_TxnAppNameCaller() reports a special string */
    memset(&Txn, 0, sizeof(Txn));
    UtAssert_VOIDCALL(CFE_TBL_TxnLockRegistry(&Txn));
    UtAssert_STUB_COUNT(OS_MutSemTake, 1);
    UtAssert_STRINGBUF_EQ(CFE_TBL_TxnAppNameCaller(&Txn), -1, "[!LOCKED!]", -1);

    /* calling lock again should not re-take the sem */
    UtAssert_VOIDCALL(CFE_TBL_TxnLockRegistry(&Txn));
    UtAssert_STUB_COUNT(OS_MutSemTake, 1);
    UtAssert_STUB_COUNT(OS_MutSemGive, 0);
    UtAssert_VOIDCALL(CFE_TBL_TxnUnlockRegistry(&Txn));
    UtAssert_STUB_COUNT(OS_MutSemGive, 0);
    UtAssert_VOIDCALL(CFE_TBL_TxnUnlockRegistry(&Txn));
    UtAssert_STUB_COUNT(OS_MutSemGive, 1);

    /* Calling unlock again should not re-give the sem */
    UtAssert_VOIDCALL(CFE_TBL_TxnUnlockRegistry(&Txn));
    UtAssert_STUB_COUNT(OS_MutSemGive, 1);

    CFE_UtAssert_SUCCESS(CFE_TBL_TxnInit(&Txn, true));
    UtAssert_VOIDCALL(CFE_TBL_TxnFinish(&Txn));

    /* Check that the "CFE_TBL_TxnGetFullTableName" works as expected, including buffer too short error */
    CFE_UtAssert_SUCCESS(CFE_TBL_TxnInit(&Txn, false));

    memset(&Cfg, 0, sizeof(Cfg));
    CFE_UtAssert_SUCCESS(CFE_TBL_TxnCheckConfig(&Txn, &Cfg, "ut", 0, 10, NULL));

    /* Check table name validation */
    /* This name should be too short */
    memset(Buf, 0, sizeof(Buf));
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableName(&Cfg, Buf, Buf), CFE_TBL_ERR_INVALID_NAME);
    UtAssert_INT32_EQ(CFE_TBL_TxnCheckConfig(&Txn, &Cfg, Buf, 0, 0, NULL), CFE_TBL_ERR_INVALID_NAME);

    /* This name should be too long */
    memset(Buf, 'a', sizeof(Buf) - 1);
    Buf[sizeof(Buf) - 1] = 0;
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableName(&Cfg, Buf, "app"), CFE_TBL_ERR_INVALID_NAME);

    /* Set an app name that is too long */
    /* In this case the table name is within limits but the app name pushes it above full name limit */
    memset(FullBuf, 'b', sizeof(FullBuf) - 1);
    FullBuf[sizeof(FullBuf) - 1]             = 0;
    Buf[CFE_MISSION_TBL_MAX_NAME_LENGTH - 1] = 0;
    UtAssert_INT32_EQ(CFE_TBL_ValidateTableName(&Cfg, Buf, FullBuf), CFE_TBL_ERR_INVALID_NAME);

    UtAssert_VOIDCALL(CFE_TBL_TxnFinish(&Txn));

    /* Check that the "start" routines get the correct error code if no table is registered */
    UtAssert_INT32_EQ(CFE_TBL_TxnStartFromName(&Txn, "ut", CFE_TBL_TxnContext_UNDEFINED), CFE_TBL_ERR_INVALID_NAME);
    UtAssert_ZERO(Txn.RegLockCount);
    UtAssert_INT32_EQ(CFE_TBL_TxnStartFromHandle(&Txn, CFE_TBL_HANDLEID_UNDEFINED, CFE_TBL_TxnContext_UNDEFINED),
                      CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_ZERO(Txn.RegLockCount);

    /* Now register a table and check that the "start" routines work in the nominal case */
    CFE_UtAssert_SUCCESS(CFE_TBL_Register(&App1TblHandle1, "ut", sizeof(UT_Table1_t), CFE_TBL_OPT_DEFAULT, NULL));

    CFE_UtAssert_SUCCESS(CFE_TBL_TxnStartFromName(&Txn, "ut_cfe_tbl.ut", CFE_TBL_TxnContext_ALL));
    UtAssert_NOT_NULL(CFE_TBL_TxnRegRec(&Txn));
    UtAssert_NONZERO(Txn.RegLockCount);
    UtAssert_VOIDCALL(CFE_TBL_TxnFinish(&Txn));
    UtAssert_ZERO(Txn.RegLockCount);

    UT_SetAppID(UT_TBL_APPID_10);
    UT_TBL_SetupTxnFromExtHandle(&Txn, App1TblHandle1, CFE_TBL_TxnContext_ALL);
    UtAssert_NOT_NULL(CFE_TBL_TxnRegRec(&Txn));
    UtAssert_NONZERO(Txn.RegLockCount);

    CFE_UtAssert_SUCCESS(CFE_TBL_FindAccessDescriptorForSelf(&Txn));
    UtAssert_NOT_NULL(CFE_TBL_TxnAccDesc(&Txn));

    /* Sanity check various status calls before closing the transaction */
    CFE_UtAssert_SUCCESS(CFE_TBL_TxnGetTableStatus(&Txn));
    UtAssert_INT32_EQ(CFE_TBL_TxnGetTableAddress(&Txn, &addr), CFE_TBL_ERR_NEVER_LOADED);
    UtAssert_INT32_EQ(CFE_TBL_TxnGetNextNotification(&Txn), CFE_TBL_ERR_NEVER_LOADED);

    UtAssert_VOIDCALL(CFE_TBL_TxnFinish(&Txn));
    UtAssert_ZERO(Txn.RegLockCount);
}

void Test_CFE_TBL_ResourceID_LoadBuff(void)
{
    uint32                 Serial;
    uint32                 CategoryIdx;
    uint32                 LoadIdx;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_ResourceId_t       PendingId;
    uint32                 TempBuf;
    CFE_TBL_LoadBuff_t *   BufferPtr;
    void *                 MemPtr;

    CFE_TBL_LoadBuffFindNextState_t FindNext;

    UT_InitData();

    /* Test cases for:
     * void CFE_TBL_LoadBuffSerialDecompose(uint32 Serial, uint32 *CategoryOut, uint32 *IdxOut)
     * void CFE_TBL_LoadBuffSerialCompose(uint32 *SerialInOut, uint32 ReqCategory, uint32 ReqIdx)
     */
    Serial      = 0;
    CategoryIdx = -1;
    LoadIdx     = -1;
    CFE_TBL_LoadBuffSerialCompose(&Serial, 0, 0);
    UtAssert_ZERO(Serial);
    CFE_TBL_LoadBuffSerialDecompose(Serial, &CategoryIdx, &LoadIdx);
    UtAssert_ZERO(CategoryIdx);
    UtAssert_ZERO(LoadIdx);

    /* null pointer outputs are allowed */
    CFE_TBL_LoadBuffSerialDecompose(Serial, NULL, &LoadIdx);
    CFE_TBL_LoadBuffSerialDecompose(Serial, &CategoryIdx, NULL);
    CFE_TBL_LoadBuffSerialDecompose(Serial, NULL, NULL);

    CFE_TBL_LoadBuffSerialCompose(&Serial, 1, 1);
    UtAssert_NONZERO(Serial);
    CFE_TBL_LoadBuffSerialDecompose(Serial, &CategoryIdx, &LoadIdx);
    UtAssert_UINT32_EQ(CategoryIdx, 1);
    UtAssert_UINT32_EQ(LoadIdx, 1);

    /* null pointer outputs are allowed */
    CFE_TBL_LoadBuffSerialDecompose(Serial, NULL, &LoadIdx);
    CFE_TBL_LoadBuffSerialDecompose(Serial, &CategoryIdx, NULL);
    CFE_TBL_LoadBuffSerialDecompose(Serial, NULL, NULL);

    /* Test cases for:
     * CFE_Status_t CFE_TBL_LoadBuffId_ToIndex(CFE_TBL_LoadBuffId_t BuffId, uint32 *Category, uint32 *Idx)
     */
    /* nominal */
    CFE_UtAssert_SUCCESS(CFE_TBL_LoadBuffId_ToIndex(UT_CFE_TBL_LOADBUFFID_GLB_0, &CategoryIdx, &LoadIdx));
    UtAssert_ZERO(CategoryIdx);
    UtAssert_ZERO(LoadIdx);

    CFE_UtAssert_SUCCESS(CFE_TBL_LoadBuffId_ToIndex(UT_CFE_TBL_LOADBUFFID_REG_0_1, &CategoryIdx, &LoadIdx));
    UtAssert_UINT32_EQ(CategoryIdx, 1);
    UtAssert_UINT32_EQ(LoadIdx, 1);

    /* error cases */
    /* with resourceid-based load buffer IDs, this now depends on CFE_ResourceId_ToIndex */
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_ToIndex), 1, CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_INT32_EQ(CFE_TBL_LoadBuffId_ToIndex(UT_CFE_TBL_LOADBUFFID_INVL, &CategoryIdx, &LoadIdx),
                      CFE_TBL_ERR_INVALID_HANDLE);

    /* Test cases for:
     * CFE_TBL_LoadBuff_t *CFE_TBL_LocateLoadBufferByID(CFE_TBL_LoadBuffId_t BufferId)
     */
    UT_SetDeferredRetcode(UT_KEY(CFE_ResourceId_ToIndex), 1, CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_NULL(CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_INVL));

    /* Check locating by ID */
    UT_ResetState(0);
    UT_TBL_SetupSingleReg(&RegRecPtr, NULL, CFE_TBL_OPT_DEFAULT);

    /* Single-buffered: all local refs resolve to entry 0 */
    UT_TBL_Config(RegRecPtr)->DoubleBuffered = false;
    UtAssert_ADDRESS_EQ(CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_REG_1_0), &RegRecPtr->Buffers[0]);
    UtAssert_ADDRESS_EQ(CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_REG_1_1), &RegRecPtr->Buffers[0]);
    UtAssert_ADDRESS_EQ(CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_GLB_1), &CFE_TBL_Global.LoadBuffs[1]);

    /* Double-buffered: local refs may be entry 0 or 1 */
    UT_TBL_Config(RegRecPtr)->DoubleBuffered = true;
    UtAssert_ADDRESS_EQ(CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_REG_1_0), &RegRecPtr->Buffers[0]);
    UtAssert_ADDRESS_EQ(CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_REG_1_1), &RegRecPtr->Buffers[1]);
    UtAssert_ADDRESS_EQ(CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_GLB_1), &CFE_TBL_Global.LoadBuffs[1]);

    /* Test cases for:
     * bool CFE_TBL_CheckLoadBuffSlotUsed(CFE_ResourceId_t CheckId)
     */
    UtAssert_BOOL_TRUE(CFE_TBL_CheckLoadBuffSlotUsed(CFE_RESOURCEID_UNDEFINED));
    PendingId                                = CFE_ResourceId_FromInteger(CFE_TBL_LOADBUFFID_BASE + 0);
    CFE_TBL_Global.LoadBuffs[0].LoadBufferId = CFE_TBL_LOADBUFFID_C(PendingId);
    UtAssert_BOOL_TRUE(CFE_TBL_CheckLoadBuffSlotUsed(PendingId));
    CFE_TBL_Global.LoadBuffs[0].LoadBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
    UtAssert_BOOL_FALSE(CFE_TBL_CheckLoadBuffSlotUsed(PendingId));

    /* Test cases for:
     * CFE_ResourceId_t CFE_TBL_LoadBuffIncrementSerial(CFE_ResourceId_t Id, void *Arg)
     */
    memset(&FindNext, 0, sizeof(FindNext));

    PendingId = CFE_ResourceId_FromInteger(UT_CFE_TBL_LOADBUFFID_GLB_BASE);
    PendingId = CFE_TBL_LoadBuffIncrementSerial(PendingId, &FindNext);
    UtAssert_BOOL_FALSE(CFE_ResourceId_IsDefined(PendingId));

    PendingId               = CFE_ResourceId_FromInteger(UT_CFE_TBL_LOADBUFFID_GLB_BASE);
    FindNext.RemainingCount = 1;
    FindNext.BaseCategory   = 0;
    PendingId               = CFE_TBL_LoadBuffIncrementSerial(PendingId, &FindNext);
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));
    UtAssert_UINT32_EQ(CFE_ResourceId_ToInteger(PendingId), UT_CFE_TBL_LOADBUFFID_GLB_BASE + 1);
    UtAssert_ZERO(FindNext.RemainingCount);

    FindNext.RemainingCount = 1;
    FindNext.BaseCategory   = 1;
    PendingId               = CFE_TBL_LoadBuffIncrementSerial(PendingId, &FindNext);
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));
    UtAssert_ZERO(FindNext.RemainingCount);

    /* This is intended to test the wrap-around */
    PendingId               = CFE_ResourceId_FromInteger(INT32_MAX - 1);
    FindNext.RemainingCount = 1;
    FindNext.BaseCategory   = 1;
    PendingId               = CFE_TBL_LoadBuffIncrementSerial(PendingId, &FindNext);
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));
    UtAssert_UINT32_EQ(CFE_ResourceId_ToInteger(PendingId), UT_CFE_TBL_LOADBUFFID_REG_BASE + 1);
    UtAssert_ZERO(FindNext.RemainingCount);

    /* Test cases for:
     * CFE_ResourceId_t CFE_TBL_GetNextLocalBufferId(CFE_TBL_RegistryRec_t *RegRecPtr)
     */
    UT_TBL_Config(RegRecPtr)->DoubleBuffered = false;
    PendingId                                = CFE_TBL_GetNextLocalBufferId(RegRecPtr);
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));

    UT_TBL_Config(RegRecPtr)->DoubleBuffered = true;
    PendingId                                = CFE_TBL_GetNextLocalBufferId(RegRecPtr);
    UtAssert_BOOL_TRUE(CFE_ResourceId_IsDefined(PendingId));

    /* Force the ID conversion to fail (in the current impl this can only happen if out of range) */
    PendingId = CFE_TBL_GetNextLocalBufferId(RegRecPtr + CFE_PLATFORM_TBL_MAX_NUM_TABLES);
    UtAssert_BOOL_FALSE(CFE_ResourceId_IsDefined(PendingId));

    /* Test cases for:
     * void CFE_TBL_LoadBuffClearData(CFE_TBL_LoadBuff_t *BufferPtr);
     * void CFE_TBL_LoadBuffCopyData(CFE_TBL_LoadBuff_t *BufferPtr, const void *SourcePtr, size_t SourceSize);
     * void CFE_TBL_LoadBuffRecomputeCRC(CFE_TBL_LoadBuff_t *BufferPtr);
     */
    UtAssert_NOT_NULL(BufferPtr = CFE_TBL_LocateLoadBufferByID(UT_CFE_TBL_LOADBUFFID_GLB_0));
    UtAssert_NOT_NULL(MemPtr = CFE_TBL_LoadBuffGetWritePointer(BufferPtr));
    UtAssert_NONZERO(CFE_TBL_LoadBuffGetAllocSize(BufferPtr));

    memset(MemPtr, 0xAA, CFE_TBL_LoadBuffGetAllocSize(BufferPtr));
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffClearData(BufferPtr));
    UtAssert_ZERO(*((const uint8 *)MemPtr));
    UtAssert_ZERO(CFE_TBL_LoadBuffGetContentSize(BufferPtr));
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffRecomputeCRC(BufferPtr));

    memset(&TempBuf, 0xBB, sizeof(TempBuf));
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffCopyData(BufferPtr, &TempBuf, sizeof(TempBuf)));
    UtAssert_UINT8_EQ(*((const uint8 *)MemPtr), 0xBB);
    UtAssert_EQ(size_t, CFE_TBL_LoadBuffGetContentSize(BufferPtr), sizeof(TempBuf));
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffRecomputeCRC(BufferPtr));

    memset(&TempBuf, 0xCC, sizeof(TempBuf));
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffCopyData(BufferPtr, &TempBuf, CFE_TBL_LoadBuffGetAllocSize(BufferPtr) + 10));
    UtAssert_UINT8_EQ(*((const uint8 *)MemPtr), 0xBB);
    UtAssert_EQ(size_t, CFE_TBL_LoadBuffGetContentSize(BufferPtr), sizeof(TempBuf));

    CFE_TBL_LoadBuffSetExternalBlock(BufferPtr, &TempBuf, sizeof(TempBuf));
    UtAssert_NULL(CFE_TBL_LoadBuffGetWritePointer(BufferPtr));
    UtAssert_NOT_NULL(CFE_TBL_LoadBuffGetReadPointer(BufferPtr));
    memset(&TempBuf, 0xAA, sizeof(TempBuf));
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffClearData(BufferPtr));
    UtAssert_UINT32_EQ(TempBuf, 0xAAAAAAAA); /* should not have cleared */
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffCopyData(BufferPtr, NULL, 0));
    UtAssert_VOIDCALL(CFE_TBL_LoadBuffRecomputeCRC(BufferPtr));
}

static bool UT_TBL_TxnEventProcFunc(const CFE_TBL_TxnEvent_t *Txn, void *Arg)
{
    return (UT_DEFAULT_IMPL(UT_TBL_TxnEventProcFunc));
}

void Test_CFE_TBL_TxnEvents(void)
{
    /*
     * Test the following API set:
     *
     * void CFE_TBL_TxnAddEvent(CFE_TBL_TxnState_t *Txn, uint16 EventId, int32 EventData1, int32 EventData2)
     * uint32 CFE_TBL_TxnGetEventCount(const CFE_TBL_TxnState_t *Txn)
     * uint32 CFE_TBL_TxnProcessEvents(const CFE_TBL_TxnState_t *Txn, CFE_TBL_TxnEventProcFunc_t EventProc, void *Arg)
     * void CFE_TBL_TxnClearEvents(CFE_TBL_TxnState_t *Txn)
     */

    CFE_TBL_TxnState_t Txn;
    uint32             i;

    /* set the memory to something nonzero to validate correct initialization */
    memset(&Txn, 0xAA, sizeof(Txn));

    CFE_TBL_TxnInit(&Txn, false);

    UtAssert_UINT32_EQ(CFE_TBL_TxnProcessEvents(&Txn, UT_TBL_TxnEventProcFunc, NULL), 0);
    UtAssert_STUB_COUNT(UT_TBL_TxnEventProcFunc, 0);

    for (i = 0; i < CFE_TBL_MAX_EVENTS_PER_TXN; ++i)
    {
        UtAssert_UINT32_EQ(CFE_TBL_TxnGetEventCount(&Txn), i);
        UtAssert_VOIDCALL(CFE_TBL_TxnAddEvent(&Txn, i + 1, i + 2, i + 3));
    }

    UtAssert_UINT32_EQ(CFE_TBL_TxnGetEventCount(&Txn), CFE_TBL_MAX_EVENTS_PER_TXN);

    /* Check successful processing of events */
    UT_ResetState(UT_KEY(UT_TBL_TxnEventProcFunc));
    UT_SetDefaultReturnValue(UT_KEY(UT_TBL_TxnEventProcFunc), true);
    UtAssert_UINT32_EQ(CFE_TBL_TxnProcessEvents(&Txn, UT_TBL_TxnEventProcFunc, NULL), CFE_TBL_MAX_EVENTS_PER_TXN);
    UtAssert_STUB_COUNT(UT_TBL_TxnEventProcFunc, CFE_TBL_MAX_EVENTS_PER_TXN);

    /* Check unsuccessful processing of events */
    UT_ResetState(UT_KEY(UT_TBL_TxnEventProcFunc));
    UT_SetDefaultReturnValue(UT_KEY(UT_TBL_TxnEventProcFunc), false);
    UtAssert_UINT32_EQ(CFE_TBL_TxnProcessEvents(&Txn, UT_TBL_TxnEventProcFunc, NULL), 0);
    UtAssert_STUB_COUNT(UT_TBL_TxnEventProcFunc, CFE_TBL_MAX_EVENTS_PER_TXN);

    /* Add another event (overflow) */
    UtAssert_VOIDCALL(CFE_TBL_TxnAddEvent(&Txn, 555, 666, 777));
    UtAssert_UINT32_EQ(CFE_TBL_TxnGetEventCount(&Txn), 1 + CFE_TBL_MAX_EVENTS_PER_TXN);

    /* Check successful processing of events */
    UT_ResetState(UT_KEY(UT_TBL_TxnEventProcFunc));
    UT_SetDefaultReturnValue(UT_KEY(UT_TBL_TxnEventProcFunc), true);
    UtAssert_UINT32_EQ(CFE_TBL_TxnProcessEvents(&Txn, UT_TBL_TxnEventProcFunc, NULL), CFE_TBL_MAX_EVENTS_PER_TXN);
    UtAssert_STUB_COUNT(UT_TBL_TxnEventProcFunc, CFE_TBL_MAX_EVENTS_PER_TXN);

    /* Check clearing of events */
    UtAssert_VOIDCALL(CFE_TBL_TxnClearEvents(&Txn));
    UtAssert_ZERO(CFE_TBL_TxnGetEventCount(&Txn));
}
