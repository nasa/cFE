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
 * Declarations and prototypes for cfe_sb_extern_typedefs module
 */

#ifndef CFE_SB_EXTERN_TYPEDEFS_H
#define CFE_SB_EXTERN_TYPEDEFS_H

#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "cfe_resourceid_typedef.h"

#define CFE_SB_SUB_ENTRIES_PER_PKT 20 /**< \brief Configuration parameter used by SBN App */

/**
 * @brief Label definitions associated with CFE_SB_QosPriority_Enum_t
 */
enum CFE_SB_QosPriority
{
    /**
     * @brief Normal priority level
     */
    CFE_SB_QosPriority_LOW = 0,

    /**
     * @brief High priority
     */
    CFE_SB_QosPriority_HIGH = 1
};

/**
 * @brief Selects the priority level for message routing
 *
 * @sa enum CFE_SB_QosPriority
 */
typedef uint8 CFE_SB_QosPriority_Enum_t;

/**
 * @brief Label definitions associated with CFE_SB_QosReliability_Enum_t
 */
enum CFE_SB_QosReliability
{
    /**
     * @brief Normal (best-effort) reliability
     */
    CFE_SB_QosReliability_LOW = 0,

    /**
     * @brief High reliability
     */
    CFE_SB_QosReliability_HIGH = 1
};

/**
 * @brief Selects the reliability level for message routing
 *
 * @sa enum CFE_SB_QosReliability
 */
typedef uint8 CFE_SB_QosReliability_Enum_t;

/**
 * @brief An integer type that should be used for indexing into the Routing Table
 */
typedef uint16 CFE_SB_RouteId_Atom_t;

/**
 * @brief  CFE_SB_MsgId_Atom_t primitive type definition
 *
 * This is an integer type capable of holding any Message ID value
 * Note: This value is limited via #CFE_PLATFORM_SB_HIGHEST_VALID_MSGID
 */
typedef uint32 CFE_SB_MsgId_Atom_t;

/**
 * @brief  CFE_SB_MsgId_t type definition
 *
 * Software Bus message identifier used in many SB APIs
 *
 * Currently this is directly mapped to the underlying holding type (not wrapped) for
 * compatibility with existing usage semantics in apps (mainly switch/case statements)
 *
 * @note In a future version it could become a type-safe wrapper similar to the route index,
 * to avoid message IDs getting mixed between other integer values.
 */
typedef struct
{
    CFE_SB_MsgId_Atom_t Value;
} CFE_SB_MsgId_t;

/** \brief  CFE_SB_PipeId_t to primitive type definition
 *
 * Software Bus pipe identifier used in many SB APIs, as well as SB Telemetry messages
 * and data files.
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_SB_PipeId_t;

/** \brief Quality Of Service Type Definition
**
** Currently an unused parameter in #CFE_SB_SubscribeEx
** Intended to be used for interprocessor communication only
**/
typedef struct
{
    uint8 Priority; /**< \brief  Specify high(1) or low(0) message priority for off-board routing, currently unused */
    uint8 Reliability; /**< \brief  Specify high(1) or low(0) message transfer reliability for off-board routing,
                          currently unused */
} CFE_SB_Qos_t;

#endif /* CFE_SB_EXTERN_TYPEDEFS_H */
