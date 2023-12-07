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
 *   CFE Time Services (CFE_TIME) Application Topic IDs
 */
#ifndef CFE_TIME_TOPICIDS_H
#define CFE_TIME_TOPICIDS_H

/**
**  \cfemissioncfg cFE Portable Message Numbers for Commands
**
**  \par Description:
**      Portable message numbers for the cFE command messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_TIME_CMD_TOPICID       5
#define CFE_MISSION_TIME_SEND_HK_TOPICID   13
#define CFE_MISSION_TIME_TONE_CMD_TOPICID  16
#define CFE_MISSION_TIME_ONEHZ_CMD_TOPICID 17

/**
**  \cfemissioncfg cFE Portable Message Numbers for Global Messages
**
**  \par Description:
**      Portable message numbers for the cFE global messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_TIME_DATA_CMD_TOPICID 0
#define CFE_MISSION_TIME_SEND_CMD_TOPICID 2

/**
**  \cfemissioncfg cFE Portable Message Numbers for Telemetry
**
**  \par Description:
**      Portable message numbers for the cFE telemetry messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_TIME_HK_TLM_TOPICID   5
#define CFE_MISSION_TIME_DIAG_TLM_TOPICID 6

#endif
