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
 *  Define helper functions to work with table services unit tests
 */

/*
** Includes
*/
#include "tbl_UT.h"
#include "tbl_ut_helpers.h"
#include "cfe_config.h"

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

void UT_TBL_SetupSingleRegWithSize(CFE_TBL_RegistryRec_t **RegRecOut, CFE_TBL_AccessDescriptor_t **AccDescOut,
                                   size_t TblSz, uint16 Flags)
{
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_Handle_t            Handle;
    static uint32               Num = 0;
    char                        TableName[16];

    ++Num;
    snprintf(TableName, sizeof(TableName), "ut%03u", (unsigned int)Num);

    UT_TBL_SetupCodec(TblSz);

    CFE_UtAssert_SETUP(CFE_TBL_Register(&Handle, TableName, TblSz, Flags, NULL));

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

void UT_TBL_SetupSingleReg(CFE_TBL_RegistryRec_t **RegRecOut, CFE_TBL_AccessDescriptor_t **AccDescOut, uint16 Flags)
{
    UT_TBL_SetupSingleRegWithSize(RegRecOut, AccDescOut, sizeof(UT_Table1_t), Flags);
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
        if (CFE_TBL_LOADBUFFID_IS_VALID(RegRecPtr->Status.NextBufferId))
        {
            DumpCtrlPtr->SourceBuffId = RegRecPtr->Status.NextBufferId;
        }
        else
        {
            DumpCtrlPtr->SourceBuffId = RegRecPtr->Status.ActiveBufferId;
        }
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
void UT_TBL_SetName(char *BufPtr, size_t BufSz, const char *Src)
{
    if (Src != NULL)
    {
        strncpy(BufPtr, Src, BufSz - 1);
        BufPtr[BufSz - 1] = '\0';
    }
    else
    {
        memset(BufPtr, 0, BufSz);
    }
}

void UT_ArrayConfigHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_Config_ArrayValue_t Val = *((const CFE_Config_ArrayValue_t *)UserObj);
    UT_Stub_SetReturnValue(FuncKey, Val);
}

void UT_TBL_SetDumpOnly(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    UT_TBL_Config(RegRecPtr)->DumpOnly = true;
}

void UT_TBL_ClearLoadPending(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    UT_TBL_Status(RegRecPtr)->NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
}

void UT_TBL_SetLoadBuffTaken(CFE_TBL_LoadBuff_t *LoadBuffPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
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

void UT_TBL_SetLoadBuffFree(CFE_TBL_LoadBuff_t *LoadBuffPtr)
{
    CFE_TBL_LoadBuffSetFree(LoadBuffPtr);
}

void UT_TBL_SetHandleUsed(CFE_TBL_AccessDescriptor_t *AccDescPtr)
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

void UT_TBL_LoadBuffFindNextHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    /*
    CFE_ResourceId_t CFE_ResourceId_FindNextEx(CFE_ResourceId_t StartId, CFE_ResourceId_IncrementFunc_t IncrFunc,
        void *IncrArg, CFE_ResourceId_CheckFunc_t CheckFunc);
    */
    CFE_ResourceId_t                 StartId = UT_Hook_GetArgValueByName(Context, "StartId", CFE_ResourceId_t);
    CFE_TBL_LoadBuffFindNextState_t *State   = UT_Hook_GetArgValueByName(Context, "IncrArg", void *);

    CFE_ResourceId_t CheckId;

    /* Only do the loop if the caller did not set a return value already */
    if (!UT_Stub_GetInt32StatusCode(Context, NULL))
    {
        CheckId = StartId;
        while (true)
        {
            /* Yes, this will simply call the "real" impl from the stub.  The real thing should be implemented
             * per the contract and there is no sense in duplicating it in UT with some other impl that does not
             * meet the required contract. */
            CheckId = CFE_TBL_LoadBuffIncrementSerial(CheckId, State);
            if (!CFE_ResourceId_IsDefined(CheckId))
            {
                break;
            }
            if (!CFE_TBL_CheckLoadBuffSlotUsed(CheckId))
            {
                break;
            }
        }

        UT_Stub_SetReturnValue(FuncKey, CheckId);
    }
}

void UT_TBL_GlobalDataReset(void)
{
    memset(&CFE_TBL_Global, 0, sizeof(CFE_TBL_Global));

    UT_ResetPoolBufferIndex();
    UtAssert_VOIDCALL(CFE_TBL_EarlyInit());
    UtAssert_VOIDCALL(CFE_TBL_InitData());

    CFE_TBL_Global.TableTaskAppId = UT_TBL_APPID_10;
}

/* Add custom logic to cFE common UT_InitData */
void UT_InitData_TBL(void)
{
    UT_InitData();

    /* For load buffer ID allocation, the standard handler for CFE_ResourceId_FindNextEx
     * does not handle the wrap-around (it is the same handler as for CFE_ResourceId_FindNext).
     * This is a case where the function really has to work as the description says it works,
     * or else the wrong thing happens.  This is a problem only because it is stubbed out here.
     * By registering a handler that makes it work a little more like the real thing, this issue
     * can be avoided.  Note that TBL currently only uses the Ex variant for load buffers.
     */
    UT_SetHandlerFunction(UT_KEY(CFE_ResourceId_FindNextEx), UT_TBL_LoadBuffFindNextHandler, NULL);
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
