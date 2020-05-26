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

#ifndef INCLUDE_JPH_EXTHDR_MSGFORMAT_ACCESSORS_H
#define INCLUDE_JPH_EXTHDR_MSGFORMAT_ACCESSORS_H

/*
 * the SB "extern typedefs" defines the CFE_SB_MsgAddress_t type, among others.
 * This header should have minimal external dependencies.
 */
#include "cfe_sb_extern_typedefs.h"

/*
 * The "types" header defines the data structures specific to this format.
 */
#include "exthdr_msgformat_types.h"
#include "ccsds_msgformat_accessors.h"

/*
 * The "cfe_sb.h" file defines the header structures actually used in the CFE software bus.
 * In particular it provides the "ExtHdr_MsgFormat_BaseHeader_t" type, which may in turn include
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
static inline void ExtHdr_MsgFormat_AddressToMsgId(const ExtHdr_MsgFormat_MsgAddress_t *MsgAddressBuf, CFE_SB_MsgId_t *MsgIdBuf)
{
    CFE_SB_MsgId_Atom_t MsgIdVal;

    /*
     * In order to keep within the original 16 bit size, the MsgId bits are organized as:
     *  - 0x00FF - Bits [7:0] of APID from primary (base) header
     *  - 0x0F00 - Bits [3:0] of SubSystemId from APIDQ (extended) header
     *  - 0x1000 - Command (1) / Telemetry (0) flag bit.  Kept in the "traditional" location for now.
     *  - 0xE000 - Bits [6:4] of SubSystemId from APIDQ (extended) header
     *
     * The "SystemId" bits are not currently used, but could be used as needed.
     */

    MsgIdVal =  MsgAddressBuf->Base.Apid & 0x7FF;
    MsgIdVal |= ((CFE_SB_MsgId_Atom_t)MsgAddressBuf->SubSysId & 0x0F) << 8;
    MsgIdVal |= ((CFE_SB_MsgId_Atom_t)MsgAddressBuf->Base.CmdFlag & 0x01) << 12;
    MsgIdVal |= ((CFE_SB_MsgId_Atom_t)MsgAddressBuf->SubSysId & 0x70) << 9;

    *MsgIdBuf = CFE_SB_ValueToMsgId(MsgIdVal);
}

/* Conversion of CFE_SB_MsgId_t to message address/APID field */
static inline void ExtHdr_MsgFormat_MsgIdToAddress(const CFE_SB_MsgId_t *MsgIdBuf, ExtHdr_MsgFormat_MsgAddress_t *MsgAddressBuf)
{
    CFE_SB_MsgId_Atom_t MsgIdVal;

    MsgIdVal = CFE_SB_MsgIdToValue(*MsgIdBuf);

    MsgAddressBuf->Base.Apid = MsgIdVal & 0x7FF;
    MsgAddressBuf->Base.CmdFlag = (MsgIdVal >> 12) & 0x01;
    MsgAddressBuf->SubSysId = (MsgIdVal >> 8) & 0x0F;
    MsgAddressBuf->SubSysId |= (MsgIdVal >> 9) & 0x70;
    MsgAddressBuf->SystemId = 0;    /* not in MsgId for now */
}

/* Convert the message length into a CFE abstract value */
static inline void ExtHdr_MsgFormat_MsgLengthToSize(const ExtHdr_MsgFormat_Length_t *MsgLengthBuf, uint32 *SysSizeBuf)
{
    /* pass through */
    CCSDS_MsgFormat_MsgLengthToSize(MsgLengthBuf, SysSizeBuf);
}

static inline void ExtHdr_MsgFormat_SizeToMsgLength(const uint32 *SysSizeBuf, ExtHdr_MsgFormat_Length_t *MsgLengthBuf)
{
    CCSDS_MsgFormat_SizeToMsgLength(SysSizeBuf, MsgLengthBuf);
}

/* Convert the message sequence number into a CFE abstract value (pass through) */
static inline void ExtHdr_MsgFormat_MsgSequenceToValue(const ExtHdr_MsgFormat_Sequence_t *MsgSeqBuf, uint32 *ValueBuf)
{
    CCSDS_MsgFormat_MsgSequenceToValue(MsgSeqBuf, ValueBuf);
}

static inline void ExtHdr_MsgFormat_ValueToMsgSequence(const uint32 *ValueBuf, ExtHdr_MsgFormat_Sequence_t *MsgSeqBuf)
{
    CCSDS_MsgFormat_ValueToMsgSequence(ValueBuf, MsgSeqBuf);
}

/* Convert the message time into a CFE time representation */
static inline void ExtHdr_MsgFormat_MsgTimeToSysTime(const ExtHdr_MsgFormat_Time_t *MsgTimeBuf, CFE_TIME_SysTime_t *SysTimeBuf)
{
    CCSDS_MsgFormat_MsgTimeToSysTime(MsgTimeBuf, SysTimeBuf);
}

static inline void ExtHdr_MsgFormat_SysTimeToMsgTime(const CFE_TIME_SysTime_t *SysTimeBuf, ExtHdr_MsgFormat_Time_t *MsgTimeBuf)
{
    CCSDS_MsgFormat_SysTimeToMsgTime(SysTimeBuf, MsgTimeBuf);
}

/* Convert the message function code into a CFE abstract value (pass through) */
static inline void ExtHdr_MsgFormat_MsgFunctionCodeToValue(const ExtHdr_MsgFormat_FunctionCode_t *MsgFunctionCodeBuf, uint16 *ValueBuf)
{
    CCSDS_MsgFormat_MsgFunctionCodeToValue(MsgFunctionCodeBuf, ValueBuf);
}

static inline void ExtHdr_MsgFormat_ValueToMsgFunctionCode(const uint16 *ValueBuf, ExtHdr_MsgFormat_FunctionCode_t *MsgFunctionCodeBuf)
{
    CCSDS_MsgFormat_ValueToMsgFunctionCode(ValueBuf, MsgFunctionCodeBuf);
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
static inline int32 ExtHdr_MsgFormat_SequenceDifference(const ExtHdr_MsgFormat_Sequence_t *SequenceBuf1, const ExtHdr_MsgFormat_Sequence_t *SequenceBuf2)
{
    return CCSDS_MsgFormat_SequenceDifference(SequenceBuf1, SequenceBuf2);
}

/**
 * Check if two checksum values are equal
 * One value is typically from the field within the packet, the other value is calculated from message content
 *
 * @returns true if the two checksum values are equal
 */
static inline bool ExtHdr_MsgFormat_ChecksumCompare(const ExtHdr_MsgFormat_Checksum_t *CheckBuf1, const ExtHdr_MsgFormat_Checksum_t *CheckBuf2)
{
    return CCSDS_MsgFormat_ChecksumCompare(CheckBuf1, CheckBuf2);
}

/**
 * Convert error control field value to uint16
 *
 * This is for backward compatibility with the CFE API that represents it this way.
 * There is no reverse operation from a uint16.
 *
 * @returns the error control field info as a uint16.
 */
static inline uint16 ExtHdr_MsgFormat_MsgChecksumToValue(const ExtHdr_MsgFormat_Checksum_t *CheckBuf)
{
    return CCSDS_MsgFormat_MsgChecksumToValue(CheckBuf);
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
static inline bool ExtHdr_MsgFormat_GetIsTelemetry(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr)
{
    return CCSDS_MsgFormat_GetIsTelemetry(&BasePtr->Base);
}

/**
 * Quickly determine whether a given message buffer is a command type.
 * @returns true if buffer represents a command message with a CMD header.
 */
static inline bool ExtHdr_MsgFormat_GetIsCommand(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr)
{
    return CCSDS_MsgFormat_GetIsCommand(&BasePtr->Base);
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
int32 ExtHdr_MsgFormat_SetInitialFields(ExtHdr_MsgFormat_BaseHeader_t *BasePtr,
        uint32         TotalLength,
        bool           Clear);

/**
 * Get the message ID (abstract SB endpoint) from the message header
 */
int32 ExtHdr_MsgFormat_GetMsgAddress(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_MsgAddress_t *MsgAddressBuf);

/**
 * Set the message ID (abstract SB endpoint) in the message header
 */
int32 ExtHdr_MsgFormat_SetMsgAddress(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_MsgAddress_t *MsgAddressBuf);

/**
 * Get the size of the header data.
 * This is not settable, it is fixed based on the message header format and type of message.
 *
 * This outputs the size of the actual header data, not including any alignment padding.
 */
int32 ExtHdr_MsgFormat_GetHeaderLength(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, uint32 *SizeBuf);


/**
 * Get the total size of the complete message.
 *
 * This is the total size including all padding, from start of header to the end of the trailer.
 * It should reflect the sizeof() of the C structure that represents the entire message
 */
static inline int32 ExtHdr_MsgFormat_GetTotalSize(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Length_t *SizeBuf)
{
    /* pass through to basic framing */
    return CCSDS_MsgFormat_GetTotalSize(&BasePtr->Base, SizeBuf);
}

/**
 * Set the size of the payload data.
 *
 * This is the total size including all padding, from start of header to the end of the trailer.
 * It should reflect the sizeof() of the C structure that represents the entire message
 */
static inline int32 ExtHdr_MsgFormat_SetTotalSize(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_Length_t *SizeBuf)
{
    /* pass through to basic framing */
    return CCSDS_MsgFormat_SetTotalSize(&BasePtr->Base, SizeBuf);
}

/**
 * Get the sequence number from the message header
 */
static inline int32 ExtHdr_MsgFormat_GetSequence(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Sequence_t *SequenceBuf)
{
    /* pass through to basic framing */
    return CCSDS_MsgFormat_GetSequence(&BasePtr->Base, SequenceBuf);
}

/**
 * Set the sequence number in the message header
 */
static inline int32 ExtHdr_MsgFormat_SetSequence(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_Sequence_t *SequenceBuf)
{
    /* pass through to basic framing */
    return CCSDS_MsgFormat_SetSequence(&BasePtr->Base, SequenceBuf);
}


/**
 * Get the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this gets the timestamp.
 * Returns an error code if message format does not contain a timestamp.
 */
int32 ExtHdr_MsgFormat_GetTimestamp(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Time_t *TimeBuf);

/**
 * Set the timestamp of the message, if applicable.
 *
 * If the message header contains a timestamp, this sets the field to the given value
 * Returns an error code if message format does not contain a timestamp.
 */
int32 ExtHdr_MsgFormat_SetTimestamp(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_Time_t *TimeBuf);

/**
 * Get the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 ExtHdr_MsgFormat_GetFunctionCode(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_FunctionCode_t *CodeBuf);

/**
 * Set the Function Code of the message, if applicable.
 *
 * If the message header contains a function or command code number, this sets the field to the given value
 * Returns an error code if message format does not contain a function code.
 */
int32 ExtHdr_MsgFormat_SetFunctionCode(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_FunctionCode_t *CodeBuf);

/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 ExtHdr_MsgFormat_GetChecksum(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Checksum_t *ChecksumBuf);

/**
 * Reads the error control / checksum field, if applicable
 *
 * If the message header contains an error control field, this retrieves the value of that field
 * Returns an error code if message format does not contain an error control field.
 */
int32 ExtHdr_MsgFormat_SetChecksum(ExtHdr_MsgFormat_BaseHeader_t *BasePtr, const ExtHdr_MsgFormat_Checksum_t *ChecksumBuf);

/**
 * Compute the error control / checksum field, if applicable
 */
int32 ExtHdr_MsgFormat_GetComputedChecksum(const ExtHdr_MsgFormat_BaseHeader_t *BasePtr, ExtHdr_MsgFormat_Checksum_t *ChecksumBuf);


#endif /* INCLUDE_JPH_EXTHDR_MSGFORMAT_ACCESSORS_H */
