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
 *   CFE Executive Services (CFE_ES) Application Message IDs
 */
#ifndef CFE_ES_MSGIDS_H
#define CFE_ES_MSGIDS_H

#include "cfe_core_api_base_msgids.h"
#include "cfe_es_topicids.h"

/*
** cFE ES Command Message Id's
*/
#define CFE_ES_CMD_MID     CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_ES_CMD_TOPICID)     /* 0x1806 */
#define CFE_ES_SEND_HK_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_ES_SEND_HK_TOPICID) /* 0x1808 */

/*
** CFE ES Telemetry Message Id's
*/
#define CFE_ES_HK_TLM_MID       CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_ES_HK_TLM_TOPICID)       /* 0x0800 */
#define CFE_ES_APP_TLM_MID      CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_ES_APP_TLM_TOPICID)      /* 0x080B */
#define CFE_ES_MEMSTATS_TLM_MID CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_ES_MEMSTATS_TLM_TOPICID) /* 0x0810 */

#endif
