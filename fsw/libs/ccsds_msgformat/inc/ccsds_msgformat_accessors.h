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

#ifndef INCLUDE_CCSDS_MSGFORMAT_ACCESSORS_H
#define INCLUDE_CCSDS_MSGFORMAT_ACCESSORS_H

/*
 * the SB "extern typedefs" defines the CFE_SB_MsgAddress_t type, among others.
 * This header should have minimal external dependencies.
 */
#include "cfe_sb_extern_typedefs.h"

/*
 * The "types" header defines the data structures specific to this format.
 */
#include "ccsds_msgformat_types.h"

/*
 * The "cfe_sb.h" file defines the header structures actually used in the CFE software bus.
 * In particular it provides the "CCSDS_MsgFormat_BaseHeader_t" type, which may in turn include
 * structures defined by the types header above.
 */
#include "cfe_sb.h"


/* ------------------------------------------------------------------------------
 *
 * FIELD CONVERSION HELPERS
 *
 * These are invoked from CFE/FSW to convert between the abstract CFE representation of
 * various field values and the representation defined/required by the message format
 *
 * Standard fields required by CFE:
 *  - MsgId:        Address/endpoint/route designation, CFE_SB_MsgId_t
 *  - Size:         Total size of the entire packet structure in memory, uint32 value
 *  - Sequence:     Sequential packet counter, uint32 value
 *  - Time:         Timestamp, Telemetry packets only, CFE_TIME_SysTime_t
 *  - FunctionCode: command/function identifier, Command packets only, uint16 value
 *  - Checksum:     Error control field.  Historically a uint16 in CFE SB API, but should
 *                  be a completely abstract type to allow arbitrary error control data.
 *
 * ------------------------------------------------------------------------------ */


/* Conversion of message address/APID field to a CFE_SB_MsgId_t */
static inline void CCSDS_MsgFormat_AddressToMsgId(const CCSDS_MsgFormat_MsgAddress_t *MsgAddressBuf, CFE_SB_MsgId_t *MsgIdBuf)
{
    /* for backward compatibility, reconstruct the CFE_SB_MsgId_t number value with the bit meanings the same as CCSDSv1 */
    CFE_SB_MsgId_Atom_t MsgIdVal = (MsgAddressBuf->CmdFlag << 12) | (MsgAddressBuf->Apid & 0x7FF);
    *MsgIdBuf = CFE_SB_ValueToMsgId(MsgIdVal);
}

/* Conversion of CFE_SB_MsgId_t to message address/APID field */
static inline void CCSDS_MsgFormat_MsgIdToAddress(const CFE_SB_MsgId_t *MsgIdBuf, CCSDS_MsgFormat_MsgAddress_t *MsgAddressBuf)
{
    /* for backward compatibility, deconstruct the CFE_SB_MsgId_t number value with the bit meanings the same as CCSDSv1 */
    CFE_SB_MsgId_Atom_t MsgIdVal = CFE_SB_MsgIdToValue(*MsgIdBuf);
    MsgAddressBuf->Apid = (MsgIdVal & 0x7FF);
    MsgAddressBuf->CmdFlag = (MsgIdVal >> 12) & 0x1;
}

/* Convert the message length into a CFE abstract value */
static inline void CCSDS_MsgFormat_MsgLengthToSize(const CCSDS_MsgFormat_Length_t *MsgLengthBuf, uint32 *SysSizeBuf)
{
    *SysSizeBuf = MsgLengthBuf->ExtraByteCount + 7;
}

static inline void CCSDS_MsgFormat_SizeToMsgLength(const uint32 *SysSizeBuf, CCSDS_MsgFormat_Length_t *MsgLengthBuf)
{
    MsgLengthBuf->ExtraByteCount = *SysSizeBuf - 7;
}

/* Convert the message sequence number into a CFE abstract value (pass through) */
static inline void CCSDS_MsgFormat_MsgSequenceToValue(const CCSDS_MsgFormat_Sequence_t *MsgSeqBuf, uint32 *ValueBuf)
{
    *ValueBuf = MsgSeqBuf->Number;
}

static inline void CCSDS_MsgFormat_ValueToMsgSequence(const uint32 *ValueBuf, CCSDS_MsgFormat_Sequence_t *MsgSeqBuf)
{
    MsgSeqBuf->Number = *ValueBuf;
}

/* Convert the message time into a CFE time representation */
static inline void CCSDS_MsgFormat_MsgTimeToSysTime(const CCSDS_MsgFormat_Time_t *MsgTimeBuf, CFE_TIME_SysTime_t *SysTimeBuf)
{
    SysTimeBuf->Seconds = MsgTimeBuf->Seconds;
    SysTimeBuf->Subseconds = MsgTimeBuf->Subseconds16 << 16;
}

static inline void CCSDS_MsgFormat_SysTimeToMsgTime(const CFE_TIME_SysTime_t *SysTimeBuf, CCSDS_MsgFormat_Time_t *MsgTimeBuf)
{
    MsgTimeBuf->Seconds = SysTimeBuf->Seconds;
    MsgTimeBuf->Subseconds16 = SysTimeBuf->Subseconds >> 16;
}

/* Convert the message function code into a CFE abstract value (pass through) */
static inline void CCSDS_MsgFormat_MsgFunctionCodeToValue(const CCSDS_MsgFormat_FunctionCode_t *MsgFunctionCodeBuf, uint16 *ValueBuf)
{
    *ValueBuf = MsgFunctionCodeBuf->Code;
}

static inline void CCSDS_MsgFormat_ValueToMsgFunctionCode(const uint16 *ValueBuf, CCSDS_MsgFormat_FunctionCode_t *MsgFunctionCodeBuf)
{
    MsgFunctionCodeBuf->Code = *ValueBuf;
}



/**
 * Calculate the difference between two sequence number values
 * This takes into account the increment+modulo behavior of the sequence number field.
 *
 * When applied to two equal sequence numbers, the result is 0.
 * If SequenceBuf1 represents 1 greater than SequenceBuf2, the result is 1.
 *
 * The result is positive if the SequenceBuf1 value is sequentially greater than SequenceBuf2.
 * The result is negative if the SequenceBuf1 value is sequentially less than SequenceBuf2.
 * (this is not a simple comparison, as the modulo/wrap-around has to be considered).
 *
 * @returns the difference between the two sequence numbers as a signed int
 */
static inline int32 CCSDS_MsgFormat_SequenceDifference(const CCSDS_MsgFormat_Sequence_t *SequenceBuf1, const CCSDS_MsgFormat_Sequence_t *SequenceBuf2)
{
    uint32 diff = SequenceBuf1->Number - SequenceBuf2->Number;
    int32 result;

    /* sign-extend the result */
    result = (diff & 0x1FFF);
    result -= (diff & 0x2000);

    return result;
}

/**
 * Check if two checksum values are equal
 * One value is typically from the field within the packet, the other value is calculated from message content
 *
 * @returns true if the two checksum values are equal
 */
static inline bool CCSDS_MsgFormat_ChecksumCompare(const CCSDS_MsgFormat_Checksum_t *CheckBuf1, const CCSDS_MsgFormat_Checksum_t *CheckBuf2)
{
    return CheckBuf1->Checksum == CheckBuf2->Checksum;
}

/**
 * Convert error control field value to uint16
 *
 * This is for backward compatibility with the CFE API that represents it this way.
 * There is no reverse operation from a uint16.
 *
 * @returns the error control field info as a uint16.
 */
static inline uint16 CCSDS_MsgFormat_MsgChecksumToValue(const CCSDS_MsgFormat_Checksum_t *CheckBuf)
{
    return CheckBuf->Checksum;
}


/* ------------------------------------------------------------------------------
 *
 * QUERY HELPERS
 *
 * These are invoked from CFE/FSW to query information about a given message.
 *
 * These are generally only "get" operations, as it is based on various other metadata
 * within the message, these fields cannot be set directly.
 *
 * ------------------------------------------------------------------------------ */

/**
 * Quickly determine whether a given message buffer is a telemetry type.
 * @returns true if buffer represents a telemetry message with a TLM header.
 */
static inline bool CCSDS_MsgFormat_GetIsTelemetry(const CCSDS_MsgFormat_BaseHeader_t *BasePtr)
{
    return ((BasePtr->JStreamId[0] & CCSDS_MSGFORMAT_STREAMID_TYPE_MASK) == CCSDS_MSGFORMAT_STREAMID_TYPE_TELEMETRY);
}

/**
 * Quickly determine whether a given message buffer is a command type.
 * @returns true if buffer represents a command message with a CMD header.
 */
static inline bool CCSDS_MsgFormat_GetIsCommand(const CCSDS_MsgFormat_BaseHeader_t *BasePtr)
{
    return ((BasePtr->JStreamId[0] & CCSDS_MSGFORMAT_STREAMID_TYPE_MASK) == CCSDS_MSGFORMAT_STREAMID_TYPE_COMMAND);
}


/* ------------------------------------------------------------------------------
 *
 * GETTER/SETTER HELPERS
 *
 * These are invoked from CFE/FSW to actually read/write the values within a
 * message structure.  The I/O buffer should be of the type prescribed by the
 * message format.  A separate conversion function (above) can translate to the
 * CFE abstract value where necessary.
 *
 * The return value should be an int32 status code, nominally CFE_SUCCESS.
 * ------------------------------------------------------------------------------ */

/**
 * Set all header fields in the message buffer to their initial state
 */
int32 CCSDS_MsgFormat_SetInitialFields(CCSDS_MsgFormat_BaseHeader_t *BasePtr,
        uint32         TotalLength,
        bool           Clear);

/**
 * Get the message ID (abstract SB endpoint) from the message header
 */
int32 CCSDS_MsgFormat_GetMsgAddress(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_MsgAddress_t *MsgAddressBuf);

/**
 * Set the message ID (abstract SB endpoint) in the message header
 */
int32 CCSDS_MsgFormat_SetMsgAddress(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_MsgAddress_t *MsgAddressBuf);

/**
 * Get the size of the header data.
 * This is not settable, it is fixed based on the message header format and type of message.
 *
 * This outputs the size of the actual header data, not including any alignment padding.
 */
int32 CCSDS_MsgFormat_GetHeaderLength(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 *SizeBuf);


/**
 * Get the total size of the complete message.
 *
 * This is the total size including all padding, from start of header to the end of the trailer.
 * It should reflect the sizeof() of the C structure that represents the entire message
 */
int32 CCSDS_MsgFormat_GetTotalSize(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Length_t *SizeBuf);

/**
 * Set the size of the payload data.
 *
 * This is the total size including all padding, from start of header to the end of the trailer.
 * It should reflect the sizeof() of the C structure that represents the entire message
 */
int32 CCSDS_MsgFormat_SetTotalSize(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Length_t *SizeBuf);

/**
 * Get the sequence number from the message header
 */
int32 CCSDS_MsgFormat_GetSequence(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Sequence_t *SequenceBuf);

/**
 * Set the sequence number in the message header
 */
int32 CCSDS_MsgFormat_SetSequence(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Sequence_t *SequenceBuf);


/**
 * Get the timestamp of the telemetry message
 */
int32 CCSDS_MsgFormat_GetTlmTimestamp(const CCSDS_MsgFormat_TlmSecHdr_t *TlmPtr, CCSDS_MsgFormat_Time_t *TimeBuf);

/**
 * Get the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this gets the timestamp.
 * Returns an error code if message format does not contain a timestamp.
 */
int32 CCSDS_MsgFormat_GetTimestamp(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Time_t *TimeBuf);

/**
 * Set the timestamp of the telemetry message
 */
int32 CCSDS_MsgFormat_SetTlmTimestamp(CCSDS_MsgFormat_TlmSecHdr_t *TlmPtr, const CCSDS_MsgFormat_Time_t *TimeBuf);

/**
 * Set the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this sets the field to the given value
 * Returns an error code if message format does not contain a timestamp.
 */
int32 CCSDS_MsgFormat_SetTimestamp(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Time_t *TimeBuf);

/**
 * Get the Function Code of the command message
 */
int32 CCSDS_MsgFormat_GetCmdFunctionCode(const CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, CCSDS_MsgFormat_FunctionCode_t *CodeBuf);

/**
 * Get the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 CCSDS_MsgFormat_GetFunctionCode(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_FunctionCode_t *CodeBuf);

/**
 * Set the Function Code of the command message
 */
int32 CCSDS_MsgFormat_SetCmdFunctionCode(CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, const CCSDS_MsgFormat_FunctionCode_t *CodeBuf);

/**
 * Set the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 CCSDS_MsgFormat_SetFunctionCode(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_FunctionCode_t *CodeBuf);

/**
 * Reads the error control / checksum field of the command message
 */
int32 CCSDS_MsgFormat_GetCmdChecksum(const CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, CCSDS_MsgFormat_Checksum_t *ChecksumBuf);

/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 CCSDS_MsgFormat_GetChecksum(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Checksum_t *ChecksumBuf);

/**
 * Reads the error control / checksum field of the command message
 */
int32 CCSDS_MsgFormat_SetCmdChecksum(CCSDS_MsgFormat_CmdSecHdr_t *CmdPtr, const CCSDS_MsgFormat_Checksum_t *ChecksumBuf);

/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 CCSDS_MsgFormat_SetChecksum(CCSDS_MsgFormat_BaseHeader_t *BasePtr, const CCSDS_MsgFormat_Checksum_t *ChecksumBuf);

/**
 * Compute the value for the error control / checksum field
 */
CCSDS_MsgFormat_Checksum_t CCSDS_MsgFormat_ComputeChecksum_Impl(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, uint32 ChecksumOffset);

/**
 * Compute the error control / checksum field, if applicable
 */
int32 CCSDS_MsgFormat_GetComputedChecksum(const CCSDS_MsgFormat_BaseHeader_t *BasePtr, CCSDS_MsgFormat_Checksum_t *ChecksumBuf);

#endif /* INCLUDE_CCSDS_MSGFORMAT_ACCESSORS_H */
