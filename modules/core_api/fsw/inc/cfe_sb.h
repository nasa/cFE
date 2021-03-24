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

/**
 * @file
 *
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

#ifndef CFE_OMIT_DEPRECATED_6_8
/* only deprecated functions depend on TIME types */
#include "cfe_time_api_typedefs.h"
#endif

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
** \param[in, out]  PipeIdPtr    A pointer to a variable of type #CFE_SB_PipeId_t,
**                          which will be filled in with the pipe ID information
**                          by the #CFE_SB_CreatePipe routine. *PipeIdPtr is the identifier for the created pipe.
**
** \param[in]  Depth        The maximum number of messages that will be allowed on
**                          this pipe at one time.
**
** \param[in]  PipeName     A string to be used to identify this pipe in error messages
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
 * @param[out]  Idx    Buffer where the calculated index will be stored
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
** \param[in]  Opts         A bit field of options.
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
** \param[out] *OptPtr      A bit field of options.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
**
** \sa #CFE_SB_CreatePipe #CFE_SB_DeletePipe #CFE_SB_SetPipeOpts #CFE_SB_GetPipeIdByName #CFE_SB_PIPEOPTS_IGNOREMINE
**/
CFE_Status_t CFE_SB_GetPipeOpts(CFE_SB_PipeId_t PipeId, uint8 *OptPtr);

/*****************************************************************************/
/**
** \brief Get the pipe name for a given id.
**
** \par Description
**          This routine finds the pipe name for a pipe id.
**
** \param[out] PipeNameBuf  The buffer to receive the pipe name.
**
** \param[in] PipeNameSize  The size (in chars) of the PipeName buffer.
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
** \param[in]  PipeName     The name of the pipe.
**
** \param[out] PipeIdPtr    The PipeId for that name.
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
** \retval #CFE_SB_MAX_MSGS_MET  \copybrief CFE_SB_MAX_MSGS_MET
** \retval #CFE_SB_MAX_DESTS_MET \copybrief CFE_SB_MAX_DESTS_MET
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_BUF_ALOC_ERR  \copybrief CFE_SB_BUF_ALOC_ERR
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
** \retval #CFE_SB_MAX_MSGS_MET  \copybrief CFE_SB_MAX_MSGS_MET
** \retval #CFE_SB_MAX_DESTS_MET \copybrief CFE_SB_MAX_DESTS_MET
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_BUF_ALOC_ERR  \copybrief CFE_SB_BUF_ALOC_ERR
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
**          Subscription Reporting is enabled for interprocessor communication
**          by way of the Software Bus Network (SBN) Application.
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
** \retval #CFE_SB_MAX_MSGS_MET  \copybrief CFE_SB_MAX_MSGS_MET
** \retval #CFE_SB_MAX_DESTS_MET \copybrief CFE_SB_MAX_DESTS_MET
** \retval #CFE_SB_BAD_ARGUMENT  \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_BUF_ALOC_ERR  \copybrief CFE_SB_BUF_ALOC_ERR
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
**          None
**
** \param[in]  MsgId        The message ID of the message to be unsubscribed.
**
** \param[in]  PipeId       The pipe ID of the pipe the subscribed message
**                          should no longer be sent to.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_NO_SUBSCRIBERS \copybrief CFE_SB_NO_SUBSCRIBERS
** \retval #CFE_SB_INTERNAL_ERR   \copybrief CFE_SB_INTERNAL_ERR
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
**          - This API is typically only used by Software Bus Network (SBN) Application
**
** \param[in]  MsgId        The message ID of the message to be unsubscribed.
**
** \param[in]  PipeId       The pipe ID of the pipe the subscribed message
**                          should no longer be sent to.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_NO_SUBSCRIBERS \copybrief CFE_SB_NO_SUBSCRIBERS
** \retval #CFE_SB_INTERNAL_ERR   \copybrief CFE_SB_INTERNAL_ERR
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
** \par Assumptions, External Events, and Notes:
**          - This routine will not normally wait for the receiver tasks to
**            process the message before returning control to the caller's task.
**          - However, if a higher priority task is pending and subscribed to
**            this message, that task may get to run before returning
**            control to the caller.
**
** \param[in]  MsgPtr       A pointer to the message to be sent.  This must point
**                          to the first byte of the message header.
** \param[in] IncrementSequenceCount Boolean to increment the internally tracked
**                                   sequence count and update the message if the
**                                   buffer contains a telemetry message
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG  \copybrief CFE_SB_MSG_TOO_BIG
** \retval #CFE_SB_BUF_ALOC_ERR \copybrief CFE_SB_BUF_ALOC_ERR
**/
CFE_Status_t CFE_SB_TransmitMsg(CFE_MSG_Message_t *MsgPtr, bool IncrementSequenceCount);

#ifndef CFE_OMIT_DEPRECATED_6_8
/*****************************************************************************/
/**
** \brief DEPRECATED: Send a software bus message
** \deprecated Use CFE_SB_TransmitMsg
**
** \par Description
**          This routine sends the specified message to all subscribers.  The
**          software bus will read the message ID from the message header to
**          determine which pipes should receive the message.
**
** \par Assumptions, External Events, and Notes:
**          - This routine will not normally wait for the receiver tasks to
**            process the message before returning control to the caller's task.
**          - However, if a higher priority task is pending and subscribed to
**            this message, that task may get to run before returning
**            control to the caller.
**          - This function tracks and increments the source sequence counter
**            of a telemetry message.
**
** \param[in]  MsgPtr       A pointer to the message to be sent.  This must point
**                          to the first byte of the message header.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG  \copybrief CFE_SB_MSG_TOO_BIG
** \retval #CFE_SB_BUF_ALOC_ERR \copybrief CFE_SB_BUF_ALOC_ERR
**/
CFE_Status_t CFE_SB_SendMsg(CFE_MSG_Message_t *MsgPtr);

/*****************************************************************************/
/**
** \brief DEPRECATED: Passes a software bus message
** \deprecated Use CFE_SB_TransmitMsg
**
** \par Description
**          This routine sends the specified message to all subscribers.  The
**          software bus will read the message ID from the message header to
**          determine which pipes should receive the message. This routine is
**          intended to pass messages not generated by the sending application.
**
** \par Assumptions, External Events, and Notes:
**          - This routine will not normally wait for the receiver tasks to
**            process the message before returning control to the caller's task.
**          - However, if a higher priority task is pending and subscribed to
**            this message, that task may get to run before #CFE_SB_PassMsg
**            returns control to the caller.
**          - This routine will not modify the sequence counter in the message.
**
** \param[in]  MsgPtr       A pointer to the message to be sent.  This must point
**                          to the first byte of the message header.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG  \copybrief CFE_SB_MSG_TOO_BIG
** \retval #CFE_SB_BUF_ALOC_ERR \copybrief CFE_SB_BUF_ALOC_ERR
**/
CFE_Status_t CFE_SB_PassMsg(CFE_MSG_Message_t *MsgPtr);
#endif /* CFE_OMIT_DEPRECATED_6_8 */

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
** \param[in, out] BufPtr   A pointer to the software bus buffer to receive to.
**                          Typically a caller declares a ptr of type CFE_SB_Buffer_t
**                          (i.e. CFE_SB_Buffer_t *Ptr) then gives the address of that
**                          pointer (&Ptr) as this parmeter. After a successful
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
** \retval #CFE_SB_PIPE_RD_ERR  \copybrief CFE_SB_PIPE_RD_ERR
** \retval #CFE_SB_NO_MESSAGE   \copybrief CFE_SB_NO_MESSAGE
**/
CFE_Status_t CFE_SB_ReceiveBuffer(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut);

#ifndef CFE_OMIT_DEPRECATED_6_8
/**
 * \brief DEPRECATED: receive buffer
 * \deprecated use CFE_SB_ReceiveBuffer
 */
CFE_Status_t CFE_SB_RcvMsg(CFE_SB_Buffer_t **BufPtr, CFE_SB_PipeId_t PipeId, int32 TimeOut);
#endif /* CFE_OMIT_DEPRECATED_6_8 */
/**@}*/

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
**          the #CFE_SB_TransmitBuffer function.  This interface is more complicated
**          than the normal #CFE_SB_TransmitMsg interface, but it avoids an extra
**          copy of the message from the user's memory buffer to the software bus
**          internal buffer.  The "zero copy" interface can be used to improve
**          performance in high-rate, high-volume software bus traffic.
**
** \par Assumptions, External Events, and Notes:
**          -# The pointer returned by #CFE_SB_ZeroCopyGetPtr is only good for one
**             call to #CFE_SB_TransmitBuffer.
**          -# Applications should be written as if #CFE_SB_ZeroCopyGetPtr is
**             equivalent to a \c malloc() and #CFE_SB_TransmitBuffer is equivalent to
**             a \c free().
**          -# Applications must not de-reference the message pointer (for reading
**             or writing) after the call to #CFE_SB_TransmitBuffer.
**
** \param[in]  MsgSize  The size of the SB message buffer the caller wants
**                      (including the SB message header).
**
** \param[out] BufferHandle  A handle that must be supplied when sending or releasing
**                           in zero copy mode.
**
** \return A pointer to a memory buffer that message data can be written to
**         for use with #CFE_SB_TransmitBuffer.
**/
CFE_SB_Buffer_t *CFE_SB_ZeroCopyGetPtr(size_t MsgSize, CFE_SB_ZeroCopyHandle_t *BufferHandle);

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
**             #CFE_SB_ZeroCopyGetPtr, but (due to some error condition) never uses
**             that pointer for a #CFE_SB_TransmitBuffer
**
** \param[in]  Ptr2Release  A pointer to the SB internal buffer.  This must be a
**                          pointer returned by a call to #CFE_SB_ZeroCopyGetPtr,
**                          but never used in a call to #CFE_SB_TransmitBuffer.
**
** \param[in]  ZeroCopyHandle  This must be the handle supplied with the pointer
**                             when #CFE_SB_ZeroCopyGetPtr was called.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_BUFFER_INVALID \copybrief CFE_SB_BUFFER_INVALID
**/
CFE_Status_t CFE_SB_ZeroCopyReleasePtr(CFE_SB_Buffer_t *Ptr2Release, CFE_SB_ZeroCopyHandle_t ZeroCopyHandle);

/*****************************************************************************/
/**
** \brief Transmit a buffer
**
** \par Description
**          This routine sends a message that has been created directly in an
**          internal SB message buffer by an application (after a call to
**          #CFE_SB_ZeroCopyGetPtr).  This interface is more complicated than
**          the normal #CFE_SB_TransmitMsg interface, but it avoids an extra copy of
**          the message from the user's memory buffer to the software bus
**          internal buffer.  The "zero copy" interface can be used to improve
**          performance in high-rate, high-volume software bus traffic.
**
** \par Assumptions, External Events, and Notes:
**          -# A handle returned by #CFE_SB_ZeroCopyGetPtr is "consumed" by
**             a _successful_ call to #CFE_SB_TransmitBuffer.
**          -# If this function returns CFE_SUCCESS, this indicates the zero copy handle is
**             now owned by software bus, and is no longer owned by the calling application,
**             and should not be re-used.
**          -# Howver if this function fails (returns any error status) it does not change
**             the state of the buffer at all, meaning the calling application still owns it.
**             (a failure means the buffer is left in the same state it was before the call).
**          -# Applications should be written as if #CFE_SB_ZeroCopyGetPtr is
**             equivalent to a \c malloc() and a successful call to #CFE_SB_TransmitBuffer
**             is equivalent to a \c free().
**          -# Applications must not de-reference the message pointer (for reading
**             or writing) after a successful call to #CFE_SB_TransmitBuffer.
**          -# This function will increment and apply the internally tracked
**             sequence counter if set to do so.
**
** \param[in] BufPtr                 A pointer to the buffer to be sent.
** \param[in] ZeroCopyHandle         The handle supplied by the #CFE_SB_ZeroCopyGetPtr call
** \param[in] IncrementSequenceCount Boolean to increment the internally tracked
**                                   sequence count and update the message if the
**                                   buffer contains a telemetry message
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS         \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG  \copybrief CFE_SB_MSG_TOO_BIG
** \retval #CFE_SB_BUF_ALOC_ERR \copybrief CFE_SB_BUF_ALOC_ERR
**/
CFE_Status_t CFE_SB_TransmitBuffer(CFE_SB_Buffer_t *BufPtr, CFE_SB_ZeroCopyHandle_t ZeroCopyHandle,
                                   bool IncrementSequenceCount);

#ifndef CFE_OMIT_DEPRECATED_6_8
/*****************************************************************************/
/**
** \brief DEPRECATED: Send an SB message in "zero copy" mode.
** \deprecated use CFE_SB_TransmitBuffer
**
** \par Description
**          This routine sends a message that has been created directly in an
**          internal SB message buffer by an application (after a call to
**          #CFE_SB_ZeroCopyGetPtr).  This interface is more complicated than
**          the normal #CFE_SB_TransmitMsg interface, but it avoids an extra copy of
**          the message from the user's memory buffer to the software bus
**          internal buffer.  The "zero copy" interface can be used to improve
**          performance in high-rate, high-volume software bus traffic.
**
** \par Assumptions, External Events, and Notes:
**          -# The pointer returned by #CFE_SB_ZeroCopyGetPtr is only good for
**             one call to #CFE_SB_TransmitBuffer.
**          -# Callers must not use the same SB message buffer for multiple sends.
**          -# Applications should be written as if #CFE_SB_ZeroCopyGetPtr is
**             equivalent to a \c malloc() and #CFE_SB_TransmitBuffer is equivalent
**             to a \c free().
**          -# Applications must not de-reference the message pointer (for reading
**             or writing) after the call to #CFE_SB_TransmitBuffer.
**          -# This function tracks and increments the source sequence counter
**             of a telemetry message.
**
** \param[in]  BufPtr  A pointer to the SB buffer to be sent.
**
** \param[in]  BufferHandle  The handle supplied with the #CFE_SB_ZeroCopyGetPtr call.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT   \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG    \copybrief CFE_SB_MSG_TOO_BIG
** \retval #CFE_SB_BUF_ALOC_ERR   \copybrief CFE_SB_BUF_ALOC_ERR
** \retval #CFE_SB_BUFFER_INVALID \copybrief CFE_SB_BUFFER_INVALID
**/
CFE_Status_t CFE_SB_ZeroCopySend(CFE_SB_Buffer_t *BufPtr, CFE_SB_ZeroCopyHandle_t BufferHandle);

/*****************************************************************************/
/**
** \brief DEPRECATED: Pass an SB message in "zero copy" mode.
** \deprecated use CFE_SB_TransmitBuffer
**
** \par Description
**          This routine sends a message that has been created directly in an
**          internal SB message buffer by an application (after a call to
**          #CFE_SB_ZeroCopyGetPtr).  This interface is more complicated than
**          the normal #CFE_SB_TransmitMsg interface, but it avoids an extra copy of
**          the message from the user's memory buffer to the software bus
**          internal buffer.  The "zero copy" interface can be used to improve
**          performance in high-rate, high-volume software bus traffic. This
**          version is intended to pass messages not generated by the caller
**          (to preserve the source sequence count).
**
** \par Assumptions, External Events, and Notes:
**          -# The pointer returned by #CFE_SB_ZeroCopyGetPtr is only good for
**             one call to #CFE_SB_TransmitBuffer or #CFE_SB_ZeroCopyPass.
**          -# Callers must not use the same SB message buffer for multiple sends.
**          -# Applications should be written as if #CFE_SB_ZeroCopyGetPtr is
**             equivalent to a \c malloc() and #CFE_SB_ZeroCopyPass is equivalent
**             to a \c free().
**          -# Applications must not de-reference the message pointer (for reading
**             or writing) after the call to #CFE_SB_ZeroCopyPass.
**          -# This routine will not modify the sequence counter in a telemetry
**             message
**
** \param[in]  BufPtr  A pointer to the SB buffer to be sent.
**
** \param[in]  BufferHandle  The handle supplied with the #CFE_SB_ZeroCopyGetPtr call.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_BAD_ARGUMENT   \copybrief CFE_SB_BAD_ARGUMENT
** \retval #CFE_SB_MSG_TOO_BIG    \copybrief CFE_SB_MSG_TOO_BIG
** \retval #CFE_SB_BUF_ALOC_ERR   \copybrief CFE_SB_BUF_ALOC_ERR
** \retval #CFE_SB_BUFFER_INVALID \copybrief CFE_SB_BUFFER_INVALID
**/
CFE_Status_t CFE_SB_ZeroCopyPass(CFE_SB_Buffer_t *BufPtr, CFE_SB_ZeroCopyHandle_t BufferHandle);
/**@}*/
#endif

/** @defgroup CFEAPISBSetMessage cFE Setting Message Characteristics APIs
 * @{
 */

#ifndef CFE_OMIT_DEPRECATED_6_8

/*****************************************************************************/
/**
** \brief DEPRECATED - Initialize a buffer for a software bus message.
** \deprecated Use CFE_MSG_Init
**
** \par Description
**          This routine fills in the header information needed to create a
**          valid software bus message.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  MsgPtr  A pointer to the buffer that will contain the message.
**                     This will point to the first byte of the message header.
**                     The \c void* data type allows the calling routine to use
**                     any data type when declaring its message buffer.
**
** \param[in]  MsgId   The message ID to put in the message header.
**
** \param[in]  Length  The total number of bytes of message data, including the SB
**                     message header  .
**
** \param[in]  Clear   A flag indicating whether to clear the rest of the message:
**                     \arg true - fill sequence count and packet data with zeroes.
**                     \arg false - leave sequence count and packet data unchanged.
**/
void CFE_SB_InitMsg(void *MsgPtr, CFE_SB_MsgId_t MsgId, size_t Length, bool Clear);

/*****************************************************************************/
/**
** \brief DEPRECATED - Sets the message ID of a software bus message.
** \deprecated Use CFE_MSG_SetMsgId
**
** \par Description
**          This routine sets the Message ID in a software bus message header.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**                     This must point to the first byte of the message header.
**
** \param[in]  MsgId   The message ID to put into the message header.
**/
void CFE_SB_SetMsgId(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId);
#endif /* CFE_OMIT_DEPRECATED_6_8 */

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
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  DataLength  The length to set (size of the user data, in bytes).
**/
void CFE_SB_SetUserDataLength(CFE_MSG_Message_t *MsgPtr, size_t DataLength);

#ifndef CFE_OMIT_DEPRECATED_6_8
/*****************************************************************************/
/**
** \brief DEPRECATED: Sets the total length of a software bus message.
** \deprecated Use CFE_MSG_SetSize
**
** \par Description
**          This routine sets the field in the SB message header that determines
**          the total length of the message.  SB message header formats can be
**          different for each deployment of the cFE.  So, applications should
**          use this function rather than trying to poke a length value directly
**          into their SB message buffers.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  TotalLength The length to set (total size of the message, in bytes,
**                         including headers).
**/
void CFE_SB_SetTotalMsgLength(CFE_MSG_Message_t *MsgPtr, size_t TotalLength);

/*****************************************************************************/
/**
** \brief Sets the time field in a software bus message.
** \deprecated Use CFE_MSG_SetMsgTime
**
** \par Description
**          This routine sets the time of a software bus message.  Most applications
**          will want to use #CFE_SB_TimeStampMsg instead of this function.  But,
**          when needed, this API can be used to send a group of SB messages
**          with identical time stamps.
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not include
**            a time field, then this routine will do nothing to the message contents
**            and will return #CFE_SB_WRONG_MSG_TYPE.
**          - Note default implementation of command messages do not have a time field
**            and will trigger the #CFE_SB_WRONG_MSG_TYPE error
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  Time        The time to include in the message.  This will usually be a time
**                         returned by the function #CFE_TIME_GetTime.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_WRONG_MSG_TYPE \copybrief CFE_SB_WRONG_MSG_TYPE
**/
CFE_Status_t CFE_SB_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t Time);
#endif /* CFE_OMIT_DEPRECATED_6_8 */

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
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**/
void CFE_SB_TimeStampMsg(CFE_MSG_Message_t *MsgPtr);

#ifndef CFE_OMIT_DEPRECATED_6_8
/*****************************************************************************/
/**
** \brief DEPRECATED:Sets the command code field in a software bus message.
** \deprecated Use CFE_MSG_SetFcnCode
**
** \par Description
**          This routine sets the command code of a software bus message (if SB
**          messages are implemented as CCSDS packets, this will be the function code).
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not
**            include a command code field, then this routine will do nothing to
**            the message contents and will return #CFE_SB_WRONG_MSG_TYPE.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  CmdCode     The command code to include in the message.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS           \copybrief CFE_SUCCESS
** \retval #CFE_SB_WRONG_MSG_TYPE \copybrief CFE_SB_WRONG_MSG_TYPE
**/
CFE_Status_t CFE_SB_SetCmdCode(CFE_MSG_Message_t *MsgPtr, uint16 CmdCode);
#endif /* CFE_OMIT_DEPRECATED_6_8 */

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
** \param[out] DestStringPtr    Pointer to destination buffer (component of SB message definition)
** \param[in]  SourceStringPtr  Pointer to source buffer
** \param[in]  DestMaxSize      Size of destination buffer as defined by the message definition
** \param[in]  SourceMaxSize    Size of source buffer
**
** \return Number of characters copied or error code, see \ref CFEReturnCodes
**
*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, size_t DestMaxSize,
                              size_t SourceMaxSize);
/**@}*/

/** @defgroup CFEAPIGetMessage cFE Getting Message Characteristics APIs
 * @{
 */

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
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**
** \return A pointer to the first byte of user data within the software bus message.
**/
void *CFE_SB_GetUserData(CFE_MSG_Message_t *MsgPtr);

#ifndef CFE_OMIT_DEPRECATED_6_8

/*****************************************************************************/
/**
** \brief DEPRECATED:Get the message ID of a software bus message.
** \deprecated Use CFE_MSG_GetMsgId
**
** \par Description
**          This routine returns the message ID from a software bus message.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**
** \return The software bus Message ID from the message header.
**/
CFE_SB_MsgId_t CFE_SB_GetMsgId(const CFE_MSG_Message_t *MsgPtr);
#endif /* CFE_OMIT_DEPRECATED_6_8 */

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
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**                     This must point to the first byte of the message header.
**
** \return The size (in bytes) of the user data in the software bus message.
**/
size_t CFE_SB_GetUserDataLength(const CFE_MSG_Message_t *MsgPtr);

#ifndef CFE_OMIT_DEPRECATED_6_8

/*****************************************************************************/
/**
** \brief DEPRECATED: Gets the total length of a software bus message.
** \deprecated Use CFE_MSG_GetSize
**
** \par Description
**          This routine returns the total size of the software bus message.
**
** \par Assumptions, External Events, and Notes:
**          - For the CCSDS implementation of this API, the size is derived from
**            the message header.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \return The total size (in bytes) of the software bus message, including headers.
**/
size_t CFE_SB_GetTotalMsgLength(const CFE_MSG_Message_t *MsgPtr);

/*****************************************************************************/
/**
** \brief DEPRECATED: Gets the command code field from a software bus message.
** \deprecated Use CFE_MSG_GetFcnCode
**
** \par Description
**          This routine gets the command code from a software bus message (if
**          SB messages are implemented as CCSDS packets, this will be the function
**          code).
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not
**            include a command code field, then this routine will return a zero.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \return The command code included in the software bus message header (if present).
**         Otherwise, returns a command code value of zero.
**/
uint16 CFE_SB_GetCmdCode(CFE_MSG_Message_t *MsgPtr);

/*****************************************************************************/
/**
** \brief DEPRECATED: Gets the time field from a software bus message.
** \deprecated Use CFE_MSG_GetMsgTime
**
** \par Description
**          This routine gets the time from a software bus message.
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not
**            include a time field, then this routine will return a zero time.
**          - Note default implementation of command messages do not have a time field.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \return The system time included in the software bus message header (if present),
**         otherwise, returns a time value of zero.
**/
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_MSG_Message_t *MsgPtr);
#endif /* CFE_OMIT_DEPRECATED_6_8 */

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
**    coped from the source buffer to the destination buffer, and a NUL termination
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
** \param[out] DestStringPtr    Pointer to destination buffer
** \param[in]  SourceStringPtr  Pointer to source buffer (component of SB message definition)
** \param[in]  DefaultString    Default string to use if source is empty
** \param[in]  DestMaxSize      Size of destination storage buffer (must be at least 2)
** \param[in]  SourceMaxSize    Size of source buffer as defined by the message definition
**
** \return Number of characters copied or error code, see \ref CFEReturnCodes
**
*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString,
                              size_t DestMaxSize, size_t SourceMaxSize);
/**@}*/

#ifndef CFE_OMIT_DEPRECATED_6_8
/** @defgroup CFEAPISBChecksum cFE Checksum Control APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief DEPRECATED:Gets the checksum field from a software bus message.
** \deprecated No use case
**
** \par Description
**          This routine gets the checksum (or other message integrity check
**          value) from a software bus message.  The contents and location of
**          this field will depend on the underlying implementation of software
**          bus messages.  It may be a checksum, a CRC, or some other algorithm.
**          Users should not call this function as part of a message integrity
**          check (call #CFE_SB_ValidateChecksum instead).
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not
**            include a checksum field, then this routine will return a zero.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \return The checksum included in the software bus message header (if present), otherwise,
**         returns a checksum value of zero.
**/
uint16 CFE_SB_GetChecksum(CFE_MSG_Message_t *MsgPtr);

/*****************************************************************************/
/**
** \brief DEPRECATED:Calculates and sets the checksum of a software bus message
** \deprecated Use CFE_MSG_GenerateChecksum
**
** \par Description
**          This routine calculates the checksum of a software bus message according
**          to an implementation-defined algorithm.  Then, it sets the checksum field
**          in the message with the calculated value.  The contents and location of
**          this field will depend on the underlying implementation of software bus
**          messages.  It may be a checksum, a CRC, or some other algorithm.
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not
**            include a checksum field, then this routine will do nothing.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**/
void CFE_SB_GenerateChecksum(CFE_MSG_Message_t *MsgPtr);

/*****************************************************************************/
/**
** \brief DEPRECATED:Validates the checksum of a software bus message.
** \deprecated Use CFE_MSG_ValidateChecksum
**
** \par Description
**          This routine calculates the expected checksum of a software bus message
**          according to an implementation-defined algorithm.  Then, it checks the
**          calculated value against the value in the message's checksum.  If the
**          checksums do not match, this routine will generate an event message
**          reporting the error.
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not
**            include a checksum field this routine will always return false.
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \return Boolean checksum result
** \retval true  The checksum field in the packet is valid.
** \retval false The checksum field in the packet is not valid or the message type is wrong.
**/
bool CFE_SB_ValidateChecksum(CFE_MSG_Message_t *MsgPtr);
/**@}*/
#endif /* CFE_OMIT_DEPRECATED_6_8 */

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
    CFE_SB_MsgId_t Result = CFE_SB_MSGID_WRAP_VALUE(MsgIdValue);
    return Result;
}

#ifndef CFE_OMIT_DEPRECATED_6_8
/*****************************************************************************/
/**
 * \brief Identifies packet type given message ID

 *
 * Provides the packet type associated with the given message ID
 *
 * \return Packet type
 * \retval #CFE_MSG_Type_Cmd     Command packet type
 * \retval #CFE_MSG_Type_Tlm     Telemetry packet type
 * \retval #CFE_MSG_Type_Invalid Invalid/unknown packet type
 */
uint32 CFE_SB_GetPktType(CFE_SB_MsgId_t MsgId);
#endif /* CFE_OMIT_DEPRECATED_6_8 */

/**@}*/

#endif /* CFE_SB_H */
