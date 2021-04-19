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
 * Purpose:
 *      This header file contains all definitions for the cFE Software Bus
 *      Application Programmer's Interface.
 *
 * Author:   R.McGraw/SSI
 *
 */

#ifndef CFE_SB_API_TYPEDEFS_H
#define CFE_SB_API_TYPEDEFS_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_sb_extern_typedefs.h"
#include "cfe_msg_api_typedefs.h"
#include "cfe_resourceid_api_typedefs.h"
#include "cfe_msg_hdr.h"

/*
** Defines
*/
#define CFE_SB_POLL           0  /**< \brief Option used with #CFE_SB_ReceiveBuffer to request immediate pipe status */
#define CFE_SB_PEND_FOREVER   -1 /**< \brief Option used with #CFE_SB_ReceiveBuffer to force a wait for next message */
#define CFE_SB_SUBSCRIPTION   0  /**< \brief Subtype specifier used in #CFE_SB_SingleSubscriptionTlm_t by SBN App */
#define CFE_SB_UNSUBSCRIPTION 1  /**< \brief Subtype specified used in #CFE_SB_SingleSubscriptionTlm_t by SBN App */

/* ------------------------------------------------------ */
/* Macro Constants for use with the CFE_SB_MsgId_t type   */
/* ------------------------------------------------------ */

/**
 * \brief Translation macro to convert from MsgId integer values to opaque/abstract API values
 *
 * This conversion exists in macro form to allow compile-time evaluation for constants, and
 * should not be used directly in application code.
 *
 * For applications, use the CFE_SB_ValueToMsgId() inline function instead.
 *
 * \sa CFE_SB_ValueToMsgId()
 */
#define CFE_SB_MSGID_WRAP_VALUE(val) ((CFE_SB_MsgId_t)(val))

/**
 * \brief Translation macro to convert to MsgId integer values from opaque/abstract API values
 *
 * This conversion exists in macro form to allow compile-time evaluation for constants, and
 * should not be used directly in application code.
 *
 * For applications, use the CFE_SB_MsgIdToValue() inline function instead.
 *
 * \sa CFE_SB_MsgIdToValue()
 */
#define CFE_SB_MSGID_UNWRAP_VALUE(mid) ((CFE_SB_MsgId_Atom_t)(mid))

/**
 * \brief Reserved value for CFE_SB_MsgId_t that will not match any valid MsgId
 *
 * This rvalue macro can be used for static/compile-time data initialization to ensure that
 * the initialized value does not alias to a valid MsgId object.
 */
#define CFE_SB_MSGID_RESERVED CFE_SB_MSGID_WRAP_VALUE(-1)

/**
 * \brief A literal of the CFE_SB_MsgId_t type representing an invalid ID
 *
 * This value should be used for runtime initialization of CFE_SB_MsgId_t values.
 *
 * \note This may be a compound literal in a future revision.  Per C99, compound
 * literals are lvalues, not rvalues, so this value should not be used in
 * static/compile-time data initialization.  For static data initialization
 * purposes (rvalue), #CFE_SB_MSGID_RESERVED should be used instead.
 * However, in the current implementation, they are equivalent.
 */
#define CFE_SB_INVALID_MSG_ID CFE_SB_MSGID_RESERVED

/**
 * \brief Cast/Convert a generic CFE_ResourceId_t to a CFE_SB_PipeId_t
 */
#define CFE_SB_PIPEID_C(val) ((CFE_SB_PipeId_t)CFE_RESOURCEID_WRAP(val))

/**
 * \brief  A CFE_SB_PipeId_t value which is always invalid
 *
 * This may be used as a safe initializer for CFE_SB_PipeId_t values
 */
#define CFE_SB_INVALID_PIPE CFE_SB_PIPEID_C(CFE_RESOURCEID_UNDEFINED)

/*
** Pipe option bit fields.
*/
#define CFE_SB_PIPEOPTS_IGNOREMINE \
    0x00000001 /**< \brief Messages sent by the app that owns this pipe will not be sent to this pipe. */

#define CFE_SB_DEFAULT_QOS ((CFE_SB_Qos_t) {0}) /**< \brief Default Qos macro */

/*
** Type Definitions
*/

/** \brief Software Bus generic message */
typedef union CFE_SB_Msg
{
    CFE_MSG_Message_t Msg;        /**< \brief Base message type without enforced alignment */
    long long int     LongInt;    /**< \brief Align to support Long Integer */
    long double       LongDouble; /**< \brief Align to support Long Double */
} CFE_SB_Buffer_t;

#ifndef CFE_OMIT_DEPRECATED_6_8

/** \brief Deperecated type to minimize required changes */
typedef CFE_SB_Buffer_t CFE_SB_Msg_t;

/** \brief Deperecated type to minimize required changes */
typedef CFE_MSG_CommandHeader_t CFE_SB_CmdHdr_t;

/** \brief Deperecated type to minimize required changes */
typedef CFE_MSG_TelemetryHeader_t CFE_SB_TlmHdr_t;

#define CFE_SB_CMD_HDR_SIZE (sizeof(CFE_MSG_CommandHeader_t))   /**< \brief Size of command header */
#define CFE_SB_TLM_HDR_SIZE (sizeof(CFE_MSG_TelemetryHeader_t)) /**< \brief Size of telemetry header */

/** \brief  Pointer to an SB Message */
typedef CFE_MSG_Message_t *CFE_SB_MsgPtr_t;

/** \brief  CFE_SB_MsgPayloadPtr_t defined as an opaque pointer to a message Payload portion */
typedef uint8 *CFE_SB_MsgPayloadPtr_t;

#define CFE_SB_Default_Qos CFE_SB_DEFAULT_QOS /**< \deprecated use CFE_SB_DEFAULT_QOS */

#define CFE_SB_CMD_HDR_SIZE (sizeof(CFE_MSG_CommandHeader_t))   /**< \brief Size of command header */
#define CFE_SB_TLM_HDR_SIZE (sizeof(CFE_MSG_TelemetryHeader_t)) /**< \brief Size of telemetry header */

#endif /* CFE_OMIT_DEPRECATED_6_8 */

#endif /* CFE_SB_API_TYPEDEFS_H */
