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
 * Define CCSDS packet header types
 *  - Avoid direct access for portability, use APIs
 *  - Used to construct message structures
 */

#ifndef _ccsds_hdr_
#define _ccsds_hdr_

/*
 * Include Files
 */

#include "common_types.h"
#include "cfe_mission_cfg.h"

/*
 * Type Definitions
 */

/**********************************************************************
 * Structure definitions for CCSDS headers.
 *
 * CCSDS headers must always be in network byte order per the standard.
 * MSB at the lowest address which is commonly refered to as "BIG Endian"
 *
 */

/**
 * \brief CCSDS packet primary header
 */
typedef struct CCSDS_PrimaryHeader {

   uint8   StreamId[2];  /**< \brief packet identifier word (stream ID) */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x07FF    0  : application ID                            */
      /* 0x0800   11  : secondary header: 0 = absent, 1 = present */
      /* 0x1000   12  : packet type:      0 = TLM, 1 = CMD        */
      /* 0xE000   13  : CCSDS version:    0 = ver 1, 1 = ver 2    */

   uint8   Sequence[2];  /**< \brief packet sequence word */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x3FFF    0  : sequence count                            */
      /* 0xC000   14  : segmentation flags:  3 = complete packet  */

   uint8  Length[2];     /**< \brief packet length word */
      /*  bits  shift   ------------ description ---------------- */
      /* 0xFFFF    0  : (total packet length) - 7                 */

} CCSDS_PrimaryHeader_t;

/**
 * \brief CCSDS packet extended header
 */
typedef struct CCSDS_ExtendedHeader {

   uint8 Subsystem[2]; /**< \brief subsystem qualifier */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x01FF   0  : Subsystem Id  mission defined              */
      /* 0x0200   9  : Playback flag  0 = original, 1 = playback  */
      /* 0x0400  10  : Endian:   Big = 0, Little (Intel) = 1      */
      /* 0xF800  11  : EDS Version for packet definition used     */
 
   uint8 SystemId[2]; /**< \brief system qualifier */
      /* 0xFFFF   0  : System Id      mission defined             */

} CCSDS_ExtendedHeader_t;

#endif  /* _ccsds_hdr_ */
