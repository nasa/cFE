/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
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
 *  Checksum field access functions
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Computes checksum -
 * MsgPtr is Message pointer to checksum
 * Return Value is Calculated checksum
 *
 *-----------------------------------------------------------------*/
CFE_MSG_Checksum_t CFE_MSG_ComputeCheckSum(const CFE_MSG_Message_t *MsgPtr)
{
    CFE_MSG_Size_t     PktLen  = 0;
    const uint8 *      BytePtr = (const uint8 *)MsgPtr;
    CFE_MSG_Checksum_t chksum  = 0xFF;

    /* Message already checked, no error case reachable */
    CFE_MSG_GetSize(MsgPtr, &PktLen);

    while (PktLen--)
    {
        chksum ^= *(BytePtr++);
    }

    return chksum;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GenerateChecksum(CFE_MSG_Message_t *MsgPtr)
{
    CFE_Status_t             status;
    CFE_MSG_Type_t           type;
    bool                     hassechdr = false;
    CFE_MSG_CommandHeader_t *cmd       = (CFE_MSG_CommandHeader_t *)MsgPtr;

    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Cmd || !hassechdr)
    {
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    /* Zero checksum so new checksum will be correct */
    cmd->Sec.Checksum = 0;

    /* Compute using aligned MsgPtr and set, suppress false style warning */
    /* cppcheck-suppress redundantAssignment */
    cmd->Sec.Checksum = CFE_MSG_ComputeCheckSum(MsgPtr);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_ValidateChecksum(const CFE_MSG_Message_t *MsgPtr, bool *IsValid)
{
    CFE_Status_t   status;
    CFE_MSG_Type_t type;
    bool           hassechdr = false;

    if (MsgPtr == NULL || IsValid == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Cmd || !hassechdr)
    {
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    /* Compute, valid if == 0 */
    *IsValid = (CFE_MSG_ComputeCheckSum(MsgPtr) == 0);

    return CFE_SUCCESS;
}
