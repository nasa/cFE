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
 * @file
 *
 * Header definining the table services table load (read from file) methods
 */

#ifndef CFE_TBL_LOAD_H
#define CFE_TBL_LOAD_H

/*
 * Required header files...
 */
#include "cfe_tbl_transaction.h"
#include "cfe_fs_extern_typedefs.h"
#include "cfe_error.h"

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Opens a table load file
 *
 * Opens a file descriptor for reading, and extracts the necessary headers from it
 *
 * If successful, the file descriptor will be left in an open state positioned appropritely
 * to read the table data.  If not successful, then the file will be closed and events
 * will be added to the transaction object for deferred reporting.
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    Filename          File name to open
 * \param[out]   FileDescOut       Buffer to hold the file descriptor ID
 * \param[out]   TblFileHeader     Pointer to buffer that will hold header data
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnOpenTableLoadFile(CFE_TBL_TxnState_t *Txn, const char *Filename, osal_id_t *FileDescOut,
                                          CFE_TBL_CombinedFileHdr_t *TblFileHeader);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Checks compatibilty between file headers and destination table
 *
 * Confirms that the file headers match the table into which it is going to be loaded
 *
 * If this returns CFE_SUCCESS, then all header checks passed and the file can be loaded
 * If this returns an error code, then the file should not be loaded.  The transaction
 * object will be populated with event data related to the mismatch.
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    TblFileHeader     Pointer to buffer that holds header data
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_ValidateFileIsLoadable(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *TblFileHeader);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Loads the file data into the table holding buffer
 *
 * Reads data from the file descriptor, which is assumed to refer to an open file that is positioned
 * at the start of table data (i.e. after the headers).  The data will be read into the load in
 * progress buffer associated with the table registry record pointed at in the transaction.
 *
 * For now, Offset and NumBytes are specified to facilitate partial file loads, but this feature
 * is likely to be deprecated/removed in a future version.
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    FileDescriptor    Descriptor of an open file, positioned at the file data
 * \param[in]    Offset            Position in buffer to load data (typically 0)
 * \param[in]    NumBytes          Number of bytes to load (typically the size of the table)
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_LoadContentFromFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor, size_t Offset,
                                         size_t NumBytes);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Sends events related to table load preparation
 *
 * Sends events that are not related to a specific file or file header
 *
 * \param[inout] Txn               Pointer to transaction object
 */
void CFE_TBL_SendTableLoadEvents(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Sends events related to file data
 *
 * Sends events that are related to a specific file or file header
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    Filename          The name of the file being loaded
 * \param[in]    FileHdr           Buffer containing file header data
 */
void CFE_TBL_SendTableLoadFileEvents(CFE_TBL_TxnState_t *Txn, const char *Filename, const CFE_TBL_CombinedFileHdr_t *FileHdr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Sets the registry metadata from fields in the file header
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    Filename          The name of the file being loaded
 * \param[in]    StdHeader         Buffer containing file header data
 */
void CFE_TBL_SetMetaDataFromFileHeader(CFE_TBL_TxnState_t *Txn, const char *Filename, const CFE_FS_Header_t *StdHeader);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Reads Table File Headers
**
** \par Description
**        Reads Table File Headers and performs rudimentary checks
**        on header contents to ensure the acceptability of the
**        file format.
**
** \par Assumptions, External Events, and Notes:
**        -# FileDescriptor is assumed to be valid
**
** \param[inout] Txn               Pointer to transaction object
** \param[in]    FileDescriptor    File Descriptor, as provided by OS_fopen
** \param[out]   FileHeader        Pointer to buffer to be filled with the header data
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_NO_STD_HEADER       \copydoc CFE_TBL_ERR_NO_STD_HEADER
** \retval #CFE_TBL_ERR_NO_TBL_HEADER       \copydoc CFE_TBL_ERR_NO_TBL_HEADER
** \retval #CFE_TBL_ERR_BAD_CONTENT_ID      \copydoc CFE_TBL_ERR_BAD_CONTENT_ID
** \retval #CFE_TBL_ERR_BAD_SUBTYPE_ID      \copydoc CFE_TBL_ERR_BAD_SUBTYPE_ID
** \retval #CFE_TBL_ERR_BAD_SPACECRAFT_ID   \copydoc CFE_TBL_ERR_BAD_SPACECRAFT_ID
** \retval #CFE_TBL_ERR_BAD_PROCESSOR_ID    \copydoc CFE_TBL_ERR_BAD_PROCESSOR_ID
**
*/
CFE_Status_t CFE_TBL_ReadHeaders(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                 CFE_TBL_CombinedFileHdr_t *FileHeader);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Loads a table buffer with data from a specified file
**
** \par Description
**        Locates the specified filename in the onboard filesystem
**        and loads its contents into the specified working buffer.
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes parameters have been verified.
**
** \param[inout]  Txn          Pointer to a transaction object
**
** \param[in]  Filename         Pointer to ASCII string containing full path and filename
**                              of table image file to be loaded
**
** \returns CFE status code
** \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
*/
CFE_Status_t CFE_TBL_TxnLoadFromFile(CFE_TBL_TxnState_t *Txn, const char *Filename);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Loads a table buffer with data from a specified memory address
 *
 * \par Description
 *        Copies the data from the specified source address
 *        and loads contents into the specified working buffer.
 *
 * \par Assumptions, External Events, and Notes:
 *        -# This function assumes parameters have been verified.
 *
 * \param[inout]  Txn          Pointer to a transaction object
 * \param[in]     Address      Memory address to copy data from
 *
 * \returns CFE status code
 * \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnLoadFromSourceAddr(CFE_TBL_TxnState_t *Txn, const void *Address);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Aborts a load that was previously started
 *
 * This cancels the pending load and drops the load in progress buffer, if it
 * is not yet activated.  This can be used, for example, if the data did not
 * validate and thus the buffer should never be activated.
 *
 * \param[inout] RegRecPtr    Pointer to registry record with pending load to cancel
 */
void CFE_TBL_AbortLoad(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Validates a load request against table state and configuration
 *
 * Checks that the load request is valid and the table configuration and status permit
 * it to be loaded
 *
 * \param[inout]  Txn          Pointer to a transaction object
 * \param[in]     SrcType      The data source from user request
 *
 * \returns CFE status code
 * \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_ValidateLoadRequest(CFE_TBL_TxnState_t *Txn, CFE_TBL_SrcEnum_t SrcType);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Validates the contents of the Load In Progress buffer
 *
 * Calls the validation function on the working buffer.  This should be done as part of the
 * loading process prior to activating the new table data.
 *
 * If things validate properly, this passes through the LoadStatus (preserving any INFO codes)
 *
 * \param[inout]  Txn          Pointer to a transaction object
 * \param[in]     LoadStatus   The status of the loading operation
 *
 * \returns CFE status code based on LoadStatus
 * \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_ValidateLoadInProgress(CFE_TBL_TxnState_t *Txn, CFE_Status_t LoadStatus);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Completes the initial table load process
 *
 * When initially loading a table, the data was loaded directly into the buffer
 * and does not need to be separately copied or activated.  This handles the process
 * of setting the table state to indicate it has been loaded successfully.
 *
 * \param[inout]  Txn          Pointer to a transaction object
 *
 * \returns CFE status code
 * \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_CompleteInitialLoad(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Completes the table reload process
 *
 * When re-loading a table, the data was loaded into a working buffer and this will
 * need separate activation, either via copying the data or switching the active buffer.
 * This handles the process of setting the table state to do this activation.
 *
 * \param[inout]  Txn          Pointer to a transaction object
 *
 * \returns CFE status code
 * \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_CompleteReload(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Cleans up after a table load and completes the process
 *
 * If the table load failed, this drops the working buffer and resets things to a proper
 * state where loading can be attempted again.
 *
 * If the load was successful, this sets the status of the table as needed, calling either
 * CFE_TBL_CompleteReload() or CFE_TBL_CompleteInitialLoad() as appropriate (the activation
 * varies depending on whether it is an initial load or a reload).
 *
 * This passes through the LoadStatus (thus preserving any INFO codes from the load process)
 *
 * \param[inout]  Txn          Pointer to a transaction object
 * \param[in]     LoadStatus   The status of the loading operation
 *
 * \returns CFE status code based on LoadStatus
 * \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_LoadFinish(CFE_TBL_TxnState_t *Txn, CFE_Status_t LoadStatus);

#endif /* CFE_TBL_TRANSACTION_H */
