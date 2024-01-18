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
 *  Message id access functions
 *
 * cFS default version 2 implementation using CCSDS headers
 *
 * Message Id:
 *   7 bits from the primary header APID (0x7F of 0x7FF)
 *   1 bit for the command/telemetry flag
 *   0 bits from the Playback flag
 *   8 bits from the secondary header APID qualifier (Subsystem) (0xFF of 0x01FF)
 *   0 bits from the secondary header APID qualifier as the System
 *   = 16 bits total
 *
 *              Byte 1              Byte 0
 *        7 6 5 4 3 2 1 0     7      6 5 4 3 2 1 0
 *       +-+-+-+-+-+-+-+-+|--------|+-+-+-+-+-+-+-+
 *       | APID Qualifier |C/T flg | Pri Hdr APID |
 *       +-+-+-+-+-+-+-+-+|--------|+-+-+-+-+-+-+-+
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"
#include "cfe_sb.h"
#include "cfe_platform_cfg.h"

/* cFS MsgId definitions */
#define CFE_MSG_MSGID_APID_MASK   0x007F /**< \brief CCSDS ApId mask for MsgId */
#define CFE_MSG_MSGID_TYPE_MASK   0x0080 /**< \brief Message type mask for MsgId, set = cmd */
#define CFE_MSG_MSGID_SUBSYS_MASK 0xFF00 /**< \brief Subsystem mask for MsgId */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetMsgId(const CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t *MsgId)
{
    CFE_SB_MsgId_Atom_t msgidval;
    CFE_MSG_Type_t      type;

    if (MsgPtr == NULL || MsgId == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, assumes tlm if invalid */
    CFE_MSG_GetType(MsgPtr, &type);

    /* Set message ID bits from CCSDS header fields */
    msgidval = MsgPtr->CCSDS.Pri.StreamId[1] & CFE_MSG_MSGID_APID_MASK;
    if (type == CFE_MSG_Type_Cmd)
    {
        msgidval |= CFE_MSG_MSGID_TYPE_MASK;
    }
    msgidval |= (MsgPtr->CCSDS.Ext.Subsystem[1] << 8) & CFE_MSG_MSGID_SUBSYS_MASK;

    *MsgId = CFE_SB_ValueToMsgId(msgidval);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetMsgId(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId)
{
    CFE_SB_MsgId_Atom_t msgidval = CFE_SB_MsgIdToValue(MsgId);

    if (MsgPtr == NULL || !CFE_SB_IsValidMsgId(MsgId))
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Clear and set PID_MSGID_MASK bits */
    MsgPtr->CCSDS.Pri.StreamId[1] =
        (MsgPtr->CCSDS.Pri.StreamId[1] & ~CFE_MSG_MSGID_APID_MASK) | (msgidval & CFE_MSG_MSGID_APID_MASK);

    /* Set APIDQ Subsystem bits */
    MsgPtr->CCSDS.Ext.Subsystem[1] = ((msgidval & CFE_MSG_MSGID_SUBSYS_MASK) >> 8);

    /* Set type, ignores return since no failure action */
    if ((msgidval & CFE_MSG_MSGID_TYPE_MASK) != 0)
    {
        CFE_MSG_SetType(MsgPtr, CFE_MSG_Type_Cmd);
    }
    else
    {
        CFE_MSG_SetType(MsgPtr, CFE_MSG_Type_Tlm);
    }

    return CFE_SUCCESS;
}
