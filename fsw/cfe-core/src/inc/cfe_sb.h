/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
** File: cfe_sb.h
**
** Purpose:
**      This header file contains all definitions for the cFE Software Bus
**      Application Programmer's Interface.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

#ifndef _cfe_sb_
#define _cfe_sb_

/*
** Includes
*/
#include "cfe_sb_extern_typedefs.h"
#include "osconfig.h"
#include "cfe_psp.h"
#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "ccsds.h"
#include "cfe_time.h"


/*
** Defines
*/
#define CFE_SB_POLL                     0      /**< \brief Option used with #CFE_SB_RcvMsg to request immediate pipe status */
#define CFE_SB_PEND_FOREVER            -1      /**< \brief Option used with #CFE_SB_RcvMsg to force a wait for next message */
#define CFE_SB_SUB_ENTRIES_PER_PKT      20     /**< \brief Configuration parameter used by SBN App */
#define CFE_SB_SUBSCRIPTION             0      /**< \brief Subtype specifier used in #CFE_SB_SingleSubscriptionTlm_t by SBN App */
#define CFE_SB_UNSUBSCRIPTION           1      /**< \brief Subtype specified used in #CFE_SB_SingleSubscriptionTlm_t by SBN App */

#define CFE_SB_INVALID_MSG_ID           0xFFFF /**< \brief Initializer for CFE_SB_MsgId_t values that will not match any real MsgId */

/*
** Macro Definitions
*/
#define CFE_BIT(x)   (1 << (x))               /**< \brief Places a one at bit positions 0 - 31*/
#define CFE_SET(i,x) ((i) |= CFE_BIT(x))      /**< \brief Sets bit x of i */
#define CFE_CLR(i,x) ((i) &= ~CFE_BIT(x))     /**< \brief Clears bit x of i */
#define CFE_TST(i,x) (((i) & CFE_BIT(x)) != 0)/**< \brief true(non zero) if bit x of i is set */

/**
 * Macro that should be used to set memory addresses within software bus messages.
 * For now this does a straight copy, but in a future revision this may translate the
 * raw memory address into a "safe" integer value.  This is particularly important if
 * the message is to be sent off this CPU.
 */
#define CFE_SB_SET_MEMADDR(msgdst,src)       msgdst = (cpuaddr)src

/**
 * Macro that should be used to get memory addresses from software bus messages.
 * This is the inverse operation of CFE_SB_SET_MEMADDR.
 */
#define CFE_SB_GET_MEMADDR(msgsrc)           (cpuaddr)msgsrc

/*
** Pipe option bit fields.
*/
#define CFE_SB_PIPEOPTS_IGNOREMINE 0x00000001 /**< \brief Messages sent by the app that owns this pipe will not be sent to this pipe. */

/*
** Type Definitions
*/
#ifdef MESSAGE_FORMAT_IS_CCSDS

    /**< \brief Generic Software Bus Message Type Definition */
    typedef union {
        CCSDS_PriHdr_t      Hdr;   /**< \brief CCSDS Primary Header #CCSDS_PriHdr_t */
        CCSDS_SpacePacket_t SpacePacket;
        uint32              Dword; /**< \brief Forces minimum of 32-bit alignment for this object */
        uint8               Byte[sizeof(CCSDS_PriHdr_t)];   /**< \brief Allows byte-level access */
    }CFE_SB_Msg_t;
        
    /** \brief Generic Software Bus Command Header Type Definition */
    typedef CCSDS_CommandPacket_t   CFE_SB_CmdHdr_t;

    /** \brief Generic Software Bus Telemetry Header Type Definition */
    typedef CCSDS_TelemetryPacket_t CFE_SB_TlmHdr_t;

    #define CFE_SB_CMD_HDR_SIZE     (sizeof(CFE_SB_CmdHdr_t))/**< \brief Size of #CFE_SB_CmdHdr_t in bytes */
    #define CFE_SB_TLM_HDR_SIZE     (sizeof(CFE_SB_TlmHdr_t))/**< \brief Size of #CFE_SB_TlmHdr_t in bytes */

#endif /* MESSAGE_FORMAT_IS_CCSDS */

/**< \brief  CFE_SB_TimeOut_t to primitive type definition
** 
** Internally used by SB in the #CFE_SB_RcvMsg API. Translated from the
** input parmater named TimeOut which specifies the maximum time in 
** milliseconds that the caller wants to wait for a message.
*/
typedef uint32 CFE_SB_TimeOut_t;

/**< \brief  CFE_SB_PipeId_t to primitive type definition 
** 
** Software Bus pipe identifier used in many SB APIs
*/
typedef uint8  CFE_SB_PipeId_t;

/**< \brief  CFE_SB_MsgPtr_t defined as a pointer to an SB Message */
typedef CFE_SB_Msg_t *CFE_SB_MsgPtr_t;

/**< \brief  CFE_SB_MsgPayloadPtr_t defined as an opaque pointer to a message Payload portion */
typedef uint8 *CFE_SB_MsgPayloadPtr_t;

/**< \brief  CFE_SB_ZeroCopyId_t to primitive type definition 
** 
** Software Zero Copy handle used in many SB APIs
*/
typedef cpuaddr CFE_SB_ZeroCopyHandle_t;

/**< \brief Quality Of Service Type Definition
**
** Currently an unused parameter in #CFE_SB_SubscribeEx
** Intended to be used for interprocessor communication only
**/ 
typedef  struct {
    uint8 Priority;/**< \brief  Specify high(1) or low(0) message priority for off-board routing, currently unused */
    uint8 Reliability;/**< \brief  Specify high(1) or low(0) message transfer reliability for off-board routing, currently unused */
}CFE_SB_Qos_t;

extern CFE_SB_Qos_t CFE_SB_Default_Qos;/**< \brief  Defines a default priority and reliabilty for off-board routing */


/**< \brief Message Sender Identification Type Definition
**
** Parameter used in #CFE_SB_GetLastSenderId API which allows the receiver of a message 
** to validate the sender of the message.
**/
typedef struct {
    uint32  ProcessorId;/**< \brief Processor Id from which the message was sent */
    char    AppName[OS_MAX_API_NAME];/**< \brief Application that sent the message */
} CFE_SB_SenderId_t;

/*
**  cFE SB Application Programmer Interface's (API's)
*/

/*****************************************************************************/
/** 
** \brief Creates a new software bus pipe.
**
** \par Description
**          This routine creates and initializes an input pipe that the calling 
**          application can use to receive software bus messages.  By default, no 
**          messages are routed to the new pipe.  So, the application must use 
**          #CFE_SB_Subscribe() to specify which messages it wants to receive on 
**          this pipe.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  PipeIdPtr    A pointer to a variable of type #CFE_SB_PipeId_t, 
**                          which will be filled in with the pipe ID information 
**                          by the #CFE_SB_CreatePipe routine. 
**
** \param[in]  Depth        The maximum number of messages that will be allowed on 
**                          this pipe at one time. 
**
** \param[in]  PipeName     A string to be used to identify this pipe in error messages 
**                          and routing information telemetry.  The string must be no 
**                          longer than #OS_MAX_API_NAME.  Longer strings will be truncated. 
**
** \param[out] *PipeIdPtr   The identifier for the created pipe. 
**
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS          \endcode
** \retcode #CFE_SB_BAD_ARGUMENT   \retdesc \copydoc CFE_SB_BAD_ARGUMENT  \endcode
** \retcode #CFE_SB_MAX_PIPES_MET  \retdesc \copydoc CFE_SB_MAX_PIPES_MET \endcode
** \retcode #CFE_SB_PIPE_CR_ERR    \retdesc \copydoc CFE_SB_PIPE_CR_ERR   \endcode
** \endreturns
**
** \sa #CFE_SB_DeletePipe #CFE_SB_GetPipeOpts #CFE_SB_SetPipeOpts
**/
int32  CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr,
                         uint16  Depth,
                         const char *PipeName);

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
** \returns
** \retcode #CFE_SUCCESS           \retdesc  \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_SB_BAD_ARGUMENT   \retdesc  \copydoc CFE_SB_BAD_ARGUMENT \endcode
** \endreturns
**
** \sa #CFE_SB_CreatePipe #CFE_SB_GetPipeOpts #CFE_SB_SetPipeOpts
**/
int32  CFE_SB_DeletePipe(CFE_SB_PipeId_t PipeId);

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
** \returns
** \retcode #CFE_SUCCESS           \retdesc  \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_SB_BAD_ARGUMENT   \retdesc  \copydoc CFE_SB_BAD_ARGUMENT \endcode
** \endreturns
**
** \sa #CFE_SB_CreatePipe #CFE_SB_DeletePipe #CFE_SB_GetPipeOpts #CFE_SB_PIPEOPTS_IGNOREMINE
**/
int32  CFE_SB_SetPipeOpts(CFE_SB_PipeId_t     PipeId,
                          uint8               Opts);

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
** \returns
** \retcode #CFE_SUCCESS           \retdesc  \copydoc CFE_SUCCESS         \endcode
** \retcode #CFE_SB_BAD_ARGUMENT   \retdesc  \copydoc CFE_SB_BAD_ARGUMENT \endcode
** \endreturns
**
** \sa #CFE_SB_CreatePipe #CFE_SB_DeletePipe #CFE_SB_SetPipeOpts #CFE_SB_PIPEOPTS_IGNOREMINE
**/
int32  CFE_SB_GetPipeOpts(CFE_SB_PipeId_t     PipeId,
                          uint8               *OptPtr);

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
**                          the messages. Most callers will use #CFE_SB_Default_Qos
**                          for this parameter.
**
** \param[in]  MsgLim       The maximum number of messages with this Message ID to
**                          allow in this pipe at the same time. 
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc  \copydoc CFE_SUCCESS           \endcode
** \retcode #CFE_SB_MAX_MSGS_MET    \retdesc  \copydoc CFE_SB_MAX_MSGS_MET   \endcode
** \retcode #CFE_SB_MAX_DESTS_MET   \retdesc  \copydoc CFE_SB_MAX_DESTS_MET  \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc  \copydoc CFE_SB_BAD_ARGUMENT   \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc  \copydoc CFE_SB_BUF_ALOC_ERR   \endcode
** \endreturns
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeLocal, #CFE_SB_Unsubscribe, #CFE_SB_UnsubscribeLocal
**/
int32  CFE_SB_SubscribeEx(CFE_SB_MsgId_t      MsgId,
                          CFE_SB_PipeId_t     PipeId,
                          CFE_SB_Qos_t        Quality,
                          uint16              MsgLim);

/*****************************************************************************/
/** 
** \brief Subscribe to a message on the software bus with default parameters
**
** \par Description
**          This routine adds the specified pipe to the destination list for 
**          the specified message ID.  This is the same as #CFE_SB_SubscribeEx 
**          with the Quality field set to #CFE_SB_Default_Qos and MsgLim set 
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
** \returns
** \retcode #CFE_SUCCESS            \retdesc  \copydoc CFE_SUCCESS           \endcode
** \retcode #CFE_SB_MAX_MSGS_MET    \retdesc  \copydoc CFE_SB_MAX_MSGS_MET   \endcode
** \retcode #CFE_SB_MAX_DESTS_MET   \retdesc  \copydoc CFE_SB_MAX_DESTS_MET  \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc  \copydoc CFE_SB_BAD_ARGUMENT   \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc  \copydoc CFE_SB_BUF_ALOC_ERR   \endcode
** \endreturns
**
** \sa #CFE_SB_SubscribeEx, #CFE_SB_SubscribeLocal, #CFE_SB_Unsubscribe, #CFE_SB_UnsubscribeLocal
**/
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t  MsgId, CFE_SB_PipeId_t PipeId);

/*****************************************************************************/
/** 
** \brief Subscribe to a message while keeping the request local to a cpu
**
** \par Description
**          This routine adds the specified pipe to the destination list for 
**          the specified message ID.  This is similar to #CFE_SB_SubscribeEx 
**          with the Quality field set to #CFE_SB_Default_Qos and MsgLim set 
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
** \returns
** \retcode #CFE_SUCCESS            \retdesc  \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_MAX_MSGS_MET    \retdesc  \copydoc CFE_SB_MAX_MSGS_MET    \endcode
** \retcode #CFE_SB_MAX_DESTS_MET   \retdesc  \copydoc CFE_SB_MAX_DESTS_MET   \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc  \copydoc CFE_SB_BAD_ARGUMENT    \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc  \copydoc CFE_SB_BUF_ALOC_ERR   \endcode
** \endreturns
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeEx, #CFE_SB_Unsubscribe, #CFE_SB_UnsubscribeLocal
**/
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId,
                            CFE_SB_PipeId_t PipeId,
                            uint16 MsgLim);

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
** \returns
** \retcode #CFE_SUCCESS            \retdesc  \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_NO_SUBSCRIBERS  \retdesc  \copydoc CFE_SB_NO_SUBSCRIBERS  \endcode
** \retcode #CFE_SB_INTERNAL_ERR    \retdesc  \copydoc CFE_SB_INTERNAL_ERR    \endcode
** \endreturns
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeEx, #CFE_SB_SubscribeLocal, #CFE_SB_UnsubscribeLocal
**/
int32  CFE_SB_Unsubscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);

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
** \returns
** \retcode #CFE_SUCCESS             \retdesc \copydoc CFE_SUCCESS             \endcode
** \retcode #CFE_SB_NO_SUBSCRIBERS   \retdesc \copydoc CFE_SB_NO_SUBSCRIBERS   \endcode
** \retcode #CFE_SB_INTERNAL_ERR     \retdesc \copydoc CFE_SB_INTERNAL_ERR     \endcode
** \endreturns
**
** \sa #CFE_SB_Subscribe, #CFE_SB_SubscribeEx, #CFE_SB_SubscribeLocal, #CFE_SB_Unsubscribe
**/
int32 CFE_SB_UnsubscribeLocal(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId);

/*****************************************************************************/
/** 
** \brief Send a software bus message
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
**            this message, that task may get to run before #CFE_SB_SendMsg 
**            returns control to the caller.
**          - This function tracks and increments the source sequence counter 
**            of a telemetry message.
**
** \param[in]  MsgPtr       A pointer to the message to be sent.  This must point
**                          to the first byte of the software bus message header
**                          (#CFE_SB_Msg_t). 
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc   \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc   \copydoc CFE_SB_BAD_ARGUMENT    \endcode
** \retcode #CFE_SB_MSG_TOO_BIG     \retdesc   \copydoc CFE_SB_MSG_TOO_BIG     \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc   \copydoc CFE_SB_BUF_ALOC_ERR    \endcode
** \endreturns
**
** \sa #CFE_SB_RcvMsg, #CFE_SB_ZeroCopySend, #CFE_SB_PassMsg
**/
int32  CFE_SB_SendMsg(CFE_SB_Msg_t   *MsgPtr);

/*****************************************************************************/
/** 
** \brief Passes a software bus message
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
**          - Unlike #CFE_SB_SendMsg this routine will preserve the source
**            sequence counter in a telemetry message.
**
** \param[in]  MsgPtr       A pointer to the message to be sent.  This must point
**                          to the first byte of the software bus message header
**                          (#CFE_SB_Msg_t). 
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc   \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc   \copydoc CFE_SB_BAD_ARGUMENT    \endcode
** \retcode #CFE_SB_MSG_TOO_BIG     \retdesc   \copydoc CFE_SB_MSG_TOO_BIG     \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc   \copydoc CFE_SB_BUF_ALOC_ERR    \endcode
** \endreturns
**
** \sa #CFE_SB_RcvMsg, #CFE_SB_ZeroCopySend, #CFE_SB_SendMsg
**/
int32  CFE_SB_PassMsg(CFE_SB_Msg_t   *MsgPtr);

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
** \param[in]  BufPtr       A pointer to a local variable of type #CFE_SB_MsgPtr_t.  
**                          Typically a caller declares a ptr of type CFE_SB_Msg_t 
**                          (i.e. CFE_SB_Msg_t *Ptr) then gives the address of that 
**                          pointer (&Ptr) as this parmeter. After a successful 
**                          receipt of a message, *BufPtr will point to the first 
**                          byte of the software bus message header. This should be 
**                          used as a read-only pointer (in systems with an MMU, 
**                          writes to this pointer may cause a memory protection fault).
**                          The *BufPtr is valid only until the next call to 
**                          CFE_SB_RcvMsg for the same pipe.                            
**                          
** \param[in]  PipeId       The pipe ID of the pipe containing the message to be obtained. 
**
** \param[in]  TimeOut      The number of milliseconds to wait for a new message if the
**                          pipe is empty at the time of the call.  This can also be set
**                          to #CFE_SB_POLL for a non-blocking receive or 
**                          #CFE_SB_PEND_FOREVER to wait forever for a message to arrive.
**
** \param[out] *BufPtr      A pointer to the message obtained from the pipe. Valid 
**                          only until the next call to CFE_SB_RcvMsg for the same pipe.
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc  \copydoc CFE_SUCCESS          \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc  \copydoc CFE_SB_BAD_ARGUMENT  \endcode
** \retcode #CFE_SB_TIME_OUT        \retdesc  \copydoc CFE_SB_TIME_OUT      \endcode
** \retcode #CFE_SB_PIPE_RD_ERR     \retdesc  \copydoc CFE_SB_PIPE_RD_ERR   \endcode
** \retcode #CFE_SB_NO_MESSAGE      \retdesc  \copydoc CFE_SB_NO_MESSAGE    \endcode
** \endreturns
**
** \sa #CFE_SB_SendMsg, #CFE_SB_ZeroCopySend
**/
int32  CFE_SB_RcvMsg(CFE_SB_MsgPtr_t  *BufPtr,
                     CFE_SB_PipeId_t  PipeId,
                     int32            TimeOut);

/*****************************************************************************/
/** 
** \brief Retrieve the application Info of the sender for the last message.
**
** \par Description
**          This routine can be used after a successful #CFE_SB_RcvMsg call 
**          to find out which application sent the message that was received.
**
** \par Assumptions, External Events, and Notes:
**          Note - If an error occurs in this API, the *Ptr value may be NULL or
**          random. Therefore, it is recommended that the return code be tested 
**          for CFE_SUCCESS before reading the sender information.
**
** \param[in]  Ptr       A pointer to a local variable of type #CFE_SB_SenderId_t.  
**                       Typically a caller declares a ptr of type CFE_SB_SenderId_t 
**                       (i.e. CFE_SB_SenderId_t *Ptr) then gives the address of that 
**                       pointer (&Ptr) for this parameter. After a successful call   
**                       to this API, *Ptr will point to the first byte of the 
**                       CFE_SB_SenderId_t structure containing the sender information  
**                       for the last message received on the given pipe. This should 
**                       be used as a read-only pointer (in systems with an MMU, writes  
**                       to this pointer may cause a memory protection fault).  The *Ptr  
**                       is valid only until the next call to CFE_SB_RcvMsg for the 
**                       same pipe.
**
** \param[in]  PipeId    The pipe ID of the pipe the message was taken from. 
**
** \returns
** \retcode The sender's application ID  \retdesc \endcode
** \endreturns
**
** \sa 
**/
uint32  CFE_SB_GetLastSenderId(CFE_SB_SenderId_t **Ptr,CFE_SB_PipeId_t  PipeId);

/*****************************************************************************/
/** 
** \brief Get a buffer pointer to use for "zero copy" SB sends.
**
** \par Description
**          This routine can be used to get a pointer to one of the software bus' 
**          internal memory buffers that are used for sending messages.  The caller 
**          can use this memory buffer to build an SB message, then send it using 
**          the #CFE_SB_ZeroCopySend function.  This interface is more complicated 
**          than the normal #CFE_SB_ZeroCopySend interface, but it avoids an extra 
**          copy of the message from the user's memory buffer to the software bus 
**          internal buffer.  The "zero copy" interface can be used to improve 
**          performance in high-rate, high-volume software bus traffic.
**
** \par Assumptions, External Events, and Notes:
**          -# The pointer returned by #CFE_SB_ZeroCopyGetPtr is only good for one 
**             call to #CFE_SB_ZeroCopySend.  
**          -# Applications should be written as if #CFE_SB_ZeroCopyGetPtr is 
**             equivalent to a \c malloc() and #CFE_SB_ZeroCopySend is equivalent to 
**             a \c free().
**          -# Applications must not de-reference the message pointer (for reading 
**             or writing) after the call to #CFE_SB_ZeroCopySend. 
**
** \param[in]  MsgSize  The size of the SB message buffer the caller wants 
**                      (including the SB message header). 
**
** \param[out] BufferHandle  A handle that must be supplied when sending or releasing
**                           in zero copy mode. 
**
** \returns
** \retstmt A pointer to a memory buffer that can be used to build one SB message 
**         for use with #CFE_SB_ZeroCopySend.  \endstmt
** \endreturns
**
** \sa #CFE_SB_ZeroCopyReleasePtr, #CFE_SB_ZeroCopySend
**/
CFE_SB_Msg_t  *CFE_SB_ZeroCopyGetPtr(uint16  MsgSize,
                                     CFE_SB_ZeroCopyHandle_t *BufferHandle);

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
**             that pointer for a #CFE_SB_ZeroCopySend  
**
** \param[in]  Ptr2Release  A pointer to the SB internal buffer.  This must be a 
**                          pointer returned by a call to #CFE_SB_ZeroCopyGetPtr, 
**                          but never used in a call to #CFE_SB_ZeroCopySend. 
**
** \param[in]  BufferHandle  This must be the handle supplied with the pointer 
**                           when #CFE_SB_ZeroCopyGetPtr was called. 
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc   \copydoc CFE_SUCCESS  \endcode
** \retcode #CFE_SB_BUFFER_INVALID  \retdesc   \copydoc CFE_SB_BUFFER_INVALID  \endcode
** \endreturns
**
** \sa #CFE_SB_ZeroCopyGetPtr, #CFE_SB_ZeroCopySend
**/
int32 CFE_SB_ZeroCopyReleasePtr(CFE_SB_Msg_t  *Ptr2Release,
                                CFE_SB_ZeroCopyHandle_t         BufferHandle);

/*****************************************************************************/
/** 
** \brief Send an SB message in "zero copy" mode.
**
** \par Description
**          This routine sends a message that has been created directly in an 
**          internal SB message buffer by an application (after a call to 
**          #CFE_SB_ZeroCopyGetPtr).  This interface is more complicated than 
**          the normal #CFE_SB_SendMsg interface, but it avoids an extra copy of 
**          the message from the user's memory buffer to the software bus 
**          internal buffer.  The "zero copy" interface can be used to improve 
**          performance in high-rate, high-volume software bus traffic.
**
** \par Assumptions, External Events, and Notes:
**          -# The pointer returned by #CFE_SB_ZeroCopyGetPtr is only good for 
**             one call to #CFE_SB_ZeroCopySend.  
**          -# Callers must not use the same SB message buffer for multiple sends.  
**          -# Applications should be written as if #CFE_SB_ZeroCopyGetPtr is 
**             equivalent to a \c malloc() and #CFE_SB_ZeroCopySend is equivalent 
**             to a \c free().  
**          -# Applications must not de-reference the message pointer (for reading 
**             or writing) after the call to #CFE_SB_ZeroCopySend.  
**          -# This function tracks and increments the source sequence counter 
**             of a telemetry message.
**
** \param[in]  MsgPtr  A pointer to the SB message to be sent. 
**
** \param[in]  BufferHandle  The handle supplied with the #CFE_SB_ZeroCopyGetPtr call. 
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc   \copydoc CFE_SUCCESS  \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc   \copydoc CFE_SB_BAD_ARGUMENT    \endcode
** \retcode #CFE_SB_MSG_TOO_BIG     \retdesc   \copydoc CFE_SB_MSG_TOO_BIG     \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc   \copydoc CFE_SB_BUF_ALOC_ERR    \endcode
** \retcode #CFE_SB_BUFFER_INVALID  \retdesc   \copydoc CFE_SB_BUFFER_INVALID  \endcode
** \endreturns
**
** \sa #CFE_SB_SendMsg, #CFE_SB_RcvMsg, #CFE_SB_ZeroCopyReleasePtr, #CFE_SB_ZeroCopyGetPtr  
**/
int32 CFE_SB_ZeroCopySend(CFE_SB_Msg_t   *MsgPtr,
                          CFE_SB_ZeroCopyHandle_t          BufferHandle);

/*****************************************************************************/
/** 
** \brief Pass an SB message in "zero copy" mode.
**
** \par Description
**          This routine sends a message that has been created directly in an 
**          internal SB message buffer by an application (after a call to 
**          #CFE_SB_ZeroCopyGetPtr).  This interface is more complicated than 
**          the normal #CFE_SB_SendMsg interface, but it avoids an extra copy of 
**          the message from the user's memory buffer to the software bus 
**          internal buffer.  The "zero copy" interface can be used to improve 
**          performance in high-rate, high-volume software bus traffic. This 
**          version is intended to pass messages not generated by the caller
**          (to preserve the source sequence count).
**
** \par Assumptions, External Events, and Notes:
**          -# The pointer returned by #CFE_SB_ZeroCopyGetPtr is only good for 
**             one call to #CFE_SB_ZeroCopySend or #CFE_SB_ZeroCopyPass.  
**          -# Callers must not use the same SB message buffer for multiple sends.  
**          -# Applications should be written as if #CFE_SB_ZeroCopyGetPtr is 
**             equivalent to a \c malloc() and #CFE_SB_ZeroCopyPass is equivalent 
**             to a \c free().  
**          -# Applications must not de-reference the message pointer (for reading 
**             or writing) after the call to #CFE_SB_ZeroCopyPass.  
**          -# Unlike #CFE_SB_ZeroCopySend this routine will preserve the source
**             sequence counter in a telemetry message.
**
** \param[in]  MsgPtr  A pointer to the SB message to be sent. 
**
** \param[in]  BufferHandle  The handle supplied with the #CFE_SB_ZeroCopyGetPtr call. 
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc   \copydoc CFE_SUCCESS  \endcode
** \retcode #CFE_SB_BAD_ARGUMENT    \retdesc   \copydoc CFE_SB_BAD_ARGUMENT    \endcode
** \retcode #CFE_SB_MSG_TOO_BIG     \retdesc   \copydoc CFE_SB_MSG_TOO_BIG     \endcode
** \retcode #CFE_SB_BUF_ALOC_ERR    \retdesc   \copydoc CFE_SB_BUF_ALOC_ERR    \endcode
** \retcode #CFE_SB_BUFFER_INVALID  \retdesc   \copydoc CFE_SB_BUFFER_INVALID  \endcode
** \endreturns
**
** \sa #CFE_SB_PassMsg, #CFE_SB_ZeroCopySend, #CFE_SB_ZeroCopyReleasePtr, #CFE_SB_ZeroCopyGetPtr  
**/
int32 CFE_SB_ZeroCopyPass(CFE_SB_Msg_t   *MsgPtr,
                          CFE_SB_ZeroCopyHandle_t          BufferHandle);


/*****************************************************************************/
/** 
** \brief Initialize a buffer for a software bus message.
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
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode 
**/
void CFE_SB_InitMsg(void           *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16         Length,
                    bool           Clear );

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
** \returns
** \retstmt A pointer to the first byte of user data within the software bus message. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
void *CFE_SB_GetUserData(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Get the message ID of a software bus message.
**
** \par Description
**          This routine returns the message ID from a software bus message.
**
** \par Assumptions, External Events, and Notes:
**          None  
**
** \param[in]  MsgPtr  A pointer to the buffer that contains the software bus message.
**
** \returns
** \retstmt The software bus Message ID from the message header. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_SetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
CFE_SB_MsgId_t CFE_SB_GetMsgId(const CFE_SB_Msg_t *MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the message ID of a software bus message.
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
**
** \returns
** \retstmt The software bus Message ID from the message header. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr,
                     CFE_SB_MsgId_t MsgId);

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
** \returns
** \retstmt The size (in bytes) of the user data in the software bus message. \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
uint16 CFE_SB_GetUserDataLength(const CFE_SB_Msg_t *MsgPtr);

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
**
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_SetUserDataLength(CFE_SB_MsgPtr_t MsgPtr,uint16 DataLength);

/*****************************************************************************/
/** 
** \brief Gets the total length of a software bus message.
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
** \returns
** \retstmt The total size (in bytes) of the software bus message, including headers.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
uint16 CFE_SB_GetTotalMsgLength(const CFE_SB_Msg_t *MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the total length of a software bus message.
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
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_SetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr,uint16 TotalLength);

/*****************************************************************************/
/** 
** \brief Gets the time field from a software bus message.
**
** \par Description
**          This routine gets the time from a software bus message.    
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not 
**            include a time field, then this routine will return a zero time.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header. 
** \returns
** \retstmt The system time included in the software bus message header (if present), 
**         otherwise, returns a time value of zero.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the time field in a software bus message.
**
** \par Description
**          This routine sets the time of a software bus message.  Most applications 
**          will want to use #CFE_SB_TimeStampMsg instead of this function.  But, 
**          when needed, #CFE_SB_SetMsgTime can be used to send a group of SB messages 
**          with identical time stamps.    
**
** \par Assumptions, External Events, and Notes:
**          - If the underlying implementation of software bus messages does not include 
**            a time field, then this routine will do nothing to the message contents 
**            and will return #CFE_SB_WRONG_MSG_TYPE.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \param[in]  Time        The time to include in the message.  This will usually be a time 
**                         returned by the function #CFE_TIME_GetTime().
**
** \returns
** \retcode #CFE_SUCCESS            \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_WRONG_MSG_TYPE  \retdesc \copydoc CFE_SB_WRONG_MSG_TYPE  \endcode
** \endreturns
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
int32 CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr,
                       CFE_TIME_SysTime_t Time);

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
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_SetCmdCode, #CFE_SB_InitMsg 
**/
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Gets the command code field from a software bus message.
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
** \returns
** \retstmt The command code included in the software bus message header (if present).
**         Otherwise, returns a command code value of zero.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_SetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize 
**/
uint16 CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Sets the command code field in a software bus message.
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
** \returns
** \retcode #CFE_SUCCESS           \retdesc \copydoc CFE_SUCCESS            \endcode
** \retcode #CFE_SB_WRONG_MSG_TYPE \retdesc \copydoc CFE_SB_WRONG_MSG_TYPE  \endcode
** \endreturns
**
** \sa #CFE_SB_SetMsgId, #CFE_SB_SetUserDataLength, #CFE_SB_SetTotalMsgLength,
**     #CFE_SB_SetMsgTime, #CFE_SB_TimeStampMsg, #CFE_SB_GetCmdCode, #CFE_SB_InitMsg 
**/
int32 CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr,
                        uint16 CmdCode);

/*****************************************************************************/
/** 
** \brief Gets the checksum field from a software bus message.
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
** \returns
** \retstmt The checksum included in the software bus message header (if present), otherwise,
**         returns a checksum value of zero.  \endstmt
** \endreturns
**
** \sa #CFE_SB_GetUserData, #CFE_SB_GetMsgId, #CFE_SB_GetUserDataLength, #CFE_SB_GetTotalMsgLength,
**     #CFE_SB_GetMsgTime, #CFE_SB_GetCmdCode, #CFE_SB_GetChecksum, #CFE_SB_MsgHdrSize,
**     #CFE_SB_ValidateChecksum, #CFE_SB_GenerateChecksum 
**/
uint16 CFE_SB_GetChecksum(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Calculates and sets the checksum of a software bus message
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
**
** \sa #CFE_SB_ValidateChecksum, #CFE_SB_GetChecksum
**/
void CFE_SB_GenerateChecksum(CFE_SB_MsgPtr_t MsgPtr);

/*****************************************************************************/
/** 
** \brief Validates the checksum of a software bus message.
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
**            include a checksum field, then this routine will always return \c true.  
**
** \param[in]  MsgPtr      A pointer to the buffer that contains the software bus message.
**                         This must point to the first byte of the message header.
**
** \returns
** \retcode true  \retdesc The checksum field in the packet is valid.   \endcode
** \retcode false \retdesc The checksum field in the packet is not valid or the message type is wrong. \endcode
** \endreturns
**
** \sa #CFE_SB_GenerateChecksum, #CFE_SB_GetChecksum
**/
bool CFE_SB_ValidateChecksum(CFE_SB_MsgPtr_t MsgPtr);

/******************************************************************************
**  Function:  CFE_SB_MessageStringGet()
**
**  Purpose:
**    Copies a string out of a software bus message
**
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
**  Arguments:
**    DestStringPtr - pointer to destination buffer
**    SourceStringPtr - pointer to source buffer (component of SB message definition)
**    DefaultString - alternative string to use if the source is an empty string
**    DestMaxSize - size of the destination storage buffer (must be at least 2)
**    SourceMaxSize - size of the source buffer as defined by the message definition
**
**  Returns:
**    Length of result - number of characters copied, not including the terminating NUL.
**       This should match the value that would be returned by "strlen(DestStringPtr)"
**
*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString, uint32 DestMaxSize, uint32 SourceMaxSize);

/******************************************************************************
**  Function:  CFE_SB_MessageStringSet()
**
**  Purpose:
**    Copies a string into a software bus message
**
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
**    NOTE - if the source string buffer is already guaranteed to be null terminated,
**    then there is no difference between the C library "strncpy()" function and this
**    implementation.  It is only necessary to use this when termination of the source
**    buffer is not guaranteed.
**
**  Arguments:
**    DestStringPtr - pointer to destination buffer
**    SourceStringPtr - pointer to source buffer (component of SB message definition)
**    DestMaxSize - size of the destination storage buffer (must be at least 2)
**    SourceMaxSize - size of the source buffer as defined by the message definition
**
**  Returns:
**    Length of result - number of actual data characters copied
**       This should match the value that would be returned by "strlen(DestStringPtr)"
**
*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, uint32 DestMaxSize, uint32 SourceMaxSize);

/**
 * @brief Identifies whether a two CFE_SB_MsgId_t values are equal
 *
 * @par Description
 *    In cases where the CFE_SB_MsgId_t type is not a simple integer
 *    type, it may not be possible to do a direct equality check.
 *    This inline function provides an abstraction for the equality
 *    check between two CFE_SB_MsgId_t values.
 *
 *    Applications should transition to using this function to compare
 *    MsgId values for equality to remain compatible with future versions
 *    of cFE.
 *
 * @return true if equality checks passed, false otherwise.
 */
static inline bool CFE_SB_MsgId_Equal(CFE_SB_MsgId_t MsgId1, CFE_SB_MsgId_t MsgId2)
{
    return (MsgId1 == MsgId2);
}

/*****************************************************************************/
/**
 * @brief Converts a CFE_SB_MsgId_t to a normal integer
 *
 * @par Description
 *    In cases where the CFE_SB_MsgId_t type is not a simple integer
 *    type, it is not possible to directly display the value in a
 *    printf-style statement, use it in a switch() statement, or other
 *    similar use cases.
 *
 *    This inline function provides the ability to map a CFE_SB_MsgId_t
 *    type back into a simple integer value.
 *
 *    Applications should transition to using this function wherever a
 *    CFE_SB_MsgId_t type needs to be used as an integer.
 *
 * @par Assumptions and Notes:
 *    This negates the type safety that was gained by using a non-
 *    integer type for the CFE_SB_MsgId_t value.  This should only be used
 *    in specific cases such as UI display (printf, events, etc) where the
 *    value is being sent externally.  Any internal API calls should be
 *    updated to use the CFE_SB_MsgId_t type directly, rather than an
 *    integer type.
 *
 * @return Integer representation of the CFE_SB_MsgId_t
 */
static inline CFE_SB_MsgId_Atom_t CFE_SB_MsgIdToValue(CFE_SB_MsgId_t MsgId)
{
    return MsgId;
}

/*****************************************************************************/
/**
 * @brief Converts a normal integer into a CFE_SB_MsgId_t
 *
 * @par Description:
 *    In cases where the CFE_SB_MsgId_t type is not a simple integer
 *    type, it is not possible to directly use an integer value
 *    supplied via a define or similar method.
 *
 *    This inline function provides the ability to map an integer value
 *    into a corresponding CFE_SB_MsgId_t value.
 *
 *    Applications should transition to using this function wherever an
 *    integer needs to be used for a CFE_SB_MsgId_t.
 *
 * @par Assumptions and Notes:
 *    This negates the type safety that was gained by using a non-
 *    integer type for the CFE_SB_MsgId_t value.  This should only be
 *    used in specific cases where the value is coming from an external
 *    source.  Any internal API calls should be updated to return the
 *    CFE_SB_MsgId_t type directly, rather than an integer type.
 *
 * @return CFE_SB_MsgId_t representation of the integer
 */
static inline CFE_SB_MsgId_t CFE_SB_ValueToMsgId(CFE_SB_MsgId_Atom_t MsgIdValue)
{
    return MsgIdValue;
}


#endif  /* _cfesb_ */
/*****************************************************************************/
