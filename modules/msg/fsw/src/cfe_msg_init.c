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
 * Message initialization
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_msg_defaults.h"
#include "string.h"

/******************************************************************************
 * Top level message initialization - See API and header file for details
 */
int32 CFE_MSG_Init(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId, CFE_MSG_Size_t Size)
{

    int32 status;

    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Clear and set defaults */
    memset(MsgPtr, 0, Size);
    CFE_MSG_InitDefaultHdr(MsgPtr);

    /* Set values input */
    status = CFE_MSG_SetMsgId(MsgPtr, MsgId);
    if (status == CFE_SUCCESS)
    {
        status = CFE_MSG_SetSize(MsgPtr, Size);
    }

    return status;
}
