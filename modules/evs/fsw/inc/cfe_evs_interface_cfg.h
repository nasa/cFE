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
 *
 * CFE Event Services (CFE_EVS) Interface Configuration Header File
 */
#ifndef CFE_EVS_INTERFACE_CFG_H
#define CFE_EVS_INTERFACE_CFG_H

#include "cfe_evs_interface_cfg_values.h"

/**
**  \cfeevscfg Maximum Event Message Length
**
**  \par Description:
**      Indicates the maximum length (in characters) of the formatted text
**      string portion of an event message
**
**      This length does not need to include an extra character for NULL termination.
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_EVS_MAX_MESSAGE_LENGTH         CFE_MISSION_EVS_CFGVAL(MAX_MESSAGE_LENGTH)
#define DEFAULT_CFE_MISSION_EVS_MAX_MESSAGE_LENGTH 122

#endif
