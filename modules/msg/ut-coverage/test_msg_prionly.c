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

/*
 * Extended header stubbed out, use when no extended header is defined
 */

/*
 * Includes
 */
#include "cfe_msg.h"
#include "test_msg_not.h"
#include "test_cfe_msg_ccsdsext.h"

unsigned int Test_MSG_Ext_NotZero(const CFE_MSG_Message_t *MsgPtr)
{
    return 0;
}

unsigned int Test_MSG_Ext_NotF(const CFE_MSG_Message_t *MsgPtr)
{
    return 0;
}

void Test_MSG_CCSDSExt(void) {}
