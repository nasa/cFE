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
** File:  ccsds.h
**
** Purpose:
**      Define typedefs and macros for CCSDS packet headers.
**
******************************************************************************/

#ifndef _ccsds_
#define _ccsds_

/*
** Include Files
*/
#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "cfe_msg_hdr.h"

/*
 * COMPATIBILITY TYPEDEFS:
 * These typdefs provide compatibility for existing code.  These should be
 * removed in the next CFE release.
 */
#ifndef CFE_OMIT_DEPRECATED_6_6

typedef CFE_MSG_CommandHeader_t     CCSDS_CmdPkt_t;
typedef CFE_MSG_TelemetryHeader_t   CCSDS_TlmPkt_t;

#endif /* CFE_OMIT_DEPRECATED_6_6 */

#ifndef CFE_OMIT_DEPRECATED_6_8

typedef struct{
   CCSDS_PrimaryHeader_t Pri;
   CCSDS_ExtendedHeader_t ApidQ;
} CCSDS_APIDQHdr_t;
typedef CCSDS_ExtendedHeader_t CCSDS_APIDqualifiers_t;
typedef CCSDS_PrimaryHeader_t  CCSDS_PriHdr_t;
typedef CFE_MSG_CommandSecondaryHeader_t CCSDS_CmdSecHdr_t;
typedef CFE_MSG_TelemetrySecondaryHeader_t CCSDS_TlmSecHdr_t;
typedef CFE_MSG_CommandHeader_t CCSDS_CommandPacket_t;  /* Element names changed, direct access will break */
typedef CFE_MSG_TelemetryHeader_t CCSDS_TelemetryPacket_t;  /* Element names changed, direct access will break */

#endif /* CFE_OMIT_DEPRECATED_6_8 */

/* Macro to convert 16/32 bit types from platform "endianness" to Big Endian */
#ifdef SOFTWARE_BIG_BIT_ORDER
  #define CFE_MAKE_BIG16(n) (n)
  #define CFE_MAKE_BIG32(n) (n)
#else
  #define CFE_MAKE_BIG16(n) ( (((n) << 8) & 0xFF00) | (((n) >> 8) & 0x00FF) )
  #define CFE_MAKE_BIG32(n) ( (((n) << 24) & 0xFF000000) | (((n) << 8) & 0x00FF0000) | (((n) >> 8) & 0x0000FF00) | (((n) >> 24) & 0x000000FF) )
#endif

#endif  /* _ccsds_ */
