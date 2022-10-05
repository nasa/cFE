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
**  File:
**    cfe_es_syslog.c
**
**  Purpose:
**    This file implements the cFE Executive Services System Log functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
**     Some functions have EXTERNAL SYNC REQUIREMENTS
**
**     SysLog functions marked with "Unsync" in their name are designated
**     as functions which are _not_ safe to be called concurrently by multiple
**     threads, and also do _not_ implement any locking or protection.  These
**     functions expect the caller to perform all thread synchronization before
**     calling it.
**
**     The synchronization requirement is across all functions; i.e. it is not safe
**     to call B_Unsync() while A_Unsync() is executing or vice-versa.  The external
**     lock must wait until A_Unsync() finishes before calling B_Unsync().
**
**     The expectation is that the required level of synchronization can be achieved
**     using the existing ES shared data lock.  However, if it becomes necessary, this
**     could be replaced with a finer grained syslog-specific lock.
*/

/*
** Required header files.
*/
#include "cfe_es_module_all.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

/*******************************************************************
 *
 * Non-synchronized helper functions
 *
 * An external mutex be held while calling any function marked "Unsync"
 * These helper functions are local to the ES subsystem and must _NOT_
 * be exposed to the public API.
 *
 * For external access, a public wrapper API must first acquire the
 * necessary mutex before calling any function marked as "Unsync"
 *
 *******************************************************************/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_SysLogClear_Unsync(void)
{
    /*
     * Note - no need to actually memset the SystemLog buffer -
     * by simply zeroing out the indices will cover it.
     */

    CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = 0;
    CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = 0;
    CFE_ES_Global.ResetDataPtr->SystemLogEntryNum = 0;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_SysLogReadStart_Unsync(CFE_ES_SysLogReadBuffer_t *Buffer)
{
    size_t ReadIdx;
    size_t EndIdx;
    size_t TotalSize;

    ReadIdx   = CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx;
    EndIdx    = CFE_ES_Global.ResetDataPtr->SystemLogEndIdx;
    TotalSize = EndIdx;

    /*
     * Ensure that we start reading at the start of a message
     * Likely pointing to an old fragment right now -- find the end of it
     */
    while (TotalSize > 0 && ReadIdx < EndIdx)
    {
        ++ReadIdx;
        --TotalSize;
        if (CFE_ES_Global.ResetDataPtr->SystemLog[ReadIdx - 1] == '\n')
        {
            break;
        }
    }

    Buffer->SizeLeft   = TotalSize;
    Buffer->LastOffset = ReadIdx;
    Buffer->EndIdx     = EndIdx;
    Buffer->BlockSize  = 0;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SysLogAppend_Unsync(const char *LogString)
{
    int32  ReturnCode;
    size_t MessageLen;
    size_t WriteIdx;
    size_t EndIdx;

    /*
     * Sanity check - Make sure the message length is actually reasonable
     * Do not allow any single message to consume more than half of the total log
     * (even this may be overly generous)
     */
    MessageLen = strlen(LogString);
    if (MessageLen > (CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2))
    {
        MessageLen = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE / 2;
        ReturnCode = CFE_ES_ERR_SYS_LOG_TRUNCATED;
    }
    else
    {
        ReturnCode = CFE_SUCCESS;
    }

    /*
     * Final sanity check -- do not bother logging empty messages
     */
    if (MessageLen == 0)
    {
        return ReturnCode;
    }

    /*
     * Real work begins --
     * Take a local snapshot of the head & tail index values
     *
     * WriteIdx -> indicates 1 byte past the end of the newest message
     *      (this is the place where new messages will be added)
     *
     * EndIdx -> indicates the entire size of the buffer
     *
     * Keeping them in local stack variables allows more efficient modification,
     * since CFE_ES_Global.ResetDataPtr may point directly into a slower NVRAM space.
     */
    WriteIdx = CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx;
    EndIdx   = CFE_ES_Global.ResetDataPtr->SystemLogEndIdx;

    /*
     * Check if the log message plus will fit between
     * the HeadIdx and the end of the buffer.
     *
     * If so, then the process can proceed as normal.
     *
     * If not, then the action depends on the setting of "SystemLogMode" which will be
     * to either discard (default) or overwrite
     */
    if ((WriteIdx + MessageLen) > CFE_PLATFORM_ES_SYSTEM_LOG_SIZE)
    {
        if (CFE_ES_Global.ResetDataPtr->SystemLogMode == CFE_ES_LogMode_OVERWRITE)
        {
            /* In "overwrite" mode, start back at the beginning of the buffer */
            EndIdx   = WriteIdx;
            WriteIdx = 0;
        }
        else if (WriteIdx < (CFE_PLATFORM_ES_SYSTEM_LOG_SIZE - CFE_TIME_PRINTED_STRING_SIZE))
        {
            /* In "discard" mode, save as much as possible and discard the remainder of the message
             * However this should only be done if there is enough room for at least a full timestamp,
             * otherwise the fragment will not be useful at all. */
            MessageLen = CFE_PLATFORM_ES_SYSTEM_LOG_SIZE - WriteIdx;
            ReturnCode = CFE_ES_ERR_SYS_LOG_TRUNCATED;
        }
        else
        {
            /* entire message must be discarded */
            MessageLen = 0;
        }
    }

    if (MessageLen == 0)
    {
        ReturnCode = CFE_ES_ERR_SYS_LOG_FULL;
    }
    else
    {
        /*
         * Copy the message in, EXCEPT for the last char which is probably a newline
         */
        memcpy(&CFE_ES_Global.ResetDataPtr->SystemLog[WriteIdx], LogString, MessageLen - 1);
        WriteIdx += MessageLen;

        /*
         * Ensure that the last-written character is a newline.
         * This would have been enforced already except in cases where
         * the message got truncated.
         */
        CFE_ES_Global.ResetDataPtr->SystemLog[WriteIdx - 1] = '\n';

        /*
         * Keep track of the buffer endpoint for future reference
         */
        if (WriteIdx > EndIdx)
        {
            EndIdx = WriteIdx;
        }

        /*
         * Export updated index values to the reset area for next time.
         */
        CFE_ES_Global.ResetDataPtr->SystemLogWriteIdx = WriteIdx;
        CFE_ES_Global.ResetDataPtr->SystemLogEndIdx   = EndIdx;
        ++CFE_ES_Global.ResetDataPtr->SystemLogEntryNum;
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SysLogWrite_Unsync(const char *SpecStringPtr, ...)
{
    char    TmpString[CFE_ES_MAX_SYSLOG_MSG_SIZE];
    va_list ArgPtr;

    va_start(ArgPtr, SpecStringPtr);
    CFE_ES_SysLog_vsnprintf(TmpString, sizeof(TmpString), SpecStringPtr, ArgPtr);
    va_end(ArgPtr);

    /* Output the entry to the console */
    OS_printf("%s", TmpString);

    /*
     * Append to the syslog buffer
     */
    return CFE_ES_SysLogAppend_Unsync(TmpString);
}

/*******************************************************************
 *
 * Additional helper functions
 *
 * These functions either perform all necessary synchronization internally,
 * or they have no specific synchronization requirements
 *
 *******************************************************************/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_SysLogReadData(CFE_ES_SysLogReadBuffer_t *Buffer)
{
    size_t BlockSize;

    Buffer->BlockSize = 0;
    while (Buffer->SizeLeft > 0 && Buffer->BlockSize < sizeof(Buffer->Data))
    {
        /*
         * The next block to copy will be the SMALLEST of:
         *  - total remaining (un-copied) size of the syslog data
         *  - space available in the output buffer
         *  - space between the current read offset and the end of the log buffer (wrap point)
         */
        BlockSize = sizeof(Buffer->Data) - Buffer->BlockSize;
        if (Buffer->LastOffset >= Buffer->EndIdx)
        {
            Buffer->LastOffset = 0;
        }
        if ((Buffer->LastOffset + BlockSize) > Buffer->EndIdx)
        {
            BlockSize = Buffer->EndIdx - Buffer->LastOffset;
        }
        if (BlockSize > Buffer->SizeLeft)
        {
            BlockSize = Buffer->SizeLeft;
        }

        if (BlockSize == 0)
        {
            /* should be impossible for this to happen,
             * just in case, do not spin endlessly */
            break;
        }

        memcpy(&Buffer->Data[Buffer->BlockSize], &CFE_ES_Global.ResetDataPtr->SystemLog[Buffer->LastOffset], BlockSize);

        Buffer->BlockSize += BlockSize;
        Buffer->LastOffset += BlockSize;
        Buffer->SizeLeft -= BlockSize;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SysLogSetMode(CFE_ES_LogMode_Enum_t Mode)
{
    int32 Status;

    if ((Mode == CFE_ES_LogMode_OVERWRITE) || (Mode == CFE_ES_LogMode_DISCARD))
    {
        CFE_ES_Global.ResetDataPtr->SystemLogMode = Mode;
        Status                                    = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_ES_BAD_ARGUMENT;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_SysLog_vsnprintf(char *Buffer, size_t BufferSize, const char *SpecStringPtr, va_list ArgPtr)
{
    size_t StringLen;
    size_t MaxLen;
    int    PrintLen;

    /*
     * write the current time into the TmpString buffer
     *
     * Note that CFE_TIME_Print() is expected to produce a string of exactly
     * CFE_TIME_PRINTED_STRING_SIZE in length.
     */
    StringLen = 0;
    if (BufferSize > (CFE_TIME_PRINTED_STRING_SIZE + 2))
    {
        /*
         * The "useful" buffer size is two less than the supplied buffer -
         * due to the addition of a newline and a null char to terminate the string
         */
        MaxLen = BufferSize - 2;

        CFE_TIME_Print(Buffer, CFE_TIME_GetTime());

        /* using strlen() anyway in case the specific format of CFE_TIME_Print() changes someday */
        StringLen = strlen(Buffer);
        if (StringLen < MaxLen)
        {
            /* overwrite null with a space to separate the timestamp from the content */
            Buffer[StringLen] = ' ';
            ++StringLen;

            /* note that vsnprintf() may return a size larger than the buffer, if it truncates. */
            PrintLen = vsnprintf(&Buffer[StringLen], BufferSize - StringLen, SpecStringPtr, ArgPtr);
            if (PrintLen > 0)
            {
                StringLen += PrintLen;
            }
        }

        if (StringLen > MaxLen)
        {
            /* the message got truncated */
            StringLen = MaxLen;
        }

        /*
         * Finalize the output string.
         *
         * To be consistent when writing to the console, it is important that
         * every printed string end in a newline - particularly if the console is buffered.
         *
         * The caller may or may not have included a newline in the original format
         * string.  Most callers do, but some do not.
         *
         * Strip off all trailing whitespace, and add back a single newline
         */
        while (StringLen > 0 && isspace((unsigned char)Buffer[StringLen - 1]))
        {
            --StringLen;
        }
        Buffer[StringLen] = '\n';
        ++StringLen;
    }

    if (BufferSize > 0)
    {
        /* always output a null terminated string */
        Buffer[StringLen] = 0;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SysLogDump(const char *Filename)
{
    osal_id_t fd = OS_OBJECT_ID_UNDEFINED;
    int32     OsStatus;
    int32     Status;
    size_t    WritePos;
    size_t    TotalSize;
    size_t    LastReqSize;
    union
    {
        CFE_ES_SysLogReadBuffer_t LogData;
        CFE_FS_Header_t           FileHdr;
    } Buffer;

    OsStatus = OS_OpenCreate(&fd, Filename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
    if (OsStatus != OS_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_ES_SYSLOG2_ERR_EID, CFE_EVS_EventType_ERROR, "Error creating file %s, RC = %ld", Filename,
                          (long)OsStatus);
        return CFE_ES_FILE_IO_ERR;
    }

    CFE_FS_InitHeader(&Buffer.FileHdr, CFE_ES_SYS_LOG_DESC, CFE_FS_SubType_ES_SYSLOG);

    TotalSize   = 0;
    LastReqSize = sizeof(CFE_FS_Header_t);
    Status      = CFE_FS_WriteHeader(fd, &Buffer.FileHdr);
    if (Status >= 0)
    {
        TotalSize += Status;

        /*
         * Get a snapshot of the buffer pointers and read the first block of
         * data while locked - ensuring that nothing additional can be written
         * into the syslog buffer while getting the first block of log data.
         */
        CFE_ES_LockSharedData(__func__, __LINE__);
        CFE_ES_SysLogReadStart_Unsync(&Buffer.LogData);
        CFE_ES_SysLogReadData(&Buffer.LogData);
        CFE_ES_UnlockSharedData(__func__, __LINE__);

        while (Buffer.LogData.BlockSize > 0)
        {
            WritePos = 0;
            while (WritePos < Buffer.LogData.BlockSize)
            {
                LastReqSize = Buffer.LogData.BlockSize - WritePos;
                OsStatus    = OS_write(fd, &Buffer.LogData.Data[WritePos], LastReqSize);

                /* NOTE: this stops for errors or EOF code (0) */
                if (OsStatus <= 0)
                {
                    break;
                }

                WritePos += (long)OsStatus;
                TotalSize += (long)OsStatus;
            }

            if (OsStatus <= 0)
            {
                Status = (long)OsStatus;
                break;
            }

            /*
             * _NOT_ taking the lock for subsequent reads --
             *
             * All syslog index values use the local snapshots that were taken earlier.
             * (The shared memory index values are not referenced on subsequent reads)
             *
             * If a new syslog message _does_ get written while this is in progress, it
             * should be writing to a different part of the syslog buffer anyway, and
             * probably will not overwrite the data about to be read here.
             *
             * There is still a possibility of a "flood" of syslogs coming in which would
             * potentially overwrite unread data and cause message loss/corruption.  However
             * taking a lock here will not alleviate that situation - this means that the
             * buffer simply isn't big enough.
             */
            CFE_ES_SysLogReadData(&Buffer.LogData);
        }
    }

    OS_close(fd);

    if (Status <= 0)
    {
        CFE_ES_FileWriteByteCntErr(Filename, LastReqSize, Status);
        Status = CFE_ES_FILE_IO_ERR;
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_SYSLOG2_EID, CFE_EVS_EventType_DEBUG, "%s written:Size=%lu,Entries=%u", Filename,
                          (unsigned long)TotalSize,
                          (unsigned int)CFE_ES_Global.TaskData.HkPacket.Payload.SysLogEntries);
        Status = CFE_SUCCESS;
    }

    return Status;
}
