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
 *   Specification for the CFE Event Services (CFE_EVS) command and telemetry
 *   message constant definitions.
 *
 *  For CFE_EVS this is only the function/command code definitions
 */
#ifndef CFE_EVS_MSGDEFS_H
#define CFE_EVS_MSGDEFS_H

#include "cfe_evs_fcncodes.h"

/* Event Type bit masks */
#define CFE_EVS_DEBUG_BIT       0x0001
#define CFE_EVS_INFORMATION_BIT 0x0002
#define CFE_EVS_ERROR_BIT       0x0004
#define CFE_EVS_CRITICAL_BIT    0x0008

/* Output Port bit masks */
#define CFE_EVS_PORT1_BIT 0x0001
#define CFE_EVS_PORT2_BIT 0x0002
#define CFE_EVS_PORT3_BIT 0x0004
#define CFE_EVS_PORT4_BIT 0x0008

#endif
