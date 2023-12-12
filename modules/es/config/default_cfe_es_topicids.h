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
 *   CFE Executive Services (CFE_ES) Application Topic IDs
 */
#ifndef CFE_ES_TOPICIDS_H
#define CFE_ES_TOPICIDS_H

/**
**  \cfemissioncfg cFE Portable Message Numbers for Commands
**
**  \par Description:
**      Portable message numbers for the cFE ES command messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_ES_CMD_TOPICID     6
#define CFE_MISSION_ES_SEND_HK_TOPICID 8

/**
**  \cfemissioncfg cFE Portable Message Numbers for Telemetry
**
**  \par Description:
**      Portable message numbers for the cFE ES telemetry messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_ES_HK_TLM_TOPICID       0
#define CFE_MISSION_ES_APP_TLM_TOPICID      11
#define CFE_MISSION_ES_MEMSTATS_TLM_TOPICID 16

#endif
