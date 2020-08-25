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
 *  Message id access functions, shared cFS implementation
 */
#include "cfe_msg_api.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"

/******************************************************************************
 * Get type from message id - See API and header file for details
 * cFS default implementation
 */
int32 CFE_MSG_GetTypeFromMsgId(CFE_SB_MsgId_t MsgId, CFE_MSG_Type_t *Type)
{

    CFE_MSG_Message_t msg;

    if (Type == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    CFE_MSG_SetMsgId(&msg, MsgId);
    CFE_MSG_GetType(&msg, Type);

    return CFE_SUCCESS;
}
