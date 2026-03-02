/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 *   Specification for the CFE Executive Services (CFE_ES) command and telemetry
 *   message constant definitions.
 *
 *  For CFE_ES this is only the function/command code definitions
 */
#ifndef EDS_CFE_TIME_MSGDEFS_H
#define EDS_CFE_TIME_MSGDEFS_H

#include "cfe_time_eds_typedefs.h"
#include "cfe_time_fcncodes.h"

#define CFE_TIME_FLAG_TO_BITMASK(f) (0x1 << (15 - EdsLabel_CFE_TIME_FlagBit_##f))

#define CFE_TIME_FLAG_CLKSET CFE_TIME_FLAG_TO_BITMASK(CLKSET)
#define CFE_TIME_FLAG_FLYING CFE_TIME_FLAG_TO_BITMASK(FLYING)
#define CFE_TIME_FLAG_SRCINT CFE_TIME_FLAG_TO_BITMASK(SRCINT)
#define CFE_TIME_FLAG_SIGPRI CFE_TIME_FLAG_TO_BITMASK(SIGPRI)
#define CFE_TIME_FLAG_SRVFLY CFE_TIME_FLAG_TO_BITMASK(SRVFLY)
#define CFE_TIME_FLAG_CMDFLY CFE_TIME_FLAG_TO_BITMASK(CMDFLY)
#define CFE_TIME_FLAG_ADDADJ CFE_TIME_FLAG_TO_BITMASK(ADDADJ)
#define CFE_TIME_FLAG_ADD1HZ CFE_TIME_FLAG_TO_BITMASK(ADD1HZ)
#define CFE_TIME_FLAG_ADDTCL CFE_TIME_FLAG_TO_BITMASK(ADDTCL)
#define CFE_TIME_FLAG_SERVER CFE_TIME_FLAG_TO_BITMASK(SERVER)
#define CFE_TIME_FLAG_GDTONE CFE_TIME_FLAG_TO_BITMASK(GDTONE)
#define CFE_TIME_FLAG_REFERR CFE_TIME_FLAG_TO_BITMASK(REFERR)

#define CFE_TIME_FLAG_UNUSED 0x000F /**< \brief Reserved flags - should be zero */

#endif
