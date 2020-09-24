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
** File: cfe_sb_msg_id_util.c
** Purpose: message ID utility functions
*/

/*
** Include Files
*/
#include "cfe_mission_cfg.h"
#include "ccsds.h"
#include "cfe_sb.h"
#include "osapi.h"
#include "cfe_error.h"
#include "cfe_sb_priv.h"
#include "cfe_msg_api.h"


/******************************************************************************
**  Function:  CFE_SB_ConvertMsgIdtoMsgKey
**
**  Purpose:
**    Convert the full message Id to the mission defined MsgKey format
**    No conversion is needed for the default implementation as it
**    is limited to less than 16 bits by CFE_PLATFORM_SB_HIGHEST_VALID_MSGID
**     If CFE_PLATFORM_SB_HIGHEST_VALID_MSGID is greater than 16 bits this function
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
    return CFE_SB_ValueToMsgKey(CFE_SB_MsgIdToValue(MsgId));
}/* CFE_SB_ConvertMsgIdtoMsgKey */

/*
 * Function: CFE_SB_GetMsgId - See API and header file for details
 */
CFE_SB_MsgId_t CFE_SB_GetMsgId(const CFE_SB_Msg_t *MsgPtr)
{
   CFE_SB_MsgId_t MsgId;

   /* Ignore return since no alternative action */
   CFE_MSG_GetMsgId(MsgPtr, &MsgId);

   return MsgId;

}/* end CFE_SB_GetMsgId */


/*
 * Function: CFE_SB_SetMsgId - See API and header file for details
 */
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr,
                     CFE_SB_MsgId_t MsgId)
{

  /* Ignore return, no alternate action */
  CFE_MSG_SetMsgId(MsgPtr, MsgId);

}/* end CFE_SB_SetMsgId */

/*
 * Function: CFE_SB_GetPktType - See API and header file for details
 */
uint32 CFE_SB_GetPktType(CFE_SB_MsgId_t MsgId)
{

  CFE_MSG_Type_t type;

  /* Ignores return, no alternate action */
  CFE_MSG_GetTypeFromMsgId(MsgId, &type);

  return type;

}/* end CFE_SB_GetPktType */

/*
 * Function: CFE_SB_IsValidMsgId - See API and header file for details
 */
bool CFE_SB_IsValidMsgId(CFE_SB_MsgId_t MsgId)
{
    return (!CFE_SB_MsgId_Equal(MsgId, CFE_SB_INVALID_MSG_ID) &&
            CFE_SB_MsgIdToValue(MsgId) <= CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
} /* end CFE_SB_IsValidMsgId */



