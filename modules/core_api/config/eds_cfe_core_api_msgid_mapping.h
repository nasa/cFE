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
 * In an EDS deployment, there is a runtime function that should be called, and this
 * does a lookup into the EDS DB to perform the translation.
 */

#ifndef EDS_CFE_CORE_API_MSGID_MAPPING_H
#define EDS_CFE_CORE_API_MSGID_MAPPING_H

/**
 * \brief Convert a command topic ID to a MsgID value
 *
 * \note The result of this conversion is a simple integer, thus also needs to
 * go through CFE_SB_ValueToMsgId() to obtain a properly-typed CFE_SB_MsgId_t
 * for interacting with SB APIs.
 */
#define CFE_PLATFORM_CMD_TOPICID_TO_MIDV(topic) CFE_SB_LocalCmdTopicIdToMsgId(topic)

/**
 * \brief Convert a telemetry topic ID to a MsgID value
 *
 * \note The result of this conversion is a simple integer, thus also needs to
 * go through CFE_SB_ValueToMsgId() to obtain a properly-typed CFE_SB_MsgId_t
 * for interacting with SB APIs.
 */
#define CFE_PLATFORM_TLM_TOPICID_TO_MIDV(topic) CFE_SB_LocalTlmTopicIdToMsgId(topic)

/**
 * \brief Convert a "global" command topic ID to a MsgID value
 *
 * A global command is one that is not specific to an individual instance of CFE,
 * but rather intended to be broadcast to all CFE instances at the same time.
 *
 * This is otherwise identical to #CFE_PLATFORM_CMD_TOPICID_TO_MIDV
 */
#define CFE_GLOBAL_CMD_TOPICID_TO_MIDV(topic) CFE_SB_GlobalCmdTopicIdToMsgId(topic)

/**
 * \brief Convert a "global" telemetry topic ID to a MsgID value
 *
 * A global telemetry is one that is not specific to an individual instance of CFE,
 * but rather intended to be broadcast to all CFE instances at the same time.
 *
 * This is otherwise identical to #CFE_PLATFORM_TLM_TOPICID_TO_MIDV
 */
#define CFE_GLOBAL_TLM_TOPICID_TO_MIDV(topic) CFE_SB_GlobalTlmTopicIdToMsgId(topic)

#endif /* CFE_CORE_BASE_MSGIDS_H */
