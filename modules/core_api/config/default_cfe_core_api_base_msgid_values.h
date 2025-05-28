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
 *
 * This header file contains the platform-specific base msg ID values and
 * logic to convert a topic ID to a message ID value.
 *
 */

#ifndef DEFAULT_CFE_CORE_BASE_MSGID_VALUES_H
#define DEFAULT_CFE_CORE_BASE_MSGID_VALUES_H

#define CFE_PLATFORM_BASE_MIDVAL(x) DEFAULT_CPU1_##x##_MID_BASE
#define CFE_GLOBAL_BASE_MIDVAL(x)   DEFAULT_GLOBAL_##x##_MID_BASE

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
#define CFE_PLATFORM_CMD_TOPICID_TO_MIDV(topic) (CFE_PLATFORM_CMD_MID_BASE | (topic))

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
#define CFE_PLATFORM_TLM_TOPICID_TO_MIDV(topic) (CFE_PLATFORM_TLM_MID_BASE | (topic))

/**
 * \brief Convert a "global" command topic ID to a MsgID value
 *
 * A global command is one that is not specific to an individual instance of CFE,
 * but rather intended to be broadcast to all CFE instances at the same time.
 *
 * This is otherwise identical to #CFE_PLATFORM_CMD_TOPICID_TO_MIDV
 */
#define CFE_GLOBAL_CMD_TOPICID_TO_MIDV(topic) (CFE_GLOBAL_CMD_MID_BASE | (topic))

/**
 * \brief Convert a "global" telemetry topic ID to a MsgID value
 *
 * A global telemetry is one that is not specific to an individual instance of CFE,
 * but rather intended to be broadcast to all CFE instances at the same time.
 *
 * This is otherwise identical to #CFE_PLATFORM_TLM_TOPICID_TO_MIDV
 */
#define CFE_GLOBAL_TLM_TOPICID_TO_MIDV(topic) (CFE_GLOBAL_TLM_MID_BASE | (topic))

#endif /* CFE_CORE_BASE_MSGIDS_H */
