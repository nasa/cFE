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
 * Purpose:
 *      This header file contains all definitions for the cFE Software Bus
 *      Application Programmer's Interface.
 *
 * Author:   R.McGraw/SSI
 *
 */

#ifndef CFE_SB_H
#define CFE_SB_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_es_api_typedefs.h"

/*
** Macro Definitions
*/
#define CFE_BIT(x)    (1 << (x))              /**< \brief Places a one at bit positions 0 - 31*/
#define CFE_SET(i, x) ((i) |= CFE_BIT(x))     /**< \brief Sets bit x of i */
#define CFE_CLR(i, x) ((i) &= ~CFE_BIT(x))    /**< \brief Clears bit x of i */
#define CFE_TST(i, x) (((i)&CFE_BIT(x)) != 0) /**< \brief true(non zero) if bit x of i is set */

/****************** Function Prototypes **********************/

/** @defgroup CFEAPISBPipe cFE Pipe Management APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Creates a new software bus pipe.
**
** \par Description
**          This routine creates and initializes an input pipe that the calling
**          application can use to receive software bus messages.  By default, no
**          messages are routed to the new pipe.  So, the application must use
**          #CFE_SB_Subscribe to specify which messages it wants to receive on
**          this pipe.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[out]  PipeIdPtr   A pointer to a variable of type #CFE_SB_PipeId_t @nonnull,
**                          which will be filled in with the pipe ID information
**                          by the #CFE_SB_CreatePipe routine. *PipeIdPtr is the identifier for the created pipe.
**
** \param[in]  Depth        The maximum number of messages that will be allowed on
**                          this pipe at one time.
**
** \param[in]  PipeName     A string @nonnull to be used to identify this pipe in error messages
**                          and routing information telemetry.  The string must be no
**                          longer than #OS_MAX_API_NAME (including terminator).
**                          Longer strings will be truncated.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MAX_PIPES_MET \copybrief CFE_SB_MAX_PIPES_MET
** \retval #CFE_SB_PIPE_CR_ERR   \copybrief CFE_SB_PIPE_CR_ERR
**
** \sa #CFE_SB_DeletePipe #CFE_SB_GetPipeOpts #CFE_SB_SetPipeOpts #CFE_SB_GetPipeIdByName
**/
CFE_Status_t CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth, const char *PipeName);

/*****************************************************************************/
/**
** \brief Delete a software bus pipe.
**
** \par Description
**          This routine deletes an input pipe and cleans up all data structures
**          associated with the pipe.  All subscriptions made for this pipe by
**          calls to #CFE_SB_Subscribe will be automatically removed from the
**          SB routing tables.  Any messages in the pipe will be discarded.
**
**          Applications should not call this routine for all of their
**          SB pipes as part of their orderly shutdown process, as the
**          pipe will be deleted by the support framework at the
**          appropriate time.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  PipeId       The pipe ID (obtained previously from #CFE_SB_CreatePipe)
**                          of the pipe to be deleted.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_CreatePipe #CFE_SB_GetPipeOpts #CFE_SB_SetPipeOpts #CFE_SB_GetPipeIdByName
**/
CFE_Status_t CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId);

/**
 * @brief Obtain an index value correlating to an SB Pipe ID
 *
 * This calculates a zero based integer value that may be used for indexing
 * into a local resource table/array.
 *
 * Index values are only guaranteed to be unique for resources of the same
 * type.  For instance, the indices corresponding to two [valid] application
 * IDs will never overlap, but the index of a pipe ID and an app ID
 * may be the same.  Furthermore, indices may be reused if a resource is
 * deleted and re-created.
 *
 * @note There is no inverse of this function - indices cannot be converted
 * back to the original PipeID value.  The caller should retain the original ID
 * for future use.
 *
 * @param[in]   PipeID  Pipe ID to convert
 * @param[out]  Idx    Buffer where the calculated index will be stored @nonnull
 *
 * @return Execution status, see @ref CFEReturnCodes
 * @retval #CFE_SUCCESS                      @copybrief CFE_SUCCESS
 * @retval #CFE_ES_ERR_RESOURCEID_NOT_VALID  @copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
 */
CFE_Status_t CFE_SB_PipeId_ToIndex(CFE_SB_PipeId_t PipeID, uint32 *Idx);

/*****************************************************************************/
/**
** \brief Set options on a pipe.
**
** \par Description
**          This routine sets (or clears) options to alter the pipe's behavior.
**          Options are (re)set every call to this routine.
**
** \param[in]  PipeId       The pipe ID of the pipe to set options on.
**
** \param[in]  Opts         A bit field of options: \ref CFESBPipeOptions
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_CreatePipe #CFE_SB_DeletePipe #CFE_SB_GetPipeOpts #CFE_SB_GetPipeIdByName #CFE_SB_PIPEOPTS_IGNOREMINE
**/
CFE_Status_t CFE_SB_SetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 Opts);

/*****************************************************************************/
/**
** \brief Get options on a pipe.
**
** \par Description
**          This routine gets the current options on a pipe.
**
** \param[in]  PipeId       The pipe ID of the pipe to get options from.
**
** \param[out] OptsPtr     A bit field of options: \ref CFESBPipeOptions  @nonnull
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_CreatePipe #CFE_SB_DeletePipe #CFE_SB_SetPipeOpts #CFE_SB_GetPipeIdByName #CFE_SB_PIPEOPTS_IGNOREMINE
**/
CFE_Status_t CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptsPtr);

/*****************************************************************************/
/**
** \brief Get the pipe name for a given id.
**
** \par Description
**          This routine finds the pipe name for a pipe id.
**
** \param[out] PipeNameBuf  The buffer to receive the pipe name @nonnull.
**
** \param[in] PipeNameSize  The size (in chars) of the PipeName buffer @nonzero.
**
** \param[in] PipeId        The PipeId for that name.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_CreatePipe #CFE_SB_DeletePipe #CFE_SB_SetPipeOpts #CFE_SB_GetPipeIdByName
**/
CFE_Status_t CFE_SB_GetPipeName(char *PipeNameBuf, size_t PipeNameSize, CFE_SB_PipeId_t PipeId);

/*****************************************************************************/
/**
** \brief Get pipe id by pipe name.
**
** \par Description
**          This routine finds the pipe id for a pipe name.
**
** \param[in]  PipeName     The name of the pipe @nonnull.
**
** \param[out] PipeIdPtr    The PipeId for that name @nonnull.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_CreatePipe #CFE_SB_DeletePipe #CFE_SB_SetPipeOpts #CFE_SB_PIPEOPTS_IGNOREMINE
**/
CFE_Status_t CFE_SB_GetPipeIdByName(CFE_SB_PipeId_t *PipeIdPtr, const char *PipeName);
/**@}*/

/** @defgroup CFEAPISBSubscription cFE Message Subscription Control APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Subscribe to a message on the software bus
**
** \par Description
**          This routine adds the specified pipe to the destination list associated
**          with the specified message ID.
**
** \par Assumptions, External Events, and Notes:
**          Note:   As subscriptions are received, the destinations are added to
**          the head of a linked list. During the sending of a message, the list
**          is traversed beginning at the head of the list. Therefore the
**          message will first be sent to the last subscriber. If an application
**          has timing constraints and needs to receive a message in the
**          shortest possible time, the developer may consider holding off its
**          subscription until other applications have subscribed to the message.
**
** \param[in]  MsgId        The message ID of the message to be subscribed to.
**
** \param[in]  PipeId       The pipe ID of the pipe the subscribed message
**                          should be sent to.
**
** \param[in]  Quality      The requested Quality of Service (QoS) required of
**                          the messages. Most callers will use #CFE_SB_DEFAULT_QOS
**                          for this parameter.
**
** \param[in]  MsgLim       The maximum number of messages with this Message ID to
**                          allow in this pipe at the same time.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_SB_MAX_MSGS_MET  \covtest \copybrief CFE_SB_MAX_MSGS_MET
** \retval #CFE_SB_MAX_DESTS_MET \copybrief CFE_SB_MAX_DESTS_MET
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_BUF_ALOC_ERR  \covtest \copybrief CFE_SB_BUF_ALOC_ERR
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeLocal, #CFE_SB_Unsubscribe, #CFE_SB_UnsubscribeLocal
**/
CFE_Status_t CFE_SB_SubscribeEx(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, CFE_SB_Qos_t Quality, uint16 MsgLim);

/*****************************************************************************/
/**
** \brief Subscribe to a message on the software bus with default parameters
**
** \par Description
**          This routine adds the specified pipe to the destination list for
**          the specified message ID.  This is the same as #CFE_SB_SubscribeEx
**          with the Quality field set to #CFE_SB_DEFAULT_QOS and MsgLim set
**          to #CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT (4).
**
** \par Assumptions, External Events, and Notes:
**          Note:   As subscriptions are received, the destinations are added to
**          the head of a linked list. During the sending of a message, the list
**          is traversed beginning at the head of the list. Therefore the
**          message will first be sent to the last subscriber. If an application
**          has timing constraints and needs to receive a message in the
**          shortest possible time, the developer may consider holding off its
**          subscription until other applications have subscribed to the message.
**
** \param[in]  MsgId        The message ID of the message to be subscribed to.
**
** \param[in]  PipeId       The pipe ID of the pipe the subscribed message
**                          should be sent to.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_SB_MAX_MSGS_MET  \covtest \copybrief CFE_SB_MAX_MSGS_MET
** \retval #CFE_SB_MAX_DESTS_MET \copybrief CFE_SB_MAX_DESTS_MET
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_BUF_ALOC_ERR  \covtest \copybrief CFE_SB_BUF_ALOC_ERR
**
** \sa #CFE_SB_SubscribeEx, #CFE_SB_SubscribeLocal, #CFE_SB_Unsubscribe, #CFE_SB_UnsubscribeLocal
**/
CFE_Status_t CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);

/*****************************************************************************/
/**
** \brief Subscribe to a message while keeping the request local to a cpu
**
** \par Description
**          This routine adds the specified pipe to the destination list for
**          the specified message ID.  This is similar to #CFE_SB_SubscribeEx
**          with the Quality field set to #CFE_SB_DEFAULT_QOS and MsgLim set
**          to #CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT, but will not report the subscription.
**
**          Software Bus Network (SBN) application is an example use case,
**          where local subscriptions should not be reported to peers.
**
** \par Assumptions, External Events, and Notes:
**          - This API is typically only used by Software Bus Network (SBN) Application
**
** \param[in]  MsgId        The message ID of the message to be subscribed to.
**
** \param[in]  PipeId       The pipe ID of the pipe the subscribed message
**                          should be sent to.
**
** \param[in]  MsgLim       The maximum number of messages with this Message ID to
**                          allow in this pipe at the same time.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS          \copybrief CFE_SUCCESS
** \retval #CFE_SB_MAX_MSGS_MET  \covtest \copybrief CFE_SB_MAX_MSGS_MET
** \retval #CFE_SB_MAX_DESTS_MET \copybrief CFE_SB_MAX_DESTS_MET
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_BUF_ALOC_ERR  \covtest \copybrief CFE_SB_BUF_ALOC_ERR
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeEx, #CFE_SB_Unsubscribe, #CFE_SB_UnsubscribeLocal
**/
CFE_Status_t CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId, uint16 MsgLim);

/*****************************************************************************/
/**
** \brief Remove a subscription to a message on the software bus
**
** \par Description
**          This routine removes the specified pipe from the destination
**          list for the specified message ID.
**
** \par Assumptions, External Events, and Notes:
**          If the Pipe is not subscribed to MsgId, the CFE_SB_UNSUB_NO_SUBS_EID
**          event will be generated and #CFE_SUCCESS will be returned
**
** \param[in]  MsgId        The message ID of the message to be unsubscribed.
**
** \param[in]  PipeId       The pipe ID of the pipe the subscribed message
**                          should no longer be sent to.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT   \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeEx, #CFE_SB_SubscribeLocal, #CFE_SB_UnsubscribeLocal
**/
CFE_Status_t CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);

/*****************************************************************************/
/**
** \brief Remove a subscription to a message on the software bus on the current CPU
**
** \par Description
**          This routine removes the specified pipe from the destination
**          list for the specified message ID on the current CPU.
**
** \par Assumptions, External Events, and Notes:
**          This API is typically only used by Software Bus Network (SBN) Application.
**          If the Pipe is not subscribed to MsgId, the CFE_SB_UNSUB_NO_SUBS_EID
**          event will be generated and #CFE_SUCCESS will be returned
**
** \param[in]  MsgId        The message ID of the message to be unsubscribed.
**
** \param[in]  PipeId       The pipe ID of the pipe the subscribed message
**                          should no longer be sent to.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT   \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeEx, #CFE_SB_SubscribeLocal, #CFE_SB_Unsubscribe
**/
CFE_Status_t CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);
/**@}*/

/** @defgroup CFEAPISBMessage cFE Send/Receive Message APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Transmit a message
**
** \par Description
**          This routine copies the specified message into a software bus
**          buffer which is then transmitted to all subscribers.  The
**          software bus will read the message ID from the message header to
**          determine which pipes should receive the message.
**
**          The IsOrigination parameter should be passed as "true" if the message was
**          newly constructed by the sender and is being sent for the first time.  This
**          enables the message origination actions as determined by the CFE MSG module,
**          which may include (but not limited to):
**            - Updating sequence number
**            - Updating timestamp
**            - Calcualating a CRC, checksum, or other message error control field
**
**          Conversely, when forwarding a message that originated from
**          an external entity (e.g. messages passing through CI or SBN), the
**          parameter should be passed as "false" to not overwrite existing data.
**
** \par Assumptions, External Events, and Notes:
**          - This routine will not normally wait for the receiver tasks to
**            process the message before returning control to the caller's task.
**          - However, if a higher priority task is pending and subscribed to
**            this message, that task may get to run before returning
**            control to the caller.
**          - In previous versions of CFE, the boolean parameter referred to the
**            sequence number header of telemetry messages only.  This has been
**            extended to apply more generically to any headers, as determined by
**            the CFE MSG implementation.
**
** \param[in]  MsgPtr       A pointer to the message to be sent @nonnull.  This must point
**                          to the first byte of the message header.
** \param[in] IsOrigination  Update the headers of the message
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG  \copybrief CFE_SB_MSG_TOO_BIG
** \retval #CFE_SB_BUF_ALOC_ERR \covtest \copybrief CFE_SB_BUF_ALOC_ERR
**/
CFE_Status_t CFE_SB_TransmitMsg(const CFE_MSG_Message_t *MsgPtr, bool IsOrigination);

/*****************************************************************************/
/**
** \brief Receive a message from a software bus pipe
**
** \par Description
**          This routine retrieves the next message from the specified pipe.
**          If the pipe is empty, this routine will block until either a new
**          message comes in or the timeout value is reached.
**
** \par Assumptions, External Events, and Notes:
**          Note - If an error occurs in this API, the *BufPtr value may be NULL or
**          random. Therefore, it is recommended that the return code be tested
**          for CFE_SUCCESS before processing the message.
**
** \param[in, out] BufPtr   A pointer to the software bus buffer to receive to @nonnull.
**                          Typically a caller declares a ptr of type CFE_SB_Buffer_t
**                          (i.e. CFE_SB_Buffer_t *Ptr) then gives the address of that
**                          pointer (&Ptr) as this parameter. After a successful
**                          receipt of a message, *BufPtr will point to the first
**                          byte of the software bus buffer. This should be
**                          used as a read-only pointer (in systems with an MMU,
**                          writes to this pointer may cause a memory protection fault).
**                          The *BufPtr is valid only until the next call to
**                          CFE_SB_ReceiveBuffer for the same pipe.
**
** \param[in]  PipeId       The pipe ID of the pipe containing the message to be obtained.
**
** \param[in]  TimeOut      The number of milliseconds to wait for a new message if the
**                          pipe is empty at the time of the call.  This can also be set
**                          to #CFE_SB_POLL for a non-blocking receive or
**                          #CFE_SB_PEND_FOREVER to wait forever for a message to arrive.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_TIME_OUT     \copybrief CFE_SB_TIME_OUT
** \retval #CFE_SB_PIPE_RD_ERR  \covtest \copybrief CFE_SB_PIPE_RD_ERR
** \retval #CFE_SB_NO_MESSAGE   \copybrief CFE_SB_NO_MESSAGE
**/
CFE_Status_t CFE_SB_ReceiveBuffer(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut);

/** @} */

/** @defgroup CFEAPISBZeroCopy cFE Zero Copy APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Get a buffer pointer to use for "zero copy" SB sends.
**
** \par Description
**          This routine can be used to get a pointer to one of the software bus'
**          internal memory buffers that are used for sending messages.  The caller
**          can use this memory buffer to build an SB message, then send it using
**          the CFE_SB_TransmitBuffer() function.  This interface avoids an extra
**          copy of the message from the user's memory buffer to the software bus
**          internal buffer.
**
** \par Assumptions, External Events, and Notes:
**          -# The pointer returned by CFE_SB_AllocateMessageBuffer() is only good for one
**             call to CFE_SB_TransmitBuffer().
**          -# Once a buffer has been successfully transmitted (as indicated by a successful
**             return from CFE_SB_TransmitBuffer()) the buffer becomes owned by the SB application.
**             It will automatically be freed by SB once all recipients have finished reading it.
**          -# Applications must not de-reference the message pointer (for reading
**             or writing) after the call to CFE_SB_TransmitBuffer().
**          -# If #CFE_SB_ReleaseMessageBuffer should be used only if a message is not transmitted
**
** \param[in]  MsgSize  The size of the SB message buffer the caller wants
**                      (including the SB message header).
**
** \return A pointer to a memory buffer that message data can be written to
**         for use with CFE_SB_TransmitBuffer().
**/
CFE_SB_Buffer_t *CFE_SB_AllocateMessageBuffer(size_t MsgSize);

/*****************************************************************************/
/**
** \brief Release an unused "zero copy" buffer pointer.
**
** \par Description
**          This routine can be used to release a pointer to one of the software
**          bus' internal memory buffers.
**
** \par Assumptions, External Events, and Notes:
**          -# This function is not needed for normal "zero copy" transfers.  It
**             is needed only for cleanup when an application gets a pointer using
**             CFE_SB_AllocateMessageBuffer(), but (due to some error condition) never
**             uses that pointer in a call to CFE_SB_TransmitBuffer().
**
** \param[in]  BufPtr  A pointer to the SB internal buffer @nonnull.  This must be a
**                     pointer returned by a call to CFE_SB_AllocateMessageBuffer(),
**                     but never used in a call to CFE_SB_TransmitBuffer().
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_BUFFER_INVALID \copybrief CFE_SB_BUFFER_INVALID
**/
CFE_Status_t CFE_SB_ReleaseMessageBuffer(CFE_SB_Buffer_t *BufPtr);

/*****************************************************************************/
/**
** \brief Transmit a buffer
**
** \par Description
**          This routine sends a message that has been created directly in an
**          internal SB message buffer by an application (after a call to
**          #CFE_SB_AllocateMessageBuffer).  This interface is more complicated than
**          the normal #CFE_SB_TransmitMsg interface, but it avoids an extra copy of
**          the message from the user's memory buffer to the software bus
**          internal buffer.  The "zero copy" interface can be used to improve
**          performance in high-rate, high-volume software bus traffic.
**
**          The IsOrigination parameter should be passed as "true" if the message was
**          newly constructed by the sender and is being sent for the first time.  This
**          enables the message origination actions as determined by the CFE MSG module,
**          which may include (but not limited to):
**            - Updating sequence number
**            - Updating timestamp
**            - Calcualating a CRC, checksum, or other message error control field
**
**          Conversely, when forwarding a message that originated from
**          an external entity (e.g. messages passing through CI or SBN), the
**          parameter should be passed as "false" to not overwrite existing data.
**
** \par Assumptions, External Events, and Notes:
**          -# A handle returned by #CFE_SB_AllocateMessageBuffer is "consumed" by
**             a _successful_ call to #CFE_SB_TransmitBuffer.
**          -# If this function returns CFE_SUCCESS, this indicates the zero copy handle is
**             now owned by software bus, and is no longer owned by the calling application,
**             and should not be re-used.
**          -# However if this function fails (returns any error status) it does not change
**             the state of the buffer at all, meaning the calling application still owns it.
**             (a failure means the buffer is left in the same state it was before the call).
**          -# Applications should be written as if #CFE_SB_AllocateMessageBuffer is
**             equivalent to a \c malloc() and a successful call to #CFE_SB_TransmitBuffer
**             is equivalent to a \c free().
**          -# Applications must not de-reference the message pointer (for reading
**             or writing) after a successful call to #CFE_SB_TransmitBuffer.
**          -# This function will increment and apply the internally tracked
**             sequence counter if set to do so.
**
** \param[in] BufPtr         A pointer to the buffer to be sent @nonnull.
** \param[in] IsOrigination  Update applicable header field(s) of a newly constructed message
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG  \copybrief CFE_SB_MSG_TOO_BIG
**/
CFE_Status_t CFE_SB_TransmitBuffer(CFE_SB_Buffer_t *BufPtr, bool IsOrigination);

/** @} */

/** @defgroup CFEAPISBMessageCharacteristics cFE Message Characteristics APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Sets the length of user data in a software bus message.
**
** \par Description
**          This routine sets the field in the SB message header that determines
**          the size of the user data in a software bus message.  SB message header
**          formats can be different for each deployment of the cFE.  So, applications
**          should use this function rather than trying to poke a length value directly
**          into their SB message buffers.
**
** \par Assumptions, External Events, and Notes:
**          - You must set a valid message ID in the SB message header before
**            calling this function.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message @nonnull.
**                         This must point to the first byte of the message header.
**
** \param[in]  DataLength  The length to set (size of the user data, in bytes).
**/
void CFE_SB_SetUserDataLength(CFE_MSG_Message_t *MsgPtr, size_t DataLength);

/*****************************************************************************/
/**
** \brief Sets the time field in a software bus message with the current spacecraft time.
**
** \par Description
**          This routine sets the time of a software bus message with the current
**          spacecraft time.  This will be the same time that is returned by the
**          function #CFE_TIME_GetTime.
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not
**            include a time field, then this routine will do nothing.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message @nonnull.
**                         This must point to the first byte of the message header.
**/
void CFE_SB_TimeStampMsg(CFE_MSG_Message_t *MsgPtr);

/******************************************************************************/
/**
** \brief Copies a string into a software bus message
**
** \par Description
**    Strings within software bus messages have a defined/fixed maximum length, and
**    may not necessarily be null terminated within the message.  This presents a possible
**    issue when using the C library functions to copy strings out of a message.
**
**    This performs a very similar function to "strncpy()" except that the sizes
**    of _both_ buffers are passed in.  Neither buffer is required to be null-terminated,
**    but copying will stop after the first termination character is encountered.
**
**    If the destination buffer is not completely filled by the source data (such as if
**    the supplied string was shorter than the allotted length) the destination buffer
**    will be padded with NUL characters up to the size of the buffer, similar to what
**    strncpy() does.  This ensures that the entire destination buffer is set.
**
** \note
**    If the source string buffer is already guaranteed to be null terminated,
**    then there is no difference between the C library "strncpy()" function and this
**    implementation.  It is only necessary to use this when termination of the source
**    buffer is not guaranteed.
**
** \param[out] DestStringPtr    Pointer to destination buffer (component of SB message definition) @nonnull
** \param[in]  SourceStringPtr  Pointer to source buffer @nonnull
** \param[in]  DestMaxSize      Size of destination buffer as defined by the message definition
** \param[in]  SourceMaxSize    Size of source buffer
**
** \return Number of characters copied or error code, see \ref CFEReturnCodes
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
**
*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, size_t DestMaxSize,
                              size_t SourceMaxSize);

/*****************************************************************************/
/**
** \brief Get a pointer to the user data portion of a software bus message.
**
** \par Description
**          This routine returns a pointer to the user data portion of a software
**          bus message.  SB message header formats can be different for each
**          deployment of the cFE.  So, applications should use this function and
**          avoid hard coding offsets into their SB message buffers.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message @nonnull.
**
** \return A pointer to the first byte of user data within the software bus message.
**/
void *CFE_SB_GetUserData(CFE_MSG_Message_t *MsgPtr);

/*****************************************************************************/
/**
** \brief Gets the length of user data in a software bus message.
**
** \par Description
**          This routine returns the size of the user data in a software bus message.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message @nonnull.
**                     This must point to the first byte of the message header.
**
** \return The size (in bytes) of the user data in the software bus message.
** \retval 0 if an error occurs, such as if the MsgPtr argument is not valid.
**/
size_t CFE_SB_GetUserDataLength(const CFE_MSG_Message_t *MsgPtr);

/******************************************************************************/
/**
** \brief Copies a string out of a software bus message
**
** \par Description
**    Strings within software bus messages have a defined/fixed maximum length, and
**    may not necessarily be null terminated within the message.  This presents a possible
**    issue when using the C library functions to copy strings out of a message.
**
**    This function should replace use of C library functions such as strcpy/strncpy
**    when copying strings out of software bus messages to local storage buffers.
**
**    Up to [SourceMaxSize] or [DestMaxSize-1] (whichever is smaller) characters will be
**    copied from the source buffer to the destination buffer, and a NUL termination
**    character will be written to the destination buffer as the last character.
**
**    If the DefaultString pointer is non-NULL, it will be used in place of the source
**    string if the source is an empty string.  This is typically a string constant that
**    comes from the platform configuration, allowing default values to be assumed for
**    fields that are unspecified.
**
**    IMPORTANT - the default string, if specified, must be null terminated.  This will
**    be the case if a string literal is passed in (the typical/expected use case).
**
**    If the default is NULL, then only the source string will be copied, and the result
**    will be an empty string if the source was empty.
**
**    If the destination buffer is too small to store the entire string, it will be
**    truncated, but it will still be null terminated.
**
** \param[out] DestStringPtr    Pointer to destination buffer @nonnull
** \param[in]  SourceStringPtr  Pointer to source buffer (component of SB message definition)
** \param[in]  DefaultString    Default string to use if source is empty
** \param[in]  DestMaxSize      Size of destination storage buffer @nonzero
** \param[in]  SourceMaxSize    Size of source buffer as defined by the message definition
**
** \return Number of characters copied or error code, see \ref CFEReturnCodes
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
**
*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString,
                              size_t DestMaxSize, size_t SourceMaxSize);
/** @} */

/** @defgroup CFEAPISBMessageID cFE Message ID APIs
 * @{
 */

/*****************************************************************************/
/**
 * \brief Identifies whether a given CFE_SB_MsgId_t is valid
 *
 * \par Description
 *    Implements a basic sanity check on the value provided
 *
 * \return Boolean message ID validity indicator
 * \retval true  Message ID is within the valid range
 * \retval false Message ID is not within the valid range
 */
bool CFE_SB_IsValidMsgId(CFE_SB_MsgId_t MsgId);

/*****************************************************************************/
/**
 * \brief Identifies whether two #CFE_SB_MsgId_t values are equal
 *
 * \par Description
 *    In cases where the #CFE_SB_MsgId_t type is not a simple integer
 *    type, it may not be possible to do a direct equality check.
 *    This inline function provides an abstraction for the equality
 *    check between two #CFE_SB_MsgId_t values.
 *
 *    Applications should transition to using this function to compare
 *    MsgId values for equality to remain compatible with future versions
 *    of cFE.
 *
 * \return Boolean message ID equality indicator
 * \retval true  Message IDs are Equal
 * \retval false Message IDs are not Equal
 */
static inline bool CFE_SB_MsgId_Equal(CFE_SB_MsgId_t MsgId1, CFE_SB_MsgId_t MsgId2)
{
    return CFE_SB_MSGID_UNWRAP_VALUE(MsgId1) == CFE_SB_MSGID_UNWRAP_VALUE(MsgId2);
}

/*****************************************************************************/
/**
 * \brief Converts a #CFE_SB_MsgId_t to a normal integer
 *
 * \par Description
 *    In cases where the #CFE_SB_MsgId_t type is not a simple integer
 *    type, it is not possible to directly display the value in a
 *    printf-style statement, use it in a switch() statement, or other
 *    similar use cases.
 *
 *    This inline function provides the ability to map a #CFE_SB_MsgId_t
 *    type back into a simple integer value.
 *
 *    Applications should transition to using this function wherever a
 *    #CFE_SB_MsgId_t type needs to be used as an integer.
 *
 * \par Assumptions and Notes:
 *    This negates the type safety that was gained by using a non-
 *    integer type for the #CFE_SB_MsgId_t value.  This should only be used
 *    in specific cases such as UI display (printf, events, etc) where the
 *    value is being sent externally.  Any internal API calls should be
 *    updated to use the #CFE_SB_MsgId_t type directly, rather than an
 *    integer type.
 *
 * \return Integer representation of the #CFE_SB_MsgId_t
 */
static inline CFE_SB_MsgId_Atom_t CFE_SB_MsgIdToValue(CFE_SB_MsgId_t MsgId)
{
    return CFE_SB_MSGID_UNWRAP_VALUE(MsgId);
}

/*****************************************************************************/
/**
 * \brief Converts a normal integer into a #CFE_SB_MsgId_t
 *
 * \par Description
 *    In cases where the #CFE_SB_MsgId_t type is not a simple integer
 *    type, it is not possible to directly use an integer value
 *    supplied via a define or similar method.
 *
 *    This inline function provides the ability to map an integer value
 *    into a corresponding #CFE_SB_MsgId_t value.
 *
 *    Applications should transition to using this function wherever an
 *    integer needs to be used for a #CFE_SB_MsgId_t.
 *
 * \par Assumptions and Notes:
 *    This negates the type safety that was gained by using a non-
 *    integer type for the #CFE_SB_MsgId_t value.  This should only be
 *    used in specific cases where the value is coming from an external
 *    source.  Any internal API calls should be updated to return the
 *    #CFE_SB_MsgId_t type directly, rather than an integer type.
 *
 * \return #CFE_SB_MsgId_t representation of the integer
 */
static inline CFE_SB_MsgId_t CFE_SB_ValueToMsgId(CFE_SB_MsgId_Atom_t MsgIdValue)
{
    CFE_SB_MsgId_t Result = CFE_SB_MSGID_C(MsgIdValue);
    return Result;
}

/*****************************************************************************/
/**
 * \brief Converts a topic ID and instance number combination into a MsgID value integer
 *
 * \par Description
 *    This function accepts a data pair of topic ID + instance number and returns the
 *    corresponding MsgID Value (integer) for commands.
 *
 * \par Assumptions and Notes:
 *    A topic ID identifies a certain data stream from an application, for example
 *    the CFE Software bus ground commands (CFE_MISSION_SB_CMD_TOPICID).  In
 *    contrast to MsgID, the topic ID is consistent across all CPUs in a system, whereas
 *    each CPU instance will have a unique MsgID.
 *
 *    CPU instance numbers are 1-based.  The instance number of 0 is reserved
 *    for "global" MsgID values that are the same on all CPUs.  The PSP function
 *    may be used to obtain the current CPU number for the host processor.
 *
 * \sa CFE_SB_TlmTopicIdToMsgId(), CFE_PSP_GetProcessorId()
 *
 * \return Integer representation of the #CFE_SB_MsgId_t
 */
CFE_SB_MsgId_Atom_t CFE_SB_CmdTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum);

/*****************************************************************************/
/**
 * \brief Converts a topic ID and instance number combination into a MsgID value integer
 *
 * \par Description
 *    This function accepts a data pair of topic ID + instance number and returns the
 *    corresponding MsgID Value (integer) for telemetry.
 *
 * \par Assumptions and Notes:
 *    A topic ID identifies a certain data stream from an application, for example
 *    the CFE Software bus housekeeping telemetry (CFE_MISSION_SB_HK_TLM_TOPICID).  In
 *    contrast to MsgID, the topic ID is consistent across all CPUs in a system, whereas
 *    each CPU instance will have a unique MsgID.
 *
 *    CPU instance numbers are 1-based.  The instance number of 0 is reserved
 *    for "global" MsgID values that are the same on all CPUs.  The PSP function
 *    may be used to obtain the current CPU number for the host processor.
 *
 * \sa CFE_SB_CmdTopicIdToMsgId(), CFE_PSP_GetProcessorId()
 *
 * \return Integer representation of the #CFE_SB_MsgId_t
 */
CFE_SB_MsgId_Atom_t CFE_SB_TlmTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum);

/*****************************************************************************/
/**
 * \brief Converts a topic ID to a MsgID value integer for Global commands
 *
 * \par Description
 *    This is a wrapper around CFE_SB_CmdTopicIdToMsgId() for topic IDs which
 *    are the same on all CPUs within a system (i.e. not specific to a certain
 *    processor)
 *
 * \par Assumptions and Notes:
 *    Global MsgIDs may be used when only a single instance of a service exists
 *    within the system.  The CFE framework does not use this feature for commands,
 *    but is defined for future use.
 *
 * \sa CFE_SB_CmdTopicIdToMsgId(), CFE_SB_LocalCmdTopicIdToMsgId()
 *
 * \return Integer representation of the #CFE_SB_MsgId_t
 */
CFE_SB_MsgId_Atom_t CFE_SB_GlobalCmdTopicIdToMsgId(uint16 TopicId);

/*****************************************************************************/
/**
 * \brief Converts a topic ID to a MsgID value integer for Global telemetry
 *
 * \par Description
 *    This is a wrapper around CFE_SB_TlmTopicIdToMsgId() for topic IDs which
 *    are the same on all CPUs within a system (i.e. not specific to a certain
 *    processor)
 *
 * \par Assumptions and Notes:
 *    Global MsgIDs may be used when only a single instance of a service exists
 *    within the system.  An example for such telemetry is the time synchronization
 *    service published by CFE_TIME.
 *
 * \sa CFE_SB_TlmTopicIdToMsgId(), CFE_SB_LocalTlmTopicIdToMsgId()
 *
 * \return Integer representation of the #CFE_SB_MsgId_t
 */
CFE_SB_MsgId_Atom_t CFE_SB_GlobalTlmTopicIdToMsgId(uint16 TopicId);

/*****************************************************************************/
/**
 * \brief Converts a topic ID to a MsgID value integer for local commands
 *
 * \par Description
 *    This is a wrapper around CFE_SB_CmdTopicIdToMsgId() for topic IDs which
 *    are unique on all CPUs within a system (i.e. specific to a certain
 *    processor)
 *
 * \par Assumptions and Notes:
 *    This assumes the caller is referring to a service running on the same
 *    processor instance as itself.
 *
 * \sa CFE_SB_CmdTopicIdToMsgId(), CFE_SB_LocalTlmTopicIdToMsgId()
 *
 * \return Integer representation of the #CFE_SB_MsgId_t
 */
CFE_SB_MsgId_Atom_t CFE_SB_LocalCmdTopicIdToMsgId(uint16 TopicId);

/*****************************************************************************/
/**
 * \brief Converts a topic ID to a MsgID value integer for local telemetry
 *
 * \par Description
 *    This is a wrapper around CFE_SB_TlmTopicIdToMsgId() for topic IDs which
 *    are unique on all CPUs within a system (i.e. specific to a certain
 *    processor)
 *
 * \par Assumptions and Notes:
 *    This assumes the caller is referring to a service running on the same
 *    processor instance as itself.
 *
 * \sa CFE_SB_TlmTopicIdToMsgId(), CFE_SB_LocalCmdTopicIdToMsgId()
 *
 * \return Integer representation of the #CFE_SB_MsgId_t
 */
CFE_SB_MsgId_Atom_t CFE_SB_LocalTlmTopicIdToMsgId(uint16 TopicId);

/** @} */

#endif /* CFE_SB_H */
