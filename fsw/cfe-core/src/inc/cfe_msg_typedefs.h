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
 * Message type defines
 *  - Separate from API so these can be adjusted for custom implementations
 */

#ifndef _cfe_msg_typedefs_
#define _cfe_msg_typedefs_

/*
 * Includes
 */
#include "common_types.h"
#include "cfe_error.h"

/*
 * Defines
 */
/* Error bits won't fit a new module so reuse SB error codes */
#define CFE_MSG_BAD_ARGUMENT    CFE_SB_BAD_ARGUMENT    /**< \brief Error - bad argument */
#define CFE_MSG_NOT_IMPLEMENTED CFE_SB_NOT_IMPLEMENTED /**< \brief Error - not implemented */
#define CFE_MSG_WRONG_MSG_TYPE  CFE_SB_WRONG_MSG_TYPE  /**< \brief Error - wrong type */

/*
 * Types
 */
typedef size_t CFE_MSG_Size_t;          /**< \brief Message size (CCSDS needs uint32 for max size) */
typedef uint32 CFE_MSG_Checksum_t;      /**< \brief Message checksum (Oversized to avoid redefine) */
typedef uint16 CFE_MSG_FcnCode_t;       /**< \brief Message function code */
typedef uint16 CFE_MSG_HeaderVersion_t; /**< \brief Message header version */
typedef uint16 CFE_MSG_ApId_t;          /**< \brief Message application ID */
typedef uint16 CFE_MSG_SequenceCount_t; /**< \brief Message sequence count */
typedef uint16 CFE_MSG_EDSVersion_t;    /**< \brief Message EDS version */
typedef uint16 CFE_MSG_Subsystem_t;     /**< \brief Message subsystem */
typedef uint16 CFE_MSG_System_t;        /**< \brief Message system */

/** \brief Message type */
typedef enum CFE_MSG_Type
{
    CFE_MSG_Type_Invalid, /**< \brief Message type invalid, undefined, not implemented */
    CFE_MSG_Type_Cmd,     /**< \brief Command message type */
    CFE_MSG_Type_Tlm      /**< \brief Telemetry message type */
} CFE_MSG_Type_t;

/** \brief Segmentation flags */
typedef enum CFE_MSG_SegmentationFlag
{
    CFE_MSG_SegFlag_Invalid,    /**< \brief Invalid segmentation flag */
    CFE_MSG_SegFlag_Continue,   /**< \brief Continuation segment of User Data */
    CFE_MSG_SegFlag_First,      /**< \brief First segment of User Data */
    CFE_MSG_SegFlag_Last,       /**< \brief Last segment of User Data */
    CFE_MSG_SegFlag_Unsegmented /**< \brief Unsegemented data */
} CFE_MSG_SegmentationFlag_t;

/** \brief Endian flag */
typedef enum CFE_MSG_Endian
{
    CFE_MSG_Endian_Invalid, /**< \brief Invalid endian setting */
    CFE_MSG_Endian_Big,     /**< \brief Big endian */
    CFE_MSG_Endian_Little   /**< \brief Little endian */
} CFE_MSG_Endian_t;

/** \brief Playback flag */
typedef enum CFE_MSG_PlaybackFlag
{
    CFE_MSG_PlayFlag_Invalid,  /**< \brief Invalid playback setting */
    CFE_MSG_PlayFlag_Original, /**< \brief Original */
    CFE_MSG_PlayFlag_Playback  /**< \brief Playback */
} CFE_MSG_PlaybackFlag_t;

#endif /* _cfe_msg_typedefs_ */
