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
 * Auto-Generated stub implementations for functions defined in cfe_resourceid header
 */

#include "cfe_resourceid.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_ResourceId_FindNext(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ResourceId_GetBase(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ResourceId_GetSerial(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_ResourceId_ToIndex(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ResourceId_FindNext()
 * ----------------------------------------------------
 */
CFE_ResourceId_t CFE_ResourceId_FindNext(CFE_ResourceId_t StartId, uint32 TableSize,
                                         bool (*CheckFunc)(CFE_ResourceId_t))
{
    UT_GenStub_SetupReturnBuffer(CFE_ResourceId_FindNext, CFE_ResourceId_t);

    UT_GenStub_AddParam(CFE_ResourceId_FindNext, CFE_ResourceId_t, StartId);
    UT_GenStub_AddParam(CFE_ResourceId_FindNext, uint32, TableSize);

    UT_GenStub_Execute(CFE_ResourceId_FindNext, Basic, UT_DefaultHandler_CFE_ResourceId_FindNext);

    return UT_GenStub_GetReturnValue(CFE_ResourceId_FindNext, CFE_ResourceId_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ResourceId_GetBase()
 * ----------------------------------------------------
 */
uint32 CFE_ResourceId_GetBase(CFE_ResourceId_t ResourceId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ResourceId_GetBase, uint32);

    UT_GenStub_AddParam(CFE_ResourceId_GetBase, CFE_ResourceId_t, ResourceId);

    UT_GenStub_Execute(CFE_ResourceId_GetBase, Basic, UT_DefaultHandler_CFE_ResourceId_GetBase);

    return UT_GenStub_GetReturnValue(CFE_ResourceId_GetBase, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ResourceId_GetSerial()
 * ----------------------------------------------------
 */
uint32 CFE_ResourceId_GetSerial(CFE_ResourceId_t ResourceId)
{
    UT_GenStub_SetupReturnBuffer(CFE_ResourceId_GetSerial, uint32);

    UT_GenStub_AddParam(CFE_ResourceId_GetSerial, CFE_ResourceId_t, ResourceId);

    UT_GenStub_Execute(CFE_ResourceId_GetSerial, Basic, UT_DefaultHandler_CFE_ResourceId_GetSerial);

    return UT_GenStub_GetReturnValue(CFE_ResourceId_GetSerial, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ResourceId_ToIndex()
 * ----------------------------------------------------
 */
int32 CFE_ResourceId_ToIndex(CFE_ResourceId_t Id, uint32 BaseValue, uint32 TableSize, uint32 *Idx)
{
    UT_GenStub_SetupReturnBuffer(CFE_ResourceId_ToIndex, int32);

    UT_GenStub_AddParam(CFE_ResourceId_ToIndex, CFE_ResourceId_t, Id);
    UT_GenStub_AddParam(CFE_ResourceId_ToIndex, uint32, BaseValue);
    UT_GenStub_AddParam(CFE_ResourceId_ToIndex, uint32, TableSize);
    UT_GenStub_AddParam(CFE_ResourceId_ToIndex, uint32 *, Idx);

    UT_GenStub_Execute(CFE_ResourceId_ToIndex, Basic, UT_DefaultHandler_CFE_ResourceId_ToIndex);

    return UT_GenStub_GetReturnValue(CFE_ResourceId_ToIndex, int32);
}
