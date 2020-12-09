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

/*
** File: cfe_tbl_msg.h
**
** Purpose:  cFE Table Services (TBL) SB message definitions header file
**
** Author:   D.Kobe/Hammers
**
** Notes:
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_tbl_msg_
#define _cfe_tbl_msg_

/*
** Required header files...
*/
#include "cfe.h"

/*************************************************************************/

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
#define CFE_TBL_NOOP_CC         0    

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
**         increment
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
#define CFE_TBL_RESET_COUNTERS_CC        1

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
**       - The table image file's header indicates the file contains 'x' 
**         number of bytes of data but the file contains less.
**       - No working buffers are available for the load.  This would indicate 
**         that too many single-buffered table loads are in progress at the same 
**         time.
**       - The table image file's header indicates the data to be loaded is 
**         beyond the size of the table.  Either the number of bytes in the 
**         file are too many or the starting offset into the table is too high.
**       - The table image file's header indicates there is no data in the 
**         file (ie - Number of bytes to load is zero).
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
#define CFE_TBL_LOAD_CC         2

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
#define CFE_TBL_DUMP_CC         3

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
**       - A single buffered table's inactive buffer was requested to be dumped 
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
#define CFE_TBL_VALIDATE_CC     4

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
**       - A single buffered table's inactive buffer was requested to be 
**         dumped and no such buffer is currently allocated.
**       - The specified table name was not found in the table registry.
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
#define CFE_TBL_ACTIVATE_CC     5

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
#define CFE_TBL_DUMP_REGISTRY_CC     6

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
#define CFE_TBL_SEND_REGISTRY_CC      7

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
#define CFE_TBL_DELETE_CDS_CC   8

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
#define CFE_TBL_ABORT_LOAD_CC   9
/** \} */

/*************************************************************************/

/********************************/
/* Command Message Data Formats */
/********************************/
/**
** \brief Generic "no arguments" command
**
** This command structure is used for commands that do not have any parameters.
** This includes:
** -# The Housekeeping Request Message
** -# The No-Op Command (For details, see #CFE_TBL_NOOP_CC)
** -# The Reset Counters Command (For details, see #CFE_TBL_RESET_COUNTERS_CC)
*/
typedef struct CFE_TBL_NoArgsCmd
{
    CFE_MSG_CommandHeader_t CmdHeader; /**< \brief Command header */
} CFE_TBL_NoArgsCmd_t;

/*
 * Typedefs for each of the messages that share the command format
 * Allows each command to evolve independently, while following the command
 * handler prototype convention
 */
typedef CFE_TBL_NoArgsCmd_t CFE_TBL_NoopCmd_t;
typedef CFE_TBL_NoArgsCmd_t CFE_TBL_ResetCountersCmd_t;

/**
** \brief Load Table Command Payload
**
** For command details, see #CFE_TBL_LOAD_CC
**
**/
typedef struct CFE_TBL_LoadCmd_Payload
{
    char LoadFilename[CFE_MISSION_MAX_PATH_LEN];  /**< \brief Filename (and path) of data to be loaded */
} CFE_TBL_LoadCmd_Payload_t;

/**
 * \brief Load Table Command
 */
typedef struct CFE_TBL_LoadCmd
{
    CFE_MSG_CommandHeader_t   CmdHeader; /**< \brief Command header */
    CFE_TBL_LoadCmd_Payload_t Payload;   /**< \brief Command payload */
} CFE_TBL_LoadCmd_t;

/**
** \brief Dump Table Command Payload
**
** For command details, see #CFE_TBL_DUMP_CC
*/
typedef struct CFE_TBL_DumpCmd_Payload
{
    uint16                ActiveTableFlag;                        /**< \brief #CFE_TBL_BufferSelect_INACTIVE=Inactive Table, 
                                                                            #CFE_TBL_BufferSelect_ACTIVE=Active Table */
                                                                /**< Selects either the "Inactive" 
                                                                     (#CFE_TBL_BufferSelect_INACTIVE) buffer or the 
                                                                     "Active" (#CFE_TBL_BufferSelect_ACTIVE) buffer 
                                                                     to be dumped */
    char                  TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full name of table to be dumped */
                                                                /**< ASCII string containing full table name 
                                                                     identifier of table to be dumped */
    char                  DumpFilename[CFE_MISSION_MAX_PATH_LEN];        /**< \brief Full Filename where data is to be written */
                                                                /**< ASCII string containing full path of filename 
                                                                     where data is to be dumped */
} CFE_TBL_DumpCmd_Payload_t;

/**
 * /brief Dump Table Command
 */
typedef struct CFE_TBL_DumpCmd
{
    CFE_MSG_CommandHeader_t   CmdHeader; /**< \brief Command header */
    CFE_TBL_DumpCmd_Payload_t Payload;   /**< \brief Command payload */
} CFE_TBL_DumpCmd_t;

/**
** \brief Validate Table Command Payload
**
** For command details, see #CFE_TBL_VALIDATE_CC
*/
typedef struct CFE_TBL_ValidateCmd_Payload
{
    uint16                ActiveTableFlag;                         /**< \brief #CFE_TBL_BufferSelect_INACTIVE=Inactive Table, 
                                                                             #CFE_TBL_BufferSelect_ACTIVE=Active Table */
                                                                 /**< Selects either the "Inactive" 
                                                                      (#CFE_TBL_BufferSelect_INACTIVE) buffer or the 
                                                                      "Active" (#CFE_TBL_BufferSelect_ACTIVE) buffer 
                                                                      to be validated */
    char                  TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table to be validated */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of table to be validated */
} CFE_TBL_ValidateCmd_Payload_t;

/**
 * \brief Validate Table Command
 */
typedef struct CFE_TBL_ValidateCmd
{
    CFE_MSG_CommandHeader_t       CmdHeader; /**< \brief Command header */
    CFE_TBL_ValidateCmd_Payload_t Payload;   /**< \brief Command payload */
} CFE_TBL_ValidateCmd_t;

/**
** \brief Activate Table Command Payload
**
** For command details, see #CFE_TBL_ACTIVATE_CC
*/
typedef struct CFE_TBL_ActivateCmd_Payload
{
    char                  TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table to be activated */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of table to be activated */
} CFE_TBL_ActivateCmd_Payload_t;

/**
 * \brief Activate Table Command
 */
typedef struct CFE_TBL_ActivateCmd
{
    CFE_MSG_CommandHeader_t       CmdHeader; /**< \brief Command header */
    CFE_TBL_ActivateCmd_Payload_t Payload;   /**< \brief Command paylod */
} CFE_TBL_ActivateCmd_t;

/**
** \brief Dump Registry Command Payload
**
** For command details, see #CFE_TBL_DUMP_REGISTRY_CC
*/
typedef struct CFE_TBL_DumpRegistryCmd_Payload
{
    char                  DumpFilename[CFE_MISSION_MAX_PATH_LEN];        /**< \brief Full Filename where dumped data is to be 
                                                                     written */
                                                                /**< ASCII string containing full path of filename 
                                                                     where registry is to be dumped */
} CFE_TBL_DumpRegistryCmd_Payload_t;

/**
 * \brief Dump Registry Command
 */
typedef struct CFE_TBL_DumpRegistryCmd
{
    CFE_MSG_CommandHeader_t           CmdHeader; /**< \brief Command header */
    CFE_TBL_DumpRegistryCmd_Payload_t Payload;   /**< \brief Command payload */
} CFE_TBL_DumpRegistryCmd_t;

/**
** \brief Send Table Registry Command Payload
**
** For command details, see #CFE_TBL_SEND_REGISTRY_CC
*/
typedef struct CFE_TBL_SendRegistryCmd_Payload
{
    char                  TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table whose registry entry
                                                                      is to be telemetered */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of table whose registry entry is
                                                                      to be telemetered via #CFE_TBL_TableRegistryTlm_t */
} CFE_TBL_SendRegistryCmd_Payload_t;

/**
 * \brief Send Table Registry Command
 */
typedef struct CFE_TBL_SendRegistryCmd
{
    CFE_MSG_CommandHeader_t           CmdHeader; /**< \brief Command header */
    CFE_TBL_SendRegistryCmd_Payload_t Payload;   /**< \brief Command payload */
} CFE_TBL_SendRegistryCmd_t;

/**
** \brief Delete Critical Table CDS Command Payload
**
** For command details, see #CFE_TBL_DELETE_CDS_CC
*/
typedef struct CFE_TBL_DelCDSCmd_Payload
{
    char                  TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table whose CDS is
                                                                      to be deleted */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of a critical table whose 
                                                                      CDS is to be deleted */
} CFE_TBL_DelCDSCmd_Payload_t;

/**
 * \brief Delete Critical Table CDS Command
 */
typedef struct CFE_TBL_DeleteCDSCmd
{
    CFE_MSG_CommandHeader_t     CmdHeader; /**< \brief Command header */
    CFE_TBL_DelCDSCmd_Payload_t Payload;   /**< \brief Command payload */
} CFE_TBL_DeleteCDSCmd_t;

/**
** \brief Abort Load Command Payload
**
** For command details, see #CFE_TBL_ABORT_LOAD_CC
*/
typedef struct CFE_TBL_AbortLoadCmd_Payload
{
    char                  TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];  /**< \brief Full Name of Table whose load is to be aborted */
                                                                 /**< ASCII string containing full table name 
                                                                      identifier of a table whose load is to be aborted */
} CFE_TBL_AbortLoadCmd_Payload_t;

/**
 * \brief Abort Load Command
 */
typedef struct CFE_TBL_AbortLoadCmd
{
    CFE_MSG_CommandHeader_t        CmdHeader; /**< \brief Command header */
    CFE_TBL_AbortLoadCmd_Payload_t Payload;   /**< \brief Command paylod */
} CFE_TBL_AbortLoadCmd_t;


/*************************************************************************/
/******************************************/
/* Generated Command Message Data Formats */
/******************************************/
/**
** \brief Table Management Notification Command Payload
**
** \par Description
**      Whenever an application that owns a table calls the #CFE_TBL_NotifyByMessage API
**      following the table registration, Table services will generate the following
**      command message with the application specified message ID, command code and
**      parameter whenever the table requires management (e.g. - loads and validations).
*/
typedef struct CFE_TBL_NotifyCmd_Payload
{
    uint32                Parameter;                             /**< \brief Application specified command parameter */
} CFE_TBL_NotifyCmd_Payload_t;

/**
 * /brief Table Management Notification Command
 */
typedef struct CFE_TBL_NotifyCmd
{
    CFE_MSG_CommandHeader_t     CmdHeader; /**< \brief Command header */
    CFE_TBL_NotifyCmd_Payload_t Payload;   /**< \brief Command payload */
} CFE_TBL_NotifyCmd_t;

/*************************************************************************/
/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
/** 
**  \cfetbltlm Table Services Housekeeping Packet
**/
typedef struct CFE_TBL_HousekeepingTlm_Payload
{
    /*
    ** Task command interface counters...
    */
    uint8                 CommandCounter;                           /**< \cfetlmmnemonic \TBL_CMDPC 
                                                                     \brief Count of valid commands received */
    uint8                 CommandErrorCounter;                           /**< \cfetlmmnemonic \TBL_CMDEC 
                                                                     \brief Count of invalid commands received */

    /*
    ** Table Registry Statistics
    */
    uint16                NumTables;                            /**< \cfetlmmnemonic \TBL_NUMTABLES 
                                                                     \brief Number of Tables Registered */
    uint16                NumLoadPending;                       /**< \cfetlmmnemonic \TBL_NUMUPDATESPEND 
                                                                     \brief Number of Tables pending on Applications for their update */

    /*
    ** Last Table Validation Results
    */
    uint16                ValidationCounter;                        /**< \cfetlmmnemonic \TBL_VALCOMPLTDCTR 
                                                                     \brief Number of completed table validations */
    uint32                LastValCrc;                           /**< \cfetlmmnemonic \TBL_LASTVALCRC 
                                                                     \brief Data Integrity Value computed for last table validated */
    int32                 LastValStatus;                        /**< \cfetlmmnemonic \TBL_LASTVALS 
                                                                     \brief Returned status from validation function for last table validated */
    bool                  ActiveBuffer;                         /**< \cfetlmmnemonic \TBL_LASTVALBUF 
                                                                     \brief Indicator of whether table buffer validated was 0=Inactive, 1=Active */ 
    char                  LastValTableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTVALTBLNAME
                                                                            \brief Name of last table validated */
    uint8                 SuccessValCounter;                        /**< \cfetlmmnemonic \TBL_VALSUCCESSCTR 
                                                                     \brief Total number of successful table validations */
    uint8                 FailedValCounter;                         /**< \cfetlmmnemonic \TBL_VALFAILEDCTR 
                                                                     \brief Total number of unsuccessful table validations */
    uint8                 NumValRequests;                       /**< \cfetlmmnemonic \TBL_VALREQCTR 
                                                                     \brief Number of times Table Services has requested validations from Apps */
    
    /*
    ** Ground system interface information
    */
    uint8                 NumFreeSharedBufs;                    /**< \cfetlmmnemonic \TBL_NUMFREESHRBUF 
                                                                     \brief Number of free Shared Working Buffers */
    uint8                 ByteAlignPad1;                        /**< \cfetlmmnemonic \TBL_BYTEALIGNPAD1 
                                                                     \brief Spare byte to ensure longword alignment */
    CFE_ES_MemHandle_t    MemPoolHandle;                        /**< \cfetlmmnemonic \TBL_MEMPOOLHANDLE 
                                                                     \brief Handle to TBL's memory pool */
    CFE_TIME_SysTime_t    LastUpdateTime;                       /**< \cfetlmmnemonic \TBL_LASTUPDTIME 
                                                                     \brief Time of last table update */
    char                  LastUpdatedTable[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTUPDTBLNAME
                                                                          \brief Name of the last table updated */
    char                  LastFileLoaded[CFE_MISSION_MAX_PATH_LEN];      /**< \cfetlmmnemonic \TBL_LASTFILELOADED 
                                                                     \brief Path and Name of last table image file loaded */
    char                  LastFileDumped[CFE_MISSION_MAX_PATH_LEN];      /**< \cfetlmmnemonic \TBL_LASTFILEDUMPED 
                                                                     \brief Path and Name of last file dumped to */
    char                  LastTableLoaded[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTTABLELOADED
                                                                          \brief Name of the last table loaded */
} CFE_TBL_HousekeepingTlm_Payload_t;

typedef struct CFE_TBL_HousekeepingTlm
{
    CFE_MSG_TelemetryHeader_t         TlmHeader; /**< \brief Telemetry header */
    CFE_TBL_HousekeepingTlm_Payload_t Payload;   /**< \brief Telemetry payload */
} CFE_TBL_HousekeepingTlm_t;


/** 
**  \cfetbltlm Table Registry Info Packet
**/
typedef struct CFE_TBL_TblRegPacket_Payload
{
    CFE_ES_MemOffset_t          Size;                           /**< \cfetlmmnemonic \TBL_SIZE
                                                                     \brief Size, in bytes, of Table */
    uint32                      Crc;                            /**< \cfetlmmnemonic \TBL_CRC 
                                                                     \brief Most recently calculated CRC of Table */
    CFE_ES_MemAddress_t         ActiveBufferAddr;               /**< \cfetlmmnemonic \TBL_ACTBUFADD
                                                                     \brief Address of Active Buffer */
    CFE_ES_MemAddress_t         InactiveBufferAddr;             /**< \cfetlmmnemonic \TBL_IACTBUFADD
                                                                     \brief Address of Inactive Buffer */
    CFE_ES_MemAddress_t         ValidationFuncPtr;              /**< \cfetlmmnemonic \TBL_VALFUNCPTR
                                                                     \brief Ptr to Owner App's function that validates tbl contents */
    CFE_TIME_SysTime_t          TimeOfLastUpdate;               /**< \cfetlmmnemonic \TBL_TIMELASTUPD 
                                                                     \brief Time when Table was last updated */
    uint32                      FileCreateTimeSecs;             /**< \cfetlmmnemonic \TBL_FILECSECONDS 
                                                                     \brief File creation time from last file loaded into table */
    uint32                      FileCreateTimeSubSecs;          /**< \cfetlmmnemonic \TBL_FILECSUBSECONDS 
                                                                     \brief File creation time from last file loaded into table */
    bool                        TableLoadedOnce;                /**< \cfetlmmnemonic \TBL_LOADEDONCE 
                                                                     \brief Flag indicating whether table has been loaded once or not */
    bool                        LoadPending;                    /**< \cfetlmmnemonic \TBL_UPDATEPNDNG 
                                                                     \brief Flag indicating an inactive buffer is ready to be copied */
    bool                        DumpOnly;                       /**< \cfetlmmnemonic \TBL_DUMPONLY 
                                                                     \brief Flag indicating Table is NOT to be loaded */
    bool                        DoubleBuffered;                    /**< \cfetlmmnemonic \TBL_DBLBUFFERED 
                                                                     \brief Flag indicating Table has a dedicated inactive buffer */
    char                        Name[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];/**< \cfetlmmnemonic \TBL_NAME
                                                                     \brief Processor specific table name */
    char                        LastFileLoaded[CFE_MISSION_MAX_PATH_LEN];/**< \cfetlmmnemonic \TBL_LASTFILEUPD 
                                                                     \brief Filename of last file loaded into table */
    char                        OwnerAppName[CFE_MISSION_MAX_API_LEN];  /**< \cfetlmmnemonic \TBL_OWNERAPP 
                                                                     \brief Name of owning application */
    bool                        Critical;                       /**< \cfetlmmnemonic \TBL_CRITICAL
                                                                     \brief Indicates whether table is Critical or not */
    uint8                       ByteAlign4;                     /**< \cfetlmmnemonic \TBL_SPARE4
                                                                     \brief Spare byte to maintain byte alignment */
} CFE_TBL_TblRegPacket_Payload_t;

typedef struct CFE_TBL_TableRegistryTlm
{
    CFE_MSG_TelemetryHeader_t      TlmHeader; /**< \brief Telemetry header */
    CFE_TBL_TblRegPacket_Payload_t Payload;   /**< \brief Telemetry payload */
} CFE_TBL_TableRegistryTlm_t;


#endif /* _cfe_tbl_msg_ */

/************************/
/*  End of File Comment */
/************************/
