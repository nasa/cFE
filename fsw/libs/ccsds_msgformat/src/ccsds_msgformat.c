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

#include "ccsds_msgformat_accessors.h"
#include "cfe_error.h"


int32 CCSDS_MsgFormat_SetInitialFields(CCSDS_MsgFormat_BaseHeader_t *BasePtr,
        uint32         TotalLength,
        bool           Clear)
{
    CCSDS_MsgFormat_Sequence_t    PreservedSeq;
    CCSDS_MsgFormat_Length_t      MsgLen;
    uint32 SysMsgSize;

    /* Zero the entire packet if needed. */
    if (Clear)
    {
        memset(BasePtr, 0, TotalLength);
    }
    else    /* Clear only the primary header. */
    {
        /* Save the sequence count in case it must be preserved. */
        CCSDS_MsgFormat_GetSequence(BasePtr, &PreservedSeq);

        memset(BasePtr, 0, sizeof(*BasePtr));
    }

    /* Set the length fields in the primary header. */
    SysMsgSize = TotalLength;
    CCSDS_MsgFormat_SizeToMsgLength(&SysMsgSize, &MsgLen);
    CCSDS_MsgFormat_SetTotalSize(BasePtr, &MsgLen);

    /* Restore the sequence count if needed. */
    if (!Clear)
    {
        CCSDS_MsgFormat_SetSequence(BasePtr, &PreservedSeq);
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
int32 CCSDS_MsgFormat_GetMsgAddress(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_MsgAddress_t *MsgAddressBuf)
{
    uint32 LocalReg;

    LocalReg = BasePtr->JStreamId[0];

    /* Extract the CMD/TLM flag bits from this byte */
    if ((LocalReg & CCSDS_MSGFORMAT_STREAMID_TYPE_MASK) == CCSDS_MSGFORMAT_STREAMID_TYPE_TELEMETRY)
    {
        MsgAddressBuf->CmdFlag = 0;
    }
    else if ((LocalReg & CCSDS_MSGFORMAT_STREAMID_TYPE_MASK) == CCSDS_MSGFORMAT_STREAMID_TYPE_TELEMETRY)
    {
        MsgAddressBuf->CmdFlag = 1;
    }
    else
    {
        /* unrecognized first byte - bad version or other unacceptable value */
        return CFE_SB_NOT_IMPLEMENTED;
    }

    LocalReg <<= 8;
    LocalReg |= BasePtr->JStreamId[1];
    MsgAddressBuf->Apid = LocalReg & 0x07FF;

    return CFE_SUCCESS;
}

/**
 * Set the message ID (abstract SB endpoint) in the message header
 */
int32 CCSDS_MsgFormat_SetMsgAddress(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_MsgAddress_t *MsgAddressBuf)
{
    uint32 LocalReg;

    LocalReg = MsgAddressBuf->Apid;
    BasePtr->JStreamId[1] = LocalReg & 0xFF;
    LocalReg >>= 8;

    /* merge the CMD/TLM flag bits into the next byte */
    if (MsgAddressBuf->CmdFlag)
    {
        LocalReg |= CCSDS_MSGFORMAT_STREAMID_TYPE_COMMAND;
    }
    else
    {
        LocalReg |= CCSDS_MSGFORMAT_STREAMID_TYPE_TELEMETRY;
    }

    BasePtr->JStreamId[0] = (BasePtr->JStreamId[0] & 0xE0) | (LocalReg & 0x1F);

    return CFE_SUCCESS;
}

/**
 * Get the sequence number from the message header
 */
int32 CCSDS_MsgFormat_GetSequence(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Sequence_t *SequenceBuf)
{
    uint32 LocalReg;

    LocalReg = BasePtr->JSequence[0];
    LocalReg <<= 8;
    LocalReg |= BasePtr->JSequence[1];
    SequenceBuf->Number = LocalReg & 0x3FFF;

    return -1;
}

/**
 * Set the sequence number in the message header
 */
int32 CCSDS_MsgFormat_SetSequence(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Sequence_t *SequenceBuf)
{
    uint32 LocalReg;

    LocalReg = SequenceBuf->Number;
    BasePtr->JSequence[1] = LocalReg & 0xFF;
    LocalReg >>= 8;
    BasePtr->JSequence[0] = (BasePtr->JStreamId[0] & ~0x3F) | (LocalReg & 0x3F);

    return CFE_SUCCESS;
}


/**
 * Get the size of the header data.
 * This is not settable, it is fixed based on the message header format and type of message.
 *
 * This outputs the size of the actual header data, not including any alignment padding.
 */
int32 CCSDS_MsgFormat_GetHeaderLength(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 *SizeBuf)
{
    if (CCSDS_MsgFormat_GetIsCommand(BasePtr))
    {
        *SizeBuf = sizeof(CCSDS_MsgFormat_CommandPacket_t);
    }
    else if (CCSDS_MsgFormat_GetIsTelemetry(BasePtr))
    {
        *SizeBuf = sizeof(CCSDS_MsgFormat_TelemetryPacket_t);
    }
    else
    {
        /* fallback - should not be possible in CFE */
        *SizeBuf = sizeof(CCSDS_MsgFormat_BaseHeader_t);
    }

    return CFE_SUCCESS;
}


/**
 * Get the offset of the payload data.
 * This is not settable, it is fixed based on the message header format and type of message.
 *
 * Note this is not necessarily the same as "GetHdrSize", as padding may need to exist
 * between the header and payload to satisfy local CPU architecture requirements.
 *
 * This outputs the offset at which the actual payload data starts within the data structure.
 */
int32 CCSDS_MsgFormat_GetPayloadOffset(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 *OffsetBuf)
{
    return -1;
}


/**
 * Get the size of the payload data.
 *
 * This outputs the size of the actual payload data, not including any alignment padding.
 */
int32 CCSDS_MsgFormat_GetPayloadLength(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 *SizeBuf)
{
    return -1;
}


/**
 * Set the size of the payload data.
 *
 * This should be the size of the actual payload data, not including any alignment padding
 */
int32 CCSDS_MsgFormat_SetPayloadLength(CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 Size)
{
    return -1;
}

int32 CCSDS_MsgFormat_GetPayloadArea(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 *PayloadOffsetBuf, uint32 *PayloadLenBuf)
{
    return -1;
}


/**
 * Get the total size of the complete message.
 *
 * This is the total size including all padding, from start of header to the end of the trailer.
 * It should reflect the sizeof() of the C structure that represents the entire message
 */
int32 CCSDS_MsgFormat_GetTotalSize(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Length_t *SizeBuf)
{
    uint32 LocalReg;

    LocalReg = BasePtr->JLength[0];
    LocalReg <<= 8;
    LocalReg |= BasePtr->JLength[1];
    SizeBuf->ExtraByteCount = LocalReg;

    return CFE_SUCCESS;
}


/**
 * Set the size of the payload data.
 *
 * This is the total size including all padding, from start of header to the end of the trailer.
 * It should reflect the sizeof() of the C structure that represents the entire message
 */
int32 CCSDS_MsgFormat_SetTotalSize(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Length_t *SizeBuf)
{
    uint32 LocalReg;

    LocalReg = SizeBuf->ExtraByteCount;
    BasePtr->JLength[1] = LocalReg & 0xFF;
    LocalReg >>= 8;
    BasePtr->JLength[0] = LocalReg & 0xFF;

    return CFE_SUCCESS;
}


/**
 * Get the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this gets the timestamp.
 * Returns an error code if message format does not contain a timestamp.
 */
int32 CCSDS_MsgFormat_GetTlmTimestamp(const CCSDS_MsgFormat_TlmSecHdr_t *TlmPtr, CCSDS_MsgFormat_Time_t *TimeBuf)
{
    uint32 LocalReg;

    LocalReg = TlmPtr->JTime[0];
    LocalReg <<= 8;
    LocalReg |= TlmPtr->JTime[1];
    LocalReg <<= 8;
    LocalReg |= TlmPtr->JTime[2];
    LocalReg <<= 8;
    LocalReg |= TlmPtr->JTime[3];
    TimeBuf->Seconds    = LocalReg;

    LocalReg = TlmPtr->JTime[4];
    LocalReg <<= 8;
    LocalReg |= TlmPtr->JTime[5];
    TimeBuf->Subseconds16    = LocalReg;

    return CFE_SUCCESS;
}


/**
 * Get the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this gets the timestamp.
 * Returns an error code if message format does not contain a timestamp.
 */
int32 CCSDS_MsgFormat_GetTimestamp(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Time_t *TimeBuf)
{
    const CCSDS_MsgFormat_TelemetryPacket_t* TlmMsg;

    /* Check that the msg is a TLM type */
    if (!CCSDS_MsgFormat_GetIsTelemetry(BasePtr))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    TlmMsg = (const CCSDS_MsgFormat_TelemetryPacket_t*)BasePtr;

    return CCSDS_MsgFormat_GetTlmTimestamp(&TlmMsg->Sec, TimeBuf);
}

/**
 * Set the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this sets the field to the given value
 * Returns an error code if message format does not contain a timestamp.
 */
int32 CCSDS_MsgFormat_SetTlmTimestamp(CCSDS_MsgFormat_TlmSecHdr_t *TlmPtr, const CCSDS_MsgFormat_Time_t *Time)
{
    uint32 LocalReg;

    LocalReg = Time->Subseconds16;
    TlmPtr->JTime[5] = LocalReg & 0xFF;
    LocalReg >>= 8;
    TlmPtr->JTime[4] = LocalReg & 0xFF;

    LocalReg = Time->Seconds;
    TlmPtr->JTime[3] = LocalReg & 0xFF;
    LocalReg >>= 8;
    TlmPtr->JTime[2] = LocalReg & 0xFF;
    LocalReg >>= 8;
    TlmPtr->JTime[1] = LocalReg & 0xFF;
    LocalReg >>= 8;
    TlmPtr->JTime[0] = LocalReg & 0xFF;

    return CFE_SUCCESS;
}

/**
 * Set the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this sets the field to the given value
 * Returns an error code if message format does not contain a timestamp.
 */
int32 CCSDS_MsgFormat_SetTimestamp(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Time_t *Time)
{
    CCSDS_MsgFormat_TelemetryPacket_t* TlmMsg;

    /* Check that the msg is a TLM type */
    if (!CCSDS_MsgFormat_GetIsTelemetry(BasePtr))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    TlmMsg = (CCSDS_MsgFormat_TelemetryPacket_t*)BasePtr;

    return CCSDS_MsgFormat_SetTlmTimestamp(&TlmMsg->Sec, Time);
}

/**
 * Get the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 CCSDS_MsgFormat_GetCmdFunctionCode(const CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, CCSDS_MsgFormat_FunctionCode_t *CodeBuf)
{
    CodeBuf->Code = CmdPtr->JFunctionCode & 0x7F;
    return CFE_SUCCESS;
}

/**
 * Get the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 CCSDS_MsgFormat_GetFunctionCode(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_FunctionCode_t *CodeBuf)
{
    CCSDS_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(BasePtr))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (CCSDS_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_GetCmdFunctionCode(&CmdMsg->Sec, CodeBuf);
}

/**
 * Set the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 CCSDS_MsgFormat_SetCmdFunctionCode(CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, const CCSDS_MsgFormat_FunctionCode_t *CodeBuf)
{
    CmdPtr->JFunctionCode = (CmdPtr->JFunctionCode & 0x80) | (CodeBuf->Code & 0x7F);
    return CFE_SUCCESS;
}


/**
 * Set the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 CCSDS_MsgFormat_SetFunctionCode(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_FunctionCode_t *CodeBuf)
{
    CCSDS_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(BasePtr))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (CCSDS_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_SetCmdFunctionCode(&CmdMsg->Sec, CodeBuf);
}


CCSDS_MsgFormat_Checksum_t CCSDS_MsgFormat_ComputeChecksum_Impl(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 ChecksumOffset)
{
    const uint8 *PacketData;
    CCSDS_MsgFormat_Length_t LengthBuf;
    uint32  TotalLength;
    uint32  Pos;
    uint8   Byte;
    CCSDS_MsgFormat_Checksum_t   Val;

    PacketData = (const uint8*)BasePtr;
    Val.Checksum = 0xFF;

    if (CCSDS_MsgFormat_GetTotalSize(BasePtr, &LengthBuf) == CFE_SUCCESS)
    {
        CCSDS_MsgFormat_MsgLengthToSize(&LengthBuf, &TotalLength);
    }
    else
    {
        TotalLength = 0;
    }

    for (Pos = 0; Pos < TotalLength; ++Pos, ++PacketData)
    {
        /*
         * When computing the checksum value,
         * pad the checksum field itself with a zero.
         */
        if (Pos == ChecksumOffset)
        {
            Byte = 0;
        }
        else
        {
            Byte = *PacketData;
        }

        Val.Checksum ^= Byte;
    }

    return Val;
}

/**
 * Reads the error control / checksum command field
 */
int32 CCSDS_MsgFormat_GetCmdChecksum(const CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, CCSDS_MsgFormat_Checksum_t *ChecksumBuf)
{
    ChecksumBuf->Checksum = CmdPtr->JChecksum;
    return CFE_SUCCESS;
}

/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 CCSDS_MsgFormat_GetChecksum(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Checksum_t *ChecksumBuf)
{
    const CCSDS_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(BasePtr))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (const CCSDS_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_GetCmdChecksum(&CmdMsg->Sec, ChecksumBuf);
}

/**
 * Reads the error control / checksum field of the command message
 */
int32 CCSDS_MsgFormat_SetCmdChecksum(CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, const CCSDS_MsgFormat_Checksum_t *ChecksumBuf)
{
    CmdPtr->JChecksum = ChecksumBuf->Checksum;
    return CFE_SUCCESS;
}

/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 CCSDS_MsgFormat_SetChecksum(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Checksum_t *ChecksumBuf)
{
    CCSDS_MsgFormat_CommandPacket_t* CmdMsg;

    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(BasePtr))
    {
        return CFE_SB_WRONG_MSG_TYPE;
    }

    CmdMsg = (CCSDS_MsgFormat_CommandPacket_t*)BasePtr;

    return CCSDS_MsgFormat_SetCmdChecksum(&CmdMsg->Sec, ChecksumBuf);
}


/**
 * Compute the error control / checksum field, if applicable
 */
int32 CCSDS_MsgFormat_GetComputedChecksum(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Checksum_t *ChecksumBuf)
{
    /* Check that the msg is a CMD type */
    if (!CCSDS_MsgFormat_GetIsCommand(BasePtr))
    {
        /* no checksum field, nothing to do */
        return CFE_SB_WRONG_MSG_TYPE;
    }

    *ChecksumBuf = CCSDS_MsgFormat_ComputeChecksum_Impl(BasePtr,
            offsetof(CCSDS_MsgFormat_CommandPacket_t, Sec.JChecksum));

    return CFE_SUCCESS;
}

