/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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

#include "cfe_config.h"
#include "edslib_datatypedb.h"
#include "edslib_intfdb.h"
#include "cfe_missionlib_runtime.h"
#include "cfe_missionlib_api.h"
#include "cfe_mission_eds_parameters.h"
#include "cfe_mission_eds_interface_parameters.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
size_t CFE_SB_MsgHdrSize(const CFE_MSG_Message_t *MsgPtr)
{
    union
    {
        CFE_SB_Listener_Component_t  Listener;
        CFE_SB_Publisher_Component_t Publisher;

    } FuncParams;
    CFE_MSG_Type_t                           MsgType;
    bool                                     HasSecHdr;
    EdsLib_Id_t                              CmdEdsId;
    uint16                                   TopicId;
    int32                                    EdsStatus;
    EdsLib_Id_t                              EdsId;
    EdsLib_DataTypeDB_DerivativeObjectInfo_t DerivObjInfo;
    CFE_SB_SoftwareBus_PubSub_Interface_t    PubSubParams;
    EdsLib_DataTypeDB_EntityInfo_t           PayloadInfo;
    CFE_MissionLib_TopicInfo_t               TopicInfo;
    CFE_MSG_Size_t                           MsgSize;

    const EdsLib_DatabaseObject_t *               EDS_DB    = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_EDS_DB);
    const CFE_MissionLib_SoftwareBus_Interface_t *SBINTF_DB = CFE_Config_GetObjPointer(CFE_CONFIGID_MISSION_SBINTF_DB);

    /* Value of 0 for IDs is always reserved/invalid */
    CmdEdsId = EDSLIB_ID_INVALID;
    TopicId  = 0;

    HasSecHdr = false;
    MsgType   = CFE_MSG_Type_Invalid;
    EdsStatus = CFE_MISSIONLIB_FAILURE;

    memset(&PayloadInfo, 0, sizeof(PayloadInfo));

    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &HasSecHdr);
    CFE_MSG_GetType(MsgPtr, &MsgType);
    CFE_MSG_GetSize(MsgPtr, &MsgSize);

    if (HasSecHdr)
    {
        CFE_MissionLib_Get_PubSub_Parameters(&PubSubParams, &MsgPtr->BaseMsg);

        if (MsgType == CFE_MSG_Type_Cmd)
        {
            CFE_MissionLib_UnmapListenerComponent(&FuncParams.Listener, &PubSubParams);

            CmdEdsId = EDSLIB_INTF_ID(EDS_INDEX(CFE_SB), EdsCommand_CFE_SB_Telecommand_indication_DECLARATION);
            TopicId  = FuncParams.Listener.Telecommand.TopicId;
        }
        else if (MsgType == CFE_MSG_Type_Tlm)
        {
            CFE_MissionLib_UnmapPublisherComponent(&FuncParams.Publisher, &PubSubParams);

            CmdEdsId = EDSLIB_INTF_ID(EDS_INDEX(CFE_SB), EdsCommand_CFE_SB_Telemetry_indication_DECLARATION);
            TopicId  = FuncParams.Publisher.Telemetry.TopicId;
        }
    }

    if (TopicId != 0)
    {
        EdsStatus = CFE_MissionLib_GetTopicInfo(SBINTF_DB, TopicId, &TopicInfo);
    }

    if (EdsStatus == CFE_MISSIONLIB_SUCCESS)
    {
        EdsStatus = EdsLib_IntfDB_FindAllArgumentTypes(EDS_DB, CmdEdsId, TopicInfo.ParentIntfId, &EdsId, 1);
    }

    /*
     * The above code yields an interface base type.  Need to potentially interpret
     * value constraints within the headers to determine final/real type.
     */
    if (EdsStatus == CFE_MISSIONLIB_SUCCESS)
    {
        EdsStatus = EdsLib_DataTypeDB_IdentifyBufferWithSize(EDS_DB, EdsId, MsgPtr, MsgSize, &DerivObjInfo);
        if (EdsStatus == EDSLIB_SUCCESS)
        {
            /* Use the derived type as the actual EdsId */
            EdsId = DerivObjInfo.EdsId;
        }
        else if (EdsStatus == EDSLIB_NO_MATCHING_VALUE)
        {
            /* This is OK if the struct is not derived or has no additional value constraints */
            EdsStatus = EDSLIB_SUCCESS;
        }
    }

    /*
     * Index 0 is always the header, and index 1 should always be the first element of real data
     */
    if (EdsStatus == EDSLIB_SUCCESS)
    {
        EdsStatus = EdsLib_DataTypeDB_GetMemberByIndex(EDS_DB, EdsId, 1, &PayloadInfo);
    }

    /*
     * Back up: Not all messages are necessarily defined in EDS.  If the above lookup didn't work,
     * then it means the content is not EDS-defined.  Revert to the traditional method of locating
     * the payload by simply adding the header size to the base pointer.
     */
    if (EdsStatus != EDSLIB_SUCCESS)
    {
        /*
         * This is basically identical to the non-EDS implementation of this function,
         * and this was lifted from cfe_sb_msg_id_util.c (not compiled in EDS builds)
         */
        if (!HasSecHdr)
        {
            PayloadInfo.Offset.Bytes = sizeof(CFE_MSG_Message_t);
        }
        else if (MsgType == CFE_MSG_Type_Cmd)
        {
            PayloadInfo.Offset.Bytes = sizeof(CFE_MSG_CommandHeader_t);
        }
        else if (MsgType == CFE_MSG_Type_Tlm)
        {
            PayloadInfo.Offset.Bytes = sizeof(CFE_MSG_TelemetryHeader_t);
        }
    }

    return PayloadInfo.Offset.Bytes;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_CmdTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum)
{
    const CFE_SB_Listener_Component_t     Params = {{.InstanceNumber = InstanceNum, .TopicId = TopicId}};
    CFE_SB_SoftwareBus_PubSub_Interface_t Output;

    CFE_MissionLib_MapListenerComponent(&Output, &Params);

    return CFE_SB_MsgIdToValue(Output.MsgId);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_TlmTopicIdToMsgId(uint16 TopicId, uint16 InstanceNum)
{
    const CFE_SB_Publisher_Component_t    Params = {{.InstanceNumber = InstanceNum, .TopicId = TopicId}};
    CFE_SB_SoftwareBus_PubSub_Interface_t Output;

    CFE_MissionLib_MapPublisherComponent(&Output, &Params);

    return CFE_SB_MsgIdToValue(Output.MsgId);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_GlobalCmdTopicIdToMsgId(uint16 TopicId)
{
    /* Instance Number 0 is reserved for globals */
    return CFE_SB_CmdTopicIdToMsgId(TopicId, 0);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_GlobalTlmTopicIdToMsgId(uint16 TopicId)
{
    /* Instance Number 0 is reserved for globals */
    return CFE_SB_TlmTopicIdToMsgId(TopicId, 0);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_LocalCmdTopicIdToMsgId(uint16 TopicId)
{
    /* PSP-reported Instance number is used for locals */
    return CFE_SB_CmdTopicIdToMsgId(TopicId, CFE_PSP_GetProcessorId());
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_SB_MsgId_Atom_t CFE_SB_LocalTlmTopicIdToMsgId(uint16 TopicId)
{
    /* PSP-reported Instance number is used for locals */
    return CFE_SB_TlmTopicIdToMsgId(TopicId, CFE_PSP_GetProcessorId());
}

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
