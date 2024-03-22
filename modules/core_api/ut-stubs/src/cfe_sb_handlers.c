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
    size_t             UserLength;
    size_t             TotalLength;
    uint16             CommandCode;
    CFE_TIME_SysTime_t TimeStamp;
} CFE_SB_StubMsg_MetaData_t;

static CFE_SB_StubMsg_MetaData_t *CFE_SB_StubMsg_GetMetaData(const CFE_MSG_Message_t *MsgPtr)
{
    void *                    MetaPtr;
    CFE_SB_StubMsg_MetaData_t DefaultMeta;
    size_t                    MetaSize;
    UT_EntryKey_t             MsgKey = (UT_EntryKey_t)MsgPtr;

    UT_GetDataBuffer(MsgKey, &MetaPtr, &MetaSize, NULL);
    if (MetaPtr == NULL || MetaSize != sizeof(DefaultMeta))
    {
        memset(&DefaultMeta, 0, sizeof(DefaultMeta));
        DefaultMeta.MsgId = CFE_SB_INVALID_MSG_ID;
        UT_ResetState(MsgKey);
        UT_SetDataBuffer(MsgKey, &DefaultMeta, sizeof(DefaultMeta), true);

        /* Because "allocate copy" is true above, this gets a pointer to the copy */
        UT_GetDataBuffer(MsgKey, &MetaPtr, &MetaSize, NULL);
    }

    return MetaPtr;
}

/*------------------------------------------------------------
 *
 * Helper function to fabricate a MsgID value for testing purposes
 *
 * Note this is not intended to match a real MsgID value used in flight
 * software builds, it is just for UT.  The fabricated value just needs
 * to pass the "CFE_SB_IsValidMsgId()" test.
 *
 *------------------------------------------------------------*/
static void UT_CFE_SB_FabricateMsgIdValue(UT_EntryKey_t FuncKey, bool IsCmd, uint16 TopicId, uint16 InstanceNum)
{
    CFE_SB_MsgId_Atom_t MsgIdValue;

    /* This makes the bits line up with the "traditional" use of the first 16
     * bits of a CCSDS space packet as a MsgId */
    MsgIdValue = 1;
    if (IsCmd)
    {
        MsgIdValue |= 2;
    }
    MsgIdValue <<= 4;
    MsgIdValue |= InstanceNum & 0xF;
    MsgIdValue <<= 7;
    MsgIdValue |= TopicId & 0x7F;

    UT_Stub_SetReturnValue(FuncKey, MsgIdValue);
}

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_LocalCmdTopicIdToMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_LocalCmdTopicIdToMsgId(void *UserObj, UT_EntryKey_t FuncKey,
                                                     const UT_StubContext_t *Context)
{
    uint16 TopicId;

    if (!UT_Stub_GetInt32StatusCode(Context, NULL))
    {
        TopicId = UT_Hook_GetArgValueByName(Context, "TopicId", uint16);

        UT_CFE_SB_FabricateMsgIdValue(FuncKey, true, TopicId, 1);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_LocalTlmTopicIdToMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_LocalTlmTopicIdToMsgId(void *UserObj, UT_EntryKey_t FuncKey,
                                                     const UT_StubContext_t *Context)
{
    uint16 TopicId;

    if (!UT_Stub_GetInt32StatusCode(Context, NULL))
    {
        TopicId = UT_Hook_GetArgValueByName(Context, "TopicId", uint16);

        UT_CFE_SB_FabricateMsgIdValue(FuncKey, false, TopicId, 1);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_GlobalCmdTopicIdToMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_GlobalCmdTopicIdToMsgId(void *UserObj, UT_EntryKey_t FuncKey,
                                                      const UT_StubContext_t *Context)
{
    uint16 TopicId;

    if (!UT_Stub_GetInt32StatusCode(Context, NULL))
    {
        TopicId = UT_Hook_GetArgValueByName(Context, "TopicId", uint16);

        UT_CFE_SB_FabricateMsgIdValue(FuncKey, true, TopicId, 0);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_GlobalTlmTopicIdToMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_GlobalTlmTopicIdToMsgId(void *UserObj, UT_EntryKey_t FuncKey,
                                                      const UT_StubContext_t *Context)
{
    uint16 TopicId;

    if (!UT_Stub_GetInt32StatusCode(Context, NULL))
    {
        TopicId = UT_Hook_GetArgValueByName(Context, "TopicId", uint16);

        UT_CFE_SB_FabricateMsgIdValue(FuncKey, false, TopicId, 0);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_CmdTopicIdToMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_CmdTopicIdToMsgId(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 TopicId;
    uint16 InstanceNum;

    if (!UT_Stub_GetInt32StatusCode(Context, NULL))
    {
        TopicId     = UT_Hook_GetArgValueByName(Context, "TopicId", uint16);
        InstanceNum = UT_Hook_GetArgValueByName(Context, "InstanceNum", uint16);

        UT_CFE_SB_FabricateMsgIdValue(FuncKey, true, TopicId, InstanceNum);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_TlmTopicIdToMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_TlmTopicIdToMsgId(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    uint16 TopicId;
    uint16 InstanceNum;

    if (!UT_Stub_GetInt32StatusCode(Context, NULL))
    {
        TopicId     = UT_Hook_GetArgValueByName(Context, "TopicId", uint16);
        InstanceNum = UT_Hook_GetArgValueByName(Context, "InstanceNum", uint16);

        UT_CFE_SB_FabricateMsgIdValue(FuncKey, false, TopicId, InstanceNum);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_CreatePipe coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_CreatePipe(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_PipeId_t *PipeIdPtr = UT_Hook_GetArgValueByName(Context, "PipeIdPtr", CFE_SB_PipeId_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_SB_CreatePipe), PipeIdPtr, sizeof(*PipeIdPtr));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_DeletePipe coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_DeletePipe(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_PipeId_t PipeId = UT_Hook_GetArgValueByName(Context, "PipeId", CFE_SB_PipeId_t);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_DeletePipe), &PipeId, sizeof(PipeId));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_GetPipeName coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_GetPipeName(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    char * PipeNameBuf  = UT_Hook_GetArgValueByName(Context, "PipeNameBuf", char *);
    size_t PipeNameSize = UT_Hook_GetArgValueByName(Context, "PipeNameSize", size_t);

    size_t      UserBuffSize;
    size_t      BuffPosition;
    void *      TempBuff;
    const char *NameBuff;
    int32       status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0 && PipeNameSize > 0)
    {
        UT_GetDataBuffer(UT_KEY(CFE_SB_GetPipeName), &TempBuff, &UserBuffSize, &BuffPosition);
        if (TempBuff == NULL || UserBuffSize == 0)
        {
            NameBuff     = "UT";
            UserBuffSize = 2;
        }
        else
        {
            NameBuff = TempBuff;
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
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_TimeStampMsg coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_TimeStampMsg(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Message_t *MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
    UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_TimeStampMsg), &MsgPtr, sizeof(MsgPtr));
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_GetPipeIdByName coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_GetPipeIdByName(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_PipeId_t *PipeIdPtr = UT_Hook_GetArgValueByName(Context, "PipeIdPtr", CFE_SB_PipeId_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_GetPipeIdByName), PipeIdPtr, sizeof(*PipeIdPtr)) == sizeof(*PipeIdPtr))
        {
            status = CFE_SUCCESS;
        }
        else
        {
            status     = CFE_SB_BAD_ARGUMENT;
            *PipeIdPtr = CFE_SB_INVALID_PIPE;
        }
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_ReceiveBuffer coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_ReceiveBuffer(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_Buffer_t **BufPtr = UT_Hook_GetArgValueByName(Context, "BufPtr", CFE_SB_Buffer_t **);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_SB_ReceiveBuffer), BufPtr, sizeof(*BufPtr));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_TransmitMsg coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_TransmitMsg(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Message_t *MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);

    int32 status = CFE_SUCCESS;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_TransmitMsg), &MsgPtr, sizeof(MsgPtr));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_TransmitBuffer coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_TransmitBuffer(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_Buffer_t *BufPtr = UT_Hook_GetArgValueByName(Context, "BufPtr", CFE_SB_Buffer_t *);

    int32 status = CFE_SUCCESS;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_TransmitBuffer), &BufPtr, sizeof(BufPtr));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_MessageStringGet coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_MessageStringGet(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    char *      DestStringPtr   = UT_Hook_GetArgValueByName(Context, "DestStringPtr", char *);
    const char *SourceStringPtr = UT_Hook_GetArgValueByName(Context, "SourceStringPtr", const char *);
    const char *DefaultString   = UT_Hook_GetArgValueByName(Context, "DefaultString", const char *);
    size_t      DestMaxSize     = UT_Hook_GetArgValueByName(Context, "DestMaxSize", size_t);
    size_t      SourceMaxSize   = UT_Hook_GetArgValueByName(Context, "SourceMaxSize", size_t);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

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

    UT_Stub_SetReturnValue(FuncKey, status);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_MessageStringSet coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_MessageStringSet(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    char *      DestStringPtr   = UT_Hook_GetArgValueByName(Context, "DestStringPtr", char *);
    const char *SourceStringPtr = UT_Hook_GetArgValueByName(Context, "SourceStringPtr", const char *);
    size_t      DestMaxSize     = UT_Hook_GetArgValueByName(Context, "DestMaxSize", size_t);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

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

    UT_Stub_SetReturnValue(FuncKey, status);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_GetUserData coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_GetUserData(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Message_t *MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
    uint8 *            BytePtr;
    void *             Result;
    size_t             HdrSize;

    if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_GetUserData), &Result, sizeof(Result)) != sizeof(Result))
    {
        BytePtr = (uint8 *)MsgPtr;
        if ((*BytePtr & 0x10) != 0)
        {
            HdrSize = sizeof(CFE_MSG_CommandHeader_t);
        }
        else
        {
            HdrSize = sizeof(CFE_MSG_TelemetryHeader_t);
        }

        Result = (BytePtr + HdrSize);
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_GetUserDataLength coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_GetUserDataLength(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    const CFE_MSG_Message_t *MsgPtr = UT_Hook_GetArgValueByName(Context, "MsgPtr", const CFE_MSG_Message_t *);
    int32                    status;
    size_t                   Result;

    if (UT_Stub_GetInt32StatusCode(Context, &status))
    {
        Result = status;
    }
    else
    {
        Result = CFE_SB_StubMsg_GetMetaData(MsgPtr)->UserLength;
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_IsValidMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_IsValidMsgId(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32          status;
    bool           return_value;
    CFE_SB_MsgId_t MsgId = UT_Hook_GetArgValueByName(Context, "MsgId", CFE_SB_MsgId_t);

    if (UT_Stub_GetInt32StatusCode(Context, &status))
    {
        return_value = status;
    }
    else
    {
        /* The only invalid value UT's should be using is CFE_SB_INVALID_MSG_ID */
        return_value = !CFE_SB_MsgId_Equal(MsgId, CFE_SB_INVALID_MSG_ID);
    }

    UT_Stub_SetReturnValue(FuncKey, return_value);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_SetUserDataLength coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_SetUserDataLength(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Message_t *MsgPtr     = UT_Hook_GetArgValueByName(Context, "MsgPtr", CFE_MSG_Message_t *);
    size_t             DataLength = UT_Hook_GetArgValueByName(Context, "DataLength", size_t);

    CFE_SB_StubMsg_GetMetaData(MsgPtr)->UserLength = DataLength;
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_SB_AllocateMessageBuffer coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_SB_AllocateMessageBuffer(void *UserObj, UT_EntryKey_t FuncKey,
                                                    const UT_StubContext_t *Context)
{
    int32            status;
    CFE_SB_Buffer_t *SBBufPtr = NULL;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status == CFE_SUCCESS)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_SB_AllocateMessageBuffer), &SBBufPtr, sizeof(SBBufPtr));
    }

    UT_Stub_SetReturnValue(FuncKey, SBBufPtr);
}
