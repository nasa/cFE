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
** File: ut_es_stubs.c
**
** Purpose:
** Unit test stubs for Executive Service routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_es.h"
#include "cfe_resourceid.h"
#include "cfe_resourceid_basevalue.h"

#include "utstubs.h"
#include "utassert.h"

/*
 * Assign ID base values for UT
 */
enum
{
    UT_CFE_ES_TASKID_BASE     = CFE_RESOURCEID_MAKE_BASE(0x21),
    UT_CFE_ES_APPID_BASE      = CFE_RESOURCEID_MAKE_BASE(0x22),
    UT_CFE_ES_LIBID_BASE      = CFE_RESOURCEID_MAKE_BASE(0x23),
    UT_CFE_ES_COUNTID_BASE    = CFE_RESOURCEID_MAKE_BASE(0x24),
    UT_CFE_ES_POOLID_BASE     = CFE_RESOURCEID_MAKE_BASE(0x25),
    UT_CFE_ES_CDSBLOCKID_BASE = CFE_RESOURCEID_MAKE_BASE(0x26)
};

struct UT_AlignTest
{
    char               Byte;
    CFE_ES_PoolAlign_t Align;
};

/*
 * Determine the actual alignment of the CFE_ES_PoolAlign_t structure.
 * This is done by checking the offset of a struct member of that type following a single byte.
 */
static const cpuaddr UT_ESPOOL_ALIGN_MASK = ((cpuaddr) & ((struct UT_AlignTest *)0)->Align) - 1;

/*
 * Unit-test stub definitions/limits
 *
 * Note these limits only apply to the ES _stubs_ and not
 * the normal implementation.  It should not be necessary
 * to configure these on a deployment basis.
 */

/*
 * Maximum block size for ES pool requests
 *
 * This is only for pool block requests where the test
 * case does _not_ register its own buffer, and therefore
 * gets serviced from the default (static) pool buffer.
 *
 * This fixed value should be enough for most simple test
 * cases.  If a test case requires a larger block, it should
 * register its own simulated pool using UT_SetDataBuffer,
 * rather than changing this value.
 */
#define CFE_UT_ES_POOL_STATIC_BLOCK_SIZE 4096

/*
 * Default value to return from calls that output an App ID, if the
 * test case does not provide a value
 */
#define CFE_UT_ES_DEFAULT_APPID CFE_ES_APPID_C(CFE_ResourceId_FromInteger(UT_CFE_ES_APPID_BASE + 1))

/*
 * Default value to return from calls that output a Task ID, if the
 * test case does not provide a value
 */
#define CFE_UT_ES_DEFAULT_TASKID CFE_ES_TASKID_C(CFE_ResourceId_FromInteger(UT_CFE_ES_TASKID_BASE + 1))

/*
 * Default value to return from calls that output a CDS ID, if the
 * test case does not provide a value
 */
#define CFE_UT_ES_DEFAULT_CDSID CFE_ES_CDSHANDLE_C(CFE_ResourceId_FromInteger(UT_CFE_ES_CDSBLOCKID_BASE + 1))

/*
 * Invalid value to output from calls as resource ID for the
 * calls that return failure.  If subsequently used by application code,
 * it will likely induce a segfault or other noticeably bad behavior.
 */
#define CFE_UT_ES_ID_INVALID CFE_ResourceId_FromInteger(0xDEADBEEF)

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_CreateChildTask coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_CreateChildTask(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_TaskId_t *TaskIdPtr = UT_Hook_GetArgValueByName(Context, "TaskIdPtr", CFE_ES_TaskId_t *);
    int32            status;
    void *           IdBuff;
    size_t           BuffSize;
    size_t           Position;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppID), &IdBuff, &BuffSize, &Position);
        if (IdBuff != NULL && BuffSize == sizeof(*TaskIdPtr))
        {
            memcpy(TaskIdPtr, IdBuff, sizeof(*TaskIdPtr));
        }
        else
        {
            *TaskIdPtr = CFE_UT_ES_DEFAULT_TASKID;
        }
    }

    if (status < 0)
    {
        *TaskIdPtr = CFE_ES_TASKID_UNDEFINED;
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_GetAppID coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_GetAppID(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_AppId_t *AppIdPtr = UT_Hook_GetArgValueByName(Context, "AppIdPtr", CFE_ES_AppId_t *);
    int32           status;
    void *          IdBuff;
    size_t          BuffSize;
    size_t          Position;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppID), &IdBuff, &BuffSize, &Position);
        if (IdBuff != NULL && BuffSize == sizeof(*AppIdPtr))
        {
            memcpy(AppIdPtr, IdBuff, sizeof(*AppIdPtr));
        }
        else
        {
            *AppIdPtr = CFE_UT_ES_DEFAULT_APPID;
        }
    }

    if (status < 0)
    {
        *AppIdPtr = CFE_ES_APPID_C(CFE_UT_ES_ID_INVALID);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_GetTaskID coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_GetTaskID(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_TaskId_t *TaskIdPtr = UT_Hook_GetArgValueByName(Context, "TaskIdPtr", CFE_ES_TaskId_t *);
    int32            status;
    void *           IdBuff;
    size_t           BuffSize;
    size_t           Position;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_ES_GetTaskID), &IdBuff, &BuffSize, &Position);
        if (IdBuff != NULL && BuffSize == sizeof(*TaskIdPtr))
        {
            memcpy(TaskIdPtr, IdBuff, sizeof(*TaskIdPtr));
        }
        else
        {
            *TaskIdPtr = CFE_UT_ES_DEFAULT_TASKID;
        }
    }

    if (status < 0)
    {
        *TaskIdPtr = CFE_ES_TASKID_C(CFE_UT_ES_ID_INVALID);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_GetAppIDByName coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_GetAppIDByName(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_AppId_t *AppIdPtr = UT_Hook_GetArgValueByName(Context, "AppIdPtr", CFE_ES_AppId_t *);
    const char *    AppName  = UT_Hook_GetArgValueByName(Context, "AppName", const char *);

    size_t UserBuffSize;
    size_t BuffPosition;
    void * NameBuff;
    void * IdBuff;
    int32  status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_GetAppIDByName), AppIdPtr, sizeof(*AppIdPtr)) < sizeof(*AppIdPtr))
        {
            IdBuff = NULL;
            UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppName), &NameBuff, &UserBuffSize, &BuffPosition);
            if (NameBuff != NULL && UserBuffSize > 0 && strncmp((const char *)NameBuff, AppName, UserBuffSize) == 0)
            {
                UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppID), &IdBuff, &UserBuffSize, &BuffPosition);
            }

            if (IdBuff != NULL && UserBuffSize == sizeof(*AppIdPtr))
            {
                memcpy(AppIdPtr, IdBuff, sizeof(*AppIdPtr));
            }
            else
            {
                *AppIdPtr = CFE_UT_ES_DEFAULT_APPID;
            }
        }
    }

    if (status < 0)
    {
        *AppIdPtr = CFE_ES_APPID_C(CFE_UT_ES_ID_INVALID);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_GetAppName coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_GetAppName(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    char * AppName      = UT_Hook_GetArgValueByName(Context, "AppName", char *);
    size_t BufferLength = UT_Hook_GetArgValueByName(Context, "BufferLength", size_t);

    size_t      UserBuffSize;
    size_t      BuffPosition;
    const char *NameBuff;
    void *      TempBuff;
    int32       status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0 && BufferLength > 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppName), &TempBuff, &UserBuffSize, &BuffPosition);
        if (TempBuff == NULL || UserBuffSize == 0)
        {
            NameBuff     = "UT";
            UserBuffSize = 2;
        }
        else
        {
            NameBuff = TempBuff;
        }

        if (UserBuffSize < BufferLength)
        {
            BuffPosition = UserBuffSize;
        }
        else
        {
            BuffPosition = BufferLength - 1;
        }

        strncpy(AppName, NameBuff, BuffPosition);
        AppName[BuffPosition] = 0;
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_WriteToSysLog coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_WriteToSysLog(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context,
                                            va_list va)
{
    const char *SpecStringPtr = UT_Hook_GetArgValueByName(Context, "SpecStringPtr", const char *);

    int32 status;
    char  str[128];
    char *newline;

    vsnprintf(str, sizeof(str), SpecStringPtr, va);

    /* Replace newline since UtDebug already adds one */
    newline = strchr(str, '\n');
    if (newline != NULL)
    {
        *newline = '\0';
    }

    UtDebug("CFE_ES_WriteToSysLog: %s", str);

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_WriteToSysLog), SpecStringPtr, strlen(SpecStringPtr));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_GetPoolBuf coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_GetPoolBuf(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_MemPoolBuf_t *BufPtr = UT_Hook_GetArgValueByName(Context, "BufPtr", CFE_ES_MemPoolBuf_t *);
    size_t               Size   = UT_Hook_GetArgValueByName(Context, "Size", size_t);

    static union
    {
        uint32             Start;
        CFE_ES_PoolAlign_t Align;
        uint8              Bytes[CFE_UT_ES_POOL_STATIC_BLOCK_SIZE];
    } Buffer;

    size_t  PoolSize;
    size_t  PositionStart;
    size_t  PositionEnd;
    void *  PoolPtr;
    cpuaddr BufAddrStart;
    cpuaddr BufAddrEnd;
    int32   status;

    if (!UT_Stub_GetInt32StatusCode(Context, &status))
    {
        status = Size;
    }

    if (status > 0)
    {
        Size = status;

        UT_GetDataBuffer(UT_KEY(CFE_ES_GetPoolBuf), (void **)&PoolPtr, &PoolSize, &PositionStart);
        if (PoolSize == 0)
        {
            /*
             * This means the test case did not register a buffer.
             * Use the static buffer to fulfill the request.
             */
            PoolPtr       = Buffer.Bytes;
            PoolSize      = sizeof(Buffer);
            PositionStart = 0;
        }

        BufAddrStart = (cpuaddr)PoolPtr + PositionStart;
        BufAddrStart = (BufAddrStart + UT_ESPOOL_ALIGN_MASK) & ~UT_ESPOOL_ALIGN_MASK;
        BufAddrEnd   = (BufAddrStart + Size + UT_ESPOOL_ALIGN_MASK) & ~UT_ESPOOL_ALIGN_MASK;
        PositionEnd  = BufAddrEnd - (cpuaddr)PoolPtr;

        if (PositionEnd <= PoolSize)
        {
            *BufPtr = CFE_ES_MEMPOOLBUF_C(BufAddrStart);
            memset((void *)BufAddrStart, 0x55, Size);

            /*
             * Unfortunately the UT assert stub library is missing
             * the ability to set the buffer position, the only way
             * to do it is by calling CopyFromLocal to advance the position.
             */
            Size = PositionEnd - PositionStart;
            while (Size > sizeof(Buffer))
            {
                UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_GetPoolBuf), &Buffer, sizeof(Buffer));
                Size -= sizeof(Buffer);
            }
            UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_GetPoolBuf), &Buffer, Size);
        }
        else
        {
            /*
             * This is a bug in the test case.
             *
             * The buffer is insufficient, so the test case must
             * use UT_SetDataBuffer() to register a pool buffer that is
             * sufficient for the code under test.
             */
            UtAssert_Failed("Pool buffer empty in %s: need at least %lu bytes, given %lu", __func__,
                            (unsigned long)PositionEnd, (unsigned long)PoolSize);

            UtAssert_Abort("Configuration error, pool buffer too small for test cases");
        }
    }

    UT_Stub_SetReturnValue(FuncKey, status);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_PoolCreate coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_PoolCreate(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_MemHandle_t *PoolID = UT_Hook_GetArgValueByName(Context, "PoolID", CFE_ES_MemHandle_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_ES_PoolCreate), PoolID, sizeof(*PoolID));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_PoolCreateNoSem coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_PoolCreateNoSem(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_MemHandle_t *PoolID = UT_Hook_GetArgValueByName(Context, "PoolID", CFE_ES_MemHandle_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_ES_PoolCreateNoSem), PoolID, sizeof(*PoolID));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_GetTaskInfo coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_GetTaskInfo(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_TaskInfo_t *TaskInfo = UT_Hook_GetArgValueByName(Context, "TaskInfo", CFE_ES_TaskInfo_t *);

    int32 status = CFE_SUCCESS;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_GetTaskInfo), (uint8 *)TaskInfo, sizeof(*TaskInfo)) < sizeof(*TaskInfo))
        {
            memset(TaskInfo, 0, sizeof(*TaskInfo));
            TaskInfo->AppId = CFE_UT_ES_DEFAULT_APPID;
            strncpy((char *)&TaskInfo->AppName, "UT", sizeof(TaskInfo->AppName));
            strncpy((char *)&TaskInfo->TaskName, "UT", sizeof(TaskInfo->TaskName));
        }
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_ExitApp coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_ExitApp(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint32 ExitStatus = UT_Hook_GetArgValueByName(Context, "ExitStatus", uint32);

    UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_ExitApp), &ExitStatus, sizeof(ExitStatus));
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_RunLoop coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_RunLoop(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32 status;
    bool  return_value;

    UT_Stub_GetInt32StatusCode(Context, &status);

    return_value = (status != 0);

    UT_Stub_SetReturnValue(FuncKey, return_value);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_CopyToCDS coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_CopyToCDS(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void *DataToCopy = UT_Hook_GetArgValueByName(Context, "DataToCopy", void *);

    int32  status;
    size_t CdsBufferSize;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        /* query the size of the supplied data buffer, if any */
        UT_GetDataBuffer(UT_KEY(CFE_ES_CopyToCDS), NULL, &CdsBufferSize, NULL);
        if (CdsBufferSize > 0)
        {
            UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_CopyToCDS), DataToCopy, CdsBufferSize);
        }
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_RestoreFromCDS coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_RestoreFromCDS(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void *RestoreToMemory = UT_Hook_GetArgValueByName(Context, "RestoreToMemory", void *);

    int32  status;
    size_t CdsBufferSize;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        /* query the size of the supplied data buffer, if any */
        UT_GetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), NULL, &CdsBufferSize, NULL);
        if (CdsBufferSize > 0)
        {
            UT_Stub_CopyToLocal(UT_KEY(CFE_ES_RestoreFromCDS), RestoreToMemory, CdsBufferSize);
        }
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_GetResetType coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_GetResetType(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint32 *ResetSubtypePtr = UT_Hook_GetArgValueByName(Context, "ResetSubtypePtr", uint32 *);
    int32   status          = CFE_SUCCESS;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_ES_GetResetType), ResetSubtypePtr, sizeof(*ResetSubtypePtr));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_AppID_ToIndex coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_AppID_ToIndex(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_AppId_t AppID = UT_Hook_GetArgValueByName(Context, "AppID", CFE_ES_AppId_t);
    uint32 *       Idx   = UT_Hook_GetArgValueByName(Context, "Idx", uint32 *);
    int32          return_code;

    UT_Stub_GetInt32StatusCode(Context, &return_code);

    if (return_code != CFE_SUCCESS)
    {
        *Idx = 0xDEADBEEFU;
    }
    else if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_AppID_ToIndex), Idx, sizeof(*Idx)) < sizeof(*Idx))
    {
        *Idx = CFE_RESOURCEID_TO_ULONG(AppID) & 0xFFFF;
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ES_TaskID_ToIndex coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ES_TaskID_ToIndex(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ES_TaskId_t TaskID = UT_Hook_GetArgValueByName(Context, "TaskID", CFE_ES_TaskId_t);
    uint32 *        Idx    = UT_Hook_GetArgValueByName(Context, "Idx", uint32 *);
    int32           return_code;

    UT_Stub_GetInt32StatusCode(Context, &return_code);

    if (return_code != CFE_SUCCESS)
    {
        *Idx = 0xDEADBEEFU;
    }
    else if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_TaskID_ToIndex), Idx, sizeof(*Idx)) < sizeof(*Idx))
    {
        *Idx = CFE_RESOURCEID_TO_ULONG(TaskID) & 0xFFFF;
    }
}
