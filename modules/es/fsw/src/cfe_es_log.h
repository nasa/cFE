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

/**
 * @file
 *
 *  Purpose:
 *    This file contains definitions needed for the cFE ES Logs. The
 *    logs include the Mode Transition log, the System Log, and the
 *    Performance log.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */

#ifndef CFE_ES_LOG_H
#define CFE_ES_LOG_H

/*
** Include Files
*/
#include "common_types.h"
#include "cfe_es_api_typedefs.h"
#include "cfe_time_api_typedefs.h"

#include <stdarg.h> /* required for "va_list" */

/*
** Macro Definitions
*/

/**
 * Buffer size for system log messages
 *
 * This is based on the EVS maximum event message size, plus a time stamp
 * and required extra formatting characters.
 *
 * Two extra characters are necessary:
 *   - for the space between the timestamp and the message in the system log
 *   - to enforce a newline character at the end of the string
 *
 * note that a null terminator byte is accounted for in "CFE_TIME_PRINTED_STRING_SIZE"
 */
#define CFE_ES_MAX_SYSLOG_MSG_SIZE (CFE_MISSION_EVS_MAX_MESSAGE_LENGTH + CFE_TIME_PRINTED_STRING_SIZE + 2)

/**
 * Size of the syslog "dump buffer"
 *
 * This is a temporary buffer that serves as a holding place for syslog data as
 * it is being dumped to a file on disk.  Since disks are comparatively slow and
 * access to the syslog buffer must be synchronized, copying to a temporary buffer
 * first significantly decreases the amount of time that the syslog is locked after
 * a file dump is requested.
 *
 * This buffer also reflects the SysLog "burst size" that is guaranteed to be
 * safe for concurrent writes and reads/dump operations.  If applications Log more than
 * this amount of data in less time than it takes to write this amount of data to disk,
 * then some log messages may be corrupt or lost in the output file.
 *
 * @note If contention occurs where applications would overwrite logs that are still
 * being "read" by a dump process, the realtime applications are given preference and
 * therefore NOT blocked.   Design preference is given to applications over the absolute
 * integrity of the dump file.
 */
#define CFE_ES_SYSLOG_READ_BUFFER_SIZE (3 * CFE_ES_MAX_SYSLOG_MSG_SIZE)

/**
 * \brief Indicates no context information Error Logs
 *
 * For use with the CFE_ES_WriteToERLog() function when no context
 * information is available.
 */
#define CFE_ES_ERLOG_NO_CONTEXT (0)

/*
** Type Definitions
*/

/**
 * \brief Buffer structure for reading data out of the SysLog
 *
 * Access to the syslog must be synchronized, so it is not possible to
 * directly access the contents.  This structure keeps the state of
 * read operations such that the syslog can be read in segments.
 *
 * @sa CFE_ES_SysLogReadData(), CFE_ES_SysLogReadStart_Unsync()
 */
typedef struct
{
    size_t SizeLeft;   /**< Total amount of unread syslog data */
    size_t BlockSize;  /**< Size of content currently in the "Data" member */
    size_t EndIdx;     /**< End of the syslog buffer at the time reading started */
    size_t LastOffset; /**< Current Read Position */

    char Data[CFE_ES_SYSLOG_READ_BUFFER_SIZE]; /**< Actual syslog content */
} CFE_ES_SysLogReadBuffer_t;

/*
** Function prototypes
*/

/*
** System log management
**
** NOTE: CFE_ES_WriteToSysLog() is a public routine in cfe_es.h, it is not prototyped here
*/

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Clear system log
 *
 * This discards the entire system log buffer and resets internal index values
 *
 * \note This function requires external thread synchronization
 */
void CFE_ES_SysLogClear_Unsync(void);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Begin reading the system log
 *
 * This a helper function is intended to assist with the "Write" command to dump
 * the contents of the syslog to a disk file.  This locates the oldest complete
 * log message currently contained in the buffer.
 *
 * The oldest log message may be overwritten when any application calls
 * CFE_ES_WriteToSysLog() if set to OVERWRITE mode.
 *
 * This function only locates the first message, it does not actually copy any
 * data to the supplied buffer.  The CFE_ES_SysLogReadData() should be called
 * to read log data.
 *
 * \param Buffer  A local buffer which will be initialized to the start of the log buffer
 *
 * \note This function requires external thread synchronization
 * \sa CFE_ES_SysLogReadData()
 */
void CFE_ES_SysLogReadStart_Unsync(CFE_ES_SysLogReadBuffer_t *Buffer);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Write a printf-style formatted string to the system log
 *
 * This is a drop-in replacement for the existing CFE_ES_WriteToSysLog() API
 * that does _not_ perform any synchronization or locking.  It is intended for
 * logging from within the ES subsystem where the appropriate lock is
 * already held for other reasons.
 *
 * \note This function requires external thread synchronization
 */
int32 CFE_ES_SysLogWrite_Unsync(const char *SpecStringPtr, ...);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Append a complete pre-formatted string to the ES SysLog
 *
 * The new message will be copied to the current write location in the
 * system log buffer.  If there is not sufficient space to completely store
 * the message, then the behavior depends on the "LogMode" setting.
 *
 * If "LogMode" is set to DISCARD, then the message will be truncated
 * to fit in the available space, or completely discarded if no space exists.
 *
 * If "LogMode" is set to OVERWRITE, then the oldest message(s) in the
 * system log will be overwritten with this new message.
 *
 * \param LogString     Message to append
 *
 * \note This function requires external thread synchronization
 * \sa CFE_ES_SysLogSetMode()
 */
int32 CFE_ES_SysLogAppend_Unsync(const char *LogString);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Read data from the system log buffer into the local buffer
 *
 * Prior to calling this function, the buffer structure should be initialized
 * using CFE_ES_SysLogReadStart_Unsync()
 *
 * This copies the data from the syslog memory space into the local buffer, starting
 * from the end of the previously read data.  To read the complete system log,
 * this function should be called repeatedly until the "BlockSize" member in the
 * returned buffer is returned as zero, indicating there is no more data in the syslog.
 *
 * There is no specific external synchronization requirement on this function, since
 * copies of the relevant log indices are kept in the buffer structure itself.  However,
 * if system log data is overwritten between calls to this function, it may result in
 * undefined data being returned to the caller.
 *
 * Therefore, in cases where it is critically important to read log message data, the
 * lock should be held for the entire procedure (initialization through complete read).
 * However this may have significant realtime implications, so it is not the required
 * mode of operation.
 *
 * \param Buffer  A local buffer which will be filled with data from the log buffer
 */
void CFE_ES_SysLogReadData(CFE_ES_SysLogReadBuffer_t *Buffer);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Sets the operating mode of the system log buffer
 *
 * The operating mode of the system log controls its behavior once filled to the point
 * where additional messages can no longer be stored.
 *
 * If "Mode" is set to DISCARD, then the message will be truncated
 * to fit in the available space, or completely discarded if no space exists.
 *
 * If "Mode" is set to OVERWRITE, then the oldest message(s) in the
 * system log will be overwritten with this new message.
 *
 * \note Switching from OVERWRITE to DISCARD mode may take effect immediately, as the
 * setting only takes effect when the buffer "wrap-point" is reached at the end.
 *
 * \param Mode   The desired operating mode
 * \return CFE_SUCCESS if set successfully
 */
int32 CFE_ES_SysLogSetMode(CFE_ES_LogMode_Enum_t Mode);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Format a message intended for output to the system log
 *
 * This function prepares a complete message for passing into CFE_ES_SysLogAppend_Unsync(),
 * based on the given vsnprintf-style specification string and argument list.
 *
 * The message is prefixed with a time stamp based on the current time, followed by the
 * caller-specified string.  An ending newline and terminating null character are both
 * ensured on the output string.
 *
 * To account for the timestamp, newline, and terminating null character, the supplied buffer
 * must be greater than (CFE_TIME_PRINTED_STRING_SIZE+2) to get a useful output.  Any user-specified
 * output string will be truncated to fit into the remaining space.
 *
 * \param Buffer        User supplied buffer to output formatted string into
 * \param BufferSize    Size of "Buffer" parameter.  Should be greater than (CFE_TIME_PRINTED_STRING_SIZE+2)
 * \param SpecStringPtr Printf-style format string
 * \param ArgPtr        Variable argument list as obtained by va_start() in the caller
 *
 * \sa CFE_ES_SysLogAppend_Unsync()
 */
void CFE_ES_SysLog_vsnprintf(char *Buffer, size_t BufferSize, const char *SpecStringPtr, va_list ArgPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Write the contents of the syslog to a disk file
 *
 * Writes the current contents of the syslog buffer to a file specified
 * by the Filename parameter.  The log messages will be written to the file
 * in the same order in which they were written into the syslog buffer.
 *
 * A snapshot of the log indices is taken at the beginning of the writing
 * process.  Additional log entries added after this (e.g. from applications
 * calling CFE_ES_WriteToSysLog() after starting a syslog dump) will not be
 * included in the dump file.
 *
 * Note that preference is given to the realtime application threads over
 * any pending log read activities, such as a dumping to a file.  The design
 * of this function can tolerate a limited level of logging activity while
 * the dump is in progress without any negative side effects.  However, a significant
 * "flood" of log messages may corrupt the output file, by overwriting older data
 * before it has actually been written.
 *
 * \param Filename      Output file to write
 * \return CFE_SUCCESS if successful, or an appropriate error code from cfe_error.h
 *
 * \sa CFE_ES_SYSLOG_READ_BUFFER_SIZE
 *
 */
int32 CFE_ES_SysLogDump(const char *Filename);

/*
** Exception and Reset Log API
*/

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Create an entry in the ES Exception and Reset Log.
 *
 * The exception and reset log is used to track significant system-level events and anomalies
 * for later analysis.
 *
 * \param EntryType Whether the event is relevant to the CORE or an APPLICATION (#CFE_ES_LogEntryType_Enum_t)
 * \param ResetType The type of the last reset
 * \param ResetSubtype The subtype of the last reset
 * \param Description A summary of the event
 *
 * \return CFE_SUCCESS if successful, or an appropriate error code from cfe_error.h
 */
int32 CFE_ES_WriteToERLog(CFE_ES_LogEntryType_Enum_t EntryType, uint32 ResetType, uint32 ResetSubtype,
                          const char *Description);

/*---------------------------------------------------------------------------------------*/
/**
 * \copydoc CFE_ES_WriteToERLog()
 *
 * This log API accepts extra context information (AppID and ContextID)
 * and is used when the app/task invoking this API is not the same app
 * as where the event occurred.
 *
 * \param AppId The Application ID associated with the task that caused the exception
 * \param PspContextId Identifier of extended context info stored in the PSP (if available)
 */
int32 CFE_ES_WriteToERLogWithContext(CFE_ES_LogEntryType_Enum_t EntryType, uint32 ResetType, uint32 ResetSubtype,
                                     const char *Description, CFE_ES_AppId_t AppId, uint32 PspContextId);

#endif /* CFE_ES_LOG_H */
