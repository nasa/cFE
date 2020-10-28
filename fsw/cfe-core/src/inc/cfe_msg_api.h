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
 * Message access APIs
 */

#ifndef _cfe_msg_api_
#define _cfe_msg_api_

/*
 * Includes
 */
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_msg_hdr.h"
#include "cfe_msg_typedefs.h"
#include "cfe_time.h"
#include "cfe_sb.h"

/** \defgroup CFEAPIMSGHeader cFE Message header APIs
 * \{
 */

/*****************************************************************************/
/**
 * \brief Initialize a message
 *
 * \par Description
 *          This routine initialize a message.  The entire message is
 *          set to zero (based on size), defaults are set, then the
 *          size and bits from MsgId are set.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       MsgId       MsgId that corresponds to message
 * \param[in]       Size        Total size of the mesage (used to set length field)
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_Init(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId, CFE_MSG_Size_t Size);

/*****************************************************************************/
/**
 * \brief Gets the total size of a message.
 *
 * \par Description
 *          This routine gets the total size of the message.
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[out] Size        Total message size
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetSize(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t *Size);

/*****************************************************************************/
/**
 * \brief Sets the total size of a message.
 *
 * \par Description
 *          This routine sets the total size of the message.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       Size        Total message size
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetSize(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t Size);

/*****************************************************************************/
/**
 * \brief Gets the message type.
 *
 * \par Description
 *          This routine gets the message type.
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[out] Type        Message type
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetType(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Type_t *Type);

/*****************************************************************************/
/**
 * \brief Sets the message type.
 *
 * \par Description
 *          This routine sets the message type.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       Type        Message type
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetType(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Type_t Type);

/*****************************************************************************/
/**
 * \brief Gets the message header version.
 *
 * \par Description
 *          This routine gets the message header version.
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[out] Version     Header version
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetHeaderVersion(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_HeaderVersion_t *Version);

/*****************************************************************************/
/**
 * \brief Sets the message header version.
 *
 * \par Description
 *          This routine sets the message header version. Typically only
 *          set within message initialization and not used by APPs.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       Version     Header version
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetHeaderVersion(CFE_MSG_Message_t *MsgPtr, CFE_MSG_HeaderVersion_t Version);

/*****************************************************************************/
/**
 * \brief Gets the message secondary header boolean
 *
 * \par Description
 *          This routine gets the message secondary header boolean.
 *
 * \param[in]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[out] HasSecondary Has secondary header flag
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetHasSecondaryHeader(const CFE_MSG_Message_t *MsgPtr, bool *HasSecondary);

/*****************************************************************************/
/**
 * \brief Sets the message secondary header boolean
 *
 * \par Description
 *          This routine sets the message has secondary header boolean. Typically only
 *          set within message initialization and not used by APPs.
 *
 * \param[in, out]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[in]       HasSecondary Has secondary header flag
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetHasSecondaryHeader(CFE_MSG_Message_t *MsgPtr, bool HasSecondary);

/*****************************************************************************/
/**
 * \brief Gets the message application ID
 *
 * \par Description
 *          This routine gets the message application ID.
 *
 * \param[in]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[out] ApId         Application ID
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetApId(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_ApId_t *ApId);

/*****************************************************************************/
/**
 * \brief Sets the message application ID
 *
 * \par Description
 *          This routine sets the message application ID. Typically set
 *          at initialization using the MsgId, but API available to set
 *          bits that may not be included in MsgId.
 *
 * \param[in, out]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[in]       ApId         Application ID
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetApId(CFE_MSG_Message_t *MsgPtr, CFE_MSG_ApId_t ApId);

/*****************************************************************************/
/**
 * \brief Gets the message segmentation flag
 *
 * \par Description
 *          This routine gets the message segmentation flag
 *
 * \param[in]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[out] SegFlag      Segmentation flag
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetSegmentationFlag(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_SegmentationFlag_t *SegFlag);

/*****************************************************************************/
/**
 * \brief Sets the message segmentation flag
 *
 * \par Description
 *          This routine sets the message segmentation flag.
 *
 * \param[in, out]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[in]       SegFlag      Segmentation flag
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetSegmentationFlag(CFE_MSG_Message_t *MsgPtr, CFE_MSG_SegmentationFlag_t SegFlag);

/*****************************************************************************/
/**
 * \brief Gets the message sequence count
 *
 * \par Description
 *          This routine gets the message sequence count.
 *
 * \param[in]    MsgPtr      A pointer to the buffer that contains the message.
 * \param[out]   SeqCnt      Sequence count
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetSequenceCount(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_SequenceCount_t *SeqCnt);

/*****************************************************************************/
/**
 * \brief Sets the message sequence count
 *
 * \par Description
 *          This routine sets the message sequence count.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       SeqCnt      Sequence count
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetSequenceCount(CFE_MSG_Message_t *MsgPtr, CFE_MSG_SequenceCount_t SeqCnt);

/*****************************************************************************/
/**
 * \brief Gets the message EDS version
 *
 * \par Description
 *          This routine gets the message EDS version.
 *
 * \param[in]    MsgPtr      A pointer to the buffer that contains the message.
 * \param[out]   Version     EDS Version
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetEDSVersion(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t *Version);

/*****************************************************************************/
/**
 * \brief Sets the message EDS version
 *
 * \par Description
 *          This routine sets the message EDS version.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       Version     EDS Version
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetEDSVersion(CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t Version);

/*****************************************************************************/
/**
 * \brief Gets the message endian
 *
 * \par Description
 *          This routine gets the message endian.
 *
 * \param[in]    MsgPtr      A pointer to the buffer that contains the message.
 * \param[out]   Endian      Endian
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetEndian(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t *Endian);

/*****************************************************************************/
/**
 * \brief Sets the message endian
 *
 * \par Description
 *          This routine sets the message endian.  Invalid endian selection
 *          will set big endian.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       Endian      Endian
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetEndian(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t Endian);

/*****************************************************************************/
/**
 * \brief Gets the message playback flag
 *
 * \par Description
 *          This routine gets the message playback flag.
 *
 * \param[in]    MsgPtr      A pointer to the buffer that contains the message.
 * \param[out]   PlayFlag    Playback Flag
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetPlaybackFlag(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t *PlayFlag);

/*****************************************************************************/
/**
 * \brief Sets the message playback flag
 *
 * \par Description
 *          This routine sets the message playback flag.
 *
 * \param[in, out]    MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]         PlayFlag    Playback Flag
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetPlaybackFlag(CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t PlayFlag);

/*****************************************************************************/
/**
 * \brief Gets the message subsystem
 *
 * \par Description
 *          This routine gets the message subsystem
 *
 * \param[in]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[out] Subsystem    Subsystem
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetSubsystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t *Subsystem);

/*****************************************************************************/
/**
 * \brief Sets the message subsystem
 *
 * \par Description
 *          This routine sets the message subsystem. Some bits may
 *          be set at initialization using the MsgId, but API available to set
 *          bits that may not be included in MsgId.
 *
 * \param[in, out]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[in]       Subsystem    Subsystem
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetSubsystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t Subsystem);

/*****************************************************************************/
/**
 * \brief Gets the message system
 *
 * \par Description
 *          This routine gets the message system id
 *
 * \param[in]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[out] System       System
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetSystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t *System);

/*****************************************************************************/
/**
 * \brief Sets the message system
 *
 * \par Description
 *          This routine sets the message system id. Some bits may
 *          be set at initialization using the MsgId, but API available to set
 *          bits that may not be included in MsgId.
 *
 * \param[in, out]  MsgPtr       A pointer to the buffer that contains the message.
 * \param[in]       System       System
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetSystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t System);

/*****************************************************************************/
/**
 * \brief Calculates and sets the checksum of a message
 *
 * \par Description
 *          This routine calculates the checksum of a message according
 *          to an implementation-defined algorithm.  Then, it sets the checksum field
 *          in the message with the calculated value.  The contents and location of
 *          this field will depend on the underlying implementation of
 *          messages.  It may be a checksum, a CRC, or some other algorithm.
 *
 * \par Assumptions, External Events, and Notes:
 *          - If the underlying implementation of messages does not
 *            include a checksum field, then this routine will return
 *            #CFE_MSG_WRONG_MSG_TYPE
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 * \retval #CFE_MSG_WRONG_MSG_TYPE  \copybrief CFE_MSG_WRONG_MSG_TYPE
 */
CFE_Status_t CFE_MSG_GenerateChecksum(CFE_MSG_Message_t *MsgPtr);

/*****************************************************************************/
/**
 * \brief Validates the checksum of a message.
 *
 * \par Description
 *          This routine validates the checksum of a message
 *          according to an implementation-defined algorithm.
 *
 * \par Assumptions, External Events, and Notes:
 *          - If the underlying implementation of messages does not
 *            include a checksum field, then this routine will return
 *            #CFE_MSG_WRONG_MSG_TYPE and set the IsValid parameter false.
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 *                         This must point to the first byte of the message header.
 * \param[out] IsValid     Checksum validation result
 *                         \arg true - valid
 *                         \arg false - invalid or not supported/implemented
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 * \retval #CFE_MSG_WRONG_MSG_TYPE  \copybrief CFE_MSG_WRONG_MSG_TYPE
 */
CFE_Status_t CFE_MSG_ValidateChecksum(const CFE_MSG_Message_t *MsgPtr, bool *IsValid);

/*****************************************************************************/
/**
 * \brief Sets the function code field in a message.
 *
 * \par Description
 *          This routine sets the function code of a message.
 *
 * \par Assumptions, External Events, and Notes:
 *          - If the underlying implementation of messages does not
 *            include a function code field, then this routine will do nothing to
 *            the message contents and will return #CFE_MSG_WRONG_MSG_TYPE.
 *
 * \param[in, out]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[in]       FcnCode     The function code to include in the message.
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS             \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT    \copybrief CFE_MSG_BAD_ARGUMENT
 * \retval #CFE_MSG_WRONG_MSG_TYPE  \copybrief CFE_MSG_WRONG_MSG_TYPE
 *
 */
CFE_Status_t CFE_MSG_SetFcnCode(CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t FcnCode);

/*****************************************************************************/
/**
 * \brief Gets the function code field from a message.
 *
 * \par Description
 *          This routine gets the function code from a message.
 *
 * \par Assumptions, External Events, and Notes:
 *          - If the underlying implementation of messages does not
 *            include a function code field, then this routine will
 *            set FcnCode to zero and return #CFE_MSG_WRONG_MSG_TYPE
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[out] FcnCode     The function code from the message
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT   \copybrief CFE_MSG_BAD_ARGUMENT
 * \retval #CFE_MSG_WRONG_MSG_TYPE \copybrief CFE_MSG_WRONG_MSG_TYPE
 */
CFE_Status_t CFE_MSG_GetFcnCode(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t *FcnCode);

/*****************************************************************************/
/**
 * \brief Gets the time field from a message.
 *
 * \par Description
 *          This routine gets the time from a message.
 *
 * \par Assumptions, External Events, and Notes:
 *          - If the underlying implementation of messages does not
 *            include a time field, then this routine will set Time
 *            to zero and return #CFE_MSG_WRONG_MSG_TYPE
 *          - Note default implementation of command messages do not have a time field.
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[out] Time        Time from the message
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT   \copybrief CFE_MSG_BAD_ARGUMENT
 * \retval #CFE_MSG_WRONG_MSG_TYPE \copybrief CFE_MSG_WRONG_MSG_TYPE
 */
CFE_Status_t CFE_MSG_GetMsgTime(const CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t *Time);

/*****************************************************************************/
/**
 * \brief Sets the time field in a message.
 *
 * \par Description
 *          This routine sets the time of a message.  Most applications
 *          will want to use #CFE_SB_TimeStampMsg instead of this function.  But,
 *          when needed, this API can be used to set multiple messages
 *          with identical time stamps.
 *
 * \par Assumptions, External Events, and Notes:
 *          - If the underlying implementation of messages does not include
 *            a time field, then this routine will do nothing to the message contents
 *            and will return #CFE_MSG_WRONG_MSG_TYPE.
 *          - Note default implementation of command messages do not have a time field.
 *
 * \param[in, out]  MsgPtr      A pointer to the message.
 * \param[in]       Time        The time to include in the message.  This will usually be a time
 *                              from #CFE_TIME_GetTime.
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT   \copybrief CFE_MSG_BAD_ARGUMENT
 * \retval #CFE_MSG_WRONG_MSG_TYPE \copybrief CFE_MSG_WRONG_MSG_TYPE
 */
CFE_Status_t CFE_MSG_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t Time);

/**\}*/

/** \defgroup CFEAPIMSGMsgId cFE Message Id APIs
 * \{
 */

/*****************************************************************************/
/**
 * \brief Gets the message id from a message.
 *
 * \par Description
 *        This routine gets the message id from a message.  The message id
 *        is a hash of bits in the message header, used by the software bus for
 *        routing.  Message id needs to be unique for each endpoint
 *        in the system.
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[out] MsgId       Message id
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT   \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetMsgId(const CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t *MsgId);

/*****************************************************************************/
/**
 * \brief Sets the message id bits in a message.
 *
 * \par Description
 *        This routine sets the message id bits in a message.  The message id
 *        is a hash of bits in the message header, used by the software bus for
 *        routing.  Message id needs to be unique for each endpoint
 *        in the system.
 * \par Note
 *        This API only sets the bits in the header that make up the message ID.
 *        No other values in the header are modified.
 *
 * \param[in]  MsgPtr      A pointer to the buffer that contains the message.
 * \param[out] MsgId       Message id
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT   \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_SetMsgId(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId);

/*****************************************************************************/
/**
 * \brief Gets message type using message ID
 *
 * \par Description
 *        This routine gets the message type using the message ID
 *
 * \param[in]  MsgId       Message id
 * \param[out] Type        Message type
 *
 * \return Execution status, see \ref CFEReturnCodes
 * \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
 * \retval #CFE_MSG_BAD_ARGUMENT   \copybrief CFE_MSG_BAD_ARGUMENT
 */
CFE_Status_t CFE_MSG_GetTypeFromMsgId(CFE_SB_MsgId_t MsgId, CFE_MSG_Type_t *Type);

/**\}*/

#endif /* _cfe_msg_api_ */
