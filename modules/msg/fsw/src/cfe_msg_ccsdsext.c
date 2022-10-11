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
 * Message CCSDS extended header implementations
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_msg_defaults.h"
#include "cfe_error.h"
#include "cfe_psp.h"

/* CCSDS Extended definitions */
#define CFE_MSG_EDSVER_SHIFT  11     /**< \brief CCSDS EDS version shift */
#define CFE_MSG_EDSVER_MASK   0xF800 /**< \brief CCSDS EDS version mask */
#define CFE_MSG_ENDIAN_MASK   0x0400 /**< \brief CCSDS endiam mask, little endian when set */
#define CFE_MSG_PLAYBACK_MASK 0x0200 /**< \brief CCSDS playback flag, playback when set */
#define CFE_MSG_SUBSYS_MASK   0x01FF /**< \brief CCSDS Subsystem mask */

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_MSG_SetDefaultCCSDSExt(CFE_MSG_Message_t *MsgPtr)
{
    CFE_MSG_SetEDSVersion(MsgPtr, (CFE_MSG_EDSVersion_t)CFE_PLATFORM_EDSVER);

#if (CFE_PLATFORM_ENDIAN == CCSDS_LITTLE_ENDIAN)
    CFE_MSG_SetEndian(MsgPtr, CFE_MSG_Endian_Little);
#else
    CFE_MSG_SetEndian(MsgPtr, CFE_MSG_Endian_Big);
#endif

    /* Default bits of the subsystem, for whatever isn't set by MsgId */
    CFE_MSG_SetSubsystem(MsgPtr, (CFE_MSG_Subsystem_t)CFE_PLATFORM_DEFAULT_SUBSYS);
    CFE_MSG_SetSystem(MsgPtr, (CFE_MSG_System_t)CFE_PSP_GetSpacecraftId());
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetEDSVersion(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t *Version)
{
    if (MsgPtr == NULL || Version == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    CFE_MSG_GetHeaderField(MsgPtr->CCSDS.Ext.Subsystem, Version, CFE_MSG_EDSVER_MASK);
    *Version >>= CFE_MSG_EDSVER_SHIFT;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetEDSVersion(CFE_MSG_Message_t *MsgPtr, CFE_MSG_EDSVersion_t Version)
{
    if (MsgPtr == NULL || (Version > (CFE_MSG_EDSVER_MASK >> CFE_MSG_EDSVER_SHIFT)))
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    CFE_MSG_SetHeaderField(MsgPtr->CCSDS.Ext.Subsystem, Version << CFE_MSG_EDSVER_SHIFT, CFE_MSG_EDSVER_MASK);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetEndian(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t *Endian)
{
    if (MsgPtr == NULL || Endian == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    if ((MsgPtr->CCSDS.Ext.Subsystem[0] & (CFE_MSG_ENDIAN_MASK >> 8)) != 0)
    {
        *Endian = CFE_MSG_Endian_Little;
    }
    else
    {
        *Endian = CFE_MSG_Endian_Big;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetEndian(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Endian_t Endian)
{
    CFE_Status_t status = CFE_SUCCESS;

    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    if (Endian == CFE_MSG_Endian_Little)
    {
        MsgPtr->CCSDS.Ext.Subsystem[0] |= CFE_MSG_ENDIAN_MASK >> 8;
    }
    else if (Endian == CFE_MSG_Endian_Big)
    {
        MsgPtr->CCSDS.Ext.Subsystem[0] &= ~(CFE_MSG_ENDIAN_MASK >> 8);
    }
    else
    {
        status = CFE_MSG_BAD_ARGUMENT;
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetPlaybackFlag(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t *PlayFlag)
{
    if (MsgPtr == NULL || PlayFlag == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    if ((MsgPtr->CCSDS.Ext.Subsystem[0] & (CFE_MSG_PLAYBACK_MASK >> 8)) != 0)
    {
        *PlayFlag = CFE_MSG_PlayFlag_Playback;
    }
    else
    {
        *PlayFlag = CFE_MSG_PlayFlag_Original;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetPlaybackFlag(CFE_MSG_Message_t *MsgPtr, CFE_MSG_PlaybackFlag_t PlayFlag)
{
    CFE_Status_t status = CFE_SUCCESS;

    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    if (PlayFlag == CFE_MSG_PlayFlag_Playback)
    {
        MsgPtr->CCSDS.Ext.Subsystem[0] |= CFE_MSG_PLAYBACK_MASK >> 8;
    }
    else if (PlayFlag == CFE_MSG_PlayFlag_Original)
    {
        MsgPtr->CCSDS.Ext.Subsystem[0] &= ~(CFE_MSG_PLAYBACK_MASK >> 8);
    }
    else
    {
        status = CFE_MSG_BAD_ARGUMENT;
    }

    return status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetSubsystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t *Subsystem)
{
    if (MsgPtr == NULL || Subsystem == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    CFE_MSG_GetHeaderField(MsgPtr->CCSDS.Ext.Subsystem, Subsystem, CFE_MSG_SUBSYS_MASK);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetSubsystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_Subsystem_t Subsystem)
{
    if (MsgPtr == NULL || ((Subsystem & ~CFE_MSG_SUBSYS_MASK) != 0))
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    CFE_MSG_SetHeaderField(MsgPtr->CCSDS.Ext.Subsystem, Subsystem, CFE_MSG_SUBSYS_MASK);

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetSystem(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t *System)
{
    if (MsgPtr == NULL || System == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    *System = (MsgPtr->CCSDS.Ext.SystemId[0] << 8) + MsgPtr->CCSDS.Ext.SystemId[1];

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetSystem(CFE_MSG_Message_t *MsgPtr, CFE_MSG_System_t System)
{
    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    MsgPtr->CCSDS.Ext.SystemId[0] = (System >> 8) & 0xFF;
    MsgPtr->CCSDS.Ext.SystemId[1] = System & 0xFF;

    return CFE_SUCCESS;
}
