/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/******************************************************************************
 *  Function code field access functions
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_msg_defaults.h"

#define CFE_MSG_FC_MASK 0x7F /**< \brief Function code mask */

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetFcnCode(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t *FcnCode)
{
    CFE_MSG_HeaderVersion_t  version;
    CFE_MSG_Type_t           type;
    bool                     hassechdr = false;
    CFE_MSG_CommandHeader_t *cmd       = (CFE_MSG_CommandHeader_t *)MsgPtr;

    if (MsgPtr == NULL || FcnCode == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);
    CFE_MSG_GetHeaderVersion(MsgPtr, &version);
    CFE_MSG_GetType(MsgPtr, &type);
    /* According to CCSDS standards, must use Version 1 CCSDS header which
     * is represented by bit pattern '000' so Version should be just 0
     * see https://public.ccsds.org/Pubs/133x0b2e2.pdf section 4.1.3.2*/
    if (version != CFE_MISSION_CCSDSVER || type != CFE_MSG_Type_Cmd || !hassechdr)
    {
        *FcnCode = 0;
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    *FcnCode = cmd->Sec.FunctionCode & CFE_MSG_FC_MASK;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetFcnCode(CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t FcnCode)
{
    CFE_MSG_HeaderVersion_t  version;
    CFE_MSG_Type_t           type;
    bool                     hassechdr = false;
    CFE_MSG_CommandHeader_t *cmd       = (CFE_MSG_CommandHeader_t *)MsgPtr;

    if (MsgPtr == NULL || (FcnCode > CFE_MSG_FC_MASK))
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);
    CFE_MSG_GetHeaderVersion(MsgPtr, &version);
    CFE_MSG_GetType(MsgPtr, &type);
    /* According to CCSDS standards, must use Version 1 CCSDS header which
     * is represented by bit pattern '000' so Version should be just 0
     * see https://public.ccsds.org/Pubs/133x0b2e2.pdf section 4.1.3.2*/
    if (version != CFE_MISSION_CCSDSVER || type != CFE_MSG_Type_Cmd || !hassechdr)
    {
        return CFE_MSG_WRONG_MSG_TYPE;
    }
    

    cmd->Sec.FunctionCode = FcnCode;

    return CFE_SUCCESS;
}
