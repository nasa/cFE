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
