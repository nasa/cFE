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
 *   CFE Software Bus (CFE_SB) Application Topic IDs
 */
#ifndef CFE_SB_TOPICIDS_H
#define CFE_SB_TOPICIDS_H

/**
**  \cfemissioncfg cFE Portable Message Numbers for Commands
**
**  \par Description:
**      Portable message numbers for the cFE command messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_SB_CMD_TOPICID          3
#define CFE_MISSION_SB_SEND_HK_TOPICID      11
#define CFE_MISSION_SB_SUB_RPT_CTRL_TOPICID 14

/**
**  \cfemissioncfg cFE Portable Message Numbers for Telemetry
**
**  \par Description:
**      Portable message numbers for the cFE telemetry messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_SB_HK_TLM_TOPICID      3
#define CFE_MISSION_SB_STATS_TLM_TOPICID   10
#define CFE_MISSION_SB_ALLSUBS_TLM_TOPICID 13
#define CFE_MISSION_SB_ONESUB_TLM_TOPICID  14

#endif
