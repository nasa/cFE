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
 * Prototypes for private functions and type definitions for SB
 * routing internal use.
 */

#ifndef CFE_SBR_PRIV_H
#define CFE_SBR_PRIV_H

/*
 * Includes
 */
#include "cfe_sbr.h"

/******************************************************************************
 * Function prototypes
 */

/*---------------------------------------------------------------------------------------*/
/**
 *  \brief Routing map initialization
 */
void CFE_SBR_Init_Map(void);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Associates the given route ID with the given message ID
 *
 * Used for implementations that use a mapping table (typically hash or direct)
 * and need this information to later get the route id from the message id.
 *
 * \note Typically not needed for a search implementation.  Assumes
 *       message ID is valid
 *
 * \param[in] MsgId   Message id to associate with route id
 * \param[in] RouteId Route id to associate with message id
 *
 * \returns Number of collisions
 */
uint32 CFE_SBR_SetRouteId(CFE_SB_MsgId_t MsgId, CFE_SBR_RouteId_t RouteId);

#endif /* CFE_SBR_PRIV_H */
