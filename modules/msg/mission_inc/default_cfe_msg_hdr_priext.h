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
 * Define cFS standard full header
 *  - Avoid direct access for portability, use APIs
 *  - Used to construct message structures
 */

#ifndef _cfe_msg_hdr_
#define _cfe_msg_hdr_

/*
 * Include Files
 */

#include "common_types.h"
#include "ccsds_hdr.h"
#include "cfe_msg_sechdr.h"

/*
 * Type Definitions
 */

/**********************************************************************
 * Structure definitions for full header
 *
 * These are based on historically supported definitions and not
 * an open source standard.
 */

/**
 * \brief Full CCSDS header
 */
typedef struct
{
    CCSDS_PrimaryHeader_t  Pri; /**< \brief CCSDS Primary Header */
    CCSDS_ExtendedHeader_t Ext; /**< \brief CCSDS Extended Header */
} CCSDS_SpacePacket_t;

/**
 * \brief cFS generic base message
 */
typedef union
{
    CCSDS_SpacePacket_t CCSDS;                             /**< \brief CCSDS Header (Pri or Pri + Ext) */
    uint8               Byte[sizeof(CCSDS_SpacePacket_t)]; /**< \brief Byte level access */
} CFE_MSG_Message_t;

/**
 * \brief cFS command header
 */
typedef struct
{

    CFE_MSG_Message_t                Msg; /**< \brief Base message */
    CFE_MSG_CommandSecondaryHeader_t Sec; /**< \brief Secondary header */

} CFE_MSG_CommandHeader_t;

/**
 * \brief cFS telemetry header
 */
typedef struct
{

    CFE_MSG_Message_t                  Msg; /**< \brief Base message */
    CFE_MSG_TelemetrySecondaryHeader_t Sec; /**< \brief Secondary header */

} CFE_MSG_TelemetryHeader_t;

#endif /* _cfe_msg_hdr_ */
