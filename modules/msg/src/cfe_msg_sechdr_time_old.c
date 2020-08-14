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
 *  Time field access functions
 */
#include "cfe_msg_api.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"
#include <string.h>

/******************************************************************************
 * Set message time - See API and header file for details
 */
int32 CFE_MSG_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t NewTime)
{

    uint32                     status;
    CFE_MSG_Type_t             type;
    bool                       hassechdr = false;
    CFE_MSG_TelemetryHeader_t *tlm       = (CFE_MSG_TelemetryHeader_t *)MsgPtr;

/* declare format specific vars */
#if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)
    uint16 LocalSubs16;
#elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_M_20)
    uint32 LocalSubs32;
#endif

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

    memcpy(&tlm->Sec.Time[0], &NewTime.Seconds, 4);

#if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)

    /* convert time from CFE_TIME_SysTime_t format to packet format */
    LocalSubs16 = (uint16)(NewTime.Subseconds >> 16);
    memcpy(&tlm->Sec.Time[4], &LocalSubs16, 2);

#elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_SUBS)

    /* no conversion necessary -- packet format = CFE_TIME_SysTime_t format */
    memcpy(&tlm->Sec.Time[4], &NewTime.Subseconds, 4);

#elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_M_20)

    /* convert time from CFE_TIME_SysTime_t format to packet format */
    LocalSubs32 = CFE_TIME_Sub2MicroSecs(NewTime.Subseconds) << 12;
    memcpy(&tlm->Sec.Time[4], &LocalSubs32, 4);

#endif

    return CFE_SUCCESS;
}

/******************************************************************************
 * Get message time -  See API and header file for details
 */
int32 CFE_MSG_GetMsgTime(const CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t *Time)
{

    uint32                     status;
    CFE_MSG_Type_t             type;
    bool                       hassechdr = false;
    CFE_MSG_TelemetryHeader_t *tlm       = (CFE_MSG_TelemetryHeader_t *)MsgPtr;

#if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)
    uint16 LocalSubs16;
#endif

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

    memcpy(&Time->Seconds, &tlm->Sec.Time[0], 4);

#if (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_16_SUBS)

    memcpy(&LocalSubs16, &tlm->Sec.Time[4], 2);
    /* convert packet data into CFE_TIME_SysTime_t format */
    Time->Subseconds = ((uint32)LocalSubs16) << 16;

#elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_SUBS)

    memcpy(&Time->Subseconds, &tlm->Sec.Time[4], 4);
    /* no conversion necessary -- packet format = CFE_TIME_SysTime_t format */

#elif (CFE_MISSION_SB_PACKET_TIME_FORMAT == CFE_MISSION_SB_TIME_32_32_M_20)

    memcpy(&LocalSubs32, &tlm->Sec.Time[4], 4);
    /* convert packet data into CFE_TIME_SysTime_t format */
    Time->Subseconds = CFE_TIME_Micro2SubSecs((LocalSubs32 >> 12));

#endif

    return CFE_SUCCESS;
}
