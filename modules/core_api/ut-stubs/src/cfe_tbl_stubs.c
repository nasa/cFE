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
 * Auto-Generated stub implementations for functions defined in cfe_tbl header
 */

#include "cfe_tbl.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_TBL_GetAddress(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TBL_GetInfo(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_TBL_Register(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_DumpToBuffer()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_DumpToBuffer(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_DumpToBuffer, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_DumpToBuffer, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_DumpToBuffer, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_DumpToBuffer, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_GetAddress()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_GetAddress(void **TblPtr, CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_GetAddress, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_GetAddress, void **, TblPtr);
    UT_GenStub_AddParam(CFE_TBL_GetAddress, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_GetAddress, Basic, UT_DefaultHandler_CFE_TBL_GetAddress);

    return UT_GenStub_GetReturnValue(CFE_TBL_GetAddress, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_GetAddresses()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_GetAddresses(void **TblPtrs[], uint16 NumTables, const CFE_TBL_Handle_t TblHandles[])
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_GetAddresses, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_GetAddresses, uint16, NumTables);

    UT_GenStub_Execute(CFE_TBL_GetAddresses, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_GetAddresses, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_GetInfo()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_GetInfo(CFE_TBL_Info_t *TblInfoPtr, const char *TblName)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_GetInfo, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_GetInfo, CFE_TBL_Info_t *, TblInfoPtr);
    UT_GenStub_AddParam(CFE_TBL_GetInfo, const char *, TblName);

    UT_GenStub_Execute(CFE_TBL_GetInfo, Basic, UT_DefaultHandler_CFE_TBL_GetInfo);

    return UT_GenStub_GetReturnValue(CFE_TBL_GetInfo, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_GetStatus()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_GetStatus(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_GetStatus, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_GetStatus, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_GetStatus, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_GetStatus, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Load()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Load(CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Load, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Load, CFE_TBL_Handle_t, TblHandle);
    UT_GenStub_AddParam(CFE_TBL_Load, CFE_TBL_SrcEnum_t, SrcType);
    UT_GenStub_AddParam(CFE_TBL_Load, const void *, SrcDataPtr);

    UT_GenStub_Execute(CFE_TBL_Load, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_Load, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Manage()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Manage(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Manage, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Manage, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_Manage, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_Manage, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Modified()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Modified(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Modified, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Modified, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_Modified, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_Modified, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_NotifyByMessage()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, CFE_SB_MsgId_t MsgId, CFE_MSG_FcnCode_t CommandCode,
                                     uint32 Parameter)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_NotifyByMessage, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_NotifyByMessage, CFE_TBL_Handle_t, TblHandle);
    UT_GenStub_AddParam(CFE_TBL_NotifyByMessage, CFE_SB_MsgId_t, MsgId);
    UT_GenStub_AddParam(CFE_TBL_NotifyByMessage, CFE_MSG_FcnCode_t, CommandCode);
    UT_GenStub_AddParam(CFE_TBL_NotifyByMessage, uint32, Parameter);

    UT_GenStub_Execute(CFE_TBL_NotifyByMessage, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_NotifyByMessage, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Register()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Register(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, size_t Size, uint16 TblOptionFlags,
                              CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Register, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Register, CFE_TBL_Handle_t *, TblHandlePtr);
    UT_GenStub_AddParam(CFE_TBL_Register, const char *, Name);
    UT_GenStub_AddParam(CFE_TBL_Register, size_t, Size);
    UT_GenStub_AddParam(CFE_TBL_Register, uint16, TblOptionFlags);
    UT_GenStub_AddParam(CFE_TBL_Register, CFE_TBL_CallbackFuncPtr_t, TblValidationFuncPtr);

    UT_GenStub_Execute(CFE_TBL_Register, Basic, UT_DefaultHandler_CFE_TBL_Register);

    return UT_GenStub_GetReturnValue(CFE_TBL_Register, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_ReleaseAddress()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_ReleaseAddress(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_ReleaseAddress, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_ReleaseAddress, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_ReleaseAddress, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_ReleaseAddress, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_ReleaseAddresses()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_ReleaseAddresses(uint16 NumTables, const CFE_TBL_Handle_t TblHandles[])
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_ReleaseAddresses, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_ReleaseAddresses, uint16, NumTables);

    UT_GenStub_Execute(CFE_TBL_ReleaseAddresses, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_ReleaseAddresses, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Share()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Share(CFE_TBL_Handle_t *TblHandlePtr, const char *TblName)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Share, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Share, CFE_TBL_Handle_t *, TblHandlePtr);
    UT_GenStub_AddParam(CFE_TBL_Share, const char *, TblName);

    UT_GenStub_Execute(CFE_TBL_Share, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_Share, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Unregister()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Unregister(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Unregister, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Unregister, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_Unregister, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_Unregister, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Update()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Update(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Update, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Update, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_Update, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_Update, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_TBL_Validate()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_TBL_Validate(CFE_TBL_Handle_t TblHandle)
{
    UT_GenStub_SetupReturnBuffer(CFE_TBL_Validate, CFE_Status_t);

    UT_GenStub_AddParam(CFE_TBL_Validate, CFE_TBL_Handle_t, TblHandle);

    UT_GenStub_Execute(CFE_TBL_Validate, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_TBL_Validate, CFE_Status_t);
}
