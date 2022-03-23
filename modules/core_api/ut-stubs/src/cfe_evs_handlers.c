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
** File: ut_evs_stubs.c
**
** Purpose:
** Unit test stubs for Event Service routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe_evs.h"

#include "utstubs.h"
#include "uttools.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_EVS_SendEvent coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_EVS_SendEvent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context,
                                         va_list va)
{
    uint16      EventID = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
    const char *Spec    = UT_Hook_GetArgValueByName(Context, "Spec", const char *);

    int32 status;

    UtDebug("CFE_EVS_SendEvent: %u - %s", EventID, Spec);

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendEvent), (uint8 *)&EventID, sizeof(EventID));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_EVS_SendTimedEvent coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_EVS_SendTimedEvent(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context,
                                              va_list va)
{
    uint16      EventID = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
    const char *Spec    = UT_Hook_GetArgValueByName(Context, "Spec", const char *);

    int32 status;

    UtDebug("CFE_EVS_SendTimedEvent: %u - %s", EventID, Spec);

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendTimedEvent), (uint8 *)&EventID, sizeof(EventID));
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_EVS_SendEventWithAppID coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_EVS_SendEventWithAppID(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context,
                                                  va_list va)
{
    uint16      EventID = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
    const char *Spec    = UT_Hook_GetArgValueByName(Context, "Spec", const char *);

    int32 status;

    UtDebug("CFE_EVS_SendEventWithAppID: %u - %s", EventID, Spec);

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_EVS_SendEventWithAppID), (uint8 *)&EventID, sizeof(EventID));
    }
}
