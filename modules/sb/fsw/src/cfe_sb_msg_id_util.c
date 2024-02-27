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

/******************************************************************************
** File: cfe_sb_msg_id_util.c
** Purpose: message ID utility functions
*/

/*
** Include Files
*/
#include "cfe_sb_module_all.h"
#include "cfe_core_api_base_msgids.h"

/*
 * IMPORTANT:
 * The "core_api_base_msgids.h" header should have defined the following macros:
 *   CFE_PLATFORM_CMD_TOPICID_TO_MIDV
 *   CFE_PLATFORM_TLM_TOPICID_TO_MIDV
 *   CFE_GLOBAL_CMD_TOPICID_TO_MIDV
 *   CFE_GLOBAL_TLM_TOPICID_TO_MIDV
 *
 * However, historically in CFE, MsgIDs were directly defined at compile-time (not run-time)
 * and not based on a topic ID.  It is possible that when using an older CFE configuration
 * that it does not define these macros, and thus run-time conversion cannot be performed.
 *
 * If that is the case, then all these functions will return the invalid MsgID value (0).
 * The coverage tests will fail (which should let the user know the feature is not working
 * correctly) but CFE and related apps will still execute just fine so long as nothing
 * relies on the run-time conversions.
 */

#ifndef CFE_PLATFORM_CMD_TOPICID_TO_MIDV
#define CFE_PLATFORM_CMD_TOPICID_TO_MIDV(x) 0
#endif

#ifndef CFE_PLATFORM_TLM_TOPICID_TO_MIDV
#define CFE_PLATFORM_TLM_TOPICID_TO_MIDV(x) 0
#endif

#ifndef CFE_GLOBAL_CMD_TOPICID_TO_MIDV
#define CFE_GLOBAL_CMD_TOPICID_TO_MIDV(x) 0
#endif

#ifndef CFE_GLOBAL_TLM_TOPICID_TO_MIDV
#define CFE_GLOBAL_TLM_TOPICID_TO_MIDV(x) 0
#endif

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
size_t CFE_SB_MsgHdrSize(const CFE_MSG_Message_t *MsgPtr)
{
    size_t         size      = 0;
    bool           hassechdr = false;
    CFE_MSG_Type_t type      = CFE_MSG_Type_Invalid;

    if (MsgPtr == NULL)
    {
        return size;
    }

    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);
    CFE_MSG_GetType(MsgPtr, &type);

    /* if secondary hdr is not present... */
    /* Since all cFE messages must have a secondary hdr this check is not needed */
    if (!hassechdr)
    {
        size = sizeof(CFE_MSG_Message_t);
    }
    else if (type == CFE_MSG_Type_Cmd)
    {
        size = sizeof(CFE_MSG_CommandHeader_t);
    }
    else if (type == CFE_MSG_Type_Tlm)
    {
        size = sizeof(CFE_MSG_TelemetryHeader_t);
    }

    return size;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_CmdTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum)
{
    CFE_SB_MsgId_Atom_t MsgIdValue;

    /* Instance Number 0 is reserved for globals */
    if (InstanceNum == 0)
    {
        MsgIdValue = CFE_SB_GlobalTlmTopicIdToMsgId(TopicId);
    }
    else
    {
        MsgIdValue = CFE_PLATFORM_CMD_TOPICID_TO_MIDV(TopicId);
    }

    return MsgIdValue;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_TlmTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum)
{
    CFE_SB_MsgId_Atom_t MsgIdValue;

    /* Instance Number 0 is reserved for globals */
    if (InstanceNum == 0)
    {
        MsgIdValue = CFE_SB_GlobalTlmTopicIdToMsgId(TopicId);
    }
    else
    {
        MsgIdValue = CFE_PLATFORM_TLM_TOPICID_TO_MIDV(TopicId);
    }

    return MsgIdValue;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_GlobalCmdTopicIdToMsgId(uint16 TopicId)
{
    return CFE_GLOBAL_CMD_TOPICID_TO_MIDV(TopicId);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_GlobalTlmTopicIdToMsgId(uint16 TopicId)
{
    return CFE_GLOBAL_TLM_TOPICID_TO_MIDV(TopicId);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_LocalCmdTopicIdToMsgId(uint16 TopicId)
{
    /* PSP-reported Instance number is used for locals */
    return CFE_SB_CmdTopicIdToMsgId(TopicId, CFE_PSP_GetProcessorId());
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_LocalTlmTopicIdToMsgId(uint16 TopicId)
{
    /* PSP-reported Instance number is used for locals */
    return CFE_SB_TlmTopicIdToMsgId(TopicId, CFE_PSP_GetProcessorId());
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_IsValidMsgId(CFE_SB_MsgId_t MsgId)
{
    return (!CFE_SB_MsgId_Equal(MsgId, CFE_SB_INVALID_MSG_ID) &&
            CFE_SB_MsgIdToValue(MsgId) <= CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
}
