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

#include "cfe_sb_module_all.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_MsgHdrSize
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
size_t CFE_SB_MsgHdrSize(const CFE_MSG_Message_t *MsgPtr)
{
    size_t         size      = 0;
    bool           hassechdr = false;
    CFE_MSG_Type_t type      = CFE_MSG_Type_Invalid;

    if (MsgPtr == NULL)
    {
        return size;
    }

    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);
    CFE_MSG_GetType(MsgPtr, &type);

    /* if secondary hdr is not present... */
    /* Since all cFE messages must have a secondary hdr this check is not needed */
    if (!hassechdr)
    {
        size = sizeof(CCSDS_SpacePacket_t);
    }
    else if (type == CFE_MSG_Type_Cmd)
    {
        size = sizeof(CFE_MSG_CommandHeader_t);
    }
    else if (type == CFE_MSG_Type_Tlm)
    {
        size = sizeof(CFE_MSG_TelemetryHeader_t);
    }

    return size;
}

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_GetUserData
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void *CFE_SB_GetUserData(CFE_MSG_Message_t *MsgPtr)
{
    uint8 *BytePtr;
    size_t HdrSize;

    if (MsgPtr == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Failed invalid arguments\n", __func__);
        return 0;
    }

    BytePtr = (uint8 *)MsgPtr;
    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);

    return (BytePtr + HdrSize);
}

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_GetUserDataLength
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
size_t CFE_SB_GetUserDataLength(const CFE_MSG_Message_t *MsgPtr)
{
    CFE_MSG_Size_t TotalMsgSize = 0;
    size_t         HdrSize;

    if (MsgPtr == NULL)
    {
        return TotalMsgSize;
    }

    CFE_MSG_GetSize(MsgPtr, &TotalMsgSize);
    HdrSize = CFE_SB_MsgHdrSize(MsgPtr);

    return TotalMsgSize - HdrSize;
}

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_SetUserDataLength
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_SetUserDataLength(CFE_MSG_Message_t *MsgPtr, size_t DataLength)
{
    CFE_MSG_Size_t TotalMsgSize;
    size_t         HdrSize;

    if (MsgPtr == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Failed invalid arguments\n", __func__);
    }
    else
    {
        HdrSize      = CFE_SB_MsgHdrSize(MsgPtr);
        TotalMsgSize = HdrSize + DataLength;

        if (TotalMsgSize <= CFE_MISSION_SB_MAX_SB_MSG_SIZE)
        {
            CFE_MSG_SetSize(MsgPtr, TotalMsgSize);
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Failed TotalMsgSize too large\n", __func__);
        }
    }
}

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_TimeStampMsg
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_SB_TimeStampMsg(CFE_MSG_Message_t *MsgPtr)
{
    CFE_MSG_SetMsgTime(MsgPtr, CFE_TIME_GetTime());
}

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_MessageStringGet
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString,
                              size_t DestMaxSize, size_t SourceMaxSize)
{
    int32 Result;

    /*
     * Error in caller if DestMaxSize == 0.
     * Cannot terminate the string, since there is no place for the NUL
     * In this case, do nothing
     */
    if (DestMaxSize == 0 || DestStringPtr == NULL)
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
            SourceMaxSize   = DestMaxSize;
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

/*----------------------------------------------------------------
 *
 * Function: CFE_SB_MessageStringSet
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, size_t DestMaxSize,
                              size_t SourceMaxSize)
{
    int32 Result;

    if (SourceStringPtr == NULL || DestStringPtr == NULL)
    {
        Result = CFE_SB_BAD_ARGUMENT;
    }
    else
    {
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
    }

    return Result;
}
