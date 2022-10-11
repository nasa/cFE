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
** File: cfe_sb_msg_id_util.c
** Purpose: message ID utility functions
*/

/*
** Include Files
*/
#include "cfe_sb_module_all.h"

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_SB_IsValidMsgId(CFE_SB_MsgId_t MsgId)
{
    return (!CFE_SB_MsgId_Equal(MsgId, CFE_SB_INVALID_MSG_ID) &&
            CFE_SB_MsgIdToValue(MsgId) <= CFE_PLATFORM_SB_HIGHEST_VALID_MSGID);
}
