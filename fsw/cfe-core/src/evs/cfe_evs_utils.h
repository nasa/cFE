/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
**  Filename: cfe_evs_utils.h
**
**  Title:    Event Services Task and API - Utility functions.
**
**  Purpose:
**            Unit specification for the event services utility functions.
**
**  Contents:
**       I.  macro and constant type definitions
**      II.  EVS utility internal structures
**     III.  function prototypes
**
**  Design Notes:
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
*/

#ifndef _cfe_evs_utils_
#define _cfe_evs_utils_

/********************* Include Files  ************************/

#include "cfe_evs_task.h"        /* EVS internal definitions */

/* ==============   Section I: Macro and Constant Type Definitions   =========== */

/* ==============   Section II: Internal Structures ============ */     

/* ==============   Section III: Function Prototypes =========== */

int32 EVS_GetAppID(uint32 *AppIdPtr);

int32 EVS_GetApplicationInfo(uint32 *pAppID, const char *pAppName);

int32 EVS_NotRegistered(uint32 AppID);

bool EVS_IsFiltered(uint32 AppID, uint16 EventID, uint16 EventType);

EVS_BinFilter_t *EVS_FindEventID(int16 EventID, EVS_BinFilter_t *FilterArray);

void EVS_EnableTypes(uint8 BitMask, uint32 AppID);

void EVS_DisableTypes(uint8 BitMask, uint32 AppID);

void EVS_GenerateEventTelemetry(uint32 AppID, uint16 EventID, uint16 EventType,
        const CFE_TIME_SysTime_t *Time, const char *MsgSpec, va_list ArgPtr);

int32 EVS_SendEvent (uint16 EventID, uint16 EventType, const char *Spec, ... );

#endif  /* _cfe_evs_utils_ */
