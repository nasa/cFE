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
** File: ut_time_stubs.c
**
** Purpose:
** Unit test stubs for Time routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <stdio.h>
#include <string.h>
#include "cfe_time.h"

#include "utstubs.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_Print coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_Print(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    char *             PrintBuffer = UT_Hook_GetArgValueByName(Context, "PrintBuffer", char *);
    CFE_TIME_SysTime_t TimeToPrint = UT_Hook_GetArgValueByName(Context, "TimeToPrint", CFE_TIME_SysTime_t);

    snprintf(PrintBuffer, CFE_TIME_PRINTED_STRING_SIZE, "UT %u.%u -", (unsigned int)TimeToPrint.Seconds,
             (unsigned int)TimeToPrint.Subseconds);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_GetTime coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_GetTime(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    static CFE_TIME_SysTime_t SimTime = {0};
    CFE_TIME_SysTime_t        Result  = {0};
    int32                     status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetTime), &Result, sizeof(Result)) < sizeof(Result))
        {
            SimTime.Seconds++;
            SimTime.Subseconds++;
            Result = SimTime;
        }
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_Add coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_Add(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    static CFE_TIME_SysTime_t SimTime = {0};
    CFE_TIME_SysTime_t        Result  = {0};
    int32                     status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_Add), (uint8 *)&Result, sizeof(Result)) < sizeof(Result))
        {
            SimTime.Seconds++;
            SimTime.Subseconds++;
            Result = SimTime;
        }
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_GetMET coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_GetMET(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32              status;
    CFE_TIME_SysTime_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetMET), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_GetSTCF coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_GetSTCF(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32              status;
    CFE_TIME_SysTime_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetSTCF), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_GetTAI coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_GetTAI(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32              status;
    CFE_TIME_SysTime_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetTAI), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_GetUTC coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_GetUTC(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32              status;
    CFE_TIME_SysTime_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_GetUTC), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_MET2SCTime coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_MET2SCTime(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32              status;
    CFE_TIME_SysTime_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_MET2SCTime), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_TIME_Subtract coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_TIME_Subtract(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32              status;
    CFE_TIME_SysTime_t Result = {0};

    UT_Stub_GetInt32StatusCode(Context, &status);
    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_TIME_Subtract), &Result, sizeof(Result));
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}
