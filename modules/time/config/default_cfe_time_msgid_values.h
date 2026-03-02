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
 *   CFE Time Services (CFE_TIME) Application Message IDs
 */
#ifndef DEFAULT_CFE_TIME_MSGID_VALUES_H
#define DEFAULT_CFE_TIME_MSGID_VALUES_H

#include "cfe_core_api_base_msgids.h"
#include "cfe_time_topicids.h"

#define CFE_PLATFORM_TIME_CMD_MIDVAL(x) CFE_PLATFORM_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_##x##_TOPICID)
#define CFE_PLATFORM_TIME_TLM_MIDVAL(x) CFE_PLATFORM_TLM_TOPICID_TO_MIDV(CFE_MISSION_TIME_##x##_TOPICID)

/* time is unique in that it uses "global" cmd msgids - these are the same on all instances */
#define CFE_PLATFORM_TIME_GLBCMD_MIDVAL(x) CFE_GLOBAL_CMD_TOPICID_TO_MIDV(CFE_MISSION_TIME_##x##_TOPICID)

#endif
