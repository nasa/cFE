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
#include "cfe_es.h"
#include "osapi.h"
#include "cfe_error.h"
#include "cfe_sb_msgformat_accessors.h"

#include <string.h>

/*
 * Macros to simplify usage of getter/setter functions.
 * The various header fields are accessed only through getter/setter functions in an external library.
 * This allows the CFE SB code to remain abstract and not assume any particular header format.
 *
 * These macros will resolve to the correct name of the accessor function for the
 * given header field name.
 */
#define CFE_SB_MSGFORMAT_CALL(f,...)            CFE_SB_MSGFORMAT_FUNCNAME(f)(__VA_ARGS__)
#define CFE_SB_MSGFORMAT_QUERY(f,m)             CFE_SB_MSGFORMAT_FUNCNAME(Get ## f)(&(m)->Base)
#define CFE_SB_MSGFORMAT_GET_ACCESSOR(f,m,...)  CFE_SB_MSGFORMAT_FUNCNAME(Get ## f)(&(m)->Base, __VA_ARGS__)
#define CFE_SB_MSGFORMAT_SET_ACCESSOR(f,m,...)  CFE_SB_MSGFORMAT_FUNCNAME(Set ## f)(&(m)->Base, __VA_ARGS__)


/*
 * Function: CFE_SB_GetMsgId - See API and header file for details
 */
CFE_SB_MsgId_t CFE_SB_GetMsgId(const CFE_SB_Msg_t *MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(MsgAddress) LocalMsgAddress;
    CFE_SB_MsgId_t LocalMsgId;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(MsgAddress, MsgPtr, &LocalMsgAddress);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        LocalMsgId = CFE_SB_INVALID_MSG_ID;
    }
    else
    {
        CFE_SB_MSGFORMAT_CALL(AddressToMsgId, &LocalMsgAddress, &LocalMsgId);
    }


    return LocalMsgId;

}/* end CFE_SB_GetMsgId */


/*
 * Function: CFE_SB_SetMsgId - See API and header file for details
 */
void CFE_SB_SetMsgId(CFE_SB_MsgPtr_t MsgPtr,
                     CFE_SB_MsgId_t MsgId)
{
    CFE_SB_MSGFORMAT_TYPE(MsgAddress) LocalMsgAddress;
    int32 status;

    CFE_SB_MSGFORMAT_CALL(MsgIdToAddress, &MsgId, &LocalMsgAddress);
    status = CFE_SB_MSGFORMAT_SET_ACCESSOR(MsgAddress, MsgPtr, &LocalMsgAddress);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
    }

}/* end CFE_SB_SetMsgId */


/*
 * Function: CFE_SB_InitMsg - See API and header file for details
 */
void CFE_SB_InitMsg(void          *MsgPtr,
                    CFE_SB_MsgId_t MsgId,
                    uint16         Length,
                    bool        Clear )
{
    CFE_SB_Msg_t   *MsgPtrReal = MsgPtr;
    int32 status;

    status = CFE_SB_MSGFORMAT_SET_ACCESSOR(InitialFields, MsgPtrReal, Length, Clear);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
    }
    else
    {
        CFE_SB_SetMsgId(MsgPtrReal, MsgId);
    }

} /* end CFE_SB_InitMsg */


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
uint16 CFE_SB_MsgHdrSize(const CFE_SB_Msg_t *MsgPtr)
{
    uint32 HeaderLength;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(HeaderLength, MsgPtr, &HeaderLength);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        HeaderLength = 0;
    }

    return HeaderLength;

}/* end CFE_SB_MsgHdrSize */


uint32 CFE_SB_GetPayloadOffset(const CFE_SB_Msg_t *MsgPtr)
{
    uint32 PayloadOffset;

    if (CFE_SB_MSGFORMAT_QUERY (IsTelemetry, MsgPtr))
    {
        PayloadOffset = sizeof(CFE_SB_TlmHdr_t);
    }
    else if (CFE_SB_MSGFORMAT_QUERY (IsCommand, MsgPtr))
    {
        PayloadOffset = sizeof(CFE_SB_CmdHdr_t);
    }
    else
    {
        /* fallback - should not be possible in CFE */
        PayloadOffset = 0;
    }

    return PayloadOffset;
}


/*
 * Function: CFE_SB_GetUserData - See API and header file for details
 */
void *CFE_SB_GetUserData(CFE_SB_MsgPtr_t MsgPtr)
{
    uint32 PayloadOffset;
    void *PayloadPtr;

    PayloadOffset = CFE_SB_GetPayloadOffset(MsgPtr);

    if (PayloadOffset == 0)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- invalid/unknown packet\n", __func__);
        PayloadPtr = NULL;
    }
    else
    {
        PayloadPtr = &MsgPtr->Byte[PayloadOffset];
    }

    return PayloadPtr;
}/* end CFE_SB_GetUserData */


/*
 * Function: CFE_SB_GetUserDataLength - See API and header file for details
 */
uint32 CFE_SB_GetUserDataLength(const CFE_SB_Msg_t *MsgPtr)
{
    uint32 PayloadOffset;
    uint32 TotalLength;

    PayloadOffset = CFE_SB_GetPayloadOffset(MsgPtr);
    TotalLength = CFE_SB_GetTotalMsgLength(MsgPtr);

    return TotalLength - PayloadOffset;
}/* end CFE_SB_GetUserDataLength */


/*
 * Function: CFE_SB_SetUserDataLength - See API and header file for details
 */
void CFE_SB_SetUserDataLength(CFE_SB_MsgPtr_t MsgPtr, uint32 DataLength)
{
    uint32 PayloadOffset;

    PayloadOffset = CFE_SB_GetPayloadOffset(MsgPtr);
    CFE_SB_SetTotalMsgLength(MsgPtr, PayloadOffset + DataLength);

}/* end CFE_SB_SetUserDataLength */


/*
 * Function: CFE_SB_GetTotalMsgLength - See API and header file for details
 */
uint32 CFE_SB_GetTotalMsgLength(const CFE_SB_Msg_t *MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(Length) LocalLength;
    uint32 TotalLength;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(TotalSize, MsgPtr, &LocalLength);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        TotalLength = 0;
    }
    else
    {
        CFE_SB_MSGFORMAT_CALL(MsgLengthToSize, &LocalLength, &TotalLength);
    }

    return TotalLength;

}/* end CFE_SB_GetTotalMsgLength */


/*
 * Function: CFE_SB_SetTotalMsgLength - See API and header file for details
 */
void CFE_SB_SetTotalMsgLength(CFE_SB_MsgPtr_t MsgPtr, uint32 TotalLength)
{
    CFE_SB_MSGFORMAT_TYPE(Length) LocalLength;
    int32 status;

    CFE_SB_MSGFORMAT_CALL(SizeToMsgLength,  &TotalLength, &LocalLength);

    status = CFE_SB_MSGFORMAT_SET_ACCESSOR(TotalSize, MsgPtr, &LocalLength);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
    }

}/* end CFE_SB_SetTotalMsgLength */


/*
 * Function: CFE_SB_GetMsgTime - See API and header file for details
 */
CFE_TIME_SysTime_t CFE_SB_GetMsgTime(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(Time) LocalMsgTime;
    CFE_TIME_SysTime_t TimeBuffer;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(Timestamp, MsgPtr, &LocalMsgTime);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        memset(&TimeBuffer, 0, sizeof(TimeBuffer));
    }
    else
    {
        CFE_SB_MSGFORMAT_CALL(MsgTimeToSysTime, &LocalMsgTime, &TimeBuffer);
    }

    return TimeBuffer;

}/* end CFE_SB_GetMsgTime */


/*
 * Function: CFE_SB_SetMsgTime - See API and header file for details
 */
int32 CFE_SB_SetMsgTime(CFE_SB_MsgPtr_t MsgPtr, CFE_TIME_SysTime_t NewTime)
{
    CFE_SB_MSGFORMAT_TYPE(Time) LocalMsgTime;
    int32 status;

    CFE_SB_MSGFORMAT_CALL(SysTimeToMsgTime, &NewTime, &LocalMsgTime);

    status = CFE_SB_MSGFORMAT_SET_ACCESSOR(Timestamp, MsgPtr, &LocalMsgTime);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
    }

    return status;

}/* end CFE_SB_SetMsgTime */


/*
 * Function: CFE_SB_TimeStampMsg - See API and header file for details
 */
void CFE_SB_TimeStampMsg(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_SetMsgTime(MsgPtr, CFE_TIME_GetTime());
}/* end CFE_SB_TimeStampMsg */


/*
 * Function: CFE_SB_GetCmdCode - See API and header file for details
 */
uint16 CFE_SB_GetCmdCode(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(FunctionCode) LocalFunctionCode;
    uint16 CmdCode;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(FunctionCode, MsgPtr, &LocalFunctionCode);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        CmdCode = 0;   /* may alias a valid command, but 0 should be reserved as no-op */
    }
    else
    {
        CFE_SB_MSGFORMAT_CALL(MsgFunctionCodeToValue, &LocalFunctionCode, &CmdCode);
    }

    return CmdCode;

}/* end CFE_SB_GetCmdCode */


/*
 * Function: CFE_SB_SetCmdCode - See API and header file for details
 */
int32 CFE_SB_SetCmdCode(CFE_SB_MsgPtr_t MsgPtr,
                      uint16 CmdCode)
{
    CFE_SB_MSGFORMAT_TYPE(FunctionCode) LocalFunctionCode;
    int32 status;

    CFE_SB_MSGFORMAT_CALL(ValueToMsgFunctionCode, &CmdCode, &LocalFunctionCode);
    status = CFE_SB_MSGFORMAT_SET_ACCESSOR(FunctionCode, MsgPtr, &LocalFunctionCode);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        CmdCode = 0;
    }
    else
    {
        CFE_SB_MSGFORMAT_CALL(MsgFunctionCodeToValue, &LocalFunctionCode, &CmdCode);
    }


    return status;

}/* end CFE_SB_SetCmdCode */


/*
 * Function: CFE_SB_GetChecksum - See API and header file for details
 */
uint16 CFE_SB_GetChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(Checksum) LocalChecksum;
    uint16 result;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(Checksum, MsgPtr, &LocalChecksum);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        result = 0xFFFF;
    }
    else
    {
        result = CFE_SB_MSGFORMAT_CALL(MsgChecksumToValue, &LocalChecksum);
    }

    return result;

}/* end CFE_SB_GetChecksum */


/*
 * Function: CFE_SB_GenerateChecksum - See API and header file for details
 */
void CFE_SB_GenerateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(Checksum) ComputedChecksum;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(ComputedChecksum, MsgPtr, &ComputedChecksum);

    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_MSGFORMAT_SET_ACCESSOR(Checksum, MsgPtr, &ComputedChecksum);
    }

    if (status != CFE_SB_WRONG_MSG_TYPE)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
    }


}/* end CFE_SB_GenerateChecksum */


/*
 * Function: CFE_SB_ValidateChecksum - See API and header file for details
 */
bool CFE_SB_ValidateChecksum(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(Checksum) LocalChecksum;
    CFE_SB_MSGFORMAT_TYPE(Checksum) ComputedChecksum;
    int32 status;
    bool result;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(Checksum, MsgPtr, &LocalChecksum);

    if (status == CFE_SUCCESS)
    {
        status = CFE_SB_MSGFORMAT_GET_ACCESSOR(ComputedChecksum, MsgPtr, &ComputedChecksum);
    }


    if (status == CFE_SUCCESS)
    {
        result = CFE_SB_MSGFORMAT_CALL(ChecksumCompare, &LocalChecksum, &ComputedChecksum);
    }
    else
    {
        result = false;
    }

    return result;

}/* end CFE_SB_ValidateChecksum */

/*
 * Function: CFE_SB_SetMsgSeqCnt - See API and header file for details
 */
uint32 CFE_SB_GetMsgSeqCnt(CFE_SB_MsgPtr_t MsgPtr)
{
    CFE_SB_MSGFORMAT_TYPE(Sequence) LocalSequence;
    uint32 SeqValue;
    int32 status;

    status = CFE_SB_MSGFORMAT_GET_ACCESSOR(Sequence, MsgPtr, &LocalSequence);
    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
        SeqValue = 0;
    }
    else
    {
        CFE_SB_MSGFORMAT_CALL(MsgSequenceToValue, &LocalSequence, &SeqValue);
    }


    return SeqValue;

}/* end CFE_SB_SetMsgSeqCnt */


/*
 * Function: CFE_SB_SetMsgSeqCnt - See API and header file for details
 */
void CFE_SB_SetMsgSeqCnt(CFE_SB_MsgPtr_t MsgPtr,uint32 Count)
{
    CFE_SB_MSGFORMAT_TYPE(Sequence) LocalSequence;
    int32 status;

    CFE_SB_MSGFORMAT_CALL(ValueToMsgSequence, &Count, &LocalSequence);

    status = CFE_SB_MSGFORMAT_SET_ACCESSOR(Sequence, MsgPtr, &LocalSequence);

    if (status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s(): ERROR -- code=%lx\n", __func__, (unsigned long)status);
    }

}/* end CFE_SB_SetMsgSeqCnt */




/*
 * Function: CFE_SB_MessageStringGet - See API and header file for details
 */
int32 CFE_SB_MessageStringGet(char *DestStringPtr, const char *SourceStringPtr, const char *DefaultString, uint32 DestMaxSize, uint32 SourceMaxSize)
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
int32 CFE_SB_MessageStringSet(char *DestStringPtr, const char *SourceStringPtr, uint32 DestMaxSize, uint32 SourceMaxSize)
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
