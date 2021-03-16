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
 * Includes
 */
#include "osapi.h"
#include "cfe.h"
#include "utstubs.h"
#include "cfe_resourceid.h"
#include "cfe_resourceid_basevalue.h"

uint32 CFE_ResourceId_GetBase(CFE_ResourceId_t ResourceId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_GetBase), ResourceId);

    int32 return_code;

    return_code = UT_DEFAULT_IMPL_RC(CFE_ResourceId_GetBase, -1);

    /* If a return code value was set, return it directly */
    if (return_code >= 0)
    {
        return (uint32)return_code;
    }

    /* otherwise mimic a typical output (mask upper bits) */
    return CFE_ResourceId_ToInteger(ResourceId) & ~((uint32)CFE_RESOURCEID_MAX);
}

uint32 CFE_ResourceId_GetSerial(CFE_ResourceId_t ResourceId)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_GetSerial), ResourceId);

    int32 return_code;

    return_code = UT_DEFAULT_IMPL_RC(CFE_ResourceId_GetSerial, -1);

    /* If a return code value was set, return it directly */
    if (return_code >= 0)
    {
        return (uint32)return_code;
    }

    /* otherwise mimic a typical output (mask lower bits) */
    return (CFE_ResourceId_ToInteger(ResourceId) & ((uint32)CFE_RESOURCEID_MAX));
}

CFE_ResourceId_t CFE_ResourceId_FindNext(CFE_ResourceId_t StartId, uint32 TableSize,
                                         bool (*CheckFunc)(CFE_ResourceId_t))
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_FindNext), StartId);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_FindNext), TableSize);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_FindNext), CheckFunc);

    int32 return_code;

    /* Using "1" by default here produces a sequential result when called multiple times */
    return_code = UT_DEFAULT_IMPL_RC(CFE_ResourceId_FindNext, 1);

    if (return_code < 0)
    {
        return CFE_RESOURCEID_UNDEFINED;
    }

    /*
     * The test case may set the return code to indicate the offset from the start ID
     */
    return CFE_ResourceId_FromInteger(CFE_ResourceId_ToInteger(StartId) + return_code);
}

int32 CFE_ResourceId_ToIndex(CFE_ResourceId_t Id, uint32 BaseValue, uint32 TableSize, uint32 *Idx)
{
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_ToIndex), Id);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_ToIndex), BaseValue);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_ToIndex), TableSize);
    UT_Stub_RegisterContextGenericArg(UT_KEY(CFE_ResourceId_ToIndex), Idx);

    int32 return_code;

    return_code = UT_DEFAULT_IMPL(CFE_ResourceId_ToIndex);

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
        }
    }

    return return_code;
}
