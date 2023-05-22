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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_es header
 */

#include <stdarg.h>

#include "cfe_es.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_ES_AppID_ToIndex(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_CopyToCDS(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_CreateChildTask(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_ExitApp(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_GetAppID(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_GetAppIDByName(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_GetAppName(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_GetPoolBuf(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_GetResetType(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_GetTaskID(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_GetTaskInfo(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_PoolCreate(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_PoolCreateNoSem(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_RestoreFromCDS(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_RunLoop(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_TaskID_ToIndex(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ES_WriteToSysLog(void *, UT_EntryKey_t, const UT_StubContext_t *, va_list);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_AppID_ToIndex()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_AppID_ToIndex(CFE_ES_AppId_t AppID, uint32 *Idx)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_AppID_ToIndex, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_AppID_ToIndex, CFE_ES_AppId_t, AppID);
    UT_GenStub_AddParam(CFE_ES_AppID_ToIndex, uint32 *, Idx);

    UT_GenStub_Execute(CFE_ES_AppID_ToIndex, Basic, UT_DefaultHandler_CFE_ES_AppID_ToIndex);

    return UT_GenStub_GetReturnValue(CFE_ES_AppID_ToIndex, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_BackgroundWakeup()
 * ----------------------------------------------------
 */
void CFE_ES_BackgroundWakeup(void)
{

    UT_GenStub_Execute(CFE_ES_BackgroundWakeup, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_CalculateCRC()
 * ----------------------------------------------------
 */
uint32 CFE_ES_CalculateCRC(const void *DataPtr, size_t DataLength, uint32 InputCRC, CFE_ES_CrcType_Enum_t TypeCRC)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_CalculateCRC, uint32);

    UT_GenStub_AddParam(CFE_ES_CalculateCRC, const void *, DataPtr);
    UT_GenStub_AddParam(CFE_ES_CalculateCRC, size_t, DataLength);
    UT_GenStub_AddParam(CFE_ES_CalculateCRC, uint32, InputCRC);
    UT_GenStub_AddParam(CFE_ES_CalculateCRC, CFE_ES_CrcType_Enum_t, TypeCRC);

    UT_GenStub_Execute(CFE_ES_CalculateCRC, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_CalculateCRC, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_CopyToCDS()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, const void *DataToCopy)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_CopyToCDS, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_CopyToCDS, CFE_ES_CDSHandle_t, Handle);
    UT_GenStub_AddParam(CFE_ES_CopyToCDS, const void *, DataToCopy);

    UT_GenStub_Execute(CFE_ES_CopyToCDS, Basic, UT_DefaultHandler_CFE_ES_CopyToCDS);

    return UT_GenStub_GetReturnValue(CFE_ES_CopyToCDS, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_CounterID_ToIndex()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_CounterID_ToIndex(CFE_ES_CounterId_t CounterId, uint32 *Idx)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_CounterID_ToIndex, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_CounterID_ToIndex, CFE_ES_CounterId_t, CounterId);
    UT_GenStub_AddParam(CFE_ES_CounterID_ToIndex, uint32 *, Idx);

    UT_GenStub_Execute(CFE_ES_CounterID_ToIndex, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_CounterID_ToIndex, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_CreateChildTask()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_CreateChildTask(CFE_ES_TaskId_t *TaskIdPtr, const char *TaskName,
                                    CFE_ES_ChildTaskMainFuncPtr_t FunctionPtr, CFE_ES_StackPointer_t StackPtr,
                                    size_t StackSize, CFE_ES_TaskPriority_Atom_t Priority, uint32 Flags)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_CreateChildTask, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_CreateChildTask, CFE_ES_TaskId_t *, TaskIdPtr);
    UT_GenStub_AddParam(CFE_ES_CreateChildTask, const char *, TaskName);
    UT_GenStub_AddParam(CFE_ES_CreateChildTask, CFE_ES_ChildTaskMainFuncPtr_t, FunctionPtr);
    UT_GenStub_AddParam(CFE_ES_CreateChildTask, CFE_ES_StackPointer_t, StackPtr);
    UT_GenStub_AddParam(CFE_ES_CreateChildTask, size_t, StackSize);
    UT_GenStub_AddParam(CFE_ES_CreateChildTask, CFE_ES_TaskPriority_Atom_t, Priority);
    UT_GenStub_AddParam(CFE_ES_CreateChildTask, uint32, Flags);

    UT_GenStub_Execute(CFE_ES_CreateChildTask, Basic, UT_DefaultHandler_CFE_ES_CreateChildTask);

    return UT_GenStub_GetReturnValue(CFE_ES_CreateChildTask, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_DeleteApp()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_DeleteApp(CFE_ES_AppId_t AppID)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_DeleteApp, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_DeleteApp, CFE_ES_AppId_t, AppID);

    UT_GenStub_Execute(CFE_ES_DeleteApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_DeleteApp, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_DeleteChildTask()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_DeleteChildTask(CFE_ES_TaskId_t TaskId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_DeleteChildTask, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_DeleteChildTask, CFE_ES_TaskId_t, TaskId);

    UT_GenStub_Execute(CFE_ES_DeleteChildTask, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_DeleteChildTask, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_DeleteGenCounter()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_DeleteGenCounter(CFE_ES_CounterId_t CounterId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_DeleteGenCounter, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_DeleteGenCounter, CFE_ES_CounterId_t, CounterId);

    UT_GenStub_Execute(CFE_ES_DeleteGenCounter, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_DeleteGenCounter, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_ExitApp()
 * ----------------------------------------------------
 */
void CFE_ES_ExitApp(uint32 ExitStatus)
{
    UT_GenStub_AddParam(CFE_ES_ExitApp, uint32, ExitStatus);

    UT_GenStub_Execute(CFE_ES_ExitApp, Basic, UT_DefaultHandler_CFE_ES_ExitApp);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_ExitChildTask()
 * ----------------------------------------------------
 */
void CFE_ES_ExitChildTask(void)
{

    UT_GenStub_Execute(CFE_ES_ExitChildTask, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetAppID()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetAppID(CFE_ES_AppId_t *AppIdPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetAppID, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetAppID, CFE_ES_AppId_t *, AppIdPtr);

    UT_GenStub_Execute(CFE_ES_GetAppID, Basic, UT_DefaultHandler_CFE_ES_GetAppID);

    return UT_GenStub_GetReturnValue(CFE_ES_GetAppID, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetAppIDByName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetAppIDByName(CFE_ES_AppId_t *AppIdPtr, const char *AppName)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetAppIDByName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetAppIDByName, CFE_ES_AppId_t *, AppIdPtr);
    UT_GenStub_AddParam(CFE_ES_GetAppIDByName, const char *, AppName);

    UT_GenStub_Execute(CFE_ES_GetAppIDByName, Basic, UT_DefaultHandler_CFE_ES_GetAppIDByName);

    return UT_GenStub_GetReturnValue(CFE_ES_GetAppIDByName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetAppInfo()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, CFE_ES_AppId_t AppId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetAppInfo, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetAppInfo, CFE_ES_AppInfo_t *, AppInfo);
    UT_GenStub_AddParam(CFE_ES_GetAppInfo, CFE_ES_AppId_t, AppId);

    UT_GenStub_Execute(CFE_ES_GetAppInfo, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetAppInfo, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetAppName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetAppName(char *AppName, CFE_ES_AppId_t AppId, size_t BufferLength)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetAppName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetAppName, char *, AppName);
    UT_GenStub_AddParam(CFE_ES_GetAppName, CFE_ES_AppId_t, AppId);
    UT_GenStub_AddParam(CFE_ES_GetAppName, size_t, BufferLength);

    UT_GenStub_Execute(CFE_ES_GetAppName, Basic, UT_DefaultHandler_CFE_ES_GetAppName);

    return UT_GenStub_GetReturnValue(CFE_ES_GetAppName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetCDSBlockIDByName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetCDSBlockIDByName(CFE_ES_CDSHandle_t *BlockIdPtr, const char *BlockName)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetCDSBlockIDByName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetCDSBlockIDByName, CFE_ES_CDSHandle_t *, BlockIdPtr);
    UT_GenStub_AddParam(CFE_ES_GetCDSBlockIDByName, const char *, BlockName);

    UT_GenStub_Execute(CFE_ES_GetCDSBlockIDByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetCDSBlockIDByName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetCDSBlockName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetCDSBlockName(char *BlockName, CFE_ES_CDSHandle_t BlockId, size_t BufferLength)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetCDSBlockName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetCDSBlockName, char *, BlockName);
    UT_GenStub_AddParam(CFE_ES_GetCDSBlockName, CFE_ES_CDSHandle_t, BlockId);
    UT_GenStub_AddParam(CFE_ES_GetCDSBlockName, size_t, BufferLength);

    UT_GenStub_Execute(CFE_ES_GetCDSBlockName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetCDSBlockName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetGenCount()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetGenCount(CFE_ES_CounterId_t CounterId, uint32 *Count)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetGenCount, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetGenCount, CFE_ES_CounterId_t, CounterId);
    UT_GenStub_AddParam(CFE_ES_GetGenCount, uint32 *, Count);

    UT_GenStub_Execute(CFE_ES_GetGenCount, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetGenCount, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetGenCounterIDByName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetGenCounterIDByName(CFE_ES_CounterId_t *CounterIdPtr, const char *CounterName)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetGenCounterIDByName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetGenCounterIDByName, CFE_ES_CounterId_t *, CounterIdPtr);
    UT_GenStub_AddParam(CFE_ES_GetGenCounterIDByName, const char *, CounterName);

    UT_GenStub_Execute(CFE_ES_GetGenCounterIDByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetGenCounterIDByName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetGenCounterName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetGenCounterName(char *CounterName, CFE_ES_CounterId_t CounterId, size_t BufferLength)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetGenCounterName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetGenCounterName, char *, CounterName);
    UT_GenStub_AddParam(CFE_ES_GetGenCounterName, CFE_ES_CounterId_t, CounterId);
    UT_GenStub_AddParam(CFE_ES_GetGenCounterName, size_t, BufferLength);

    UT_GenStub_Execute(CFE_ES_GetGenCounterName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetGenCounterName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetLibIDByName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetLibIDByName(CFE_ES_LibId_t *LibIdPtr, const char *LibName)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetLibIDByName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetLibIDByName, CFE_ES_LibId_t *, LibIdPtr);
    UT_GenStub_AddParam(CFE_ES_GetLibIDByName, const char *, LibName);

    UT_GenStub_Execute(CFE_ES_GetLibIDByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetLibIDByName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetLibInfo()
 * ----------------------------------------------------
 */
int32 CFE_ES_GetLibInfo(CFE_ES_AppInfo_t *LibInfo, CFE_ES_LibId_t LibId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetLibInfo, int32);

    UT_GenStub_AddParam(CFE_ES_GetLibInfo, CFE_ES_AppInfo_t *, LibInfo);
    UT_GenStub_AddParam(CFE_ES_GetLibInfo, CFE_ES_LibId_t, LibId);

    UT_GenStub_Execute(CFE_ES_GetLibInfo, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetLibInfo, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetLibName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetLibName(char *LibName, CFE_ES_LibId_t LibId, size_t BufferLength)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetLibName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetLibName, char *, LibName);
    UT_GenStub_AddParam(CFE_ES_GetLibName, CFE_ES_LibId_t, LibId);
    UT_GenStub_AddParam(CFE_ES_GetLibName, size_t, BufferLength);

    UT_GenStub_Execute(CFE_ES_GetLibName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetLibName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetMemPoolStats()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t Handle)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetMemPoolStats, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetMemPoolStats, CFE_ES_MemPoolStats_t *, BufPtr);
    UT_GenStub_AddParam(CFE_ES_GetMemPoolStats, CFE_ES_MemHandle_t, Handle);

    UT_GenStub_Execute(CFE_ES_GetMemPoolStats, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetMemPoolStats, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetModuleInfo()
 * ----------------------------------------------------
 */
int32 CFE_ES_GetModuleInfo(CFE_ES_AppInfo_t *ModuleInfo, CFE_ResourceId_t ResourceId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetModuleInfo, int32);

    UT_GenStub_AddParam(CFE_ES_GetModuleInfo, CFE_ES_AppInfo_t *, ModuleInfo);
    UT_GenStub_AddParam(CFE_ES_GetModuleInfo, CFE_ResourceId_t, ResourceId);

    UT_GenStub_Execute(CFE_ES_GetModuleInfo, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetModuleInfo, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetPoolBuf()
 * ----------------------------------------------------
 */
int32 CFE_ES_GetPoolBuf(CFE_ES_MemPoolBuf_t *BufPtr, CFE_ES_MemHandle_t Handle, size_t Size)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetPoolBuf, int32);

    UT_GenStub_AddParam(CFE_ES_GetPoolBuf, CFE_ES_MemPoolBuf_t *, BufPtr);
    UT_GenStub_AddParam(CFE_ES_GetPoolBuf, CFE_ES_MemHandle_t, Handle);
    UT_GenStub_AddParam(CFE_ES_GetPoolBuf, size_t, Size);

    UT_GenStub_Execute(CFE_ES_GetPoolBuf, Basic, UT_DefaultHandler_CFE_ES_GetPoolBuf);

    return UT_GenStub_GetReturnValue(CFE_ES_GetPoolBuf, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetPoolBufInfo()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t Handle, CFE_ES_MemPoolBuf_t BufPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetPoolBufInfo, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetPoolBufInfo, CFE_ES_MemHandle_t, Handle);
    UT_GenStub_AddParam(CFE_ES_GetPoolBufInfo, CFE_ES_MemPoolBuf_t, BufPtr);

    UT_GenStub_Execute(CFE_ES_GetPoolBufInfo, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetPoolBufInfo, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetResetType()
 * ----------------------------------------------------
 */
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetResetType, int32);

    UT_GenStub_AddParam(CFE_ES_GetResetType, uint32 *, ResetSubtypePtr);

    UT_GenStub_Execute(CFE_ES_GetResetType, Basic, UT_DefaultHandler_CFE_ES_GetResetType);

    return UT_GenStub_GetReturnValue(CFE_ES_GetResetType, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetTaskID()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetTaskID(CFE_ES_TaskId_t *TaskIdPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetTaskID, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetTaskID, CFE_ES_TaskId_t *, TaskIdPtr);

    UT_GenStub_Execute(CFE_ES_GetTaskID, Basic, UT_DefaultHandler_CFE_ES_GetTaskID);

    return UT_GenStub_GetReturnValue(CFE_ES_GetTaskID, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetTaskIDByName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetTaskIDByName(CFE_ES_TaskId_t *TaskIdPtr, const char *TaskName)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetTaskIDByName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetTaskIDByName, CFE_ES_TaskId_t *, TaskIdPtr);
    UT_GenStub_AddParam(CFE_ES_GetTaskIDByName, const char *, TaskName);

    UT_GenStub_Execute(CFE_ES_GetTaskIDByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetTaskIDByName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetTaskInfo()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, CFE_ES_TaskId_t TaskId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetTaskInfo, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetTaskInfo, CFE_ES_TaskInfo_t *, TaskInfo);
    UT_GenStub_AddParam(CFE_ES_GetTaskInfo, CFE_ES_TaskId_t, TaskId);

    UT_GenStub_Execute(CFE_ES_GetTaskInfo, Basic, UT_DefaultHandler_CFE_ES_GetTaskInfo);

    return UT_GenStub_GetReturnValue(CFE_ES_GetTaskInfo, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_GetTaskName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_GetTaskName(char *TaskName, CFE_ES_TaskId_t TaskId, size_t BufferLength)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_GetTaskName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_GetTaskName, char *, TaskName);
    UT_GenStub_AddParam(CFE_ES_GetTaskName, CFE_ES_TaskId_t, TaskId);
    UT_GenStub_AddParam(CFE_ES_GetTaskName, size_t, BufferLength);

    UT_GenStub_Execute(CFE_ES_GetTaskName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_GetTaskName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_IncrementGenCounter()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_IncrementGenCounter(CFE_ES_CounterId_t CounterId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_IncrementGenCounter, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_IncrementGenCounter, CFE_ES_CounterId_t, CounterId);

    UT_GenStub_Execute(CFE_ES_IncrementGenCounter, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_IncrementGenCounter, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_IncrementTaskCounter()
 * ----------------------------------------------------
 */
void CFE_ES_IncrementTaskCounter(void)
{

    UT_GenStub_Execute(CFE_ES_IncrementTaskCounter, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_LibID_ToIndex()
 * ----------------------------------------------------
 */
int32 CFE_ES_LibID_ToIndex(CFE_ES_LibId_t LibId, uint32 *Idx)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_LibID_ToIndex, int32);

    UT_GenStub_AddParam(CFE_ES_LibID_ToIndex, CFE_ES_LibId_t, LibId);
    UT_GenStub_AddParam(CFE_ES_LibID_ToIndex, uint32 *, Idx);

    UT_GenStub_Execute(CFE_ES_LibID_ToIndex, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_LibID_ToIndex, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_Main()
 * ----------------------------------------------------
 */
void CFE_ES_Main(uint32 StartType, uint32 StartSubtype, uint32 ModeId, const char *StartFilePath)
{
    UT_GenStub_AddParam(CFE_ES_Main, uint32, StartType);
    UT_GenStub_AddParam(CFE_ES_Main, uint32, StartSubtype);
    UT_GenStub_AddParam(CFE_ES_Main, uint32, ModeId);
    UT_GenStub_AddParam(CFE_ES_Main, const char *, StartFilePath);

    UT_GenStub_Execute(CFE_ES_Main, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_PerfLogAdd()
 * ----------------------------------------------------
 */
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
    UT_GenStub_AddParam(CFE_ES_PerfLogAdd, uint32, Marker);
    UT_GenStub_AddParam(CFE_ES_PerfLogAdd, uint32, EntryExit);

    UT_GenStub_Execute(CFE_ES_PerfLogAdd, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_PoolCreate()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_PoolCreate(CFE_ES_MemHandle_t *PoolID, void *MemPtr, size_t Size)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_PoolCreate, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_PoolCreate, CFE_ES_MemHandle_t *, PoolID);
    UT_GenStub_AddParam(CFE_ES_PoolCreate, void *, MemPtr);
    UT_GenStub_AddParam(CFE_ES_PoolCreate, size_t, Size);

    UT_GenStub_Execute(CFE_ES_PoolCreate, Basic, UT_DefaultHandler_CFE_ES_PoolCreate);

    return UT_GenStub_GetReturnValue(CFE_ES_PoolCreate, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_PoolCreateEx()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_PoolCreateEx(CFE_ES_MemHandle_t *PoolID, void *MemPtr, size_t Size, uint16 NumBlockSizes,
                                 const size_t *BlockSizes, bool UseMutex)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_PoolCreateEx, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_PoolCreateEx, CFE_ES_MemHandle_t *, PoolID);
    UT_GenStub_AddParam(CFE_ES_PoolCreateEx, void *, MemPtr);
    UT_GenStub_AddParam(CFE_ES_PoolCreateEx, size_t, Size);
    UT_GenStub_AddParam(CFE_ES_PoolCreateEx, uint16, NumBlockSizes);
    UT_GenStub_AddParam(CFE_ES_PoolCreateEx, const size_t *, BlockSizes);
    UT_GenStub_AddParam(CFE_ES_PoolCreateEx, bool, UseMutex);

    UT_GenStub_Execute(CFE_ES_PoolCreateEx, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_PoolCreateEx, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_PoolCreateNoSem()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t *PoolID, void *MemPtr, size_t Size)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_PoolCreateNoSem, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_PoolCreateNoSem, CFE_ES_MemHandle_t *, PoolID);
    UT_GenStub_AddParam(CFE_ES_PoolCreateNoSem, void *, MemPtr);
    UT_GenStub_AddParam(CFE_ES_PoolCreateNoSem, size_t, Size);

    UT_GenStub_Execute(CFE_ES_PoolCreateNoSem, Basic, UT_DefaultHandler_CFE_ES_PoolCreateNoSem);

    return UT_GenStub_GetReturnValue(CFE_ES_PoolCreateNoSem, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_PoolDelete()
 * ----------------------------------------------------
 */
int32 CFE_ES_PoolDelete(CFE_ES_MemHandle_t PoolID)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_PoolDelete, int32);

    UT_GenStub_AddParam(CFE_ES_PoolDelete, CFE_ES_MemHandle_t, PoolID);

    UT_GenStub_Execute(CFE_ES_PoolDelete, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_PoolDelete, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_ProcessAsyncEvent()
 * ----------------------------------------------------
 */
void CFE_ES_ProcessAsyncEvent(void)
{

    UT_GenStub_Execute(CFE_ES_ProcessAsyncEvent, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_PutPoolBuf()
 * ----------------------------------------------------
 */
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t Handle, CFE_ES_MemPoolBuf_t BufPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_PutPoolBuf, int32);

    UT_GenStub_AddParam(CFE_ES_PutPoolBuf, CFE_ES_MemHandle_t, Handle);
    UT_GenStub_AddParam(CFE_ES_PutPoolBuf, CFE_ES_MemPoolBuf_t, BufPtr);

    UT_GenStub_Execute(CFE_ES_PutPoolBuf, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_PutPoolBuf, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_RegisterCDS()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *CDSHandlePtr, size_t BlockSize, const char *Name)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_RegisterCDS, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_RegisterCDS, CFE_ES_CDSHandle_t *, CDSHandlePtr);
    UT_GenStub_AddParam(CFE_ES_RegisterCDS, size_t, BlockSize);
    UT_GenStub_AddParam(CFE_ES_RegisterCDS, const char *, Name);

    UT_GenStub_Execute(CFE_ES_RegisterCDS, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_RegisterCDS, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_RegisterGenCounter()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_RegisterGenCounter(CFE_ES_CounterId_t *CounterIdPtr, const char *CounterName)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_RegisterGenCounter, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_RegisterGenCounter, CFE_ES_CounterId_t *, CounterIdPtr);
    UT_GenStub_AddParam(CFE_ES_RegisterGenCounter, const char *, CounterName);

    UT_GenStub_Execute(CFE_ES_RegisterGenCounter, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_RegisterGenCounter, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_ReloadApp()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_ReloadApp(CFE_ES_AppId_t AppID, const char *AppFileName)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_ReloadApp, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_ReloadApp, CFE_ES_AppId_t, AppID);
    UT_GenStub_AddParam(CFE_ES_ReloadApp, const char *, AppFileName);

    UT_GenStub_Execute(CFE_ES_ReloadApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_ReloadApp, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_ResetCFE()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_ResetCFE(uint32 ResetType)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_ResetCFE, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_ResetCFE, uint32, ResetType);

    UT_GenStub_Execute(CFE_ES_ResetCFE, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_ResetCFE, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_RestartApp()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_RestartApp(CFE_ES_AppId_t AppID)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_RestartApp, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_RestartApp, CFE_ES_AppId_t, AppID);

    UT_GenStub_Execute(CFE_ES_RestartApp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_RestartApp, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_RestoreFromCDS()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_RestoreFromCDS, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_RestoreFromCDS, void *, RestoreToMemory);
    UT_GenStub_AddParam(CFE_ES_RestoreFromCDS, CFE_ES_CDSHandle_t, Handle);

    UT_GenStub_Execute(CFE_ES_RestoreFromCDS, Basic, UT_DefaultHandler_CFE_ES_RestoreFromCDS);

    return UT_GenStub_GetReturnValue(CFE_ES_RestoreFromCDS, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_RunLoop()
 * ----------------------------------------------------
 */
bool CFE_ES_RunLoop(uint32 *RunStatus)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_RunLoop, bool);

    UT_GenStub_AddParam(CFE_ES_RunLoop, uint32 *, RunStatus);

    UT_GenStub_Execute(CFE_ES_RunLoop, Basic, UT_DefaultHandler_CFE_ES_RunLoop);

    return UT_GenStub_GetReturnValue(CFE_ES_RunLoop, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_SetGenCount()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_SetGenCount(CFE_ES_CounterId_t CounterId, uint32 Count)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_SetGenCount, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_SetGenCount, CFE_ES_CounterId_t, CounterId);
    UT_GenStub_AddParam(CFE_ES_SetGenCount, uint32, Count);

    UT_GenStub_Execute(CFE_ES_SetGenCount, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_SetGenCount, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_TaskID_ToIndex()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_TaskID_ToIndex(CFE_ES_TaskId_t TaskID, uint32 *Idx)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_TaskID_ToIndex, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_TaskID_ToIndex, CFE_ES_TaskId_t, TaskID);
    UT_GenStub_AddParam(CFE_ES_TaskID_ToIndex, uint32 *, Idx);

    UT_GenStub_Execute(CFE_ES_TaskID_ToIndex, Basic, UT_DefaultHandler_CFE_ES_TaskID_ToIndex);

    return UT_GenStub_GetReturnValue(CFE_ES_TaskID_ToIndex, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_WaitForStartupSync()
 * ----------------------------------------------------
 */
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds)
{
    UT_GenStub_AddParam(CFE_ES_WaitForStartupSync, uint32, TimeOutMilliseconds);

    UT_GenStub_Execute(CFE_ES_WaitForStartupSync, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_WaitForSystemState()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_WaitForSystemState(uint32 MinSystemState, uint32 TimeOutMilliseconds)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_WaitForSystemState, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_WaitForSystemState, uint32, MinSystemState);
    UT_GenStub_AddParam(CFE_ES_WaitForSystemState, uint32, TimeOutMilliseconds);

    UT_GenStub_Execute(CFE_ES_WaitForSystemState, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_WaitForSystemState, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_WriteToSysLog()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...)
{
    va_list UtStub_ArgList;

    UT_GenStub_SetupReturnBuffer(CFE_ES_WriteToSysLog, CFE_Status_t);

    UT_GenStub_AddParam(CFE_ES_WriteToSysLog, const char *, SpecStringPtr);

    va_start(UtStub_ArgList, SpecStringPtr);
    UT_GenStub_Execute(CFE_ES_WriteToSysLog, Va, UT_DefaultHandler_CFE_ES_WriteToSysLog, UtStub_ArgList);
    va_end(UtStub_ArgList);

    return UT_GenStub_GetReturnValue(CFE_ES_WriteToSysLog, CFE_Status_t);
}
