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
** File: cfe_sb_msg_id_util.c
**
** Purpose:
**      This file contains 'access' macros and functions for reading and
**      writing message ID header fields.
**      
**      The function prototypes are in cfe_sb.h except for ConvertMsgIdToMsgKey
**      which is in cfe_sb_priv.h
*
**      The MsgId is a mission defined message identifier to publish or subscribe to that must be 
**      unique within the system(s). CFE_SB_MsgId_t is a uint32 that can be created from any 
**      combination of bits from the primary header SID (StreamId) and the secondary header APID Qualifiers
**      
**      Implementation is based on CCSDS Space Packet Protocol 133.0.B-1 with Technical Corrigendum 2, September 2012
**      The extended secondary header is expected in an upcoming revision of 133.0.B-1
**
**      For  MESSAGE_FORMAT_IS_CCSDS_VER_2 the default setup will combine:
**       1 bit for the command/telemetry flag 
**       7 bits from the primary header APID
**       0 bits from the Playback flag
**       8 bits from the secondary header APID qualifier (Subsystem)
**       0 bits from the secondary header APID qualifier as the System
**    = 16 bits total 
**     
**
**     The APID qualifier System field can be populated in the Secondary header but
**     but will be ignored in the default case for SB/SBN routing purposes. It is suggested 
**     that the CCSDS Spacecraft ID be used for that field.
**
** Notes: The following 4 terms have been or are used in the cFS architecture and implementation
**         
**      StreamId - First 16 bits of CCSDS Space Packet Protocol (SPP) 133.0-B.1c2 Blue Book 
**                 packet primary header. It contains the 3 bit Version Number, 1 bit Packet Type ID,
**                 1 bit Secondary Header flag, and 11 bit Application Process ID
**                 It was used in earlier cFS implementaions and defined here for historical reference
**                 It is NOT exposed to user applications.
**
**      MsgId    - Unique numeric message identifier within a mission namespace. It is used by cFS
**                 applications to the identify messages for publishing and subscribing
**                 It is used by the SB API and encoded in a mission defended way in the header of 
**                 all cFS messages.
**                 It is exposed to all cFS applications
**
**      ApId     - CCSDS Application Process Id field in the primary header. 
**                 It has default bit mask of 0x07FF and is part of the cFS message Id
**                 It should not be confused with the cFE Executive Services (ES) term appId which
**                 identifies the software application/component
**                 It is NOT exposed to user applications.
**
**      MsgIdkey - This is a unique numeric key within a mission namespace that is used with  
**                 cFS software bus internal structures. 
**                 It is algorithmically created in a mission defined way from the MsgId to support
**                 efficient lookup and mapping implementations 
**                 It is NOT exposed to user applications.
**
** Author:   J. Wilmot/NASA
**
******************************************************************************/


/*
** Include Files
*/
#include "cfe_mission_cfg.h"
#include "ccsds.h"
#include "cfe_sb.h"
#include "osapi.h"
#include "cfe_error.h"
#include "cfe_sb_priv.h"
#include "cfe_sb_msg_id_util.h"


/******************************************************************************
**  Function:  CFE_SB_ConvertMsgIdtoMsgKey
**
**  Purpose:
**    Convert the full message Id to the mission defined MsgKey format
**    No conversion is needed for the default implementation as it
**    is limited to less than 16 bits by CFE_SB_HIGHEST_VALID_MSGID
**     If CFE_SB_HIGHEST_VALID_MSGID is greater than 16 bits this function
**    may need modification to ensure SB internal data structures are a
**    reasonable size.
**
**  Arguments:
**    Message ID 
**
**  Return:
**    Converted MsgKey in SB internal format
*/
CFE_SB_MsgKey_t CFE_SB_ConvertMsgIdtoMsgKey( CFE_SB_MsgId_t MsgId)
{
    return CFE_SB_ValueToMsgKey(MsgId);
}/* CFE_SB_ConvertMsgIdtoMsgKey */

/******************************************************************************
**  Function:  CFE_SB_GetMsgId
**
**  Purpose:
**    Convert the CCSDS SPP APID in the packet to the internal MsgId ID format
**       used for SB APIs and routing.
**
**    For backward compatability with the existing CCSDS SPP version 1 code base
**    the function name has not been changed
**
**  Arguments:
**    MsgPtr - Pointer to a CCSDS SPP message packet
**
**  Return:
**    The Message Id in the message packet converted to the cFS MsgId
*/
CFE_SB_MsgId_t CFE_SB_GetMsgId(const CFE_SB_Msg_t *MsgPtr)
{
   CFE_SB_MsgId_t MsgId = 0;

#ifdef MESSAGE_FORMAT_IS_CCSDS

#ifndef MESSAGE_FORMAT_IS_CCSDS_VER_2  
    MsgId = CCSDS_RD_SID(MsgPtr->Hdr);
#else

    uint32            SubSystemId;

    MsgId = CCSDS_RD_APID(MsgPtr->Hdr); /* Primary header APID  */
     
    if ( CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_CMD)
      MsgId = MsgId | CFE_SB_CMD_MESSAGE_TYPE;  

    /* Add in the SubSystem ID as needed */
    SubSystemId = CCSDS_RD_SUBSYSTEM_ID(MsgPtr->SpacePacket.ApidQ);
    MsgId = (MsgId | (SubSystemId << 8));

/* Example code to add in the System ID as needed. */
/*   The default is to init this field to the Spacecraft ID but ignore for routing.   */
/*   To fully implement this field would require significant SB optimization to avoid */
/*   prohibitively large routing and index tables. */
/*      uint16            SystemId;                              */
/*      SystemId = CCSDS_RD_SYSTEM_ID(HdrPtr->ApidQ);            */
/*      MsgId = (MsgId | (SystemId << 16)); */

#endif
#endif

return MsgId;

}/* end CFE_SB_GetMsgId */


/******************************************************************************
**  Function:  CFE_SB_SetMsgId
**
**  Purpose:
**    Set the message Id of a message in CCSDS header format
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**    MsgId  - Message Id to be written
**
**
**  Return:
**    (none)
*/
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr,
                     CFE_SB_MsgId_t MsgId)
{

#ifndef MESSAGE_FORMAT_IS_CCSDS_VER_2  
    CCSDS_WR_SID(MsgPtr->Hdr, MsgId);
#else
  CCSDS_WR_VERS(MsgPtr->SpacePacket.Hdr, 1);

  /* Set the stream ID APID in the primary header. */
  CCSDS_WR_APID(MsgPtr->SpacePacket.Hdr, CFE_SB_RD_APID_FROM_MSGID(MsgId) );
  
  CCSDS_WR_TYPE(MsgPtr->SpacePacket.Hdr, CFE_SB_RD_TYPE_FROM_MSGID(MsgId) );
  
  
  CCSDS_CLR_SEC_APIDQ(MsgPtr->SpacePacket.ApidQ);
  
  CCSDS_WR_EDS_VER(MsgPtr->SpacePacket.ApidQ, 1);
  
  CCSDS_WR_ENDIAN(MsgPtr->SpacePacket.ApidQ, CFE_PLATFORM_ENDIAN);
  
  CCSDS_WR_PLAYBACK(MsgPtr->SpacePacket.ApidQ, false);
  
  CCSDS_WR_SUBSYSTEM_ID(MsgPtr->SpacePacket.ApidQ, CFE_SB_RD_SUBSYS_ID_FROM_MSGID(MsgId));
  
  CCSDS_WR_SYSTEM_ID(MsgPtr->SpacePacket.ApidQ, CFE_SPACECRAFT_ID);

#endif 
}/* end CFE_SB_SetMsgId */
