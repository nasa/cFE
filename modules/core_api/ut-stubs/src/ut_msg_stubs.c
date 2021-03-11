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
** File: ut_msg_stubs.c
**
** Purpose:
** Unit test stubs for MSG routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include "cfe.h"
#include "utstubs.h"
#include "uttools.h"

/*
** Defines
*/

/* For reporting no value for get */
#define UTASSERT_GETSTUB(Expression) \
    UtAssert_Type(TSF, Expression, "%s: Check for get value provided by test", __func__);

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GenerateChecksum
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GenerateChecksum(CFE_MSG_Message_t *MsgPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GenerateChecksum), MsgPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GenerateChecksum);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetApId
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetApId(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_ApId_t *ApId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetApId), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetApId), ApId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetApId);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetApId), (uint8 *)ApId, sizeof(*ApId)) == sizeof(*ApId));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetEDSVersion
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetEDSVersion(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t *Version)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetEDSVersion), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetEDSVersion), Version);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetEDSVersion);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetEDSVersion), (uint8 *)Version, sizeof(*Version)) ==
                         sizeof(*Version));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetEndian
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetEndian(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t *Endian)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetEndian), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetEndian), Endian);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetEndian);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetEndian), (uint8 *)Endian, sizeof(*Endian)) ==
                         sizeof(*Endian));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetFcnCode
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetFcnCode(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t *FcnCode)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetFcnCode), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetFcnCode), FcnCode);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetFcnCode);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetFcnCode), (uint8 *)FcnCode, sizeof(*FcnCode)) ==
                         sizeof(*FcnCode));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetHasSecondaryHeader
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetHasSecondaryHeader(const CFE_MSG_Message_t *MsgPtr, bool *HasSecondary)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetHasSecondaryHeader), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetHasSecondaryHeader), HasSecondary);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetHasSecondaryHeader);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetHasSecondaryHeader), (uint8 *)HasSecondary,
                                             sizeof(*HasSecondary)) == sizeof(*HasSecondary));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetHeaderVersion
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetHeaderVersion(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_HeaderVersion_t *Version)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetHeaderVersion), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetHeaderVersion), Version);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetHeaderVersion);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetHeaderVersion), (uint8 *)Version, sizeof(*Version)) ==
                         sizeof(*Version));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetMsgId
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetMsgId(const CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t *MsgId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetMsgId), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetMsgId), MsgId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetMsgId);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetMsgId), (uint8 *)MsgId, sizeof(*MsgId)) ==
                         sizeof(*MsgId));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetMsgTime
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetMsgTime(const CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t *Time)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetMsgTime), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetMsgTime), Time);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetMsgTime);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetMsgTime), (uint8 *)Time, sizeof(*Time)) ==
                         sizeof(*Time));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetPlaybackFlag
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetPlaybackFlag(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t *PlayFlag)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetPlaybackFlag), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetPlaybackFlag), PlayFlag);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetPlaybackFlag);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetPlaybackFlag), (uint8 *)PlayFlag, sizeof(*PlayFlag)) ==
                         sizeof(*PlayFlag));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetSegmentationFlag
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetSegmentationFlag(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_SegmentationFlag_t *SegFlag)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSegmentationFlag), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSegmentationFlag), SegFlag);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetSegmentationFlag);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSegmentationFlag), (uint8 *)SegFlag, sizeof(*SegFlag)) ==
                         sizeof(*SegFlag));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetSequenceCount
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetSequenceCount(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_SequenceCount_t *SeqCnt)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSequenceCount), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSequenceCount), SeqCnt);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetSequenceCount);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSequenceCount), (uint8 *)SeqCnt, sizeof(*SeqCnt)) ==
                         sizeof(*SeqCnt));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetSize
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetSize(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t *Size)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSize), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSize), Size);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetSize);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSize), (uint8 *)Size, sizeof(*Size)) == sizeof(*Size));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetSubsystem
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetSubsystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t *Subsystem)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSubsystem), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSubsystem), Subsystem);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetSubsystem);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSubsystem), (uint8 *)Subsystem, sizeof(*Subsystem)) ==
                         sizeof(*Subsystem));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetSystem
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetSystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t *System)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSystem), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetSystem), System);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetSystem);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSystem), (uint8 *)System, sizeof(*System)) ==
                         sizeof(*System));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetType
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetType(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Type_t *Type)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetType), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetType), Type);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetType);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetType), (uint8 *)Type, sizeof(*Type)) == sizeof(*Type));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_GetTypeFromMsgId
 * -----------------------------------------------------------
 */
int32 CFE_MSG_GetTypeFromMsgId(CFE_SB_MsgId_t MsgId, CFE_MSG_Type_t *Type)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetTypeFromMsgId), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_GetTypeFromMsgId), Type);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_GetTypeFromMsgId);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetTypeFromMsgId), (uint8 *)Type, sizeof(*Type)) ==
                         sizeof(*Type));
    }

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_Init
 * -----------------------------------------------------------
 */
int32 CFE_MSG_Init(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId, CFE_MSG_Size_t Size)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_Init), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_Init), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_Init), Size);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_Init);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetApId
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetApId(CFE_MSG_Message_t *MsgPtr, CFE_MSG_ApId_t ApId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetApId), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetApId), ApId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetApId);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetEDSVersion
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetEDSVersion(CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t Version)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetEDSVersion), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetEDSVersion), Version);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetEDSVersion);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetEndian
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetEndian(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t Endian)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetEndian), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetEndian), Endian);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetEndian);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetFcnCode
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetFcnCode(CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t FcnCode)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetFcnCode), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetFcnCode), FcnCode);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetFcnCode);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetHasSecondaryHeader
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetHasSecondaryHeader(CFE_MSG_Message_t *MsgPtr, bool HasSecondary)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetHasSecondaryHeader), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetHasSecondaryHeader), HasSecondary);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetHasSecondaryHeader);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetHeaderVersion
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetHeaderVersion(CFE_MSG_Message_t *MsgPtr, CFE_MSG_HeaderVersion_t Version)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetHeaderVersion), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetHeaderVersion), Version);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetHeaderVersion);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetMsgId
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetMsgId(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetMsgId), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetMsgId), MsgId);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetMsgId);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetMsgTime
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t Time)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetMsgTime), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetMsgTime), Time);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetMsgTime);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetPlaybackFlag
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetPlaybackFlag(CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t PlayFlag)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetPlaybackFlag), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetPlaybackFlag), PlayFlag);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetPlaybackFlag);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetSegmentationFlag
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetSegmentationFlag(CFE_MSG_Message_t *MsgPtr, CFE_MSG_SegmentationFlag_t SegFlag)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSegmentationFlag), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSegmentationFlag), SegFlag);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetSegmentationFlag);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetSequenceCount
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetSequenceCount(CFE_MSG_Message_t *MsgPtr, CFE_MSG_SequenceCount_t SeqCnt)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSequenceCount), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSequenceCount), SeqCnt);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetSequenceCount);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetSize
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetSize(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t Size)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSize), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSize), Size);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetSize);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetSubsystem
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetSubsystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t Subsystem)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSubsystem), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSubsystem), Subsystem);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetSubsystem);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetSystem
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetSystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t System)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSystem), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetSystem), System);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetSystem);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_SetType
 * -----------------------------------------------------------
 */
int32 CFE_MSG_SetType(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Type_t Type)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetType), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_SetType), Type);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_SetType);

    return status;
}

/*
 * -----------------------------------------------------------
 * Stub implementation of CFE_MSG_ValidateChecksum
 * -----------------------------------------------------------
 */
int32 CFE_MSG_ValidateChecksum(const CFE_MSG_Message_t *MsgPtr, bool *IsValid)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_ValidateChecksum), MsgPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_MSG_ValidateChecksum), IsValid);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_MSG_ValidateChecksum);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_ValidateChecksum), (uint8 *)IsValid, sizeof(*IsValid)) ==
                         sizeof(*IsValid));
    }

    return status;
}
