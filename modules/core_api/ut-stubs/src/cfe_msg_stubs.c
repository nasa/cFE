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
 * Auto-Generated stub implementations for functions defined in cfe_msg header
 */

#include "cfe_msg.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_MSG_GetApId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetEDSVersion(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetEndian(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetFcnCode(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetHasSecondaryHeader(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetHeaderVersion(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetMsgTime(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetNextSequenceCount(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetPlaybackFlag(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetSegmentationFlag(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetSequenceCount(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetSize(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetSubsystem(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetSystem(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetType(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_GetTypeFromMsgId(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_OriginationAction(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_ValidateChecksum(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_MSG_VerificationAction(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GenerateChecksum()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GenerateChecksum(CFE_MSG_Message_t *MsgPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GenerateChecksum, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GenerateChecksum, CFE_MSG_Message_t *, MsgPtr);

    UT_GenStub_Execute(CFE_MSG_GenerateChecksum, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_GenerateChecksum, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetApId()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetApId(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_ApId_t *ApId)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetApId, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetApId, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetApId, CFE_MSG_ApId_t *, ApId);

    UT_GenStub_Execute(CFE_MSG_GetApId, Basic, UT_DefaultHandler_CFE_MSG_GetApId);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetApId, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetEDSVersion()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetEDSVersion(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t *Version)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetEDSVersion, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetEDSVersion, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetEDSVersion, CFE_MSG_EDSVersion_t *, Version);

    UT_GenStub_Execute(CFE_MSG_GetEDSVersion, Basic, UT_DefaultHandler_CFE_MSG_GetEDSVersion);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetEDSVersion, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetEndian()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetEndian(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t *Endian)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetEndian, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetEndian, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetEndian, CFE_MSG_Endian_t *, Endian);

    UT_GenStub_Execute(CFE_MSG_GetEndian, Basic, UT_DefaultHandler_CFE_MSG_GetEndian);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetEndian, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetFcnCode()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetFcnCode(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t *FcnCode)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetFcnCode, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetFcnCode, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetFcnCode, CFE_MSG_FcnCode_t *, FcnCode);

    UT_GenStub_Execute(CFE_MSG_GetFcnCode, Basic, UT_DefaultHandler_CFE_MSG_GetFcnCode);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetFcnCode, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetHasSecondaryHeader()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetHasSecondaryHeader(const CFE_MSG_Message_t *MsgPtr, bool *HasSecondary)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetHasSecondaryHeader, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetHasSecondaryHeader, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetHasSecondaryHeader, bool *, HasSecondary);

    UT_GenStub_Execute(CFE_MSG_GetHasSecondaryHeader, Basic, UT_DefaultHandler_CFE_MSG_GetHasSecondaryHeader);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetHasSecondaryHeader, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetHeaderVersion()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetHeaderVersion(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_HeaderVersion_t *Version)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetHeaderVersion, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetHeaderVersion, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetHeaderVersion, CFE_MSG_HeaderVersion_t *, Version);

    UT_GenStub_Execute(CFE_MSG_GetHeaderVersion, Basic, UT_DefaultHandler_CFE_MSG_GetHeaderVersion);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetHeaderVersion, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetMsgId()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetMsgId(const CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t *MsgId)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetMsgId, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetMsgId, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetMsgId, CFE_SB_MsgId_t *, MsgId);

    UT_GenStub_Execute(CFE_MSG_GetMsgId, Basic, UT_DefaultHandler_CFE_MSG_GetMsgId);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetMsgId, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetMsgTime()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetMsgTime(const CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t *Time)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetMsgTime, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetMsgTime, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetMsgTime, CFE_TIME_SysTime_t *, Time);

    UT_GenStub_Execute(CFE_MSG_GetMsgTime, Basic, UT_DefaultHandler_CFE_MSG_GetMsgTime);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetMsgTime, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetNextSequenceCount()
 * ----------------------------------------------------
 */
CFE_MSG_SequenceCount_t CFE_MSG_GetNextSequenceCount(CFE_MSG_SequenceCount_t SeqCnt)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetNextSequenceCount, CFE_MSG_SequenceCount_t);

    UT_GenStub_AddParam(CFE_MSG_GetNextSequenceCount, CFE_MSG_SequenceCount_t, SeqCnt);

    UT_GenStub_Execute(CFE_MSG_GetNextSequenceCount, Basic, UT_DefaultHandler_CFE_MSG_GetNextSequenceCount);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetNextSequenceCount, CFE_MSG_SequenceCount_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetPlaybackFlag()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetPlaybackFlag(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t *PlayFlag)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetPlaybackFlag, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetPlaybackFlag, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetPlaybackFlag, CFE_MSG_PlaybackFlag_t *, PlayFlag);

    UT_GenStub_Execute(CFE_MSG_GetPlaybackFlag, Basic, UT_DefaultHandler_CFE_MSG_GetPlaybackFlag);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetPlaybackFlag, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetSegmentationFlag()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetSegmentationFlag(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_SegmentationFlag_t *SegFlag)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetSegmentationFlag, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetSegmentationFlag, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetSegmentationFlag, CFE_MSG_SegmentationFlag_t *, SegFlag);

    UT_GenStub_Execute(CFE_MSG_GetSegmentationFlag, Basic, UT_DefaultHandler_CFE_MSG_GetSegmentationFlag);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetSegmentationFlag, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetSequenceCount()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetSequenceCount(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_SequenceCount_t *SeqCnt)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetSequenceCount, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetSequenceCount, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetSequenceCount, CFE_MSG_SequenceCount_t *, SeqCnt);

    UT_GenStub_Execute(CFE_MSG_GetSequenceCount, Basic, UT_DefaultHandler_CFE_MSG_GetSequenceCount);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetSequenceCount, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetSize()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetSize(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t *Size)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetSize, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetSize, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetSize, CFE_MSG_Size_t *, Size);

    UT_GenStub_Execute(CFE_MSG_GetSize, Basic, UT_DefaultHandler_CFE_MSG_GetSize);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetSize, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetSubsystem()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetSubsystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t *Subsystem)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetSubsystem, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetSubsystem, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetSubsystem, CFE_MSG_Subsystem_t *, Subsystem);

    UT_GenStub_Execute(CFE_MSG_GetSubsystem, Basic, UT_DefaultHandler_CFE_MSG_GetSubsystem);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetSubsystem, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetSystem()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetSystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t *System)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetSystem, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetSystem, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetSystem, CFE_MSG_System_t *, System);

    UT_GenStub_Execute(CFE_MSG_GetSystem, Basic, UT_DefaultHandler_CFE_MSG_GetSystem);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetSystem, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetType()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetType(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Type_t *Type)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetType, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetType, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_GetType, CFE_MSG_Type_t *, Type);

    UT_GenStub_Execute(CFE_MSG_GetType, Basic, UT_DefaultHandler_CFE_MSG_GetType);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetType, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_GetTypeFromMsgId()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_GetTypeFromMsgId(CFE_SB_MsgId_t MsgId, CFE_MSG_Type_t *Type)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_GetTypeFromMsgId, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_GetTypeFromMsgId, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_MSG_GetTypeFromMsgId, CFE_MSG_Type_t *, Type);

    UT_GenStub_Execute(CFE_MSG_GetTypeFromMsgId, Basic, UT_DefaultHandler_CFE_MSG_GetTypeFromMsgId);

    return UT_GenStub_GetReturnValue(CFE_MSG_GetTypeFromMsgId, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_Init()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_Init(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId, CFE_MSG_Size_t Size)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_Init, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_Init, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_Init, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_MSG_Init, CFE_MSG_Size_t, Size);

    UT_GenStub_Execute(CFE_MSG_Init, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_Init, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_OriginationAction()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_OriginationAction(CFE_MSG_Message_t *MsgPtr, size_t BufferSize, bool *IsAcceptable)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_OriginationAction, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_OriginationAction, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_OriginationAction, size_t, BufferSize);
    UT_GenStub_AddParam(CFE_MSG_OriginationAction, bool *, IsAcceptable);

    UT_GenStub_Execute(CFE_MSG_OriginationAction, Basic, UT_DefaultHandler_CFE_MSG_OriginationAction);

    return UT_GenStub_GetReturnValue(CFE_MSG_OriginationAction, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetApId()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetApId(CFE_MSG_Message_t *MsgPtr, CFE_MSG_ApId_t ApId)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetApId, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetApId, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetApId, CFE_MSG_ApId_t, ApId);

    UT_GenStub_Execute(CFE_MSG_SetApId, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetApId, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetEDSVersion()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetEDSVersion(CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t Version)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetEDSVersion, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetEDSVersion, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetEDSVersion, CFE_MSG_EDSVersion_t, Version);

    UT_GenStub_Execute(CFE_MSG_SetEDSVersion, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetEDSVersion, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetEndian()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetEndian(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t Endian)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetEndian, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetEndian, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetEndian, CFE_MSG_Endian_t, Endian);

    UT_GenStub_Execute(CFE_MSG_SetEndian, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetEndian, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetFcnCode()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetFcnCode(CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t FcnCode)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetFcnCode, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetFcnCode, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetFcnCode, CFE_MSG_FcnCode_t, FcnCode);

    UT_GenStub_Execute(CFE_MSG_SetFcnCode, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetFcnCode, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetHasSecondaryHeader()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetHasSecondaryHeader(CFE_MSG_Message_t *MsgPtr, bool HasSecondary)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetHasSecondaryHeader, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetHasSecondaryHeader, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetHasSecondaryHeader, bool, HasSecondary);

    UT_GenStub_Execute(CFE_MSG_SetHasSecondaryHeader, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetHasSecondaryHeader, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetHeaderVersion()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetHeaderVersion(CFE_MSG_Message_t *MsgPtr, CFE_MSG_HeaderVersion_t Version)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetHeaderVersion, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetHeaderVersion, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetHeaderVersion, CFE_MSG_HeaderVersion_t, Version);

    UT_GenStub_Execute(CFE_MSG_SetHeaderVersion, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetHeaderVersion, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetMsgId()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetMsgId(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetMsgId, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetMsgId, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetMsgId, CFE_SB_MsgId_t, MsgId);

    UT_GenStub_Execute(CFE_MSG_SetMsgId, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetMsgId, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetMsgTime()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t NewTime)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetMsgTime, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetMsgTime, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetMsgTime, CFE_TIME_SysTime_t, NewTime);

    UT_GenStub_Execute(CFE_MSG_SetMsgTime, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetMsgTime, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetPlaybackFlag()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetPlaybackFlag(CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t PlayFlag)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetPlaybackFlag, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetPlaybackFlag, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetPlaybackFlag, CFE_MSG_PlaybackFlag_t, PlayFlag);

    UT_GenStub_Execute(CFE_MSG_SetPlaybackFlag, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetPlaybackFlag, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetSegmentationFlag()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetSegmentationFlag(CFE_MSG_Message_t *MsgPtr, CFE_MSG_SegmentationFlag_t SegFlag)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetSegmentationFlag, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetSegmentationFlag, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetSegmentationFlag, CFE_MSG_SegmentationFlag_t, SegFlag);

    UT_GenStub_Execute(CFE_MSG_SetSegmentationFlag, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetSegmentationFlag, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetSequenceCount()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetSequenceCount(CFE_MSG_Message_t *MsgPtr, CFE_MSG_SequenceCount_t SeqCnt)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetSequenceCount, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetSequenceCount, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetSequenceCount, CFE_MSG_SequenceCount_t, SeqCnt);

    UT_GenStub_Execute(CFE_MSG_SetSequenceCount, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetSequenceCount, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetSize()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetSize(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t Size)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetSize, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetSize, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetSize, CFE_MSG_Size_t, Size);

    UT_GenStub_Execute(CFE_MSG_SetSize, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetSize, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetSubsystem()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetSubsystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t Subsystem)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetSubsystem, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetSubsystem, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetSubsystem, CFE_MSG_Subsystem_t, Subsystem);

    UT_GenStub_Execute(CFE_MSG_SetSubsystem, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetSubsystem, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetSystem()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetSystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t System)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetSystem, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetSystem, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetSystem, CFE_MSG_System_t, System);

    UT_GenStub_Execute(CFE_MSG_SetSystem, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetSystem, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_SetType()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_SetType(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Type_t Type)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_SetType, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_SetType, CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_SetType, CFE_MSG_Type_t, Type);

    UT_GenStub_Execute(CFE_MSG_SetType, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_MSG_SetType, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_ValidateChecksum()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_ValidateChecksum(const CFE_MSG_Message_t *MsgPtr, bool *IsValid)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_ValidateChecksum, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_ValidateChecksum, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_ValidateChecksum, bool *, IsValid);

    UT_GenStub_Execute(CFE_MSG_ValidateChecksum, Basic, UT_DefaultHandler_CFE_MSG_ValidateChecksum);

    return UT_GenStub_GetReturnValue(CFE_MSG_ValidateChecksum, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_MSG_VerificationAction()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_MSG_VerificationAction(const CFE_MSG_Message_t *MsgPtr, size_t BufferSize, bool *IsAcceptable)
{
    UT_GenStub_SetupReturnBuffer(CFE_MSG_VerificationAction, CFE_Status_t);

    UT_GenStub_AddParam(CFE_MSG_VerificationAction, const CFE_MSG_Message_t *, MsgPtr);
    UT_GenStub_AddParam(CFE_MSG_VerificationAction, size_t, BufferSize);
    UT_GenStub_AddParam(CFE_MSG_VerificationAction, bool *, IsAcceptable);

    UT_GenStub_Execute(CFE_MSG_VerificationAction, Basic, UT_DefaultHandler_CFE_MSG_VerificationAction);

    return UT_GenStub_GetReturnValue(CFE_MSG_VerificationAction, CFE_Status_t);
}
