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
 * Includes
 */
#include "osapi.h"
#include "cfe.h"
#include "utstubs.h"
#include "cfe_resourceid.h"
#include "cfe_resourceid_basevalue.h"

/*------------------------------------------------------------
 *
 * Default handler for CFE_ResourceId_GetBase coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ResourceId_GetBase(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ResourceId_t ResourceId = UT_Hook_GetArgValueByName(Context, "ResourceId", CFE_ResourceId_t);
    int32            status;
    uint32           base_value;

    /* If a return code value was set, return it directly */
    if (UT_Stub_GetInt32StatusCode(Context, &status))
    {
        base_value = (uint32)status;
    }
    else
    {
        /* otherwise mimic a typical output (mask upper bits) */
        base_value = CFE_ResourceId_ToInteger(ResourceId) & ~((uint32)CFE_RESOURCEID_MAX);
    }

    UT_Stub_SetReturnValue(FuncKey, base_value);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ResourceId_GetSerial coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ResourceId_GetSerial(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ResourceId_t ResourceId = UT_Hook_GetArgValueByName(Context, "ResourceId", CFE_ResourceId_t);
    int32            status;
    uint32           serial_value;

    /* If a return code value was set, return it directly */
    if (UT_Stub_GetInt32StatusCode(Context, &status))
    {
        serial_value = (uint32)status;
    }
    else
    {
        /* otherwise mimic a typical output (mask lower bits) */
        serial_value = (CFE_ResourceId_ToInteger(ResourceId) & ((uint32)CFE_RESOURCEID_MAX));
    }

    UT_Stub_SetReturnValue(FuncKey, serial_value);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ResourceId_FindNext coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ResourceId_FindNext(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ResourceId_t StartId = UT_Hook_GetArgValueByName(Context, "StartId", CFE_ResourceId_t);
    CFE_ResourceId_t NextId;
    int32            status;

    /* Using "1" by default here produces a sequential result when called multiple times */
    if (!UT_Stub_GetInt32StatusCode(Context, &status))
    {
        status = 1;
    }

    if (status < 0)
    {
        NextId = CFE_RESOURCEID_UNDEFINED;
    }
    else
    {
        /*
         * The test case may set the return code to indicate the offset from the start ID
         */
        NextId = CFE_ResourceId_FromInteger(CFE_ResourceId_ToInteger(StartId) + status);
    }

    UT_Stub_SetReturnValue(FuncKey, NextId);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_ResourceId_ToIndex coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_ResourceId_ToIndex(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_ResourceId_t Id        = UT_Hook_GetArgValueByName(Context, "Id", CFE_ResourceId_t);
    uint32           BaseValue = UT_Hook_GetArgValueByName(Context, "BaseValue", uint32);
    uint32           TableSize = UT_Hook_GetArgValueByName(Context, "TableSize", uint32);
    uint32 *         Idx       = UT_Hook_GetArgValueByName(Context, "Idx", uint32 *);

    int32 return_code;

    UT_Stub_GetInt32StatusCode(Context, &return_code);

    if (return_code < 0)
    {
        /* fill with a very bad value that should cause a problem if used */
        *Idx = 0xDEADBEEF;
    }
    else if (UT_Stub_CopyToLocal(UT_KEY(CFE_ResourceId_ToIndex), Idx, sizeof(*Idx)) < sizeof(*Idx))
    {
        /* fill with default value if unspecified by test case */
        if (CFE_ResourceId_IsDefined(Id))
        {
            *Idx = (CFE_ResourceId_ToInteger(Id) - BaseValue) % TableSize;
        }
        else
        {
            *Idx        = 0xDEADBEEF;
            return_code = CFE_ES_ERR_RESOURCEID_NOT_VALID;
            UT_Stub_SetReturnValue(FuncKey, return_code);
        }
    }
}
