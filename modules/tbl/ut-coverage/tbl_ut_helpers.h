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
 *  Declare helper functions and macros to work with table services unit tests
 */

#ifndef TBL_UT_HELPERS_H
#define TBL_UT_HELPERS_H

/*
** Includes
*/
#include "cfe_core_resourceid_basevalues.h"

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
#define UT_CFE_TBL_HANDLE_INVH CFE_TBL_HANDLEID_C(CFE_RESOURCEID_WRAP(INT32_MAX))

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

/* Various canned configurations for the TaskPipe dispatcher */
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_MSG_HK;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_NOOP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_LENGTH;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_INVALID_MID;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_CC;

/*
 * UT helper routines
 */

void UT_InitData_TBL(void);

CFE_TBL_AccessDescriptor_t *UT_TBL_AccDescFromExtHandle(CFE_TBL_Handle_t TblHandle);
CFE_TBL_Handle_t            UT_TBL_AccDescToExtHandle(CFE_TBL_AccessDescriptor_t *AccDescPtr);
void UT_TBL_SetupTxnFromExtHandle(CFE_TBL_TxnState_t *Txn, CFE_TBL_Handle_t TblHandle, uint32 Context);
CFE_TBL_TableConfig_t *UT_TBL_Config(CFE_TBL_RegistryRec_t *RegRecPtr);
CFE_TBL_TableStatus_t *UT_TBL_Status(CFE_TBL_RegistryRec_t *RegRecPtr);
void                   UT_TBL_ForEveryAccDesc(void (*Func)(CFE_TBL_AccessDescriptor_t *));
void                   UT_TBL_ForEveryRegRec(void (*Func)(CFE_TBL_RegistryRec_t *));
void                   UT_TBL_ForEveryLoadBuff(void (*Func)(CFE_TBL_LoadBuff_t *));
void UT_TBL_SetupSingleReg(CFE_TBL_RegistryRec_t **RegRecOut, CFE_TBL_AccessDescriptor_t **AccDescOut, uint16 Flags);
/* Sets up the indicated validation request/result buffer as VALIDATION_PENDING */
void UT_TBL_SetupPendingValidation(uint32 ArrayIndex, bool UseActive, CFE_TBL_RegistryRec_t *RegRecPtr,
                                   CFE_TBL_ValidationResult_t **ValResultOut);
/* Resets the indicated validation request/result buffer to the free/unused state */
void UT_TBL_ResetValidationState(uint32 ArrayIndex);
void UT_TBL_SetupPendingDump(uint32 ArrayIndex, CFE_TBL_LoadBuff_t *DumpBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                             CFE_TBL_DumpControl_t **DumpCtrlOut);
/* Resets the indicated dump control block to the free/unused state */
void UT_TBL_ResetDumpCtrlState(uint32 ArrayIndex);
/* Simple helper to set a string in a fixed size buffer and ensure null term (repeated logic) */
void UT_TBL_SetName(char *BufPtr, size_t BufSz, const char *Src);
void UT_ArrayConfigHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context);
void UT_TBL_SetDumpOnly(CFE_TBL_RegistryRec_t *RegRecPtr);
void UT_TBL_ClearLoadPending(CFE_TBL_RegistryRec_t *RegRecPtr);
void UT_TBL_SetLoadBuffTaken(CFE_TBL_LoadBuff_t *LoadBuffPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                             CFE_ResourceId_t PendingId);
void UT_TBL_SetLoadBuffFree(CFE_TBL_LoadBuff_t *LoadBuffPtr);
void UT_TBL_SetHandleUsed(CFE_TBL_AccessDescriptor_t *AccDescPtr);
void UT_TBL_GlobalDataReset(void);
/* Sets up a "load in progress" buffer */
CFE_TBL_LoadBuff_t *UT_TBL_SetupLoadBuff(CFE_TBL_RegistryRec_t *RegRecPtr, bool UseLocalBuffer, uint32 BuffNum);
CFE_TBL_LoadBuff_t *UT_TBL_InitActiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr, uint32 BuffNum);
void                UT_TBL_SetActiveBufferAddr(CFE_TBL_RegistryRec_t *RegRecPtr, uint32 BuffNum, void *Addr);
void UT_TBL_CheckTxnHasEventImpl(const CFE_TBL_TxnState_t *Txn, uint16 EventId, const char *File, uint32 Line,
                                 const char *EventName);

void UT_TBL_SetupHeader(CFE_TBL_File_Hdr_t *TblFileHeader, size_t Offset, size_t NumBytes, const char *Name);
void UT_TBL_SetupCodec(size_t ByteSize);
void UT_TBL_RegisterCodecTests(void);

void UT_TBL_SetupRegRec(CFE_TBL_RegistryRec_t *RegRecPtr);
void UT_TBL_SetupAccDesc(CFE_TBL_AccessDescriptor_t *AccDescPtr);
void UT_InitializeTableRegistryNames(void);
void UT_ResetTableRegistry(void);

#endif /* TBL_UT_HELPERS_H */
