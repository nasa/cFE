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
 *   CFE Time Services (CFE_TIME) Application Message IDs
 */
#ifndef CFE_TIME_MSGIDS_H
#define CFE_TIME_MSGIDS_H

#include "cfe_core_api_base_msgids.h"
#include "cfe_time_topicids.h"

/*
** cFE Command Message Id's
*/
#define CFE_TIME_CMD_MID       CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_CMD_TOPICID)       /* 0x1805 */
#define CFE_TIME_SEND_HK_MID   CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_SEND_HK_TOPICID)   /* 0x180D */
#define CFE_TIME_TONE_CMD_MID  CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_TONE_CMD_TOPICID)  /* 0x1810 */
#define CFE_TIME_ONEHZ_CMD_MID CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_ONEHZ_CMD_TOPICID) /* 0x1811 */

/*
** cFE Global Command Message Id's
*/
#define CFE_TIME_DATA_CMD_MID CFE_GLOBAL_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_DATA_CMD_TOPICID) /* 0x1860 */
#define CFE_TIME_SEND_CMD_MID CFE_GLOBAL_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_SEND_CMD_TOPICID) /* 0x1862 */

/*
** CFE Telemetry Message Id's
*/
#define CFE_TIME_HK_TLM_MID   CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_TIME_HK_TLM_TOPICID)   /* 0x0805 */
#define CFE_TIME_DIAG_TLM_MID CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_TIME_DIAG_TLM_TOPICID) /* 0x0806 */

#ifndef CFE_OMIT_DEPRECATED
/*
 * This name was changed/deprecated because technically it constitutes an
 * identifier that starts with a digit (1) when the CFE_TIME_ namespace
 * qualifier is removed.  This matters in some circumstances.
 */
#define CFE_TIME_1HZ_CMD_MID CFE_TIME_ONEHZ_CMD_MID
#endif

#endif
