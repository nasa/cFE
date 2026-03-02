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
 *   CFE Test app (CFE_TESTCASE) Application Topic IDs
 */
#ifndef CFE_TESTCASE_TOPICIDS_H
#define CFE_TESTCASE_TOPICIDS_H

#include "cfe_test_topicid_values.h"

/**
**  \cfemissioncfg cFE Portable Message Numbers for Commands
**
**  \par Description:
**      Portable message numbers for the cFE ES command messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_TESTCASE_CMD_TOPICID         CFE_MISSION_TESTCASE_TIDVAL(CMD)
#define DEFAULT_CFE_MISSION_TESTCASE_CMD_TOPICID 2

/**
**  \cfemissioncfg cFE Portable Message Numbers for Telemetry
**
**  \par Description:
**      Portable message numbers for the cFE ES telemetry messages
**
**  \par Limits
**      Not Applicable
*/
#define CFE_MISSION_TESTCASE_HK_TLM_TOPICID         CFE_MISSION_TESTCASE_TIDVAL(HK_TLM)
#define DEFAULT_CFE_MISSION_TESTCASE_HK_TLM_TOPICID 2

#endif
