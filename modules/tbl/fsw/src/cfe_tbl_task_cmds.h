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
 * Subsystem: cFE TBL Task Command Handler Interface Definition File
 *
 * Author: David Kobe (the Hammers Company, Inc.)
 *
 * Notes:
 *
 */

#ifndef CFE_TBL_TASK_CMDS_H
#define CFE_TBL_TASK_CMDS_H

/*
** Required header files
*/
#include "common_types.h"
#include "cfe_tbl_task.h"
#include "cfe_tbl_msg.h"
#include "cfe_error.h"

#include "cfe_sb_extern_typedefs.h"

/*********************  Macro and Constant Type Definitions   ***************************/

/*
 * For backward compatibility, keep this enumeration for now but map the
 * values to the globally-defined codes in cfe_error.h, so it won't be confusing
 * if intermixed with a typical CFE int32 return code.
 */
typedef enum
{
    CFE_TBL_INC_ERR_CTR =
        CFE_TBL_MESSAGE_ERROR, /**< Error detected in (or while processing) message, increment command error counter */
    CFE_TBL_DONT_INC_CTR =
        CFE_STATUS_NO_COUNTER_INCREMENT, /**< No errors detected but don't increment command counter */
    CFE_TBL_INC_CMD_CTR = CFE_SUCCESS    /**< No errors detected and increment command counter */
} CFE_TBL_CmdProcRet_t;

typedef int32 (*CFE_TBL_MsgProcFuncPtr_t)(const void *MsgPtr);

#define CFE_TBL_BAD_CMD_CODE (-1) /**< Command Code found in Message does not match any in #CFE_TBL_CmdHandlerTbl */
#define CFE_TBL_BAD_MSG_ID   (-2) /**< Message ID found in Message does not match any in #CFE_TBL_CmdHandlerTbl */

/*
** Table task const data
*/

typedef enum
{
    CFE_TBL_TERM_MSGTYPE = 0, /**< \brief Command Handler Table Terminator Type */
    CFE_TBL_MSG_MSGTYPE,      /**< \brief Message Type (requires Message ID match) */
    CFE_TBL_CMD_MSGTYPE       /**< \brief Command Type (requires Message ID and Command Code match) */
} CFE_TBL_MsgType_t;

/* Command Message Processing Functions */
/*****************************************************************************/

/*---------------------------------------------------------------------------------------*/
/**
** \brief Gathers data and puts it into the Housekeeping Message format
**
** \par Description
**        Gathers data from the Table Services Application, computes necessary data values and identifies
**        what Table Validation information needs to be reported in Housekeeping Telemetry.
**
** \par Assumptions, External Events, and Notes:
**          None
**
*/
void CFE_TBL_GetHkData(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Convert Table Registry Entry for a Table into a Message
**
** \par Description
**        Extracts the Table Registry information for the table specified by the
**        #CFE_TBL_Global_t::HkTlmTblRegIndex variable.  It then formats the
**        Registry contents into a format appropriate for downlink.
**
** \par Assumptions, External Events, and Notes:
**        #CFE_TBL_Global_t::HkTlmTblRegIndex is assumed to be a valid index into
**           the Table Registry.
**
*/
void CFE_TBL_GetTblRegData(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Housekeeping Request Message
**
** \par Description
**        Constructs and sends a Housekeeping Packet (#CFE_TBL_HousekeepingTlm_t) from task data,
**        sends the table registry packet if requested, and dumps any "dump-only" tables
**        that are pending.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Housekeeping Request Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_DONT_INC_CTR \copydoc CFE_TBL_DONT_INC_CTR
*/
int32 CFE_TBL_SendHkCmd(const CFE_TBL_SendHkCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process NO OP Command Message
**
** \par Description
**        Responds to the NOOP command by issuing an Event Message
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a NO OP Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_NoopCmd(const CFE_TBL_NoopCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Reset Counters Command Message
**
** \par Description
**        Resets command counters and validation request counters
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Reset Counters Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_DONT_INC_CTR \copydoc CFE_TBL_DONT_INC_CTR
*/
int32 CFE_TBL_ResetCountersCmd(const CFE_TBL_ResetCountersCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Load Table Command Message
**
** \par Description
**        Locates the file specified in the command message and loads the contents of the file into
**        a buffer that is associated with the table specified within the file header.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Load Table Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_LoadCmd(const CFE_TBL_LoadCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Dump Table Command Message
**
** \par Description
**        Locates the memory associated with the table identified in the command message and copies
**        the data contents to the command message specified file.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Dump Table Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_DumpCmd(const CFE_TBL_DumpCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Validate Table Command Message
**
** \par Description
**        Computes a Data Integrity Check Value for the command message specified table and notifies
**        the table's parent Application, if it has an associated validation function, that a validation
**        of the buffer's contents is required.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Validate Table Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_ValidateCmd(const CFE_TBL_ValidateCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Activate Table Command Message
**
** \par Description
**        Notifies the table's owner Application that a new version of the table is pending and should
**        be used.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as an Activate Table Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_ActivateCmd(const CFE_TBL_ActivateCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Dump Table Registry Command Message
**
** \par Description
**        Copies the contents of the Table Registry to a command message specified file.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Dump Table Registry Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_DumpRegistryCmd(const CFE_TBL_DumpRegistryCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Telemeter Table Registry Entry Command Message
**
** \par Description
**        Extracts the Table Registry information for a command message specified table and puts it into
**        a message that is sent out.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Telemeter Table Registry Entry Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_SendRegistryCmd(const CFE_TBL_SendRegistryCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Delete Critical Table's CDS Command message
**
** \par Description
**        Deletes a Critical Data Store used to hold a Critical Table's image
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as a Delete CDS Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_DeleteCDSCmd(const CFE_TBL_DeleteCDSCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Process Abort Load Command message
**
** \par Description
**        Frees any resources associated with a previously loaded table.
**
** \par Assumptions, External Events, and Notes:
**          The message pointed to by data has been identified as an Abort Load Command Message
**
** \param[in] data points to the message received via command pipe that needs processing
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
int32 CFE_TBL_AbortLoadCmd(const CFE_TBL_AbortLoadCmd_t *data);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Output block of data to file with standard cFE Table Image Headers
**
** \par Description
**        Writes the specified block of data in memory to the specified file
**        with the standard cFE File and cFE Table Image Headers.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] DumpFilename    Character string containing the full path of the file
**                            to which the contents of the table are to be written
**
** \param[in] TableName       Name of table being dumped to a file
**
** \param[in] DumpDataAddr    Address of data buffer whose contents are to be written
**                            to the specified file
**
** \param[in] TblSizeInBytes  Size of block of data to be written to the file
**
** \retval #CFE_TBL_INC_ERR_CTR  \copydoc CFE_TBL_INC_ERR_CTR
** \retval #CFE_TBL_INC_CMD_CTR  \copydoc CFE_TBL_INC_CMD_CTR
*/
CFE_TBL_CmdProcRet_t CFE_TBL_DumpToFile(const char *DumpFilename, const char *TableName, const void *DumpDataAddr,
                                        size_t TblSizeInBytes);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Aborts load by freeing associated inactive buffers and sending event message
**
** \par Description
**        This function aborts the load for the table whose registry entry is identified
**        by the registry record pointer given as an argument.  Aborting the load consists
**        of freeing any associated inactive buffer and issuing an event message.
**
** \par Assumptions, External Events, and Notes:
**        The given registry record pointer is assumed to be valid.
**
** \param[in] RegRecPtr   Pointer to registry record entry for the table whose load is to be aborted
**
*/
void CFE_TBL_AbortLoad(CFE_TBL_RegistryRec_t *RegRecPtr);

#endif /* CFE_TBL_TASK_CMDS_H */
