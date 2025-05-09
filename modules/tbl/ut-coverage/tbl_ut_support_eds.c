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
** File:
**    tbl_UT.c
**
** Purpose:
**    Table Services unit test
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/

/*
** Includes
*/
#include "tbl_UT.h"
#include "tbl_ut_helpers.h"
#include "cfe_core_resourceid_basevalues.h"
#include "edslib_datatypedb.h"
#include "cfe_missionlib_api.h"
#include "cfe_mission_eds_parameters.h"

/* This is a generated header that defines the dispatch table */
#include "cfe_tbl_eds_interface.h"

/* EDS dispatching uses a generic function based on a lookup table.
 * This function is a stub so that stub just needs to know which entry to use. */

/* NOTE: Automatic formatting of this table tends to make it harder to read. */
/* clang-format off */

#define TBL_UT_EDS_DISPATCH(intf, cmd)                     \
    .Method      = UT_TaskPipeDispatchMethod_TABLE_OFFSET, \
    .TableOffset = offsetof(EdsDispatchTable_EdsComponent_CFE_TBL_Application_CFE_SB_Telecommand_t, intf.cmd)

const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_MSG_HK =
    { TBL_UT_EDS_DISPATCH(SEND_HK, indication), UT_TPD_SETSIZE(CFE_TBL_SendHkCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_NOOP_CC =
    { TBL_UT_EDS_DISPATCH(CMD, NoopCmd_indication), UT_TPD_SETSIZE(CFE_TBL_NoopCmd), UT_TPD_SETCC(CFE_TBL_NOOP_CC) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_LENGTH =    
    { TBL_UT_EDS_DISPATCH(CMD, NoopCmd_indication), UT_TPD_SETERR(CFE_STATUS_WRONG_MSG_LENGTH), UT_TPD_SETCC(CFE_TBL_NOOP_CC) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_RESET_COUNTERS_CC =
    { TBL_UT_EDS_DISPATCH(CMD, ResetCountersCmd_indication), UT_TPD_SETSIZE(CFE_TBL_ResetCountersCmd), UT_TPD_SETCC(CFE_TBL_RESET_COUNTERS_CC) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_INVALID_MID =
    { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TBL_CMD_INVALID_CC =
    { UT_TPD_SETCC(-1), UT_TPD_SETERR(CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */

/* Holding place for the data type info that the EdsLib stubs will return for the current test */
static char                         UT_TBL_StubIntfNameStash[64];
static EdsLib_DataTypeDB_TypeInfo_t UT_TBL_StubTypeInfo;

/* this is _not_ in the range of typical Interface IDs so it should not alias -
 * main objective is to make it non-zerzero, value does not matter */
static const uint16_t UT_TBL_StubIntfId = 0x123;

/* this is _not_ in the range of typical EDS IDs so it should not alias -
 * main objective is to make it non-zerzero, value does not matter */
#define UT_TBL_STUB_FORMATIDX 0x1234
static const EdsLib_Id_t UT_TBL_StubEdsId = EDSLIB_MAKE_ID(EDS_INDEX(CFE_TBL), UT_TBL_STUB_FORMATIDX);

static void UT_TBL_SetEdsLibUnpackData(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    EdsLib_Id_t *EdsId      = UT_Hook_GetArgValueByName(Context, "EdsId", EdsLib_Id_t *);
    void *       DestBuffer = UT_Hook_GetArgValueByName(Context, "DestBuffer", void *);
    uint32       DestSize   = UT_Hook_GetArgValueByName(Context, "MaxNativeByteSize", uint32);
    int32        Status;

    UT_Stub_GetInt32StatusCode(Context, &Status);

    if (Status == EDSLIB_SUCCESS)
    {
        if (UT_Stub_CopyToLocal(FuncKey ^ *EdsId, DestBuffer, DestSize) == 0)
        {
            memset(DestBuffer, 0, DestSize);
        }
    }
}

static void UT_TBL_SetEdsLibTypeInfo(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    const EdsLib_DataTypeDB_TypeInfo_t *RefInfo = UserObj;
    EdsLib_Id_t                         EdsId   = UT_Hook_GetArgValueByName(Context, "EdsId", EdsLib_Id_t);
    EdsLib_DataTypeDB_TypeInfo_t *      TypeInfo =
        UT_Hook_GetArgValueByName(Context, "TypeInfo", EdsLib_DataTypeDB_TypeInfo_t *);
    int32 Status;

    if (RefInfo != NULL)
    {
        /* The arg provides the desired values - always use it */
        memcpy(TypeInfo, RefInfo, sizeof(*TypeInfo));
    }
    else
    {
        /* Fake something out - most test cases use the "UT_Table1_t" structure */
        memset(TypeInfo, 0, sizeof(*TypeInfo));

        UT_Stub_GetInt32StatusCode(Context, &Status);

        /* Fake out a certain subset of known items that are frequent subjects in UT */
        /* This is just intended to simplify the test case setup for these items */
        if (Status == EDSLIB_SUCCESS)
        {
            if (EdsId == UT_TBL_StubEdsId)
            {
                /* If its is one of the UT tables, almost all tests use the UT_Table1_t struct */
                if (strncmp(UT_TBL_StubIntfNameStash, "UT.ut", 5) == 0 ||
                    strncmp(UT_TBL_StubIntfNameStash, "ut_cfe_tbl.", 11) == 0)
                {
                    TypeInfo->Size.Bytes = sizeof(UT_Table1_t);
                }
            }
            else if (EdsLib_Get_AppIdx(EdsId) == EDS_INDEX(CFE_TBL))
            {
                /* Some other "well known" EDS identifiers */
                switch (EdsLib_Get_FormatIdx(EdsId))
                {
                    case EdsContainer_CFE_TBL_File_Hdr_DATADICTIONARY:
                        TypeInfo->Size.Bytes = sizeof(CFE_TBL_File_Hdr_t);
                        break;

                    /* This is a fake UT-only EdsId referring to whatever is in the stub buffer */
                    case UT_TBL_STUB_FORMATIDX:
                        *TypeInfo = UT_TBL_StubTypeInfo;
                        break;
                }
            }

            /* Make the other fields look reasonable */
            if (TypeInfo->Size.Bytes > 0)
            {
                TypeInfo->Size.Bits = TypeInfo->Size.Bytes * 8;
            }
            if (TypeInfo->ElemType == 0)
            {
                TypeInfo->ElemType = EDSLIB_BASICTYPE_CONTAINER;
            }
        }
    }
}

void UT_TBL_SetupHeader(CFE_TBL_File_Hdr_t *TblFileHeader, size_t Offset, size_t NumBytes, const char *Name)
{
    EdsLib_Id_t EdsId = EDSLIB_MAKE_ID(EDS_INDEX(CFE_TBL), EdsContainer_CFE_TBL_File_Hdr_DATADICTIONARY);
    EdsPackedBuffer_CFE_TBL_File_Hdr_t LocalBuffer;

    UT_TBL_SetName(TblFileHeader->TableName, sizeof(TblFileHeader->TableName), Name);
    TblFileHeader->Offset   = Offset;
    TblFileHeader->NumBytes = NumBytes;

    /*
     * In this mode the binary buffer for the OS_read() call just has to exist, it is not used.
     * The data the that the subject unit will use/interpret comes from calling
     * EdsLib_DataTypeDB_UnpackCompleteObject().  Filling the mem with a pattern
     * here should be identifiable, in the event that something does (inappropriately)
     * try to use it directly without decoding it first.
     */
    UtMemFill(&LocalBuffer, sizeof(LocalBuffer));
    UT_SetReadBuffer(&LocalBuffer, sizeof(LocalBuffer));

    /* The subject code should call EdsLib_DataTypeDB_UnpackCompleteObject, at which point it will get
     * the data that the test case set up for the table header */
    UT_SetDataBuffer(UT_KEY(EdsLib_DataTypeDB_UnpackCompleteObject) ^ EdsId, TblFileHeader, sizeof(*TblFileHeader),
                     true);
    UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_UnpackCompleteObject), UT_TBL_SetEdsLibUnpackData, NULL);
    UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetTypeInfo), UT_TBL_SetEdsLibTypeInfo, NULL);
}

static void UT_TBL_FindIntfNameHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    /*
     * Handler for API:
     * int32_t CFE_MissionLib_FindInterfaceByName(const CFE_MissionLib_SoftwareBus_Interface_t *Intf,
     *        const char *IntfName, uint16_t *InterfaceIdBuffer);
     */
    const char *IntfName          = UT_Hook_GetArgValueByName(Context, "IntfName", const char *);
    uint16_t *  InterfaceIdBuffer = UT_Hook_GetArgValueByName(Context, "InterfaceIdBuffer", uint16_t *);

    if (IntfName != NULL)
    {
        /* Stash away the intf name, it may be relevant later */
        strncpy(UT_TBL_StubIntfNameStash, IntfName, sizeof(UT_TBL_StubIntfNameStash) - 1);
        UT_TBL_StubIntfNameStash[sizeof(UT_TBL_StubIntfNameStash) - 1] = 0;
    }

    if (InterfaceIdBuffer != NULL)
    {
        /* this is _not_ in the range of typical Interface IDs so it should not alias -
         * main objective is to make it non-zerzero, value does not matter */
        *InterfaceIdBuffer = UT_TBL_StubIntfId;
    }
}

static void UT_TBL_GetArgumentTypeHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    /*
     * Handler for API:
     * int32_t CFE_MissionLib_GetArgumentType(const CFE_MissionLib_SoftwareBus_Interface_t *Intf, uint16_t
     * InterfaceType, uint16_t TopicId, uint16_t IndicationId, uint16_t ArgumentId, EdsLib_Id_t *Id);
     */
    EdsLib_Id_t *EdsId = UT_Hook_GetArgValueByName(Context, "Id", EdsLib_Id_t *);

    if (EdsId != NULL)
    {
        /* this is _not_ in the range of typical EDS IDs so it should not alias -
         * main objective is to make it non-zerzero, value does not matter */
        *EdsId = UT_TBL_StubEdsId;
    }
}

void UT_TBL_SetupCodec(size_t ByteSize)
{
    UT_ResetState(UT_KEY(EdsLib_DataTypeDB_GetTypeInfo));
    UT_ResetState(UT_KEY(CFE_MissionLib_FindInterfaceByName));
    UT_ResetState(UT_KEY(CFE_MissionLib_GetArgumentType));
    UT_TBL_StubIntfNameStash[0] = 0;

    if (ByteSize != 0)
    {
        memset(&UT_TBL_StubTypeInfo, 0, sizeof(UT_TBL_StubTypeInfo));
        UT_TBL_StubTypeInfo.ElemType   = EDSLIB_BASICTYPE_CONTAINER;
        UT_TBL_StubTypeInfo.Size.Bytes = ByteSize;

        UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetTypeInfo), UT_TBL_SetEdsLibTypeInfo, &UT_TBL_StubTypeInfo);
    }
    else
    {
        UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetTypeInfo), UT_TBL_SetEdsLibTypeInfo, NULL);
    }

    UT_SetHandlerFunction(UT_KEY(CFE_MissionLib_FindInterfaceByName), UT_TBL_FindIntfNameHandler, NULL);
    UT_SetHandlerFunction(UT_KEY(CFE_MissionLib_GetArgumentType), UT_TBL_GetArgumentTypeHandler, NULL);
}
