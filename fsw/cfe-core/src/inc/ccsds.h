/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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


/* Macro to convert 16/32 bit types from platform "endianness" to Big Endian */
#ifdef SOFTWARE_BIG_BIT_ORDER
  #define CFE_MAKE_BIG16(n) (n)
  #define CFE_MAKE_BIG32(n) (n)
#else
  #define CFE_MAKE_BIG16(n) ( (((n) << 8) & 0xFF00) | (((n) >> 8) & 0x00FF) )
  #define CFE_MAKE_BIG32(n) ( (((n) << 24) & 0xFF000000) | (((n) << 8) & 0x00FF0000) | (((n) >> 8) & 0x0000FF00) | (((n) >> 24) & 0x000000FF) )
#endif


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
** Type Definitions
*/

/**********************************************************************
** Structure definitions for CCSDS headers.  All items in the structure
** must be aligned on 16-bit words.  Bitfields must be avoided since
** some compilers (such as gcc) force them into 32-bit alignment.
**
** CCSDS headers must always be in network byte order per the standard.
** MSB at the lowest address which is commonly refered to as "BIG Endian"
**
** CCSDS Space Packets can be version 1 or version 2.  Version 2 has 
** an additional 32 bits for APID Qualifier fields in the secondary 
** header. The primary header is unchanged.
**
**********************************************************************/

/*----- CCSDS packet primary header. -----*/

typedef struct {

   uint8   StreamId[2];  /* packet identifier word (stream ID) */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x07FF    0  : application ID                            */
      /* 0x0800   11  : secondary header: 0 = absent, 1 = present */
      /* 0x1000   12  : packet type:      0 = TLM, 1 = CMD        */
      /* 0xE000   13  : CCSDS version:    0 = ver 1, 1 = ver 2    */

   uint8   Sequence[2];  /* packet sequence word */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x3FFF    0  : sequence count                            */
      /* 0xC000   14  : segmentation flags:  3 = complete packet  */

   uint8  Length[2];     /* packet length word */
      /*  bits  shift   ------------ description ---------------- */
      /* 0xFFFF    0  : (total packet length) - 7                 */

} CCSDS_PriHdr_t;

/*----- CCSDS command secondary header. -----*/

typedef struct {

   uint16  Command;      /* command secondary header */
      /*  bits  shift   ------------ description ---------------- */
      /* 0x00FF    0  : checksum, calculated by ground system     */
      /* 0x7F00    8  : command function code                     */
      /* 0x8000   15  : reserved, set to 0                        */

} CCSDS_CmdSecHdr_t;

/*----- CCSDS telemetry secondary header. -----*/

typedef struct {

   uint8  Time[CCSDS_TIME_SIZE];

} CCSDS_TlmSecHdr_t;

/*----- CCSDS Endian Flag in the APID Qualifier Field. -----*/
#define CCSDS_BIG_ENDIAN 0
#define CCSDS_LITTLE_ENDIAN 1
#define CCSDS_ENDIAN_MASK 0x0400

/*------ CCSDS packet playback Flag in the APID Qualifier Field. -------*/
/* This field denotes that this packet is a playback of a stored packet */
#define CCSDS_NON_PLAYBACK_PKT 0
#define CCSDS_PLAYBACK_PKT 1
#define CCSDS_PLAYBACK_PKT_MASK 0x0200

/*------ CCSDS packet Electronic Data Sheet version in the APID Qualifier Field. -------*/
/* This is the verion of the data sheet that defines the packet payload format and other */
/* metadata like unit conversions */
#define CCSDS_EDS_MASK 0xF800

/*----- CCSDS Secondary Header APID Qualifers ----*/
typedef struct {

   uint8 APIDQSubsystem[2];

      /*  bits  shift   ------------ description ---------------- */
      /* 0x01FF   0  : Subsystem Id  mission defined              */
      /* 0x0200   9  : Playback flag  0 = original, 1 = playback  */
      /* 0x0400  10  : Endian:   Big = 0, Little (Intel) = 1      */
      /* 0xF800  11  : EDS Version for packet definition used     */
 
   uint8 APIDQSystemId[2];
      /* 0xFFFF   0  : System Id      mission defined             */

} CCSDS_APIDqualifiers_t;

/**
 * \brief CCSDS Primary with APID Qualifier Header Type Definition
 */
typedef struct{
    CCSDS_PriHdr_t         Pri;/**< \brief CCSDS Primary Header #CCSDS_PriHdr_t */
    CCSDS_APIDqualifiers_t ApidQ;/**< \brief CCSDS APID Qualifier Secondary Header #CCSDS_APIDqualifiers_t */
}CCSDS_APIDQHdr_t;

typedef struct
{
    /*
     * In Version 1 mode, the standard / non-APID qualified header is used for all packets
     */
    CCSDS_PriHdr_t      Hdr;    /**< Complete "version 1" (standard) header */

#ifdef MESSAGE_FORMAT_IS_CCSDS_VER_2

    /*
     * In Version 2 mode, the extended / APID qualified header is used for all packets
     */
    CCSDS_APIDqualifiers_t ApidQ;/**< \brief CCSDS APID Qualifier Secondary Header #CCSDS_APIDqualifiers_t */
#endif  /* MESSAGE_FORMAT_IS_CCSDS_VER_2 */

} CCSDS_SpacePacket_t;




/*----- Generic combined command header. -----*/

typedef struct
{
    CCSDS_SpacePacket_t  SpacePacket;   /**< \brief Standard Header on all packets  */
    CCSDS_CmdSecHdr_t    Sec;
} CCSDS_CommandPacket_t;

/*----- Generic combined telemetry header. -----*/

typedef struct
{
    CCSDS_SpacePacket_t  SpacePacket;   /**< \brief Standard Header on all packets */
    CCSDS_TlmSecHdr_t    Sec;
} CCSDS_TelemetryPacket_t;

/*
 * COMPATIBILITY TYPEDEFS:
 * These types were defined by CFE 6.5 and below and applications may still use them.
 * These typdefs provide compatibility for existing code.  These should be
 * removed in the next CFE release.
 */
#ifndef CFE_OMIT_DEPRECATED_6_6

typedef CCSDS_CommandPacket_t     CCSDS_CmdPkt_t;
typedef CCSDS_TelemetryPacket_t   CCSDS_TlmPkt_t;

#endif /* CFE_OMIT_DEPRECATED_6_6 */

/*
** Macro Definitions
*/

/**********************************************************************
** Constant values.
**********************************************************************/

/* Value of packet type for a telemetry packet. */
#define CCSDS_TLM  0
/* Value of packet type for a command packet. */
#define CCSDS_CMD  1

/* Value of secondary header flag if secondary header not present. */
#define CCSDS_NO_SEC_HDR   0
/* Value of secondary header flag if secondary header exists. */
#define CCSDS_HAS_SEC_HDR  1

#define NUM_CCSDS_APIDS        2048
#define NUM_CCSDS_PKT_TYPES    2


/**********************************************************************
** Initial values for CCSDS header fields.
**********************************************************************/

/* Initial value of the sequence count. */
#define CCSDS_INIT_SEQ      0
/* Initial value of the sequence flags. */
#define CCSDS_INIT_SEQFLG   3
/* Initial value of the command function code. */
#define CCSDS_INIT_FC       0
/* Initial value of the command checksum. */
#define CCSDS_INIT_CHECKSUM 0

/* Note: the stream ID and length are always explicitly set for a packet,
** so default values are not required.  */


/**********************************************************************
** Macros for reading and writing bit fields in a 16-bit integer.
** These are used to implement the read and write macros below.
**********************************************************************/

/* Read bits specified by 'mask' from 'word' and shift down by 'shift'. */
#define CCSDS_RD_BITS(word,mask,shift) \
   (((word) & mask) >> shift)

/* Shift 'value' up by 'shift' and write to those bits in 'word' that
** are specified by 'mask'.  Other bits in 'word' are unchanged.   */
#define CCSDS_WR_BITS(word,mask,shift,value) \
   ((word) = (uint16)(((word) & ~mask) | (((value) & (mask >> shift)) << shift)))


/**********************************************************************
** Macros for reading and writing the fields in a CCSDS header.  All
** of the macros are used in a similar way:
**
**   CCSDS_RD_xxx(header)        -- Read field xxx from header.
**   CCSDS_WR_xxx(header,value)  -- Write value to field xxx of header.
**
** Note that 'header' is a reference to the actual header structure,
** not to a pointer to the structure.  If using a pointer, one must
** refer to the structure as *pointer.
**
** The CCSDS_WR_xxx macros may refer to 'header' more than once; thus
** the expression for 'header' must NOT contain any side effects.
**********************************************************************/

/* Read entire stream ID from primary header. */
#define CCSDS_RD_SID(phdr)         (((phdr).StreamId[0] << 8) + ((phdr).StreamId[1]))
/* Write entire stream ID to primary header. */
#define CCSDS_WR_SID(phdr,value)   ( ((phdr).StreamId[0] = (value >> 8)   ) ,\
                                     ((phdr).StreamId[1] = (value & 0xff) ) )

/* Read application ID from primary header. */
#define CCSDS_RD_APID(phdr)         (CCSDS_RD_SID(phdr) & 0x07FF)
/* Write application ID to primary header. */
#define CCSDS_WR_APID(phdr,value)  ((((phdr).StreamId[0] = ((phdr).StreamId[0] & 0xF8) | ((value >> 8) & 0x07))) ,\
                                   (((phdr).StreamId[1]  = ((value)) & 0xff)) )

/* Read secondary header flag from primary header. */
#define CCSDS_RD_SHDR(phdr)         (((phdr).StreamId[0] & 0x08) >> 3)
/* Write secondary header flag to primary header. */
#define CCSDS_WR_SHDR(phdr,value)   ((phdr).StreamId[0] = ((phdr).StreamId[0] & 0xf7) | ((value << 3) & 0x08))

/* Read packet type (0=TLM,1=CMD) from primary header. */
#define CCSDS_RD_TYPE(phdr)         (((phdr).StreamId[0] & 0x10) >> 4)
/* Write packet type (0=TLM,1=CMD) to primary header. */
#define CCSDS_WR_TYPE(phdr,value)   ((phdr).StreamId[0] = ((phdr).StreamId[0] & 0xEF) | ((value << 4) & 0x10))

/* Read CCSDS version from primary header. */
#define CCSDS_RD_VERS(phdr)        (((phdr).StreamId[0] & 0xE0) >> 5)
/* Write CCSDS version to primary header. */
#define CCSDS_WR_VERS(phdr,value)  ((phdr).StreamId[0] = ((phdr).StreamId[0] & 0x1F) | ((value << 5) & 0xE0))

/* Read sequence count from primary header. */
#define CCSDS_RD_SEQ(phdr)         ((((phdr).Sequence[0] & 0x3F) << 8) + ((phdr).Sequence[1]))
/* Write sequence count to primary header. */
#define CCSDS_WR_SEQ(phdr,value)   ((((phdr).Sequence[0] = ((phdr).Sequence[0] & 0xC0) | ((value >> 8) & 0x3f))) ,\
                                   (((phdr).Sequence[1]  = ((value)) & 0xff)) )

/* Read sequence flags from primary header. */
#define CCSDS_RD_SEQFLG(phdr)       (((phdr).Sequence[0] & 0xC0) >> 6)
/* Write sequence flags to primary header. */
#define CCSDS_WR_SEQFLG(phdr,value) ((phdr).Sequence[0] = ((phdr).Sequence[0] & 0x3F) | ((value << 6) & 0xC0) )

/* Read total packet length from primary header. */
#define CCSDS_RD_LEN(phdr)     ( ( (phdr).Length[0] << 8) + (phdr).Length[1] + 7)
/* Write total packet length to primary header. */
#define CCSDS_WR_LEN(phdr,value)   ((((phdr).Length[0] = ((value) - 7) >> 8)) ,\
                                   (((phdr).Length[1] = ((value) - 7) & 0xff)) )

/* Read function code from command secondary header. */
#define CCSDS_RD_FC(shdr)           CCSDS_RD_BITS((shdr).Command, 0x7F00, 8)
/* Write function code to command secondary header. */
#define CCSDS_WR_FC(shdr,value)     CCSDS_WR_BITS((shdr).Command, 0x7F00, 8, value)

/* Read checksum from command secondary header. */
#define CCSDS_RD_CHECKSUM(shdr)     CCSDS_RD_BITS((shdr).Command, 0x00FF, 0)
/* Write checksum to command secondary header. */
#define CCSDS_WR_CHECKSUM(shdr,val) CCSDS_WR_BITS((shdr).Command, 0x00FF, 0, val)

/* Define additional APID Qualifier macros. */

/* These macros will convert between local endianness and network endianness */
/* The packet headers are always in network byte order */
#define CCSDS_RD_EDS_VER(shdr)       ( ((shdr).APIDQSubsystem[0] & 0xF8) >> 3)
#define CCSDS_RD_ENDIAN(shdr)        ( ((shdr).APIDQSubsystem[0] & 0x04) >> 2)
#define CCSDS_RD_PLAYBACK(shdr)      ( ((shdr).APIDQSubsystem[0] & 0x02) >> 1)
#define CCSDS_RD_SUBSYSTEM_ID(shdr)  ( (((shdr).APIDQSubsystem[0] & 0x01) << 8) + ((shdr).APIDQSubsystem[1]))
#define CCSDS_RD_SYSTEM_ID(shdr)     ( ((shdr).APIDQSystemId[0] << 8) + ((shdr).APIDQSystemId[1]))

#define CCSDS_WR_EDS_VER(shdr,val)       ( (shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0x07) | (((val) & 0x1f) << 3) )
#define CCSDS_WR_ENDIAN(shdr,val)        ( (shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0xFB) | (((val) & 0x01) << 2) )
#define CCSDS_WR_PLAYBACK(shdr,val)      ( (shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0xFD) | (((val) & 0x01) << 1) )
                                  
#define CCSDS_WR_SUBSYSTEM_ID(shdr,val)  (((shdr).APIDQSubsystem[0] = ((shdr).APIDQSubsystem[0] & 0xFE) | ((val & 0x0100) >> 8)) ,\
                                         ( (shdr).APIDQSubsystem[1] =  (val & 0x00ff)) )

#define CCSDS_WR_SYSTEM_ID(shdr,val)     (((shdr).APIDQSystemId[0] = ((val & 0xff00) >> 8)),\
                                         ( (shdr).APIDQSystemId[1] =  (val & 0x00ff)) )

/**********************************************************************
** Macros for clearing a CCSDS header to a standard initial state.  All
** of the macros are used in a similar way:
**   CCSDS_CLR_xxx_HDR(header)      -- Clear header of type xxx.
**********************************************************************/

/* Clear primary header. */
#define CCSDS_CLR_PRI_HDR(phdr) \
  ( (phdr).StreamId[0] = 0,\
    (phdr).StreamId[1] = 0,\
    (phdr).Sequence[0] = (CCSDS_INIT_SEQFLG << 6),\
    (phdr).Sequence[1] = 0,\
    (phdr).Length[0] = 0, \
    (phdr).Length[1] = 0 )

#define CCSDS_CLR_SEC_APIDQ(shdr) \
  ( (shdr).APIDQSubsystem[0] = 0,\
    (shdr).APIDQSubsystem[1] = 0,\
    (shdr).APIDQSystemId[0] = 0,\
    (shdr).APIDQSystemId[1] = 0 )

/* Clear command secondary header. */
#define CCSDS_CLR_CMDSEC_HDR(shdr) \
  ( (shdr).Command = (CCSDS_INIT_CHECKSUM << 0) | (CCSDS_INIT_FC << 8) )


#define CCSDS_WR_SEC_HDR_SEC(shdr, value)    shdr.Time[0] = ((value>>24) & 0xFF),  \
                                             shdr.Time[1] = ((value>>16) & 0xFF),  \
                                             shdr.Time[2] = ((value>>8)  & 0xFF),  \
                                             shdr.Time[3] = ((value)     & 0xFF)

#define CCSDS_RD_SEC_HDR_SEC(shdr)           (((uint32)shdr.Time[0]) << 24) | \
                                             (((uint32)shdr.Time[1]) << 16) | \
                                             (((uint32)shdr.Time[2]) << 8)  | \
                                             ((uint32)shdr.Time[3])

/* Clear telemetry secondary header. */
#if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)
  /* 32 bits seconds + 16 bits subseconds */
  #define CCSDS_CLR_TLMSEC_HDR(shdr) \
  ( (shdr).Time[0] = 0,\
    (shdr).Time[1] = 0,\
    (shdr).Time[2] = 0,\
    (shdr).Time[3] = 0,\
    (shdr).Time[4] = 0,\
    (shdr).Time[5] = 0 )


#define CCSDS_WR_SEC_HDR_SUBSEC(shdr, value) shdr.Time[4] = ((value>>8)  & 0xFF),  \
                                             shdr.Time[5] = ((value)     & 0xFF)

#define CCSDS_RD_SEC_HDR_SUBSEC(shdr)        (((uint32)shdr.Time[4]) << 8)  | \
                                             ((uint32)shdr.Time[5])
#elif ((CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_SUBS) ||\
       (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_M_20))
  /* 32 bits seconds + 32 bits subseconds */
  #define CCSDS_CLR_TLMSEC_HDR(shdr) \
  ( (shdr).Time[0] = 0,\
    (shdr).Time[1] = 0,\
    (shdr).Time[2] = 0,\
    (shdr).Time[3] = 0,\
    (shdr).Time[4] = 0,\
    (shdr).Time[5] = 0,\
    (shdr).Time[6] = 0,\
    (shdr).Time[7] = 0 )

#define CCSDS_WR_SEC_HDR_SUBSEC(shdr, value) shdr.Time[4] = ((value>>24) & 0xFF),  \
                                             shdr.Time[5] = ((value>>16) & 0xFF),  \
                                             shdr.Time[6] = ((value>>8)  & 0xFF),  \
                                             shdr.Time[7] = ((value)     & 0xFF)

#define CCSDS_RD_SEC_HDR_SUBSEC(shdr)        (((uint32)shdr.Time[4]) << 24) | \
                                             (((uint32)shdr.Time[5]) << 16) | \
                                             (((uint32)shdr.Time[6]) << 8)  | \
                                             ((uint32)shdr.Time[7])
#endif



/**********************************************************************
** Macros for extracting fields from a stream ID.  All of the macros
** are used in a similar way:
**
**   CCSDS_SID_xxx(sid)          -- Extract field xxx from sid.
**********************************************************************/

/* Extract application ID from stream ID. */
#define CCSDS_SID_APID(sid)   CCSDS_RD_BITS(sid, 0x07FF, 0)

/* Extract secondary header flag from stream ID. */
#define CCSDS_SID_SHDR(sid)   CCSDS_RD_BITS(sid, 0x0800, 11)

/* Extract packet type (0=TLM,1=CMD) from stream ID. */
#define CCSDS_SID_TYPE(sid)   CCSDS_RD_BITS(sid, 0x1000, 12)

/* Extract CCSDS version from stream ID. */
#define CCSDS_SID_VERS(sid)   CCSDS_RD_BITS(sid, 0xE000, 13)


/**********************************************************************
** Macros for frequently used combinations of operations.
**
**   CCSDS_INC_SEQ(phdr)           -- Increment sequence count.
**********************************************************************/

/* Increment sequence count in primary header by 1. */
#define CCSDS_INC_SEQ(phdr) \
   CCSDS_WR_SEQ(phdr, (CCSDS_RD_SEQ(phdr)+1))


/*********************************************************************/

/*
** Exported Functions
*/


/******************************************************************************
**  Function:  CCSDS_LoadCheckSum()
**
**  Purpose:
**    Compute and load a checksum for a CCSDS command packet that has a
**    secondary header.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.  The checksum field in the packet
**               will be modified.
**
**  Return:
**    (none)
*/

void CCSDS_LoadCheckSum (CCSDS_CommandPacket_t *PktPtr);

/******************************************************************************
**  Function:  CCSDS_ValidCheckSum()
**
**  Purpose:
**    Determine whether a checksum in a command packet is valid.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    true if checksum of packet is valid; false if not.
**    A valid checksum is 0.
*/

bool CCSDS_ValidCheckSum (CCSDS_CommandPacket_t *PktPtr);

/******************************************************************************
**  Function:  CCSDS_ComputeCheckSum()
**
**  Purpose:
**    Compute the checksum for a command packet.  The checksum is the XOR of
**    all bytes in the packet; a valid checksum is zero.
**
**  Arguments:
**    PktPtr   : Pointer to header of command packet.  The packet must
**               have a secondary header and the length in the primary
**               header must be correct.
**
**  Return:
**    true if checksum of packet is valid; false if not.
*/

uint8 CCSDS_ComputeCheckSum (CCSDS_CommandPacket_t *PktPtr);


#endif  /* _ccsds_ */
/*****************************************************************************/
