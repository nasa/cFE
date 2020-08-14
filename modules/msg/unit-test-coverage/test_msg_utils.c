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

/*
 * Message header test utilities
 */

/*
 * Includes
 */
#include "cfe_msg_api.h"
#include "test_msg_utils.h"
#include "utassert.h"

void Test_MSG_PrintMsg(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Size_t Size)
{

    int  i;
    char sbuf[121] = {0};
    char wbuf[6]   = {0};

    if (Size == 0)
    {
        CFE_MSG_GetSize(MsgPtr, &Size);
    }

    for (i = 0; i < Size / 2; i++)
    {
        sprintf(wbuf, "%02X%02X ", MsgPtr->Byte[i * 2], MsgPtr->Byte[(i * 2) + 1]);
        strcat(sbuf, wbuf);

        /* Make sure string buffer doesn't overflow */
        if (sizeof(sbuf) < (strlen(sbuf) + 6))
            break;
    }

    UtAssert_Message(UTASSERT_CASETYPE_INFO, __FILE__, __LINE__, "Message: %s", sbuf);
}

unsigned long long int Test_MSG_Sum(const CFE_MSG_Message_t *MsgPtr)
{

    CFE_MSG_Size_t         length;
    unsigned long long int sum = 0;
    int                    i;

    CFE_MSG_GetSize(MsgPtr, &length);

    for (i = 0; i < length; i++)
    {
        sum += MsgPtr->Byte[i];
    }

    return sum;
}
