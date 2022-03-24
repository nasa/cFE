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
 * Message header field not Zero or F
 */

/*
 * Includes
 */
#include "cfe_msg.h"
#include "test_msg_not.h"

unsigned int Test_MSG_NotZero(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int bits = 0;

    /* Primary */
    bits |= Test_MSG_Pri_NotZero(MsgPtr);

    /* Extended */
    bits |= Test_MSG_Ext_NotZero(MsgPtr);

    return bits;
}

unsigned int Test_MSG_NotF(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int bits = 0;

    /* Primary */
    bits |= Test_MSG_Pri_NotF(MsgPtr);

    /* Extended */
    bits |= Test_MSG_Ext_NotF(MsgPtr);

    return bits;
}
