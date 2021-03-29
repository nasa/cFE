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
** File: ut_sb_stubs.c
**
** Purpose:
** Unit test stubs for Software Bus routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_sb.h"
#include "cfe_time_extern_typedefs.h"

#include "utstubs.h"

typedef struct
{
    CFE_SB_MsgId_t     MsgId;
    uint32             UserLength;
    uint32             TotalLength;
    uint16             CommandCode;
    CFE_TIME_SysTime_t TimeStamp;

} CFE_SB_StubMsg_MetaData_t;

static CFE_SB_StubMsg_MetaData_t *CFE_SB_StubMsg_GetMetaData(const CFE_MSG_Message_t *MsgPtr)
{
    CFE_SB_StubMsg_MetaData_t *MetaPtr;
    CFE_SB_StubMsg_MetaData_t  DefaultMeta;
    size_t                     MetaSize;
    UT_EntryKey_t              MsgKey = (UT_EntryKey_t)MsgPtr;

    UT_GetDataBuffer(MsgKey, (void **)&MetaPtr, &MetaSize, NULL);
    if (MetaPtr == NULL || MetaSize != sizeof(DefaultMeta))
    {
        memset(&DefaultMeta, 0, sizeof(DefaultMeta));
        DefaultMeta.MsgId = CFE_SB_INVALID_MSG_ID;
        UT_ResetState(MsgKey);
        UT_SetDataBuffer(MsgKey, &DefaultMeta, sizeof(DefaultMeta), true);

        /* Because "allocate copy" is true above, this gets a pointer to the copy */
        UT_GetDataBuffer(MsgKey, (void **)&MetaPtr, &MetaSize, NULL);
    }

    return MetaPtr;
}
/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_SB_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_EarlyInit.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_EarlyInit(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_EarlyInit);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_TaskMain(void)
{
    UT_DEFAULT_IMPL(CFE_SB_TaskMain);
}

/*****************************************************************************/
/**
** \brief CFE_SB_CreatePipe stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_CreatePipe.  The user can adjust the response by setting
**        the values in the SB_CreatePipeRtn structure prior to this function
**        being called.  If the value SB_CreatePipeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SB_CreatePipeRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_CreatePipe), PipeIdPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_CreatePipe), Depth);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_CreatePipe), PipeName);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_CreatePipe);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_SB_CreatePipe), (uint8 *)PipeIdPtr, sizeof(*PipeIdPtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_DeletePipe stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_DeletePipe.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_DeletePipe), PipeId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_DeletePipe);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_DeletePipe), &PipeId, sizeof(PipeId));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetPipeName stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_GetPipeName. The user should set the data buffer using UT_SetDataBuffer
**        prior to this function being called. Otherwise, the dummy "UT" name will
**        be copied to PipeNameBuf.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_GetPipeName(char *PipeNameBuf, size_t PipeNameSize, CFE_SB_PipeId_t PipeId)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_GetPipeName), PipeNameBuf);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_GetPipeName), PipeNameSize);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_GetPipeName), PipeId);

    size_t      UserBuffSize;
    size_t      BuffPosition;
    const char *NameBuff;
    int32       status;

    status = UT_DEFAULT_IMPL(CFE_SB_GetPipeName);

    if (status >= 0 && PipeNameSize > 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_SB_GetPipeName), (void **)&NameBuff, &UserBuffSize, &BuffPosition);
        if (NameBuff == NULL || UserBuffSize == 0)
        {
            NameBuff     = "UT";
            UserBuffSize = 2;
        }

        if (UserBuffSize < PipeNameSize)
        {
            BuffPosition = UserBuffSize;
        }
        else
        {
            BuffPosition = PipeNameSize - 1;
        }

        strncpy(PipeNameBuf, NameBuff, BuffPosition);
        PipeNameBuf[BuffPosition] = 0;
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetPipeIdByName stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_GetPipeIdByName.  The user can adjust the response by setting
**        the value of UT_pipename prior to this function being called, then
**        choosing specific values for the pipe name (PipeName) used
**        when calling this function.  The Pipe ID returned is
**        dependent on the pipe name provided.  If pipe name
**        doesn't match the expected values the function returns an error
**        code.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either CFE_SB_BAD_ARGUMENT or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_GetPipeIdByName(CFE_SB_PipeId_t *PipeIdPtr, const char *PipeName)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_GetPipeIdByName), PipeIdPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_GetPipeIdByName), PipeName);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_GetPipeIdByName);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_GetPipeIdByName), (uint8 *)PipeIdPtr, sizeof(*PipeIdPtr)) ==
            sizeof(*PipeIdPtr))
        {
            status = CFE_SUCCESS;
        }
        else
        {
            status     = CFE_SB_BAD_ARGUMENT;
            *PipeIdPtr = CFE_SB_INVALID_PIPE;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_ReceiveBuffer stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_ReceiveBuffer.  By default it will return the TIMEOUT error response,
**        unless the test setup sequence has indicated otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS or overridden unit test value
**
******************************************************************************/
int32 CFE_SB_ReceiveBuffer(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_ReceiveBuffer), BufPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_ReceiveBuffer), PipeId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_ReceiveBuffer), TimeOut);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_ReceiveBuffer);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_SB_ReceiveBuffer), (uint8 *)BufPtr, sizeof(*BufPtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_TransmitMsg stub function
**
** \par Description
**        This function is implements the stub version of the real implementation.
**        Adds the message pointer value to the test buffer if status is
**        positive
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS or overridden unit test value
**
******************************************************************************/
int32 CFE_SB_TransmitMsg(CFE_MSG_Message_t *MsgPtr, bool IncrementSequenceCount)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_TransmitMsg), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_TransmitMsg), IncrementSequenceCount);

    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_SB_TransmitMsg);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_TransmitMsg), &MsgPtr, sizeof(MsgPtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_TransmitBuffer stub function
**
** \par Description
**        This function is implements the stub version of the real implementation.
**        Adds the buffer pointer value to the test buffer if status is
**        positive
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS or overridden unit test value
**
******************************************************************************/
int32 CFE_SB_TransmitBuffer(CFE_SB_Buffer_t *BufPtr, bool IncrementSequenceCount)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_TransmitBuffer), BufPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_TransmitBuffer), IncrementSequenceCount);

    int32 status = CFE_SUCCESS;

    status = UT_DEFAULT_IMPL(CFE_SB_TransmitBuffer);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_TransmitBuffer), &BufPtr, sizeof(BufPtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SubscribeEx stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SubscribeEx.  The user can adjust the response by setting
**        the values in the SB_SubscribeExRtn structure prior to this function
**        being called.  If the value SB_SubscribeExRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        SB_SubscribeExRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SubscribeEx(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SubscribeEx), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SubscribeEx), PipeId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SubscribeEx), Quality);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SubscribeEx), MsgLim);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_SubscribeEx);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_Subscribe stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_Subscribe.  The user can adjust the response by setting
**        the values in the SB_SubscribeRtn structure prior to this function
**        being called.  If the value SB_SubscribeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SB_SubscribeRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_Subscribe), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_Subscribe), PipeId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_Subscribe);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SubscribeLocal stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SubscribeLocal.  The user can adjust the response by setting
**        the values in the SB_SubscribeLocalRtn structure prior to this
**        function being called.  If the value SB_SubscribeLocalRtn.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        SB_SubscribeLocalRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint16 MsgLim)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SubscribeLocal), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SubscribeLocal), PipeId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SubscribeLocal), MsgLim);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_SubscribeLocal);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_TimeStampMsg stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_TimeStampMsg.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_TimeStampMsg(CFE_MSG_Message_t *MsgPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_TimeStampMsg), MsgPtr);

    UT_DEFAULT_IMPL(CFE_SB_TimeStampMsg);
    UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_TimeStampMsg), &MsgPtr, sizeof(MsgPtr));
}

/*****************************************************************************/
/**
** \brief CFE_SB_CleanUpApp stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_CleanUpApp.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
int32 CFE_SB_CleanUpApp(CFE_ES_AppId_t AppId)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_CleanUpApp);

    return status;
}

/******************************************************************************
**  Function:  CFE_SB_MessageStringGet()
**
**  See function prototype for full description
**
*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString,
                              size_t DestMaxSize, size_t SourceMaxSize)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringGet), DestStringPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringGet), SourceStringPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringGet), DefaultString);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_MessageStringGet), DestMaxSize);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_MessageStringGet), SourceMaxSize);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_MessageStringGet);

    if (status == 0)
    {
        if (DestMaxSize == 0)
        {
            status = CFE_SB_BAD_ARGUMENT;
        }
        else
        {
            /*
             * Check if should use the default, which is if
             * the source string has zero length (first char is NUL).
             */
            if (DefaultString != NULL && (SourceMaxSize == 0 || *SourceStringPtr == 0))
            {
                SourceStringPtr = DefaultString;
                SourceMaxSize   = DestMaxSize;
            }

            /* For the UT implementation, just call strncpy() */
            strncpy(DestStringPtr, SourceStringPtr, DestMaxSize - 1);
            DestStringPtr[DestMaxSize - 1] = 0;

            status = strlen(DestStringPtr);
        }
    }

    return status;
}

/******************************************************************************
**  Function:  CFE_SB_MessageStringSet()
**
**  See function prototype for full description
**
*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, size_t DestMaxSize,
                              size_t SourceMaxSize)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringSet), DestStringPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringSet), SourceStringPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_MessageStringSet), DestMaxSize);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_MessageStringSet), SourceMaxSize);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_MessageStringSet);

    if (status == 0)
    {
        if (DestMaxSize == 0)
        {
            status = CFE_SB_BAD_ARGUMENT;
        }
        else
        {
            /* For the UT implementation, just call strncpy() */
            strncpy(DestStringPtr, SourceStringPtr, DestMaxSize);
            if (DestStringPtr[DestMaxSize - 1] != 0)
            {
                status = DestMaxSize;
            }
            else
            {
                status = strlen(DestStringPtr);
            }
        }
    }

    return status;
}

int32 CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_Unsubscribe), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_Unsubscribe), PipeId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_Unsubscribe);

    return status;
}

void *CFE_SB_GetUserData(CFE_MSG_Message_t *MsgPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_GetUserData), MsgPtr);

    uint8 *BytePtr;
    void * Result;
    uint16 HdrSize;

    UT_DEFAULT_IMPL(CFE_SB_GetUserData);

    if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_GetUserData), &Result, sizeof(Result)) != sizeof(Result))
    {
        BytePtr = (uint8 *)MsgPtr;
        if ((MsgPtr->Byte[0] & 0x10) != 0)
        {
            HdrSize = sizeof(CFE_MSG_CommandHeader_t);
        }
        else
        {
            HdrSize = sizeof(CFE_MSG_TelemetryHeader_t);
        }

        Result = (BytePtr + HdrSize);
    }

    return Result;
}

int32 CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_GetPipeOpts), PipeId);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_GetPipeOpts), OptPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_GetPipeOpts);

    return status;
}

size_t CFE_SB_GetUserDataLength(const CFE_MSG_Message_t *MsgPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_GetUserDataLength), MsgPtr);

    int32 status;

    status = UT_DEFAULT_IMPL_RC(CFE_SB_GetUserDataLength, -1);
    if (status < 0)
    {
        status = CFE_SB_StubMsg_GetMetaData(MsgPtr)->UserLength;
    }

    return status;
}

bool CFE_SB_IsValidMsgId(CFE_SB_MsgId_t MsgId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_IsValidMsgId), MsgId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_IsValidMsgId);

    return status;
}

int32 CFE_SB_PassMsg(CFE_MSG_Message_t *MsgPtr)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_PassMsg), MsgPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_PassMsg);

    return status;
}

int32 CFE_SB_SetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 Opts)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SetPipeOpts), PipeId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SetPipeOpts), Opts);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_SetPipeOpts);

    return status;
}

void CFE_SB_SetUserDataLength(CFE_MSG_Message_t *MsgPtr, size_t DataLength)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SetUserDataLength), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_SetUserDataLength), DataLength);

    UT_DEFAULT_IMPL(CFE_SB_SetUserDataLength);
    CFE_SB_StubMsg_GetMetaData(MsgPtr)->UserLength = DataLength;
}

int32 CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_UnsubscribeLocal), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_UnsubscribeLocal), PipeId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_UnsubscribeLocal);

    return status;
}

CFE_SB_Buffer_t *CFE_SB_AllocateMessageBuffer(size_t MsgSize)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_AllocateMessageBuffer), MsgSize);

    int32            status;
    CFE_SB_Buffer_t *SBBufPtr = NULL;

    status = UT_DEFAULT_IMPL(CFE_SB_AllocateMessageBuffer);

    if (status == CFE_SUCCESS)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_SB_AllocateMessageBuffer), &SBBufPtr, sizeof(SBBufPtr));
    }

    return SBBufPtr;
}

int32 CFE_SB_ReleaseMessageBuffer(CFE_SB_Buffer_t *BufPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_SB_ReleaseMessageBuffer), BufPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_ReleaseMessageBuffer);

    return status;
}
