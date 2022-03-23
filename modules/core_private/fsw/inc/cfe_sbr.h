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
 *      Prototypes for private functions and type definitions for SB
 *      routing internal use.
 */

#ifndef CFE_SBR_H
#define CFE_SBR_H

/*
 * Includes
 */
#include "common_types.h"
#include "cfe_sbr_api_typedefs.h"
#include "cfe_msg_api_typedefs.h"
#include "cfe_sb_destination_typedef.h"
#include "cfe_sb_api_typedefs.h"

#include "cfe_platform_cfg.h"

/******************************************************************************
 * Function prototypes
 */

/**
 *  \brief Initialize software bus routing module
 */
void CFE_SBR_Init(void);

/**
 *  \brief Add a route for the given message id
 *
 *  Called for the first subscription to a message ID, uses up one
 *  element in the routing table.  Assumes check for existing
 *  route was already performed or routes could leak
 *
 *  \param[in]  MsgId         Message ID of the route to add
 *  \param[out] CollisionsPtr Number of collisions (if not null)
 *
 *  \returns Route ID, will be invalid if route can not be added
 */
CFE_SBR_RouteId_t CFE_SBR_AddRoute(CFE_SB_MsgId_t MsgId, uint32 *CollisionsPtr);

/**
 *  \brief Obtain the route id given a message id
 *
 *  \param[in] MsgId Message ID of the route to get
 *
 *  \returns Route ID, will be invalid if can't be returned
 */
CFE_SBR_RouteId_t CFE_SBR_GetRouteId(CFE_SB_MsgId_t MsgId);

/**
 *  \brief Obtain the message id given a route id
 *
 *  \param[in] RouteId Route ID of the message id to get
 *
 *  \returns Message ID, will be invalid if cant be returned
 */
CFE_SB_MsgId_t CFE_SBR_GetMsgId(CFE_SBR_RouteId_t RouteId);

/**
 *  \brief Obtain the destination list head pointer given a route id
 *
 *  \param[in] RouteId Route ID
 *
 *  \returns Destination list head pointer for the given route id.
 *           Will be null if route doesn't exist or no subscribers.
 */
CFE_SB_DestinationD_t *CFE_SBR_GetDestListHeadPtr(CFE_SBR_RouteId_t RouteId);

/**
 * \brief Set the destination list head pointer for given route id
 *
 * \param[in] RouteId Route Id
 * \param[in] DestPtr Destination list head pointer
 */
void CFE_SBR_SetDestListHeadPtr(CFE_SBR_RouteId_t RouteId, CFE_SB_DestinationD_t *DestPtr);

/**
 * \brief Increment the sequence counter associated with the supplied route ID
 *
 * \param[in] RouteId Route ID
 */
void CFE_SBR_IncrementSequenceCounter(CFE_SBR_RouteId_t RouteId);

/**
 * \brief Get the sequence counter associated with the supplied route ID
 *
 * \param[in] RouteId Route ID
 *
 * \returns the sequence counter
 */
CFE_MSG_SequenceCount_t CFE_SBR_GetSequenceCounter(CFE_SBR_RouteId_t RouteId);

/**
 * \brief Call the supplied callback function for all routes
 *
 * Invokes callback for each route in the table.  Message ID order
 * depends on the routing table implementation.  Possibilities include
 * in subscription order and in order if incrementing message ids.
 *
 * \param[in]     CallbackPtr Function to invoke for each matching ID
 * \param[in]     ArgPtr      Opaque argument to pass to callback function
 * \param[in,out] ThrottlePtr Throttling structure, NULL for no throttle
 */
void CFE_SBR_ForEachRouteId(CFE_SBR_CallbackPtr_t CallbackPtr, void *ArgPtr, CFE_SBR_Throttle_t *ThrottlePtr);

/******************************************************************************
** Inline functions
*/

/**
 * \brief Identifies whether a given CFE_SBR_RouteId_t is valid
 *
 * Implements a basic sanity check on the value provided
 *
 * \returns true if sanity checks passed, false otherwise.
 */
static inline bool CFE_SBR_IsValidRouteId(CFE_SBR_RouteId_t RouteId)
{
    return (RouteId.RouteId != 0 && RouteId.RouteId <= CFE_PLATFORM_SB_MAX_MSG_IDS);
}

/**
 * \brief Converts from raw value to CFE_SBR_RouteId_t
 *
 * Converts the supplied "bare number" into a type-safe CFE_SBR_RouteId_t value
 *
 * \returns A CFE_SBR_RouteId_t
 */
static inline CFE_SBR_RouteId_t CFE_SBR_ValueToRouteId(CFE_SB_RouteId_Atom_t Value)
{
    return ((CFE_SBR_RouteId_t) {.RouteId = 1 + Value});
}

/**
 * \brief Converts from CFE_SBR_RouteId_t to raw value
 *
 * Converts the supplied route id into a "bare number" suitable for performing
 * array lookups or other tasks for which the holding structure cannot be used directly.
 *
 * Use with caution, as this removes the type safety information from the value.
 *
 * \note It is assumed the value has already been validated using CFE_SB_IsValidRouteId()
 *
 * \returns The underlying value
 */
static inline CFE_SB_RouteId_Atom_t CFE_SBR_RouteIdToValue(CFE_SBR_RouteId_t RouteId)
{
    return (RouteId.RouteId - 1);
}

#endif /* CFE_SBR_H */
