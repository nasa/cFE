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
** Includes
*/
#include <string.h>
#include "cfe_tbl.h"

#include "utstubs.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_TBL_Register coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TBL_Register(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_TBL_Handle_t *TblHandlePtr = UT_Hook_GetArgValueByName(Context, "TblHandlePtr", CFE_TBL_Handle_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TBL_Register), TblHandlePtr, sizeof(CFE_TBL_Handle_t));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TBL_GetAddress coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TBL_GetAddress(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    void **TblPtr = UT_Hook_GetArgValueByName(Context, "TblPtr", void **);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TBL_GetAddress), (uint8 *)TblPtr, sizeof(void *));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TBL_GetInfo coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TBL_GetInfo(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_TBL_Info_t *TblInfoPtr = UT_Hook_GetArgValueByName(Context, "TblInfoPtr", CFE_TBL_Info_t *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0 &&
        UT_Stub_CopyToLocal(UT_KEY(CFE_TBL_GetInfo), TblInfoPtr, sizeof(*TblInfoPtr)) < sizeof(*TblInfoPtr))
    {
        /* just clear the output struct */
        memset(TblInfoPtr, 0, sizeof(*TblInfoPtr));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TBL_HandleFromID coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TBL_HandleFromID(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_TBL_HandleId_t Id = UT_Hook_GetArgValueByName(Context, "TblId", CFE_TBL_HandleId_t);
    int32              handleval32;
    int16              handleval16; /* backward-compatible definition */
    CFE_TBL_Handle_t   ret;

    if (!UT_Stub_GetInt32StatusCode(Context, &handleval32))
    {
        handleval32 = CFE_TBL_HandleID_AsInt(Id);
    }

    if (handleval32 == 0)
    {
        ret = CFE_TBL_BAD_TABLE_HANDLE;
    }
    else if (sizeof(ret) == sizeof(handleval32))
    {
        /* normal 32-bit handle value */
        memcpy(&ret, &handleval32, sizeof(ret));
    }
    else if (sizeof(ret) == sizeof(handleval16))
    {
        /* historical 16-bit handle value */
        handleval16 = handleval32;
        memcpy(&ret, &handleval16, sizeof(ret));
    }
    else
    {
        memset(&ret, 1, sizeof(ret));
    }

    UT_Stub_SetReturnValue(FuncKey, ret);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TBL_HandleToID coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TBL_HandleToID(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_TBL_Handle_t   TblHandle = UT_Hook_GetArgValueByName(Context, "TblHandle", CFE_TBL_Handle_t);
    int32              handleval;
    CFE_TBL_HandleId_t ret;

    if (!UT_Stub_GetInt32StatusCode(Context, &handleval))
    {
        handleval = CFE_TBL_HANDLE_INT(TblHandle);
    }

    if (handleval == 0)
    {
        ret = CFE_TBL_HANDLEID_UNDEFINED;
    }
    else
    {
        ret = CFE_TBL_HANDLEID_C(CFE_ResourceId_FromInteger(handleval));
    }

    UT_Stub_SetReturnValue(FuncKey, ret);
}
