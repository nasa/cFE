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
 */

#ifndef ES_UT_HELPERS_H
#define ES_UT_HELPERS_H

/*
** Includes
*/
#include "es_UT.h"
#include <setjmp.h>

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

typedef struct
{
    uint32 AppType;
    uint32 AppState;
} ES_UT_SetAppStateHook_t;

/*
 * Used for calls to setjmp and longjmp for testing CFE_ES_ExitApp
 */
extern jmp_buf OS_TaskDelay_jmp_buf;

extern CFE_ES_Global_t CFE_ES_Global;

/*
** Global variables
*/

/*
 * Pointer to reset data that will be re-configured/preserved across calls to ES_ResetUnitTest()
 */
extern CFE_ES_ResetData_t *ES_UT_PersistentResetData;

extern CFE_ES_GMP_DirectBuffer_t   UT_MemPoolDirectBuffer;
extern CFE_ES_GMP_IndirectBuffer_t UT_MemPoolIndirectBuffer;

/* A startup script buffer for a maximum of 5 lines * 80 chars/line */
extern char StartupScript[MAX_STARTUP_SCRIPT];

/* An instance of this struct needs to be defined for each of the CCs */
/* The definition is somewhat different depending on whether EDS is in use or not */
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_NOOP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESET_COUNTERS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESTART_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_START_APP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_STOP_APP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESTART_APP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RELOAD_APP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ONE_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ALL_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_QUERY_ALL_TASKS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_CLEAR_SYS_LOG_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_WRITE_SYS_LOG_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_OVER_WRITE_SYS_LOG_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_CLEAR_ER_LOG_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_WRITE_ER_LOG_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_START_PERF_DATA_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_STOP_PERF_DATA_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_PERF_FILTER_MASK_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_PERF_TRIGGER_MASK_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_RESET_PR_COUNT_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SET_MAX_PR_COUNT_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_DELETE_CDS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_SEND_MEM_POOL_STATS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_DUMP_CDS_REGISTRY_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_SEND_HK;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_INVALID_LENGTH;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_ES_CMD_INVALID_CC;

/*
** Functions
*/

CFE_ResourceId_t ES_UT_MakeAppIdForIndex(uint32 ArrayIdx);
CFE_ResourceId_t ES_UT_MakeTaskIdForIndex(uint32 ArrayIdx);
CFE_ResourceId_t ES_UT_MakeLibIdForIndex(uint32 ArrayIdx);
CFE_ResourceId_t ES_UT_MakeCounterIdForIndex(uint32 ArrayIdx);
CFE_ResourceId_t ES_UT_MakePoolIdForIndex(uint32 ArrayIdx);
CFE_ResourceId_t ES_UT_MakeCDSIdForIndex(uint32 ArrayIdx);

void  ES_ResetUnitTest(void);
void  ES_UT_TaskFunction(void);
void  ES_UT_SysLog_snprintf(char *Buffer, size_t BufferSize, const char *SpecStringPtr, ...);
void  ES_UT_FillBuffer(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void  ES_UT_SetupModuleLoadParams(CFE_ES_ModuleLoadParams_t *Params, const char *FileName, const char *EntryName);
void  ES_UT_SetupAppStartParams(CFE_ES_AppStartParams_t *Params, const char *FileName, const char *EntryName,
                                size_t StackSize, CFE_ES_TaskPriority_Atom_t Priority,
                                CFE_ES_ExceptionAction_Enum_t ExceptionAction);
void  ES_UT_SetupSingleAppId(CFE_ES_AppType_Enum_t AppType, CFE_ES_AppState_Enum_t AppState, const char *AppName,
                             CFE_ES_AppRecord_t **OutAppRec, CFE_ES_TaskRecord_t **OutTaskRec);
void  ES_UT_SetupChildTaskId(const CFE_ES_AppRecord_t *ParentApp, const char *TaskName,
                             CFE_ES_TaskRecord_t **OutTaskRec);
void  ES_UT_SetupSingleLibId(const char *LibName, CFE_ES_LibRecord_t **OutLibRec);
int32 ES_UT_PoolDirectRetrieve(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr);
int32 ES_UT_PoolDirectCommit(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr);
int32 ES_UT_PoolIndirectRetrieve(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr);
int32 ES_UT_PoolIndirectCommit(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr);
int32 ES_UT_CDSPoolRetrieve(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr);
int32 ES_UT_CDSPoolCommit(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr);
int32 ES_UT_PoolCommitFail(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, const CFE_ES_GenPoolBD_t *BdPtr);
int32 ES_UT_PoolRetrieveFail(CFE_ES_GenPoolRecord_t *PoolRecPtr, size_t Offset, CFE_ES_GenPoolBD_t **BdPtr);
void  ES_UT_SetupMemPoolId(CFE_ES_MemPoolRecord_t **OutPoolRecPtr);
void  ES_UT_SetupCDSGlobal(size_t CDS_Size);
void ES_UT_SetupSingleCDSRegistry(const char *CDSName, size_t BlockSize, bool IsTable, CFE_ES_CDS_RegRec_t **OutRegRec);
int32 ES_UT_SetupOSCleanupHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);
void  ES_UT_SetupForOSCleanup(void);
int32 ES_UT_SetAppStateHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context);
void  UT_ArrayConfigHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void  ES_UT_TaskDelay_Hook(void *UserObj);
int32 ES_UT_TaskCreate_Hook(void *UserObj);
int32 ES_UT_ModuleUnload_Hook(void *UserObj);

#endif
