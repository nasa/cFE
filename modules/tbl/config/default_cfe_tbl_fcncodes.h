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
 *   Specification for the CFE Event Services (CFE_TBL) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CFE_TBL_FCNCODES_H
#define CFE_TBL_FCNCODES_H

/*
** Table task command packet command codes
*/
/** \name Table Services Command Codes */
/** \{ */

/** \cfetblcmd Table No-Op
**
**  \par Description
**       This command performs no other function than to increment the
**       command execution counter. The command may be used to verify
**       general aliveness of the Table Services task.
**
**  \cfecmdmnemonic \TBL_NOOP
**
**  \par Command Structure
**       #CFE_TBL_NoopCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will
**         increment
**       - The #CFE_TBL_NOOP_INF_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Table
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_TBL_NOOP_CC 0

/** \cfetblcmd Table Reset Counters
**
**  \par Description
**       This command resets the following counters within the Table
**       Services housekeeping telemetry:
**       - Command Execution Counter (\TBL_CMDPC)
**       - Command Error Counter (\TBL_CMDEC)
**       - Successful Table Validations Counter (\TBL_VALSUCCESSCTR)
**       - Failed Table Validations Counter (\TBL_VALFAILEDCTR)
**       - Number of Table Validations Requested (\TBL_VALREQCTR)
**       - Number of completed table validations (\TBL_VALCOMPLTDCTR)
**
**  \cfecmdmnemonic \TBL_RESETCTRS
**
**  \par Command Structure
**       #CFE_TBL_ResetCountersCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will
**         be reset to 0
**       - The #CFE_TBL_RESET_INF_EID debug event message will be
**         generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Table
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is
**       possible for ground systems and on-board safing procedures
**       to be designed such that they react to changes in the counter
**       values that are reset by this command.
**
**  \sa
*/
#define CFE_TBL_RESET_COUNTERS_CC 1

/** \cfetblcmd Load Table
**
**  \par Description
**       This command loads the contents of the specified file into
**       an inactive buffer for the table specified within the file.
**
**  \cfecmdmnemonic \TBL_LOAD
**
**  \par Command Structure
**       #CFE_TBL_LoadCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will
**         increment
**       - The #CFE_TBL_FILE_LOADED_INF_EID informational event message
**         will be generated
**
**  \par Error Conditions
**       This command can fail for the following reasons:
**       - Table name found in table image file's table header is not found
**         in table registry (ie - The table associated with the table image
**         in the file has not been registered by an application).
**       - The table image file has an invalid or incorrect size.  The size of
**         the image file must match the size field within in the header, and
**         must also match the expected size of the table indicated in the registry.
**       - No working buffers are available for the load.  This would indicate
**         that too many single-buffered table loads are in progress at the same
**         time.
**       - An attempt is being made to load an uninitialized table with a file
**         containing only a partial table image.
**       - The table image file was unable to be opened.  Either the file does
**         not exist at the specified location, the filename is in error, or
**         the file system has been corrupted.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Command specific error event messages are issued for all error cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It is performing the first
**       step of loading a table and can be aborted (using the Abort Table Load
**       command described below) without affecting the contents of the active
**       table image.
**
**  \sa  #CFE_TBL_DUMP_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ACTIVATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_LOAD_CC 2

/** \cfetblcmd Dump Table
**
**  \par Description
**       This command will cause the Table Services to put the contents
**       of the specified table buffer into the command specified file.
**
**  \cfecmdmnemonic \TBL_DUMP
**
**  \par Command Structure
**       #CFE_TBL_DumpCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will
**         increment
**       - Either the #CFE_TBL_OVERWRITE_DUMP_INF_EID OR the
**         #CFE_TBL_WRITE_DUMP_INF_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A single buffered table's inactive buffer was requested to be
**         dumped and no such buffer is currently allocated.
**       - Error occurred during write operation to file. Possible causes
**         might be insufficient space in the file system or the filename
**         or file path is improperly specified.
**       - The specified table name was not found in the table registry.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - A command specific error event message is issued for all error
**         cases
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system and could, if performed repeatedly without
**       sufficient file management by the operator, fill the file system.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ACTIVATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_DUMP_CC 3

/** \cfetblcmd Validate Table
**
**  \par Description
**       This command will cause Table Services to calculate the Data Integrity
**       Value for the specified table and to notify the owning application that
**       the table's validation function should be executed.  The results of both
**       the Data Integrity Value computation and the validation function are
**       reported in Table Services Housekeeping Telemetry.
**
**  \cfecmdmnemonic \TBL_VALIDATE
**
**  \par Command Structure
**       #CFE_TBL_ValidateCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the following
**       telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will increment
**       - \b \c \TBL_VALREQCTR - table validation request counter will increment
**       - \b \c \TBL_LASTVALCRC - calculated data integrity value will be updated
**       - The #CFE_TBL_VAL_REQ_MADE_INF_EID debug event message (indicating the
**         application is being notified of a validation request)
**
**       If the specified table has an associated validation function, then the
**       following telemetry will also change:
**       - Either \b \c \TBL_VALSUCCESSCTR OR \b \c \TBL_VALFAILEDCTR will
**         increment
**       - \b \c \TBL_VALCOMPLTDCTR - table validations performed counter will
**         increment
**       - \b \c \TBL_LASTVALS - table validation function return status will
**         update
**       - The #CFE_TBL_VALIDATION_INF_EID informational event message (indicating the
**         validation function return status) will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A single buffered table's inactive buffer was requested to be validated
**         and no such buffer is currently allocated.
**       - Too many validations have been requested simultaneously.  The operator
**         must wait for one or more applications to perform their table validation
**         functions before trying again.
**       - The specified table name was not found in the table registry.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Command specific error event message are issued for all error cases
**
**  \par Criticality
**       The success or failure of a table validation does not have any immediate
**       impact on table contents.  The results are sent to the operator in telemetry
**       and the operator must determine whether the results are acceptable and send a
**       command to activate the validated table image.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_DUMP_CC, #CFE_TBL_ACTIVATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_VALIDATE_CC 4

/** \cfetblcmd Activate Table
**
**  \par Description
**       This command will cause Table Services to notify a table's owner
**       that an update is pending.  The owning application will then update
**       the contents of the active table buffer with the contents of the
**       associated inactive table buffer at a time of their convenience.
**
**  \cfecmdmnemonic \TBL_ACTIVATE
**
**  \par Command Structure
**       #CFE_TBL_ActivateCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will
**         increment
**       - The #CFE_TBL_UPDATE_SUCCESS_INF_EID informational event message
**         will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified table name was not found in the table registry.
**       - The table was registered as a "dump only" type and thus cannot be activated
**       - The table buffer has not been validated.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Command specific error event message are issued for all
**         error cases
**
**  \par Criticality
**       This command will cause the contents of the specified table to be
**       updated with the contents in the inactive table buffer.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_DUMP_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ABORT_LOAD_CC
*/
#define CFE_TBL_ACTIVATE_CC 5

/** \cfetblcmd Dump Table Registry
**
**  \par Description
**       This command will cause Table Services to write some of the
**       contents of the Table Registry to the command specified file.
**       This allows the operator to see the current state and configuration
**       of all tables that have been registered with the cFE.
**
**  \cfecmdmnemonic \TBL_WRITEREG2FILE
**
**  \par Command Structure
**       #CFE_TBL_DumpRegistryCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will
**         increment
**       - The generation of either #CFE_TBL_OVERWRITE_REG_DUMP_INF_EID
**         or #CFE_TBL_WRITE_REG_DUMP_INF_EID debug event messages
**       - The specified file should appear (or be updated) at the
**         specified location in the file system
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - A table registry dump is already in progress, not yet completed
**       - The specified DumpFilename could not be parsed
**       - Error occurred during write operation to file. Possible
**         causes might be insufficient space in the file system
**         or the filename or file path is improperly specified.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       This command is not inherently dangerous.  It will create a new
**       file in the file system and could, if performed repeatedly without
**       sufficient file management by the operator, fill the file system.
**
**  \sa  #CFE_TBL_SEND_REGISTRY_CC
*/
#define CFE_TBL_DUMP_REGISTRY_CC 6

/** \cfetblcmd Telemeter One Table Registry Entry
**
**  \par Description
**       This command will cause Table Services to telemeter the contents
**       of the Table Registry for the command specified table.
**
**  \cfecmdmnemonic \TBL_TLMREG
**
**  \par Command Structure
**       #CFE_TBL_SendRegistryCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will increment
**       - Receipt of a Table Registry Info Packet (see #CFE_TBL_TableRegistryTlm_t)
**       - The #CFE_TBL_TLM_REG_CMD_INF_EID debug event message will
**         be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified table name was not found in the table registry.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Error specific event message
**
**  \par Criticality
**       This command is not inherently dangerous.  It will generate
**       additional telemetry.
**
**  \sa  #CFE_TBL_DUMP_REGISTRY_CC
*/
#define CFE_TBL_SEND_REGISTRY_CC 7

/** \cfetblcmd Delete Critical Table from Critical Data Store
**
**  \par Description
**       This command will delete the Critical Data Store (CDS) associated
**       with the specified Critical Table.  Note that any table still
**       present in the Table Registry is unable to be deleted from
**       the Critical Data Store.  All Applications that are accessing
**       the critical table must release and unregister their access
**       before the CDS can be deleted.
**
**  \cfecmdmnemonic \TBL_DELETECDS
**
**  \par Command Structure
**       #CFE_TBL_DeleteCDSCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TBL_CMDPC - command execution counter will
**         increment
**       - The #CFE_TBL_CDS_DELETED_INFO_EID informational event message
**         will be generated
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified table name was not found in the critical data
**         store registry
**       - The specified table name WAS found in the table registry (all
**         registrations/sharing of the table must be unregistered before
**         the table's CDS can be deleted)
**       - The table's owning application is still active
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Error specific event message
**
**  \par Criticality
**       This command will cause the loss of the specified table's contents
**       before the owning Application was terminated.
**
**  \sa  #CFE_ES_DUMP_CDS_REGISTRY_CC, #CFE_ES_DELETE_CDS_CC
*/
#define CFE_TBL_DELETE_CDS_CC 8

/** \cfetblcmd Abort Table Load
**
**  \par Description
**       This command will cause Table Services to discard the contents of
**       a table buffer that was previously loaded with the data in a file
**       as specified by a Table Load command.  For single buffered tables,
**       the allocated shared working buffer is freed and becomes available
**       for other Table Load commands.
**
**  \cfecmdmnemonic \TBL_LOADABORT
**
**  \par Command Structure
**       #CFE_TBL_AbortLoadCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \TBL_CMDPC -  command execution counter will increment
**       - The #CFE_TBL_LOAD_ABORT_INF_EID informational event message
**         is generated
**       - If the load was aborted for a single buffered table, the
**         \b \c \TBL_NUMFREESHRBUF telemetry point should increment
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified table name was not found in the table registry.
**       - The specified table did not have a load in progress to be aborted.
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \TBL_CMDEC - command error counter will increment
**       - Error specific event message
**
**  \par Criticality
**       This command will cause the loss of data put into an inactive table buffer.
**
**  \sa  #CFE_TBL_LOAD_CC, #CFE_TBL_DUMP_CC, #CFE_TBL_VALIDATE_CC, #CFE_TBL_ACTIVATE_CC
*/
#define CFE_TBL_ABORT_LOAD_CC 9
/** \} */

#endif
