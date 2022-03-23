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
** File: cfe_fs_core_internal_handlers.c
**
** Purpose:
** Unit test stubs for Executive Service routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_fs_core_internal.h"

#include "utstubs.h"
#include "utassert.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_RunBackgroundFileDump coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_RunBackgroundFileDump(void *UserObj, UT_EntryKey_t FuncKey,
                                                    const UT_StubContext_t *Context)
{
    int32 status;
    bool  return_value;

    UT_Stub_GetInt32StatusCode(Context, &status);

    return_value = status;

    UT_Stub_SetReturnValue(FuncKey, return_value);
}
