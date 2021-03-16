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
** Includes
*/
#include <string.h>
#include "cfe_tbl.h"

#include "utstubs.h"

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_TBL_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TBL function
**        CFE_TBL_EarlyInit.  The user can adjust the response by setting
**        the values in the TBLEarlyInitRtn structure prior to this function
**        being called.  If the value TBLEarlyInitRtn.count is non-zero then
**        the return value is set to -1.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either -1 or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TBL_EarlyInit(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_EarlyInit);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_TBL_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE TBL function
**        CFE_TBL_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_TBL_TaskMain(void)
{
    UT_DEFAULT_IMPL(CFE_TBL_TaskMain);
}

/*****************************************************************************/
/**
** \brief CFE_TBL_CleanUpApp stub function
**
** \par Description
**        This function is used to mimic the response of the cFE TBL function
**        CFE_TBL_CleanUpApp.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_TBL_CleanUpApp(CFE_ES_AppId_t AppId)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_CleanUpApp);

    return status;
}

int32 CFE_TBL_Register(CFE_TBL_Handle_t *        TblHandlePtr,         /* Returned Handle */
                       const char *              Name,                 /* Application specific name  */
                       size_t                    Size,                 /* Size, in bytes, of table   */
                       uint16                    TblOptionFlags,       /* Tbl Options Settings     */
                       CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr) /* Ptr to func that validates tbl */
{
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_Register), TblHandlePtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_Register), Name);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Register), Size);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Register), TblOptionFlags);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Register), TblValidationFuncPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Register);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TBL_Register), (uint8 *)TblHandlePtr, sizeof(CFE_TBL_Handle_t));
    }

    return status;
}

int32 CFE_TBL_GetAddress(void **TblPtr, CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_GetAddress), TblPtr);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_GetAddress), TblHandle);

    int32 status;
    int32 ForceValue;

    status = UT_DEFAULT_IMPL(CFE_TBL_GetAddress);
    if (status >= 0 && !UT_Stub_CheckDefaultReturnValue(UT_KEY(CFE_TBL_GetAddress), &ForceValue))
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TBL_GetAddress), (uint8 *)TblPtr, sizeof(void *));
    }

    return status;
}

int32 CFE_TBL_Load(CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Load), TblHandle);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Load), SrcType);
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_Load), SrcDataPtr);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Load);

    return status;
}

int32 CFE_TBL_Unregister(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Unregister), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Unregister);

    return status;
}

int32 CFE_TBL_Manage(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Manage), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Manage);

    return status;
}

int32 CFE_TBL_ReleaseAddress(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_ReleaseAddress), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_ReleaseAddress);

    return status;
}

int32 CFE_TBL_ReleaseAddresses(uint16 NumTables, const CFE_TBL_Handle_t TblHandles[])
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_ReleaseAddresses), NumTables);
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_ReleaseAddresses), TblHandles);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_ReleaseAddresses);

    return status;
}

int32 CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, CFE_SB_MsgId_t MsgId, uint16 CommandCode, uint32 Parameter)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_NotifyByMessage), TblHandle);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_NotifyByMessage), MsgId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_NotifyByMessage), CommandCode);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_NotifyByMessage), Parameter);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_NotifyByMessage);

    return status;
}

int32 CFE_TBL_Modified(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Modified), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Modified);

    return status;
}

int32 CFE_TBL_GetStatus(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_GetStatus), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_GetStatus);

    return status;
}

int32 CFE_TBL_DumpToBuffer(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_DumpToBuffer), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_DumpToBuffer);

    return status;
}

int32 CFE_TBL_Validate(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Validate), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Validate);

    return status;
}

int32 CFE_TBL_Update(CFE_TBL_Handle_t TblHandle)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_Update), TblHandle);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Update);

    return status;
}

int32 CFE_TBL_GetInfo(CFE_TBL_Info_t *TblInfoPtr, const char *TblName)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_GetInfo), TblInfoPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_GetInfo), TblName);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_GetInfo);
    if (status >= 0 &&
        UT_Stub_CopyToLocal(UT_KEY(CFE_TBL_GetInfo), TblInfoPtr, sizeof(*TblInfoPtr)) < sizeof(*TblInfoPtr))
    {
        /* just clear the output struct */
        memset(TblInfoPtr, 0, sizeof(*TblInfoPtr));
    }

    return status;
}

int32 CFE_TBL_GetAddresses(void **TblPtrs[], uint16 NumTables, const CFE_TBL_Handle_t TblHandles[])
{
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_GetAddresses), TblPtrs);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_TBL_GetAddresses), NumTables);
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_GetAddresses), TblHandles);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_GetAddresses);

    return status;
}

int32 CFE_TBL_Share(CFE_TBL_Handle_t *TblHandlePtr, const char *TblName)
{
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_Share), TblHandlePtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_TBL_Share), TblName);

    int32 status;

    status = UT_DEFAULT_IMPL(CFE_TBL_Share);

    return status;
}
