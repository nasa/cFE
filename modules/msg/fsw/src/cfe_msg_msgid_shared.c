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
 *  Message id access functions, shared cFS implementation
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetTypeFromMsgId(CFE_SB_MsgId_t MsgId, CFE_MSG_Type_t *Type)
{
    CFE_MSG_Message_t msg;
    int32             Status;

    /* Memset to initialize avoids possible GCC bug 53119 */
    memset(&msg, 0, sizeof(msg));

    if (Type == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    Status = CFE_MSG_SetMsgId(&msg, MsgId);
    if (Status == CFE_SUCCESS)
    {
        Status = CFE_MSG_GetType(&msg, Type);
    }

    return Status;
}
