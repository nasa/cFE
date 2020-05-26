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

#ifndef INCLUDE_CCSDS_MSGFORMAT_TYPES_H
#define INCLUDE_CCSDS_MSGFORMAT_TYPES_H

#include <common_types.h>

/*
** Type Definitions
*/

typedef struct
{
    uint8 Checksum;
} CCSDS_MsgFormat_Checksum_t;


typedef struct
{
    uint8 Code;
} CCSDS_MsgFormat_FunctionCode_t;

typedef struct
{
    uint16 Number;
} CCSDS_MsgFormat_Sequence_t;


typedef struct
{
    uint16 ExtraByteCount;
} CCSDS_MsgFormat_Length_t;


typedef struct
{
    uint32 Seconds;
    uint16 Subseconds16;
} CCSDS_MsgFormat_Time_t;

typedef struct
{
    uint16 Apid;
    uint8  CmdFlag; /* set to 1 for command address, 0 for telemetry address */
} CCSDS_MsgFormat_MsgAddress_t;



/*----- CCSDS packet primary header. -----*/

typedef struct {

   uint8   JStreamId[2];  /* packet identifier word (stream ID) */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x07FF    0  : application ID                            */
      /* 0x0800   11  : secondary header: 0 = absent, 1 = present */
      /* 0x1000   12  : packet type:      0 = TLM, 1 = CMD        */
      /* 0xE000   13  : CCSDS version:    0 = ver 1, 1 = ver 2    */

   uint8   JSequence[2];  /* packet sequence word */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x3FFF    0  : sequence count                            */
      /* 0xC000   14  : segmentation flags:  3 = complete packet  */

   uint8  JLength[2];     /* packet length word */
      /*  bits  shift   ------------ description ---------------- */
      /* 0xFFFF    0  : (total packet length) - 7                 */

} CCSDS_MsgFormat_BaseHeader_t;

/*
 * Macros to aid in checking the type bits within StreamId[0]
 */
#define CCSDS_MSGFORMAT_STREAMID_TYPE_MASK        0xF8
#define CCSDS_MSGFORMAT_STREAMID_TYPE_COMMAND     0x18
#define CCSDS_MSGFORMAT_STREAMID_TYPE_TELEMETRY   0x08



/*----- CCSDS command secondary header. -----*/

typedef struct {

   uint8 JFunctionCode; /* Command Function Code */
                       /* bits shift ---------description-------- */
                       /* 0x7F  0    Command function code        */
                       /* 0x80  7    Reserved                     */

   uint8 JChecksum;     /* Command checksum  (all bits, 0xFF)      */

} CCSDS_MsgFormat_CmdSecHdr_t;

/*----- CCSDS telemetry secondary header. -----*/

typedef struct {

   uint8  JTime[8];

} CCSDS_MsgFormat_TlmSecHdr_t;


/*----- Generic combined command header. -----*/

typedef struct
{
    CCSDS_MsgFormat_BaseHeader_t  Base;   /**< \brief Standard Header on all packets  */
    CCSDS_MsgFormat_CmdSecHdr_t   Sec;
} CCSDS_MsgFormat_CommandPacket_t;

/*----- Generic combined telemetry header. -----*/

typedef struct
{
    CCSDS_MsgFormat_BaseHeader_t  Base;   /**< \brief Standard Header on all packets */
    CCSDS_MsgFormat_TlmSecHdr_t   Sec;
} CCSDS_MsgFormat_TelemetryPacket_t;



#endif /* INCLUDE_CCSDS_MSGFORMAT_TYPES_H */
