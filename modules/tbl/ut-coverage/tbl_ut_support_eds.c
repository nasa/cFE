/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
#include "edslib_intfdb.h"
#include "edslib_global.h"
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
static char                                UT_TBL_StubIntfNameStash[64];
static EdsLib_DataTypeDB_DerivedTypeInfo_t UT_TBL_StubDerivInfo;

/* this is _not_ in the range of typical EDS IDs so it should not alias -
 * main objective is to make it non-zerzero, value does not matter */
#define UT_TBL_STUB_FORMATIDX 0x1234
static const EdsLib_Id_t UT_TBL_StubEdsId = EDSLIB_MAKE_ID(EDS_INDEX(CFE_TBL), UT_TBL_STUB_FORMATIDX);

typedef struct
{
    const char *ParamName;
    void *      ContentPtr;
    size_t      ContentSize;
    int32       Retval;

} UT_TBL_GenericOutput_t;

static void UT_TBL_AltHandler_GenericOutput(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    UT_TBL_GenericOutput_t *Cb = UserObj;
    void *                  Dest;

    Dest = UT_Hook_GetArgValueByName(Context, Cb->ParamName, void *);
    if (Cb->ContentSize != 0 && Cb->ContentPtr != NULL && Dest != NULL)
    {
        memcpy(Dest, Cb->ContentPtr, Cb->ContentSize);
    }

    UT_Stub_SetReturnValue(FuncKey, Cb->Retval);
}

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
    const EdsLib_DataTypeDB_DerivedTypeInfo_t *RefInfo = UserObj;
    EdsLib_Id_t                                EdsId   = UT_Hook_GetArgValueByName(Context, "EdsId", EdsLib_Id_t);
    EdsLib_DataTypeDB_DerivedTypeInfo_t *      DerivInfo =
        UT_Hook_GetArgValueByName(Context, "DerivInfo", EdsLib_DataTypeDB_DerivedTypeInfo_t *);
    int32 Status;

    if (RefInfo != NULL)
    {
        /* The arg provides the desired values - always use it */
        memcpy(DerivInfo, RefInfo, sizeof(*DerivInfo));
    }
    else
    {
        /* Fake something out - most test cases use the "UT_Table1_t" structure */
        memset(DerivInfo, 0, sizeof(*DerivInfo));

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
                    DerivInfo->MaxSize.Bytes = sizeof(UT_Table1_t);
                }
            }
            else if (EdsLib_Get_AppIdx(EdsId) == EDS_INDEX(CFE_TBL))
            {
                /* Some other "well known" EDS identifiers */
                switch (EdsLib_Get_FormatIdx(EdsId))
                {
                    case EdsContainer_CFE_TBL_File_Hdr_DATADICTIONARY:
                        DerivInfo->MaxSize.Bytes = sizeof(CFE_TBL_File_Hdr_t);
                        break;

                    /* This is a fake UT-only EdsId referring to whatever is in the stub buffer */
                    case UT_TBL_STUB_FORMATIDX:
                        *DerivInfo = UT_TBL_StubDerivInfo;
                        break;
                }
            }

            /* Make the other fields look reasonable */
            if (DerivInfo->MaxSize.Bytes > 0)
            {
                DerivInfo->MaxSize.Bits = DerivInfo->MaxSize.Bytes * 8;
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

    memset(&UT_TBL_StubDerivInfo, 0, sizeof(UT_TBL_StubDerivInfo));
    UT_TBL_StubDerivInfo.MaxSize.Bits  = 8 * sizeof(UT_Table1_t);
    UT_TBL_StubDerivInfo.MaxSize.Bytes = sizeof(UT_Table1_t);

    UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetDerivedInfo), UT_TBL_SetEdsLibTypeInfo, &UT_TBL_StubDerivInfo);
}

static void UT_TBL_FindIntfNameHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    /*
     * Handler for API:
     * int32_t EdsLib_IntfDB_FindComponentInterfaceByLocalName(const EdsLib_DatabaseObject_t *GD, EdsLib_Id_t
     * ComponentId, const char *IntfName, EdsLib_Id_t *IdBuffer)
     */
    const char * IntfName = UT_Hook_GetArgValueByName(Context, "IntfName", const char *);
    EdsLib_Id_t *IdBuffer = UT_Hook_GetArgValueByName(Context, "IdBuffer", EdsLib_Id_t *);

    if (IntfName != NULL)
    {
        /* Stash away the intf name, it may be relevant later */
        strncpy(UT_TBL_StubIntfNameStash, IntfName, sizeof(UT_TBL_StubIntfNameStash) - 1);
        UT_TBL_StubIntfNameStash[sizeof(UT_TBL_StubIntfNameStash) - 1] = 0;
    }

    if (IdBuffer != NULL)
    {
        /* this is _not_ in the range of typical Interface IDs so it should not alias -
         * main objective is to make it non-zerzero, value does not matter */
        *IdBuffer = UT_TBL_StubEdsId;
    }
}

static void UT_TBL_GetArgumentTypeHandler(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    /*
     * Handler for API:
     * int32_t EdsLib_IntfDB_FindAllArgumentTypes(const EdsLib_DatabaseObject_t *GD, EdsLib_Id_t CmdEdsId,
     *                    EdsLib_Id_t CompIntfEdsId, EdsLib_Id_t *IdBuffer, size_t NumIdBufs)
     */
    EdsLib_Id_t *IdBuffer = UT_Hook_GetArgValueByName(Context, "IdBuffer", EdsLib_Id_t *);

    if (IdBuffer != NULL)
    {
        /* this is _not_ in the range of typical EDS IDs so it should not alias -
         * main objective is to make it non-zerzero, value does not matter */
        *IdBuffer = UT_TBL_StubEdsId;
    }
}

void UT_TBL_SetupCodec(size_t ByteSize)
{
    UT_ResetState(UT_KEY(EdsLib_DataTypeDB_GetDerivedInfo));
    UT_ResetState(UT_KEY(EdsLib_IntfDB_FindComponentInterfaceByLocalName));
    UT_ResetState(UT_KEY(EdsLib_IntfDB_FindAllArgumentTypes));
    UT_TBL_StubIntfNameStash[0] = 0;

    if (ByteSize != 0)
    {
        memset(&UT_TBL_StubDerivInfo, 0, sizeof(UT_TBL_StubDerivInfo));
        UT_TBL_StubDerivInfo.MaxSize.Bytes = ByteSize;

        UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetDerivedInfo), UT_TBL_SetEdsLibTypeInfo,
                              &UT_TBL_StubDerivInfo);
    }
    else
    {
        UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetDerivedInfo), UT_TBL_SetEdsLibTypeInfo, NULL);
    }

    UT_SetHandlerFunction(UT_KEY(EdsLib_IntfDB_FindComponentInterfaceByLocalName), UT_TBL_FindIntfNameHandler, NULL);
    UT_SetHandlerFunction(UT_KEY(EdsLib_IntfDB_FindAllArgumentTypes), UT_TBL_GetArgumentTypeHandler, NULL);
}

void UT_TBL_ValidateCodecConfig_Test(void)
{
    /* Test Case for:
     * CFE_Status_t CFE_TBL_ValidateCodecConfig(CFE_TBL_TableConfig_t *ReqCfg);
     */

    CFE_TBL_TableConfig_t               ReqCfg;
    UT_TBL_GenericOutput_t              Cb;
    EdsLib_DataTypeDB_DerivedTypeInfo_t DerivInfo;

    memset(&ReqCfg, 0, sizeof(ReqCfg));
    memset(&Cb, 0, sizeof(Cb));
    memset(&DerivInfo, 0, sizeof(DerivInfo));
    memset(ReqCfg.Name, 'x', sizeof(ReqCfg.Name) - 3);
    ReqCfg.Name[sizeof(ReqCfg.Name) - 3] = '.';
    ReqCfg.Name[sizeof(ReqCfg.Name) - 2] = 'u';
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_SUCCESS);

    strcpy(ReqCfg.Name, "2222");
    UT_SetDeferredRetcode(UT_KEY(EdsLib_IntfDB_FindComponentInterfaceByLocalName), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_SUCCESS);

    strcpy(ReqCfg.Name, "UT0");
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_FindPackageIdxByName), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_IntfDB_FindComponentByLocalName), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_SUCCESS);

    UT_SetDefaultReturnValue(UT_KEY(EdsLib_IntfDB_FindComponentInterfaceByLocalName), -1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_IntfDB_FindComponentInterfaceByLocalName), 2, 0);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_SUCCESS);

    UT_ResetState(UT_KEY(EdsLib_IntfDB_FindComponentInterfaceByLocalName));
    Cb.ContentPtr           = &DerivInfo;
    Cb.ContentSize          = sizeof(DerivInfo);
    Cb.ParamName            = "DerivInfo";
    DerivInfo.MaxSize.Bytes = 1000;
    ReqCfg.Size             = 100;
    UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetDerivedInfo), UT_TBL_AltHandler_GenericOutput, &Cb);
    UT_SetHandlerFunction(UT_KEY(EdsLib_IntfDB_FindAllArgumentTypes), UT_TBL_GetArgumentTypeHandler, NULL);

    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_TBL_ERR_INVALID_SIZE);

    ReqCfg.Size = 1000;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_SUCCESS);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_IntfDB_FindAllArgumentTypes), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    Cb.Retval = -1;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecConfig(&ReqCfg), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
}

void UT_TBL_ValidateCodecLoadSize_Test(void)
{
    /* Test Case for:
     * CFE_Status_t CFE_TBL_ValidateCodecLoadSize(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr);
     */

    CFE_TBL_TxnState_t    Txn;
    CFE_TBL_File_Hdr_t    Header;
    CFE_TBL_RegistryRec_t RegRec;

    memset(&Txn, 0, sizeof(Txn));
    memset(&Header, 0, sizeof(Header));
    memset(&RegRec, 0, sizeof(RegRec));

    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecLoadSize(&Txn, &Header), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_ZERO(Txn.NumPendingEvents);

    Txn.RegRecPtr = &RegRec;
    UtAssert_INT32_EQ(CFE_TBL_ValidateCodecLoadSize(&Txn, &Header), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);
}

void UT_TBL_CodecGetFinalStatus_Test(void)
{
    /* Test Case for:
     * CFE_Status_t CFE_TBL_CodecGetFinalStatus(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr);
     */

    CFE_TBL_TxnState_t                  Txn;
    CFE_TBL_File_Hdr_t                  Header;
    CFE_TBL_RegistryRec_t               RegRec;
    UT_TBL_GenericOutput_t              Cb;
    EdsLib_DataTypeDB_DerivedTypeInfo_t DerivInfo;

    memset(&Txn, 0, sizeof(Txn));
    memset(&Header, 0, sizeof(Header));
    memset(&RegRec, 0, sizeof(RegRec));
    memset(&Cb, 0, sizeof(Cb));
    memset(&DerivInfo, 0, sizeof(DerivInfo));

    UtAssert_INT32_EQ(CFE_TBL_CodecGetFinalStatus(&Txn, &Header), CFE_TBL_ERR_INVALID_HANDLE);
    UtAssert_ZERO(Txn.NumPendingEvents);

    Txn.RegRecPtr = &RegRec;
    UtAssert_INT32_EQ(CFE_TBL_CodecGetFinalStatus(&Txn, &Header), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);

    RegRec.Config.EdsId = EDSLIB_MAKE_ID(1, 1);
    Cb.ContentPtr       = &DerivInfo;
    Cb.ContentSize      = sizeof(DerivInfo);
    Cb.ParamName        = "DerivInfo";
    UT_SetHandlerFunction(UT_KEY(EdsLib_DataTypeDB_GetDerivedInfo), UT_TBL_AltHandler_GenericOutput, &Cb);

    UtAssert_INT32_EQ(CFE_TBL_CodecGetFinalStatus(&Txn, &Header), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);

    Header.NumBytes        = 100;
    DerivInfo.MaxSize.Bits = 1000;
    UtAssert_INT32_EQ(CFE_TBL_CodecGetFinalStatus(&Txn, &Header), CFE_TBL_WARN_SHORT_FILE);
    UtAssert_ZERO(Txn.NumPendingEvents);

    Header.Offset   = 25;
    Header.NumBytes = 100;
    UtAssert_INT32_EQ(CFE_TBL_CodecGetFinalStatus(&Txn, &Header), CFE_TBL_WARN_PARTIAL_LOAD);
    UtAssert_ZERO(Txn.NumPendingEvents);

    Cb.Retval = -1;
    UtAssert_INT32_EQ(CFE_TBL_CodecGetFinalStatus(&Txn, &Header), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_UINT32_EQ(Txn.NumPendingEvents, 1);
    UtAssert_UINT16_EQ(Txn.PendingEvents[0].EventId, CFE_TBL_CODEC_ERROR_ERR_EID);
}

void UT_TBL_DecodeHeadersFromFile_Test(void)
{
    /* Test Case for:
     * CFE_Status_t CFE_TBL_DecodeHeadersFromFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor, CFE_TBL_File_Hdr_t
     * *HeaderPtr);
     */
    CFE_TBL_TxnState_t Txn;
    osal_id_t          Fd;
    CFE_TBL_File_Hdr_t Header;

    memset(&Txn, 0, sizeof(Txn));
    memset(&Header, 0, sizeof(Header));
    Fd = OS_ObjectIdFromInteger(1);

    UtAssert_INT32_EQ(CFE_TBL_DecodeHeadersFromFile(&Txn, Fd, &Header), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_DataTypeDB_UnpackCompleteObject), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_DecodeHeadersFromFile(&Txn, Fd, &Header), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_UINT32_EQ(Txn.NumPendingEvents, 1);
    UtAssert_UINT16_EQ(Txn.PendingEvents[0].EventId, CFE_TBL_CODEC_ERROR_ERR_EID);
}

void UT_TBL_EncodeHeadersToFile_Test(void)
{
    /* Test Case for:
     * CFE_Status_t CFE_TBL_EncodeHeadersToFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor, const
     * CFE_TBL_File_Hdr_t *HeaderPtr);
     */
    CFE_TBL_TxnState_t Txn;
    osal_id_t          Fd;
    CFE_TBL_File_Hdr_t Header;

    memset(&Txn, 0, sizeof(Txn));
    memset(&Header, 0, sizeof(Header));
    Fd = OS_ObjectIdFromInteger(1);

    UtAssert_INT32_EQ(CFE_TBL_EncodeHeadersToFile(&Txn, Fd, &Header), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_DataTypeDB_PackPartialObject), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_EncodeHeadersToFile(&Txn, Fd, &Header), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
    UtAssert_UINT32_EQ(Txn.NumPendingEvents, 1);
    UtAssert_UINT16_EQ(Txn.PendingEvents[0].EventId, CFE_TBL_CODEC_ERROR_ERR_EID);
}

void UT_TBL_EncodeOutputData_Test(void)
{
    /* Test Case for:
     * CFE_Status_t CFE_TBL_EncodeOutputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
     * CFE_TBL_LoadBuff_t *DestBuffer);
     */

    CFE_TBL_TxnState_t    Txn;
    CFE_TBL_LoadBuff_t    SourceBuffer;
    CFE_TBL_LoadBuff_t    DestBuffer;
    CFE_TBL_RegistryRec_t RegRec;

    memset(&Txn, 0, sizeof(Txn));
    memset(&SourceBuffer, 0, sizeof(SourceBuffer));
    memset(&DestBuffer, 0, sizeof(DestBuffer));
    memset(&RegRec, 0, sizeof(RegRec));
    Txn.RegRecPtr = &RegRec;

    UtAssert_INT32_EQ(CFE_TBL_EncodeOutputData(&Txn, &SourceBuffer, &DestBuffer), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_DataTypeDB_PackCompleteObject), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_EncodeOutputData(&Txn, &SourceBuffer, &DestBuffer), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_DataTypeDB_GetTypeInfo), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_EncodeOutputData(&Txn, &SourceBuffer, &DestBuffer), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
}

void UT_TBL_DecodeInputData_Test(void)
{
    /* Test Case for:
     * CFE_Status_t CFE_TBL_DecodeInputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
     * CFE_TBL_LoadBuff_t *DestBuffer);
     */
    CFE_TBL_TxnState_t    Txn;
    CFE_TBL_LoadBuff_t    SourceBuffer;
    CFE_TBL_LoadBuff_t    DestBuffer;
    CFE_TBL_RegistryRec_t RegRec;

    memset(&Txn, 0, sizeof(Txn));
    memset(&SourceBuffer, 0, sizeof(SourceBuffer));
    memset(&DestBuffer, 0, sizeof(DestBuffer));
    memset(&RegRec, 0, sizeof(RegRec));
    Txn.RegRecPtr = &RegRec;

    UtAssert_INT32_EQ(CFE_TBL_DecodeInputData(&Txn, &SourceBuffer, &DestBuffer), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);

    RegRec.Config.EdsId = EDSLIB_MAKE_ID(1, 1);
    UtAssert_INT32_EQ(CFE_TBL_DecodeInputData(&Txn, &SourceBuffer, &DestBuffer), CFE_SUCCESS);
    UtAssert_ZERO(Txn.NumPendingEvents);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_DataTypeDB_UnpackCompleteObject), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_DecodeInputData(&Txn, &SourceBuffer, &DestBuffer), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);

    UT_SetDeferredRetcode(UT_KEY(EdsLib_DataTypeDB_GetTypeInfo), 1, -1);
    UtAssert_INT32_EQ(CFE_TBL_DecodeInputData(&Txn, &SourceBuffer, &DestBuffer), CFE_STATUS_EXTERNAL_RESOURCE_FAIL);
}

void UT_TBL_RegisterCodecTests(void)
{
    UtTest_Add(UT_TBL_ValidateCodecConfig_Test, UT_TBL_GlobalDataReset, NULL, "Test CFE_TBL_ValidateCodecConfig()");
    UtTest_Add(UT_TBL_ValidateCodecLoadSize_Test, UT_TBL_GlobalDataReset, NULL, "Test CFE_TBL_ValidateCodecLoadSize()");
    UtTest_Add(UT_TBL_CodecGetFinalStatus_Test, UT_TBL_GlobalDataReset, NULL, "Test CFE_TBL_CodecGetFinalStatus()");
    UtTest_Add(UT_TBL_DecodeHeadersFromFile_Test, UT_TBL_GlobalDataReset, NULL, "Test CFE_TBL_DecodeHeadersFromFile()");
    UtTest_Add(UT_TBL_EncodeHeadersToFile_Test, UT_TBL_GlobalDataReset, NULL, "Test CFE_TBL_EncodeHeadersToFile()");
    UtTest_Add(UT_TBL_EncodeOutputData_Test, UT_TBL_GlobalDataReset, NULL, "Test CFE_TBL_EncodeOutputData()");
    UtTest_Add(UT_TBL_DecodeInputData_Test, UT_TBL_GlobalDataReset, NULL, "Test CFE_TBL_DecodeInputData()");
}
