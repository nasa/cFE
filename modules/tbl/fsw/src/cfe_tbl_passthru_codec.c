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

/**
 * \file
 *  This file contains the source code for the TO lab application
 */

#include "cfe_tbl_module_all.h"
#include "cfe_tbl_codec.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ByteSwapTblHeader(CFE_TBL_File_Hdr_t *HdrPtr)
{
    CFE_TBL_ByteSwapUint32(&HdrPtr->Reserved);
    CFE_TBL_ByteSwapUint32(&HdrPtr->Offset);
    CFE_TBL_ByteSwapUint32(&HdrPtr->NumBytes);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ByteSwapUint32(uint32 *Uint32ToSwapPtr)
{
    int32 Temp   = *Uint32ToSwapPtr;
    char *InPtr  = (char *)&Temp;
    char *OutPtr = (char *)Uint32ToSwapPtr;

    /* SAD: Safe access to InPtr[0-3] and OutPtr[0-3] as both manipulate bytes within 4-byte integers. */
    OutPtr[0] = InPtr[3];
    OutPtr[1] = InPtr[2];
    OutPtr[2] = InPtr[1];
    OutPtr[3] = InPtr[0];
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DecodeHeadersFromFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                           CFE_TBL_File_Hdr_t *HeaderPtr)
{
    const uint32       EndianCheck = 0x01020304;
    CFE_Status_t       ReturnCode;
    int32              OsStatus;
    CFE_TBL_File_Hdr_t Buffer;

    /* In this mode file data is read directly to the struct */
    OsStatus = OS_read(FileDescriptor, &Buffer, sizeof(Buffer));

    /* Verify successful read of cFE Table File Header */
    if (OsStatus != sizeof(Buffer))
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_TBL_HDR_ERR_EID, OsStatus, sizeof(Buffer));
        ReturnCode = CFE_TBL_ERR_NO_TBL_HEADER;
    }
    else
    {
        /* cppcheck-suppress knownConditionTrueFalse */
        if ((*(const char *)&EndianCheck) == 0x04)
        {
            /* If this is a little endian processor, then the standard cFE Table Header,   */
            /* which is in big endian format, must be swapped so that the data is readable */
            CFE_TBL_ByteSwapTblHeader(&Buffer);
        }

        /*
         * Ensure termination of all local strings. These were read from a file, so they
         * must be treated with appropriate care.  This could happen in case the file got
         * damaged in transit or simply was not written properly to begin with.
         */
        Buffer.TableName[sizeof(Buffer.TableName) - 1] = '\0';

        /* Export the data to the user buffer */
        memcpy(HeaderPtr, &Buffer, sizeof(*HeaderPtr));

        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_EncodeHeadersToFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                         const CFE_TBL_File_Hdr_t *HeaderPtr)
{
    const uint32       EndianCheck = 0x01020304;
    CFE_Status_t       ReturnCode;
    int32              OsStatus;
    CFE_TBL_File_Hdr_t Buffer;

    memcpy(&Buffer, HeaderPtr, sizeof(Buffer));

    /* cppcheck-suppress knownConditionTrueFalse */
    if ((*(const char *)&EndianCheck) == 0x04)
    {
        /* If this is a little endian processor, then byte swap the header to a big endian format */
        /* to maintain the cFE Header standards */
        CFE_TBL_ByteSwapTblHeader(&Buffer);
    }

    /* Output the Table Image Header to the Dump File */
    OsStatus = OS_write(FileDescriptor, &Buffer, sizeof(Buffer));

    /* Make sure the header was output completely */
    if (OsStatus != sizeof(Buffer))
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_WRITE_TBL_HDR_ERR_EID, OsStatus, sizeof(Buffer));
        ReturnCode = CFE_TBL_ERR_ACCESS;
    }
    else
    {
        ReturnCode = CFE_SUCCESS;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateCodecLoadSize(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr)
{
    CFE_Status_t Status;
    uint32       ActualSize;
    uint32       ProjectedSize;

    /* Because the file sizes and in-memory size is the same, this check is simple */
    ActualSize    = CFE_TBL_RegRecGetSize(CFE_TBL_TxnRegRec(Txn));
    ProjectedSize = HeaderPtr->Offset + HeaderPtr->NumBytes;
    if (ProjectedSize > ActualSize)
    {
        Status = CFE_TBL_ERR_FILE_TOO_LARGE;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, ProjectedSize, ActualSize);
    }
    else
    {
        Status = CFE_SUCCESS;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_CodecGetFinalStatus(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr)
{
    CFE_Status_t Status;

    /* Any Table load that starts beyond the first byte is a "partial load" */
    /* But a file that starts with the first byte and ends before filling   */
    /* the whole table is just considered "short".                          */
    if (HeaderPtr->Offset > 0)
    {
        Status = CFE_TBL_WARN_PARTIAL_LOAD;
    }
    else if (HeaderPtr->NumBytes < CFE_TBL_RegRecGetSize(CFE_TBL_TxnRegRec(Txn)))
    {
        Status = CFE_TBL_WARN_SHORT_FILE;
    }
    else
    {
        Status = CFE_SUCCESS;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateCodecConfig(CFE_TBL_TableConfig_t *ReqCfg)
{
    /* Nothing to check here in this implementation */
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_AcquireCodecBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* No separate buffer - just load direct */
    return CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ReleaseCodecBuffer(CFE_TBL_LoadBuff_t *BufferPtr)
{
    /* Nothing to do */
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_EncodeOutputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
                                      CFE_TBL_LoadBuff_t *DestBuffer)
{
    if (SourceBuffer != DestBuffer)
    {
        /* This does a straight copy, the bits are the same */
        CFE_TBL_LoadBuffCopyData(DestBuffer, CFE_TBL_LoadBuffGetReadPointer(SourceBuffer),
                                 CFE_TBL_LoadBuffGetContentSize(SourceBuffer));
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DecodeInputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
                                     CFE_TBL_LoadBuff_t *DestBuffer)
{
    if (SourceBuffer != DestBuffer)
    {
        /* This does a straight copy, the bits are the same */
        CFE_TBL_LoadBuffCopyData(DestBuffer, CFE_TBL_LoadBuffGetReadPointer(SourceBuffer),
                                 CFE_TBL_LoadBuffGetContentSize(SourceBuffer));
    }

    return CFE_SUCCESS;
}
