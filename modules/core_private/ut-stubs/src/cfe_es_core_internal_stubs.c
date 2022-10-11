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
 * Auto-Generated stub implementations for functions defined in cfe_es_core_internal header
 */

#include "cfe_es_core_internal.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_ES_RegisterCDSEx(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_CDS_EarlyInit()
 * ----------------------------------------------------
 */
int32 CFE_ES_CDS_EarlyInit(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_CDS_EarlyInit, int32);

    UT_GenStub_Execute(CFE_ES_CDS_EarlyInit, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_CDS_EarlyInit, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_DeleteCDS()
 * ----------------------------------------------------
 */
int32 CFE_ES_DeleteCDS(const char *CDSName, bool CalledByTblServices)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_DeleteCDS, int32);

    UT_GenStub_AddParam(CFE_ES_DeleteCDS, const char *, CDSName);
    UT_GenStub_AddParam(CFE_ES_DeleteCDS, bool, CalledByTblServices);

    UT_GenStub_Execute(CFE_ES_DeleteCDS, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_ES_DeleteCDS, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_RegisterCDSEx()
 * ----------------------------------------------------
 */
int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr, size_t UserBlockSize, const char *Name, bool CriticalTbl)
{
    UT_GenStub_SetupReturnBuffer(CFE_ES_RegisterCDSEx, int32);

    UT_GenStub_AddParam(CFE_ES_RegisterCDSEx, CFE_ES_CDSHandle_t *, HandlePtr);
    UT_GenStub_AddParam(CFE_ES_RegisterCDSEx, size_t, UserBlockSize);
    UT_GenStub_AddParam(CFE_ES_RegisterCDSEx, const char *, Name);
    UT_GenStub_AddParam(CFE_ES_RegisterCDSEx, bool, CriticalTbl);

    UT_GenStub_Execute(CFE_ES_RegisterCDSEx, Basic, UT_DefaultHandler_CFE_ES_RegisterCDSEx);

    return UT_GenStub_GetReturnValue(CFE_ES_RegisterCDSEx, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_ES_TaskMain()
 * ----------------------------------------------------
 */
void CFE_ES_TaskMain(void)
{
    UT_GenStub_Execute(CFE_ES_TaskMain, Basic, NULL);
}
