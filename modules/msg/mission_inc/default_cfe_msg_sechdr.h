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
 * Define cFS standard secondary header
 *  - Avoid direct access for portability, use APIs
 *  - Used to construct message structures
 */

#ifndef _cfe_msg_sechdr_
#define _cfe_msg_sechdr_

/*
 * Include Files
 */

#include "common_types.h"
#include "cfe_mission_cfg.h"

/*
 * Defines
 */

/* These go away w/ single framework implementation */
/* CCSDS_TIME_SIZE is specific to the selected CFE_SB time format */
#if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)
/* 32 bits seconds + 16 bits subseconds */
#define CCSDS_TIME_SIZE 6
#elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_SUBS)
/* 32 bits seconds + 32 bits subseconds */
#define CCSDS_TIME_SIZE 8
#elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_M_20)
/* 32 bits seconds + 20 bits microsecs + 12 bits reserved */
#define CCSDS_TIME_SIZE 8
#else
/* unknown format */
#error unable to define CCSDS_TIME_SIZE!
#endif

/*
 * Type Definitions
 */

/**********************************************************************
 * Structure definitions for secondary headers
 *
 * These are based on historically supported definitions and not
 * an open source standard.
 */

/**
 * \brief cFS command secondary header
 */
typedef struct
{

    uint8 FunctionCode; /**< \brief Command Function Code */
                        /* bits shift ---------description-------- */
                        /* 0x7F  0    Command function code        */
                        /* 0x80  7    Reserved                     */

    uint8 Checksum; /**< \brief Command checksum  (all bits, 0xFF)      */

} CFE_MSG_CommandSecondaryHeader_t;

/**
 * \brief cFS telemetry secondary header
 */
typedef struct
{

    uint8 Time[CCSDS_TIME_SIZE]; /**< \brief Time sized for selected format */

} CFE_MSG_TelemetrySecondaryHeader_t;

#endif /* _cfe_msg_sechdr_ */
