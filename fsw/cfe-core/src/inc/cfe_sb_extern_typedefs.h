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

#ifndef _CFE_SB_EXTERN_TYPEDEFS_H_
#define _CFE_SB_EXTERN_TYPEDEFS_H_

/* This header may be generated from an EDS file, 
 * tools are available and the feature is enabled */
#ifdef CFE_EDS_ENABLED_BUILD

/* Use the EDS generated version of these types */
#include "cfe_sb_eds_typedefs.h"

#else
/* Use the local definitions of these types */

#include "common_types.h"
#include "cfe_mission_cfg.h"

/**
 * @brief Label definitions associated with CFE_SB_QosPriority_Enum_t
 */
enum CFE_SB_QosPriority
{

   /**
    * @brief Normal priority level
    */
   CFE_SB_QosPriority_LOW                             = 0,

   /**
    * @brief High priority
    */
   CFE_SB_QosPriority_HIGH                            = 1
};

/**
 * @brief Selects the priorty level for message routing
 *
 *
 * @sa enum CFE_SB_QosPriority
 */
typedef uint8                                            CFE_SB_QosPriority_Enum_t;


/**
 * @brief Label definitions associated with CFE_SB_QosReliability_Enum_t
 */
enum CFE_SB_QosReliability
{

   /**
    * @brief Normal (best-effort) reliability
    */
   CFE_SB_QosReliability_LOW                          = 0,

   /**
    * @brief High reliability
    */
   CFE_SB_QosReliability_HIGH                         = 1
};

/**
 * @brief Selects the reliability level for message routing
 *
 *
 * @sa enum CFE_SB_QosReliability
 */
typedef uint8                                            CFE_SB_QosReliability_Enum_t;

/**
 * @brief An integer type that should be used for indexing into the Routing Table
 */
typedef uint16  CFE_SB_RouteId_Atom_t;

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
typedef CFE_SB_MsgId_Atom_t CFE_SB_MsgId_t;

#endif /* CFE_EDS_ENABLED_BUILD */

#endif /* _CFE_SB_EXTERN_TYPEDEFS_H_ */

