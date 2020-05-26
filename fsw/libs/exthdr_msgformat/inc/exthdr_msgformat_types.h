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

#ifndef INCLUDE_EXTHDR_MSGFORMAT_TYPES_H
#define INCLUDE_EXTHDR_MSGFORMAT_TYPES_H

#include <common_types.h>
#include <ccsds_msgformat_types.h>

/*
** Type Definitions
*/

/*
 * Field encodings for sequence, length, function codes, time, are all the same as basic format
 */
typedef CCSDS_MsgFormat_Checksum_t        ExtHdr_MsgFormat_Checksum_t;
typedef CCSDS_MsgFormat_FunctionCode_t    ExtHdr_MsgFormat_FunctionCode_t;
typedef CCSDS_MsgFormat_Sequence_t        ExtHdr_MsgFormat_Sequence_t;
typedef CCSDS_MsgFormat_Length_t          ExtHdr_MsgFormat_Length_t;
typedef CCSDS_MsgFormat_Time_t            ExtHdr_MsgFormat_Time_t;

/*
 * Address has additional qualifiers
 */
typedef struct
{
    CCSDS_MsgFormat_MsgAddress_t Base;
    uint16 SubSysId;
    uint16 SystemId;
} ExtHdr_MsgFormat_MsgAddress_t;

/*----- CCSDS Secondary Header APID Qualifers ----*/
typedef struct
{

   uint8 APIDQSubsystem[2];

      /*  bits  shift   ------------ description ---------------- */
      /* 0x01FF   0  : Subsystem Id  mission defined              */
      /* 0x0200   9  : Playback flag  0 = original, 1 = playback  */
      /* 0x0400  10  : Endian:   Big = 0, Little (Intel) = 1      */
      /* 0xF800  11  : EDS Version for packet definition used     */

   uint8 APIDQSystemId[2];
      /* 0xFFFF   0  : System Id      mission defined             */

} ExtHdr_MsgFormat_APIDqualifiers_t;



/*----- CCSDS packet primary header. -----*/

typedef struct
{
    /*
     * This extends the basic header
     */
    CCSDS_MsgFormat_BaseHeader_t Base;

    /*
     * In Version 2 mode, the extended / APID qualified header is used for all packets
     */
    ExtHdr_MsgFormat_APIDqualifiers_t ApidQ;/**< \brief CCSDS APID Qualifier Secondary Header #CCSDS_APIDqualifiers_t */

} ExtHdr_MsgFormat_BaseHeader_t;


/*----- Generic combined command header. -----*/

/* This uses the same tertiary headers as the basic (non-extended) format */

typedef struct
{
    ExtHdr_MsgFormat_BaseHeader_t  Base;   /**< \brief Standard Header on all packets  */
    CCSDS_MsgFormat_CmdSecHdr_t      Sec;
} ExtHdr_MsgFormat_CommandPacket_t;

/*----- Generic combined telemetry header. -----*/

typedef struct
{
    ExtHdr_MsgFormat_BaseHeader_t  Base;   /**< \brief Standard Header on all packets */
    CCSDS_MsgFormat_TlmSecHdr_t      Sec;
} ExtHdr_MsgFormat_TelemetryPacket_t;



#endif /* INCLUDE_EXTHDR_MSGFORMAT_TYPES_H */
