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
** File: cfe_sb_util.c
**
** Purpose:
**      This file contains 'access' macros and functions for reading and
**      writing message header fields.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/


/*
** Include Files
*/

#include "cfe_sb.h"
#include "ccsds.h"
#include "osapi.h"
#include "cfe_error.h"
#include "cfe_msg_api.h"

#include <string.h>

#ifndef CFE_OMIT_DEPRECATED_6_8
/*
 * Function: CFE_SB_InitMsg - See API and header file for details
 */
void CFE_SB_InitMsg(void           *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    size_t         Length,
                    bool        Clear )
{

   CFE_MSG_Init((CFE_MSG_Message_t *)MsgPtr, MsgId, Length);

} /* end CFE_SB_InitMsg */
#endif

/******************************************************************************
**  Function:  CFE_SB_MsgHdrSize()
**
**  Purpose:
**    Get the size of a message header.
**
**  Arguments:
**    *MsgPtr - Pointer to a SB message 
**
**  Return:
**     Size of Message Header.
*/
size_t CFE_SB_MsgHdrSize(const CFE_MSG_Message_t *MsgPtr)
{
    size_t         size = 0;
    bool           hassechdr = false;
    CFE_MSG_Type_t type = CFE_MSG_Type_Invalid;

    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);
    CFE_MSG_GetType(MsgPtr, &type);

    /* if secondary hdr is not present... */
    /* Since all cFE messages must have a secondary hdr this check is not needed */
    if(!hassechdr)
    {
        size = sizeof(CCSDS_SpacePacket_t);
    }
    else if(type == CFE_MSG_Type_Cmd)
    {
        size = sizeof(CFE_MSG_CommandHeader_t);
    }
    else if(type == CFE_MSG_Type_Tlm)
    {
        size = sizeof(CFE_MSG_TelemetryHeader_t);
    }

    return size;

}/* end CFE_SB_MsgHdrSize */


/*
 * Function: CFE_SB_GetUserData - See API and header file for details
 */
void *CFE_SB_GetUserData(CFE_MSG_Message_t *MsgPtr)
{
    uint8           *BytePtr;
    size_t          HdrSize;

    BytePtr = (uint8 *)MsgPtr;
    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);

    return (BytePtr + HdrSize);
}/* end CFE_SB_GetUserData */


/*
 * Function: CFE_SB_GetUserDataLength - See API and header file for details
 */
size_t CFE_SB_GetUserDataLength(const CFE_MSG_Message_t *MsgPtr)
{
    CFE_MSG_Size_t TotalMsgSize;
    size_t HdrSize;

    CFE_MSG_GetSize(MsgPtr, &TotalMsgSize);
    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);

    return TotalMsgSize - HdrSize;
}/* end CFE_SB_GetUserDataLength */


/*
 * Function: CFE_SB_SetUserDataLength - See API and header file for details
 */
void CFE_SB_SetUserDataLength(CFE_MSG_Message_t *MsgPtr, size_t DataLength)
{
    CFE_MSG_Size_t TotalMsgSize;
    size_t HdrSize;

    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);
    TotalMsgSize = HdrSize + DataLength;
    
    CFE_MSG_SetSize(MsgPtr, TotalMsgSize);

}/* end CFE_SB_SetUserDataLength */

#ifndef CFE_OMIT_DEPRECATED_6_8
/*
 * Function: CFE_SB_GetTotalMsgLength - See API and header file for details
 */
size_t CFE_SB_GetTotalMsgLength(const CFE_MSG_Message_t *MsgPtr)
{

    CFE_MSG_Size_t size;

    CFE_MSG_GetSize(MsgPtr, &size);

    return size;

}/* end CFE_SB_GetTotalMsgLength */

/*
 * Function: CFE_SB_SetTotalMsgLength - See API and header file for details
 */
void CFE_SB_SetTotalMsgLength(CFE_MSG_Message_t *MsgPtr,size_t TotalLength)
{

    CFE_MSG_SetSize(MsgPtr, TotalLength);

}/* end CFE_SB_SetTotalMsgLength */

/*
 * Function: CFE_SB_GetMsgTime - See API and header file for details
 */
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_MSG_Message_t *MsgPtr)
{
    CFE_TIME_SysTime_t TimeFromMsg = {0};

    CFE_MSG_GetMsgTime(MsgPtr, &TimeFromMsg);

    return TimeFromMsg;

}/* end CFE_SB_GetMsgTime */

/*
 * Function: CFE_SB_SetMsgTime - See API and header file for details
 */
int32 CFE_SB_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t NewTime)
{

    return CFE_MSG_SetMsgTime(MsgPtr, NewTime);

}/* end CFE_SB_SetMsgTime */
#endif /* CFE_OMIT_DEPRECATED_6_8 */

/*
 * Function: CFE_SB_TimeStampMsg - See API and header file for details
 */
void CFE_SB_TimeStampMsg(CFE_MSG_Message_t *MsgPtr)
{
    CFE_MSG_SetMsgTime(MsgPtr,CFE_TIME_GetTime());

}/* end CFE_SB_TimeStampMsg */

#ifndef CFE_OMIT_DEPRECATED_6_8
/*
 * Function: CFE_SB_GetCmdCode - See API and header file for details
 */
uint16 CFE_SB_GetCmdCode(CFE_MSG_Message_t *MsgPtr)
{

    CFE_MSG_FcnCode_t fc;

    CFE_MSG_GetFcnCode(MsgPtr, &fc);

    return fc;

}/* end CFE_SB_GetCmdCode */


/*
 * Function: CFE_SB_SetCmdCode - See API and header file for details
 */
int32 CFE_SB_SetCmdCode(CFE_MSG_Message_t *MsgPtr,
                      uint16 CmdCode)
{

    return CFE_MSG_SetFcnCode(MsgPtr, CmdCode);

}/* end CFE_SB_SetCmdCode */

/*
 * Function: CFE_SB_GetChecksum - See API and header file for details
 */
uint16 CFE_SB_GetChecksum(CFE_MSG_Message_t *MsgPtr)
{

    CFE_MSG_Type_t type = CFE_MSG_Type_Invalid;
    bool           hassechdr = false;

    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);
    CFE_MSG_GetType(MsgPtr, &type);

    /* if msg type is telemetry or there is no secondary hdr... */
    if((type == CFE_MSG_Type_Tlm)||(!hassechdr))
    {
        return 0;
    }/* end if */

    /* Byte access for now to avoid error if secondary doesn't contain checksum */
    return MsgPtr->Byte[sizeof(CCSDS_SpacePacket_t) + 1];

}/* end CFE_SB_GetChecksum */

/*
 * Function: CFE_SB_GenerateChecksum - See API and header file for details
 */
void CFE_SB_GenerateChecksum(CFE_MSG_Message_t *MsgPtr)
{

    CFE_MSG_GenerateChecksum(MsgPtr);

}/* end CFE_SB_GenerateChecksum */


/*
 * Function: CFE_SB_ValidateChecksum - See API and header file for details
 */
bool CFE_SB_ValidateChecksum(CFE_MSG_Message_t *MsgPtr)
{
    bool isvalid = false;

    CFE_MSG_ValidateChecksum(MsgPtr, &isvalid);

    return isvalid;

}/* end CFE_SB_ValidateChecksum */
#endif /* CFE_OMIT_DEPRECATED_6_8 */

/*
 * Function: CFE_SB_MessageStringGet - See API and header file for details
 */
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString, size_t DestMaxSize, size_t SourceMaxSize)
{
    int32 Result;

    /*
     * Error in caller if DestMaxSize == 0.
     * Cannot terminate the string, since there is no place for the NUL
     * In this case, do nothing
     */
    if (DestMaxSize == 0)
    {
        Result = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
        Result = 0;

        /*
         * Check if should use the default, which is if
         * the source string has zero length (first char is NUL).
         */
        if (DefaultString != NULL && (SourceMaxSize == 0 || *SourceStringPtr == 0))
        {
            SourceStringPtr = DefaultString;
            SourceMaxSize = DestMaxSize;
        }

        /* Reserve 1 character for the required NUL */
        --DestMaxSize;

        while (SourceMaxSize > 0 && *SourceStringPtr != 0 && DestMaxSize > 0)
        {
            *DestStringPtr = *SourceStringPtr;
            ++DestStringPtr;
            ++SourceStringPtr;
            --SourceMaxSize;
            --DestMaxSize;

            ++Result;
        }

        /* Put the NUL in the last character */
        *DestStringPtr = 0;
    }

    return Result;
}


/*
 * Function: CFE_SB_MessageStringSet - See API and header file for details
 */
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, size_t DestMaxSize, size_t SourceMaxSize)
{
    int32 Result;

    Result = 0;

    while (SourceMaxSize > 0 && *SourceStringPtr != 0 && DestMaxSize > 0)
    {
        *DestStringPtr = *SourceStringPtr;
        ++DestStringPtr;
        ++SourceStringPtr;
        ++Result;
        --DestMaxSize;
        --SourceMaxSize;
    }

    /*
     * Pad the remaining space with NUL chars,
     * but this should NOT be included in the final size
     */
    while (DestMaxSize > 0)
    {
        /* Put the NUL in the last character */
        *DestStringPtr = 0;
        ++DestStringPtr;
        --DestMaxSize;
    }

    return Result;
}
