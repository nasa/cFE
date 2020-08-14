/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/******************************************************************************
 *  Message id access functions, cFS version 1 implementation
 */
#include "cfe_msg_api.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"
#include "cfe_platform_cfg.h"

/******************************************************************************
 * Get message id - See API and header file for details
 * cFS default version 1 implementation using CCSDS headers
 *
 * Message Id = CCSDS Stream ID (in local endian)
 */
int32 CFE_MSG_GetMsgId(const CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t *MsgId)
{

    CFE_SB_MsgId_Atom_t msgidval;

    if (MsgPtr == NULL || MsgId == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    msgidval = (MsgPtr->CCSDS.Pri.StreamId[0] << 8) + MsgPtr->CCSDS.Pri.StreamId[1];
    *MsgId   = CFE_SB_ValueToMsgId(msgidval);

    return CFE_SUCCESS;
}

/******************************************************************************
 * Set message id - See API and header file for details
 * cFS default version 1 implementations using CCSDS headers
 *
 * CCSDS Stream ID = Message Id converted to big endian
 */
int32 CFE_MSG_SetMsgId(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId)
{

    CFE_SB_MsgId_Atom_t msgidval = CFE_SB_MsgIdToValue(MsgId);

    if (MsgPtr == NULL || msgidval > CFE_PLATFORM_SB_HIGHEST_VALID_MSGID)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    MsgPtr->CCSDS.Pri.StreamId[0] = (uint8)(msgidval >> 8);
    MsgPtr->CCSDS.Pri.StreamId[1] = (uint8)(msgidval);

    return CFE_SUCCESS;
}
