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
 *  Time field access functions - cFS default 32 bit seconds, 16 bit subseconds
 *  in big endian format
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"
#include <string.h>

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t NewTime)
{
    CFE_Status_t               status;
    CFE_MSG_Type_t             type;
    bool                       hassechdr = false;
    CFE_MSG_TelemetryHeader_t *tlm       = (CFE_MSG_TelemetryHeader_t *)MsgPtr;

    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Tlm || !hassechdr)
    {
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    /* Set big endian time field with default 32/16 layout */
    tlm->Sec.Time[0] = (NewTime.Seconds >> 24) & 0xFF;
    tlm->Sec.Time[1] = (NewTime.Seconds >> 16) & 0xFF;
    tlm->Sec.Time[2] = (NewTime.Seconds >> 8) & 0xFF;
    tlm->Sec.Time[3] = NewTime.Seconds & 0xFF;
    tlm->Sec.Time[4] = (NewTime.Subseconds >> 24) & 0xFF;
    tlm->Sec.Time[5] = (NewTime.Subseconds >> 16) & 0xFF;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetMsgTime(const CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t *Time)
{
    CFE_Status_t               status;
    CFE_MSG_Type_t             type;
    bool                       hassechdr = false;
    CFE_MSG_TelemetryHeader_t *tlm       = (CFE_MSG_TelemetryHeader_t *)MsgPtr;

    if (MsgPtr == NULL || Time == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Tlm || !hassechdr)
    {
        memset(Time, 0, sizeof(*Time));
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    /* Get big endian time fields with default 32/16 layout */
    Time->Subseconds = ((uint32)tlm->Sec.Time[4] << 24) + ((uint32)tlm->Sec.Time[5] << 16);
    Time->Seconds    = ((uint32)tlm->Sec.Time[0] << 24) + ((uint32)tlm->Sec.Time[1] << 16) + ((uint32)tlm->Sec.Time[2] << 8) + (uint32)tlm->Sec.Time[3];

    return CFE_SUCCESS;
}
