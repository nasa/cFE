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
 * This header file contains the logic to convert a topic ID to a message ID value.
 *
 * In an conventional deployment, the macro simply combines the base MID with the topic ID
 * to produce the MID value.  It is simple bitwise "OR" operation.
 *
 * This logic is intended to be customizable.  By overriding this file, these macros
 * can be replaced with whatever logic is desired.  However, the same logic must be used
 * across all instances.  In order to simply tune the base value used for a given instance,
 * only the cfs_base_msgids.h file needs to be overridden.
 */

#ifndef DEFAULT_CFE_CORE_API_MSGID_MAPPING_H
#define DEFAULT_CFE_CORE_API_MSGID_MAPPING_H

/* This header must provide CFE_PLATFORM_BASE_MIDVAL and CFE_PLATFORM_BASE_MIDVAL */
#include "cfe_core_api_base_msgid_values.h"

/**
 * \brief Convert a command topic ID to a MsgID value
 *
 * This defines the logic to convert a topic ID value into a message ID value.
 * This operates on integer values and should resolve at compile time such
 * that it can be used in e.g. switch/case statements.
 *
 * \note The result of this conversion is a simple integer, thus also needs to
 * go through CFE_SB_ValueToMsgId() to obtain a properly-typed CFE_SB_MsgId_t
 * for interacting with SB APIs.
 */
#define CFE_PLATFORM_CMD_TOPICID_TO_MIDV(topic) (CFE_PLATFORM_BASE_MIDVAL(CMD) | (topic))
#define DEFAULT_CFE_PLATFORM_CMD_MID_BASE       0x1800

/**
 * \brief Convert a telemetry topic ID to a MsgID value
 *
 * This defines the logic to convert a topic ID value into a message ID value.
 * This operates on integer values and should resolve at compile time such
 * that it can be used in e.g. switch/case statements.
 *
 * \note The result of this conversion is a simple integer, thus also needs to
 * go through CFE_SB_ValueToMsgId() to obtain a properly-typed CFE_SB_MsgId_t
 * for interacting with SB APIs.
 */
#define CFE_PLATFORM_TLM_TOPICID_TO_MIDV(topic) (CFE_PLATFORM_BASE_MIDVAL(TLM) | (topic))
#define DEFAULT_CFE_PLATFORM_TLM_MID_BASE       0x0800

/**
 * \brief Convert a "global" command topic ID to a MsgID value
 *
 * A global command is one that is not specific to an individual instance of CFE,
 * but rather intended to be broadcast to all CFE instances at the same time.
 *
 * This is otherwise identical to #CFE_PLATFORM_CMD_TOPICID_TO_MIDV
 */
#define CFE_GLOBAL_CMD_TOPICID_TO_MIDV(topic) (CFE_GLOBAL_BASE_MIDVAL(CMD) | (topic))
#define DEFAULT_GLOBAL_CMD_MID_BASE           0x1860

/**
 * \brief Convert a "global" telemetry topic ID to a MsgID value
 *
 * A global telemetry is one that is not specific to an individual instance of CFE,
 * but rather intended to be broadcast to all CFE instances at the same time.
 *
 * This is otherwise identical to #CFE_PLATFORM_TLM_TOPICID_TO_MIDV
 */
#define CFE_GLOBAL_TLM_TOPICID_TO_MIDV(topic) (CFE_GLOBAL_BASE_MIDVAL(TLM) | (topic))
#define DEFAULT_GLOBAL_TLM_MID_BASE           0x0860

#endif /* CFE_CORE_BASE_MSGIDS_H */
