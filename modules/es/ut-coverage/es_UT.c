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
#include "es_UT.h"

#define ES_UT_CDS_BLOCK_SIZE 16

/*
 * A size which meets the minimum CDS size
 * requirements for the implementation, but
 * not much larger.
 */
#define ES_UT_CDS_SMALL_TEST_SIZE (56 * 1024)

/*
 * A size which has room for actual allocations
 */
#define ES_UT_CDS_LARGE_TEST_SIZE (128 * 1024)

extern CFE_ES_Global_t CFE_ES_Global;

extern int32 dummy_function(void);

/*
** Global variables
*/

/*
 * Pointer to reset data that will be re-configured/preserved across calls to ES_ResetUnitTest()
 */
static CFE_ES_ResetData_t *ES_UT_PersistentResetData = NULL;

/* Buffers to support memory pool testing */
typedef union
{
    CFE_ES_PoolAlign_t Align; /* make aligned */
    uint8              Data[300];
} CFE_ES_GMP_DirectBuffer_t;

typedef struct
{
    CFE_ES_GenPoolBD_t BD;
    CFE_ES_PoolAlign_t Align;                                        /* make aligned */
    uint8              Spare;                                        /* make unaligned */
    uint8              Data[(sizeof(CFE_ES_GenPoolBD_t) * 4) + 157]; /* oddball size */
} CFE_ES_GMP_IndirectBuffer_t;

CFE_ES_GMP_DirectBuffer_t   UT_MemPoolDirectBuffer;
CFE_ES_GMP_IndirectBuffer_t UT_MemPoolIndirectBuffer;

/* Create a startup script buffer for a maximum of 5 lines * 80 chars/line */
char StartupScript[MAX_STARTUP_SCRIPT];

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_NOOP_CC = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
                                                                   .CommandCode = CFE_ES_NOOP_CC};

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_RESET_COUNTERS_CC};

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESTART_CC = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
                                                                      .CommandCode = CFE_ES_RESTART_CC};

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_START_APP_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_START_APP_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_STOP_APP_CC = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
                                                                       .CommandCode = CFE_ES_STOP_APP_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESTART_APP_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_RESTART_APP_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RELOAD_APP_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_RELOAD_APP_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ONE_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_QUERY_ONE_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ALL_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_QUERY_ALL_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_QUERY_ALL_TASKS_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_CLEAR_SYSLOG_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_CLEAR_SYSLOG_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_WRITE_SYSLOG_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_OVER_WRITE_SYSLOG_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_CLEAR_ER_LOG_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_WRITE_ER_LOG_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_START_PERF_DATA_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_STOP_PERF_DATA_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_SET_PERF_FILTER_MASK_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_SET_PERF_TRIGGER_MASK_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_RESET_PR_COUNT_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_SET_MAX_PR_COUNT_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_DELETE_CDS_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_DELETE_CDS_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_SEND_MEM_POOL_STATS_CC};
static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC = {
    .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID), .CommandCode = CFE_ES_DUMP_CDS_REGISTRY_CC};

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_INVALID_CC = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_CMD_MID),
                                                                      .CommandCode = CFE_ES_DUMP_CDS_REGISTRY_CC + 2};

static const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_SEND_HK = {.MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_ES_SEND_HK_MID)};

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
    osal_id_t            UtOsalId;
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
    osal_id_t            UtOsalId;
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

void ES_UT_SetupCDSGlobal(uint32 CDS_Size)
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
    osal_id_t ObjList[7];

    /* On the first call, Use the stub functions to generate one object of
     * each type
     */
    if (CallCount == 0)
    {
        OS_TaskCreate(&ObjList[0], NULL, NULL, OSAL_TASK_STACK_ALLOCATE, 0, 0, 0);
        OS_QueueCreate(&ObjList[1], NULL, 0, 0, 0);
        OS_MutSemCreate(&ObjList[2], NULL, 0);
        OS_BinSemCreate(&ObjList[3], NULL, 0, 0);
        OS_CountSemCreate(&ObjList[4], NULL, 0, 0);
        OS_TimerCreate(&ObjList[5], NULL, NULL, NULL);
        OS_OpenCreate(&ObjList[6], NULL, 0, 0);

        UT_SetDataBuffer((UT_EntryKey_t)&OS_ForEachObject, ObjList, sizeof(ObjList), true);
    }

    return StubRetcode;
}

static void ES_UT_SetupForOSCleanup(void)
{
    UT_SetHookFunction(UT_KEY(OS_ForEachObject), ES_UT_SetupOSCleanupHook, NULL);
}

typedef struct
{
    uint32 AppType;
    uint32 AppState;
} ES_UT_SetAppStateHook_t;

static int32 ES_UT_SetAppStateHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
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

void UtTest_Setup(void)
{
    UT_Init("es");
    UtPrintf("cFE ES Unit Test Output File\n\n");

    UT_ADD_TEST(TestInit);
    UT_ADD_TEST(TestStartupErrorPaths);
    UT_ADD_TEST(TestResourceID);
    UT_ADD_TEST(TestApps);
    UT_ADD_TEST(TestLibs);
    UT_ADD_TEST(TestERLog);
    UT_ADD_TEST(TestTask);
    UT_ADD_TEST(TestPerf);
    UT_ADD_TEST(TestAPI);
    UT_ADD_TEST(TestGenericCounterAPI);
    UT_ADD_TEST(TestCDS);
    UT_ADD_TEST(TestGenericPool);
    UT_ADD_TEST(TestCDSMempool);
    UT_ADD_TEST(TestESMempool);
    UT_ADD_TEST(TestSysLog);
    UT_ADD_TEST(TestBackground);
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
     * so is must be re-initialized here every time CFE_ES_Global is reset.
     */
    CFE_ES_Global.ResetDataPtr = ES_UT_PersistentResetData;

} /* end ES_ResetUnitTest() */

void TestInit(void)
{
    UtPrintf("Begin Test Init");

    UT_SetCDSSize(128 * 1024);
    UT_SetSizeofESResetArea(sizeof(CFE_ES_ResetData_t));

    /* Set up the reset area */
    UT_SetStatusBSPResetArea(OS_SUCCESS, CFE_TIME_RESET_SIGNATURE, CFE_TIME_ToneSignalSelect_PRIMARY);

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            sizeof(StartupScript) - 1);
    StartupScript[sizeof(StartupScript) - 1] = '\0';
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));

    /* Go through ES_Main and cover normal paths */
    UT_SetDummyFuncRtn(OS_SUCCESS);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1, "ut_startup");
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 0, "CFE_ES_Main", "Normal startup");
}

void TestStartupErrorPaths(void)
{
    int                     j;
    ES_UT_SetAppStateHook_t StateHook;
    uint32                  PanicStatus;
    uint32                  ResetType;
    OS_statvfs_t            StatBuf;
    CFE_ES_TaskRecord_t *   TaskRecPtr;
    CFE_ES_AppRecord_t *    AppRecPtr;

    UtPrintf("Begin Test Startup Error Paths");

    /*
     * Get the reference to the reset area and save it so it will be preserved
     * across calls to ES_ResetUnitTest().  This is required since now the pointer
     * is part of CFE_ES_Global which is zeroed out as part of test reset.  Formerly
     * this was a separate global which was not cleared with the other globals.
     */
    UT_GetDataBuffer(UT_KEY(CFE_PSP_GetResetArea), (void **)&ES_UT_PersistentResetData, NULL, NULL);

    /* Set up the startup script for reading */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            sizeof(StartupScript) - 1);
    StartupScript[sizeof(StartupScript) - 1] = '\0';

    /* Perform ES main startup with a mutex creation failure */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_MutSemCreate), OS_ERROR);
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Panic), &PanicStatus, sizeof(PanicStatus), false);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, 1, 1, "ut_startup");
    UT_Report(__FILE__, __LINE__,
              PanicStatus == CFE_PSP_PANIC_STARTUP_SEM && UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 1, "CFE_ES_Main",
              "Mutex create failure");

    /* Perform ES main startup with a file open failure */
    ES_ResetUnitTest();
    UT_SetDummyFuncRtn(OS_SUCCESS);
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, 1, 1, "ut_startup");
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_OPEN_ES_APP_STARTUP]),
              "CFE_ES_Main", "File open failure");

    /* Perform ES main startup with a startup sync failure */
    ES_ResetUnitTest();
    StateHook.AppState = CFE_ES_AppState_RUNNING;
    StateHook.AppType =
        CFE_ES_AppType_CORE; /* by only setting core apps, it will appear as if external apps did not start */
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, &StateHook);
    UT_SetReadBuffer(StartupScript, strlen(StartupScript));
    CFE_ES_Main(CFE_PSP_RST_TYPE_POWERON, 1, 1, "ut_startup");
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_STARTUP_SYNC_FAIL_1]), "CFE_ES_Main",
              "Startup sync failure 1");
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_STARTUP_SYNC_FAIL_2]), "CFE_ES_Main",
              "Startup sync failure 2");

    /* Perform a power on reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_POWERON, CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_HW_SPECIAL]),
              "CFE_ES_SetupResetVariables", "Power on reset - Hardware special command");

    /* Perform a processor reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_PROC_RESET_MAX_HW_SPECIAL]),
              "CFE_ES_SetupResetVariables", "Processor reset - hardware special command");

    /* Perform a power on reset with an "other cause" sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_POWERON, -1, 1);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_OTHER]), "CFE_ES_SetupResetVariables",
              "Other cause reset");

    /* Perform the maximum number of processor resets */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CFE_ES_Global.ResetDataPtr->ResetVars.ES_CausedReset      = false;

    for (j = 0; j < CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS; j++)
    {
        CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    }

    UT_Report(__FILE__, __LINE__,
              CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount == CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS,
              "CFE_ES_SetupResetVariables", "Maximum processor resets");

    /* Attempt another processor reset after the maximum have occurred */
    ES_ResetUnitTest();
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Restart), &ResetType, sizeof(ResetType), false);
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount == CFE_PLATFORM_ES_MAX_PROCESSOR_RESETS + 1 &&
                  ResetType == CFE_PSP_RST_TYPE_POWERON && UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1,
              "CFE_ES_SetupResetVariables",
              "Processor reset - power cycle; exceeded maximum "
              "processor resets");

    /* Perform a power on reset with a hardware special sub-type */
    ES_ResetUnitTest();
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_MAX_HW_SPECIAL]),
              "CFE_ES_SetupResetVariables",
              "Processor reset - hardware special command; exceeded maximum "
              "processor resets");

    /* Perform a processor reset with an reset area failure */
    ES_ResetUnitTest();
    UT_SetStatusBSPResetArea(OS_ERROR, 0, CFE_TIME_ToneSignalSelect_PRIMARY);
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Panic), &PanicStatus, sizeof(PanicStatus), false);
    CFE_ES_Global.ResetDataPtr->ResetVars.ES_CausedReset = true;
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              PanicStatus == CFE_PSP_PANIC_MEMORY_ALLOC && UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 1,
              "CFE_ES_SetupResetVariables", "Get reset area error");

    /* Perform a processor reset triggered by ES */
    /* Added for coverage, as the "panic" case will should not cover this one */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->ResetVars.ES_CausedReset = true;
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 0, "CFE_ES_SetupResetVariables",
              "Processor Reset caused by ES");

    /* Perform a processor reset with the size of the reset area too small */
    ES_ResetUnitTest();
    UT_SetSizeofESResetArea(0);
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Panic), &PanicStatus, sizeof(PanicStatus), false);
    UT_SetStatusBSPResetArea(OS_SUCCESS, 0, CFE_TIME_ToneSignalSelect_PRIMARY);
    CFE_ES_SetupResetVariables(CFE_PSP_RST_TYPE_PROCESSOR, CFE_PSP_RST_SUBTYPE_POWER_CYCLE, 1);
    UT_Report(__FILE__, __LINE__,
              PanicStatus == CFE_PSP_PANIC_MEMORY_ALLOC && UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 1,
              "CFE_ES_SetupResetVariables", "Reset area too small");

    /* Test initialization of the file systems specifying a power on reset
     * following a failure to create the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_initfs), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_mount), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_mkfs), OS_ERROR);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_POWERON);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CREATE_VOLATILE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]),
              "CFE_ES_InitializeFileSystems", "Power on reset; error creating volatile (RAM) volume");

    /* prepare the StatBuf to reflect a RAM disk that is 99% full */
    StatBuf.block_size   = 1024;
    StatBuf.total_blocks = CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS;
    StatBuf.blocks_free  = CFE_PLATFORM_ES_RAM_DISK_NUM_SECTORS / 100;

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to reformat the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_initfs), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_mount), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_mkfs), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(OS_FileSysStatVolume), &StatBuf, sizeof(StatBuf), false);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CREATE_VOLATILE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INIT_VOLATILE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REFORMAT_VOLATILE]),
              "CFE_ES_InitializeFileSystems", "Processor reset; error reformatting volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following failure to get the volatile disk memory
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_GetVolatileDiskMem), CFE_PSP_ERROR);
    UT_SetDataBuffer(UT_KEY(OS_FileSysStatVolume), &StatBuf, sizeof(StatBuf), false);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]),
              "CFE_ES_InitializeFileSystems", "Processor reset; cannot get memory for volatile disk");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remove the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_rmfs), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(OS_FileSysStatVolume), &StatBuf, sizeof(StatBuf), false);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REMOVE_VOLATILE]),
              "CFE_ES_InitializeFileSystems", "Processor reset; error removing volatile (RAM) volume");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to unmount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_unmount), 1, -1);
    UT_SetDataBuffer(UT_KEY(OS_FileSysStatVolume), &StatBuf, sizeof(StatBuf), false);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_UNMOUNT_VOLATILE]),
              "CFE_ES_InitializeFileSystems", "Processor reset; error unmounting volatile (RAM) volume");

    /* Test successful initialization of the file systems */
    ES_ResetUnitTest();
    CFE_ES_InitializeFileSystems(4564564);
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Panic)) == 0, "CFE_ES_InitializeFileSystems",
              "Initialize file systems; successful");

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to remount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_mount), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(OS_FileSysStatVolume), &StatBuf, sizeof(StatBuf), false);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REMOUNT_VOLATILE]),
              "CFE_ES_InitializeFileSystems", "Processor reset; error remounting volatile (RAM) volume");

    /* Test initialization of the file systems with an error determining the
     * number of blocks that are free on the volume
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_FileSysStatVolume), 1, -1);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_DETERMINE_BLOCKS]),
              "CFE_ES_InitializeFileSystems", "Processor reset; error determining blocks free on volume");

    /* Test reading the object table where a record used error occurs */
    ES_ResetUnitTest();
    TaskRecPtr = CFE_ES_Global.TaskTable;
    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        /* Mark all entries as "used" to ensure that the log message gets
         * output
         */
        CFE_ES_TaskRecordSetUsed(TaskRecPtr, ES_UT_MakeTaskIdForIndex(j));
        ++TaskRecPtr;
    }

    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_RECORD_USED]), "CFE_ES_CreateObjects",
              "Record used error");

    /* Test reading the object table where an error occurs when
     * calling a function
     */
    ES_ResetUnitTest();
    TaskRecPtr = CFE_ES_Global.TaskTable;
    for (j = 0; j < OS_MAX_TASKS; j++)
    {
        /* Mark all entries as "used" to ensure that the log message gets
         * output
         */
        CFE_ES_TaskRecordSetUsed(TaskRecPtr, ES_UT_MakeAppIdForIndex(j));
        ++TaskRecPtr;
    }

    UT_SetDeferredRetcode(UT_KEY(CFE_TBL_EarlyInit), 1, -1);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_RECORD_USED]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_EARLYINIT]),
              "CFE_ES_CreateObjects", "Error returned when calling function");

    /* Test reading the object table where an error occurs when
     * creating a core app
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskCreate), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_BinSemCreate), OS_ERROR);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_APP_CREATE]), "CFE_ES_CreateObjects",
              "Error creating core application");

    /* Test reading the object table where all app slots are taken */
    ES_ResetUnitTest();

    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), OS_ERROR);
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_NO_FREE_CORE_APP_SLOTS]) == 5,
              "CFE_ES_CreateObjects", "No free application slots available, message");

    /* Test reading the object table with a NULL function pointer */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), OS_ERROR);
    CFE_ES_ObjectTable[1].ObjectType = CFE_ES_FUNCTION_CALL;
    CFE_ES_CreateObjects();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_NO_FREE_CORE_APP_SLOTS]) == 5,
              "CFE_ES_CreateObjects", "Bad function pointer, app slots message");
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_FUNCTION_POINTER]),
              "CFE_ES_CreateObjects", "Bad function pointer message");

    /* Test response to an invalid startup type */
    ES_ResetUnitTest();
    CFE_ES_Global.DebugVars.DebugFlag = 1;
    CFE_ES_SetupResetVariables(-1, CFE_PSP_RST_SUBTYPE_HW_SPECIAL_COMMAND, 1);
    UT_Report(__FILE__, __LINE__, CFE_ES_Global.DebugVars.DebugFlag == 1, "CFE_ES_SetupResetVariables",
              "Invalid startup type");
    CFE_ES_Global.DebugVars.DebugFlag = 0;

    /* Test initialization of the file systems specifying a processor reset
     * following a failure to initialize and mount the RAM volume
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_initfs), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_mount), OS_ERROR);
    UT_SetDataBuffer(UT_KEY(OS_FileSysStatVolume), &StatBuf, sizeof(StatBuf), false);
    CFE_ES_InitializeFileSystems(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INSUFF_FREE_SPACE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_INIT_VOLATILE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MOUNT_VOLATILE]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_REMOUNT_VOLATILE]),
              "CFE_ES_InitializeFileSystems",
              "Processor reset; error initializing and mounting volatile "
              "(RAM) volume");

    /* Test application sync delay where the operation times out */
    ES_ResetUnitTest();
    /* This prep is necessary so GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppType_CORE, NULL, &AppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_WaitForSystemState(CFE_ES_SystemState_OPERATIONAL, CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) ==
                  CFE_ES_OPERATION_TIMED_OUT,
              "CFE_ES_ApplicationSyncDelay", "Operation timed out");

    /* Test startup sync with alternate minimum system state
     * of CFE_ES_SystemState_SHUTDOWN
     */
    ES_ResetUnitTest();
    /* This prep is necessary so GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppType_CORE, NULL, &AppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_WaitForSystemState(CFE_ES_SystemState_SHUTDOWN, CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) ==
                      CFE_ES_OPERATION_TIMED_OUT &&
                  AppRecPtr->AppState == CFE_ES_AppState_STOPPED,
              "CFE_ES_WaitForSystemState", "Min System State is CFE_ES_SystemState_SHUTDOWN");

    /* Test startup sync with alternate minimum system state
     * of CFE_ES_SystemState_APPS_INIT
     */
    ES_ResetUnitTest();
    /* This prep is necessary so GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppType_CORE, NULL, &AppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;

    UT_Report(__FILE__, __LINE__,
              CFE_ES_WaitForSystemState(CFE_ES_SystemState_APPS_INIT, CFE_PLATFORM_ES_STARTUP_SCRIPT_TIMEOUT_MSEC) ==
                      CFE_ES_OPERATION_TIMED_OUT &&
                  AppRecPtr->AppState == CFE_ES_AppState_LATE_INIT,
              "CFE_ES_WaitForSystemState", "Min System State is CFE_ES_SystemState_APPS_INIT");

    /* Test success */
    ES_ResetUnitTest();
    /* This prep is necessary so GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppType_CORE, NULL, &AppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    ASSERT(CFE_ES_WaitForSystemState(CFE_ES_SystemState_CORE_READY, 0));
}

void TestApps(void)
{
    int                     NumBytes;
    int                     Return;
    CFE_ES_AppInfo_t        AppInfo;
    CFE_ES_AppId_t          AppId;
    CFE_ES_TaskId_t         TaskId;
    CFE_ES_TaskRecord_t *   UtTaskRecPtr;
    CFE_ES_AppRecord_t *    UtAppRecPtr;
    CFE_ES_MemPoolRecord_t *UtPoolRecPtr;
    char                    NameBuffer[OS_MAX_API_NAME + 5];
    CFE_ES_AppStartParams_t StartParams;

    UtPrintf("Begin Test Apps");

    /* Test starting an application where the startup script is too long */
    ES_ResetUnitTest();
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_fghfghfghfghfg"
            "hfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghfghLIB, "
            "0, 0, 0x0, 1; CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, "
            "70, 4096, 0x0, 1; CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, "
            "SCH_APP, 120, 4096, 0x0, 1; CFE_APP, /cf/apps/to.bundle, "
            "TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            sizeof(StartupScript) - 1);
    StartupScript[sizeof(StartupScript) - 1] = '\0';
    NumBytes                                 = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR, "ut_startup");
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_FILE_LINE_TOO_LONG]));
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]));

    /* Create a valid startup script for subsequent tests */
    strncpy(StartupScript,
            "CFE_LIB, /cf/apps/tst_lib.bundle, TST_LIB_Init, TST_LIB, 0, 0, 0x0, 1; "
            "CFE_APP, /cf/apps/ci.bundle, CI_task_main, CI_APP, 70, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/sch.bundle, SCH_TaskMain, SCH_APP, 120, 4096, 0x0, 1; "
            "CFE_APP, /cf/apps/to.bundle, TO_task_main, TO_APP, 74, 4096, 0x0, 1; !",
            sizeof(StartupScript) - 1);
    StartupScript[sizeof(StartupScript) - 1] = '\0';
    NumBytes                                 = strlen(StartupScript);
    UT_SetReadBuffer(StartupScript, NumBytes);

    /* Test starting an application with an error reading the startup file */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, -1);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR, "ut_startup");
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_STARTUP_READ]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]),
              "CFE_ES_StartApplications", "Error reading startup file");

    /* Test starting an application with an end-of-file returned by the
     * OS read
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_read), 1, 0);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR, "ut_startup");
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]),
              "CFE_ES_StartApplications", "End of file reached");

    /* Test starting an application with an open failure */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR, "ut_startup");
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_OPEN_ES_APP_STARTUP]),
              "CFE_ES_StartApplications", "Can't open ES application startup file");

    /* Test successfully starting an application */
    ES_ResetUnitTest();
    UT_SetReadBuffer(StartupScript, NumBytes);
    UT_SetHookFunction(UT_KEY(OS_TaskCreate), ES_UT_SetAppStateHook, NULL);
    CFE_ES_StartApplications(CFE_PSP_RST_TYPE_PROCESSOR, "ut_startup");
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_ES_APP_STARTUP_OPEN]));

    /* Test parsing the startup script with an unknown entry type */
    ES_ResetUnitTest();
    {
        const char *TokenList[] = {"UNKNOWN", "/cf/apps/tst_lib.bundle", "TST_LIB_Init", "TST_LIB", "0", "0", "0x0",
                                   "1"};
        UT_Report(__FILE__, __LINE__, CFE_ES_ParseFileEntry(TokenList, 8) == CFE_ES_ERR_APP_CREATE,
                  "CFE_ES_ParseFileEntry", "Unknown entry type");

        /* Test parsing the startup script with an invalid file name */
        UT_SetDefaultReturnValue(UT_KEY(CFE_FS_ParseInputFileName), CFE_FS_INVALID_PATH);
        UT_Report(__FILE__, __LINE__, CFE_ES_ParseFileEntry(TokenList, 8) == CFE_FS_INVALID_PATH,
                  "CFE_ES_ParseFileEntry", "Invalid file name");
    }

    /* Test parsing the startup script with an invalid argument passed in */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_ParseFileEntry(NULL, 0) == CFE_ES_BAD_ARGUMENT, "CFE_ES_ParseFileEntry",
              "Invalid argument");

    /* Test application loading and creation with a task creation failure */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskCreate), OS_ERROR);
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename", "EntryPoint", 170, 4096, 1);
    Return = CFE_ES_AppCreate(&AppId, "AppName", &StartParams);
    UtAssert_INT32_EQ(Return, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_APP_CREATE]));

    /* Test application creation with NULL parameters */
    ES_ResetUnitTest();
    Return = CFE_ES_AppCreate(&AppId, "AppName", NULL);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_AppCreate", "NULL file name");

    /* Test application creation with name too long */
    memset(NameBuffer, 'x', sizeof(NameBuffer) - 1);
    NameBuffer[sizeof(NameBuffer) - 1] = 0;
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename.x", "EntryPoint", 170, 4096, 1);
    Return = CFE_ES_AppCreate(&AppId, NameBuffer, &StartParams);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_AppCreate", "Name too long");

    /* Test successful application loading and creation  */
    ES_ResetUnitTest();
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename.x", "EntryPoint", 170, 8192, 1);
    Return = CFE_ES_AppCreate(&AppId, "AppName", &StartParams);
    UT_Report(__FILE__, __LINE__, Return == CFE_SUCCESS, "CFE_ES_AppCreate", "Application load/create; successful");

    /* Test application loading of the same name again */
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename.x", "EntryPoint", 170, 8192, 1);
    Return = CFE_ES_AppCreate(&AppId, "AppName", &StartParams);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_ERR_DUPLICATE_NAME, "CFE_ES_AppCreate", "Duplicate name");

    /* Test application loading and creation where the file cannot be loaded */
    UT_InitData();
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleLoad), 1, -1);
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename.x", "EntryPoint", 170, 8192, 1);
    Return = CFE_ES_AppCreate(&AppId, "AppName2", &StartParams);
    UtAssert_INT32_EQ(Return, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_EXTRACT_FILENAME_UT55]));

    /* Test application loading and creation where all app slots are taken */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), OS_ERROR);
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename.x", "EntryPoint", 170, 8192, 1);
    Return = CFE_ES_AppCreate(&AppId, "AppName", &StartParams);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_NO_RESOURCE_IDS_AVAILABLE &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_NO_FREE_APP_SLOTS]),
              "CFE_ES_AppCreate", "No free application slots available");

    /* Check operation of the CFE_ES_CheckAppIdSlotUsed() helper function */
    CFE_ES_Global.AppTable[1].AppId = CFE_ES_APPID_C(ES_UT_MakeAppIdForIndex(1));
    CFE_ES_Global.AppTable[2].AppId = CFE_ES_APPID_UNDEFINED;
    UtAssert_True(CFE_ES_CheckAppIdSlotUsed(ES_UT_MakeAppIdForIndex(1)), "App Slot Used");
    UtAssert_True(!CFE_ES_CheckAppIdSlotUsed(ES_UT_MakeAppIdForIndex(2)), "App Slot Unused");

    /* Test application loading and creation where the entry point symbol
     * cannot be found
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleSymbolLookup), 1, -1);
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename.x", "EntryPoint", 170, 8192, 1);
    Return = CFE_ES_AppCreate(&AppId, "AppName", &StartParams);
    UtAssert_INT32_EQ(Return, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_FIND_SYMBOL]));

    /* Test application loading and creation where the entry point symbol
     * cannot be found and module unload fails
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleSymbolLookup), 1, -1);
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleUnload), 1, -1);
    ES_UT_SetupAppStartParams(&StartParams, "ut/filename.x", "EntryPoint", 170, 8192, 1);
    Return = CFE_ES_AppCreate(&AppId, "AppName", &StartParams);
    UtAssert_INT32_EQ(Return, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_FIND_SYMBOL]));
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MODULE_UNLOAD_FAILED]));

    /*
     * Set up a situation where attempting to get appID by context,
     * but the task record does not match the app record.
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    UtTaskRecPtr->AppId = CFE_ES_APPID_UNDEFINED;
    UtAssert_NULL(CFE_ES_GetAppRecordByContext());

    /* Test scanning and acting on the application table where the timer
     * expires for a waiting application
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_WAITING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppTimerMsec      = 0;
    memset(&CFE_ES_Global.BackgroundAppScanState, 0, sizeof(CFE_ES_Global.BackgroundAppScanState));
    CFE_ES_RunAppTableScan(0, &CFE_ES_Global.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PCR_ERR2_EID) && UtAppRecPtr->ControlReq.AppTimerMsec == 0,
              "CFE_ES_RunAppTableScan", "Waiting; process control request");

    /* Test scanning and acting on the application table where the timer
     * has not expired for a waiting application
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_WAITING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    UtAppRecPtr->ControlReq.AppTimerMsec      = 5000;
    CFE_ES_RunAppTableScan(1000, &CFE_ES_Global.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__,
              UtAppRecPtr->ControlReq.AppTimerMsec == 4000 &&
                  UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_APP_EXIT,
              "CFE_ES_RunAppTableScan", "Decrement timer");

    /* Test scanning and acting on the application table where the application
     * has stopped and is ready to be acted on
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppTimerMsec      = 0;
    CFE_ES_RunAppTableScan(0, &CFE_ES_Global.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PCR_ERR2_EID) && UtAppRecPtr->ControlReq.AppTimerMsec == 0,
              "CFE_ES_RunAppTableScan", "Stopped; process control request");

    /* Test scanning and acting on the application table where the application
     * has stopped and is ready to be acted on
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_EARLY_INIT, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppTimerMsec = 5000;
    CFE_ES_RunAppTableScan(0, &CFE_ES_Global.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__, UT_GetNumEventsSent() == 0 && UtAppRecPtr->ControlReq.AppTimerMsec == 5000,
              "CFE_ES_RunAppTableScan", "Initializing; process control request");

    /* Test a control action request on an application with an
     * undefined control request state
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = 0x12345;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PCR_ERR2_EID), "CFE_ES_ProcessControlRequest",
              "Unknown state (default)");

    /* Test a successful control action request to exit an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupAppStartParams(&UtAppRecPtr->StartParams, "/ram/Filename", "NotNULL", 8192, 255, 0);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_EXIT_APP_INF_EID), "CFE_ES_ProcessControlRequest",
              "Exit application; successful");

    /* Test a control action request to exit an application where the
     * request fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_EXIT_APP_ERR_EID), "CFE_ES_ProcessControlRequest",
              "Exit application failure");

    /* Test a control action request to stop an application where the
     * request fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_DELETE;
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_STOP_ERR3_EID), "CFE_ES_ProcessControlRequest",
              "Stop application failure");

    /* Test a control action request to restart an application where the
     * request fails due to a CleanUpApp error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RESTART;
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR4_EID), "CFE_ES_ProcessControlRequest",
              "Restart application failure; CleanUpApp error");

    /* Test a control action request to restart an application where the
     * request fails due to an AppCreate error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RESTART;
    OS_ModuleLoad(&UtAppRecPtr->LoadStatus.ModuleId, NULL, NULL, 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskCreate), OS_ERROR);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR3_EID), "CFE_ES_ProcessControlRequest",
              "Restart application failure; AppCreate error");

    /* Test a control action request to reload an application where the
     * request fails due to a CleanUpApp error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RELOAD;
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR4_EID), "CFE_ES_ProcessControlRequest",
              "Reload application failure; CleanUpApp error");

    /* Test a control action request to reload an application where the
     * request fails due to an AppCreate error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RELOAD;
    OS_ModuleLoad(&UtAppRecPtr->LoadStatus.ModuleId, NULL, NULL, 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskCreate), OS_ERROR);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR3_EID), "CFE_ES_ProcessControlRequest",
              "Reload application failure; AppCreate error");

    /* Test a successful control action request to exit an application that
     * has an error
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupAppStartParams(&UtAppRecPtr->StartParams, "/ram/FileName", "NULL", 8192, 255, 0);

    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_ERROR;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ERREXIT_APP_INF_EID), "CFE_ES_ProcessControlRequest",
              "Exit application on error; successful");

    /* Test a control action request to exit an application that
     * has an error where the request fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_ERROR;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ERREXIT_APP_ERR_EID), "CFE_ES_ProcessControlRequest",
              "Exit application on error failure");

    /* Test a successful control action request to stop an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupAppStartParams(&UtAppRecPtr->StartParams, "/ram/FileName", "NULL", 8192, 255, 0);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_DELETE;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_STOP_INF_EID), "CFE_ES_ProcessControlRequest",
              "Stop application; successful");

    /* Test a successful control action request to restart an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupAppStartParams(&UtAppRecPtr->StartParams, "/ram/FileName", "NULL", 8192, 255, 0);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RESTART;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_INF_EID), "CFE_ES_ProcessControlRequest",
              "Restart application; successful");

    /* Test a successful control action request to reload an application */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupAppStartParams(&UtAppRecPtr->StartParams, "/ram/FileName", "NULL", 8192, 255, 0);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_RELOAD;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_INF_EID), "CFE_ES_ProcessControlRequest",
              "Reload application; successful");

    /* Test a control action request for an application that has an invalid
     * state (exception)
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupAppStartParams(&UtAppRecPtr->StartParams, "/ram/FileName", "NULL", 8192, 255, 0);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_SYS_EXCEPTION;
    AppId                                     = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_ProcessControlRequest(AppId);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PCR_ERR1_EID), "CFE_ES_ProcessControlRequest",
              "Invalid state");

    /* Test populating the application information structure with data */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppInfo(&AppInfo, AppId) == CFE_SUCCESS, "CFE_ES_GetAppInfo",
              "Get application information; successful");

    /* Test populating the application information structure with data using
     * a null application information pointer
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppInfo(NULL, AppId) == CFE_ES_BAD_ARGUMENT, "CFE_ES_GetAppInfo",
              "Null application information pointer");

    /* Test populating the application information structure using an
     * inactive application ID
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    CFE_ES_AppRecordSetFree(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppInfo(&AppInfo, AppId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetAppInfo", "Application ID not active");

    /* Test populating the application information structure using an
     * application ID value greater than the maximum allowed
     */
    ES_ResetUnitTest();
    AppId = CFE_ES_APPID_C(ES_UT_MakeAppIdForIndex(99999));
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppInfo(&AppInfo, AppId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetAppInfo", "Application ID exceeds maximum");

    /* Test populating the application information structure using a valid
     * application ID, but with a failure to retrieve the module information
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppInfo(&AppInfo, AppId) == CFE_SUCCESS, "CFE_ES_GetAppInfo",
              "Module not found");

    /* Test deleting an application and cleaning up its resources with OS
     * delete and close failures
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupForOSCleanup();

    OS_ModuleLoad(&UtAppRecPtr->LoadStatus.ModuleId, NULL, NULL, 0);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelete), OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_close), OS_ERROR);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_ES_APP_CLEANUP_ERR, "CFE_ES_CleanUpApp",
              "Task OS delete and close failure");

    /* Test deleting an application and cleaning up its resources with a
     * mutex delete failure
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemDelete), 1, OS_ERROR);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_ES_APP_CLEANUP_ERR, "CFE_ES_CleanUpApp",
              "Task mutex delete failure");

    /* Test deleting an application and cleaning up its resources with a
     * failure to unload the module
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleUnload), 1, OS_ERROR);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_ES_APP_CLEANUP_ERR, "CFE_ES_CleanUpApp",
              "Module unload failure");

    /* Test deleting an application and cleaning up its resources where the
     * EVS application cleanup fails
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_CleanUpApp), 1, -1);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_ES_APP_CLEANUP_ERR, "CFE_ES_CleanUpApp",
              "EVS application cleanup failure");

    /* Test cleaning up the OS resources for a task with a failure
     *  deleting mutexes
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_ES_MUT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources", "Mutex delete failure");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * binary semaphores
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_ES_BIN_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources", "Binary semaphore delete failure");

    /* Test cleaning up the OS resources for a task with a failure deleting
     * counting semaphores
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemDelete), 1, OS_ERROR);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_ES_COUNT_SEM_DELETE_ERR,
              "CFE_ES_CleanupTaskResources", "Counting semaphore failure");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting queues
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_QueueDelete), 1, OS_ERROR);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_ES_QUEUE_DELETE_ERR,
              "CFE_ES_CleanupTaskResources", "Queue delete failure");

    /* Test cleaning up the OS resources for a task with a failure
     * deleting timers
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    /* Just set OS_TimerDelete to fail.  There is no requirement
     * that the code call OS_TimerGetInfo first.
     */
    UT_SetDeferredRetcode(UT_KEY(OS_TimerDelete), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_ES_TIMER_DELETE_ERR,
              "CFE_ES_CleanupTaskResources", "Timer delete failure");

    /* Test cleaning up the OS resources for a task with a failure
     * closing files
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    ES_UT_SetupForOSCleanup();
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_close), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) != CFE_SUCCESS, "CFE_ES_CleanupTaskResources",
              "File close failure");

    /* Test cleaning up the OS resources for a task with a failure
     * to delete the task
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelete), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_ES_TASK_DELETE_ERR,
              "CFE_ES_CleanupTaskResources", "Task delete failure");

    /* Test successfully cleaning up the OS resources for a task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_SUCCESS, "CFE_ES_CleanupTaskResources",
              "Clean up task OS resources; successful");

    /* Test parsing the startup script for a cFE application and a restart
     * application exception action
     */
    ES_ResetUnitTest();
    {
        const char *TokenList[] = {"CFE_APP", "/cf/apps/tst_lib.bundle", "TST_LIB_Init", "TST_LIB", "0", "0", "0x0",
                                   "0"};
        UT_Report(__FILE__, __LINE__, CFE_ES_ParseFileEntry(TokenList, 8) == CFE_SUCCESS, "CFE_ES_ParseFileEntry",
                  "CFE application; restart application on exception");
    }

    /* Test scanning and acting on the application table where the timer
     * expires for a waiting application
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppTimerMsec = 0;
    CFE_ES_RunAppTableScan(0, &CFE_ES_Global.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__, UT_GetNumEventsSent() == 0 && UtAppRecPtr->ControlReq.AppTimerMsec == 0,
              "CFE_ES_RunAppTableScan", "Waiting; process control request");

    /* Test scanning and acting on the application table where the application
     * is already running
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppTimerMsec = 0;
    CFE_ES_RunAppTableScan(0, &CFE_ES_Global.BackgroundAppScanState);
    UT_Report(__FILE__, __LINE__, UT_GetNumEventsSent() == 0 && UtAppRecPtr->ControlReq.AppTimerMsec == 0,
              "CFE_ES_RunAppTableScan", "Running; process control request");

    /* Test deleting an application and cleaning up its resources where the
     * application ID matches the main task ID
     */
    ES_ResetUnitTest();
    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    /* Setup a second entry which will NOT be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    ES_UT_SetupMemPoolId(&UtPoolRecPtr);
    UtPoolRecPtr->OwnerAppID = CFE_ES_AppRecordGetID(UtAppRecPtr);
    /* Associate a child task with the app to be deleted */
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_SUCCESS, "CFE_ES_CleanUpApp",
              "Main task ID matches task ID, nominal");
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskRecordIsUsed(UtTaskRecPtr), "CFE_ES_CleanUpApp",
              "Main task ID matches task ID, other task unaffected");
    UT_Report(__FILE__, __LINE__, !CFE_ES_MemPoolRecordIsUsed(UtPoolRecPtr), "CFE_ES_CleanUpApp",
              "Main task ID matches task ID, memory pool deleted");

    /* Test deleting an application and cleaning up its resources where the
     * memory pool deletion fails
     */
    ES_ResetUnitTest();
    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupMemPoolId(&UtPoolRecPtr);
    UtPoolRecPtr->OwnerAppID = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UtPoolRecPtr->PoolID     = CFE_ES_MEMHANDLE_C(CFE_ResourceId_FromInteger(99999)); /* Mismatch */
    AppId                    = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_ES_APP_CLEANUP_ERR, "CFE_ES_CleanUpApp",
              "Mem Pool delete error");
    UT_Report(__FILE__, __LINE__, CFE_ES_MemPoolRecordIsUsed(UtPoolRecPtr), "CFE_ES_CleanUpApp", "Mem Pool not freed");

    /* Test deleting an application and cleaning up its resources where the
     * application ID doesn't match the main task ID
     */
    ES_ResetUnitTest();

    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    /* Setup a second entry which will NOT be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    /* Associate a child task with the app to be deleted */
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, NULL);

    /* switch the main task association (makes it wrong) */
    UtAppRecPtr->MainTaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);

    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelete), OS_ERROR);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_ES_APP_CLEANUP_ERR, "CFE_ES_CleanUpApp",
              "Main task ID doesn't match task ID, CFE_ES_APP_CLEANUP_ERR");
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskRecordIsUsed(UtTaskRecPtr), "CFE_ES_CleanUpApp",
              "Main task ID doesn't match task ID, second task unchanged");

    /* Test deleting an application and cleaning up its resources where the
     * application ID doesn't match and the application is a core application
     */
    ES_ResetUnitTest();
    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    /* Setup a second entry which will NOT be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, &UtTaskRecPtr);
    /* Associate a child task with the app to be deleted */
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, NULL);

    /* switch the main task association (makes it wrong) */
    UtAppRecPtr->MainTaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    AppId                   = CFE_ES_AppRecordGetID(UtAppRecPtr);

    UT_Report(__FILE__, __LINE__, CFE_ES_CleanUpApp(AppId) == CFE_SUCCESS, "CFE_ES_CleanUpApp",
              "Application ID mismatch; core application");

    UT_Report(__FILE__, __LINE__, CFE_ES_TaskRecordIsUsed(UtTaskRecPtr), "CFE_ES_CleanUpApp",
              "Application ID mismatch; core application");

    UT_Report(__FILE__, __LINE__, CFE_ES_Global.RegisteredExternalApps == 1, "CFE_ES_CleanUpApp",
              "Application ID mismatch; core application");

    /* Test successfully deleting an application and cleaning up its resources
     * and the application is an external application
     */
    ES_ResetUnitTest();

    /* Setup an entry which will be deleted */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CleanUpApp(AppId) == CFE_SUCCESS && !CFE_ES_TaskRecordIsUsed(UtTaskRecPtr) &&
                  CFE_ES_Global.RegisteredExternalApps == 0,
              "CFE_ES_CleanUpApp", "Successful application cleanup; external application");

    /* Test cleaning up the OS resources for a task with failure to
     * obtain information on mutex, binary, and counter semaphores, and
     * queues, timers, and file descriptors
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_CountSemGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_QueueGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_TimerGetInfo), 1, OS_ERROR);
    UT_SetDeferredRetcode(UT_KEY(OS_FDGetInfo), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CleanupTaskResources(TaskId) == CFE_SUCCESS, "CFE_ES_CleanupTaskResources",
              "Get OS information failures");
}

void TestResourceID(void)
{
    /*
     * Test cases for generic resource ID functions which are
     * not sufficiently covered by other app/lib tests.
     *
     * Most of the Resource ID functions have been moved to a separate module and that module
     * has its own unit test which tests these APIs.
     *
     * This is mainly to exercise the conversion of CFE ES task IDs to OSAL task IDs and vice versa.
     * The conversion is only implemented for tasks, because this is the only resource type where
     * there is overlap between OSAL and CFE (they both have task records).
     */
    CFE_ES_TaskId_t cfe_id1, cfe_id2;
    osal_id_t       osal_id;

    /*
     * In this function the actual values may or may not change,
     * depending on whether strict/simple mode is in use.  However
     * converting to and from should result in the original value
     * either way.
     */
    UT_SetDefaultReturnValue(UT_KEY(OS_IdentifyObject), OS_OBJECT_TYPE_OS_TASK);
    cfe_id1 = CFE_ES_TASKID_C(ES_UT_MakeTaskIdForIndex(0));
    osal_id = CFE_ES_TaskId_ToOSAL(cfe_id1);
    cfe_id2 = CFE_ES_TaskId_FromOSAL(osal_id);
    UtAssert_True(CFE_RESOURCEID_TEST_EQUAL(cfe_id1, cfe_id2),
                  "CFE_ES_TaskId_ToOSAL()/FromOSAL(): before=%lx, after=%lx", CFE_RESOURCEID_TO_ULONG(cfe_id1),
                  CFE_RESOURCEID_TO_ULONG(cfe_id2));
}

void TestLibs(void)
{
    CFE_ES_LibRecord_t *      UtLibRecPtr;
    char                      LongLibraryName[sizeof(UtLibRecPtr->LibName) + 1];
    CFE_ES_LibId_t            Id;
    int32                     Return;
    CFE_ES_ModuleLoadParams_t LoadParams;

    /* Test shared library loading and initialization where the initialization
     * routine returns an error
     */
    ES_ResetUnitTest();
    UT_SetDummyFuncRtn(-444);
    ES_UT_SetupModuleLoadParams(&LoadParams, "filename", "entrypt");
    Return = CFE_ES_LoadLibrary(&Id, "LibName", &LoadParams);
    UtAssert_INT32_EQ(Return, -444);
    UtAssert_NONZERO(UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_SHARED_LIBRARY_INIT]));

    /* Test Load library returning an error on a null pointer argument */
    Return = CFE_ES_LoadLibrary(&Id, "LibName", NULL);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_LoadLibrary",
              "Load shared library bad argument (NULL filename)");

    Return = CFE_ES_LoadLibrary(&Id, NULL, &LoadParams);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_LoadLibrary",
              "Load shared library bad argument (NULL library name)");

    /* Test Load library returning an error on a too long library name */
    memset(LongLibraryName, 'a', sizeof(LongLibraryName) - 1);
    LongLibraryName[sizeof(LongLibraryName) - 1] = '\0';
    Return                                       = CFE_ES_LoadLibrary(&Id, LongLibraryName, &LoadParams);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_LoadLibrary",
              "Load shared library bad argument (library name too long)");

    /* Test successful shared library loading and initialization */
    UT_InitData();
    UT_SetDummyFuncRtn(OS_SUCCESS);
    Return = CFE_ES_LoadLibrary(&Id, "TST_LIB", &LoadParams);
    UT_Report(__FILE__, __LINE__, Return == CFE_SUCCESS, "CFE_ES_LoadLibrary", "successful");

    UtLibRecPtr = CFE_ES_LocateLibRecordByID(Id);
    UtAssert_True(UtLibRecPtr != NULL, "CFE_ES_LoadLibrary() return valid ID");
    UtAssert_True(CFE_ES_LibRecordIsUsed(UtLibRecPtr), "CFE_ES_LoadLibrary() record used");

    /* Try loading same library again, should return the DUPLICATE code */
    Return = CFE_ES_LoadLibrary(&Id, "TST_LIB", &LoadParams);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_ERR_DUPLICATE_NAME, "CFE_ES_LoadLibrary", "Duplicate");
    UtAssert_True(CFE_RESOURCEID_TEST_EQUAL(Id, CFE_ES_LibRecordGetID(UtLibRecPtr)),
                  "CFE_ES_LoadLibrary() returned previous ID");

    /* Test shared library loading and initialization where the library
     * fails to load
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleLoad), 1, -1);
    Return = CFE_ES_LoadLibrary(&Id, "TST_LIB", &LoadParams);
    UtAssert_INT32_EQ(Return, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test shared library loading and initialization where the library
     * entry point symbol cannot be found
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_ModuleSymbolLookup), 1, -1);
    Return = CFE_ES_LoadLibrary(&Id, "TST_LIB", &LoadParams);
    UtAssert_INT32_EQ(Return, CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    /* Test shared library loading and initialization where the library
     * initialization function fails and then must be cleaned up
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_remove), OS_ERROR); /* for coverage of error path */
    UT_SetDefaultReturnValue(UT_KEY(dummy_function), -555);
    ES_UT_SetupModuleLoadParams(&LoadParams, "filename", "dummy_function");
    Return = CFE_ES_LoadLibrary(&Id, "TST_LIB", &LoadParams);
    UT_Report(__FILE__, __LINE__, Return == -555, "CFE_ES_LoadLibrary", "Library initialization function failure");

    /* Test shared library loading and initialization where there are no
     * library slots available
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), OS_ERROR);
    Return = CFE_ES_LoadLibrary(&Id, "LibName", &LoadParams);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_ES_NO_RESOURCE_IDS_AVAILABLE && UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_LIBRARY_SLOTS]),
              "CFE_ES_LoadLibrary", "No free library slots");

    /* check operation of the CFE_ES_CheckLibIdSlotUsed() function */
    CFE_ES_Global.LibTable[1].LibId = CFE_ES_LIBID_C(ES_UT_MakeLibIdForIndex(1));
    CFE_ES_Global.LibTable[2].LibId = CFE_ES_LIBID_UNDEFINED;
    UtAssert_True(CFE_ES_CheckLibIdSlotUsed(ES_UT_MakeLibIdForIndex(1)), "Lib Slot Used");
    UtAssert_True(!CFE_ES_CheckLibIdSlotUsed(ES_UT_MakeLibIdForIndex(2)), "Lib Slot Unused");
    /*
     * Test public Name+ID query/lookup API
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleLibId("UT", &UtLibRecPtr);
    Id = CFE_ES_LibRecordGetID(UtLibRecPtr);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(LongLibraryName, Id, sizeof(LongLibraryName)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&Id, "Nonexistent"), CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&Id, LongLibraryName), CFE_SUCCESS);
    UtAssert_True(CFE_RESOURCEID_TEST_EQUAL(Id, CFE_ES_LibRecordGetID(UtLibRecPtr)), "Library IDs Match");
    UtAssert_INT32_EQ(CFE_ES_GetLibName(LongLibraryName, CFE_ES_LIBID_UNDEFINED, sizeof(LongLibraryName)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetLibName(NULL, Id, sizeof(LongLibraryName)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetLibIDByName(&Id, NULL), CFE_ES_BAD_ARGUMENT);
}

void TestERLog(void)
{
    int                              Return;
    void *                           LocalBuffer;
    size_t                           LocalBufSize;
    CFE_ES_BackgroundLogDumpGlobal_t State;

    UtPrintf("Begin Test Exception and Reset Log");

    /* Test initial rolling over log entry,
     * null description,
     * and non-null context with small size
     */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->ERLogIndex = CFE_PLATFORM_ES_ER_LOG_ENTRIES + 1;
    Return = CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, 1, NULL);
    UT_Report(__FILE__, __LINE__,
              Return == CFE_SUCCESS &&
                  !strcmp(CFE_ES_Global.ResetDataPtr->ERLog[0].BaseInfo.Description, "No Description String Given.") &&
                  CFE_ES_Global.ResetDataPtr->ERLogIndex == 1,
              "CFE_ES_WriteToERLog", "Log entries exceeded; no description; valid context size");

    /* Test non-rolling over log entry,
     * null description,
     * and null context
     */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->ERLogIndex = 0;
    Return = CFE_ES_WriteToERLog(CFE_ES_LogEntryType_CORE, CFE_PSP_RST_TYPE_POWERON, 1, NULL);
    UT_Report(__FILE__, __LINE__, Return == CFE_SUCCESS && CFE_ES_Global.ResetDataPtr->ERLogIndex == 1,
              "CFE_ES_WriteToERLog", "No log entry rollover; no description; no context");

    /* Test ER log background write functions */
    memset(&State, 0, sizeof(State));
    LocalBuffer  = NULL;
    LocalBufSize = 0;

    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_CopyContext), 1, 128);
    UtAssert_True(!CFE_ES_BackgroundERLogFileDataGetter(&State, 0, &LocalBuffer, &LocalBufSize),
                  "CFE_ES_BackgroundERLogFileDataGetter at start, with context");
    UtAssert_UINT32_EQ(State.EntryBuffer.ContextSize, 128);
    UtAssert_NOT_NULL(LocalBuffer);
    UtAssert_NONZERO(LocalBufSize);

    memset(&State.EntryBuffer, 0xEE, sizeof(State.EntryBuffer));
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_Exception_CopyContext), 1, -1);
    UtAssert_True(
        CFE_ES_BackgroundERLogFileDataGetter(&State, CFE_PLATFORM_ES_ER_LOG_ENTRIES - 1, &LocalBuffer, &LocalBufSize),
        "CFE_ES_BackgroundERLogFileDataGetter at EOF, no context");
    UtAssert_ZERO(State.EntryBuffer.ContextSize);

    UtAssert_True(
        CFE_ES_BackgroundERLogFileDataGetter(&State, CFE_PLATFORM_ES_ER_LOG_ENTRIES, &LocalBuffer, &LocalBufSize),
        "CFE_ES_BackgroundERLogFileDataGetter beyond EOF");
    UtAssert_NULL(LocalBuffer);
    UtAssert_ZERO(LocalBufSize);

    /* Test ER log background write event handling */
    UT_ClearEventHistory();
    CFE_ES_BackgroundERLogFileEventHandler(&State, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, 10, 0, 100);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_ERLOG2_EID), "COMPLETE: CFE_ES_ERLOG2_EID generated");

    UT_ClearEventHistory();
    CFE_ES_BackgroundERLogFileEventHandler(&State, CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR, -1, 10, 10, 100);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
                  "HEADER_WRITE_ERROR: CFE_ES_FILEWRITE_ERR_EID generated");

    UT_ClearEventHistory();
    CFE_ES_BackgroundERLogFileEventHandler(&State, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR, -1, 10, 10, 100);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
                  "RECORD_WRITE_ERROR: CFE_ES_FILEWRITE_ERR_EID generated");

    UT_ClearEventHistory();
    CFE_ES_BackgroundERLogFileEventHandler(&State, CFE_FS_FileWriteEvent_CREATE_ERROR, -1, 10, 10, 100);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_ERLOG2_ERR_EID), "CREATE_ERROR: CFE_ES_ERLOG2_ERR_EID generated");

    UT_ClearEventHistory();
    CFE_ES_BackgroundERLogFileEventHandler(&State, CFE_FS_FileWriteEvent_UNDEFINED, CFE_SUCCESS, 10, 0, 100);
    UtAssert_True(UT_GetNumEventsSent() == 0, "UNDEFINED: No event generated");
}

void TestGenericPool(void)
{
    CFE_ES_GenPoolRecord_t Pool1;
    CFE_ES_GenPoolRecord_t Pool2;
    size_t                 Offset1;
    size_t                 Offset2;
    size_t                 Offset3;
    size_t                 Offset4;
    size_t                 OffsetEnd;
    size_t                 BlockSize;
    CFE_ES_MemOffset_t     FreeSize;
    CFE_ES_MemOffset_t     TotalSize;
    uint16                 NumBlocks;
    uint32                 CountBuf;
    uint32                 ErrBuf;
    CFE_ES_BlockStats_t    BlockStats;
    static const size_t    UT_POOL_BLOCK_SIZES[CFE_PLATFORM_ES_POOL_MAX_BUCKETS] = {
        /*
         * These are intentionally in a mixed order
         * so that the implementation will sort them.
         */
        16, 56, 60, 40, 44, 48, 64, 128, 20, 24, 28, 12, 52, 32, 4, 8, 36};
    uint16 i;
    uint32 ExpectedCount;

    ES_ResetUnitTest();

    /* Test successfully creating direct access pool, with alignment, no mutex */
    memset(&UT_MemPoolDirectBuffer, 0xee, sizeof(UT_MemPoolDirectBuffer));
    OffsetEnd = sizeof(UT_MemPoolDirectBuffer.Data);
    UtAssert_INT32_EQ(CFE_ES_GenPoolInitialize(&Pool1, 0, OffsetEnd, 32, CFE_PLATFORM_ES_POOL_MAX_BUCKETS,
                                               UT_POOL_BLOCK_SIZES, ES_UT_PoolDirectRetrieve, ES_UT_PoolDirectCommit),
                      CFE_SUCCESS);

    /* Allocate buffers until no space left */
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset1, 44), CFE_SUCCESS);
    UtAssert_True(Offset1 > 0 && Offset1 < OffsetEnd, "0 < Offset(%lu) < %lu", (unsigned long)Offset1,
                  (unsigned long)OffsetEnd);
    UtAssert_True((Offset1 & 0x1F) == 0, "Offset(%lu) 32 byte alignment", (unsigned long)Offset1);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset2, 72), CFE_SUCCESS);
    UtAssert_True(Offset2 > Offset1 && Offset2 < OffsetEnd, "%lu < Offset(%lu) < %lu", (unsigned long)Offset1,
                  (unsigned long)Offset2, (unsigned long)OffsetEnd);
    UtAssert_True((Offset2 & 0x1F) == 0, "Offset(%lu) 32 byte alignment", (unsigned long)Offset2);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset3, 72), CFE_ES_ERR_MEM_BLOCK_SIZE);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset3, 6), CFE_SUCCESS);
    UtAssert_True(Offset3 > Offset2 && Offset3 < OffsetEnd, "%lu < Offset(%lu) < %lu", (unsigned long)Offset2,
                  (unsigned long)Offset3, (unsigned long)OffsetEnd);
    UtAssert_True((Offset3 & 0x1F) == 0, "Offset(%lu) 32 byte alignment", (unsigned long)Offset3);

    /* Free a buffer and attempt to reallocate */
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool1, &BlockSize, Offset2), CFE_SUCCESS);
    UtAssert_UINT32_EQ(BlockSize, 72);

    /* Should not be able to free more than once */
    /* This should increment the validation error count */
    UtAssert_ZERO(Pool1.ValidationErrorCount);
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool1, &BlockSize, Offset2), CFE_ES_POOL_BLOCK_INVALID);
    UtAssert_NONZERO(Pool1.ValidationErrorCount);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset4, 100), CFE_SUCCESS);
    UtAssert_True(Offset4 == Offset2, "New Offset(%lu) == Old Offset(%lu)", (unsigned long)Offset4,
                  (unsigned long)Offset2);

    /* Attempt Bigger than the largest bucket */
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset1, 1000), CFE_ES_ERR_MEM_BLOCK_SIZE);

    /* Call stats functions for coverage (no return code) */
    CFE_ES_GenPoolGetUsage(&Pool1, &FreeSize, &TotalSize);
    CFE_ES_GenPoolGetCounts(&Pool1, &NumBlocks, &CountBuf, &ErrBuf);
    CFE_ES_GenPoolGetBucketUsage(&Pool1, 1, &BlockStats);

    /* Check various outputs to ensure correctness */
    UtAssert_UINT32_EQ(TotalSize, OffsetEnd);
    UtAssert_UINT32_EQ(CountBuf, 3);
    UtAssert_True(FreeSize > 0, "FreeSize(%lu) > 0", (unsigned long)FreeSize);

    /* put blocks so the pool has a mixture of allocated an deallocated blocks */
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool1, &BlockSize, Offset1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool1, &BlockSize, Offset2), CFE_SUCCESS);

    /* Now wipe the pool management structure, and attempt to rebuild it. */
    UtAssert_INT32_EQ(CFE_ES_GenPoolInitialize(&Pool2, 0, OffsetEnd, 32, CFE_PLATFORM_ES_POOL_MAX_BUCKETS,
                                               UT_POOL_BLOCK_SIZES, ES_UT_PoolDirectRetrieve, ES_UT_PoolDirectCommit),
                      CFE_SUCCESS);

    UtAssert_INT32_EQ(CFE_ES_GenPoolRebuild(&Pool2), CFE_SUCCESS);

    /* After rebuilding, Pool2 should have similar state data to Pool1. */
    UtAssert_UINT32_EQ(Pool1.TailPosition, Pool2.TailPosition);
    UtAssert_UINT32_EQ(Pool1.AllocationCount, Pool2.AllocationCount);

    for (i = 0; i < Pool1.NumBuckets; ++i)
    {
        /* Allocation counts should match exactly */
        UtAssert_UINT32_EQ(Pool1.Buckets[i].AllocationCount, Pool2.Buckets[i].AllocationCount);

        /*
         * The recovery is not aware of how many times the block was
         * recycled, so this needs to be adjusted.
         */
        ExpectedCount = Pool1.Buckets[i].ReleaseCount - Pool1.Buckets[i].RecycleCount;
        UtAssert_UINT32_EQ(ExpectedCount, Pool2.Buckets[i].ReleaseCount);
    }

    /* Get blocks again, from the recovered pool, to demonstrate that
     * the pool is functional after recovery. */
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset3, 44), CFE_SUCCESS);
    UtAssert_UINT32_EQ(Offset3, Offset1);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset4, 72), CFE_SUCCESS);
    UtAssert_UINT32_EQ(Offset4, Offset2);

    /* Test successfully creating indirect memory pool, no alignment, with mutex */
    memset(&UT_MemPoolIndirectBuffer, 0xee, sizeof(UT_MemPoolIndirectBuffer));
    OffsetEnd = sizeof(UT_MemPoolIndirectBuffer.Data);
    UtAssert_INT32_EQ(CFE_ES_GenPoolInitialize(&Pool2, 2, OffsetEnd - 2, 0, CFE_PLATFORM_ES_POOL_MAX_BUCKETS,
                                               UT_POOL_BLOCK_SIZES, ES_UT_PoolIndirectRetrieve,
                                               ES_UT_PoolIndirectCommit),
                      CFE_SUCCESS);

    /* Do Series of allocations - confirm that the implementation is
     * properly adhering to the block sizes specified. */
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset1, 1), CFE_SUCCESS);

    /* With no alignment adjustments, the result offset should be exactly matching */
    UtAssert_True(Offset1 == 2 + sizeof(CFE_ES_GenPoolBD_t), "Offset(%lu) match", (unsigned long)Offset1);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset2, 55), CFE_SUCCESS);
    /* the previous block should be 4 in size (smallest block) */
    UtAssert_True(Offset2 == Offset1 + 4 + sizeof(CFE_ES_GenPoolBD_t), "Offset(%lu) match", (unsigned long)Offset2);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset3, 15), CFE_SUCCESS);
    /* the previous block should be 56 in size */
    UtAssert_True(Offset3 == Offset2 + 56 + sizeof(CFE_ES_GenPoolBD_t), "Offset(%lu) match", (unsigned long)Offset3);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset4, 54), CFE_SUCCESS);
    /* the previous block should be 16 in size */
    UtAssert_True(Offset4 == Offset3 + 16 + sizeof(CFE_ES_GenPoolBD_t), "Offset(%lu) match", (unsigned long)Offset4);

    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool2, &BlockSize, Offset1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool2, &BlockSize, Offset2), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool2, &BlockSize, Offset3), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool2, &BlockSize, Offset4), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset1, 56), CFE_SUCCESS);

    /* should re-issue previous block */
    UtAssert_True(Offset4 == Offset1, "Offset(%lu) match", (unsigned long)Offset1);

    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset3, 56), CFE_SUCCESS);

    /* should re-issue previous block */
    UtAssert_True(Offset3 == Offset2, "Offset(%lu) match", (unsigned long)Offset3);

    /* Getting another will fail, despite being enough space,
     * because its now fragmented. */
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset2, 12), CFE_ES_ERR_MEM_BLOCK_SIZE);

    /* Put the buffer, then corrupt the memory and try to recycle */
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool2, &BlockSize, Offset3), CFE_SUCCESS);
    memset(UT_MemPoolIndirectBuffer.Data, 0xee, sizeof(UT_MemPoolIndirectBuffer.Data));
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool2, &Offset3, 56), CFE_ES_ERR_MEM_BLOCK_SIZE);

    /* Test with just a single block size */
    ES_ResetUnitTest();
    memset(&UT_MemPoolDirectBuffer, 0xee, sizeof(UT_MemPoolDirectBuffer));
    /* Calculate exact (predicted) pool consumption per block */
    OffsetEnd = (sizeof(CFE_ES_GenPoolBD_t) + 31) & 0xFFF0;
    OffsetEnd *= 2; /* make enough for 2 */
    UtAssert_INT32_EQ(CFE_ES_GenPoolInitialize(&Pool1, 0, OffsetEnd, 16, 1, UT_POOL_BLOCK_SIZES,
                                               ES_UT_PoolDirectRetrieve, ES_UT_PoolDirectCommit),
                      CFE_SUCCESS);

    /*
     * This should be exactly enough for 2 allocations.
     * Allocation larger than 16 should fail.
     */
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset1, 32), CFE_ES_ERR_MEM_BLOCK_SIZE);
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset2, 1), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GenPoolGetBlock(&Pool1, &Offset3, 16), CFE_SUCCESS);

    /* Call stats functions for coverage (no return code) */
    CFE_ES_GenPoolGetUsage(&Pool1, &FreeSize, &TotalSize);
    CFE_ES_GenPoolGetCounts(&Pool1, &NumBlocks, &CountBuf, &ErrBuf);

    /* Check various outputs to ensure correctness */
    UtAssert_UINT32_EQ(TotalSize, OffsetEnd);
    UtAssert_UINT32_EQ(FreeSize, 0);
    UtAssert_UINT32_EQ(CountBuf, 2);

    /*
     * Check other validation
     */
    UtAssert_INT32_EQ(CFE_ES_GenPoolPutBlock(&Pool1, &BlockSize, 0), CFE_ES_BUFFER_NOT_IN_POOL);
    UtAssert_True(CFE_ES_GenPoolValidateState(&Pool1), "Nominal Handle validation");

    Pool1.TailPosition = 0xFFFFFF;
    UtAssert_True(!CFE_ES_GenPoolValidateState(&Pool1), "Validate Corrupt handle");
}

void TestTask(void)
{
    uint32    ResetType;
    osal_id_t UT_ContextTask;
    union
    {
        CFE_MSG_Message_t            Msg;
        CFE_ES_NoArgsCmd_t           NoArgsCmd;
        CFE_ES_ClearSysLogCmd_t      ClearSysLogCmd;
        CFE_ES_ClearERLogCmd_t       ClearERLogCmd;
        CFE_ES_ResetPRCountCmd_t     ResetPRCountCmd;
        CFE_ES_RestartCmd_t          RestartCmd;
        CFE_ES_StartAppCmd_t         StartAppCmd;
        CFE_ES_StopAppCmd_t          StopAppCmd;
        CFE_ES_RestartAppCmd_t       RestartAppCmd;
        CFE_ES_ReloadAppCmd_t        ReloadAppCmd;
        CFE_ES_QueryOneCmd_t         QueryOneCmd;
        CFE_ES_QueryAllCmd_t         QueryAllCmd;
        CFE_ES_OverWriteSysLogCmd_t  OverwriteSysLogCmd;
        CFE_ES_WriteSysLogCmd_t      WriteSysLogCmd;
        CFE_ES_WriteERLogCmd_t       WriteERLogCmd;
        CFE_ES_SetMaxPRCountCmd_t    SetMaxPRCountCmd;
        CFE_ES_DeleteCDSCmd_t        DeleteCDSCmd;
        CFE_ES_SendMemPoolStatsCmd_t SendMemPoolStatsCmd;
        CFE_ES_DumpCDSRegistryCmd_t  DumpCDSRegistryCmd;
        CFE_ES_QueryAllTasksCmd_t    QueryAllTasksCmd;
    } CmdBuf;
    CFE_ES_AppRecord_t *    UtAppRecPtr;
    CFE_ES_TaskRecord_t *   UtTaskRecPtr;
    CFE_ES_CDS_RegRec_t *   UtCDSRegRecPtr;
    CFE_ES_MemPoolRecord_t *UtPoolRecPtr;
    CFE_SB_MsgId_t          MsgId = CFE_SB_INVALID_MSG_ID;

    UtPrintf("Begin Test Task");

    /* Reset the log index; CFE_ES_TaskMain() calls CFE_ES_TaskInit() which
     * sets SystemLogMode to DISCARD, which can result in a log overflow
     * depending on the value that the index has reached from previous tests
     */
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx;

    /* Test task main process loop with a command pipe error */
    ES_ResetUnitTest();
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);

    /* Set up buffer for first cycle, pipe failure is on 2nd */
    UT_SetDataBuffer(UT_KEY(CFE_MSG_GetMsgId), &MsgId, sizeof(MsgId), false);
    CFE_ES_TaskMain();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_COMMAND_PIPE]), "CFE_ES_TaskMain",
              "Command pipe error, UT_OSP_COMMAND_PIPE message");

    /* Test task main process loop with bad checksum information */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_GetCFETextSegmentInfo), 1, -1);
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == CFE_SUCCESS, "CFE_ES_TaskInit",
              "Checksum fail, task init result");
    UT_Report(__FILE__, __LINE__, CFE_ES_Global.TaskData.HkPacket.Payload.CFECoreChecksum == 0xFFFF, "CFE_ES_TaskInit",
              "Checksum fail, checksum value");

    /* Test successful task main process loop - Power On Reset Path */
    ES_ResetUnitTest();
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    CFE_ES_Global.ResetDataPtr->ResetVars.ResetType = 2;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_SUCCESS && CFE_ES_Global.TaskData.HkPacket.Payload.CFECoreChecksum != 0xFFFF,
              "CFE_ES_TaskInit", "Checksum success, POR Path");

    /* Test successful task main process loop - Processor Reset Path */
    ES_ResetUnitTest();
    /* this is needed so CFE_ES_GetAppId works */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    CFE_ES_Global.ResetDataPtr->ResetVars.ResetType = 1;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_TaskInit() == CFE_SUCCESS && CFE_ES_Global.TaskData.HkPacket.Payload.CFECoreChecksum != 0xFFFF,
              "CFE_ES_TaskInit", "Checksum success, PR Path");

    /* Test task main process loop with a with an EVS register failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_Register), 1, -1);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == -1, "CFE_ES_TaskInit", "EVS register fail");

    /* Test task main process loop with a SB pipe create failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_CreatePipe), 1, -2);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == -2, "CFE_ES_TaskInit", "SB pipe create fail");

    /* Test task main process loop with a HK packet subscribe failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 1, -3);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == -3, "CFE_ES_TaskInit", "HK packet subscribe fail");

    /* Test task main process loop with a ground command subscribe failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_Subscribe), 2, -4);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == -4, "CFE_ES_TaskInit", "Ground command subscribe fail");

    /* Test task main process loop with an init event send failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 1, -5);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == -5, "CFE_ES_TaskInit", "Initialization event send fail");

    /* Test task main process loop with version event send failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_EVS_SendEvent), 2, -6);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == -6, "CFE_ES_TaskInit", "Version event send fail");

    /* Test task init with background init fail */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemCreate), 1, -7);
    UT_Report(__FILE__, __LINE__, CFE_ES_TaskInit() == -7, "CFE_ES_TaskInit", "Background init fail");

    /* Set the log mode to OVERWRITE; CFE_ES_TaskInit() sets SystemLogMode to
     * DISCARD, which can result in a log overflow depending on the value that
     * the index reaches during subsequent tests
     */
    CFE_ES_Global.ResetDataPtr->SystemLogMode = CFE_ES_LogMode_OVERWRITE;

    /* Test a successful HK request */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_ES_SEND_HK);
    UT_Report(__FILE__, __LINE__, CFE_ES_Global.TaskData.HkPacket.Payload.HeapBytesFree > 0, "CFE_ES_HousekeepingCmd",
              "HK packet - get heap successful");

    /* Test the HK request with a get heap failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_HeapGetInfo), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_ES_SEND_HK);
    UT_Report(__FILE__, __LINE__, CFE_ES_Global.TaskData.HkPacket.Payload.HeapBytesFree == 0, "CFE_ES_HousekeepingCmd",
              "HK packet - get heap fail");

    /* Test successful no-op command */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_ES_CMD_NOOP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_NOOP_INF_EID), "CFE_ES_NoopCmd", "No-op");

    /* Test successful reset counters command */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESET_INF_EID), "CFE_ES_ResetCountersCmd",
              "Reset counters");

    /* Test successful cFE restart */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount = 0;
    CmdBuf.RestartCmd.Payload.RestartType                     = CFE_PSP_RST_TYPE_PROCESSOR;
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Restart), &ResetType, sizeof(ResetType), false);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartCmd), UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__,
              ResetType == CFE_PSP_RST_TYPE_PROCESSOR && UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1,
              "CFE_ES_RestartCmd", "Restart cFE");

    /* Test cFE restart with bad restart type */
    ES_ResetUnitTest();
    CmdBuf.RestartCmd.Payload.RestartType = 4524;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartCmd), UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_BOOT_ERR_EID), "CFE_ES_RestartCmd",
              "Invalid restart type");

    /* Test successful app create */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "filename", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1);
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1] = '\0';
    memset(CmdBuf.StartAppCmd.Payload.Application, 'x', sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1);
    CmdBuf.StartAppCmd.Payload.Application[sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.Priority                                                        = 160;
    CmdBuf.StartAppCmd.Payload.StackSize       = CFE_ES_MEMOFFSET_C(8192);
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_INF_EID), "CFE_ES_StartAppCmd",
              "Start application from file name");

    /* Test app create with an OS task create failure */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_ERR_EID), "CFE_ES_StartAppCmd",
              "Start application from file name fail");

    /* Test app create with an invalid file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "123", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1);
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.Application, "appName", sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1);
    CmdBuf.StartAppCmd.Payload.Application[sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.Priority                                                        = 160;
    CmdBuf.StartAppCmd.Payload.StackSize       = CFE_ES_MEMOFFSET_C(12096);
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_INVALID_FILENAME_ERR_EID), "CFE_ES_StartAppCmd",
              "Invalid file name");

    /* Test app create with a null application entry point */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "filename", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[0]                                                = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.Application, "appName", sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1);
    CmdBuf.StartAppCmd.Payload.Application[sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.Priority                                                        = 160;
    CmdBuf.StartAppCmd.Payload.StackSize       = CFE_ES_MEMOFFSET_C(12096);
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_INVALID_ENTRY_POINT_ERR_EID), "CFE_ES_StartAppCmd",
              "Application entry point null");

    /* Test app create with a null application name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "filename", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1);
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.Application[0]                                                      = '\0';
    CmdBuf.StartAppCmd.Payload.Priority                                                            = 160;
    CmdBuf.StartAppCmd.Payload.StackSize       = CFE_ES_MEMOFFSET_C(12096);
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_NULL_APP_NAME_ERR_EID), "CFE_ES_StartAppCmd",
              "Application name null");

    /* Test app create with with an invalid exception action */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "filename", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1);
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.Application, "appName", sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1);
    CmdBuf.StartAppCmd.Payload.Application[sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.Priority                                                        = 160;
    CmdBuf.StartAppCmd.Payload.StackSize       = CFE_ES_MEMOFFSET_C(12096);
    CmdBuf.StartAppCmd.Payload.ExceptionAction = 255;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_EXC_ACTION_ERR_EID), "CFE_ES_StartAppCmd",
              "Invalid exception action");

    /* Test app create with a default stack size */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "filename", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1);
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.Application, "appName", sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1);
    CmdBuf.StartAppCmd.Payload.Application[sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.Priority                                                        = 160;
    CmdBuf.StartAppCmd.Payload.StackSize                                                       = CFE_ES_MEMOFFSET_C(0);
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_INF_EID), "CFE_ES_StartAppCmd",
              "Default Stack Size");

    /* Test app create with a bad priority */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "filename", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1);
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.Application, "appName", sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1);
    CmdBuf.StartAppCmd.Payload.Application[sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.Priority                                                        = 1000;
    CmdBuf.StartAppCmd.Payload.StackSize       = CFE_ES_MEMOFFSET_C(12096);
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_RESTART_APP;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_PRIORITY_ERR_EID), "CFE_ES_StartAppCmd",
              "Priority is too large");

    /* Test successful app stop */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.StopAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.StopAppCmd.Payload.Application) - 1);
    CmdBuf.StopAppCmd.Payload.Application[sizeof(CmdBuf.StopAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StopAppCmd), UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_STOP_DBG_EID), "CFE_ES_StopAppCmd",
              "Stop application initiated");

    /* Test app stop failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StopAppCmd), UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_STOP_ERR1_EID), "CFE_ES_StopAppCmd",
              "Stop application failed");

    /* Test app stop with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StopAppCmd.Payload.Application, "BAD_APP_NAME", sizeof(CmdBuf.StopAppCmd.Payload.Application) - 1);
    CmdBuf.StopAppCmd.Payload.Application[sizeof(CmdBuf.StopAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StopAppCmd), UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_STOP_ERR2_EID), "CFE_ES_StopAppCmd",
              "Stop application bad name");

    /* Test successful app restart */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.RestartAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1);
    CmdBuf.RestartAppCmd.Payload.Application[sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartAppCmd), UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_DBG_EID), "CFE_ES_RestartAppCmd",
              "Restart application initiated");

    /* Test app restart with failed file check */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_stat), OS_ERROR);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.RestartAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.RestartAppCmd.Payload.Application));
    CmdBuf.RestartAppCmd.Payload.Application[sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartAppCmd), UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR1_EID), "CFE_ES_RestartAppCmd",
              "Restart application failed");

    /* Test app restart with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.RestartAppCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1);
    CmdBuf.RestartAppCmd.Payload.Application[sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartAppCmd), UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR2_EID), "CFE_ES_RestartAppCmd",
              "Restart application bad name");

    /* Test failed app restart, core app */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.RestartAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1);
    CmdBuf.RestartAppCmd.Payload.Application[sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartAppCmd), UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR1_EID), "CFE_ES_RestartAppCmd",
              "Restart application failed");

    /* Test failed app restart, not running */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.RestartAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.RestartAppCmd.Payload.Application));
    CmdBuf.RestartAppCmd.Payload.Application[sizeof(CmdBuf.RestartAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartAppCmd), UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESTART_APP_ERR1_EID), "CFE_ES_RestartAppCmd",
              "Restart application failed");

    /* Test successful app reload */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.ReloadAppCmd.Payload.AppFileName, "New_Name", sizeof(CmdBuf.ReloadAppCmd.Payload.AppFileName) - 1);
    CmdBuf.ReloadAppCmd.Payload.AppFileName[sizeof(CmdBuf.ReloadAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.ReloadAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.ReloadAppCmd.Payload.Application) - 1);
    CmdBuf.ReloadAppCmd.Payload.Application[sizeof(CmdBuf.ReloadAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ReloadAppCmd), UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_DBG_EID), "CFE_ES_ReloadAppCmd",
              "Reload application initiated");

    /* Test app reload with missing file */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_stat), OS_ERROR);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.ReloadAppCmd.Payload.AppFileName, "New_Name", sizeof(CmdBuf.ReloadAppCmd.Payload.AppFileName));
    CmdBuf.ReloadAppCmd.Payload.AppFileName[sizeof(CmdBuf.ReloadAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.ReloadAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.ReloadAppCmd.Payload.Application));
    CmdBuf.ReloadAppCmd.Payload.Application[sizeof(CmdBuf.ReloadAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ReloadAppCmd), UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR1_EID), "CFE_ES_ReloadAppCmd",
              "Reload application failed");

    /* Test app reload with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.ReloadAppCmd.Payload.Application, "BAD_APP_NAME",
            sizeof(CmdBuf.ReloadAppCmd.Payload.Application) - 1);
    CmdBuf.ReloadAppCmd.Payload.Application[sizeof(CmdBuf.ReloadAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ReloadAppCmd), UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR2_EID), "CFE_ES_ReloadAppCmd",
              "Reload application bad name");

    /* Test failed app reload, core app */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.ReloadAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.ReloadAppCmd.Payload.Application));
    CmdBuf.ReloadAppCmd.Payload.Application[sizeof(CmdBuf.ReloadAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ReloadAppCmd), UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR1_EID), "CFE_ES_ReloadAppCmd",
              "Reload application failed");

    /* Test failed app reload, not RUNNING */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.ReloadAppCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.ReloadAppCmd.Payload.Application));
    CmdBuf.ReloadAppCmd.Payload.Application[sizeof(CmdBuf.ReloadAppCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ReloadAppCmd), UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RELOAD_APP_ERR1_EID), "CFE_ES_ReloadAppCmd",
              "Reload application failed");

    /* Test successful telemetry packet request for single app data */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_WAITING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.QueryOneCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.QueryOneCmd.Payload.Application) - 1);
    CmdBuf.QueryOneCmd.Payload.Application[sizeof(CmdBuf.QueryOneCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryOneCmd), UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ONE_APP_EID), "CFE_ES_QueryOneCmd",
              "Query application - success");

    /* Test telemetry packet request for single app data with send message failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.QueryOneCmd.Payload.Application, "CFE_ES", sizeof(CmdBuf.QueryOneCmd.Payload.Application) - 1);
    CmdBuf.QueryOneCmd.Payload.Application[sizeof(CmdBuf.QueryOneCmd.Payload.Application) - 1] = '\0';
    UT_SetDeferredRetcode(UT_KEY(CFE_SB_TransmitMsg), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryOneCmd), UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ONE_ERR_EID), "CFE_ES_QueryOneCmd",
              "Query application - SB send message fail");

    /* Test telemetry packet request for single app data with a bad app name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.QueryOneCmd.Payload.Application, "BAD_APP_NAME", sizeof(CmdBuf.QueryOneCmd.Payload.Application) - 1);
    CmdBuf.QueryOneCmd.Payload.Application[sizeof(CmdBuf.QueryOneCmd.Payload.Application) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryOneCmd), UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ONE_APPID_ERR_EID), "CFE_ES_QueryOneCmd",
              "Query application - bad application name");

    /* Test successful write of all app data to file */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    strncpy(CmdBuf.QueryAllCmd.Payload.FileName, "AllFilename", sizeof(CmdBuf.QueryAllCmd.Payload.FileName) - 1);
    CmdBuf.QueryAllCmd.Payload.FileName[sizeof(CmdBuf.QueryAllCmd.Payload.FileName) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllCmd), UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ALL_APPS_EID), "CFE_ES_QueryAllCmd",
              "Query all applications - success");

    /* Test write of all app data to file with a null file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllCmd), UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ALL_APPS_EID), "CFE_ES_QueryAllCmd",
              "Query all applications - null file name");

    /* Test write of all app data to file with a bad file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllCmd), UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_OSCREATE_ERR_EID), "CFE_ES_QueryAllCmd",
              "Query all applications - bad file name");

    /* Test write of all app data to file with a write header failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllCmd), UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_WRHDR_ERR_EID), "CFE_ES_QueryAllCmd",
              "Write application information file fail; write header");

    /* Test write of all app data to file with a file write failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllCmd), UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TASKWR_ERR_EID), "CFE_ES_QueryAllCmd",
              "Write application information file fail; task write");

    /* Test write of all app data to file with a file create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllCmd), UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_OSCREATE_ERR_EID), "CFE_ES_QueryAllCmd",
              "Write application information file fail; OS create");

    /* Test successful write of all task data to a file */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllTasksCmd),
                    UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TASKINFO_EID), "CFE_ES_QueryAllTasksCmd",
              "Task information file written");

    /* Test write of all task data to a file with file name validation failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllTasksCmd),
                    UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TASKINFO_OSCREATE_ERR_EID), "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; OS create");

    /* Test write of all task data to a file with write header failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.QueryAllTasksCmd.Payload.FileName, "filename", sizeof(CmdBuf.QueryAllTasksCmd.Payload.FileName) - 1);
    CmdBuf.QueryAllTasksCmd.Payload.FileName[sizeof(CmdBuf.QueryAllTasksCmd.Payload.FileName) - 1] = '\0';
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllTasksCmd),
                    UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TASKINFO_WRHDR_ERR_EID), "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; write header");

    /* Test write of all task data to a file with a task write failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllTasksCmd),
                    UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TASKINFO_WR_ERR_EID), "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; task write");

    /* Test write of all task data to a file with an OS create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllTasksCmd),
                    UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TASKINFO_OSCREATE_ERR_EID), "CFE_ES_QueryAllTasksCmd",
              "Task information file write fail; OS create");

    /* Test successful clearing of the system log */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ClearSysLogCmd), UT_TPID_CFE_ES_CMD_CLEAR_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SYSLOG1_INF_EID), "CFE_ES_ClearSysLogCmd",
              "Clear ES log data");

    /* Test successful overwriting of the system log using discard mode */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.OverwriteSysLogCmd.Payload.Mode = CFE_ES_LogMode_OVERWRITE;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.OverwriteSysLogCmd),
                    UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SYSLOGMODE_EID), "CFE_ES_OverWriteSysLogCmd",
              "Overwrite system log received (discard mode)");

    /* Test overwriting the system log using an invalid mode */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.OverwriteSysLogCmd.Payload.Mode = 255;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.OverwriteSysLogCmd),
                    UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ERR_SYSLOGMODE_EID), "CFE_ES_OverWriteSysLogCmd",
              "Overwrite system log using invalid mode");

    /* Test successful writing of the system log */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.WriteSysLogCmd.Payload.FileName, "filename", sizeof(CmdBuf.WriteSysLogCmd.Payload.FileName) - 1);
    CmdBuf.WriteSysLogCmd.Payload.FileName[sizeof(CmdBuf.WriteSysLogCmd.Payload.FileName) - 1] = '\0';
    CFE_ES_Global.TaskData.HkPacket.Payload.SysLogEntries                                      = 123;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteSysLogCmd), UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SYSLOG2_EID), "CFE_ES_WriteSysLogCmd",
              "Write system log; success");

    /* Test writing the system log using a bad file name */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteSysLogCmd), UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SYSLOG2_ERR_EID), "CFE_ES_WriteSysLogCmd",
              "Write system log; bad file name");

    /* Test writing the system log using a null file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.WriteSysLogCmd.Payload.FileName[0] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteSysLogCmd), UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SYSLOG2_EID), "CFE_ES_WriteSysLogCmd",
              "Write system log; null file name");

    /* Test writing the system log with an OS create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    CmdBuf.WriteSysLogCmd.Payload.FileName[0] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteSysLogCmd), UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SYSLOG2_ERR_EID), "CFE_ES_WriteSysLogCmd",
              "Write system log; OS create");

    /* Test writing the system log with an OS write failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx =
        snprintf(CFE_ES_Global.ResetDataPtr->SystemLog, sizeof(CFE_ES_Global.ResetDataPtr->SystemLog),
                 "0000-000-00:00:00.00000 Test Message\n");
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx = CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx;
    CmdBuf.WriteSysLogCmd.Payload.FileName[0]   = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteSysLogCmd), UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID), "CFE_ES_WriteSysLogCmd",
              "Write system log; OS write");

    /* Test writing the system log with a write header failure */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteSysLogCmd), UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID), "CFE_ES_WriteSysLogCmd",
              "Write system log; write header");

    /* Test successful clearing of the E&R log */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ClearERLogCmd), UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ERLOG1_INF_EID), "CFE_ES_ClearERLogCmd", "Clear E&R log");

    /* Test successful writing of the E&R log */
    /* In the current implementation, it does not directly write the file,
     * this just sets a flag for the background task */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.WriteERLogCmd.Payload.FileName, "filename", sizeof(CmdBuf.WriteERLogCmd.Payload.FileName) - 1);
    CmdBuf.WriteERLogCmd.Payload.FileName[sizeof(CmdBuf.WriteERLogCmd.Payload.FileName) - 1] = '\0';
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), false);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteERLogCmd), UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_FS_BackgroundFileDumpRequest)) == 1,
              "CFE_ES_WriteERLogCmd", "Write E&R log command; pending");
    UT_Report(__FILE__, __LINE__, !UT_EventIsInHistory(CFE_ES_ERLOG_PENDING_ERR_EID), "CFE_ES_WriteERLogCmd",
              "Write E&R log command; no events");

    /* Failure of parsing the file name */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteERLogCmd), UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ERLOG2_ERR_EID), "CFE_ES_WriteERLogCmd",
              "Write E&R log command; bad file name");

    /* Failure from CFE_FS_BackgroundFileDumpRequest() should send the pending error event ID */
    UT_ClearEventHistory();
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_BackgroundFileDumpRequest), 1, CFE_STATUS_REQUEST_ALREADY_PENDING);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteERLogCmd), UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ERLOG_PENDING_ERR_EID), "CFE_ES_WriteERLogCmd",
              "Write E&R log command; already pending event (from FS)");

    /* Same event but pending locally */
    UT_ClearEventHistory();
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_BackgroundFileDumpIsPending), true);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.WriteERLogCmd), UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ERLOG_PENDING_ERR_EID), "CFE_ES_WriteERLogCmd",
              "Write E&R log command; already pending event (local)");

    /* Test scan for exceptions in the PSP, should invoke a Processor Reset */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1, "CFE_ES_RunExceptionScan",
              "Scan for exceptions; processor restart");

    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount    = 0;
    CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount = 1;
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    CFE_ES_RunExceptionScan(0, NULL);
    /* first time should do a processor restart (limit reached) */
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1, "CFE_ES_RunExceptionScan",
              "Scan for exceptions; processor restart");
    /* next time should do a poweron restart (limit reached) */
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 2, "CFE_ES_RunExceptionScan",
              "Scan for exceptions; poweron restart");

    /* nominal for app restart - associate exception with a task ID */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    UT_ContextTask = CFE_ES_TaskId_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Exception_GetSummary), &UT_ContextTask, sizeof(UT_ContextTask), false);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->StartParams.ExceptionAction  = CFE_ES_ExceptionAction_RESTART_APP;
    CFE_ES_RunExceptionScan(0, NULL);
    /* should have changed AppControlRequest from RUN to SYS_RESTART,
     * and the call to CFE_PSP_Restart should NOT increment */
    UT_Report(__FILE__, __LINE__, UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_SYS_RESTART,
              "CFE_ES_RunExceptionScan", "Scan for exceptions; app restart request pending");

    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 0, "CFE_ES_RunExceptionScan",
              "Scan for exceptions; no psp restart");

    /* repeat, but for a CORE app, which cannot be restarted */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_Exception_GetCount), 1);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    UT_ContextTask = CFE_ES_TaskId_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetDataBuffer(UT_KEY(CFE_PSP_Exception_GetSummary), &UT_ContextTask, sizeof(UT_ContextTask), false);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->StartParams.ExceptionAction  = CFE_ES_ExceptionAction_RESTART_APP;
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 1, "CFE_ES_RunExceptionScan",
              "Scan for exceptions; core app, psp restart");

    /* check failure of getting summary data */
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_Exception_GetSummary), CFE_PSP_NO_EXCEPTION_DATA);
    CFE_ES_RunExceptionScan(0, NULL);
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(CFE_PSP_Restart)) == 2, "CFE_ES_RunExceptionScan",
              "Scan for exceptions; fail to get context");

    /* Test clearing the log with a bad size in the verify command
     * length call
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_ClearERLogCmd",
              "Packet length error");

    /* Test resetting and setting the max for the processor reset count */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.ResetPRCountCmd), UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_RESET_PR_COUNT_EID), "CFE_ES_ResetPRCountCmd",
              "Set processor reset count to zero");

    /* Test setting the maximum processor reset count */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.SetMaxPRCountCmd.Payload.MaxPRCount = 3;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.SetMaxPRCountCmd),
                    UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SET_MAX_PR_COUNT_EID), "CFE_ES_SetMaxPRCountCmd",
              "Set maximum processor reset count");

    /* Test failed deletion of specified CDS */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, false, &UtCDSRegRecPtr);
    UtCDSRegRecPtr->BlockOffset = 0xFFFFFFFF; /* Fails validation in PutBuf */
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.DeleteCDSCmd.Payload.CdsName, "CFE_ES.CDS_NAME", sizeof(CmdBuf.DeleteCDSCmd.Payload.CdsName) - 1);
    CmdBuf.DeleteCDSCmd.Payload.CdsName[sizeof(CmdBuf.DeleteCDSCmd.Payload.CdsName) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DeleteCDSCmd), UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_DELETE_ERR_EID), "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; error");

    /* Test failed deletion of specified critical table CDS */
    /* NOTE - reuse command from previous test */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, true, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DeleteCDSCmd), UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_DELETE_TBL_ERR_EID), "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; wrong type");

    /* Test successful deletion of a specified CDS */
    ES_ResetUnitTest();
    UT_SetCDSSize(0); /* defeats the "ReadFromCDS" and causes it to use the value here */
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, false, NULL);

    /* Set up the block to read what we need to from the CDS */
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DeleteCDSCmd), UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_DELETED_INFO_EID), "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; success");

    /* Test deletion of a specified CDS with the owning app being active */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, false, NULL);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DeleteCDSCmd), UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_OWNER_ACTIVE_EID), "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; owner active");

    /* Test deletion of a specified CDS with the name not found */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, false, &UtCDSRegRecPtr);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_BAD", NULL, NULL);
    CFE_ES_CDSBlockRecordSetFree(UtCDSRegRecPtr);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DeleteCDSCmd), UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_NAME_ERR_EID), "CFE_ES_DeleteCDSCmd",
              "Delete from CDS; not found");

    /* Test successful dump of CDS to file using the default dump file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DumpCDSRegistryCmd),
                    UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_REG_DUMP_INF_EID), "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; success (default dump file)");

    /* Test dumping of the CDS to a file with a bad file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_ParseInputFileNameEx), 1, CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DumpCDSRegistryCmd),
                    UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CREATING_CDS_DUMP_ERR_EID), "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; bad file name");

    /* Test dumping of the CDS to a file with a bad FS write header */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(CFE_FS_WriteHeader), 1, -1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DumpCDSRegistryCmd),
                    UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_WRITE_CFE_HDR_ERR_EID), "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; write header");

    /* Test dumping of the CDS to a file with an OS create failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DumpCDSRegistryCmd),
                    UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CREATING_CDS_DUMP_ERR_EID), "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; OS create");

    /* Test dumping of the CDS to a file with an OS write failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(OS_write), OS_ERROR);
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, false, NULL);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DumpCDSRegistryCmd),
                    UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_DUMP_ERR_EID), "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; OS write");

    /* Test telemetry pool statistics retrieval with an invalid handle */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.SendMemPoolStatsCmd),
                    UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_INVALID_POOL_HANDLE_ERR_EID), "CFE_ES_SendMemPoolStatsCmd",
              "Telemetry pool; bad handle");

    /* Test successful telemetry pool statistics retrieval */
    ES_ResetUnitTest();
    ES_UT_SetupMemPoolId(&UtPoolRecPtr);
    CmdBuf.SendMemPoolStatsCmd.Payload.PoolHandle = CFE_ES_MemPoolRecordGetID(UtPoolRecPtr);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.SendMemPoolStatsCmd),
                    UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TLM_POOL_STATS_INFO_EID), "CFE_ES_SendMemPoolStatsCmd",
              "Telemetry pool; success");

    /* Test the command pipe message process with an invalid command */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.NoArgsCmd), UT_TPID_CFE_ES_CMD_INVALID_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CC1_ERR_EID), "CFE_ES_TaskPipe", "Invalid ground command");

    /* Test sending a no-op command with an invalid command length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_NOOP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_NoopCmd",
              "No-op; invalid command length");

    /* Test sending a reset counters command with an invalid command length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_ResetCountersCmd",
              "Reset counters; invalid command length");

    /* Test sending a cFE restart command with an invalid command length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_RestartCmd",
              "Restart cFE; invalid command length");

    /* Test cFE restart with a power on reset */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.RestartCmd.Payload.RestartType = CFE_PSP_RST_TYPE_POWERON;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.RestartCmd), UT_TPID_CFE_ES_CMD_RESTART_CC);
    UT_Report(__FILE__, __LINE__, !UT_EventIsInHistory(CFE_ES_BOOT_ERR_EID), "CFE_ES_RestartCmd",
              "Power on reset restart type");

    /* Test sending a start application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_StartAppCmd",
              "Start application command; invalid command length");

    /* Test start application command with a processor restart on application
     * exception
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    strncpy(CmdBuf.StartAppCmd.Payload.AppFileName, "filename", sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1);
    CmdBuf.StartAppCmd.Payload.AppFileName[sizeof(CmdBuf.StartAppCmd.Payload.AppFileName) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.AppEntryPoint, "entrypoint",
            sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1);
    CmdBuf.StartAppCmd.Payload.AppEntryPoint[sizeof(CmdBuf.StartAppCmd.Payload.AppEntryPoint) - 1] = '\0';
    strncpy(CmdBuf.StartAppCmd.Payload.Application, "appName", sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1);
    CmdBuf.StartAppCmd.Payload.Application[sizeof(CmdBuf.StartAppCmd.Payload.Application) - 1] = '\0';
    CmdBuf.StartAppCmd.Payload.ExceptionAction = CFE_ES_ExceptionAction_PROC_RESTART;
    CmdBuf.StartAppCmd.Payload.Priority        = 160;
    CmdBuf.StartAppCmd.Payload.StackSize       = CFE_ES_MEMOFFSET_C(CFE_PLATFORM_ES_DEFAULT_STACK_SIZE);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.StartAppCmd), UT_TPID_CFE_ES_CMD_START_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_START_INF_EID), "CFE_ES_StartAppCmd",
              "Processor restart on application exception");

    /* Test sending a stop application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_STOP_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_StopAppCmd",
              "Stop application command; invalid command length");

    /* Test sending a restart application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_RESTART_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_RestartAppCmd",
              "Restart application command; invalid command length");

    /* Test sending a reload application command with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_RELOAD_APP_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_ReloadAppCmd",
              "Reload application command; invalid command length");

    /* Test sending a write request for a single application with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_QUERY_ONE_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_QueryOneAppCmd",
              "Query one application command; invalid command length");

    /* Test sending a write request for all applications with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_QueryAllAppCmd",
              "Query all applications command; invalid command length");

    /* Test write of all app data to file with a file open failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllCmd), UT_TPID_CFE_ES_CMD_QUERY_ALL_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_ALL_APPS_EID), "CFE_ES_QueryAllCmd",
              "Write application information file fail; file open");

    /* Test sending a write request for all tasks with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_QueryAllAppCmd",
              "Query all tasks command; invalid command length");

    /* Test write of all task data to file with a file open failure */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDeferredRetcode(UT_KEY(OS_OpenCreate), 1, OS_ERROR);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.QueryAllTasksCmd),
                    UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_TASKINFO_EID), "CFE_ES_QueryAllCmd",
              "Write task information file fail; file open");

    /* Test sending a request to clear the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_CLEAR_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_ClearSysLogCmd",
              "Clear system log command; invalid command length");

    /* Test sending a request to overwrite the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_OverwriteSysLogCmd",
              "Overwrite system log command; invalid command length");

    /* Test sending a request to write the system log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_WriteSysLogCmd",
              "Write system log command; invalid command length");

    /* Test successful overwriting of the system log using overwrite mode */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.OverwriteSysLogCmd.Payload.Mode = CFE_ES_LogMode_OVERWRITE;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.OverwriteSysLogCmd),
                    UT_TPID_CFE_ES_CMD_OVER_WRITE_SYSLOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_SYSLOGMODE_EID), "CFE_ES_OverWriteSysLogCmd",
              "Overwrite system log received (overwrite mode)");

    /* Test sending a request to write the error log with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_WriteErrlogCmd",
              "Write error log command; invalid command length");

    /* Test sending a request to reset the processor reset count with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_ResetPRCountCmd",
              "Reset processor reset count command; invalid command length");

    /* Test sending a request to set the maximum processor reset count with
     * an invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_SetMaxPRCountCmd",
              "Set maximum processor reset count command; invalid "
              "command length");

    /* Test sending a request to delete the CDS with an invalid command
     * length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_DELETE_CDS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_DeleteCDSCmd",
              "Delete CDS command; invalid command length");

    /* Test sending a telemetry pool statistics retrieval command with an
     * invalid command length
     */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_LEN_ERR_EID), "CFE_ES_DeleteCDSCmd",
              "Telemetry pool command; invalid command length");

    /* Test successful dump of CDS to file using a specified dump file name */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, false, NULL);
    strncpy(CmdBuf.DumpCDSRegistryCmd.Payload.DumpFilename, "DumpFile",
            sizeof(CmdBuf.DumpCDSRegistryCmd.Payload.DumpFilename) - 1);
    CmdBuf.DumpCDSRegistryCmd.Payload.DumpFilename[sizeof(CmdBuf.DumpCDSRegistryCmd.Payload.DumpFilename) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.DumpCDSRegistryCmd),
                    UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_CDS_REG_DUMP_INF_EID), "CFE_ES_DumpCDSRegistryCmd",
              "Dump CDS; success (dump file specified)");
} /* end TestTask */

void TestPerf(void)
{
    union
    {
        CFE_MSG_Message_t              Msg;
        CFE_ES_StartPerfDataCmd_t      PerfStartCmd;
        CFE_ES_StopPerfDataCmd_t       PerfStopCmd;
        CFE_ES_SetPerfFilterMaskCmd_t  PerfSetFilterMaskCmd;
        CFE_ES_SetPerfTriggerMaskCmd_t PerfSetTrigMaskCmd;
    } CmdBuf;

    UtPrintf("Begin Test Performance Log");

    CFE_ES_PerfData_t *Perf;

    /*
    ** Set the pointer to the data area
    */
    UT_GetDataBuffer(UT_KEY(CFE_PSP_GetResetArea), (void **)&ES_UT_PersistentResetData, NULL, NULL);
    Perf = &ES_UT_PersistentResetData->Perf;

    /* Test successful performance mask and value initialization */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    Perf->MetaData.State = CFE_ES_PERF_MAX_STATES;
    CFE_ES_SetupPerfVariables(CFE_PSP_RST_TYPE_PROCESSOR);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.State == CFE_ES_PERF_IDLE, "CFE_ES_SetupPerfVariables",
              "Idle data collection");

    /* Test successful performance data collection start in START
     * trigger mode
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd), UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID), "CFE_ES_StartPerfDataCmd",
              "Collect performance data; mode START");

    /* Test successful performance data collection start in CENTER
     * trigger mode
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_CENTER;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd), UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID), "CFE_ES_StartPerfDataCmd",
              "Collect performance data; mode CENTER");

    /* Test successful performance data collection start in END
     * trigger mode
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_END;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd), UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID), "CFE_ES_StartPerfDataCmd",
              "Collect performance data; mode END");

    /* Test performance data collection start with an invalid trigger mode
     * (too high)
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = (CFE_ES_PERF_TRIGGER_END + 1);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd), UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_TRIG_ERR_EID), "CFE_ES_StartPerfDataCmd",
              "Trigger mode out of range (high)");

    /* Test performance data collection start with an invalid trigger mode
     * (too low)
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = 0xffffffff;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd), UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_TRIG_ERR_EID), "CFE_ES_StartPerfDataCmd",
              "Trigger mode out of range (low)");

    /* Test performance data collection start with a file write in progress */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    /* clearing the BackgroundPerfDumpState will fully reset to initial state */
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    CFE_ES_Global.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_INIT;
    CmdBuf.PerfStartCmd.Payload.TriggerMode            = CFE_ES_PERF_TRIGGER_START;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd), UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_ERR_EID), "CFE_ES_StartPerfDataCmd",
              "Cannot collect performance data; write in progress");

    /* Test performance data collection by sending another valid
     * start command
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfStartCmd.Payload.TriggerMode = CFE_ES_PERF_TRIGGER_START;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStartCmd), UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID), "CFE_ES_StartPerfDataCmd",
              "Start collecting performance data");

    /* Test successful performance data collection stop */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStopCmd), UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_EID), "CFE_ES_StopPerfDataCmd",
              "Stop collecting performance data");

    /* Test performance data collection stop with a file name validation issue */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    UT_SetDefaultReturnValue(UT_KEY(CFE_FS_ParseInputFileNameEx), CFE_FS_INVALID_PATH);
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStopCmd), UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_LOG_ERR_EID), "CFE_ES_StopPerfDataCmd",
              "Stop performance data command bad file name");

    /* Test successful performance data collection stop with a non-default
         file name */
    ES_ResetUnitTest();

    /* clearing the BackgroundPerfDumpState will fully reset to initial state */
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    strncpy(CmdBuf.PerfStopCmd.Payload.DataFileName, "filename", sizeof(CmdBuf.PerfStopCmd.Payload.DataFileName) - 1);
    CmdBuf.PerfStopCmd.Payload.DataFileName[sizeof(CmdBuf.PerfStopCmd.Payload.DataFileName) - 1] = '\0';
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStopCmd), UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_EID), "CFE_ES_StopPerfDataCmd",
              "Stop collecting performance data (non-default file name)");

    /* Test performance data collection stop with a file write in progress */
    ES_ResetUnitTest();
    CFE_ES_Global.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_INIT;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfStopCmd), UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_ERR2_EID), "CFE_ES_StopPerfDataCmd",
              "Stop performance data command ignored");

    /* Test performance filter mask command with out of range filter
         mask value */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetFilterMaskCmd.Payload.FilterMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfSetFilterMaskCmd),
                    UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_FILTMSKERR_EID), "CFE_ES_SetPerfFilterMaskCmd",
              "Performance filter mask command error; index out of range");

    /* Test successful performance filter mask command */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetFilterMaskCmd.Payload.FilterMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK / 2;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfSetFilterMaskCmd),
                    UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_FILTMSKCMD_EID), "CFE_ES_SetPerfFilterMaskCmd",
              "Set performance filter mask command received");

    /* Test successful performance filter mask command with minimum filter
         mask value */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetTrigMaskCmd.Payload.TriggerMaskNum = 0;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfSetTrigMaskCmd),
                    UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKCMD_EID), "CFE_ES_SetPerfTriggerMaskCmd",
              "Set performance trigger mask command received; minimum index");

    /* Test successful performance filter mask command with maximum filter
     * mask value
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetTrigMaskCmd.Payload.TriggerMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK - 1;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfSetTrigMaskCmd),
                    UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKCMD_EID), "CFE_ES_SetPerfTriggerMaskCmd",
              "Set performance trigger mask command received; maximum index");

    /* Test successful performance filter mask command with a greater than the
     * maximum filter mask value
     */
    ES_ResetUnitTest();
    memset(&CmdBuf, 0, sizeof(CmdBuf));
    CmdBuf.PerfSetTrigMaskCmd.Payload.TriggerMaskNum = CFE_ES_PERF_32BIT_WORDS_IN_MASK + 1;
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, sizeof(CmdBuf.PerfSetTrigMaskCmd),
                    UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__, UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKERR_EID), "CFE_ES_SetPerfTriggerMaskCmd",
              "Performance trigger mask command error; index out of range");

    /* Test successful addition of a new entry to the performance log */
    ES_ResetUnitTest();
    Perf->MetaData.State                 = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.TriggerCount          = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE + 1;
    Perf->MetaData.InvalidMarkerReported = false;
    Perf->MetaData.DataEnd               = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE + 1;
    CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.InvalidMarkerReported == true, "CFE_ES_PerfLogAdd",
              "Invalid performance marker");

    /* Test addition of a new entry to the performance log with START
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.InvalidMarkerReported = true;
    Perf->MetaData.Mode                  = CFE_ES_PERF_TRIGGER_START;
    Perf->MetaData.DataCount             = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE + 1;
    Perf->MetaData.TriggerMask[0]        = 0xFFFF;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_START && Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd", "Triggered; START");

    /* Test addition of a new entry to the performance log with CENTER
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.State        = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode         = CFE_ES_PERF_TRIGGER_CENTER;
    Perf->MetaData.TriggerCount = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE / 2 + 1;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_CENTER && Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd", "Triggered; CENTER");

    /* Test addition of a new entry to the performance log with END
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode  = CFE_ES_PERF_TRIGGER_END;
    CFE_ES_PerfLogAdd(1, 0);
    UT_Report(__FILE__, __LINE__,
              Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_END && Perf->MetaData.State == CFE_ES_PERF_IDLE,
              "CFE_ES_PerfLogAdd", "Triggered; END");

    /* Test addition of a new entry to the performance log with an invalid
     * marker after an invalid marker has already been reported
     */
    ES_ResetUnitTest();
    Perf->MetaData.State                 = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.InvalidMarkerReported = 2;
    CFE_ES_PerfLogAdd(CFE_MISSION_ES_PERF_MAX_IDS + 1, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.InvalidMarkerReported == 2, "CFE_ES_PerfLogAdd",
              "Invalid marker after previous invalid marker");

    /* Test addition of a new entry to the performance log with a marker that
     * is not in the filter mask
     */
    ES_ResetUnitTest();
    Perf->MetaData.State         = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.FilterMask[0] = 0x0;
    Perf->MetaData.DataEnd       = 0;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.DataEnd == 0, "CFE_ES_PerfLogAdd", "Marker not in filter mask");

    /* Test addition of a new entry to the performance log with the data count
     * below the maximum allowed
     */
    ES_ResetUnitTest();
    Perf->MetaData.State         = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.DataCount     = 0;
    Perf->MetaData.FilterMask[0] = 0xffff;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.DataCount == 1, "CFE_ES_PerfLogAdd", "Data count below maximum");

    /* Test addition of a new entry to the performance log with a marker that
     * is not in the trigger mask
     */
    ES_ResetUnitTest();
    Perf->MetaData.State          = CFE_ES_PERF_WAITING_FOR_TRIGGER;
    Perf->MetaData.TriggerMask[0] = 0x0;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.State != CFE_ES_PERF_TRIGGERED, "CFE_ES_PerfLogAdd",
              "Marker not in trigger mask");

    /* Test addition of a new entry to the performance log with a start
     * trigger mode and the trigger count is less the buffer size
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount   = 0;
    Perf->MetaData.State          = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode           = CFE_ES_PERF_TRIGGER_START;
    Perf->MetaData.TriggerMask[0] = 0xffff;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.TriggerCount == 1, "CFE_ES_PerfLogAdd",
              "Start trigger mode; trigger count less than the buffer size");

    /* Test addition of a new entry to the performance log with a center
     * trigger mode and the trigger count is less than half the buffer size
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE / 2 - 2;
    Perf->MetaData.State        = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode         = CFE_ES_PERF_TRIGGER_CENTER;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.State != CFE_ES_PERF_IDLE, "CFE_ES_PerfLogAdd",
              "Center trigger mode; trigger count less than half the "
              "buffer size");

    /* Test addition of a new entry to the performance log with an invalid
     * trigger mode
     */
    ES_ResetUnitTest();
    Perf->MetaData.TriggerCount = 0;
    Perf->MetaData.State        = CFE_ES_PERF_TRIGGERED;
    Perf->MetaData.Mode         = -1;
    CFE_ES_PerfLogAdd(0x1, 0);
    UT_Report(__FILE__, __LINE__, Perf->MetaData.State != CFE_ES_PERF_IDLE, "CFE_ES_PerfLogAdd",
              "Invalid trigger mode");

    /* Test performance data collection start with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, !UT_EventIsInHistory(CFE_ES_PERF_STARTCMD_EID), "CFE_ES_StartPerfDataCmd",
              "Invalid message length");

    /* Test performance data collection stop with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC);
    UT_Report(__FILE__, __LINE__, !UT_EventIsInHistory(CFE_ES_PERF_STOPCMD_EID), "CFE_ES_StopPerfDataCmd",
              "Invalid message length");

    /* Test performance data filer mask with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC);
    UT_Report(__FILE__, __LINE__, !UT_EventIsInHistory(CFE_ES_PERF_FILTMSKCMD_EID), "CFE_ES_SetPerfFilterMaskCmd",
              "Invalid message length");

    /* Test performance data trigger mask with an invalid message length */
    ES_ResetUnitTest();
    UT_CallTaskPipe(CFE_ES_TaskPipe, &CmdBuf.Msg, 0, UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC);
    UT_Report(__FILE__, __LINE__, !UT_EventIsInHistory(CFE_ES_PERF_TRIGMSKCMD_EID), "CFE_ES_SetPerfTriggerMaskCmd",
              "Invalid message length");

    /* Test perf log dump state machine */
    /* Nominal call 1 - should go through up to the DELAY state */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    CFE_ES_Global.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_INIT;
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_Global.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_Global.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_DELAY,
                  "CFE_ES_RunPerfLogDump - CFE_ES_Global.BackgroundPerfDumpState.CurrentState (%d) == DELAY (%d)",
                  (int)CFE_ES_Global.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_DELAY);
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_OpenCreate)) == 1, "CFE_ES_RunPerfLogDump - OS_OpenCreate() called");

    /* Nominal call 2 - should go through up to the remainder of states, back to IDLE */
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_Global.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_Global.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_IDLE,
                  "CFE_ES_RunPerfLogDump - CFE_ES_Global.BackgroundPerfDumpState.CurrentState (%d) == IDLE (%d)",
                  (int)CFE_ES_Global.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_IDLE);
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_close)) == 1, "CFE_ES_RunPerfLogDump - OS_close() called");

    /* Test a failure to open the output file */
    /* This should go immediately back to idle, and generate CFE_ES_PERF_LOG_ERR_EID */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    CFE_ES_Global.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_INIT;
    UT_SetDefaultReturnValue(UT_KEY(OS_OpenCreate), -10);
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_Global.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_Global.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_IDLE,
                  "CFE_ES_RunPerfLogDump - OS create fail, CFE_ES_Global.BackgroundPerfDumpState.CurrentState (%d) "
                  "== IDLE (%d)",
                  (int)CFE_ES_Global.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_IDLE);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_PERF_LOG_ERR_EID),
                  "CFE_ES_RunPerfLogDump - OS create fail, generated CFE_ES_PERF_LOG_ERR_EID");

    /* Test a failure to write to the output file */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    UT_SetDefaultReturnValue(UT_KEY(OS_write), -10);
    CFE_ES_Global.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_INIT;
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_Global.BackgroundPerfDumpState);
    UtAssert_True(CFE_ES_Global.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_DELAY,
                  "CFE_ES_RunPerfLogDump - OS_write fail, CFE_ES_Global.BackgroundPerfDumpState.CurrentState (%d) == "
                  "DELAY (%d)",
                  (int)CFE_ES_Global.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_DELAY);

    /* This will trigger the OS_write() failure, which should go through up to the remainder of states, back to IDLE */
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_Global.BackgroundPerfDumpState);
    UtAssert_True(
        CFE_ES_Global.BackgroundPerfDumpState.CurrentState == CFE_ES_PerfDumpState_IDLE,
        "CFE_ES_RunPerfLogDump - OS_write fail, CFE_ES_Global.BackgroundPerfDumpState.CurrentState (%d) == IDLE (%d)",
        (int)CFE_ES_Global.BackgroundPerfDumpState.CurrentState, (int)CFE_ES_PerfDumpState_IDLE);
    UtAssert_True(UT_EventIsInHistory(CFE_ES_FILEWRITE_ERR_EID),
                  "CFE_ES_RunPerfLogDump - OS_write fail, generated CFE_ES_FILEWRITE_ERR_EID");

    /* Test the ability of the file writer to handle the "wrap around" from the end of
     * the perflog buffer back to the beginning.  Just need to set up the metadata
     * so that the writing position is toward the end of the buffer.
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    OS_OpenCreate(&CFE_ES_Global.BackgroundPerfDumpState.FileDesc, "UT", 0, OS_WRITE_ONLY);
    CFE_ES_Global.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES;
    CFE_ES_Global.BackgroundPerfDumpState.PendingState = CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES;
    CFE_ES_Global.BackgroundPerfDumpState.DataPos      = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE - 2;
    CFE_ES_Global.BackgroundPerfDumpState.StateCounter = 4;
    CFE_ES_RunPerfLogDump(1000, &CFE_ES_Global.BackgroundPerfDumpState);
    /* check that the wraparound occurred */
    UtAssert_True(CFE_ES_Global.BackgroundPerfDumpState.DataPos == 2,
                  "CFE_ES_RunPerfLogDump - wraparound, DataPos (%u) == 2",
                  (unsigned int)CFE_ES_Global.BackgroundPerfDumpState.DataPos);
    /* should have written 4 entries to the log */
    UtAssert_True(CFE_ES_Global.BackgroundPerfDumpState.FileSize == sizeof(CFE_ES_PerfDataEntry_t) * 4,
                  "CFE_ES_RunPerfLogDump - wraparound, FileSize (%u) == sizeof(CFE_ES_PerfDataEntry_t) * 4",
                  (unsigned int)CFE_ES_Global.BackgroundPerfDumpState.FileSize);

    /* Confirm that the "CFE_ES_GetPerfLogDumpRemaining" function works.
     * This requires that the state is not idle, in order to get nonzero results.
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    OS_OpenCreate(&CFE_ES_Global.BackgroundPerfDumpState.FileDesc, "UT", 0, OS_WRITE_ONLY);
    CFE_ES_Global.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_WRITE_PERF_METADATA;
    CFE_ES_Global.BackgroundPerfDumpState.StateCounter = 10;
    Perf->MetaData.DataCount                           = 100;
    /* in states other than WRITE_PERF_ENTRIES, it should report the full size of the log */
    UtAssert_True(CFE_ES_GetPerfLogDumpRemaining() == 100, " CFE_ES_GetPerfLogDumpRemaining - Setup Phase");
    /* in WRITE_PERF_ENTRIES, it should report the StateCounter */
    CFE_ES_Global.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES;
    UtAssert_True(CFE_ES_GetPerfLogDumpRemaining() == 10, " CFE_ES_GetPerfLogDumpRemaining - Active Phase");
}

void TestAPI(void)
{
    osal_id_t            TestObjId;
    char                 AppName[OS_MAX_API_NAME + 12];
    uint32               StackBuf[8];
    int32                Return;
    uint8                Data[12];
    uint32               ResetType;
    uint32 *             ResetTypePtr;
    CFE_ES_AppId_t       AppId;
    CFE_ES_TaskId_t      TaskId;
    uint32               RunStatus;
    CFE_ES_TaskInfo_t    TaskInfo;
    CFE_ES_AppInfo_t     AppInfo;
    CFE_ES_AppRecord_t * UtAppRecPtr;
    CFE_ES_TaskRecord_t *UtTaskRecPtr;

    UtPrintf("Begin Test API");

    /* Test resetting the cFE with a processor reset */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_PROCESSOR;
    CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount - 1;
    CFE_ES_ResetCFE(ResetType);
    CFE_ES_Global.ResetDataPtr->ResetVars.ProcessorResetCount =
        CFE_ES_Global.ResetDataPtr->ResetVars.MaxProcessorResetCount;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_MAX_PROC_RESETS]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_PROC_RESET_COMMANDED]),
              "CFE_ES_ResetCFE", "Processor reset");

    /* Test getting the reset type using a valid pointer and a null pointer */
    ES_ResetUnitTest();
    Return       = CFE_ES_GetResetType(&ResetType);
    ResetTypePtr = NULL;
    CFE_ES_GetResetType(ResetTypePtr);
    UT_Report(__FILE__, __LINE__, Return == CFE_PSP_RST_TYPE_PROCESSOR && ResetTypePtr == NULL, "CFE_ES_GetResetType",
              "Get reset type successful");

    /* Test resetting the cFE with a power on reset */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_POWERON;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_ResetCFE(ResetType) == CFE_ES_NOT_IMPLEMENTED &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_COMMANDED]),
              "CFE_ES_ResetCFE", "Power on reset");

    /* Test resetting the cFE with an invalid argument */
    ES_ResetUnitTest();
    ResetType = CFE_PSP_RST_TYPE_POWERON + 3;
    UT_Report(__FILE__, __LINE__, CFE_ES_ResetCFE(ResetType) == CFE_ES_BAD_ARGUMENT, "CFE_ES_ResetCFE",
              "Bad reset type");

    /* Test restarting an app that doesn't exist */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_APPID_C(
        ES_UT_MakeAppIdForIndex(CFE_PLATFORM_ES_MAX_APPLICATIONS - 1)); /* Should be within range, but not used */
    UT_Report(__FILE__, __LINE__, CFE_ES_RestartApp(AppId) == CFE_ES_ERR_RESOURCEID_NOT_VALID, "CFE_ES_RestartApp",
              "Bad application ID");

    /* Test restarting an app with an ID out of range (high) */
    ES_ResetUnitTest();
    AppId = CFE_ES_APPID_C(ES_UT_MakeAppIdForIndex(99999));
    UT_Report(__FILE__, __LINE__, CFE_ES_RestartApp(AppId) == CFE_ES_ERR_RESOURCEID_NOT_VALID, "CFE_ES_RestartApp",
              "Application ID too large");

    /* Test reloading an app that doesn't exist */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_ReloadApp(AppId, "filename") == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_ReloadApp", "Bad application ID");

    /* Test deleting an app that doesn't exist */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteApp(AppId) == CFE_ES_ERR_RESOURCEID_NOT_VALID, "CFE_ES_DeleteApp",
              "Bad application ID");

    /* Test exiting an app with an init error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    UT_Report(__FILE__, __LINE__,
              UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_INIT]) &&
                  UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_POR_MAX_PROC_RESETS]),
              "CFE_ES_ExitApp", "Application initialization error");

    /* Test exiting an app with a runtime error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_STOPPED, NULL, &UtAppRecPtr, NULL);
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_RUNTIME]), "CFE_ES_ExitApp",
              "Application runtime error");

    /* Test exiting an app with an exit error */
    /* Note - this exit code of 1000 is invalid, which causes
     * an extra message to be logged in syslog about this.  This
     * should also be stored in the AppControlRequest as APP_ERROR. */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_STOPPED, "UT", &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    CFE_ES_ExitApp(1000);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CORE_APP_EXIT]), "CFE_ES_ExitApp",
              "Application exit error");
    UtAssert_True(UtAppRecPtr->ControlReq.AppControlRequest == CFE_ES_RunStatus_APP_ERROR,
                  "CFE_ES_ExitApp - AppControlRequest (%u) == CFE_ES_RunStatus_APP_ERROR (%u)",
                  (unsigned int)UtAppRecPtr->ControlReq.AppControlRequest, (unsigned int)CFE_ES_RunStatus_APP_ERROR);

#if 0
    /* Can't cover this path since it contains a while(1) (i.e.,
     * infinite) loop
     */
    OS_TaskCreate(&TestObjId, "UT", NULL, NULL, 0, 0, 0);
    Id = ES_UT_OSALID_TO_ARRAYIDX(TestObjId);
    CFE_ES_TaskRecordSetUsed(TaskRecPtr);
    TaskRecPtr->AppId = Id;
    AppRecPtr->Type = CFE_ES_AppType_EXTERNAL;
    AppRecPtr->AppState = CFE_ES_AppState_RUNNING;
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);
#endif

    /* Test successful run loop app run request */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus                                 = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UT_Report(__FILE__, __LINE__, CFE_ES_RunLoop(&RunStatus) == true, "CFE_ES_RunLoop", "Request to run application");

    /* Test successful run loop app stop request */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus                                 = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    UT_Report(__FILE__, __LINE__, CFE_ES_RunLoop(&RunStatus) == false, "CFE_ES_RunLoop",
              "Request to stop running application");

    /* Test successful run loop app exit request */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus                                 = CFE_ES_RunStatus_APP_EXIT;
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    UT_Report(__FILE__, __LINE__, CFE_ES_RunLoop(&RunStatus) == false, "CFE_ES_RunLoop", "Request to exit application");

    /* Test run loop with bad app ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    RunStatus                                 = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    CFE_ES_TaskRecordSetFree(UtTaskRecPtr); /* make it so task ID is bad */
    UT_Report(__FILE__, __LINE__, CFE_ES_RunLoop(&RunStatus) == false, "CFE_ES_RunLoop", "Bad internal application ID");

    /* Test run loop with an invalid run status */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    RunStatus                                 = 1000;
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_EXIT;
    UT_Report(__FILE__, __LINE__, CFE_ES_RunLoop(&RunStatus) == false, "CFE_ES_RunLoop", "Invalid run status");

    /* Test run loop with a NULL run status */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UT_Report(__FILE__, __LINE__, CFE_ES_RunLoop(NULL), "CFE_ES_RunLoop", "Nominal, NULL output pointer");

    /* Test run loop with startup sync code */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_LATE_INIT, NULL, &UtAppRecPtr, NULL);
    RunStatus                                 = CFE_ES_RunStatus_APP_RUN;
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_RUN;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RunLoop(&RunStatus) == true && UtAppRecPtr->AppState == CFE_ES_AppState_RUNNING, "CFE_ES_RunLoop",
              "Status change from initializing to run");

    /* Test getting the cFE application and task ID by context */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppID(&AppId) == CFE_SUCCESS, "CFE_ES_GetAppID",
              "Get application ID by context successful");
    UT_Report(__FILE__, __LINE__, CFE_ES_GetTaskID(&TaskId) == CFE_SUCCESS, "CFE_ES_GetTaskID",
              "Get task ID by context successful");

    /* Test getting the app name with a bad app ID */
    ES_ResetUnitTest();
    AppId = CFE_ES_APPID_C(ES_UT_MakeAppIdForIndex(99999));
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppName(AppName, AppId, sizeof(AppName)) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetAppName", "Get application name by ID; bad application ID");

    /* Test getting the app name with that app ID out of range */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    AppId = CFE_ES_APPID_C(ES_UT_MakeAppIdForIndex(99999));
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppName(AppName, AppId, sizeof(AppName)) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetAppName", "Get application name by ID; ID out of range");

    /* Test successfully getting the app name using the app ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
    AppId = CFE_ES_AppRecordGetID(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetAppName(AppName, AppId, sizeof(AppName)) == CFE_SUCCESS,
              "CFE_ES_GetAppName", "Get application name by ID successful");

    /* Test getting task information using the task ID - NULL buffer */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetTaskInfo(NULL, TaskId) == CFE_ES_BAD_ARGUMENT, "CFE_ES_GetTaskInfo",
              "Get task info by ID; NULL buffer");

    /* Test getting task information using the task ID - bad task ID  */
    UT_SetDefaultReturnValue(UT_KEY(OS_ObjectIdToArrayIndex), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetTaskInfo", "Get task info by ID; bad task ID");

    /* Test getting task information using the task ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);
    TaskId                = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UtAppRecPtr->AppState = CFE_ES_AppState_RUNNING;
    UT_Report(__FILE__, __LINE__, CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_SUCCESS, "CFE_ES_GetTaskInfo",
              "Get task info by ID successful");

    /* Test getting task information using the task ID with parent inactive */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    CFE_ES_AppRecordSetFree(UtAppRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetTaskInfo", "Get task info by ID; parent application not active");

    /* Test getting task information using the task ID with task inactive */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    CFE_ES_TaskRecordSetFree(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetTaskInfo", "Get task info by ID; task not active");

    /* Test getting task information using the task ID with invalid task ID */
    ES_ResetUnitTest();
    TaskId = CFE_ES_TASKID_UNDEFINED;
    UT_Report(__FILE__, __LINE__, CFE_ES_GetTaskInfo(&TaskInfo, TaskId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetTaskInfo", "Get task info by ID; invalid task ID");

    /* Test creating a child task with a bad app ID */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId, "TaskName", TestAPI, StackBuf, sizeof(StackBuf), 400, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_ERR_RESOURCEID_NOT_VALID, "CFE_ES_ChildTaskCreate",
              "Bad application ID");

    /* Test creating a child task with an OS task create failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, NULL, NULL);
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskCreate), OS_ERROR);
    Return = CFE_ES_CreateChildTask(&TaskId, "TaskName", TestAPI, StackBuf, sizeof(StackBuf), 400, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_STATUS_EXTERNAL_RESOURCE_FAIL, "CFE_ES_ChildTaskCreate",
              "OS task create failed");

    /* Test creating a child task with a null task ID */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(NULL, "TaskName", TestAPI, StackBuf, sizeof(StackBuf), 400, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_ChildTaskCreate", "Task ID null");

    /* Test creating a child task with a null task name */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId, NULL, TestAPI, StackBuf, sizeof(StackBuf), 400, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_ChildTaskCreate", "Task name null");

    /* Test creating a child task with a null task ID and name */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(NULL, NULL, TestAPI, StackBuf, sizeof(StackBuf), 400, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_ChildTaskCreate", "Task name and ID null");

    /* Test creating a child task with a null function pointer */
    ES_ResetUnitTest();
    Return = CFE_ES_CreateChildTask(&TaskId, "TaskName", NULL, StackBuf, sizeof(StackBuf), 2, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_BAD_ARGUMENT, "CFE_ES_ChildTaskCreate", "Function pointer null");

    /* Test creating a child task within a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    TestObjId = CFE_ES_TaskId_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetId), OS_ObjectIdToInteger(TestObjId)); /* Set context to that of child */
    Return = CFE_ES_CreateChildTask(&TaskId, "TaskName", TestAPI, StackBuf, sizeof(StackBuf), 400, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_ES_ERR_CHILD_TASK_CREATE, "CFE_ES_CreateChildTask",
              "Cannot call from a child task");

    /* Test successfully creating a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    Return = CFE_ES_CreateChildTask(&TaskId, "TaskName", TestAPI, StackBuf, sizeof(StackBuf), 400, 0);
    UT_Report(__FILE__, __LINE__, Return == CFE_SUCCESS, "CFE_ES_CreateChildTask", "Create child task successful");

    /* Test common entry point */
    ES_ResetUnitTest();

    /*
     * Without no app/task set up the entry point will not be found.
     * There is no return value to check here, it just will not do anything.
     */
    CFE_ES_TaskEntryPoint();

    /* Now set up an app+task */
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, &UtTaskRecPtr);

    /* Test with app/task set up but no entry point defined */
    CFE_ES_TaskEntryPoint();

    /* Finally set entry point, nominal mode */
    UtTaskRecPtr->EntryFunc = ES_UT_TaskFunction;
    CFE_ES_TaskEntryPoint();
    UtAssert_STUB_COUNT(ES_UT_TaskFunction, 1);

    /* Test deleting a child task using a main task's ID */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", NULL, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteChildTask(TaskId) == CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK,
              "CFE_ES_DeleteChildTask", "Task ID belongs to a main task");

    /* Test deleting a child task with an invalid task ID */
    UT_SetDefaultReturnValue(UT_KEY(OS_ObjectIdToArrayIndex), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteChildTask(TaskId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_DeleteChildTask", "Task ID invalid");

    /* Test successfully deleting a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    AppId  = CFE_ES_AppRecordGetID(UtAppRecPtr);   /* the app ID */
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr); /* the child task ID */
    Return = CFE_ES_GetAppInfo(&AppInfo, AppId);
    UtAssert_True(Return == CFE_SUCCESS, "CFE_ES_GetAppInfo() return=%x", (unsigned int)Return);
    UtAssert_True(AppInfo.NumOfChildTasks == 1, "AppInfo.NumOfChildTaskss == %u",
                  (unsigned int)AppInfo.NumOfChildTasks);
    Return = CFE_ES_DeleteChildTask(TaskId);
    UtAssert_True(Return == CFE_SUCCESS, "DeleteChildResult() return=%x", (unsigned int)Return);
    Return = CFE_ES_GetAppInfo(&AppInfo, AppId);
    UtAssert_True(Return == CFE_SUCCESS, "CFE_ES_GetAppInfo() return=%x", (unsigned int)Return);
    UtAssert_True(AppInfo.NumOfChildTasks == 0, "AppInfo.NumOfChildTaskss == %u",
                  (unsigned int)AppInfo.NumOfChildTasks);

    /* Test deleting a child task with an OS task delete failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    AppId  = CFE_ES_AppRecordGetID(UtAppRecPtr);   /* the app ID */
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr); /* the child task ID */
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskDelete), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteChildTask(TaskId) <= 0, "CFE_ES_DeleteChildTask",
              "OS task delete failure");

    /* Test deleting a child task with the task ID out of range */
    ES_ResetUnitTest();
    TaskId = CFE_ES_TASKID_UNDEFINED;
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteChildTask(TaskId) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_DeleteChildTask", "Task ID too large");

    /* Test successfully exiting a child task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    TestObjId = CFE_ES_TaskId_ToOSAL(CFE_ES_TaskRecordGetID(UtTaskRecPtr));
    UT_SetDefaultReturnValue(UT_KEY(OS_TaskGetId), OS_ObjectIdToInteger(TestObjId)); /* Set context to that of child */
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__, UT_GetStubCount(UT_KEY(OS_TaskExit)) == 1, "CFE_ES_ExitChildTask",
              "Exit child task successful");

    /* Test exiting a child task within an app main task */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, NULL, &UtAppRecPtr, NULL);
    ES_UT_SetupChildTaskId(UtAppRecPtr, NULL, &UtTaskRecPtr);
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_CANNOT_CALL_APP_MAIN]),
              "CFE_ES_ExitChildTask", "Cannot call from a cFE application main task");

    /* Test exiting a child task with an error retrieving the app ID */
    ES_ResetUnitTest();
    CFE_ES_ExitChildTask();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_TASKEXIT_BAD_CONTEXT]),
              "CFE_ES_ExitChildTask", "Invalid context");

    /* Test successfully adding a time-stamped message to the system log that
     * must be truncated
     */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE - CFE_TIME_PRINTED_STRING_SIZE - 4;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx;
    CFE_ES_Global.ResetDataPtr->SystemLogMode     = CFE_ES_LogMode_DISCARD;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_SysLogWrite_Unsync("SysLogText This message should be truncated") == CFE_ES_ERR_SYS_LOG_TRUNCATED,
              "CFE_ES_SysLogWrite_Internal", "Add message to log that must be truncated");

    /* Reset the system log index to prevent an overflow in later tests */
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = 0;

    /* Test calculating a CRC on a range of memory using CRC type 8
     * NOTE: This capability is not currently implemented in cFE
     */
    memset(Data, 1, sizeof(Data));
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_MISSION_ES_CRC_8) == 0,
              "CFE_ES_CalculateCRC", "*Not implemented* CRC-8 algorithm");

    /* Test calculating a CRC on a range of memory using CRC type 16 */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_MISSION_ES_CRC_16) == 2688,
              "CFE_ES_CalculateCRC", "CRC-16 algorithm - memory read successful");

    /*
     * CRC memory read failure test case removed in #322 -
     * deprecated CFE_PSP_MemRead8, now the FSW code does a direct read
     * which has no failure path.
     */

    /* Test calculating a CRC on a range of memory using CRC type 32
     * NOTE: This capability is not currently implemented in cFE
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CalculateCRC(&Data, 12, 345353, CFE_MISSION_ES_CRC_32) == 0,
              "CFE_ES_CalculateCRC", "*Not implemented* CRC-32 algorithm");

    /* Test calculating a CRC on a range of memory using an invalid CRC type
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CalculateCRC(&Data, 12, 345353, -1) == 0, "CFE_ES_CalculateCRC",
              "Invalid CRC type");

    /* Test shared mutex take with a take error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, -1);
    CFE_ES_LockSharedData(__func__, 12345);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MUTEX_TAKE]), "CFE_ES_LockSharedData",
              "Mutex take error");

    /* Test shared mutex release with a release error */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, -1);
    CFE_ES_UnlockSharedData(__func__, 98765);
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_MUTEX_GIVE]), "CFE_ES_UnlockSharedData",
              "Mutex release error");

    /* Test waiting for apps to initialize before continuing; transition from
     * initializing to running
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_EARLY_INIT, "UT", &UtAppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_OPERATIONAL;
    CFE_ES_WaitForStartupSync(0);
    UT_Report(__FILE__, __LINE__, UtAppRecPtr->AppState == CFE_ES_AppState_RUNNING, "CFE_ES_WaitForStartupSync",
              "Transition from initializing to running");

    /* Test waiting for apps to initialize before continuing with the semaphore
     * already released
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    CFE_ES_WaitForStartupSync(99);

    /* Note - CFE_ES_WaitForStartupSync() returns void, nothing to check for
     * here.  This is for code coverage
     */
    UT_Report(__FILE__, __LINE__, 1, "CFE_ES_WaitForStartupSync", "System state core ready");

    /* Test waiting for apps to initialize as an external app
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_EARLY_INIT, "UT", &UtAppRecPtr, NULL);
    CFE_ES_Global.SystemState = CFE_ES_SystemState_CORE_READY;
    CFE_ES_WaitForStartupSync(99);

    /* Note - CFE_ES_WaitForStartupSync() returns void, nothing to check for
     * here.  This is for code coverage
     */
    UT_Report(__FILE__, __LINE__, 1, "CFE_ES_WaitForStartupSync", "System state operational");

    /* Test adding a time-stamped message to the system log using an invalid
     * log mode
     *
     * TEST CASE REMOVED as the invalid log mode follow the same path as Discard,
     * this test case added nothing new
     */

    /* Test successfully adding a time-stamped message to the system log that
     * causes the log index to be reset
     */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx;
    CFE_ES_Global.ResetDataPtr->SystemLogMode     = CFE_ES_LogMode_DISCARD;
    UT_Report(__FILE__, __LINE__, CFE_ES_WriteToSysLog("SysLogText") == CFE_ES_ERR_SYS_LOG_FULL, "CFE_ES_WriteToSysLog",
              "Add message to log that resets the log index");

    /* Test successfully adding a time-stamped message to the system log that
     * causes the log index to be reset
     */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx;
    CFE_ES_Global.ResetDataPtr->SystemLogMode     = CFE_ES_LogMode_OVERWRITE;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_WriteToSysLog("SysLogText") == CFE_SUCCESS &&
                  CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx < CFE_PLATFORM_ES_SYSTEM_LOG_SIZE,
              "CFE_ES_WriteToSysLog", "Add message to log that resets the log index");

    /* Test run loop with an application error status */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, NULL);
    RunStatus                                 = CFE_ES_RunStatus_APP_ERROR;
    UtAppRecPtr->ControlReq.AppControlRequest = CFE_ES_RunStatus_APP_ERROR;
    UT_Report(__FILE__, __LINE__, CFE_ES_RunLoop(&RunStatus) == false, "CFE_ES_RunLoop",
              "Application error run status");

    /*
     * Test public Name+ID query/lookup API for tasks
     * This just uses OSAL routines to implement, but need to cover error paths.
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_EXTERNAL, CFE_ES_AppState_RUNNING, "UT", &UtAppRecPtr, &UtTaskRecPtr);
    TaskId = CFE_ES_TaskRecordGetID(UtTaskRecPtr);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(AppName, CFE_ES_TASKID_UNDEFINED, sizeof(AppName)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(NULL, TaskId, sizeof(AppName)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(AppName, TaskId, sizeof(AppName)), CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_GetResourceName), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_ES_GetTaskName(AppName, TaskId, sizeof(AppName)), CFE_ES_ERR_RESOURCEID_NOT_VALID);

    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskId, NULL), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskId, AppName), CFE_SUCCESS);
    UT_SetDeferredRetcode(UT_KEY(OS_TaskGetIdByName), 1, OS_ERROR);
    UtAssert_INT32_EQ(CFE_ES_GetTaskIDByName(&TaskId, "Nonexistent"), CFE_ES_ERR_NAME_NOT_FOUND);
}

void TestGenericCounterAPI(void)
{
    char               CounterName[11];
    CFE_ES_CounterId_t CounterId;
    CFE_ES_CounterId_t CounterId2;
    uint32             CounterCount;
    int                i;

    /* Test successfully registering a generic counter */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterGenCounter(&CounterId, "Counter1") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter", "Register counter successful");

    /* Test registering a generic counter that is already registered */
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterGenCounter(&CounterId, "Counter1") == CFE_ES_ERR_DUPLICATE_NAME,
              "CFE_ES_RegisterGenCounter", "Attempt to register an existing counter");

    /* Test registering the maximum number of generic counters */
    for (i = 1; i < CFE_PLATFORM_ES_MAX_GEN_COUNTERS; i++)
    {
        snprintf(CounterName, sizeof(CounterName), "Counter%d", i + 1);

        if (CFE_ES_RegisterGenCounter(&CounterId, CounterName) != CFE_SUCCESS)
        {
            break;
        }
    }

    UT_Report(__FILE__, __LINE__, i == CFE_PLATFORM_ES_MAX_GEN_COUNTERS, "CFE_ES_RegisterGenCounter",
              "Register maximum number of counters");

    /* Test registering a generic counter after the maximum are registered */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterGenCounter(&CounterId, "Counter999") == CFE_ES_NO_RESOURCE_IDS_AVAILABLE,
              "CFE_ES_RegisterGenCounter", "Maximum number of counters exceeded");
    UT_ResetState(UT_KEY(CFE_ResourceId_FindNext));

    /* Check operation of the CFE_ES_CheckCounterIdSlotUsed() helper function */
    CFE_ES_Global.CounterTable[1].CounterId = CFE_ES_COUNTERID_C(ES_UT_MakeCounterIdForIndex(1));
    CFE_ES_Global.CounterTable[2].CounterId = CFE_ES_COUNTERID_UNDEFINED;
    UtAssert_True(CFE_ES_CheckCounterIdSlotUsed(ES_UT_MakeCounterIdForIndex(1)), "Counter Slot Used");
    UtAssert_True(!CFE_ES_CheckCounterIdSlotUsed(ES_UT_MakeCounterIdForIndex(2)), "Counter Slot Unused");

    /* Test getting a registered generic counter that doesn't exist */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetGenCounterIDByName(&CounterId, "Counter999") == CFE_ES_ERR_NAME_NOT_FOUND,
              "CFE_ES_GetGenCounterIDByName", "Cannot get counter that does not exist");

    /* Test successfully getting a registered generic counter ID by name */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetGenCounterIDByName(&CounterId, "Counter5") == CFE_SUCCESS,
              "CFE_ES_GetGenCounterIDByName", "Get generic counter ID successful");

    /* Test deleting a registered generic counter that doesn't exist */
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteGenCounter(CFE_ES_COUNTERID_UNDEFINED) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_DeleteGenCounter", "Cannot delete counter that does not exist");

    /* Test successfully deleting a registered generic counter by ID */
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteGenCounter(CounterId) == CFE_SUCCESS, "CFE_ES_DeleteGenCounter",
              "Successful");

    /* Test successfully registering a generic counter to verify a place for
     * it is now available and to provide an ID for subsequent tests
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterGenCounter(&CounterId, "CounterX") == CFE_SUCCESS,
              "CFE_ES_RegisterGenCounter", "Register counter; back to maximum number");

    /* Test incrementing a generic counter that doesn't exist */
    UT_Report(__FILE__, __LINE__, CFE_ES_IncrementGenCounter(CFE_ES_COUNTERID_UNDEFINED) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_IncrementGenCounter", "Bad counter ID");

    /* Test successfully incrementing a generic counter */
    UT_Report(__FILE__, __LINE__, CFE_ES_IncrementGenCounter(CounterId) == CFE_SUCCESS, "CFE_ES_IncrementGenCounter",
              "Increment counter successful");

    /* Test getting a generic counter value for a counter that doesn't exist */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetGenCount(CFE_ES_COUNTERID_UNDEFINED, &CounterCount) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCount", "Bad counter ID");

    /* Test successfully getting a generic counter value */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetGenCount(CounterId, &CounterCount) == CFE_SUCCESS && CounterCount == 1,
              "CFE_ES_GetGenCount", "Get counter value successful");

    /* Test setting a generic counter value for a counter that doesn't exist */
    UT_Report(__FILE__, __LINE__, CFE_ES_SetGenCount(CFE_ES_COUNTERID_UNDEFINED, 5) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_SetGenCount", "Bad counter ID");

    /* Test successfully setting a generic counter value */
    UT_Report(__FILE__, __LINE__, CFE_ES_SetGenCount(CounterId, 5) == CFE_SUCCESS, "CFE_ES_SetGenCount",
              "Set counter value successful");

    /* Test value retrieved from a generic counter value */
    CFE_ES_GetGenCount(CounterId, &CounterCount);
    UT_Report(__FILE__, __LINE__, (CounterCount == 5), "CFE_ES_SetGenCount", "Check value for counter set");

    /* Test registering a generic counter with a null counter ID pointer */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterGenCounter(NULL, "Counter1") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter", "Attempt to register using a null counter ID pointer");

    /* Test registering a generic counter with a null counter name */
    ES_ResetUnitTest();

    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterGenCounter(&CounterId, NULL) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_RegisterGenCounter", "Attempt to register using a null counter name");

    /* Test incrementing a generic counter where the record is not in use */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_IncrementGenCounter(CounterId) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_IncrementGenCounter", "Record not in use");

    /* Test setting a generic counter where the record is not in use */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_SetGenCount(CounterId, 0) == CFE_ES_BAD_ARGUMENT, "CFE_ES_SetGenCount",
              "Record not in use");

    /* Test getting a generic counter where the record is not in use */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_GetGenCount(CounterId, &CounterCount) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCount", "Record not in use");

    /* Test getting a generic counter where the count is null */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_GetGenCount(CounterId, NULL) == CFE_ES_BAD_ARGUMENT, "CFE_ES_GetGenCount",
              "Null count");

    /* Test getting a registered generic counter ID using a null counter
     * pointer
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_GetGenCounterIDByName(NULL, "CounterX") == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_GetGenCounterIDByName", "Null name");

    /*
     * Test Name-ID query/conversion API
     */
    ES_ResetUnitTest();
    UtAssert_INT32_EQ(CFE_ES_RegisterGenCounter(&CounterId, "Counter1"), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterName(CounterName, CounterId, sizeof(CounterName)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterIDByName(&CounterId2, "Nonexistent"), CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterIDByName(&CounterId2, CounterName), CFE_SUCCESS);
    UtAssert_True(CFE_RESOURCEID_TEST_EQUAL(CounterId, CounterId2), "Counter IDs Match");
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterName(CounterName, CFE_ES_COUNTERID_UNDEFINED, sizeof(CounterName)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterName(NULL, CounterId, sizeof(CounterName)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetGenCounterIDByName(&CounterId, NULL), CFE_ES_BAD_ARGUMENT);
}

void TestCDS()
{
    size_t               CdsSize;
    uint8 *              CdsPtr;
    char                 CDSName[CFE_MISSION_ES_CDS_MAX_FULL_NAME_LEN + 4];
    CFE_ES_CDSHandle_t   CDSHandle;
    CFE_ES_CDS_RegRec_t *UtCDSRegRecPtr;
    uint32               i;
    uint32               TempSize;
    uint8                BlockData[ES_UT_CDS_BLOCK_SIZE];

    UtPrintf("Begin Test CDS");

    /* Test init with a mutex create failure */
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == CFE_STATUS_EXTERNAL_RESOURCE_FAIL, "CFE_ES_CDS_EarlyInit",
              "Mutex create failed");

    /* Test locking the CDS registry with a mutex take failure */
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemTake), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_LockCDS() == CFE_STATUS_EXTERNAL_RESOURCE_FAIL, "CFE_ES_LockCDS",
              "Mutex take failed");

    /* Test unlocking the CDS registry with a mutex give failure */
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemGive), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_UnlockCDS() == CFE_STATUS_EXTERNAL_RESOURCE_FAIL, "CFE_ES_UnlockCDS",
              "Mutex give failed");

    /* Set up the PSP stubs for CDS testing */
    UT_SetCDSSize(128 * 1024);

    /* Test the CDS Cache Fetch/Flush/Load routine error cases */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_CacheFetch(&CFE_ES_Global.CDSVars.Cache, 4, 0) == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_CDS_CacheFetch", "Invalid Size");
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_CacheFlush(&CFE_ES_Global.CDSVars.Cache) == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_CDS_CacheFlush", "Invalid Size");

    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_CachePreload(&CFE_ES_Global.CDSVars.Cache, NULL, 4, 0) == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_CDS_CachePreload", "Invalid Size");
    TempSize = 5;
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_CachePreload(&CFE_ES_Global.CDSVars.Cache, &TempSize, 4, 4) == CFE_SUCCESS,
              "CFE_ES_CDS_CachePreload", "Nominal");

    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_CacheFetch(&CFE_ES_Global.CDSVars.Cache, 4, 4) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_CDS_CacheFetch", "Access error");

    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_CacheFlush(&CFE_ES_Global.CDSVars.Cache) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_CDS_CacheFlush", "Access Error");

    /* Test CDS registering with a write CDS failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    ES_UT_SetupCDSGlobal(ES_UT_CDS_SMALL_TEST_SIZE);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, "Name3") == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RegisterCDS", "Writing to BSP CDS failure");

    /* Test successful CDS registering */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    ES_UT_SetupCDSGlobal(ES_UT_CDS_SMALL_TEST_SIZE);
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_SUCCESS, "CFE_ES_RegisterCDS",
              "Register CDS successful");

    /* Test CDS registering using an already registered name */
    /* No reset here -- just attempt to register the same name again */
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_ES_CDS_ALREADY_EXISTS,
              "CFE_ES_RegisterCDS", "Retrieve existing CDS");

    /* Test CDS registering using the same name, but a different size */
    /* No reset here -- just attempt to register the same name again */
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 6, "Name") == CFE_SUCCESS, "CFE_ES_RegisterCDS",
              "Get CDS of same name, but new size");

    /* Test CDS registering using a null name */
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, "") == CFE_ES_CDS_INVALID_NAME,
              "CFE_ES_RegisterCDS", "Invalid name size");

    /* Test CDS registering with a block size of zero */
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 0, "Name") == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_RegisterCDS", "Block size zero");

    /* Test CDS registering with no memory pool available */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, "Name") == CFE_ES_NOT_IMPLEMENTED,
              "CFE_ES_RegisterCDS", "No memory pool available");

    /* Test CDS registering with all the CDS registries taken */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    ES_UT_SetupCDSGlobal(ES_UT_CDS_SMALL_TEST_SIZE);

    /* Set all the CDS registries to 'taken' */
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, "Name2") == CFE_ES_NO_RESOURCE_IDS_AVAILABLE,
              "CFE_ES_RegisterCDS", "No available entries");

    /* Check operation of the CFE_ES_CheckCDSHandleSlotUsed() helper function */
    CFE_ES_Global.CDSVars.Registry[1].BlockID = CFE_ES_CDSHANDLE_C(ES_UT_MakeCDSIdForIndex(1));
    CFE_ES_Global.CDSVars.Registry[2].BlockID = CFE_ES_CDS_BAD_HANDLE;
    UtAssert_True(CFE_ES_CheckCDSHandleSlotUsed(ES_UT_MakeCDSIdForIndex(1)), "CDS Slot Used");
    UtAssert_True(!CFE_ES_CheckCDSHandleSlotUsed(ES_UT_MakeCDSIdForIndex(2)), "CDS Slot Unused");

    /* Test CDS registering using a bad app ID */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, "Name2") == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_RegisterCDS", "Bad application ID");

    /* Test copying to CDS with bad handle */
    CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    UT_Report(__FILE__, __LINE__, CFE_ES_CopyToCDS(CDSHandle, &TempSize) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_CopyToCDS", "Copy to CDS bad handle");
    /* Test restoring from a CDS with bad handle */
    UT_Report(__FILE__, __LINE__, CFE_ES_RestoreFromCDS(&TempSize, CDSHandle) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_RestoreFromCDS", "Restore from CDS bad handle");

    /* Test successfully copying to a CDS */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_SUCCESS);
    ES_UT_SetupSingleCDSRegistry("UT", ES_UT_CDS_BLOCK_SIZE, false, &UtCDSRegRecPtr);
    CDSHandle = CFE_ES_CDSBlockRecordGetID(UtCDSRegRecPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_CopyToCDS(CDSHandle, &BlockData) == CFE_SUCCESS, "CFE_ES_CopyToCDS",
              "Copy to CDS successful");

    /* Test successfully restoring from a CDS */
    UT_Report(__FILE__, __LINE__, CFE_ES_RestoreFromCDS(&BlockData, CDSHandle) == CFE_SUCCESS, "CFE_ES_RestoreFromCDS",
              "Restore from CDS successful");

    /* Test CDS registering using a name longer than the maximum allowed */
    ES_ResetUnitTest();
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "UT", NULL, NULL);
    ES_UT_SetupCDSGlobal(ES_UT_CDS_SMALL_TEST_SIZE);

    for (i = 0; i < CFE_MISSION_ES_CDS_MAX_NAME_LENGTH + 1; i++)
    {
        CDSName[i] = 'a';
    }

    CDSName[i] = '\0';

    UT_Report(__FILE__, __LINE__, CFE_ES_RegisterCDS(&CDSHandle, 4, CDSName) == CFE_ES_CDS_INVALID_NAME,
              "CFE_ES_RegisterCDS", "Invalid name size");

    /* Test unsuccessful CDS registering */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, CDS_ABS_MAX_BLOCK_SIZE + 1, "Name") == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_RegisterCDS", "Register CDS unsuccessful");

    UT_Report(__FILE__, __LINE__,
              CFE_ES_RegisterCDS(&CDSHandle, CDS_ABS_MAX_BLOCK_SIZE - 1, "Name") == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_RegisterCDS", "Register CDS unsuccessful");

    /* Test memory pool rebuild and registry recovery with an
     * unreadable registry
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, -1);
    UT_Report(__FILE__, __LINE__, CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID, "CFE_ES_RebuildCDS",
              "First read from CDS bad");

    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, -1);
    UT_Report(__FILE__, __LINE__, CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID, "CFE_ES_RebuildCDS",
              "Second read from CDS bad");

    /* Test CDS registry initialization with a CDS write failure */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, -1);
    UT_Report(__FILE__, __LINE__, CFE_ES_InitCDSRegistry() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_InitCDSRegistry",
              "Failed to write registry size");

    /* Test successful CDS initialization */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == CFE_SUCCESS, "CFE_ES_CDS_EarlyInit",
              "Initialization successful");

    /* Test CDS initialization with a read error */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, -1);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_CDS_EarlyInit",
              "Unrecoverable read error");

    /* Test CDS initialization with size below the minimum */
    ES_ResetUnitTest();
    UT_SetCDSSize(1024);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_CDS_EarlyInit() == CFE_SUCCESS && UT_GetStubCount(UT_KEY(CFE_PSP_GetCDSSize)) == 1,
              "CFE_ES_CDS_EarlyInit", "CDS size less than minimum");

    /* Test CDS initialization with size not obtainable */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_PSP_GetCDSSize), -1);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == OS_ERROR, "CFE_ES_CDS_EarlyInit",
              "Unable to obtain CDS size");

    /* Reset back to a sufficient CDS size */
    UT_SetCDSSize(128 * 1024);
    UT_GetDataBuffer(UT_KEY(CFE_PSP_ReadFromCDS), (void **)&CdsPtr, &CdsSize, NULL);

    /* Test CDS initialization with rebuilding not possible */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 3, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == CFE_SUCCESS, "CFE_ES_CDS_EarlyInit",
              "Rebuilding not possible; create new CDS");

    /* Test CDS validation with first CDS read call failure */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_ValidateCDS() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_ValidateCDS",
              "CDS read (first call) failed");

    /* Test CDS validation with second CDS read call failure */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_ValidateCDS() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_ValidateCDS",
              "CDS read (second call) failed");

    /* Test CDS validation with CDS read end check failure */
    memset(CdsPtr + CdsSize - CFE_ES_CDS_SIGNATURE_LEN, 'x', CFE_ES_CDS_SIGNATURE_LEN);
    UT_Report(__FILE__, __LINE__, CFE_ES_ValidateCDS() == CFE_ES_CDS_INVALID, "CFE_ES_ValidateCDS",
              "Reading from CDS failed end check");

    /* Test CDS validation with CDS read begin check failure */
    UT_GetDataBuffer(UT_KEY(CFE_PSP_ReadFromCDS), (void **)&CdsPtr, &CdsSize, NULL);
    memset(CdsPtr, 'x', CFE_ES_CDS_SIGNATURE_LEN);
    UT_Report(__FILE__, __LINE__, CFE_ES_ValidateCDS() == CFE_ES_CDS_INVALID, "CFE_ES_ValidateCDS",
              "Reading from CDS failed begin check");

    /* Test CDS initialization where first write call to the CDS fails */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_InitCDSSignatures() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_InitCDSSignatures",
              "CDS write (first call) failed");

    /* Test CDS initialization where second write call to the CDS fails */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_InitCDSSignatures() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_InitCDSSignatures",
              "CDS write (second call) failed");

    /* Test CDS clear where write call to the CDS fails */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_ClearCDS() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_ClearCDS", "CDS write failed");

    /* Test rebuilding the CDS where the registry is not the same size */
    ES_ResetUnitTest();
    UT_GetDataBuffer(UT_KEY(CFE_PSP_ReadFromCDS), (void **)&CdsPtr, &CdsSize, NULL);
    TempSize = CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES + 1;
    memcpy(CdsPtr + CDS_REG_SIZE_OFFSET, &TempSize, sizeof(TempSize));
    UT_Report(__FILE__, __LINE__, CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID, "CFE_ES_RebuildCDS",
              "Registry too large to recover");

    /* Test clearing CDS where size is an odd number (requires partial write) */
    ES_ResetUnitTest();
    CFE_ES_Global.CDSVars.TotalSize = 53;
    UT_Report(__FILE__, __LINE__, CFE_ES_ClearCDS() == CFE_SUCCESS, "CFE_ES_ClearCDS", "CDS write failed");

    /*
     * To prepare for the rebuild tests, set up a clean area in PSP mem,
     * and make a registry entry.
     */
    ES_UT_SetupCDSGlobal(ES_UT_CDS_SMALL_TEST_SIZE);
    ES_UT_SetupSingleCDSRegistry("UT", 8, false, &UtCDSRegRecPtr);
    UtAssert_NONZERO(UtCDSRegRecPtr->BlockOffset);
    UtAssert_NONZERO(UtCDSRegRecPtr->BlockSize);
    UtAssert_INT32_EQ(CFE_ES_UpdateCDSRegistry(), CFE_SUCCESS);

    /* Test successfully rebuilding the CDS */
    ES_ResetUnitTest();

    /* The reset would have cleared the registry data */
    UtAssert_ZERO(UtCDSRegRecPtr->BlockOffset);
    UtAssert_ZERO(UtCDSRegRecPtr->BlockSize);

    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == CFE_SUCCESS, "CFE_ES_RebuildCDS", "CDS rebuild successful");

    /* Check that the registry entry exists again (was recovered) */
    UtAssert_NONZERO(UtCDSRegRecPtr->BlockOffset);
    UtAssert_NONZERO(UtCDSRegRecPtr->BlockSize);

    /* Test rebuilding the CDS with the registry unreadable */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_RebuildCDS() == CFE_ES_CDS_INVALID, "CFE_ES_RebuildCDS",
              "CDS registry unreadable");

    /* Test deleting the CDS from the registry with a registry write failure */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("NO_APP.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, true, NULL);
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteCDS("NO_APP.CDS_NAME", true) == -1, "CFE_ES_DeleteCDS",
              "CDS block descriptor write failed");
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteCDS("NO_APP.CDS_NAME", true) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_DeleteCDS", "CDS registry write failed");

    /* Test deleting the CDS from the registry with the owner application
     * still active
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CFE_ES.CDS_NAME", ES_UT_CDS_BLOCK_SIZE, true, NULL);
    ES_UT_SetupSingleAppId(CFE_ES_AppType_CORE, CFE_ES_AppState_RUNNING, "CFE_ES", NULL, NULL);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteCDS("CFE_ES.CDS_NAME", true) == CFE_ES_CDS_OWNER_ACTIVE_ERR,
              "CFE_ES_DeleteCDS", "Owner application still active");

    /*
     * To prepare for the rebuild tests, set up a clean area in PSP mem
     */
    ES_UT_SetupCDSGlobal(ES_UT_CDS_LARGE_TEST_SIZE);

    /* Test CDS initialization where rebuilding the CDS is successful */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == CFE_SUCCESS, "CFE_ES_CDS_EarlyInit",
              "Initialization with successful rebuild");

    /* Test CDS initialization where rebuilding the CDS is unsuccessful */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 3, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDS_EarlyInit() == CFE_SUCCESS, "CFE_ES_CDS_EarlyInit",
              "Initialization with unsuccessful rebuild");

    /* Test CDS initialization where initializing the CDS registry fails */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_InitCDSRegistry() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_InitCDSRegistry",
              "CDS registry write size failed");

    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_InitCDSRegistry() == CFE_ES_CDS_ACCESS_ERROR, "CFE_ES_InitCDSRegistry",
              "CDS registry write content failed");

    /* Test deleting the CDS from the registry with a CDS name longer than the
     * maximum allowed
     */
    ES_ResetUnitTest();
    memset(CDSName, 'a', sizeof(CDSName) - 1);
    CDSName[sizeof(CDSName) - 1] = '\0';
    ES_UT_SetupSingleCDSRegistry(CDSName, ES_UT_CDS_BLOCK_SIZE, true, NULL);
    UT_Report(__FILE__, __LINE__, CFE_ES_DeleteCDS(CDSName, true) == CFE_ES_ERR_NAME_NOT_FOUND, "CFE_ES_DeleteCDS",
              "CDS name too long");

    /*
     * Test Name-ID query/conversion API
     */
    ES_ResetUnitTest();
    ES_UT_SetupSingleCDSRegistry("CDS1", ES_UT_CDS_BLOCK_SIZE, false, &UtCDSRegRecPtr);
    CDSHandle = CFE_ES_CDSBlockRecordGetID(UtCDSRegRecPtr);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(CDSName, CDSHandle, sizeof(CDSName)), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(&CDSHandle, "Nonexistent"), CFE_ES_ERR_NAME_NOT_FOUND);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(&CDSHandle, CDSName), CFE_SUCCESS);
    UtAssert_True(CFE_RESOURCEID_TEST_EQUAL(CDSHandle, CFE_ES_CDSBlockRecordGetID(UtCDSRegRecPtr)),
                  "CDS Handle IDs Match");
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(CDSName, CFE_ES_CDS_BAD_HANDLE, sizeof(CDSName)),
                      CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockName(NULL, CDSHandle, sizeof(CDSName)), CFE_ES_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_ES_GetCDSBlockIDByName(&CDSHandle, NULL), CFE_ES_BAD_ARGUMENT);

} /* End TestCDS */

void TestCDSMempool(void)
{
    CFE_ES_CDS_RegRec_t *UtCdsRegRecPtr;
    int                  Data;
    CFE_ES_CDSHandle_t   BlockHandle;
    size_t               SavedSize;
    size_t               SavedOffset;
    uint8 *              CdsPtr;

    UtPrintf("Begin Test CDS memory pool");

    ES_UT_SetupCDSGlobal(0);

    /* Test creating the CDS pool with the pool size too small */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_CreateCDSPool(2, 1) == CFE_ES_CDS_INVALID_SIZE, "CFE_ES_CreateCDSPool",
              "CDS pool size too small");

    /* Test rebuilding the CDS pool with the pool size too small */
    UT_Report(__FILE__, __LINE__, CFE_ES_RebuildCDSPool(2, 1) == CFE_ES_CDS_INVALID_SIZE, "CFE_ES_RebuildCDSPool",
              "CDS pool size too small");

    /* Test rebuilding CDS pool with CDS access errors */
    /*
     * To setup - Create a CDS registry and delete it, which creates
     * a freed block in the pool.  Then attempt to rebuild.
     */
    ES_ResetUnitTest();
    ES_UT_SetupCDSGlobal(ES_UT_CDS_SMALL_TEST_SIZE);
    SavedSize   = CFE_ES_Global.CDSVars.TotalSize;
    SavedOffset = CFE_ES_Global.CDSVars.Pool.TailPosition;
    ES_UT_SetupSingleCDSRegistry("UT", sizeof(Data) + sizeof(CFE_ES_CDS_BlockHeader_t), false, &UtCdsRegRecPtr);
    UtAssert_NONZERO(UtCdsRegRecPtr->BlockOffset);
    UtAssert_NONZERO(UtCdsRegRecPtr->BlockSize);
    CFE_ES_DeleteCDS("UT", false);
    UtAssert_INT32_EQ(CFE_ES_UpdateCDSRegistry(), CFE_SUCCESS);

    /* Clear/reset the global state */
    ES_ResetUnitTest();

    /* Test rebuilding the CDS pool with a descriptor retrieve error */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_RebuildCDSPool(SavedSize, SavedOffset) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool", "CDS descriptor retrieve error");

    /* Test rebuilding the CDS pool with a descriptor commit error */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_RebuildCDSPool(SavedSize, SavedOffset) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_RebuildCDSPool", "CDS descriptor commit error");

    /* Test CDS block write using an invalid memory handle */
    ES_ResetUnitTest();
    BlockHandle = CFE_ES_CDSHANDLE_C(CFE_ResourceId_FromInteger(7));
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockWrite(BlockHandle, &Data) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_CDSBlockWrite", "Invalid memory handle");
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_CDSBlockRead", "Invalid memory handle");

    /* Test CDS block access */
    ES_ResetUnitTest();
    ES_UT_SetupCDSGlobal(ES_UT_CDS_SMALL_TEST_SIZE);
    ES_UT_SetupSingleCDSRegistry("UT", sizeof(Data) + sizeof(CFE_ES_CDS_BlockHeader_t), false, &UtCdsRegRecPtr);
    BlockHandle = CFE_ES_CDSBlockRecordGetID(UtCdsRegRecPtr);
    Data        = 42;

    /* Basic success path */
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockWrite(BlockHandle, &Data) == CFE_SUCCESS, "CFE_ES_CDSBlockWrite",
              "Nominal");
    Data = 0;
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_SUCCESS, "CFE_ES_CDSBlockRead",
              "Nominal");

    UtAssert_INT32_EQ(Data, 42);

    /* Corrupt/change the block offset, should fail validation */
    --UtCdsRegRecPtr->BlockOffset;
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockWrite(BlockHandle, &Data) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_CDSBlockWrite", "Block offset error");
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_CDSBlockRead", "Block offset error");

    ++UtCdsRegRecPtr->BlockOffset;

    /* Corrupt/change the block size, should trigger invalid size error */
    --UtCdsRegRecPtr->BlockSize;
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockWrite(BlockHandle, &Data) == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_CDSBlockWrite", "Block size error");
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_CDS_INVALID_SIZE,
              "CFE_ES_CDSBlockRead", "Block size error");
    ++UtCdsRegRecPtr->BlockSize;

    /* Test CDS block read/write with a CDS read error (block descriptor) */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockWrite(BlockHandle, &Data) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_CDSBlockWrite", "Read error on descriptor");
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_CDSBlockRead", "Read error on descriptor");

    /* Test CDS block write with a CDS write error (block header) */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockWrite(BlockHandle, &Data) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_CDSBlockWrite", "Write error on header");

    /* Test CDS block read with a CDS read error (block header) */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_CDS_ACCESS_ERROR,
              "CFE_ES_CDSBlockRead", "Read error on header");

    /* Test CDS block write with a CDS write error (data content) */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_WriteToCDS), 2, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockWrite(BlockHandle, &Data) == OS_ERROR, "CFE_ES_CDSBlockWrite",
              "Write error on content");

    /* Test CDS block read with a CDS read error (data content) */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_ReadFromCDS), 3, OS_ERROR);
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == OS_ERROR, "CFE_ES_CDSBlockRead",
              "Read error on content");

    /* Corrupt the data as to cause a CRC mismatch */
    UT_GetDataBuffer(UT_KEY(CFE_PSP_ReadFromCDS), (void **)&CdsPtr, NULL, NULL);
    CdsPtr[UtCdsRegRecPtr->BlockOffset] ^= 0x02; /* Bit flip */
    UT_Report(__FILE__, __LINE__, CFE_ES_CDSBlockRead(&Data, BlockHandle) == CFE_ES_CDS_BLOCK_CRC_ERR,
              "CFE_ES_CDSBlockRead", "CRC error on content");
    CdsPtr[UtCdsRegRecPtr->BlockOffset] ^= 0x02; /* Fix Bit */
}

void TestESMempool(void)
{
    CFE_ES_MemHandle_t      PoolID1; /* Poo1 1 handle, no mutex */
    CFE_ES_MemHandle_t      PoolID2; /* Poo1 2 handle, with mutex */
    uint8                   Buffer1[1024];
    uint8                   Buffer2[1024];
    CFE_ES_MemPoolBuf_t     addressp1 = CFE_ES_MEMPOOLBUF_C(0); /* Pool 1 buffer address */
    CFE_ES_MemPoolBuf_t     addressp2 = CFE_ES_MEMPOOLBUF_C(0); /* Pool 2 buffer address */
    CFE_ES_MemPoolRecord_t *PoolPtr;
    CFE_ES_MemPoolStats_t   Stats;
    size_t                  BlockSizes[CFE_PLATFORM_ES_POOL_MAX_BUCKETS + 2];
    CFE_ES_GenPoolBD_t *    BdPtr;
    uint32                  i;

    UtPrintf("Begin Test ES memory pool");

    memset(BlockSizes, 0, sizeof(BlockSizes));

    /* Test creating memory pool without using a mutex with the pool size
     * too small
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreateNoSem(&PoolID1, Buffer1, 0) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateNoSem", "Pool size too small");

    /* Test successfully creating memory pool without using a mutex */
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreateNoSem(&PoolID1, Buffer1, sizeof(Buffer1)) == CFE_SUCCESS,
              "CFE_ES_PoolCreateNoSem", "Memory pool create; successful");

    /* Test creating memory pool using a mutex with the pool size too small */
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreate(&PoolID2, Buffer2, 0) == CFE_ES_BAD_ARGUMENT, "CFE_ES_PoolCreate",
              "Pool size too small");

    /* Test successfully creating memory pool using a mutex */
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreate(&PoolID2, Buffer2, sizeof(Buffer2)) == CFE_SUCCESS,
              "CFE_ES_PoolCreate", "Create memory pool (using mutex) [1]; successful");

    /* Test successfully allocating a pool buffer */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp1, PoolID1, 256) > 0, "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [1]; successful");

    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp2, PoolID2, 256) > 0, "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [2]; successful");

    /* Test successfully getting the size of an existing pool buffer */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBufInfo(PoolID2, addressp2) > 0, "CFE_ES_GetPoolBufInfo",
              "Get pool buffer size; successful");

    /* Test successfully getting the size of an existing pool buffer.  Use no
     * mutex in order to get branch path coverage
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBufInfo(PoolID1, addressp1) > 0, "CFE_ES_GetPoolBufInfo",
              "Get pool buffer size; successful (no mutex)");

    /* Test successfully returning a pool buffer to the memory pool */
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID1, addressp1) > 0, "CFE_ES_PutPoolBuf",
              "Return buffer to the memory pool; successful");

    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID2, addressp2) > 0, "CFE_ES_PutPoolBuf",
              "Return buffer to the memory pool; successful");

    /* Test successfully allocating an additional pool buffer */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp2, PoolID2, 256) > 0, "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [2]; successful");

    /* Test successfully returning a pool buffer to the second memory pool.
     * Use no mutex in order to get branch path coverage
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID2, addressp2) > 0, "CFE_ES_PutPoolBuf",
              "Return buffer to the second memory pool; successful");

    /* Test handle validation using a handle with an invalid memory address */
    UT_SetDeferredRetcode(UT_KEY(CFE_PSP_MemValidateRange), 1, -1);
    UT_Report(__FILE__, __LINE__, CFE_ES_ValidateHandle(PoolID2) == false, "CFE_ES_ValidateHandle",
              "Invalid handle; bad memory address");

    /* Test handle validation using a handle where the first pool structure
     * field is not the pool start address
     */
    PoolPtr = CFE_ES_LocateMemPoolRecordByID(PoolID2);

    /*
     * Intentionally corrupt the Pool ID value - whether strict or simple
     * types are in use, underneath the wrapper(s) lies a uint32 eventually.
     * This is intentionally a type-UNSAFE access to this value.
     */
    *((uint32 *)&PoolPtr->PoolID) ^= 10; /* cause it to fail validation */

    UT_Report(__FILE__, __LINE__, CFE_ES_ValidateHandle(PoolID2) == false, "CFE_ES_ValidateHandle",
              "Invalid handle; not pool start address");

    /* Test allocating a pool buffer where the memory handle is not the pool
     * start address
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp2, PoolID2, 256) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetPoolBuf", "Invalid handle; not pool start address");

    /* Test getting memory pool statistics where the memory handle is not
     * the pool start address
     */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetMemPoolStats(&Stats, CFE_ES_MEMHANDLE_UNDEFINED) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetMemPoolStats", "Invalid handle; not pool start address");

    /* Test allocating a pool buffer where the memory block doesn't fit within
     * the remaining memory
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp1, PoolID1, 75000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf", "Requested pool size too large");

    /* Test getting the size of an existing pool buffer using an
     * invalid handle
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBufInfo(PoolID2, addressp2) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetPoolBufInfo", "Invalid memory pool handle");

    /* Undo the previous memory corruption */
    *((uint32 *)&PoolPtr->PoolID) ^= 10; /* Repair Pool2 ID */

    /* Test returning a pool buffer using an invalid memory block */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(PoolID2, CFE_ES_MEMPOOLBUF_C((cpuaddr)addressp2 - 40)) == CFE_ES_BUFFER_NOT_IN_POOL,
              "CFE_ES_PutPoolBuf", "Invalid memory block");

    /* Test initializing a pre-allocated pool specifying a number of block
     * sizes greater than the maximum
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, Buffer1, sizeof(Buffer1), CFE_PLATFORM_ES_POOL_MAX_BUCKETS + 2, BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx", "Number of block sizes exceeds maximum");

    /* Test initializing a pre-allocated pool specifying a pool size that
     * is too small and using the default block size
     */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, Buffer1, sizeof(CFE_ES_GenPoolBD_t) / 2,
                                  CFE_PLATFORM_ES_POOL_MAX_BUCKETS - 2, BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx", "Memory pool size too small (default block size)");

    /* Test calling CFE_ES_PoolCreateEx() with NULL pointer arguments
     */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(NULL, Buffer1, sizeof(Buffer1), CFE_PLATFORM_ES_POOL_MAX_BUCKETS, BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx", "Memory pool bad arguments (NULL handle pointer)");

    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, NULL, sizeof(Buffer1), CFE_PLATFORM_ES_POOL_MAX_BUCKETS, BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_BAD_ARGUMENT,
              "CFE_ES_PoolCreateEx", "Memory pool bad arguments (NULL mem pointer)");

    /*
     * Test to use default block sizes if none are given
     */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, Buffer1, sizeof(Buffer1), 0, NULL, CFE_ES_USE_MUTEX) == CFE_SUCCESS,
              "CFE_ES_PoolCreateEx", "Use default block sizes when none are given");

    /*
     * Test creating a memory pool after the limit reached (no slots)
     */
    ES_ResetUnitTest();
    UT_SetDefaultReturnValue(UT_KEY(CFE_ResourceId_FindNext), OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, Buffer1, sizeof(Buffer1), CFE_PLATFORM_ES_POOL_MAX_BUCKETS, BlockSizes,
                                  CFE_ES_USE_MUTEX) == CFE_ES_NO_RESOURCE_IDS_AVAILABLE,
              "CFE_ES_PoolCreateEx", "Memory pool limit reached");

    /* Check operation of the CFE_ES_CheckCounterIdSlotUsed() helper function */
    CFE_ES_Global.MemPoolTable[1].PoolID = CFE_ES_MEMHANDLE_C(ES_UT_MakePoolIdForIndex(1));
    CFE_ES_Global.MemPoolTable[2].PoolID = CFE_ES_MEMHANDLE_UNDEFINED;
    UtAssert_True(CFE_ES_CheckMemPoolSlotUsed(ES_UT_MakePoolIdForIndex(1)), "MemPool Slot Used");
    UtAssert_True(!CFE_ES_CheckMemPoolSlotUsed(ES_UT_MakePoolIdForIndex(2)), "MemPool Slot Unused");

    /*
     * Test creating a memory pool with a semaphore error
     */
    ES_ResetUnitTest();
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemCreate), 1, OS_ERROR);
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreate(&PoolID1, Buffer1, sizeof(Buffer1)) == CFE_STATUS_EXTERNAL_RESOURCE_FAIL,
              "CFE_ES_PoolCreateEx", "Memory pool mutex create error");

    /*
     * Test creating a memory pool with a semaphore error
     * This still returns success as there is no recourse, but there
     * should be a syslog about it.
     */
    ES_UT_SetupMemPoolId(&PoolPtr);
    OS_MutSemCreate(&PoolPtr->MutexId, "UT", 0);
    UT_SetDeferredRetcode(UT_KEY(OS_MutSemDelete), 1, OS_ERROR);
    PoolID1 = CFE_ES_MemPoolRecordGetID(PoolPtr);
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolDelete(PoolID1) == CFE_SUCCESS, "CFE_ES_PoolDelete",
              "Memory pool delete with semaphore delete error");

    /* Test initializing a pre-allocated pool specifying
     * the block size with one block size set to zero
     */
    ES_ResetUnitTest();
    BlockSizes[0] = 10;
    BlockSizes[1] = 50;
    BlockSizes[2] = 100;
    BlockSizes[3] = 0;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, Buffer1, sizeof(Buffer1), 4, BlockSizes, CFE_ES_USE_MUTEX) ==
                  CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_PoolCreateEx", "Memory pool block size zero (block size specified)");

    BlockSizes[0] = 10;
    BlockSizes[1] = 50;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, Buffer1, sizeof(Buffer1), 2, BlockSizes, CFE_ES_USE_MUTEX) == CFE_SUCCESS,
              "CFE_ES_PoolCreateEx", "Make space for new size");

    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreate(&PoolID1, Buffer1, sizeof(Buffer1)) == CFE_SUCCESS,
              "CFE_ES_PoolCreate", "Create memory pool (using mutex) [2]; successful");
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreate(&PoolID2, Buffer2, sizeof(Buffer2)) == CFE_SUCCESS,
              "CFE_ES_PoolCreate", "Create memory pool (no mutex) [2]; successful");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp1, PoolID1, 256) > 0, "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [3]; successful");

    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp2, PoolID2, 256) > 0, "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [3]; successful");

    /* Test getting the size of an existing pool buffer using an
     * unallocated block
     */
    BdPtr = ((CFE_ES_GenPoolBD_t *)addressp1) - 1;
    BdPtr->Allocated ^= 717;
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBufInfo(PoolID1, addressp1) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_GetPoolBufInfo", "Invalid memory pool handle; unallocated block");

    /* Test returning a pool buffer using an unallocated block */
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID1, addressp1) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_PutPoolBuf", "Deallocate an unallocated block");

    BdPtr->Allocated ^= 717; /* repair */

    /* Test getting the size of an existing pool buffer using an
     * invalid check bit pattern
     */
    BdPtr->Allocated = 0xaaaa;
    BdPtr->CheckBits ^= 717;
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBufInfo(PoolID1, addressp1) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_GetPoolBufInfo", "Invalid memory pool handle; check bit pattern");

    BdPtr->CheckBits ^= 717; /* repair */

    /* Test returning a pool buffer using an invalid or corrupted
     * memory descriptor
     */
    BdPtr->ActualSize = 0xFFFFFFFF;
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID1, addressp1) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_PutPoolBuf", "Invalid/corrupted memory descriptor");

    /* Test getting the size of an existing pool buffer using an
     * unallocated block.  Use no mutex in order to get branch path coverage
     */
    BdPtr = ((CFE_ES_GenPoolBD_t *)addressp2) - 1;
    BdPtr->Allocated ^= 717;
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBufInfo(PoolID2, addressp2) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_GetPoolBufInfo", "Invalid memory pool handle; unallocated block (no mutex)");

    /* Test returning a pool buffer using an unallocated block.  Use no mutex
     * in order to get branch path coverage
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID2, addressp2) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_PutPoolBuf", "Deallocate an unallocated block (no mutex)");

    BdPtr->Allocated ^= 717; /* repair */

    /* Test getting the size of an existing pool buffer using an
     * invalid check bit pattern.  Use no mutex in order to get branch path
     * coverage
     */
    BdPtr->CheckBits ^= 717;
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBufInfo(PoolID2, addressp2) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_GetPoolBufInfo", "Invalid memory pool handle; check bit pattern (no mutex)");

    BdPtr->CheckBits ^= 717; /* repair */

    /* Test returning a pool buffer using an invalid or corrupted
     * memory descriptor.  Use no mutex in order to get branch path coverage
     */
    BdPtr->ActualSize = 0xFFFFFFFF;
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID2, addressp2) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_PutPoolBuf", "Invalid/corrupted memory descriptor (no mutex)");

    /* Test successfully creating memory pool using a mutex for
     * subsequent tests
     */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreate(&PoolID1, Buffer1, sizeof(Buffer1)) == CFE_SUCCESS,
              "CFE_ES_PoolCreate", "Create memory pool (using mutex) [3]; successful");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests.  Use no mutex in order to get branch path coverage
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_PoolCreate(&PoolID2, Buffer2, sizeof(Buffer2)) == CFE_SUCCESS,
              "CFE_ES_PoolCreate", "Create memory pool (using mutex) [3]; successful");

    /* Test successfully allocating an additional pool buffer for
     * subsequent tests
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp1, PoolID1, 256) > 0, "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [3]; successful");

    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp2, PoolID2, 256) > 0, "CFE_ES_GetPoolBuf",
              "Allocate pool buffer [3]; successful");

    /* Test returning a pool buffer using a buffer size larger than
     * the maximum
     */
    BdPtr             = ((CFE_ES_GenPoolBD_t *)addressp1) - 1;
    BdPtr->ActualSize = CFE_PLATFORM_ES_MAX_BLOCK_SIZE + 1;
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID1, addressp1) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_PutPoolBuf", "Pool buffer size exceeds maximum");

    /* Test returning a pool buffer using a buffer size larger than
     * the maximum.  Use no mutex in order to get branch path coverage
     */
    BdPtr             = ((CFE_ES_GenPoolBD_t *)addressp2) - 1;
    BdPtr->ActualSize = CFE_PLATFORM_ES_MAX_BLOCK_SIZE + 1;
    UT_Report(__FILE__, __LINE__, CFE_ES_PutPoolBuf(PoolID2, addressp2) == CFE_ES_POOL_BLOCK_INVALID,
              "CFE_ES_PutPoolBuf", "Pool buffer size exceeds maximum (no mutex)");

    /* Test allocating an additional pool buffer using a buffer size larger
     * than the maximum
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp2, PoolID1, 99000) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf", "Pool buffer size exceeds maximum");

    /* Test handle validation using a null handle */
    UT_Report(__FILE__, __LINE__, CFE_ES_ValidateHandle(CFE_ES_MEMHANDLE_UNDEFINED) == false, "CFE_ES_ValidateHandle",
              "NULL handle");

    /* Test returning a pool buffer using a null handle */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(CFE_ES_MEMHANDLE_UNDEFINED, addressp2) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_PutPoolBuf", "NULL memory handle");

    /* Test allocating a pool buffer using a null handle */
    ES_ResetUnitTest();
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBuf(&addressp2, CFE_ES_MEMHANDLE_UNDEFINED, 256) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetPoolBuf", "NULL memory handle");

    /* Test getting the size of an existing pool buffer using a null handle */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(CFE_ES_MEMHANDLE_UNDEFINED, addressp1) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_GetPoolBufInfo", "NULL memory handle");

    /* Test initializing a pre-allocated pool specifying a small block size */
    ES_ResetUnitTest();
    BlockSizes[0] = 16;
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PoolCreateEx(&PoolID1, Buffer1, 128, 1, BlockSizes, CFE_ES_USE_MUTEX) == CFE_SUCCESS,
              "CFE_ES_PoolCreateEx", "Allocate small memory pool");

    /* Test allocating an additional pool buffer using a buffer size larger
     * than the maximum.
     */
    UT_Report(__FILE__, __LINE__, CFE_ES_GetPoolBuf(&addressp1, PoolID1, 32) == CFE_ES_ERR_MEM_BLOCK_SIZE,
              "CFE_ES_GetPoolBuf", "Pool buffer size exceeds maximum (no mutex)");

    /*
     * Test allocating a pool buffer where the memory block doesn't fit within
     * the remaining memory.  Use no mutex in order to get branch path coverage
     *
     * NOTE: Theoretically with a 128 byte pool this should fail after ~4 allocations.
     * (16 byte block plus 12 byte BD = 28 bytes each)
     *
     * However due to alignment requirements of the local CPU padding might be added
     * and the sizeof(BD_t) might be bigger too, resulting in fewer allocations.
     *
     * There should always be at least 1 successful allocation, but the number of
     * successful ones is dependent on the CPU architecture and the setting of
     * CFE_PLATFORM_ES_MEMPOOL_ALIGN_SIZE_MIN.  Expect a failure within 20 allocations.
     */
    for (i = 0; i < 25; ++i)
    {
        if (CFE_ES_GetPoolBuf(&addressp1, PoolID1, 12) == CFE_ES_ERR_MEM_BLOCK_SIZE)
        {
            break;
        }
    }

    UT_Report(__FILE__, __LINE__, i >= 1 && i <= 20, "CFE_ES_GetPoolBuf", "Pool fully allocated");

    /* Test getting the size of a pool buffer that is not in the pool */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_GetPoolBufInfo(PoolID1, CFE_ES_MEMPOOLBUF_C((cpuaddr)addressp1 + 400)) ==
                  CFE_ES_BUFFER_NOT_IN_POOL,
              "CFE_ES_GetPoolBufInfo", "Invalid pool buffer");

    /* Test getting the size of a pool buffer with an invalid memory handle */
    UT_Report(__FILE__, __LINE__,
              CFE_ES_PutPoolBuf(CFE_ES_MEMHANDLE_UNDEFINED, addressp1) == CFE_ES_ERR_RESOURCEID_NOT_VALID,
              "CFE_ES_PutPoolBuf", "Invalid memory handle");
}

/* Tests to fill gaps in coverage in SysLog */
void TestSysLog(void)
{
    CFE_ES_SysLogReadBuffer_t SysLogBuffer;
    char                      LogString[(CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 2];

    char TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE + 1];

    UtPrintf("Begin Test Sys Log");

    /* Test loop in CFE_ES_SysLogReadStart_Unsync that ensures
     * reading at the start of a message */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = sizeof(CFE_ES_Global.ResetDataPtr->SystemLog) - 1;

    memset(CFE_ES_Global.ResetDataPtr->SystemLog, 'a', CFE_ES_Global.ResetDataPtr->SystemLogEndIdx);
    CFE_ES_Global.ResetDataPtr->SystemLog[CFE_ES_Global.ResetDataPtr->SystemLogEndIdx - 1] = '\n';

    CFE_ES_SysLogReadStart_Unsync(&SysLogBuffer);

    UT_Report(__FILE__, __LINE__,
              SysLogBuffer.EndIdx == sizeof(CFE_ES_Global.ResetDataPtr->SystemLog) - 1 &&
                  SysLogBuffer.LastOffset == sizeof(CFE_ES_Global.ResetDataPtr->SystemLog) - 1 &&
                  SysLogBuffer.BlockSize == 0 && SysLogBuffer.SizeLeft == 0,
              "CFE_ES_SysLogReadStart_Unsync(SysLogBuffer)", "ResetDataPtr pointing to an old fragment of a message");

    /* Test truncation of a sys log message that is over half
     * the size of the total log */
    ES_ResetUnitTest();
    memset(LogString, 'a', (CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 1);
    LogString[(CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 1] = '\0';
    UT_Report(__FILE__, __LINE__, CFE_ES_SysLogAppend_Unsync(LogString) == CFE_ES_ERR_SYS_LOG_TRUNCATED,
              "CFE_ES_SysLogAppend_Unsync", "Truncated sys log message");

    /* Test code that skips writing an empty string to the sys log */
    ES_ResetUnitTest();
    memset(LogString, 'a', (CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2) + 1);
    LogString[0] = '\0';
    UT_Report(__FILE__, __LINE__, CFE_ES_SysLogAppend_Unsync(LogString) == CFE_SUCCESS, "CFE_ES_SysLogAppend_Unsync",
              "Don't log an empty string");

    /* Test Reading space between the current read offset and end of the log buffer */
    ES_ResetUnitTest();
    SysLogBuffer.EndIdx     = 3;
    SysLogBuffer.LastOffset = 0;
    SysLogBuffer.BlockSize  = 3;
    SysLogBuffer.SizeLeft   = 1;

    CFE_ES_SysLogReadData(&SysLogBuffer);

    UT_Report(__FILE__, __LINE__,
              SysLogBuffer.EndIdx == 3 && SysLogBuffer.LastOffset == 1 && SysLogBuffer.BlockSize == 1 &&
                  SysLogBuffer.SizeLeft == 0,
              "CFE_ES_SysLogReadData", "Read space between current offset and end of log buffer");

    /* Test nominal flow through CFE_ES_SysLogDump
     * with multiple reads and writes  */
    ES_ResetUnitTest();
    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = sizeof(CFE_ES_Global.ResetDataPtr->SystemLog) - 1;

    CFE_ES_SysLogDump("fakefilename");

    UT_Report(__FILE__, __LINE__, true, "CFE_ES_SysLogDump", "Multiple reads and writes to sys log");

    /* Test "message got truncated"  */
    ES_ResetUnitTest();
    memset(TmpString, 'a', CFE_ES_MAX_SYSLOG_MSG_SIZE);
    TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE] = '\0';

    CFE_ES_WriteToSysLog("%s", TmpString);
    UT_Report(__FILE__, __LINE__, true, "CFE_ES_WriteToSysLog", "Truncate message");
}

void TestBackground(void)
{
    int32 status;

    /* CFE_ES_BackgroundInit() with default setup
     * causes  CFE_ES_CreateChildTask to fail.
     */
    ES_ResetUnitTest();
    status = CFE_ES_BackgroundInit();
    UtAssert_True(status == CFE_ES_ERR_RESOURCEID_NOT_VALID,
                  "CFE_ES_BackgroundInit - CFE_ES_CreateChildTask failure (%08x)", (unsigned int)status);

    /* The CFE_ES_BackgroundCleanup() function has no conditionals -
     * it just needs to be executed as part of this routine,
     * and confirm that it deleted the semaphore.
     */
    ES_ResetUnitTest();
    OS_BinSemCreate(&CFE_ES_Global.BackgroundTask.WorkSem, "UT", 0, 0);
    CFE_ES_BackgroundCleanup();
    UtAssert_True(UT_GetStubCount(UT_KEY(OS_BinSemDelete)) == 1, "CFE_ES_BackgroundCleanup - OS_BinSemDelete called");

    /*
     * When testing the background task loop, it is normally an infinite loop,
     * so this is needed to set a condition for the loop to exit.
     *
     * This also sets a state so the background perf log dump will be "Active" to
     * execute the code which counts the number of active jobs.
     */
    ES_ResetUnitTest();
    memset(&CFE_ES_Global.BackgroundPerfDumpState, 0, sizeof(CFE_ES_Global.BackgroundPerfDumpState));
    UT_SetDefaultReturnValue(UT_KEY(OS_write), -10);
    CFE_ES_Global.BackgroundPerfDumpState.CurrentState = CFE_ES_PerfDumpState_INIT;
    UT_SetDeferredRetcode(UT_KEY(OS_BinSemTimedWait), 3, -4);
    CFE_ES_BackgroundTask();
    UT_Report(__FILE__, __LINE__, UT_PrintfIsInHistory(UT_OSP_MESSAGES[UT_OSP_BACKGROUND_TAKE]),
              "CFE_ES_BackgroundTask", "Failed to take background sem");
    /* The number of jobs running should be 1 (perf log dump) */
    UtAssert_True(CFE_ES_Global.BackgroundTask.NumJobsRunning == 1,
                  "CFE_ES_BackgroundTask - Nominal, CFE_ES_Global.BackgroundTask.NumJobsRunning (%u) == 1",
                  (unsigned int)CFE_ES_Global.BackgroundTask.NumJobsRunning);
}
