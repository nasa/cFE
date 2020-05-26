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
** File: cfe_sb_util.c
**
** Purpose:
**      This file contains 'access' macros and functions for reading and
**      writing message header fields.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/


/*
** Include Files
*/

#include <string.h>
#include <stdlib.h>

#include "exthdr_msgformat_accessors.h"
#include "ccsds_msgformat_accessors.h"
#include "cfe_error.h"


int32 ExtHdr_MsgFormat_SetInitialFields(ExtHdr_MsgFormat_BaseHeader_t *BasePtr,
        uint32         TotalLength,
        bool           Clear)
{
    ExtHdr_MsgFormat_Sequence_t    PreservedSeq;
    ExtHdr_MsgFormat_Length_t      MsgLen;
    uint32 SysMsgSize;

    /* Zero the entire packet if needed. */
    if (Clear)
    {
        memset(BasePtr, 0, TotalLength);
    }
    else    /* Clear only the primary header. */
    {
        /* Save the sequence count in case it must be preserved. */
        ExtHdr_MsgFormat_GetSequence(BasePtr, &PreservedSeq);

        memset(BasePtr, 0, sizeof(*BasePtr));
    }

    /* Set the length fields in the primary header. */
    SysMsgSize = TotalLength;
    ExtHdr_MsgFormat_SizeToMsgLength(&SysMsgSize, &MsgLen);
    ExtHdr_MsgFormat_SetTotalSize(BasePtr, &MsgLen);

    /* Restore the sequence count if needed. */
    if (!Clear)
    {
        ExtHdr_MsgFormat_SetSequence(BasePtr, &PreservedSeq);
    }

    return CFE_SUCCESS;
}


/*
 * GETTER/SETTER Functions ---
 * The general pattern should be to use an IO buffer parameter (pointer) for the value to get/set
 * The return value should be an int32 status code, nominally CFE_SUCCESS.
 */

/**
 * Get the message ID (abstract SB endpoint) from the message header
 */
int32 ExtHdr_MsgFormat_GetMsgAddress(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_MsgAddress_t *MsgAddressBuf)
{
    /* get APID data from base message */
    CCSDS_MsgFormat_GetMsgAddress(&BasePtr->Base, &MsgAddressBuf->Base);
    uint32 LocalReg;

    /* append with extra information from APIDQ header */
    LocalReg = BasePtr->ApidQ.APIDQSubsystem[0];
    LocalReg <<= 8;
    LocalReg |= BasePtr->ApidQ.APIDQSubsystem[1];
    MsgAddressBuf->SubSysId = LocalReg & 0x1FF;

    LocalReg = BasePtr->ApidQ.APIDQSystemId[0];
    LocalReg <<= 8;
    LocalReg |= BasePtr->ApidQ.APIDQSystemId[1];
    MsgAddressBuf->SystemId = LocalReg & 0xFFFF;

    return CFE_SUCCESS;
}

/**
 * Set the message ID (abstract SB endpoint) in the message header
 */
int32 ExtHdr_MsgFormat_SetMsgAddress(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_MsgAddress_t *MsgAddressBuf)
{
    /* set APID data in base message */
    CCSDS_MsgFormat_SetMsgAddress(&BasePtr->Base, &MsgAddressBuf->Base);
    uint32 LocalReg;

    /* append with extra information in APIDQ header */
    LocalReg = MsgAddressBuf->SubSysId;
    BasePtr->ApidQ.APIDQSubsystem[1] = LocalReg & 0xFF;
    LocalReg >>= 8;
    BasePtr->ApidQ.APIDQSubsystem[0] = (BasePtr->ApidQ.APIDQSubsystem[0] & 0xFE) | (LocalReg & 0x1);

    LocalReg = MsgAddressBuf->SystemId;
    BasePtr->ApidQ.APIDQSystemId[1] = LocalReg & 0xFF;
    LocalReg >>= 8;
    BasePtr->ApidQ.APIDQSystemId[0] = LocalReg & 0xFF;

    return CFE_SUCCESS;
}

/**
 * Get the size of the header data.
 * This is not settable, it is fixed based on the message header format and type of message.
 *
 * This outputs the size of the actual header data, not including any alignment padding.
 */
int32 ExtHdr_MsgFormat_GetHeaderLength(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, uint32 *SizeBuf)
{
    if (ExtHdr_MsgFormat_GetIsCommand(BasePtr))
    {
        *SizeBuf = sizeof(ExtHdr_MsgFormat_CommandPacket_t);
    }
    else if (ExtHdr_MsgFormat_GetIsTelemetry(BasePtr))
    {
        *SizeBuf = sizeof(ExtHdr_MsgFormat_TelemetryPacket_t);
    }
    else
    {
        /* fallback - should not be possible in CFE */
        *SizeBuf = sizeof(ExtHdr_MsgFormat_BaseHeader_t);
    }

    return CFE_SUCCESS;
}


/**
 * Get the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this gets the timestamp.
 * Returns an error code if message format does not contain a timestamp.
 */
int32 ExtHdr_MsgFormat_GetTimestamp(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Time_t *TimeBuf)
{
    const ExtHdr_MsgFormat_TelemetryPacket_t* TlmMsg;

    /* Check that the msg is a TLM type */
    if (!CCSDS_MsgFormat_GetIsTelemetry(&BasePtr->Base))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    TlmMsg = (const ExtHdr_MsgFormat_TelemetryPacket_t*)BasePtr;

    return CCSDS_MsgFormat_GetTlmTimestamp(&TlmMsg->Sec, TimeBuf);
}

/**
 * Set the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this sets the field to the given value
 * Returns an error code if message format does not contain a timestamp.
 */
int32 ExtHdr_MsgFormat_SetTimestamp(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_Time_t *Time)
{
    ExtHdr_MsgFormat_TelemetryPacket_t* TlmMsg;

    /* Check that the msg is a TLM type */
    if (!CCSDS_MsgFormat_GetIsTelemetry(&BasePtr->Base))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    TlmMsg = (ExtHdr_MsgFormat_TelemetryPacket_t*)BasePtr;

    return CCSDS_MsgFormat_SetTlmTimestamp(&TlmMsg->Sec, Time);
}

/**
 * Get the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 ExtHdr_MsgFormat_GetFunctionCode(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_FunctionCode_t *CodeBuf)
{
    ExtHdr_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(&BasePtr->Base))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (ExtHdr_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_GetCmdFunctionCode(&CmdMsg->Sec, CodeBuf);
}

/**
 * Set the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 ExtHdr_MsgFormat_SetFunctionCode(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_FunctionCode_t *CodeBuf)
{
    ExtHdr_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(&BasePtr->Base))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (ExtHdr_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_SetCmdFunctionCode(&CmdMsg->Sec, CodeBuf);
}


/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 ExtHdr_MsgFormat_GetChecksum(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Checksum_t *ChecksumBuf)
{
    const ExtHdr_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(&BasePtr->Base))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (const ExtHdr_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_GetCmdChecksum(&CmdMsg->Sec, ChecksumBuf);
}

/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 ExtHdr_MsgFormat_SetChecksum(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_Checksum_t *ChecksumBuf)
{
    ExtHdr_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(&BasePtr->Base))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (ExtHdr_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_SetCmdChecksum(&CmdMsg->Sec, ChecksumBuf);
}


/**
 * Compute the error control / checksum field, if applicable
 */
int32 ExtHdr_MsgFormat_GetComputedChecksum(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Checksum_t *ChecksumBuf)
{
    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(&BasePtr->Base))
   {
        /* no checksum field, nothing to do */
        return CFE_SB_WRONG_MSG_TYPE;
    }

    *ChecksumBuf = CCSDS_MsgFormat_ComputeChecksum_Impl(&BasePtr->Base,
            offsetof(ExtHdr_MsgFormat_CommandPacket_t, Sec.JChecksum));

    return CFE_SUCCESS;
}

