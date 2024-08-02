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
 * Auto-Generated stub implementations for functions defined in cfe_config header
 */

#include "cfe_config.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_Config_GetObjPointer(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_Config_GetString(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_GetArrayValue()
 * ----------------------------------------------------
 */
CFE_Config_ArrayValue_t CFE_Config_GetArrayValue(CFE_ConfigId_t ConfigId)
{
    UT_GenStub_SetupReturnBuffer(CFE_Config_GetArrayValue, CFE_Config_ArrayValue_t);

    UT_GenStub_AddParam(CFE_Config_GetArrayValue, CFE_ConfigId_t, ConfigId);

    UT_GenStub_Execute(CFE_Config_GetArrayValue, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_Config_GetArrayValue, CFE_Config_ArrayValue_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_GetIdByName()
 * ----------------------------------------------------
 */
CFE_ConfigId_t CFE_Config_GetIdByName(const char *Name)
{
    UT_GenStub_SetupReturnBuffer(CFE_Config_GetIdByName, CFE_ConfigId_t);

    UT_GenStub_AddParam(CFE_Config_GetIdByName, const char *, Name);

    UT_GenStub_Execute(CFE_Config_GetIdByName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_Config_GetIdByName, CFE_ConfigId_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_GetName()
 * ----------------------------------------------------
 */
const char *CFE_Config_GetName(CFE_ConfigId_t ConfigId)
{
    UT_GenStub_SetupReturnBuffer(CFE_Config_GetName, const char *);

    UT_GenStub_AddParam(CFE_Config_GetName, CFE_ConfigId_t, ConfigId);

    UT_GenStub_Execute(CFE_Config_GetName, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_Config_GetName, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_GetObjPointer()
 * ----------------------------------------------------
 */
const void *CFE_Config_GetObjPointer(CFE_ConfigId_t ConfigId)
{
    UT_GenStub_SetupReturnBuffer(CFE_Config_GetObjPointer, const void *);

    UT_GenStub_AddParam(CFE_Config_GetObjPointer, CFE_ConfigId_t, ConfigId);

    UT_GenStub_Execute(CFE_Config_GetObjPointer, Basic, UT_DefaultHandler_CFE_Config_GetObjPointer);

    return UT_GenStub_GetReturnValue(CFE_Config_GetObjPointer, const void *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_GetString()
 * ----------------------------------------------------
 */
const char *CFE_Config_GetString(CFE_ConfigId_t ConfigId)
{
    UT_GenStub_SetupReturnBuffer(CFE_Config_GetString, const char *);

    UT_GenStub_AddParam(CFE_Config_GetString, CFE_ConfigId_t, ConfigId);

    UT_GenStub_Execute(CFE_Config_GetString, Basic, UT_DefaultHandler_CFE_Config_GetString);

    return UT_GenStub_GetReturnValue(CFE_Config_GetString, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_GetValue()
 * ----------------------------------------------------
 */
uint32 CFE_Config_GetValue(CFE_ConfigId_t ConfigId)
{
    UT_GenStub_SetupReturnBuffer(CFE_Config_GetValue, uint32);

    UT_GenStub_AddParam(CFE_Config_GetValue, CFE_ConfigId_t, ConfigId);

    UT_GenStub_Execute(CFE_Config_GetValue, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_Config_GetValue, uint32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_GetVersionString()
 * ----------------------------------------------------
 */
void CFE_Config_GetVersionString(char *Buf, size_t Size, const char *Component, const char *SrcVersion,
                                 const char *CodeName, const char *LastOffcRel)
{
    UT_GenStub_AddParam(CFE_Config_GetVersionString, char *, Buf);
    UT_GenStub_AddParam(CFE_Config_GetVersionString, size_t, Size);
    UT_GenStub_AddParam(CFE_Config_GetVersionString, const char *, Component);
    UT_GenStub_AddParam(CFE_Config_GetVersionString, const char *, SrcVersion);
    UT_GenStub_AddParam(CFE_Config_GetVersionString, const char *, CodeName);
    UT_GenStub_AddParam(CFE_Config_GetVersionString, const char *, LastOffcRel);

    UT_GenStub_Execute(CFE_Config_GetVersionString, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_Config_IterateAll()
 * ----------------------------------------------------
 */
void CFE_Config_IterateAll(void *Arg, CFE_Config_Callback_t Callback)
{
    UT_GenStub_AddParam(CFE_Config_IterateAll, void *, Arg);
    UT_GenStub_AddParam(CFE_Config_IterateAll, CFE_Config_Callback_t, Callback);

    UT_GenStub_Execute(CFE_Config_IterateAll, Basic, NULL);
}
