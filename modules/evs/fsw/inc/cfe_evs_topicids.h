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
 *   CFE Event Services (CFE_EVS) Application Topic IDs
 */
#ifndef CFE_EVS_TOPICIDS_H
#define CFE_EVS_TOPICIDS_H

#include "cfe_evs_topicid_values.h"

/**
**  \cfemissioncfg cFE Portable Message Numbers for Commands
**
**  \par Description:
**      Portable message numbers for the cFE EVS command messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_EVS_CMD_TOPICID             CFE_MISSION_EVS_TIDVAL(CMD)
#define DEFAULT_CFE_MISSION_EVS_CMD_TOPICID     1
#define CFE_MISSION_EVS_SEND_HK_TOPICID         CFE_MISSION_EVS_TIDVAL(SEND_HK)
#define DEFAULT_CFE_MISSION_EVS_SEND_HK_TOPICID 9

/**
**  \cfemissioncfg cFE Portable Message Numbers for Telemetry
**
**  \par Description:
**      Portable message numbers for the cFE EVS telemetry messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_EVS_HK_TLM_TOPICID                  CFE_MISSION_EVS_TIDVAL(HK_TLM)
#define DEFAULT_CFE_MISSION_EVS_HK_TLM_TOPICID          1
#define CFE_MISSION_EVS_LONG_EVENT_MSG_TOPICID          CFE_MISSION_EVS_TIDVAL(LONG_EVENT_MSG)
#define DEFAULT_CFE_MISSION_EVS_LONG_EVENT_MSG_TOPICID  8
#define CFE_MISSION_EVS_SHORT_EVENT_MSG_TOPICID         CFE_MISSION_EVS_TIDVAL(SHORT_EVENT_MSG)
#define DEFAULT_CFE_MISSION_EVS_SHORT_EVENT_MSG_TOPICID 9

#endif
