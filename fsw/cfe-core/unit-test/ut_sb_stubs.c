/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
** File: ut_sb_stubs.c
**
** Purpose:
** Unit test stubs for Software Bus routines
** 
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "utstubs.h"

/*
** Global variables
**
** NOTE: CFE_SB_Default_Qos is an oddball in that it is directly referenced by the code
** in other applications.  Therefore the UT stub has to instantiate this in order to get
** any dependent code to link.
*/
CFE_SB_Qos_t CFE_SB_Default_Qos;

/*
** Functions
*/
/*****************************************************************************/
/**
** \brief CFE_SB_EarlyInit stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_EarlyInit.  It always returns CFE_SUCCESS when called.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_EarlyInit(void)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_EarlyInit);

    return status;
}
    
/*****************************************************************************/
/**
** \brief CFE_SB_TaskMain stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_TaskMain.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_TaskMain(void)
{
    UT_DEFAULT_IMPL(CFE_SB_TaskMain);
}

/*****************************************************************************/
/**
** \brief CFE_SB_CreatePipe stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_CreatePipe.  The user can adjust the response by setting
**        the values in the SB_CreatePipeRtn structure prior to this function
**        being called.  If the value SB_CreatePipeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SB_CreatePipeRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_CreatePipe(CFE_SB_PipeId_t *PipeIdPtr, uint16 Depth,
                        const char *PipeName)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_CreatePipe);

    if (status >= 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_SB_CreatePipe), (uint8*)PipeIdPtr, sizeof(*PipeIdPtr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetCmdCode stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_GetCmdCode.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either the function code from command secondary header or
**        CFE_SUCCESS.
**
******************************************************************************/
uint16 CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr)
{
    int32          status;
    CFE_SB_CmdHdr_t *CmdHdrPtr;

    status = UT_DEFAULT_IMPL(CFE_SB_GetCmdCode);

    if (status == 0 && CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_TLM)
    {
        /* Cast the input pointer to a Cmd Msg pointer */
        CmdHdrPtr = (CFE_SB_CmdHdr_t *)MsgPtr;
        status = CCSDS_RD_FC(CmdHdrPtr->Sec);
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetMsgId stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_GetMsgId.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the entire stream ID from the primary header.
**
******************************************************************************/
CFE_SB_MsgId_t CFE_SB_GetMsgId(const CFE_SB_Msg_t *MsgPtr)
{
    CFE_SB_MsgId_t MsgId = 0;

    UT_DEFAULT_IMPL(CFE_SB_GetMsgId);

    if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_GetMsgId), &MsgId, sizeof(MsgId)) < sizeof(MsgId))
    {
#ifdef MESSAGE_FORMAT_IS_CCSDS

#ifndef MESSAGE_FORMAT_IS_CCSDS_VER_2  
        MsgId = CCSDS_RD_SID(MsgPtr->Hdr);
#else

        uint32            SubSystemId;

        MsgId = CCSDS_RD_APID(MsgPtr->Hdr); /* Primary header APID  */
     
        if ( CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_CMD)
              MsgId = MsgId | CFE_SB_CMD_MESSAGE_TYPE;  

        /* Add in the SubSystem ID as needed */
        SubSystemId = CCSDS_RD_SUBSYSTEM_ID(MsgPtr->SpacePacket.ApidQ);
        MsgId = (MsgId | (SubSystemId << 8));

/* Example code to add in the System ID as needed. */
/*   The default is to init this field to the Spacecraft ID but ignore for routing.   */
/*   To fully implement this field would require significant SB optimization to avoid */
/*   prohibitively large routing and index tables. */
/*      uint16            SystemId;                              */
/*      SystemId = CCSDS_RD_SYSTEM_ID(HdrPtr->ApidQ);            */
/*      MsgId = (MsgId | (SystemId << 16)) */
#endif
#endif

    }

return MsgId;
}

/*****************************************************************************/
/**
** \brief CFE_SB_InitMsg stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_InitMsg.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_InitMsg(void *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16 Length,
                    bool Clear)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_InitMsg);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_InitMsg), (uint8*)MsgPtr, Length) < Length)
        {
            CFE_SB_SetMsgId(MsgPtr, MsgId);
            CFE_SB_SetTotalMsgLength(MsgPtr, Length);
        }
    }
}

/*****************************************************************************/
/**
** \brief CFE_SB_RcvMsg stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_RcvMsg.  By default it will return the TIMEOUT error response,
**        unless the test setup sequence has indicated otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS on the first call, then -1 on the second.
**
******************************************************************************/
int32 CFE_SB_RcvMsg(CFE_SB_MsgPtr_t *BufPtr,
                    CFE_SB_PipeId_t PipeId,
                    int32 TimeOut)
{
    int32 status;
    static union
    {
        CFE_SB_Msg_t Msg;
        uint8 Ext[CFE_MISSION_SB_MAX_SB_MSG_SIZE];
    } Buffer;


    status = UT_DEFAULT_IMPL(CFE_SB_RcvMsg);

    if (status >= 0)
    {
        if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_RcvMsg), (uint8*)BufPtr, sizeof(*BufPtr)) < sizeof(*BufPtr))
        {
            memset(&Buffer, 0, sizeof(Buffer));
            *BufPtr = &Buffer.Msg;
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SendMsg stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SendMsg.  The user can adjust the response by setting
**        the values in the SBSendMsgRtn structure prior to this function
**        being called.  If the value SBSendMsgRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SBSendMsgRtn.value.
**        CFE_SUCCESS is returned otherwise.  Only EVS and TIME messages are
**        handled directly by this function; other messages are passed to the
**        unit test function, UT_ProcessSBMsg, for any further action.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
/* Only doing subset of total messages;
** NOTE: Currently does EVS, TIME
*/
int32 CFE_SB_SendMsg(CFE_SB_Msg_t *MsgPtr)
{
    int32            status = CFE_SUCCESS;

    /*
     * Create a context entry so a hook function
     * could do something useful with the message
     */
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_SendMsg), MsgPtr);

    status = UT_DEFAULT_IMPL(CFE_SB_SendMsg);

    if (status >= 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_SendMsg), MsgPtr->Byte, CCSDS_RD_LEN(MsgPtr->Hdr));
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SetCmdCode stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SetCmdCode.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either CFE_SB_WRONG_MSG_TYPE or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr, uint16 CmdCode)
{
    int32           status;
    CFE_SB_CmdHdr_t *CmdHdrPtr;

    status = UT_DEFAULT_IMPL(CFE_SB_SetCmdCode);

    if (status >= 0)
    {
        /* If msg type is telemetry, ignore the request */
        if (CCSDS_RD_TYPE(MsgPtr->Hdr) == CCSDS_TLM)
        {
            status = CFE_SB_WRONG_MSG_TYPE;
        }
        else
        {
            /* Cast the input pointer to a Cmd Msg pointer */
            CmdHdrPtr = (CFE_SB_CmdHdr_t *) MsgPtr;
            CCSDS_WR_FC(CmdHdrPtr->Sec,CmdCode);
        }
    }

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SetMsgId stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SetMsgId.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr, CFE_SB_MsgId_t MsgId)
{
    UT_DEFAULT_IMPL(CFE_SB_SetMsgId);
    UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_SetMsgId), (uint8*)&MsgId, sizeof(MsgId));
#ifndef MESSAGE_FORMAT_IS_CCSDS_VER_2  
    CCSDS_WR_SID(MsgPtr->Hdr, MsgId);
#else
  CCSDS_WR_VERS(MsgPtr->SpacePacket.Hdr, 1);

  /* Set the stream ID APID in the primary header. */
  CCSDS_WR_APID(MsgPtr->SpacePacket.Hdr, CFE_SB_RD_APID_FROM_MSGID(MsgId) );
  
  CCSDS_WR_TYPE(MsgPtr->SpacePacket.Hdr, CFE_SB_RD_TYPE_FROM_MSGID(MsgId) );
  
  
  CCSDS_CLR_SEC_APIDQ(MsgPtr->SpacePacket.ApidQ);
  
  CCSDS_WR_EDS_VER(MsgPtr->SpacePacket.ApidQ, 1);
  
  CCSDS_WR_ENDIAN(MsgPtr->SpacePacket.ApidQ, CFE_PLATFORM_ENDIAN);
  
  CCSDS_WR_PLAYBACK(MsgPtr->SpacePacket.ApidQ, false);
  
  CCSDS_WR_SUBSYSTEM_ID(MsgPtr->SpacePacket.ApidQ, CFE_SB_RD_SUBSYS_ID_FROM_MSGID(MsgId));
  
  CCSDS_WR_SYSTEM_ID(MsgPtr->SpacePacket.ApidQ, CFE_SPACECRAFT_ID);

#endif
}


/*****************************************************************************/
/**
** \brief CFE_SB_SetMsgTime stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SetMsgTime.  It always returns CFE_SUCCESS.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr, CFE_TIME_SysTime_t time)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_SetMsgTime);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SubscribeEx stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SubscribeEx.  The user can adjust the response by setting
**        the values in the SB_SubscribeExRtn structure prior to this function
**        being called.  If the value SB_SubscribeExRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value
**        SB_SubscribeExRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SubscribeEx(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId,
                         CFE_SB_Qos_t Quality, uint16 MsgLim)
{
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_SB_SubscribeEx), &MsgId);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_SubscribeEx), &PipeId);
    status = UT_DEFAULT_IMPL(CFE_SB_SubscribeEx);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_Subscribe stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_Subscribe.  The user can adjust the response by setting
**        the values in the SB_SubscribeRtn structure prior to this function
**        being called.  If the value SB_SubscribeRtn.count is greater than
**        zero then the counter is decremented; if it then equals zero the
**        return value is set to the user-defined value SB_SubscribeRtn.value.
**        CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_Subscribe(CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId)
{
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_SB_Subscribe), &MsgId);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_Subscribe), &PipeId);
    status = UT_DEFAULT_IMPL(CFE_SB_Subscribe);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_SubscribeLocal stub function
**
** \par Description
**        This function is used to mimic the response of the cFE SB function
**        CFE_SB_SubscribeLocal.  The user can adjust the response by setting
**        the values in the SB_SubscribeLocalRtn structure prior to this
**        function being called.  If the value SB_SubscribeLocalRtn.count is
**        greater than zero then the counter is decremented; if it then equals
**        zero the return value is set to the user-defined value
**        SB_SubscribeLocalRtn.value.  CFE_SUCCESS is returned otherwise.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns either a user-defined status flag or CFE_SUCCESS.
**
******************************************************************************/
int32 CFE_SB_SubscribeLocal(CFE_SB_MsgId_t MsgId,
                            CFE_SB_PipeId_t PipeId,
                            uint16 MsgLim)
{
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_SB_SubscribeLocal), &MsgId);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_SubscribeLocal), &PipeId);
    status = UT_DEFAULT_IMPL(CFE_SB_SubscribeLocal);

    return status;
}

/*****************************************************************************/
/**
** \brief CFE_SB_TimeStampMsg stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_TimeStampMsg.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr)
{
    UT_DEFAULT_IMPL(CFE_SB_TimeStampMsg);
}

/*****************************************************************************/
/**
** \brief CFE_SB_GetTotalMsgLength stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_GetTotalMsgLength.  It returns the user-defined value,
**        UT_SB_TotalMsgLen.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns a user-defined status value, UT_SB_TotalMsgLen.
**
******************************************************************************/
uint16 CFE_SB_GetTotalMsgLength(const CFE_SB_Msg_t *MsgPtr)
{
    int32 status;
    uint16 result;

    status = UT_DEFAULT_IMPL_RC(CFE_SB_GetTotalMsgLength,-1);

    if (status >= 0)
    {
        result = status;
    }
    else
    {
        result = CCSDS_RD_LEN(MsgPtr->Hdr);
    }
    return result;
}

/*****************************************************************************/
/**
** \brief CFE_SB_CleanUpApp stub function
**
** \par Description
**        This function is used as a placeholder for the cFE SB function
**        CFE_SB_CleanUpApp.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
int32 CFE_SB_CleanUpApp(uint32 AppId)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_CleanUpApp);

    return status;
}

/******************************************************************************
**  Function:  CFE_SB_MessageStringGet()
**
**  See function prototype for full description
**
*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringGet), DestStringPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringGet), SourceStringPtr);

    status = UT_DEFAULT_IMPL(CFE_SB_MessageStringGet);

    if (status == 0)
    {
        if (DestMaxSize == 0)
        {
            status = CFE_SB_BAD_ARGUMENT;
        }
        else
        {
            /*
             * Check if should use the default, which is if
             * the source string has zero length (first char is NUL).
             */
            if (DefaultString != NULL && (SourceMaxSize == 0 || *SourceStringPtr == 0))
            {
                SourceStringPtr = DefaultString;
                SourceMaxSize = DestMaxSize;
            }

            /* For the UT implementation, just call strncpy() */
            strncpy(DestStringPtr, SourceStringPtr, DestMaxSize - 1);
            DestStringPtr[DestMaxSize - 1] = 0;

            status = strlen(DestStringPtr);
        }
    }

    return status;
}


/******************************************************************************
**  Function:  CFE_SB_MessageStringSet()
**
**  See function prototype for full description
**
*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, uint32 DestMaxSize, uint32 SourceMaxSize)
{
    int32 status;

    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringGet), DestStringPtr);
    UT_Stub_RegisterContext(UT_KEY(CFE_SB_MessageStringGet), SourceStringPtr);

    status = UT_DEFAULT_IMPL(CFE_SB_MessageStringSet);

    if (status == 0)
    {
        if (DestMaxSize == 0)
        {
            status = CFE_SB_BAD_ARGUMENT;
        }
        else
        {
            /* For the UT implementation, just call strncpy() */
            strncpy(DestStringPtr, SourceStringPtr, DestMaxSize);
            if (DestStringPtr[DestMaxSize - 1] != 0)
            {
                status = DestMaxSize;
            }
            else
            {
                status = strlen(DestStringPtr);
            }
        }
    }

    return status;
}

UT_DEFAULT_STUB(CFE_SB_Unsubscribe, (CFE_SB_MsgId_t MsgId, CFE_SB_PipeId_t PipeId))

/******************************************************************************
**  Function:  CFE_SB_GetMsgTime()
**
**  Purpose:
**    Get the time field from a message.
**
**  Arguments:
**    MsgPtr - Pointer to a CFE_SB_Msg_t
**
**  Return:
**    Time field from message or
**    Time value of zero for msgs that do not have a Time field in header
*/
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_TIME_SysTime_t TimeFromMsg;

    UT_DEFAULT_IMPL(CFE_SB_GetMsgTime);

    if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_GetMsgTime), &TimeFromMsg, sizeof(CFE_TIME_SysTime_t)) != sizeof(CFE_TIME_SysTime_t))
    {
        TimeFromMsg.Seconds = 123;
        TimeFromMsg.Subseconds = 456;
    }

    return TimeFromMsg;

}/* end CFE_SB_GetMsgTime */

bool CFE_SB_ValidateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
    int32 status;

    status = UT_DEFAULT_IMPL(CFE_SB_ValidateChecksum);
    
    return (bool) status;
}

void *CFE_SB_GetUserData(CFE_SB_MsgPtr_t MsgPtr)
{
    uint8           *BytePtr;
    void            *Result;
    uint16          HdrSize;

    UT_DEFAULT_IMPL(CFE_SB_GetUserData);

    if (UT_Stub_CopyToLocal(UT_KEY(CFE_SB_GetUserData), &Result, sizeof(Result)) != sizeof(Result))
    {
        BytePtr = (uint8 *)MsgPtr;
        if (CCSDS_RD_TYPE(MsgPtr->Hdr) != CCSDS_TLM)
        {
            HdrSize = CFE_SB_CMD_HDR_SIZE;
        }
        else
        {
            HdrSize = CFE_SB_TLM_HDR_SIZE;
        }

        Result = (BytePtr + HdrSize);
    }

    return Result;
}

void CFE_SB_SetTotalMsgLength (CFE_SB_MsgPtr_t MsgPtr,uint16 TotalLength)
{
    UT_DEFAULT_IMPL(CFE_SB_SetTotalMsgLength);
    CCSDS_WR_LEN(MsgPtr->Hdr,TotalLength);
    UT_Stub_CopyFromLocal(UT_KEY(CFE_SB_SetTotalMsgLength), &TotalLength, sizeof(TotalLength));
}

