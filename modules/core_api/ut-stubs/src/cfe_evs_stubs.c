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
 * Auto-Generated stub implementations for functions defined in cfe_evs header
 */

#include <stdarg.h>

#include "cfe_evs.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_EVS_SendEvent(void *, UT_EntryKey_t, const UT_StubContext_t *, va_list);
void UT_DefaultHandler_CFE_EVS_SendEventWithAppID(void *, UT_EntryKey_t, const UT_StubContext_t *, va_list);
void UT_DefaultHandler_CFE_EVS_SendTimedEvent(void *, UT_EntryKey_t, const UT_StubContext_t *, va_list);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_Register()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_EVS_Register(const void *Filters, uint16 NumEventFilters, uint16 FilterScheme)
{
    UT_GenStub_SetupReturnBuffer(CFE_EVS_Register, CFE_Status_t);

    UT_GenStub_AddParam(CFE_EVS_Register, const void *, Filters);
    UT_GenStub_AddParam(CFE_EVS_Register, uint16, NumEventFilters);
    UT_GenStub_AddParam(CFE_EVS_Register, uint16, FilterScheme);

    UT_GenStub_Execute(CFE_EVS_Register, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_EVS_Register, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_ResetAllFilters()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_EVS_ResetAllFilters(void)
{
    UT_GenStub_SetupReturnBuffer(CFE_EVS_ResetAllFilters, CFE_Status_t);

    UT_GenStub_Execute(CFE_EVS_ResetAllFilters, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_EVS_ResetAllFilters, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_ResetFilter()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_EVS_ResetFilter(uint16 EventID)
{
    UT_GenStub_SetupReturnBuffer(CFE_EVS_ResetFilter, CFE_Status_t);

    UT_GenStub_AddParam(CFE_EVS_ResetFilter, uint16, EventID);

    UT_GenStub_Execute(CFE_EVS_ResetFilter, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_EVS_ResetFilter, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_SendEvent()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_EVS_SendEvent(uint16 EventID, CFE_EVS_EventType_Enum_t EventType, const char *Spec, ...)
{
    va_list UtStub_ArgList;

    UT_GenStub_SetupReturnBuffer(CFE_EVS_SendEvent, CFE_Status_t);

    UT_GenStub_AddParam(CFE_EVS_SendEvent, uint16, EventID);
    UT_GenStub_AddParam(CFE_EVS_SendEvent, CFE_EVS_EventType_Enum_t, EventType);
    UT_GenStub_AddParam(CFE_EVS_SendEvent, const char *, Spec);

    va_start(UtStub_ArgList, Spec);
    UT_GenStub_Execute(CFE_EVS_SendEvent, Va, UT_DefaultHandler_CFE_EVS_SendEvent, UtStub_ArgList);
    va_end(UtStub_ArgList);

    return UT_GenStub_GetReturnValue(CFE_EVS_SendEvent, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_SendEventWithAppID()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_EVS_SendEventWithAppID(uint16 EventID, CFE_EVS_EventType_Enum_t EventType, CFE_ES_AppId_t AppID,
                                        const char *Spec, ...)
{
    va_list UtStub_ArgList;

    UT_GenStub_SetupReturnBuffer(CFE_EVS_SendEventWithAppID, CFE_Status_t);

    UT_GenStub_AddParam(CFE_EVS_SendEventWithAppID, uint16, EventID);
    UT_GenStub_AddParam(CFE_EVS_SendEventWithAppID, CFE_EVS_EventType_Enum_t, EventType);
    UT_GenStub_AddParam(CFE_EVS_SendEventWithAppID, CFE_ES_AppId_t, AppID);
    UT_GenStub_AddParam(CFE_EVS_SendEventWithAppID, const char *, Spec);

    va_start(UtStub_ArgList, Spec);
    UT_GenStub_Execute(CFE_EVS_SendEventWithAppID, Va, UT_DefaultHandler_CFE_EVS_SendEventWithAppID, UtStub_ArgList);
    va_end(UtStub_ArgList);

    return UT_GenStub_GetReturnValue(CFE_EVS_SendEventWithAppID, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_EVS_SendTimedEvent()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_EVS_SendTimedEvent(CFE_TIME_SysTime_t Time, uint16 EventID, CFE_EVS_EventType_Enum_t EventType,
                                    const char *Spec, ...)
{
    va_list UtStub_ArgList;

    UT_GenStub_SetupReturnBuffer(CFE_EVS_SendTimedEvent, CFE_Status_t);

    UT_GenStub_AddParam(CFE_EVS_SendTimedEvent, CFE_TIME_SysTime_t, Time);
    UT_GenStub_AddParam(CFE_EVS_SendTimedEvent, uint16, EventID);
    UT_GenStub_AddParam(CFE_EVS_SendTimedEvent, CFE_EVS_EventType_Enum_t, EventType);
    UT_GenStub_AddParam(CFE_EVS_SendTimedEvent, const char *, Spec);

    va_start(UtStub_ArgList, Spec);
    UT_GenStub_Execute(CFE_EVS_SendTimedEvent, Va, UT_DefaultHandler_CFE_EVS_SendTimedEvent, UtStub_ArgList);
    va_end(UtStub_ArgList);

    return UT_GenStub_GetReturnValue(CFE_EVS_SendTimedEvent, CFE_Status_t);
}
