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

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetApId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetApId(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_ApId_t *ApId = UT_Hook_GetArgValueByName(Context, "ApId", CFE_MSG_ApId_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetApId), ApId, sizeof(*ApId)) == sizeof(*ApId));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetEDSVersion coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetEDSVersion(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_EDSVersion_t *Version = UT_Hook_GetArgValueByName(Context, "Version", CFE_MSG_EDSVersion_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetEDSVersion), Version, sizeof(*Version)) ==
                         sizeof(*Version));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetEndian coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetEndian(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Endian_t *Endian = UT_Hook_GetArgValueByName(Context, "Endian", CFE_MSG_Endian_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetEndian), Endian, sizeof(*Endian)) == sizeof(*Endian));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetFcnCode coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetFcnCode(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_FcnCode_t *FcnCode = UT_Hook_GetArgValueByName(Context, "FcnCode", CFE_MSG_FcnCode_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetFcnCode), FcnCode, sizeof(*FcnCode)) ==
                         sizeof(*FcnCode));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetHasSecondaryHeader coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetHasSecondaryHeader(void *UserObj, UT_EntryKey_t FuncKey,
                                                     const UT_StubContext_t *Context)
{
    bool *HasSecondary = UT_Hook_GetArgValueByName(Context, "HasSecondary", bool *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetHasSecondaryHeader), HasSecondary,
                                             sizeof(*HasSecondary)) == sizeof(*HasSecondary));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetHeaderVersion coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetHeaderVersion(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_HeaderVersion_t *Version = UT_Hook_GetArgValueByName(Context, "Version", CFE_MSG_HeaderVersion_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetHeaderVersion), Version, sizeof(*Version)) ==
                         sizeof(*Version));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetMsgId(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_SB_MsgId_t *MsgId = UT_Hook_GetArgValueByName(Context, "MsgId", CFE_SB_MsgId_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetMsgId), MsgId, sizeof(*MsgId)) == sizeof(*MsgId));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetMsgTime coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetMsgTime(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_TIME_SysTime_t *Time = UT_Hook_GetArgValueByName(Context, "Time", CFE_TIME_SysTime_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetMsgTime), Time, sizeof(*Time)) == sizeof(*Time));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetPlaybackFlag coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetPlaybackFlag(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_PlaybackFlag_t *PlayFlag = UT_Hook_GetArgValueByName(Context, "PlayFlag", CFE_MSG_PlaybackFlag_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetPlaybackFlag), PlayFlag, sizeof(*PlayFlag)) ==
                         sizeof(*PlayFlag));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetSegmentationFlag coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetSegmentationFlag(void *UserObj, UT_EntryKey_t FuncKey,
                                                   const UT_StubContext_t *Context)
{
    CFE_MSG_SegmentationFlag_t *SegFlag = UT_Hook_GetArgValueByName(Context, "SegFlag", CFE_MSG_SegmentationFlag_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSegmentationFlag), SegFlag, sizeof(*SegFlag)) ==
                         sizeof(*SegFlag));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetSequenceCount coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetSequenceCount(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_SequenceCount_t *SeqCnt = UT_Hook_GetArgValueByName(Context, "SeqCnt", CFE_MSG_SequenceCount_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSequenceCount), SeqCnt, sizeof(*SeqCnt)) ==
                         sizeof(*SeqCnt));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetSize coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetSize(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Size_t *Size = UT_Hook_GetArgValueByName(Context, "Size", CFE_MSG_Size_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSize), Size, sizeof(*Size)) == sizeof(*Size));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetSubsystem coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetSubsystem(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Subsystem_t *Subsystem = UT_Hook_GetArgValueByName(Context, "Subsystem", CFE_MSG_Subsystem_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSubsystem), Subsystem, sizeof(*Subsystem)) ==
                         sizeof(*Subsystem));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetSystem coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetSystem(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_System_t *System = UT_Hook_GetArgValueByName(Context, "System", CFE_MSG_System_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetSystem), System, sizeof(*System)) == sizeof(*System));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetType coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetType(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Type_t *Type = UT_Hook_GetArgValueByName(Context, "Type", CFE_MSG_Type_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetType), Type, sizeof(*Type)) == sizeof(*Type));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetTypeFromMsgId coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetTypeFromMsgId(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_MSG_Type_t *Type = UT_Hook_GetArgValueByName(Context, "Type", CFE_MSG_Type_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_GetTypeFromMsgId), Type, sizeof(*Type)) == sizeof(*Type));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_ValidateChecksum coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_ValidateChecksum(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *IsValid = UT_Hook_GetArgValueByName(Context, "IsValid", bool *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UTASSERT_GETSTUB(UT_Stub_CopyToLocal(UT_KEY(CFE_MSG_ValidateChecksum), IsValid, sizeof(*IsValid)) ==
                         sizeof(*IsValid));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_GetNextSequenceCount coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_GetNextSequenceCount(void *UserObj, UT_EntryKey_t FuncKey,
                                                    const UT_StubContext_t *Context)
{
    int32                   status;
    CFE_MSG_SequenceCount_t return_value;

    UT_Stub_GetInt32StatusCode(Context, &status);

    return_value = status;

    UT_Stub_SetReturnValue(FuncKey, return_value);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_OriginationAction coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_OriginationAction(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *IsAcceptable = UT_Hook_GetArgValueByName(Context, "IsAcceptable", bool *);

    /* by default just always return true -- a UT case that needs something else can override this handler */
    if (IsAcceptable != NULL)
    {
        *IsAcceptable = true;
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_MSG_VerificationAction coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_MSG_VerificationAction(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    bool *IsAcceptable = UT_Hook_GetArgValueByName(Context, "IsAcceptable", bool *);

    /* by default just always return true -- a UT case that needs something else can override this handler */
    if (IsAcceptable != NULL)
    {
        *IsAcceptable = true;
    }
}
