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
** File: cfe_sb_msg_id_util.h
**
** Purpose:
**      This header file contains prototypes for private functions and type
**      definitions for SB internal use.
**
** Author:   J. Wilmot/NASA
**
******************************************************************************/

#ifndef _cfe_sb_msg_id_util_
#define _cfe_sb_msg_id_util_

/*
** Includes
*/
#include "common_types.h"

/**
**      For MESSAGE_FORMAT_IS_CCSDS_VER_2 the default layout of the message id is:
**       7 bits from the primary header APID
**       1 bit for the command/telemetry flag 
**       0 bits from the Playback flag
**       8 bits from the secondary header APID qualifier (Subsystem)
**       0 bits from the secondary header APID qualifier as the System
**    = 16 bits total 
**     
**                  Byte 1              Byte 0
**            7 6 5 4 3 2 1 0     7      6 5 4 3 2 1 0
**           +-+-+-+-+-+-+-+-+|--------|+-+-+-+-+-+-+-+
**           | APID Qualifier |C/T flg | Pri Hdr APID |
**           +-+-+-+-+-+-+-+-+|--------|+-+-+-+-+-+-+-+
**   This layout may be modified via the 4 macros
**   CFE_SB_CMD_MESSAGE_TYPE, CFE_SB_RD_APID_FROM_MSGID
**   CFE_SB_RD_SUBSYS_ID_FROM_MSGID and CFE_SB_RD_TYPE_FROM_MSGID
**   
*/

/*
** Macro Definitions
*/

/* 
 * Mission defined bit used to indicate message is a command type. A 0 in this bit position indicates 
 * a telemetry message type. This bit is included in the message id.
 */
#define CFE_SB_CMD_MESSAGE_TYPE 0x00000080 /* 1 bit (position 7) for Cmd/Tlm  */

/* 
 * Mission defined macros to extract message id fields from the  primary and secondary headers
*/
#define CFE_SB_RD_APID_FROM_MSGID(MsgId)         (MsgId & 0x0000007F) /* 0-6(7) bits for Pri Hdr APID */
#define CFE_SB_RD_SUBSYS_ID_FROM_MSGID(MsgId)  ( (MsgId & 0x0000FF00) >> 8) /* bits 8-15(8) bits for APID Subsystem ID */ 
#define CFE_SB_RD_TYPE_FROM_MSGID(MsgId)       ( (MsgId & CFE_SB_CMD_MESSAGE_TYPE) >> 7) /* 1 Cmd/Tlm Bit (bit #7) */

#endif /* _cfe_sb_msg_id_util_ */
/*****************************************************************************/
