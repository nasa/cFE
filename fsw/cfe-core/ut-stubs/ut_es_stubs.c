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
#include "cfe.h"
#include "utstubs.h"
#include "utassert.h"

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
#define CFE_UT_ES_POOL_STATIC_BLOCK_SIZE    4096

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_ES_CreateChildTask stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_CreateChildTask.  The user can adjust the response by setting
**        the values in the ES_CreateChildRtn structure prior to this function
**        being called.  If the value ES_CreateChildRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        ES_CreateChildRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_CreateChildTask(uint32 *TaskIdPtr,
                             const char *TaskName,
                             CFE_ES_ChildTaskMainFuncPtr_t FunctionPtr,
                             uint32 *StackPtr,
                             uint32 StackSize,
                             uint32 Priority,
                             uint32 Flags)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_CreateChildTask), TaskIdPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_CreateChildTask), TaskName);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_CreateChildTask), FunctionPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_CreateChildTask), StackPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_CreateChildTask), StackSize);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_CreateChildTask), Priority);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_CreateChildTask), Flags);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_CreateChildTask);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetAppID stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetAppID.  The user can adjust the response by setting
**        the values in the GetAppIDRtn structure prior to this function
**        being called.  If the value GetAppIDRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value GetAppIDRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetAppID(uint32 *AppIdPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetAppID), AppIdPtr);

    int32 status;
    uint32 *IdBuff;
    uint32 BuffSize;
    uint32 Position;

    status = UT_DEFAULT_IMPL(CFE_ES_GetAppID);

    if (status >= 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppID), (void **)&IdBuff, &BuffSize, &Position);
        if (IdBuff != NULL && BuffSize == sizeof(*AppIdPtr))
        {
            *AppIdPtr = *IdBuff;
        }
        else
        {
            *AppIdPtr = 0;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetAppIDByName stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetAppIDByName.  The user can adjust the response by setting
**        the value of UT_appname prior to this function being called, then
**        choosing specific values for the application name (pAppName) used
**        when calling this function.  The application ID returned is
**        dependent on the application name provided.  If application name
**        doesn't match the expected values the function returns an error
**        code.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either CFE_ES_ERR_APPNAME or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetAppIDByName(uint32 *AppIdPtr, const char *AppName)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetAppIDByName), AppIdPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetAppIDByName), AppName);

    uint32 UserBuffSize;
    uint32 BuffPosition;
    const char *NameBuff;
    uint32 *IdBuff;
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_GetAppIDByName);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_GetAppIDByName), (uint8*)AppIdPtr, sizeof(*AppIdPtr)) < sizeof(*AppIdPtr))
        {
            IdBuff = NULL;
            UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppName), (void**)&NameBuff, &UserBuffSize, &BuffPosition);
            if (NameBuff != NULL && UserBuffSize > 0 &&
                    strncmp(NameBuff, AppName, UserBuffSize) == 0)
            {
                UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppID), (void**)&IdBuff, &UserBuffSize, &BuffPosition);
            }

            if (IdBuff != NULL && UserBuffSize == sizeof(*AppIdPtr))
            {
                *AppIdPtr = *IdBuff;
            }
            else
            {
                *AppIdPtr = 0;
            }
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetAppName stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetAppName.  The user must set the value of UT_appname prior
**        to this function being called.  The function uses UT_appname for the
**        retrieved application name and returns CFE_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetAppName(char *AppName, uint32 AppId, uint32 BufferLength)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetAppName), AppName);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetAppName), AppId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetAppName), BufferLength);

    uint32 UserBuffSize;
    uint32 BuffPosition;
    const char *NameBuff;
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_GetAppName);

    if (status >= 0 && BufferLength > 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_ES_GetAppName), (void**)&NameBuff, &UserBuffSize, &BuffPosition);
        if (NameBuff == NULL || UserBuffSize == 0)
        {
            NameBuff = "UT";
            UserBuffSize = 2;
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

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RegisterApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RegisterApp.  The user can adjust the response by setting
**        the values in the ES_RegisterRtn structure prior to this function
**        being called.  If the value ES_RegisterRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_RegisterRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RegisterApp(void)
{
    int32   status;

    status = UT_DEFAULT_IMPL(CFE_ES_RegisterApp);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RegisterChildTask stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RegisterChildTask.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RegisterChildTask(void)
{
    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_ES_RegisterChildTask);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_WriteToSysLog stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_WriteToSysLog.  It always returns CFE_SUCCESS when called.
**        The log message is compared to known CFE function responses and if
**        a match is found the value WriteSysLogRtn.value is set to a
**        specified constant value.  WriteSysLogRtn.count is incremented to
**        keep track of the number of messages logged.  The unit test code
**        compares the WriteSysLogRtn value and count variables against
**        expected totals to ensure the proper response of other functions
**        being tested.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_WriteToSysLog(const char *SpecStringPtr, ...)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_WriteToSysLog), SpecStringPtr);

    int32   status;
    va_list va;

    va_start(va,SpecStringPtr);
    status = UT_DEFAULT_IMPL_VARARGS(CFE_ES_WriteToSysLog, va);
    va_end(va);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_WriteToSysLog), (const uint8*)SpecStringPtr, strlen(SpecStringPtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetPoolBuf stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetPoolBuf.  The user can adjust the response by setting
**        the values in the GetPoolRtn structure prior to this function
**        being called.  If the value GetPoolRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value GetPoolRtn.value.
**        Otherwise the buffer size is computed based on the Size value and
**        returned.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined value, or the buffer size based on Size
**        value (0xffffffff if Size exceeds maximum value allowed).
**
******************************************************************************/
int32 CFE_ES_GetPoolBuf(uint32 **BufPtr,
                        CFE_ES_MemHandle_t HandlePtr,
                        uint32 Size)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetPoolBuf), BufPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetPoolBuf), HandlePtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetPoolBuf), Size);

    static union
    {
        uint32 Start;
        CFE_ES_PoolAlign_t Align;
        uint8 Bytes[CFE_UT_ES_POOL_STATIC_BLOCK_SIZE];
    } Buffer;

    uint32 PoolSize;
    uint32 PositionStart;
    uint32 PositionEnd;
    void *PoolPtr;
    cpuaddr BufAddrStart;
    cpuaddr BufAddrEnd;
    int32 status;

    /*
     * Determine the actual alignment of the CFE_ES_PoolAlign_t structure.
     * This is done by checking the offset of a struct member of that type following a single byte.
     */
    const cpuaddr AlignMask = ((cpuaddr)&((struct { char Byte; CFE_ES_PoolAlign_t Align; } *)0)->Align) - 1;


    status = UT_DEFAULT_IMPL_RC(CFE_ES_GetPoolBuf, Size);

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
            PoolPtr = Buffer.Bytes;
            PoolSize = sizeof(Buffer);
            PositionStart = 0;
        }

        BufAddrStart = (cpuaddr)PoolPtr + PositionStart;
        BufAddrStart = (BufAddrStart + AlignMask) & ~AlignMask;
        BufAddrEnd = (BufAddrStart + Size + AlignMask) & ~AlignMask;
        PositionEnd = BufAddrEnd - (cpuaddr)PoolPtr;

        if (PositionEnd <= PoolSize)
        {
            *BufPtr = (uint32 *)BufAddrStart;
            memset((void*)BufAddrStart, 0x55, Size);

            /*
             * Unfortunately the UT assert stub library is missing
             * the ability to set the buffer position, the only way
             * to do it is by calling CopyFromLocal to advance the position.
             */
            Size = PositionEnd - PositionStart;
            while (Size > sizeof(Buffer))
            {
                UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_GetPoolBuf), &Buffer,
                        sizeof(Buffer));
                Size -= sizeof(Buffer);
            }
            UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_GetPoolBuf), &Buffer, Size);
        }
        else
        {
            /*
             * This a a bug in the test case.
             *
             * The buffer is insufficient, so the test case must
             * use UT_SetDataBuffer() to register a pool buffer that is
             * sufficient for the code under test.
             */
            UtAssert_Failed("Pool buffer empty in %s: need at least %lu bytes, given %lu",
                    __func__, (unsigned long)PositionEnd, (unsigned long)PoolSize);
            status = -1;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PoolCreate stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PoolCreate.  The user can adjust the response by setting
**        the values in the CreatePoolRtn structure prior to this function
**        being called.  If the value CreatePoolRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value CreatePoolRtn.value.
**        OS_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or OS_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_PoolCreate(cpuaddr *HandlePtr, uint8 *MemPtr, uint32 Size)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PoolCreate), HandlePtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PoolCreate), MemPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PoolCreate), Size);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_PoolCreate);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_ES_PoolCreate), (uint8*)HandlePtr, sizeof(*HandlePtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PoolCreateNoSem stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PoolCreateNoSem.  It always returns OS_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns OS_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_PoolCreateNoSem(CFE_ES_MemHandle_t *HandlePtr,
                             uint8 *MemPtr,
                             uint32 Size)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PoolCreateNoSem), HandlePtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PoolCreateNoSem), MemPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PoolCreateNoSem), Size);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_PoolCreateNoSem);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_ES_PoolCreateNoSem), (uint8*)HandlePtr, sizeof(*HandlePtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PoolCreateEx stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PoolCreateEx.  The user can adjust the response by setting
**        the values in the PoolCreateExRtn structure prior to this function
**        being called.  If the value PoolCreateExRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value PoolCreateExRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_PoolCreateEx(cpuaddr *HandlePtr,
                          uint8 *MemPtr,
                          uint32 Size,
                          uint32 NumBlockSizes,
                          uint32 *BlockSizes,
                          uint16 UseMutex)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PoolCreateEx), HandlePtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PoolCreateEx), MemPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PoolCreateEx), Size);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PoolCreateEx), NumBlockSizes);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PoolCreateEx), BlockSizes);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PoolCreateEx), UseMutex);

    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_ES_PoolCreateEx);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PutPoolBuf stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_PutPoolBuf.  The user can adjust the response by setting
**        the values in the PutPoolRtn structure prior to this function
**        being called.  If the value PutPoolRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value PutPoolRtn.value.
**        Otherwise the return value is positive, indicating success, if the
**        user sets UT_PutPool_Fail equal to zero prior to calling this
**        function; if UT_PutPool_Fail is non-zero then -1 is returned,
**        indicating failure.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag, 16, or -1.
**
******************************************************************************/
int32 CFE_ES_PutPoolBuf(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PutPoolBuf), HandlePtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_PutPoolBuf), BufPtr);

    int32   status;

    status = UT_DEFAULT_IMPL_RC(CFE_ES_PutPoolBuf, 16);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetPoolBufInfo stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetPoolBufInfo.  The user can adjust the response by setting
**        the values in the GetPoolInfoRtn structure prior to this function
**        being called.  If the value GetPoolInfoRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value GetPoolInfoRtn.value.
**        A positive value, indicating success, is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or 16.
**
******************************************************************************/
int32 CFE_ES_GetPoolBufInfo(CFE_ES_MemHandle_t HandlePtr, uint32 *BufPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetPoolBufInfo), HandlePtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetPoolBufInfo), BufPtr);

    int32   status;

    status = UT_DEFAULT_IMPL_RC(CFE_ES_GetPoolBufInfo, 16);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_PerfLogAdd stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_PerfLogAdd.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PerfLogAdd), Marker);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_PerfLogAdd), EntryExit);

    UT_DEFAULT_IMPL(CFE_ES_PerfLogAdd);
}

/*****************************************************************************/
/**
** \brief CFE_ES_CalculateCRC stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_CalculateCRC.  It is set to return a fixed CRC value for the
**        unit tests.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns 332424.
**
******************************************************************************/
uint32 CFE_ES_CalculateCRC(const void *DataPtr,
                           uint32 DataLength,
                           uint32 InputCRC,
                           uint32 TypeCRC)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_CalculateCRC), DataPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_CalculateCRC), DataLength);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_CalculateCRC), InputCRC);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_CalculateCRC), TypeCRC);

    uint32 result;

    UT_DEFAULT_IMPL(CFE_ES_CalculateCRC);

    if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_CalculateCRC), (uint8*)&result, sizeof(result)) < sizeof(result))
    {
        result = 332424;
    }

    return result;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetTaskInfo stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_GetTaskInfo.  The user can adjust the response by setting
**        the values in the ES_GetTaskInfoRtn structure prior to this function
**        being called.  If the value ES_GetTaskInfoRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_GetTaskInfoRtn.value.
**        CFE_SUCCESS is returned otherwise.  The user can adjust the
**        application name and task name by setting the value of UT_appname
**        prior to this function being called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_GetTaskInfo(CFE_ES_TaskInfo_t *TaskInfo, uint32 TaskId)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetTaskInfo), TaskInfo);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetTaskInfo), TaskId);

    int32   status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_ES_GetTaskInfo);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_GetTaskInfo), (uint8*)TaskInfo, sizeof(*TaskInfo)) < sizeof(*TaskInfo))
        {
            memset(TaskInfo, 0, sizeof(*TaskInfo));
            TaskInfo->AppId = 3; /* Fake ID number */
            strncpy((char *) &TaskInfo->AppName, "UT", sizeof(TaskInfo->AppName));
            strncpy((char *) &TaskInfo->TaskName, "UT", sizeof(TaskInfo->TaskName));
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_ExitApp stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_ExitApp.  The value passed to the function, ExitStatus, is
**        added to the variable ES_ExitAppRtn.value and the variable
**        ES_ExitAppRtn.count is incremented each time this function is called.
**        The unit tests compare these values to expected results to verify
**        proper system response.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_ExitApp(uint32 ExitStatus)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_ExitApp), ExitStatus);

    UT_DEFAULT_IMPL(CFE_ES_ExitApp);
    UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_ExitApp), (uint8*)&ExitStatus, sizeof(ExitStatus));
}

/*****************************************************************************/
/**
** \brief CFE_ES_CopyToCDS stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_CopyToCDS.  The user can adjust the response by setting
**        the values in the ES_CopyToCDSRtn structure prior to this function
**        being called.  If the value ES_CopyToCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_CopyToCDSRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_CopyToCDS(CFE_ES_CDSHandle_t Handle, void *DataToCopy)
{
    int32   status;
    uint32  CdsBufferSize;

    UT_Stub_RegisterContext(UT_KEY(CFE_ES_CopyToCDS), (void*)Handle);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_CopyToCDS), DataToCopy);
    status = UT_DEFAULT_IMPL(CFE_ES_CopyToCDS);

    if (status >= 0)
    {
        /* query the size of the supplied data buffer, if any */
        UT_GetDataBuffer(UT_KEY(CFE_ES_CopyToCDS), NULL, &CdsBufferSize, NULL);
        if (CdsBufferSize > 0)
        {
            UT_Stub_CopyFromLocal(UT_KEY(CFE_ES_CopyToCDS), DataToCopy, CdsBufferSize);
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RestoreFromCDS stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RestoreFromCDS.  The user can adjust the response by setting
**        the values in the ES_RestoreFromCDSRtn structure prior to this
**        function being called.  If the value ES_RestoreFromCDSRtn.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        ES_RestoreFromCDSRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RestoreFromCDS(void *RestoreToMemory, CFE_ES_CDSHandle_t Handle)
{
    int32   status;
    uint32  CdsBufferSize;

    UT_Stub_RegisterContext(UT_KEY(CFE_ES_RestoreFromCDS), RestoreToMemory);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_RestoreFromCDS), (void*)Handle);
    status = UT_DEFAULT_IMPL(CFE_ES_RestoreFromCDS);

    if (status >= 0)
    {
        /* query the size of the supplied data buffer, if any */
        UT_GetDataBuffer(UT_KEY(CFE_ES_RestoreFromCDS), NULL, &CdsBufferSize, NULL);
        if (CdsBufferSize > 0)
        {
            UT_Stub_CopyToLocal(UT_KEY(CFE_ES_RestoreFromCDS), RestoreToMemory, CdsBufferSize);
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_RegisterCDSEx stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_RegisterCDSEx.  The user can adjust the response by setting
**        the values in the ES_RegisterCDSRtn structure prior to this function
**        being called.  If the value ES_RegisterCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        ES_RegisterCDSRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr,
                           int32 BlockSize,
                           const char *Name,
                           bool CriticalTbl)
{
    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_ES_RegisterCDSEx);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_ES_RegisterCDSEx), (uint8*)HandlePtr, sizeof(*HandlePtr)) < sizeof(*HandlePtr))
        {
            *HandlePtr = 1;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_DeleteCDS stub function
**
** \par Description
**        This function is used to mimic the response of the cFE ES function
**        CFE_ES_DeleteCDS.  The user can adjust the response by setting
**        the values in the ES_DeleteCDSRtn structure prior to this function
**        being called.  If the value ES_DeleteCDSRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value ES_DeleteCDSRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_ES_DeleteCDS(const char *CDSName, bool CalledByTblServices)
{
    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_ES_DeleteCDS);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_GetResetType stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_GetResetType.  It returns the user-defined value,
**        GetResetTypeRtn.value.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns a user-defined status flag.
**
******************************************************************************/
int32 CFE_ES_GetResetType(uint32 *ResetSubtypePtr)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetResetType), ResetSubtypePtr);

    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_ES_GetResetType);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_ES_GetResetType), (uint8*)ResetSubtypePtr, sizeof(*ResetSubtypePtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_IncrementTaskCounter stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_IncrementTaskCounter.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_IncrementTaskCounter(void)
{
    UT_DEFAULT_IMPL(CFE_ES_IncrementTaskCounter);
}

int32 CFE_ES_WaitForSystemState(uint32 MinSystemState, uint32 TimeOutMilliseconds)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_WaitForSystemState), MinSystemState);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_WaitForSystemState), TimeOutMilliseconds);

    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_ES_WaitForSystemState);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_ES_WaitForStartupSync stub function
**
** \par Description
**        This function is used as a placeholder for the cFE ES function
**        CFE_ES_WaitForStartupSync.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_ES_WaitForStartupSync(uint32 TimeOutMilliseconds)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_WaitForStartupSync), TimeOutMilliseconds);

    UT_DEFAULT_IMPL(CFE_ES_WaitForStartupSync);
}

bool CFE_ES_RunLoop(uint32 *ExitStatus)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_RunLoop), ExitStatus);

    return UT_DEFAULT_IMPL(CFE_ES_RunLoop) != 0;
}

int32 CFE_ES_RegisterCDS(CFE_ES_CDSHandle_t *HandlePtr, int32 BlockSize, const char *Name)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_RegisterCDS), HandlePtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_RegisterCDS), BlockSize);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_RegisterCDS), Name);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_RegisterCDS);

    return status;
}

void CFE_ES_ExitChildTask(void)
{
    UT_DEFAULT_IMPL(CFE_ES_ExitChildTask);
}

int32 CFE_ES_DeleteApp(uint32 AppID)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_DeleteApp), AppID);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_DeleteApp);

    return status;
}

int32 CFE_ES_DeleteChildTask(uint32 TaskId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_DeleteChildTask), TaskId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_DeleteChildTask);

    return status;
}

int32 CFE_ES_DeleteGenCounter(uint32 CounterId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_DeleteGenCounter), CounterId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_DeleteGenCounter);

    return status;
}

int32 CFE_ES_GetAppInfo(CFE_ES_AppInfo_t *AppInfo, uint32 AppId)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetAppInfo), AppInfo);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetAppInfo), AppId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_GetAppInfo);

    return status;
}

int32 CFE_ES_GetGenCount(uint32 CounterId, uint32 *Count)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetGenCount), CounterId);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetGenCount), Count);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_GetGenCount);

    return status;
}

int32 CFE_ES_GetGenCounterIDByName(uint32 *CounterIdPtr, const char *CounterName)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetGenCounterIDByName), CounterIdPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetGenCounterIDByName), CounterName);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_GetGenCounterIDByName);

    return status;
}

int32 CFE_ES_GetMemPoolStats(CFE_ES_MemPoolStats_t *BufPtr, CFE_ES_MemHandle_t Handle)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_GetMemPoolStats), BufPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_GetMemPoolStats), Handle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_GetMemPoolStats);

    return status;
}

int32 CFE_ES_IncrementGenCounter(uint32 CounterId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_IncrementGenCounter), CounterId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_IncrementGenCounter);

    return status;
}

int32 CFE_ES_RegisterGenCounter(uint32 *CounterIdPtr, const char *CounterName)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_RegisterGenCounter), CounterIdPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_RegisterGenCounter), CounterName);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_RegisterGenCounter);

    return status;
}

int32 CFE_ES_ReloadApp(uint32 AppID, const char *AppFileName)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_ReloadApp), AppID);
    UT_Stub_RegisterContext(UT_KEY(CFE_ES_ReloadApp), AppFileName);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_ReloadApp);

    return status;
}

int32 CFE_ES_ResetCFE(uint32 ResetType)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_ResetCFE), ResetType);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_ResetCFE);

    return status;
}

int32 CFE_ES_RestartApp(uint32 AppID)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_RestartApp), AppID);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_RestartApp);

    return status;
}

int32 CFE_ES_SetGenCount(uint32 CounterId, uint32 Count)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_SetGenCount), CounterId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ES_SetGenCount), Count);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_ES_SetGenCount);

    return status;
}

