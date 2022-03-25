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
 * Extended message header field not Zero or F
 */

/*
 * Includes
 */
#include "cfe_msg.h"
#include "test_msg_not.h"

unsigned int Test_MSG_Ext_NotZero(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int           bits = 0;
    CFE_MSG_EDSVersion_t   edsver;
    CFE_MSG_Endian_t       endian;
    CFE_MSG_PlaybackFlag_t playflag;
    CFE_MSG_Subsystem_t    subsystem;
    CFE_MSG_System_t       system;

    CFE_MSG_GetEDSVersion(MsgPtr, &edsver);
    if (edsver != 0)
        bits |= MSG_EDSVER_FLAG;
    CFE_MSG_GetEndian(MsgPtr, &endian);
    if (endian != CFE_MSG_Endian_Big)
        bits |= MSG_ENDIAN_FLAG;
    CFE_MSG_GetPlaybackFlag(MsgPtr, &playflag);
    if (playflag != CFE_MSG_PlayFlag_Original)
        bits |= MSG_PBACK_FLAG;
    CFE_MSG_GetSubsystem(MsgPtr, &subsystem);
    if (subsystem != 0)
        bits |= MSG_SUBSYS_FLAG;
    CFE_MSG_GetSystem(MsgPtr, &system);
    if (system != 0)
        bits |= MSG_SYSTEM_FLAG;

    return bits;
}

unsigned int Test_MSG_Ext_NotF(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int           bits = 0;
    CFE_MSG_EDSVersion_t   edsver;
    CFE_MSG_Endian_t       endian;
    CFE_MSG_PlaybackFlag_t playflag;
    CFE_MSG_Subsystem_t    subsystem;
    CFE_MSG_System_t       system;

    CFE_MSG_GetEDSVersion(MsgPtr, &edsver);
    if (edsver != 0x1F)
        bits |= MSG_EDSVER_FLAG;
    CFE_MSG_GetEndian(MsgPtr, &endian);
    if (endian != CFE_MSG_Endian_Little)
        bits |= MSG_ENDIAN_FLAG;
    CFE_MSG_GetPlaybackFlag(MsgPtr, &playflag);
    if (playflag != CFE_MSG_PlayFlag_Playback)
        bits |= MSG_PBACK_FLAG;
    CFE_MSG_GetSubsystem(MsgPtr, &subsystem);
    if (subsystem != 0x1FF)
        bits |= MSG_SUBSYS_FLAG;
    CFE_MSG_GetSystem(MsgPtr, &system);
    if (system != 0xFFFF)
        bits |= MSG_SYSTEM_FLAG;

    return bits;
}
