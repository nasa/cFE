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
** File: ut_config_handlers.c
**
** Purpose:
** Unit test handlers for Configuration Service routines
**
** Notes:
** Mostly implement a NULL default return where needed, as this is not done
** automatically by UT assert.
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_config.h"

#include "utstubs.h"
#include "utassert.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_Config_GetString coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_Config_GetString(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32      status;
    const char FIXED_BUFFER[] = "UT";
    void *     Buffer;

    Buffer = NULL;
    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_GetDataBuffer(FuncKey, &Buffer, NULL, NULL);

        /*
         * The public API has a contract _not_ to return a NULL
         * pointer, so this should be mimiced by the UT stub
         */
        if (Buffer == NULL)
        {
            Buffer = (void *)FIXED_BUFFER;
        }
    }

    UT_Stub_SetReturnValue(FuncKey, Buffer);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_Config_GetObjPointer coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_Config_GetObjPointer(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32 status;
    void *Buffer;

    Buffer = NULL;
    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_GetDataBuffer(FuncKey, &Buffer, NULL, NULL);
    }

    UT_Stub_SetReturnValue(FuncKey, Buffer);
}
