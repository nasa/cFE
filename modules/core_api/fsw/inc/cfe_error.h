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
 *  Title:    cFE Status Code Definition Header File
 *
 *  Purpose:
 *            Common source of cFE API return status codes.
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 */

#ifndef CFE_ERROR_H
#define CFE_ERROR_H

/* Include Files */
#include "osapi.h"

/**
 * \brief cFE Status type for readability and eventually type safety
 */
typedef int32 CFE_Status_t;

/**
 * \brief cFE Status macro for literal
 */
#define CFE_STATUS_C(X) ((CFE_Status_t)(X))

/**
 * \brief cFE Status converted to string length limit
 *
 * Used for sizing CFE_StatusString_t intended for use in printing CFE_Status_t values
 * Sized for 0x%08x and NULL
 */
#define CFE_STATUS_STRING_LENGTH 11

/**
 * @brief For the @ref CFE_ES_StatusToString() function, to ensure
 * everyone is making an array of the same length.
 */
typedef char CFE_StatusString_t[CFE_STATUS_STRING_LENGTH];

/**
 * @brief Convert status to a string
 *
 * @param[in] status Status value to convert
 * @param[out] status_string Buffer to store status converted to string
 *
 * @return Passed in string pointer
 */
char *CFE_ES_StatusToString(CFE_Status_t status, CFE_StatusString_t *status_string);

/*
**  Status Codes are 32 bit values formatted as follows:
**
**   3 3 2 2 2 2 2 2 2 2 2 2 1 1 1 1 1 1 1 1 1 1
**   1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0 9 8 7 6 5 4 3 2 1 0
**  +---+---+-----+-----------------+-------------------------------+
**  |Sev| R | Srv | Mission Defined |               Code            |
**  +---+---+-----+-----------------+-------------------------------+
**
**  where
**
**      Sev - is the severity code
**
**          00 - Success
**          01 - Informational
**          11 - Error
**
**      R - are reserved bits
**
**      Srv - is the cFE Service Identifier
**
**          000 - Not a cFE Service
**          001 - Events Services
**          010 - Executive Services
**          011 - File Services
**          100 - Generic code for all services
**          101 - Software Bus Services
**          110 - Tables Services
**          111 - Time Services
**
**      Mission Defined - These bits are available for Mission
**                        specific coding standards.  They can
**                        be used to classify error codes related
**                        to mission specific library function calls, etc.
**
**      Code - is the status code
*/

/*
** Error Severity
*/
#define CFE_SEVERITY_BITMASK ((CFE_Status_t)0xc0000000) /**< @brief Error Severity Bitmask */

#define CFE_SEVERITY_SUCCESS ((CFE_Status_t)0x00000000) /**< @brief Severity Success */
#define CFE_SEVERITY_INFO    ((CFE_Status_t)0x40000000) /**< @brief Severity Info */
#define CFE_SEVERITY_ERROR   ((CFE_Status_t)0xc0000000) /**< @brief Severity Error */

/*
** cFE Service Identifiers
*/
#define CFE_SERVICE_BITMASK ((CFE_Status_t)0x0e000000) /**< @brief Error Service Bitmask */

#define CFE_EVENTS_SERVICE       ((CFE_Status_t)0x02000000) /**< @brief Event Service */
#define CFE_EXECUTIVE_SERVICE    ((CFE_Status_t)0x04000000) /**< @brief Executive Service */
#define CFE_FILE_SERVICE         ((CFE_Status_t)0x06000000) /**< @brief File Service */
#define CFE_GENERIC_SERVICE      ((CFE_Status_t)0x08000000) /**< @brief Generic Service */
#define CFE_SOFTWARE_BUS_SERVICE ((CFE_Status_t)0x0a000000) /**< @brief Software Bus Service */
#define CFE_TABLE_SERVICE        ((CFE_Status_t)0x0c000000) /**< @brief Table Service */
#define CFE_TIME_SERVICE         ((CFE_Status_t)0x0e000000) /**< @brief Time Service */

/*
************* COMMON STATUS CODES *************
*/

/** @defgroup CFEReturnCodes cFE Return Code Defines
 * @{
 */

/**
 * @brief Successful execution
 *
 *  Operation was performed successfully
 */
#define CFE_SUCCESS ((CFE_Status_t)0)

/**
 * @brief No Counter Increment
 *
 *  Informational code indicating that a command was processed
 *  successfully but that the command counter should _not_ be incremented.
 */
#define CFE_STATUS_NO_COUNTER_INCREMENT ((CFE_Status_t)0x48000001)

/**
 * @brief Wrong Message Length
 *
 *  This error code will be returned when a message validation process
 *  determined that the message length is incorrect
 *
 */
#define CFE_STATUS_WRONG_MSG_LENGTH ((CFE_Status_t)0xc8000002)

/**
 * @brief Unknown Message ID
 *
 *  This error code will be returned when a message identification process
 *  determined that the message ID does not correspond to a known value
 *
 */
#define CFE_STATUS_UNKNOWN_MSG_ID ((CFE_Status_t)0xc8000003)

/**
 * @brief Bad Command Code
 *
 *  This error code will be returned when a message identification process
 *  determined that the command code is does not correspond to any known value
 *
 */
#define CFE_STATUS_BAD_COMMAND_CODE ((CFE_Status_t)0xc8000004)

/**
 * @brief External failure
 *
 *  This error indicates that the operation failed for
 *  some reason outside the scope of CFE.  The real failure may
 *  have been in OSAL, PSP, or another dependent library.
 *
 *  Details of the original failure should be written to syslog
 *  and/or a system event before returning this error.
 */
#define CFE_STATUS_EXTERNAL_RESOURCE_FAIL ((CFE_Status_t)0xc8000005)

/**
 * @brief Request already pending
 *
 *  Commands or requests are already pending or the pending request
 *  limit has been reached.  No more requests can be made until
 *  the current request(s) complete.
 */
#define CFE_STATUS_REQUEST_ALREADY_PENDING ((int32)0xc8000006)

/**
 * @brief Request or input value failed basic structural validation
 *
 * A message or table input was not in the proper format to be understood
 * and processed by an application, and was rejected.
 */
#define CFE_STATUS_VALIDATION_FAILURE ((int32)0xc8000007)

/**
 * @brief Request or input value is out of range
 *
 * A message, table, or function call input contained a value that was outside
 * the acceptable range, and the request was rejected.
 */
#define CFE_STATUS_RANGE_ERROR ((int32)0xc8000008)

/**
 * @brief Cannot process request at this time
 *
 * The system is not currently in the correct state to accept the request at
 * this time.
 */
#define CFE_STATUS_INCORRECT_STATE ((int32)0xc8000009)

/**
 * @brief Not Implemented
 *
 *  Current version does not have the function or the feature
 *  of the function implemented.  This could be due to either an early
 *  build for this platform or the platform does not support
 *  the specified feature.
 *
 */
#define CFE_STATUS_NOT_IMPLEMENTED ((CFE_Status_t)0xc800ffff)

/*
************* EVENTS SERVICES STATUS CODES *************
*/

/**
 * @brief Unknown Filter
 *
 *  #CFE_EVS_Register FilterScheme parameter was illegal
 *
 */
#define CFE_EVS_UNKNOWN_FILTER ((CFE_Status_t)0xc2000001)

/**
 * @brief Application Not Registered
 *
 *  Calling application never previously called #CFE_EVS_Register
 *
 */
#define CFE_EVS_APP_NOT_REGISTERED ((CFE_Status_t)0xc2000002)

/**
 * @brief Illegal Application ID
 *
 *  Application ID returned by #CFE_ES_GetAppIDByName is greater
 *  than #CFE_PLATFORM_ES_MAX_APPLICATIONS
 *
 */
#define CFE_EVS_APP_ILLEGAL_APP_ID ((CFE_Status_t)0xc2000003)

/**
 * @brief Application Filter Overload
 *
 *  Number of Application event filters input upon
 *  registration is greater than #CFE_PLATFORM_EVS_MAX_EVENT_FILTERS
 *
 */
#define CFE_EVS_APP_FILTER_OVERLOAD ((CFE_Status_t)0xc2000004)

/**
 * @brief Reset Area Pointer Failure
 *
 *  Could not get pointer to the ES Reset area, so we could
 *  not get the pointer to the EVS Log.
 *
 */
#define CFE_EVS_RESET_AREA_POINTER ((CFE_Status_t)0xc2000005)

/**
 * @brief Event Not Registered
 *
 *  #CFE_EVS_ResetFilter EventID argument was not found in
 *  any event filter registered by the calling application.
 *
 */
#define CFE_EVS_EVT_NOT_REGISTERED ((CFE_Status_t)0xc2000006)

/**
 * @brief File Write Error
 *
 *  A file write error occurred while processing an EVS command
 *
 */
#define CFE_EVS_FILE_WRITE_ERROR ((CFE_Status_t)0xc2000007)

/**
 * @brief Invalid Pointer
 *
 *  Invalid parameter supplied to EVS command
 *
 */
#define CFE_EVS_INVALID_PARAMETER ((CFE_Status_t)0xc2000008)

/**
 * @brief Event squelched
 *
 *  Event squelched due to being sent at too high a rate
 *
 */
#define CFE_EVS_APP_SQUELCHED ((CFE_Status_t)0xc2000009)

/**
 * @brief Not Implemented
 *
 *  Current version of cFE does not have the function or the feature
 *  of the function implemented.  This could be due to either an early
 *  build of the cFE for this platform or the platform does not support
 *  the specified feature.
 *
 */
#define CFE_EVS_NOT_IMPLEMENTED ((CFE_Status_t)0xc200ffff)

/*
************* EXECUTIVE SERVICES STATUS CODES *************
*/

/**
 * @brief Resource ID is not valid
 *
 *  This error indicates that the passed in resource identifier
 *  (App ID, Lib ID, Counter ID, etc) did not validate.
 *
 */
#define CFE_ES_ERR_RESOURCEID_NOT_VALID ((CFE_Status_t)0xc4000001)

/**
 * @brief Resource Name Error
 *
 *  There is no match in the system for the given name.
 *
 */
#define CFE_ES_ERR_NAME_NOT_FOUND ((CFE_Status_t)0xc4000002)

/**
 * @brief Application Create Error
 *
 *  There was an error loading or creating the App.
 *
 */
#define CFE_ES_ERR_APP_CREATE ((CFE_Status_t)0xc4000004)

/**
 * @brief Child Task Create Error
 *
 *  There was an error creating a child task.
 *
 */
#define CFE_ES_ERR_CHILD_TASK_CREATE ((CFE_Status_t)0xc4000005)

/**
 * @brief System Log Full
 *
 *  The cFE system Log is full.
 *  This error means the message was not logged at all
 *
 */
#define CFE_ES_ERR_SYS_LOG_FULL ((CFE_Status_t)0xc4000006)

/**
 * @brief Memory Block Size Error
 *
 *  The block size requested is invalid.
 *
 */
#define CFE_ES_ERR_MEM_BLOCK_SIZE ((CFE_Status_t)0xc4000008)

/**
 * @brief Load Library Error
 *
 *  Could not load the shared library.
 *
 */
#define CFE_ES_ERR_LOAD_LIB ((CFE_Status_t)0xc4000009)

/**
 * @brief Bad Argument
 *
 *  Bad parameter passed into an ES API.
 *
 */
#define CFE_ES_BAD_ARGUMENT ((CFE_Status_t)0xc400000a)

/**
 * @brief Child Task Register Error
 *
 *  Errors occurred when trying to register a child task.
 *
 */
#define CFE_ES_ERR_CHILD_TASK_REGISTER ((CFE_Status_t)0xc400000b)

/**
 * @brief CDS Already Exists
 *
 *  The Application is receiving the pointer to a CDS that was already present.
 *
 */
#define CFE_ES_CDS_ALREADY_EXISTS ((CFE_Status_t)0x4400000d)

/**
 * @brief CDS Insufficient Memory
 *
 *  The Application is requesting a CDS Block that is larger than the remaining
 *  CDS memory.
 *
 */
#define CFE_ES_CDS_INSUFFICIENT_MEMORY ((CFE_Status_t)0xc400000e)

/**
 * @brief CDS Invalid Name
 *
 *  The Application is requesting a CDS Block with an invalid ASCII string name.
 *  Either the name is too long (> #CFE_MISSION_ES_CDS_MAX_NAME_LENGTH) or was an empty string.
 *
 */
#define CFE_ES_CDS_INVALID_NAME ((CFE_Status_t)0xc400000f)

/**
 * @brief CDS Invalid Size
 *
 *  The Application is requesting a CDS Block or Pool with a size
 *  beyond the applicable limits, either too large or too small/zero.
 *
 */
#define CFE_ES_CDS_INVALID_SIZE ((CFE_Status_t)0xc4000010)

/**
 * @brief CDS Invalid
 *
 *  The CDS contents are invalid.
 *
 */
#define CFE_ES_CDS_INVALID ((CFE_Status_t)0xc4000012)

/**
 * @brief CDS Access Error
 *
 *  The CDS was inaccessible
 *
 */
#define CFE_ES_CDS_ACCESS_ERROR ((CFE_Status_t)0xc4000013)

/**
 * @brief File IO Error
 *
 *  Occurs when a file operation fails
 *
 */
#define CFE_ES_FILE_IO_ERR ((CFE_Status_t)0xc4000014)

/**
 * @brief Reset Area Access Error
 *
 *  Occurs when the BSP is not successful in returning the reset area address.
 *
 */
#define CFE_ES_RST_ACCESS_ERR ((CFE_Status_t)0xc4000015)

/**
 * @brief Application Register Error
 *
 *  Occurs when a task cannot be registered in ES global tables
 *
 */
#define CFE_ES_ERR_APP_REGISTER ((CFE_Status_t)0xc4000017)

/**
 * @brief Child Task Delete Error
 *
 *  There was an error deleting a child task.
 *
 */
#define CFE_ES_ERR_CHILD_TASK_DELETE ((CFE_Status_t)0xc4000018)

/**
 * @brief Child Task Delete Passed Main Task
 *
 *  There was an attempt to delete a cFE App Main Task with
 *  the #CFE_ES_DeleteChildTask API.
 *
 */
#define CFE_ES_ERR_CHILD_TASK_DELETE_MAIN_TASK ((CFE_Status_t)0xc4000019)

/**
 * @brief CDS Block CRC Error
 *
 *  Occurs when trying to read a CDS Data block and the CRC of the current
 *  data does not match the stored CRC for the data.  Either the contents of
 *  the CDS Data Block are corrupted or the CDS Control Block is corrupted.
 *
 */
#define CFE_ES_CDS_BLOCK_CRC_ERR ((CFE_Status_t)0xc400001A)

/**
 * @brief Mutex Semaphore Delete Error
 *
 *  Occurs when trying to delete a Mutex that belongs to a task that ES
 *  is cleaning up.
 *
 */
#define CFE_ES_MUT_SEM_DELETE_ERR ((CFE_Status_t)0xc400001B)

/**
 * @brief Binary Semaphore Delete Error
 *
 *  Occurs when trying to delete a Binary Semaphore that belongs to a task that ES
 *  is cleaning up.
 *
 */
#define CFE_ES_BIN_SEM_DELETE_ERR ((CFE_Status_t)0xc400001C)

/**
 * @brief Counting Semaphore Delete Error
 *
 *  Occurs when trying to delete a Counting Semaphore that belongs to a task that ES
 *  is cleaning up.
 *
 */
#define CFE_ES_COUNT_SEM_DELETE_ERR ((CFE_Status_t)0xc400001D)

/**
 * @brief Queue Delete Error
 *
 *  Occurs when trying to delete a Queue that belongs to a task that ES
 *  is cleaning up.
 *
 */
#define CFE_ES_QUEUE_DELETE_ERR ((CFE_Status_t)0xc400001E)

/**
 * @brief File Close Error
 *
 *  Occurs when trying to close a file that belongs to a task that ES
 *  is cleaning up.
 *
 */
#define CFE_ES_FILE_CLOSE_ERR ((CFE_Status_t)0xc400001F)

/**
 * @brief CDS Wrong Type Error
 *
 *  Occurs when Table Services is trying to delete a Critical Data Store that
 *  is not a Critical Table Image or when Executive Services is trying to delete
 *  a Critical Table Image.
 *
 */
#define CFE_ES_CDS_WRONG_TYPE_ERR ((CFE_Status_t)0xc4000020)

/**
 * @brief CDS Owner Active Error
 *
 *  Occurs when an attempt was made to delete a CDS when an application
 *  with the same name associated with the CDS is still present.  CDSs
 *  can ONLY be deleted when Applications that created them are not present
 *  in the system.
 *
 */
#define CFE_ES_CDS_OWNER_ACTIVE_ERR ((CFE_Status_t)0xc4000022)

/**
 * @brief Application Cleanup Error
 *
 *  Occurs when an attempt was made to Clean Up an application
 *  which involves calling Table, EVS, and SB cleanup functions, then
 *  deleting all ES resources, child tasks, and unloading the
 *  object module. The approach here is to keep going even though one
 *  of these steps had an error. There will be syslog messages detailing
 *  each problem.
 *
 */
#define CFE_ES_APP_CLEANUP_ERR ((CFE_Status_t)0xc4000023)

/**
 * @brief Timer Delete Error
 *
 *  Occurs when trying to delete a Timer that belongs to a task that ES
 *  is cleaning up.
 *
 */
#define CFE_ES_TIMER_DELETE_ERR ((CFE_Status_t)0xc4000024)

/**
 * @brief Buffer Not In Pool
 *
 *  The specified address is not in the memory pool.
 *
 */
#define CFE_ES_BUFFER_NOT_IN_POOL ((CFE_Status_t)0xc4000025)

/**
 * @brief Task Delete Error
 *
 *  Occurs when trying to delete a task that ES
 *  is cleaning up.
 *
 */
#define CFE_ES_TASK_DELETE_ERR ((CFE_Status_t)0xc4000026)

/**
 * @brief Operation Timed Out
 *
 *  Occurs if the timeout for a given operation was exceeded
 *
 */
#define CFE_ES_OPERATION_TIMED_OUT ((CFE_Status_t)0xc4000027)

/**
 * @brief Library Already Loaded
 *
 *  Occurs if CFE_ES_LoadLibrary detects that the requested
 *  library name is already loaded.
 *
 */
#define CFE_ES_LIB_ALREADY_LOADED ((CFE_Status_t)0x44000028)

/**
 * @brief System Log Message Truncated
 *
 *  This information code means the last syslog message was truncated
 *  due to insufficient space in the log buffer.
 *
 */
#define CFE_ES_ERR_SYS_LOG_TRUNCATED ((CFE_Status_t)0x44000029)

/**
 * @brief Resource ID is not available
 *
 *  This error indicates that the maximum resource identifiers
 *  (App ID, Lib ID, Counter ID, etc) has already been reached
 *  and a new ID cannot be allocated.
 *
 */
#define CFE_ES_NO_RESOURCE_IDS_AVAILABLE ((CFE_Status_t)0xc400002B)

/**
 * @brief Invalid pool block
 *
 *  Software attempted to "put" a block back into a pool which
 *  does not appear to belong to that pool.  This may mean the
 *  pool has become unusable due to memory corruption.
 *
 */
#define CFE_ES_POOL_BLOCK_INVALID ((CFE_Status_t)0xc400002C)

/**
 * @brief Duplicate Name Error
 *
 *  Resource creation failed due to the name already existing in the system.
 *
 */
#define CFE_ES_ERR_DUPLICATE_NAME ((CFE_Status_t)0xc400002E)

/**
 * @brief Not Implemented
 *
 *  Current version of cFE does not have the function or the feature
 *  of the function implemented.  This could be due to either an early
 *  build of the cFE for this platform or the platform does not support
 *  the specified feature.
 *
 */
#define CFE_ES_NOT_IMPLEMENTED ((CFE_Status_t)0xc400ffff)

/*
************* FILE SERVICES STATUS CODES *************
*/

/**
 * @brief Bad Argument
 *
 *  A parameter given by a caller to a File Services API did not pass
 *  validation checks.
 *
 */
#define CFE_FS_BAD_ARGUMENT ((CFE_Status_t)0xc6000001)

/**
 * @brief Invalid Path
 *
 *  FS was unable to extract a filename from a path string
 *
 */
#define CFE_FS_INVALID_PATH ((CFE_Status_t)0xc6000002)

/**
 * @brief Filename Too Long
 *
 *  FS filename string is too long
 *
 */
#define CFE_FS_FNAME_TOO_LONG ((CFE_Status_t)0xc6000003)

/**
 * @brief Not Implemented
 *
 *  Current version of cFE does not have the function or the feature
 *  of the function implemented.  This could be due to either an early
 *  build of the cFE for this platform or the platform does not support
 *  the specified feature.
 *
 */
#define CFE_FS_NOT_IMPLEMENTED ((CFE_Status_t)0xc600ffff)

/*
************* SOFTWARE BUS SERVICES STATUS CODES *************
*/

/**
 * @brief Time Out
 *
 *  In #CFE_SB_ReceiveBuffer, this return value indicates that a packet has not
 *  been received in the time given in the "timeout" parameter.
 *
 */
#define CFE_SB_TIME_OUT ((CFE_Status_t)0xca000001)

/**
 * @brief No Message
 *
 *  When "Polling" a pipe for a message in #CFE_SB_ReceiveBuffer, this return
 *  value indicates that there was not a message on the pipe.
 *
 */
#define CFE_SB_NO_MESSAGE ((CFE_Status_t)0xca000002)

/**
 * @brief Bad Argument
 *
 *  A parameter given by a caller to a Software Bus API did not pass
 *  validation checks.
 *
 */
#define CFE_SB_BAD_ARGUMENT ((CFE_Status_t)0xca000003)

/**
 * @brief Max Pipes Met
 *
 *  This error code will be returned from #CFE_SB_CreatePipe when the
 *  SB cannot accommodate the request to create a pipe because the maximum
 *  number of pipes (#CFE_PLATFORM_SB_MAX_PIPES) are in use. This configuration
 *  parameter is defined in the cfe_platform_cfg.h file.
 *
 */
#define CFE_SB_MAX_PIPES_MET ((CFE_Status_t)0xca000004)

/**
 * @brief Pipe Create Error
 *
 *  The maximum number of queues(#OS_MAX_QUEUES) are in use. Or possibly a
 *  lower level problem with creating the underlying queue has occurred
 *  such as a lack of memory. If the latter is the problem, the status
 *  code displayed in the event must be tracked.
 *
 */
#define CFE_SB_PIPE_CR_ERR ((CFE_Status_t)0xca000005)

/**
 * @brief Pipe Read Error
 *
 *  This return value indicates an error at the Queue read level. This
 *  error typically cannot be corrected by the caller. Some possible
 *  causes are: queue was not properly initialized or created, the number
 *  of bytes read from the queue was not the number of bytes requested in
 *  the read. The queue id is invalid. Similar errors regarding the pipe
 *  will be caught by higher level code in the Software Bus.
 *
 */
#define CFE_SB_PIPE_RD_ERR ((CFE_Status_t)0xca000006)

/**
 * @brief Message Too Big
 *
 *  The size field in the message header indicates the message exceeds the
 *  max Software Bus message size. The max size is defined by
 *  configuration parameter #CFE_MISSION_SB_MAX_SB_MSG_SIZE in cfe_mission_cfg.h
 *
 */
#define CFE_SB_MSG_TOO_BIG ((CFE_Status_t)0xca000007)

/**
 * @brief Buffer Allocation Error
 *
 *  Returned when the memory in the SB message buffer pool has been depleted.
 *  The amount of memory in the pool is dictated by the configuration parameter
 *  #CFE_PLATFORM_SB_BUF_MEMORY_BYTES specified in the cfe_platform_cfg.h file. Also
 *  the memory statistics, including current utilization figures and high
 *  water marks for the SB Buffer memory pool can be monitored by sending
 *  a Software Bus command to send the SB statistics packet.
 *
 */
#define CFE_SB_BUF_ALOC_ERR ((CFE_Status_t)0xca000008)

/**
 * @brief Max Messages Met
 *
 *  Will be returned when calling one of the SB subscription API's if the
 *  SB routing table cannot accommodate another unique message ID because
 *  the platform configuration parameter #CFE_PLATFORM_SB_MAX_MSG_IDS has been met.
 *
 */
#define CFE_SB_MAX_MSGS_MET ((CFE_Status_t)0xca000009)

/**
 * @brief Max Destinations Met
 *
 *  Will be returned when calling one of the SB subscription API's if the
 *  SB routing table cannot accommodate another destination for a
 *  particular the given message ID. This occurs when the number of
 *  destinations in use meets the platform configuration parameter
 *  #CFE_PLATFORM_SB_MAX_DEST_PER_PKT.
 *
 */
#define CFE_SB_MAX_DESTS_MET ((CFE_Status_t)0xca00000a)

/**
 * @brief Internal Error
 *
 *  This error code will be returned by the #CFE_SB_Subscribe API if the
 *  code detects an internal index is out of range. The most likely
 *  cause would be a Single Event Upset.
 *
 */
#define CFE_SB_INTERNAL_ERR ((CFE_Status_t)0xca00000c)

/**
 * @brief Wrong Message Type
 *
 *  This error code will be returned when a request such as #CFE_MSG_SetMsgTime
 *  is made on a packet that does not include a field for msg time.
 *
 */
#define CFE_SB_WRONG_MSG_TYPE ((CFE_Status_t)0xca00000d)

/**
 * @brief Buffer Invalid
 *
 *  This error code will be returned when a request to release or send a
 *  zero copy buffer is invalid, such as if the handle or buffer is not
 *  correct or the buffer was previously released.
 *
 */
#define CFE_SB_BUFFER_INVALID ((CFE_Status_t)0xca00000e)

/**
 * @brief Not Implemented
 *
 *  Current version of cFE does not have the function or the feature
 *  of the function implemented.  This could be due to either an early
 *  build of the cFE for this platform or the platform does not support
 *  the specified feature.
 *
 */
#define CFE_SB_NOT_IMPLEMENTED ((CFE_Status_t)0xca00ffff)

/*
************* TABLE SERVICES STATUS CODES *************
*/

/**
 * @brief Invalid Handle
 *
 *  The calling Application attempted to pass a
 *  Table handle that represented too large an index or
 *  identified a Table Access Descriptor that was not used.
 *
 */
#define CFE_TBL_ERR_INVALID_HANDLE ((CFE_Status_t)0xcc000001)

/**
 * @brief Invalid Name
 *
 *  The calling Application attempted to register a table whose
 *  name length exceeded the platform configuration value of
 *  #CFE_MISSION_TBL_MAX_NAME_LENGTH or was zero characters long.
 *
 */
#define CFE_TBL_ERR_INVALID_NAME ((CFE_Status_t)0xcc000002)

/**
 * @brief Invalid Size
 *
 *  The calling Application attempted to register a table:
 *    a) that was a double buffered table with size greater than #CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE
 *    b) that was a single buffered table with size greater than #CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE
 *    c) that had a size of zero
 *
 */
#define CFE_TBL_ERR_INVALID_SIZE ((CFE_Status_t)0xcc000003)

/**
 * @brief Update Pending
 *
 *   The calling Application has identified a table that has a load pending.
 *
 */
#define CFE_TBL_INFO_UPDATE_PENDING ((CFE_Status_t)0x4c000004)

/**
 * @brief Never Loaded
 *
 *  Table has not been loaded with data.
 *
 */
#define CFE_TBL_ERR_NEVER_LOADED ((CFE_Status_t)0xcc000005)

/**
 * @brief Registry Full
 *
 *  An application attempted to create a table and the Table
 *  registry already contained #CFE_PLATFORM_TBL_MAX_NUM_TABLES in it.
 *
 */
#define CFE_TBL_ERR_REGISTRY_FULL ((CFE_Status_t)0xcc000006)

/**
 * @brief Duplicate Warning
 *
 *  This is an error that the registration is trying to replace
 *  an existing table with the same name.  The previous table
 *  stays in place and the new table is rejected.
 *
 */
#define CFE_TBL_WARN_DUPLICATE ((CFE_Status_t)0x4c000007)

/**
 * @brief No Access
 *
 *   The calling application either failed when calling #CFE_TBL_Register,
 *   failed when calling #CFE_TBL_Share or forgot to call either one.
 *
 */
#define CFE_TBL_ERR_NO_ACCESS ((CFE_Status_t)0xcc000008)

/**
 * @brief Unregistered
 *
 *  The calling application is trying to access a table that has
 *  been unregistered.
 *
 */
#define CFE_TBL_ERR_UNREGISTERED ((CFE_Status_t)0xcc000009)

/**
 * @brief Handles Full
 *
 *   An application attempted to create a table and the Table
 *   Handle Array already used all CFE_PLATFORM_TBL_MAX_NUM_HANDLES in it.
 *
 */
#define CFE_TBL_ERR_HANDLES_FULL ((CFE_Status_t)0xcc00000B)

/**
 * @brief Duplicate Table With Different Size
 *
 *  An application attempted to register a table with the same name
 *  as a table that is already in the registry.  The size of the new
 *  table is different from the size already in the registry.
 *
 */
#define CFE_TBL_ERR_DUPLICATE_DIFF_SIZE ((CFE_Status_t)0xcc00000C)

/**
 * @brief Duplicate Table And Not Owned
 *
 *  An application attempted to register a table with the same name
 *  as a table that is already in the registry.  The previously registered
 *  table is owned by a different application.
 *
 */
#define CFE_TBL_ERR_DUPLICATE_NOT_OWNED ((CFE_Status_t)0xcc00000D)

/**
 * @brief Updated
 *
 *  The calling Application has identified a table that has been updated.<BR>
 *  \b NOTE: This is a nominal return code informing the calling application
 *  that the table identified in the call has had its contents updated since
 *  the last time the application obtained its address or status.
 *
 */
#define CFE_TBL_INFO_UPDATED ((CFE_Status_t)0x4c00000E)

/**
 * @brief No Buffer Available
 *
 *  The calling Application has tried to allocate a working buffer but
 *  none were available.
 *
 */
#define CFE_TBL_ERR_NO_BUFFER_AVAIL ((CFE_Status_t)0xcc00000F)

/**
 * @brief Dump Only Error
 *
 *  The calling Application has attempted to perform a load on a
 *  table that was created with "Dump Only" attributes.
 *
 */
#define CFE_TBL_ERR_DUMP_ONLY ((CFE_Status_t)0xcc000010)

/**
 * @brief Illegal Source Type
 *
 *  The calling Application called #CFE_TBL_Load with an illegal
 *  value for the second parameter.
 *
 */
#define CFE_TBL_ERR_ILLEGAL_SRC_TYPE ((CFE_Status_t)0xcc000011)

/**
 * @brief Load In Progress
 *
 *  The calling Application called #CFE_TBL_Load when another Application
 *  was trying to load the table.
 *
 */
#define CFE_TBL_ERR_LOAD_IN_PROGRESS ((CFE_Status_t)0xcc000012)

/**
 * @brief File Too Large
 *
 *  The calling Application called #CFE_TBL_Load with a filename that specified a file
 *  that contained more data than the size of the table OR which contained more data
 *  than specified in the table header.
 *
 */
#define CFE_TBL_ERR_FILE_TOO_LARGE ((CFE_Status_t)0xcc000014)

/**
 * @brief Short File Warning
 *
 *  The calling Application called #CFE_TBL_Load with a filename that specified a file
 *  that started with the first byte of the table but contained less data than the size of the table.
 *  It should be noted that #CFE_TBL_WARN_PARTIAL_LOAD also indicates a partial load (one that starts
 *  at a non-zero offset).
 *
 */
#define CFE_TBL_WARN_SHORT_FILE ((CFE_Status_t)0x4c000015)

/**
 * @brief Bad Content ID
 *
 *  The calling Application called #CFE_TBL_Load with a filename that specified a file
 *  whose content ID was not that of a table image.
 *
 */
#define CFE_TBL_ERR_BAD_CONTENT_ID ((CFE_Status_t)0xcc000016)

/**
 * @brief No Update Pending
 *
 *  The calling Application has attempted to update a table without a pending load.
 *
 */
#define CFE_TBL_INFO_NO_UPDATE_PENDING ((CFE_Status_t)0x4c000017)

/**
 * @brief Table Locked
 *
 *  The calling Application tried to update a table that is locked by another user.
 *
 */
#define CFE_TBL_INFO_TABLE_LOCKED ((CFE_Status_t)0x4c000018)

/**
 * Validation Pending
 *
 *   The calling Application should call #CFE_TBL_Validate for the specified table.
 *
 */
#define CFE_TBL_INFO_VALIDATION_PENDING ((CFE_Status_t)0x4c000019)

/**
 * No Validation Pending
 *
 *  The calling Application tried to validate a table that did not have a validation request pending.
 *
 */
#define CFE_TBL_INFO_NO_VALIDATION_PENDING ((CFE_Status_t)0x4c00001A)

/**
 * @brief Bad Subtype ID
 *
 *  The calling Application tried to access a table file whose Subtype identifier indicated it was not
 *  a table image file.
 *
 */
#define CFE_TBL_ERR_BAD_SUBTYPE_ID ((CFE_Status_t)0xcc00001B)

/**
 * @brief File Size Inconsistent
 *
 *  The calling Application tried to access a table file whose Subtype identifier indicated it was not
 *  a table image file.
 *
 */
#define CFE_TBL_ERR_FILE_SIZE_INCONSISTENT ((CFE_Status_t)0xcc00001C)

/**
 * @brief No Standard Header
 *
 *  The calling Application tried to access a table file whose standard cFE File Header was the wrong size, etc.
 *
 */
#define CFE_TBL_ERR_NO_STD_HEADER ((CFE_Status_t)0xcc00001D)

/**
 * @brief No Table Header
 *
 *  The calling Application tried to access a table file whose standard cFE
 *  Table File Header was the wrong size, etc.
 *
 */
#define CFE_TBL_ERR_NO_TBL_HEADER ((CFE_Status_t)0xcc00001E)

/**
 * @brief Filename Too Long
 *
 *  The calling Application tried to load a table using a filename
 *  that was too long.
 *
 */
#define CFE_TBL_ERR_FILENAME_TOO_LONG ((CFE_Status_t)0xcc00001F)

/**
 * @brief File For Wrong Table
 *
 *  The calling Application tried to load a table using a file whose
 *  header indicated that it was for a different table.
 *
 */
#define CFE_TBL_ERR_FILE_FOR_WRONG_TABLE ((CFE_Status_t)0xcc000020)

/**
 * @brief Load Incomplete
 *
 *  The calling Application tried to load a table file whose header
 *  claimed the load was larger than what was actually read from the file.
 *
 */
#define CFE_TBL_ERR_LOAD_INCOMPLETE ((CFE_Status_t)0xcc000021)

/**
 * @brief Partial Load Warning
 *
 *  The calling Application tried to load a table file whose header
 *  claimed the load did not start with the first byte. It should be noted
 *  that #CFE_TBL_WARN_SHORT_FILE also indicates a partial load.
 *
 */
#define CFE_TBL_WARN_PARTIAL_LOAD ((CFE_Status_t)0x4c000022)

/**
 * @brief Partial Load Error
 *
 *  The calling Application tried to load a table file whose header
 *  claimed the load did not start with the first byte and the table
 *  image had NEVER been loaded before.  Partial loads are not allowed
 *  on uninitialized tables.  It should be noted that
 *  #CFE_TBL_WARN_SHORT_FILE also indicates a partial load.
 *
 */
#define CFE_TBL_ERR_PARTIAL_LOAD ((CFE_Status_t)0xcc000023)

/**
 * @brief Dump Pending
 *
 *  The calling Application should call #CFE_TBL_Manage for the specified table.
 *  The ground has requested a dump of the Dump-Only table and needs to synchronize
 *  with the owning application.
 *
 */
#define CFE_TBL_INFO_DUMP_PENDING ((CFE_Status_t)0x4c000024)

/**
 * @brief Invalid Options
 *
 *   The calling Application has used an illegal combination of table options.
 *   A summary of the illegal combinations are as follows:
 *   \par   #CFE_TBL_OPT_USR_DEF_ADDR cannot be combined with any of the following:
 *             -# #CFE_TBL_OPT_DBL_BUFFER
 *             -# #CFE_TBL_OPT_LOAD_DUMP
 *             -# #CFE_TBL_OPT_CRITICAL
 *   \par   #CFE_TBL_OPT_DBL_BUFFER cannot be combined with the following:
 *             -# #CFE_TBL_OPT_USR_DEF_ADDR
 *             -# #CFE_TBL_OPT_DUMP_ONLY
 *
 */
#define CFE_TBL_ERR_INVALID_OPTIONS ((CFE_Status_t)0xcc000025)

/**
 * @brief Not Critical Warning
 *
 *  The calling Application attempted to register a table as "Critical".
 *  Table Services failed to create an appropriate Critical Data Store
 *  (See System Log for reason) to save the table contents.  The table
 *  will be treated as a normal table from now on.
 *
 */
#define CFE_TBL_WARN_NOT_CRITICAL ((CFE_Status_t)0x4c000026)

/**
 * @brief Recovered Table
 *
 *  The calling Application registered a critical table whose previous
 *  contents were discovered in the Critical Data Store.  The discovered
 *  contents were copied back into the newly registered table as the
 *  table's initial contents.<BR>
 *  \b NOTE: In this situation, the contents of the table are \b NOT
 *  validated using the table's validation function.
 *
 */
#define CFE_TBL_INFO_RECOVERED_TBL ((CFE_Status_t)0x4c000027)

/**
 * @brief Bad Spacecraft ID
 *
 *  The selected table file failed validation for Spacecraft ID.
 *  The platform configuration file has verification of table files
 *  enabled for Spacecraft ID and an attempt was made to load a table
 *  with an invalid Spacecraft ID in the table file header.
 *
 */
#define CFE_TBL_ERR_BAD_SPACECRAFT_ID ((CFE_Status_t)0xcc000028)

/**
 * @brief Bad Processor ID
 *
 *  The selected table file failed validation for Processor ID.
 *  The platform configuration file has verification of table files
 *  enabled for Processor ID and an attempt was made to load a table
 *  with an invalid Processor ID in the table file header.
 *
 */
#define CFE_TBL_ERR_BAD_PROCESSOR_ID ((CFE_Status_t)0xcc000029)

/**
 * @brief Message Error
 *
 *  Error code indicating that the TBL command was not processed
 *  successfully and that the error counter should be incremented.
 */
#define CFE_TBL_MESSAGE_ERROR ((CFE_Status_t)0xcc00002a)

/**
**  Error code indicating that the TBL file is shorter than
**  indicated in the file header.
*/
#define CFE_TBL_ERR_SHORT_FILE ((CFE_Status_t)0xcc00002b)

/**
**  Error code indicating that the TBL file could not be
**  opened by the OS.
*/
#define CFE_TBL_ERR_ACCESS ((CFE_Status_t)0xcc00002c)

/**
 * @brief Bad Argument
 *
 *  A parameter given by a caller to a Table API did not pass
 *  validation checks.
 *
 */
#define CFE_TBL_BAD_ARGUMENT ((CFE_Status_t)0xcc00002d)

/**
 * @brief Not Implemented
 *
 *  Current version of cFE does not have the function or the feature
 *  of the function implemented.  This could be due to either an early
 *  build of the cFE for this platform or the platform does not support
 *  the specified feature.
 *
 */
#define CFE_TBL_NOT_IMPLEMENTED ((CFE_Status_t)0xcc00ffff)

/*
************* TIME SERVICES STATUS CODES *************
*/

/**
 * @brief Not Implemented
 *
 *  Current version of cFE does not have the function or the feature
 *  of the function implemented.  This could be due to either an early
 *  build of the cFE for this platform or the platform does not support
 *  the specified feature.
 *
 */
#define CFE_TIME_NOT_IMPLEMENTED ((CFE_Status_t)0xce00ffff)

/**
 * @brief Internal Only
 *
 *  One of the TIME Services API functions to set the time with data
 *  from an external time source has been called, but TIME Services
 *  has been commanded to not accept external time data.  However,
 *  the command is still a signal for the Time Server to generate
 *  a "time at the tone" command packet using internal data.
 *
 */
#define CFE_TIME_INTERNAL_ONLY ((CFE_Status_t)0xce000001)

/**
 * @brief Out Of Range
 *
 *  One of the TIME Services API functions to set the time with data
 *  from an external time source has been called, but TIME Services
 *  has determined that the new time data is invalid.  However,
 *  the command is still a signal for the Time Server to generate
 *  a "time at the tone" command packet using internal data.
 *
 * Note that the test for invalid time update data only occurs if TIME
 *        Services has previously been commanded to set the clock state
 *        to "valid".
 */
#define CFE_TIME_OUT_OF_RANGE ((CFE_Status_t)0xce000002)

/**
 * @brief Too Many Sync Callbacks
 *
 *  An attempt to register too many cFE Time Services Synchronization
 *  callbacks has been made.  Only one callback function is allowed per
 *  application.  It is expected that the application itself will
 *  distribute the single callback to child threads as needed.
 *
 */
#define CFE_TIME_TOO_MANY_SYNCH_CALLBACKS ((CFE_Status_t)0xce000003)

/**
 * @brief Callback Not Registered
 *
 *  An attempt to unregister a cFE Time Services Synchronization
 *  callback has failed because the specified callback function was not
 *  located in the Synchronization Callback Registry.
 *
 */
#define CFE_TIME_CALLBACK_NOT_REGISTERED ((CFE_Status_t)0xce000004)

/**
 * @brief Bad Argument
 *
 *  A parameter given by a caller to a TIME Services API did not pass
 *  validation checks.
 *
 */
#define CFE_TIME_BAD_ARGUMENT ((CFE_Status_t)0xce000005)
/**@}*/

#endif /* CFE_ERROR_H */
