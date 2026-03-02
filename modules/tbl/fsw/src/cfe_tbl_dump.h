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
 * Header definining the table services table dump (write to file) methods
 */

#ifndef CFE_TBL_DUMP_H
#define CFE_TBL_DUMP_H

/*
** Required header files...
*/
#include "cfe_error.h"
#include "cfe_tbl_resource.h"

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Opens a table dump file
 *
 * Opens a file descriptor for writing, and writes the necessary headers to it
 *
 * If successful, the file descriptor will be left in an open state positioned appropritely
 * to accept the table data.  If not successful, then the file will be closed and events
 * will be added to the transaction object for deferred reporting.
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    Filename          File name to open
 * \param[out]   FileDescOut       Buffer to hold the file descriptor ID
 * \param[in]    FileHeader        Pointer to buffer containing header data to write
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnOpenTableDumpFile(CFE_TBL_TxnState_t *Txn, const char *Filename, osal_id_t *FileDescOut,
                                          const CFE_TBL_CombinedFileHdr_t *FileHeader);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Writes Table File Headers
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    FileDescriptor    File Descriptor, as provided by OS_fopen
 * \param[in]    FileHeader        Pointer to buffer containing header data to write
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_WriteHeaders(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                  const CFE_TBL_CombinedFileHdr_t *FileHeader);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Prepare the snapshot buffer for a pending dump request
 *
 * Table data will be exported to a separate snapshot buffer in preparation to write to
 * the dump file.  This is necessary to capture table data in a consistent state, as the
 * snapshot will be synchronized with table updates from the owning application (depending
 * on the table configuration).
 *
 * \param[inout] Txn            Pointer to transaction object
 * \param[in]    DumpCtrlId     Identifier of dump control block
 * \param[in]    DumpFilename   Pending filename to write to
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_PrepareDumpSnapshotBuffer(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t DumpCtrlId,
                                               const char *DumpFilename);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Allocate a dump control block
 *
 * Table dump requests utilize a dump control block to track the status/progress of the operation
 * Every dump request should obtain one of these blocks, and free the block when the dump is complete
 *
 * \param[inout] Txn            Pointer to transaction object
 * \param[out]   DumpCtrlIdOut  Buffer to store the dump control ID
 * \param[in]    BufferSelect   Table Buffer selection from original request
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_AllocateDumpCtrlBlock(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t *DumpCtrlIdOut,
                                           CFE_TBL_BufferSelect_Enum_t BufferSelect);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Send events related to a table dump transaction
 *
 * This sends all deferred events associated with the specified transaction
 *
 * \param[inout] Txn                 Pointer to transaction object
 * \param[out]   RequestedTableName  The table name from the original request
 *
 */
void CFE_TBL_SendTableDumpEvents(CFE_TBL_TxnState_t *Txn, const char *RequestedTableName);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Takes the snapshot of the source table data
 *
 * Takes a copy of the source table data, and saves it into the snapshot buffer
 * Depending on table configuration, this may be synchronized with the owning application
 * to ensure that the data is in a consistent state.
 *
 * \note This function only takes a snapshot, it does not write the data to a file
 *
 * \param[inout]   DumpCtrlPtr  Pointer to the table dump control block
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_ExecuteDumpSnapshot(CFE_TBL_DumpControl_t *DumpCtrlPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Writes the table data snapshot to the output file
 *
 * Writes the data from the snapshot buffer into the final output file
 * This is the last thing to happen in the table dump process
 *
 * \param[in]   DumpCtlPtr  Pointer to the table dump control block
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_WriteSnapshotToFile(const CFE_TBL_DumpControl_t *DumpCtlPtr);

#endif /* CFE_TBL_DUMP_H */
