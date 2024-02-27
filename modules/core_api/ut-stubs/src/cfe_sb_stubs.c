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
 * Auto-Generated stub implementations for functions defined in cfe_sb header
 */

#include "cfe_sb.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_SB_AllocateMessageBuffer(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_CmdTopicIdToMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_CreatePipe(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_DeletePipe(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_GetPipeIdByName(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_GetPipeName(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_GetUserData(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_GetUserDataLength(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_GlobalCmdTopicIdToMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_GlobalTlmTopicIdToMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_IsValidMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_LocalCmdTopicIdToMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_LocalTlmTopicIdToMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_MessageStringGet(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_MessageStringSet(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_ReceiveBuffer(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_SetUserDataLength(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_TimeStampMsg(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_TlmTopicIdToMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_TransmitBuffer(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_SB_TransmitMsg(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_AllocateMessageBuffer()
 * ----------------------------------------------------
 */
CFE_SB_Buffer_t *CFE_SB_AllocateMessageBuffer(size_t MsgSize)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_AllocateMessageBuffer, CFE_SB_Buffer_t *);

    UT_GenStub_AddParam(CFE_SB_AllocateMessageBuffer, size_t, MsgSize);

    UT_GenStub_Execute(CFE_SB_AllocateMessageBuffer, Basic, UT_DefaultHandler_CFE_SB_AllocateMessageBuffer);

    return UT_GenStub_GetReturnValue(CFE_SB_AllocateMessageBuffer, CFE_SB_Buffer_t *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_CmdTopicIdToMsgId()
 * ----------------------------------------------------
 */
CFE_SB_MsgId_Atom_t CFE_SB_CmdTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_CmdTopicIdToMsgId, CFE_SB_MsgId_Atom_t);

    UT_GenStub_AddParam(CFE_SB_CmdTopicIdToMsgId, uint16, TopicId);
    UT_GenStub_AddParam(CFE_SB_CmdTopicIdToMsgId, uint16, InstanceNum);

    UT_GenStub_Execute(CFE_SB_CmdTopicIdToMsgId, Basic, UT_DefaultHandler_CFE_SB_CmdTopicIdToMsgId);

    return UT_GenStub_GetReturnValue(CFE_SB_CmdTopicIdToMsgId, CFE_SB_MsgId_Atom_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_CreatePipe()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_CreatePipe, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_CreatePipe, CFE_SB_PipeId_t *, PipeIdPtr);
    UT_GenStub_AddParam(CFE_SB_CreatePipe, uint16, Depth);
    UT_GenStub_AddParam(CFE_SB_CreatePipe, const char *, PipeName);

    UT_GenStub_Execute(CFE_SB_CreatePipe, Basic, UT_DefaultHandler_CFE_SB_CreatePipe);

    return UT_GenStub_GetReturnValue(CFE_SB_CreatePipe, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_DeletePipe()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_DeletePipe, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_DeletePipe, CFE_SB_PipeId_t, PipeId);

    UT_GenStub_Execute(CFE_SB_DeletePipe, Basic, UT_DefaultHandler_CFE_SB_DeletePipe);

    return UT_GenStub_GetReturnValue(CFE_SB_DeletePipe, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_GetPipeIdByName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_GetPipeIdByName(CFE_SB_PipeId_t *PipeIdPtr, const char *PipeName)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_GetPipeIdByName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_GetPipeIdByName, CFE_SB_PipeId_t *, PipeIdPtr);
    UT_GenStub_AddParam(CFE_SB_GetPipeIdByName, const char *, PipeName);

    UT_GenStub_Execute(CFE_SB_GetPipeIdByName, Basic, UT_DefaultHandler_CFE_SB_GetPipeIdByName);

    return UT_GenStub_GetReturnValue(CFE_SB_GetPipeIdByName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_GetPipeName()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_GetPipeName(char *PipeNameBuf, size_t PipeNameSize, CFE_SB_PipeId_t PipeId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_GetPipeName, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_GetPipeName, char *, PipeNameBuf);
    UT_GenStub_AddParam(CFE_SB_GetPipeName, size_t, PipeNameSize);
    UT_GenStub_AddParam(CFE_SB_GetPipeName, CFE_SB_PipeId_t, PipeId);

    UT_GenStub_Execute(CFE_SB_GetPipeName, Basic, UT_DefaultHandler_CFE_SB_GetPipeName);

    return UT_GenStub_GetReturnValue(CFE_SB_GetPipeName, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_GetPipeOpts()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptsPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_GetPipeOpts, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_GetPipeOpts, CFE_SB_PipeId_t, PipeId);
    UT_GenStub_AddParam(CFE_SB_GetPipeOpts, uint8 *, OptsPtr);

    UT_GenStub_Execute(CFE_SB_GetPipeOpts, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_GetPipeOpts, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_GetUserData()
 * ----------------------------------------------------
 */
void *CFE_SB_GetUserData(CFE_MSG_Message_t *MsgPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_GetUserData, void *);

    UT_GenStub_AddParam(CFE_SB_GetUserData, CFE_MSG_Message_t *, MsgPtr);

    UT_GenStub_Execute(CFE_SB_GetUserData, Basic, UT_DefaultHandler_CFE_SB_GetUserData);

    return UT_GenStub_GetReturnValue(CFE_SB_GetUserData, void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_GetUserDataLength()
 * ----------------------------------------------------
 */
size_t CFE_SB_GetUserDataLength(const CFE_MSG_Message_t *MsgPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_GetUserDataLength, size_t);

    UT_GenStub_AddParam(CFE_SB_GetUserDataLength, const CFE_MSG_Message_t *, MsgPtr);

    UT_GenStub_Execute(CFE_SB_GetUserDataLength, Basic, UT_DefaultHandler_CFE_SB_GetUserDataLength);

    return UT_GenStub_GetReturnValue(CFE_SB_GetUserDataLength, size_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_GlobalCmdTopicIdToMsgId()
 * ----------------------------------------------------
 */
CFE_SB_MsgId_Atom_t CFE_SB_GlobalCmdTopicIdToMsgId(uint16 TopicId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_GlobalCmdTopicIdToMsgId, CFE_SB_MsgId_Atom_t);

    UT_GenStub_AddParam(CFE_SB_GlobalCmdTopicIdToMsgId, uint16, TopicId);

    UT_GenStub_Execute(CFE_SB_GlobalCmdTopicIdToMsgId, Basic, UT_DefaultHandler_CFE_SB_GlobalCmdTopicIdToMsgId);

    return UT_GenStub_GetReturnValue(CFE_SB_GlobalCmdTopicIdToMsgId, CFE_SB_MsgId_Atom_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_GlobalTlmTopicIdToMsgId()
 * ----------------------------------------------------
 */
CFE_SB_MsgId_Atom_t CFE_SB_GlobalTlmTopicIdToMsgId(uint16 TopicId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_GlobalTlmTopicIdToMsgId, CFE_SB_MsgId_Atom_t);

    UT_GenStub_AddParam(CFE_SB_GlobalTlmTopicIdToMsgId, uint16, TopicId);

    UT_GenStub_Execute(CFE_SB_GlobalTlmTopicIdToMsgId, Basic, UT_DefaultHandler_CFE_SB_GlobalTlmTopicIdToMsgId);

    return UT_GenStub_GetReturnValue(CFE_SB_GlobalTlmTopicIdToMsgId, CFE_SB_MsgId_Atom_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_IsValidMsgId()
 * ----------------------------------------------------
 */
bool CFE_SB_IsValidMsgId(CFE_SB_MsgId_t MsgId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_IsValidMsgId, bool);

    UT_GenStub_AddParam(CFE_SB_IsValidMsgId, CFE_SB_MsgId_t, MsgId);

    UT_GenStub_Execute(CFE_SB_IsValidMsgId, Basic, UT_DefaultHandler_CFE_SB_IsValidMsgId);

    return UT_GenStub_GetReturnValue(CFE_SB_IsValidMsgId, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_LocalCmdTopicIdToMsgId()
 * ----------------------------------------------------
 */
CFE_SB_MsgId_Atom_t CFE_SB_LocalCmdTopicIdToMsgId(uint16 TopicId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_LocalCmdTopicIdToMsgId, CFE_SB_MsgId_Atom_t);

    UT_GenStub_AddParam(CFE_SB_LocalCmdTopicIdToMsgId, uint16, TopicId);

    UT_GenStub_Execute(CFE_SB_LocalCmdTopicIdToMsgId, Basic, UT_DefaultHandler_CFE_SB_LocalCmdTopicIdToMsgId);

    return UT_GenStub_GetReturnValue(CFE_SB_LocalCmdTopicIdToMsgId, CFE_SB_MsgId_Atom_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_LocalTlmTopicIdToMsgId()
 * ----------------------------------------------------
 */
CFE_SB_MsgId_Atom_t CFE_SB_LocalTlmTopicIdToMsgId(uint16 TopicId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_LocalTlmTopicIdToMsgId, CFE_SB_MsgId_Atom_t);

    UT_GenStub_AddParam(CFE_SB_LocalTlmTopicIdToMsgId, uint16, TopicId);

    UT_GenStub_Execute(CFE_SB_LocalTlmTopicIdToMsgId, Basic, UT_DefaultHandler_CFE_SB_LocalTlmTopicIdToMsgId);

    return UT_GenStub_GetReturnValue(CFE_SB_LocalTlmTopicIdToMsgId, CFE_SB_MsgId_Atom_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_MessageStringGet()
 * ----------------------------------------------------
 */
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString,
                              size_t DestMaxSize, size_t SourceMaxSize)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_MessageStringGet, int32);

    UT_GenStub_AddParam(CFE_SB_MessageStringGet, char *, DestStringPtr);
    UT_GenStub_AddParam(CFE_SB_MessageStringGet, const char *, SourceStringPtr);
    UT_GenStub_AddParam(CFE_SB_MessageStringGet, const char *, DefaultString);
    UT_GenStub_AddParam(CFE_SB_MessageStringGet, size_t, DestMaxSize);
    UT_GenStub_AddParam(CFE_SB_MessageStringGet, size_t, SourceMaxSize);

    UT_GenStub_Execute(CFE_SB_MessageStringGet, Basic, UT_DefaultHandler_CFE_SB_MessageStringGet);

    return UT_GenStub_GetReturnValue(CFE_SB_MessageStringGet, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_MessageStringSet()
 * ----------------------------------------------------
 */
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, size_t DestMaxSize,
                              size_t SourceMaxSize)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_MessageStringSet, int32);

    UT_GenStub_AddParam(CFE_SB_MessageStringSet, char *, DestStringPtr);
    UT_GenStub_AddParam(CFE_SB_MessageStringSet, const char *, SourceStringPtr);
    UT_GenStub_AddParam(CFE_SB_MessageStringSet, size_t, DestMaxSize);
    UT_GenStub_AddParam(CFE_SB_MessageStringSet, size_t, SourceMaxSize);

    UT_GenStub_Execute(CFE_SB_MessageStringSet, Basic, UT_DefaultHandler_CFE_SB_MessageStringSet);

    return UT_GenStub_GetReturnValue(CFE_SB_MessageStringSet, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_PipeId_ToIndex()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_PipeId_ToIndex(CFE_SB_PipeId_t PipeID, uint32 *Idx)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_PipeId_ToIndex, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_PipeId_ToIndex, CFE_SB_PipeId_t, PipeID);
    UT_GenStub_AddParam(CFE_SB_PipeId_ToIndex, uint32 *, Idx);

    UT_GenStub_Execute(CFE_SB_PipeId_ToIndex, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_PipeId_ToIndex, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_ReceiveBuffer()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_ReceiveBuffer(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_ReceiveBuffer, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_ReceiveBuffer, CFE_SB_Buffer_t **, BufPtr);
    UT_GenStub_AddParam(CFE_SB_ReceiveBuffer, CFE_SB_PipeId_t, PipeId);
    UT_GenStub_AddParam(CFE_SB_ReceiveBuffer, int32, TimeOut);

    UT_GenStub_Execute(CFE_SB_ReceiveBuffer, Basic, UT_DefaultHandler_CFE_SB_ReceiveBuffer);

    return UT_GenStub_GetReturnValue(CFE_SB_ReceiveBuffer, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_ReleaseMessageBuffer()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_ReleaseMessageBuffer(CFE_SB_Buffer_t *BufPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_ReleaseMessageBuffer, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_ReleaseMessageBuffer, CFE_SB_Buffer_t *, BufPtr);

    UT_GenStub_Execute(CFE_SB_ReleaseMessageBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_ReleaseMessageBuffer, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_SetPipeOpts()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_SetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 Opts)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_SetPipeOpts, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_SetPipeOpts, CFE_SB_PipeId_t, PipeId);
    UT_GenStub_AddParam(CFE_SB_SetPipeOpts, uint8, Opts);

    UT_GenStub_Execute(CFE_SB_SetPipeOpts, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_SetPipeOpts, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_SetUserDataLength()
 * ----------------------------------------------------
 */
void CFE_SB_SetUserDataLength(CFE_MSG_Message_t *MsgPtr, size_t DataLength)
{
    UT_GenStub_AddParam(CFE_SB_SetUserDataLength, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_SB_SetUserDataLength, size_t, DataLength);

    UT_GenStub_Execute(CFE_SB_SetUserDataLength, Basic, UT_DefaultHandler_CFE_SB_SetUserDataLength);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_Subscribe()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_Subscribe, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_Subscribe, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_SB_Subscribe, CFE_SB_PipeId_t, PipeId);

    UT_GenStub_Execute(CFE_SB_Subscribe, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_Subscribe, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_SubscribeEx()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_SubscribeEx(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_SubscribeEx, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_SubscribeEx, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_SB_SubscribeEx, CFE_SB_PipeId_t, PipeId);
    UT_GenStub_AddParam(CFE_SB_SubscribeEx, CFE_SB_Qos_t, Quality);
    UT_GenStub_AddParam(CFE_SB_SubscribeEx, uint16, MsgLim);

    UT_GenStub_Execute(CFE_SB_SubscribeEx, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_SubscribeEx, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_SubscribeLocal()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint16 MsgLim)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_SubscribeLocal, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_SubscribeLocal, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_SB_SubscribeLocal, CFE_SB_PipeId_t, PipeId);
    UT_GenStub_AddParam(CFE_SB_SubscribeLocal, uint16, MsgLim);

    UT_GenStub_Execute(CFE_SB_SubscribeLocal, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_SubscribeLocal, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_TimeStampMsg()
 * ----------------------------------------------------
 */
void CFE_SB_TimeStampMsg(CFE_MSG_Message_t *MsgPtr)
{
    UT_GenStub_AddParam(CFE_SB_TimeStampMsg, CFE_MSG_Message_t *, MsgPtr);

    UT_GenStub_Execute(CFE_SB_TimeStampMsg, Basic, UT_DefaultHandler_CFE_SB_TimeStampMsg);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_TlmTopicIdToMsgId()
 * ----------------------------------------------------
 */
CFE_SB_MsgId_Atom_t CFE_SB_TlmTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_TlmTopicIdToMsgId, CFE_SB_MsgId_Atom_t);

    UT_GenStub_AddParam(CFE_SB_TlmTopicIdToMsgId, uint16, TopicId);
    UT_GenStub_AddParam(CFE_SB_TlmTopicIdToMsgId, uint16, InstanceNum);

    UT_GenStub_Execute(CFE_SB_TlmTopicIdToMsgId, Basic, UT_DefaultHandler_CFE_SB_TlmTopicIdToMsgId);

    return UT_GenStub_GetReturnValue(CFE_SB_TlmTopicIdToMsgId, CFE_SB_MsgId_Atom_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_TransmitBuffer()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_TransmitBuffer(CFE_SB_Buffer_t *BufPtr, bool IsOrigination)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_TransmitBuffer, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_TransmitBuffer, CFE_SB_Buffer_t *, BufPtr);
    UT_GenStub_AddParam(CFE_SB_TransmitBuffer, bool, IsOrigination);

    UT_GenStub_Execute(CFE_SB_TransmitBuffer, Basic, UT_DefaultHandler_CFE_SB_TransmitBuffer);

    return UT_GenStub_GetReturnValue(CFE_SB_TransmitBuffer, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_TransmitMsg()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_TransmitMsg(const CFE_MSG_Message_t *MsgPtr, bool IsOrigination)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_TransmitMsg, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_TransmitMsg, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_SB_TransmitMsg, bool, IsOrigination);

    UT_GenStub_Execute(CFE_SB_TransmitMsg, Basic, UT_DefaultHandler_CFE_SB_TransmitMsg);

    return UT_GenStub_GetReturnValue(CFE_SB_TransmitMsg, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_Unsubscribe()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_Unsubscribe, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_Unsubscribe, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_SB_Unsubscribe, CFE_SB_PipeId_t, PipeId);

    UT_GenStub_Execute(CFE_SB_Unsubscribe, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_Unsubscribe, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_SB_UnsubscribeLocal()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    UT_GenStub_SetupReturnBuffer(CFE_SB_UnsubscribeLocal, CFE_Status_t);

    UT_GenStub_AddParam(CFE_SB_UnsubscribeLocal, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_SB_UnsubscribeLocal, CFE_SB_PipeId_t, PipeId);

    UT_GenStub_Execute(CFE_SB_UnsubscribeLocal, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_SB_UnsubscribeLocal, CFE_Status_t);
}
