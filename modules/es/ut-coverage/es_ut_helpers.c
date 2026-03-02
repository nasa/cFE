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

/*
** File:
**    es_UT.c
**
** Purpose:
**    Executive Services unit test
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
#include "es_ut_helpers.h"
#include "target_config.h"
#include "cfe_config.h"
#include <setjmp.h>

/*
** Global variables
*/

/*
 * Pointer to reset data that will be re-configured/preserved across calls to ES_ResetUnitTest()
 */
CFE_ES_ResetData_t *ES_UT_PersistentResetData = NULL;

CFE_ES_GMP_DirectBuffer_t   UT_MemPoolDirectBuffer;
CFE_ES_GMP_IndirectBuffer_t UT_MemPoolIndirectBuffer;

/*
 * Memory pool block sizes used for unit test
 * The platform config values are not used for UT as the test cases
 * require certain sizes.  A large max block and small min block
 * are needed for testing size thresholds when creating pools.
 */
static const size_t                  UT_MemPoolSizeArray[5] = {131072, 512, 128, 32, 8};
static const CFE_Config_ArrayValue_t UT_MemPoolAV           = {5, UT_MemPoolSizeArray};

/* A jump buffer to imitate a function call that does not return */
jmp_buf OS_TaskDelay_jmp_buf;

/* Create a startup script buffer for a maximum of 5 lines * 80 chars/line */
char StartupScript[MAX_STARTUP_SCRIPT];

/*
** Functions
*/
CFE_ResourceId_t ES_UT_MakeAppIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up AppID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ResourceId_FromInteger(ArrayIdx + CFE_ES_APPID_BASE);
}

CFE_ResourceId_t ES_UT_MakeTaskIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up TaskID values in a manner similar to FSW.
     * Real apps should never do this. */
    uint32 Base;

    /* The base to use depends on whether STRICT mode is enabled or not */
#ifndef CFE_RESOURCEID_STRICT //_MODE
    Base = CFE_ES_TASKID_BASE;
#else
    Base = 0x40010000; /* note this is NOT the same as the normal OSAL task ID base */
#endif

    return CFE_ResourceId_FromInteger(ArrayIdx + Base);
}

CFE_ResourceId_t ES_UT_MakeLibIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up LibID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ResourceId_FromInteger(ArrayIdx + CFE_ES_LIBID_BASE);
}

CFE_ResourceId_t ES_UT_MakeCounterIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up CounterID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ResourceId_FromInteger(ArrayIdx + CFE_ES_COUNTID_BASE);
}

CFE_ResourceId_t ES_UT_MakePoolIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up PoolID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ResourceId_FromInteger(ArrayIdx + CFE_ES_POOLID_BASE);
}

CFE_ResourceId_t ES_UT_MakeCDSIdForIndex(uint32 ArrayIdx)
{
    /* UT hack - make up CDSID values in a manner similar to FSW.
     * Real apps should never do this. */
    return CFE_ResourceId_FromInteger(ArrayIdx + CFE_ES_CDSBLOCKID_BASE);
}

/*
 * A local stub that can serve as the user function for testing ES tasks
 */
void ES_UT_TaskFunction(void)
{
    UT_DEFAULT_IMPL(ES_UT_TaskFunction);
}

/* Local function to test CFE_ES_SysLog_vsnprintf */
void ES_UT_SysLog_snprintf(char *Buffer, size_t BufferSize, const char *SpecStringPtr, ...)
{
    va_list ap;

    va_start(ap, SpecStringPtr);
    CFE_ES_SysLog_vsnprintf(Buffer, BufferSize, SpecStringPtr, ap);
    va_end(ap);
}

void ES_UT_FillBuffer(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    char * PrintBuffer = UT_Hook_GetArgValueByName(Context, "PrintBuffer", char *);
    uint32 Size        = *((uint32 *)UserObj);

    memset(PrintBuffer, ' ', Size - 1);
    PrintBuffer[Size - 1] = 0;
}

/*
 * Helper function to assemble basic bits of info into the "CFE_ES_ModuleLoadParams_t" struct
 */
void ES_UT_SetupModuleLoadParams(CFE_ES_ModuleLoadParams_t *Params, const char *FileName, const char *EntryName)
{
    char Empty = 0;

    if (FileName == NULL)
    {
        FileName = &Empty;
    }

    if (EntryName == NULL)
    {
        EntryName = &Empty;
    }

    strncpy(Params->FileName, FileName, sizeof(Params->FileName));
    strncpy(Params->InitSymbolName, EntryName, sizeof(Params->InitSymbolName));
}

/*
 * Helper function to assemble basic bits of info into the "CFE_ES_AppStartParams_t" struct
 */
void ES_UT_SetupAppStartParams(CFE_ES_AppStartParams_t *Params, const char *FileName, const char *EntryName,
                               size_t StackSize, CFE_ES_TaskPriority_Atom_t Priority,
                               CFE_ES_ExceptionAction_Enum_t ExceptionAction)
{
    ES_UT_SetupModuleLoadParams(&Params->BasicInfo, FileName, EntryName);
    Params->MainTaskInfo.StackSize = StackSize;
    Params->MainTaskInfo.Priority  = Priority;
    Params->ExceptionAction        = ExceptionAction;
}

/*
 * Helper function to setup a single app ID in the given state, along with
 * a main task ID.  A pointer to the App and Task record is output so the
 * record can be modified
 */
void ES_UT_SetupSingleAppId(CFE_ES_AppType_Enum_t AppType, CFE_ES_AppState_Enum_t AppState, const char *AppName,
                            CFE_ES_AppRecord_t **OutAppRec, CFE_ES_TaskRecord_t **OutTaskRec)
{
    osal_id_t            UtOsalId = OS_OBJECT_ID_UNDEFINED;
    CFE_ResourceId_t     UtTaskId;
    CFE_ResourceId_t     UtAppId;
    CFE_ES_AppRecord_t * LocalAppPtr;
    CFE_ES_TaskRecord_t *LocalTaskPtr;

    OS_TaskCreate(&UtOsalId, "UT", NULL, OSAL_TASK_STACK_ALLOCATE, 0, 0, 0);
    UtTaskId                = CFE_RESOURCEID_UNWRAP(CFE_ES_TaskId_FromOSAL(UtOsalId));
    UtAppId                 = CFE_ES_Global.LastAppId;
    CFE_ES_Global.LastAppId = CFE_ResourceId_FromInteger(CFE_ResourceId_ToInteger(UtAppId) + 1);

    LocalTaskPtr = CFE_ES_LocateTaskRecordByID(CFE_ES_TASKID_C(UtTaskId));
    LocalAppPtr  = CFE_ES_LocateAppRecordByID(CFE_ES_APPID_C(UtAppId));
    CFE_ES_TaskRecordSetUsed(LocalTaskPtr, UtTaskId);
    CFE_ES_AppRecordSetUsed(LocalAppPtr, UtAppId);
    LocalTaskPtr->AppId     = CFE_ES_AppRecordGetID(LocalAppPtr);
    LocalAppPtr->MainTaskId = CFE_ES_TaskRecordGetID(LocalTaskPtr);
    LocalAppPtr->AppState   = AppState;
    LocalAppPtr->Type       = AppType;

    if (AppName)
    {
        strncpy(LocalAppPtr->AppName, AppName, sizeof(LocalAppPtr->AppName) - 1);
        LocalAppPtr->AppName[sizeof(LocalAppPtr->AppName) - 1] = 0;
        strncpy(LocalTaskPtr->TaskName, AppName, sizeof(LocalTaskPtr->TaskName) - 1);
        LocalTaskPtr->TaskName[sizeof(LocalTaskPtr->TaskName) - 1] = 0;
    }

    if (OutAppRec)
    {
        *OutAppRec = LocalAppPtr;
    }
    if (OutTaskRec)
    {
        *OutTaskRec = LocalTaskPtr;
    }

    if (AppType == CFE_ES_AppType_CORE)
    {
        ++CFE_ES_Global.RegisteredCoreApps;
    }
    if (AppType == CFE_ES_AppType_EXTERNAL)
    {
        ++CFE_ES_Global.RegisteredExternalApps;

        OS_ModuleLoad(&UtOsalId, NULL, NULL, 0);
        LocalAppPtr->LoadStatus.ModuleId = UtOsalId;
    }
    ++CFE_ES_Global.RegisteredTasks;
}

/*
 * Helper function to setup a child task ID associated with the given
 * app record.  A pointer to the Task record is output so the record
 * can be modified
 */
void ES_UT_SetupChildTaskId(const CFE_ES_AppRecord_t *ParentApp, const char *TaskName, CFE_ES_TaskRecord_t **OutTaskRec)
{
    osal_id_t            UtOsalId = OS_OBJECT_ID_UNDEFINED;
    CFE_ES_TaskId_t      UtTaskId;
    CFE_ES_AppId_t       UtAppId;
    CFE_ES_TaskRecord_t *LocalTaskPtr;

    UtAppId = CFE_ES_AppRecordGetID(ParentApp);

    OS_TaskCreate(&UtOsalId, "C", NULL, OSAL_TASK_STACK_ALLOCATE, 0, 0, 0);
    UtTaskId = CFE_ES_TaskId_FromOSAL(UtOsalId);

    LocalTaskPtr = CFE_ES_LocateTaskRecordByID(UtTaskId);
    CFE_ES_TaskRecordSetUsed(LocalTaskPtr, CFE_RESOURCEID_UNWRAP(UtTaskId));
    LocalTaskPtr->AppId = UtAppId;

    if (TaskName)
    {
        strncpy(LocalTaskPtr->TaskName, TaskName, sizeof(LocalTaskPtr->TaskName) - 1);
        LocalTaskPtr->TaskName[sizeof(LocalTaskPtr->TaskName) - 1] = 0;
    }

    if (OutTaskRec)
    {
        *OutTaskRec = LocalTaskPtr;
    }

    ++CFE_ES_Global.RegisteredTasks;
}

/*
 * Helper function to setup a single Lib ID.  A pointer to the Lib
 * record is output so the record can be modified
 */
void ES_UT_SetupSingleLibId(const char *LibName, CFE_ES_LibRecord_t **OutLibRec)
{
    CFE_ResourceId_t    UtLibId;
    CFE_ES_LibRecord_t *LocalLibPtr;

    UtLibId                 = CFE_ES_Global.LastLibId;
    CFE_ES_Global.LastLibId = CFE_ResourceId_FromInteger(CFE_ResourceId_ToInteger(UtLibId) + 1);

    LocalLibPtr = CFE_ES_LocateLibRecordByID(CFE_ES_LIBID_C(UtLibId));
    CFE_ES_LibRecordSetUsed(LocalLibPtr, UtLibId);

    if (LibName)
    {
        strncpy(LocalLibPtr->LibName, LibName, sizeof(LocalLibPtr->LibName) - 1);
        LocalLibPtr->LibName[sizeof(LocalLibPtr->LibName) - 1] = 0;
    }

    if (OutLibRec)
    {
        *OutLibRec = LocalLibPtr;
    }

    ++CFE_ES_Global.RegisteredLibs;
}

int32 ES_UT_PoolDirectRetrieve(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr)
{
    *BdPtr = (CFE_ES_GenPoolBD_t *)((void *)&UT_MemPoolDirectBuffer.Data[Offset]);
    return CFE_SUCCESS;
}

int32 ES_UT_PoolDirectCommit(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr)
{
    return CFE_SUCCESS;
}

int32 ES_UT_PoolIndirectRetrieve(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr)
{
    memcpy(&UT_MemPoolIndirectBuffer.BD, &UT_MemPoolIndirectBuffer.Data[Offset], sizeof(CFE_ES_GenPoolBD_t));
    *BdPtr = &UT_MemPoolIndirectBuffer.BD;
    return CFE_SUCCESS;
}

int32 ES_UT_PoolIndirectCommit(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr)
{
    memcpy(&UT_MemPoolIndirectBuffer.Data[Offset], BdPtr, sizeof(CFE_ES_GenPoolBD_t));
    return CFE_SUCCESS;
}

int32 ES_UT_CDSPoolRetrieve(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr)
{
    static CFE_ES_GenPoolBD_t BdBuf;

    *BdPtr = &BdBuf;
    return CFE_PSP_ReadFromCDS(&BdBuf, Offset, sizeof(BdBuf));
}

int32 ES_UT_CDSPoolCommit(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr)
{
    return CFE_PSP_WriteToCDS(BdPtr, Offset, sizeof(*BdPtr));
}

/* Commit failure routine for pool coverage testing */
int32 ES_UT_PoolCommitFail(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr)
{
    return CFE_ES_CDS_ACCESS_ERROR;
}

/* Retrieve failure routine for pool coverage testing */
int32 ES_UT_PoolRetrieveFail(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr)
{
    return CFE_ES_CDS_ACCESS_ERROR;
}

void ES_UT_SetupMemPoolId(CFE_ES_MemPoolRecord_t **OutPoolRecPtr)
{
    CFE_ResourceId_t        UtPoolID;
    CFE_ES_MemPoolRecord_t *LocalPoolRecPtr;

    UtPoolID                    = CFE_ES_Global.LastMemPoolId;
    CFE_ES_Global.LastMemPoolId = CFE_ResourceId_FromInteger(CFE_ResourceId_ToInteger(UtPoolID) + 1);

    LocalPoolRecPtr = CFE_ES_LocateMemPoolRecordByID(CFE_ES_MEMHANDLE_C(UtPoolID));

    /* in order to validate the size must be nonzero */
    LocalPoolRecPtr->Pool.PoolTotalSize        = sizeof(UT_MemPoolDirectBuffer.Data);
    LocalPoolRecPtr->Pool.PoolMaxOffset        = sizeof(UT_MemPoolDirectBuffer.Data);
    LocalPoolRecPtr->Pool.Buckets[0].BlockSize = 16;
    LocalPoolRecPtr->Pool.NumBuckets           = 1;
    LocalPoolRecPtr->Pool.Retrieve             = ES_UT_PoolDirectRetrieve;
    LocalPoolRecPtr->Pool.Commit               = ES_UT_PoolDirectCommit;
    LocalPoolRecPtr->BaseAddr                  = (cpuaddr)UT_MemPoolDirectBuffer.Data;
    OS_MutSemCreate(&LocalPoolRecPtr->MutexId, NULL, 0);

    CFE_ES_MemPoolRecordSetUsed(LocalPoolRecPtr, UtPoolID);

    if (OutPoolRecPtr)
    {
        *OutPoolRecPtr = LocalPoolRecPtr;
    }
}

void ES_UT_SetupCDSGlobal(size_t CDS_Size)
{
    CFE_ES_CDS_Instance_t *CDS = &CFE_ES_Global.CDSVars;

    UT_SetCDSSize(CDS_Size);

    if (CDS_Size > CDS_RESERVED_MIN_SIZE)
    {
        OS_MutSemCreate(&CDS->GenMutex, "UT", 0);
        CDS->TotalSize = CDS_Size;
        CDS->DataSize  = CDS->TotalSize;
        CDS->DataSize -= CDS_RESERVED_MIN_SIZE;

        CFE_ES_InitCDSSignatures();
        CFE_ES_CreateCDSPool(CDS->DataSize, CDS_POOL_OFFSET);
        CFE_ES_InitCDSRegistry();

        CFE_ES_Global.CDSIsAvailable = true;
    }
}

void ES_UT_SetupSingleCDSRegistry(const char *CDSName, size_t BlockSize, bool IsTable, CFE_ES_CDS_RegRec_t **OutRegRec)
{
    CFE_ES_CDS_RegRec_t *LocalRegRecPtr;
    CFE_ResourceId_t     UtCDSID;
    CFE_ES_GenPoolBD_t   LocalBD;
    size_t               UT_CDS_BufferSize;

    /* first time this is done, set up the global */
    if (!CFE_ES_Global.CDSIsAvailable)
    {
        UT_GetDataBuffer(UT_KEY(CFE_PSP_GetCDSSize), NULL, &UT_CDS_BufferSize, NULL);
        if (UT_CDS_BufferSize > (2 * CFE_ES_CDS_SIGNATURE_LEN))
        {
            /* Use the CDS buffer from ut_support.c if it was configured */
            CFE_ES_Global.CDSVars.Pool.PoolMaxOffset = UT_CDS_BufferSize - CFE_ES_CDS_SIGNATURE_LEN;
            CFE_ES_Global.CDSVars.Pool.Retrieve      = ES_UT_CDSPoolRetrieve;
            CFE_ES_Global.CDSVars.Pool.Commit        = ES_UT_CDSPoolCommit;
        }
        else
        {
            CFE_ES_Global.CDSVars.Pool.PoolMaxOffset = sizeof(UT_MemPoolIndirectBuffer.Data);
            CFE_ES_Global.CDSVars.Pool.Retrieve      = ES_UT_PoolIndirectRetrieve;
            CFE_ES_Global.CDSVars.Pool.Commit        = ES_UT_PoolIndirectCommit;
        }

        CFE_ES_Global.CDSVars.Pool.Buckets[0].BlockSize = ES_UT_CDS_BLOCK_SIZE;
        CFE_ES_Global.CDSVars.Pool.NumBuckets           = 1;
        CFE_ES_Global.CDSVars.Pool.TailPosition         = CFE_ES_CDS_SIGNATURE_LEN;
        CFE_ES_Global.CDSVars.Pool.PoolTotalSize =
            CFE_ES_Global.CDSVars.Pool.PoolMaxOffset - CFE_ES_Global.CDSVars.Pool.TailPosition;

        CFE_ES_Global.CDSIsAvailable = true;
    }

    UtCDSID                              = CFE_ES_Global.CDSVars.LastCDSBlockId;
    CFE_ES_Global.CDSVars.LastCDSBlockId = CFE_ResourceId_FromInteger(CFE_ResourceId_ToInteger(UtCDSID) + 1);

    LocalRegRecPtr = CFE_ES_LocateCDSBlockRecordByID(CFE_ES_CDSHANDLE_C(UtCDSID));
    if (CDSName != NULL)
    {
        strncpy(LocalRegRecPtr->Name, CDSName, sizeof(LocalRegRecPtr->Name) - 1);
        LocalRegRecPtr->Name[sizeof(LocalRegRecPtr->Name) - 1] = 0;
    }
    else
    {
        LocalRegRecPtr->Name[0] = 0;
    }

    LocalRegRecPtr->Table       = IsTable;
    LocalRegRecPtr->BlockOffset = CFE_ES_Global.CDSVars.Pool.TailPosition + sizeof(LocalBD);
    LocalRegRecPtr->BlockSize   = BlockSize;

    LocalBD.CheckBits  = CFE_ES_CHECK_PATTERN;
    LocalBD.Allocated  = CFE_ES_MEMORY_ALLOCATED + 1;
    LocalBD.ActualSize = BlockSize;
    LocalBD.NextOffset = 0;
    CFE_ES_Global.CDSVars.Pool.Commit(&CFE_ES_Global.CDSVars.Pool, CFE_ES_Global.CDSVars.Pool.TailPosition, &LocalBD);

    CFE_ES_Global.CDSVars.Pool.TailPosition = LocalRegRecPtr->BlockOffset + LocalRegRecPtr->BlockSize;

    CFE_ES_CDSBlockRecordSetUsed(LocalRegRecPtr, UtCDSID);

    if (OutRegRec)
    {
        *OutRegRec = LocalRegRecPtr;
    }
}

int32 ES_UT_SetupOSCleanupHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    osal_id_t ObjList[8];

    /* On the first call, Use the stub functions to generate one object of
     * each type
     */
    if (CallCount == 0)
    {
        /* Initialize to avoid static analysis warnings */
        memset(ObjList, 0, sizeof(ObjList));

        OS_TaskCreate(&ObjList[0], NULL, NULL, OSAL_TASK_STACK_ALLOCATE, 0, 0, 0);
        OS_QueueCreate(&ObjList[1], NULL, 0, 0, 0);
        OS_MutSemCreate(&ObjList[2], NULL, 0);
        OS_BinSemCreate(&ObjList[3], NULL, 0, 0);
        OS_CountSemCreate(&ObjList[4], NULL, 0, 0);
        OS_TimerCreate(&ObjList[5], NULL, NULL, NULL);
        OS_OpenCreate(&ObjList[6], NULL, 0, 0);
        OS_ModuleLoad(&ObjList[7], NULL, NULL, 0);

        UT_SetDataBuffer((UT_EntryKey_t)&OS_ForEachObject, ObjList, sizeof(ObjList), true);
    }

    return StubRetcode;
}

void ES_UT_SetupForOSCleanup(void)
{
    UT_SetHookFunction(UT_KEY(OS_ForEachObject), ES_UT_SetupOSCleanupHook, NULL);
}

int32 ES_UT_SetAppStateHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    ES_UT_SetAppStateHook_t *StateHook = UserObj;
    uint32                   i;
    CFE_ES_AppRecord_t *     AppRecPtr;

    AppRecPtr = CFE_ES_Global.AppTable;
    for (i = 0; i < CFE_PLATFORM_ES_MAX_APPLICATIONS; ++i)
    {
        if (CFE_ES_AppRecordIsUsed(AppRecPtr))
        {
            /* If no filter object supplied, set all apps to RUNNING */
            if (StateHook == NULL)
            {
                AppRecPtr->AppState = CFE_ES_AppState_RUNNING;
            }
            else if (StateHook->AppType == 0 || AppRecPtr->Type == StateHook->AppType)
            {
                AppRecPtr->AppState = StateHook->AppState;
            }
        }
        ++AppRecPtr;
    }

    return StubRetcode;
}

void UT_ArrayConfigHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_Config_ArrayValue_t Val = *((const CFE_Config_ArrayValue_t *)UserObj);
    UT_Stub_SetReturnValue(FuncKey, Val);
}

/* See es_UT.h for more info */
void ES_UT_TaskDelay_Hook(void *UserObj)
{
    /* Set jmp_val to 1 so we don't redo the CFE_ES_ExitApp call.
     * (See last CFE_ES_ExitApp test case below) */
    longjmp(OS_TaskDelay_jmp_buf, 1);
}

int32 ES_UT_TaskCreate_Hook(void *UserObj)
{
    /* Simulate OS_TaskCreate error */
    return OS_ERROR;
}

int32 ES_UT_ModuleUnload_Hook(void *UserObj)
{
    /* Simulate module unloading error */
    return OS_ERROR;
}

/*
** Reset variable values prior to a test
*/
void ES_ResetUnitTest(void)
{
    UT_InitData();

    memset(&CFE_ES_Global, 0, sizeof(CFE_ES_Global));

    /*
    ** Initialize the Last Id
    */
    CFE_ES_Global.LastAppId              = CFE_ResourceId_FromInteger(CFE_ES_APPID_BASE);
    CFE_ES_Global.LastLibId              = CFE_ResourceId_FromInteger(CFE_ES_LIBID_BASE);
    CFE_ES_Global.LastCounterId          = CFE_ResourceId_FromInteger(CFE_ES_COUNTID_BASE);
    CFE_ES_Global.LastMemPoolId          = CFE_ResourceId_FromInteger(CFE_ES_POOLID_BASE);
    CFE_ES_Global.CDSVars.LastCDSBlockId = CFE_ResourceId_FromInteger(CFE_ES_CDSBLOCKID_BASE);

    /*
     * (Re-)Initialize the reset data pointer
     * This was formerly a separate global, but now part of CFE_ES_Global.
     *
     * Some unit tests assume/rely on it preserving its value across tests,
     * so it must be re-initialized here every time CFE_ES_Global is reset.
     */
    CFE_ES_Global.ResetDataPtr = ES_UT_PersistentResetData;

    UT_SetHandlerFunction(UT_KEY(CFE_Config_GetArrayValue), UT_ArrayConfigHandler, (void *)&UT_MemPoolAV);

} /* end ES_ResetUnitTest() */
