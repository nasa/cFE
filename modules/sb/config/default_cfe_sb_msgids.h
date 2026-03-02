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
 *   CFE Event Services (CFE_SB) Application Message IDs
 */
#ifndef DEFAULT_CFE_SB_MSGIDS_H
#define DEFAULT_CFE_SB_MSGIDS_H

#include "cfe_sb_msgid_values.h"

/*
** cFE Command Message Id's
*/
#define CFE_SB_CMD_MID          CFE_PLATFORM_SB_CMD_MIDVAL(CMD)          /* Default=0x1803 */
#define CFE_SB_SEND_HK_MID      CFE_PLATFORM_SB_CMD_MIDVAL(SEND_HK)      /* Default=0x180B */
#define CFE_SB_SUB_RPT_CTRL_MID CFE_PLATFORM_SB_CMD_MIDVAL(SUB_RPT_CTRL) /* Default=0x180E */

/*
** CFE Telemetry Message Id's
*/
#define CFE_SB_HK_TLM_MID      CFE_PLATFORM_SB_TLM_MIDVAL(HK_TLM)      /* Default=0x0803 */
#define CFE_SB_STATS_TLM_MID   CFE_PLATFORM_SB_TLM_MIDVAL(STATS_TLM)   /* Default=0x080A */
#define CFE_SB_ALLSUBS_TLM_MID CFE_PLATFORM_SB_TLM_MIDVAL(ALLSUBS_TLM) /* Default=0x080D */
#define CFE_SB_ONESUB_TLM_MID  CFE_PLATFORM_SB_TLM_MIDVAL(ONESUB_TLM)  /* Default=0x080E */

#endif
