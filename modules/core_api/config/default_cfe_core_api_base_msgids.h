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

#ifndef CFE_CORE_BASE_MSGIDS_H
#define CFE_CORE_BASE_MSGIDS_H

/**
 * \brief Platform command message ID base offset
 *
 * Example mechanism for setting default command bits and deconflicting MIDs across multiple
 * platforms in a mission.  For any sufficiently complex mission this method is
 * typically replaced by a centralized message ID management scheme.
 *
 * 0x1800 - Nominal value for default message ID implementation (V1). This sets the command
 *          field and the secondary header present field.  Typical V1 command MID range
 *          is 0x1800-1FFF.  Additional cpus can deconflict message IDs by incrementing
 *          this value to provide sub-allocations (0x1900 for example).
 * 0x0080 - Command bit for MISSION_MSGID_V2 message ID implementation (V2).  Although
 *          this can be used for the value below due to the relatively small set
 *          of MIDs in the framework it will not scale so an alternative
 *          method of deconfliction is recommended.
 */
#define CFE_CPU1_CMD_MID_BASE 0x1800

/**
 * \brief Platform telemetry message ID base offset
 *
 * 0x0800 - Nominal for message ID V1
 * 0x0000 - Potential value for MISSION_MSGID_V2, but limited to a range of
 *          0x0000-0x007F since the command bit is 0x0080.  Alternative
 *          method of deconfliction is recommended.
 *
 * See #CFE_CPU1_CMD_MID_BASE for more information
 */
#define CFE_CPU1_TLM_MID_BASE 0x0800

/**
 * \brief "Global" command message ID base offset
 *
 * 0x1860 - Nominal value for message ID V1
 * 0x00E0 - Potential value for MISSION_MSGID_V2, note command bit is 0x0080.
 *          Works in limited cases only, alternative method of deconfliction
 *          is recommended.
 * See #CFE_CPU1_CMD_MID_BASE for more information
 */
#define CFE_GLOBAL_CMD_MID_BASE 0x1860

/**
 * \brief "Global" telemetry message ID base offset
 *
 * 0x0860 - Nominal value for message ID V1
 * 0x0060 - Potential value for MISSION_MSGID_V2, note command bit is 0x0080.
 *          Works in limited cases only, alternative method of deconfliction
 *          is recommended.
 * See #CFE_CPU1_CMD_MID_BASE for more information
 */
#define CFE_GLOBAL_TLM_MID_BASE 0x0860

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
#define CFE_PLATFORM_CMD_TOPICID_TO_MIDV(topic) (CFE_CPU1_CMD_MID_BASE | (topic))

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
#define CFE_PLATFORM_TLM_TOPICID_TO_MIDV(topic) (CFE_CPU1_TLM_MID_BASE | (topic))

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
