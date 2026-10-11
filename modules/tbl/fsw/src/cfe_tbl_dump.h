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

/** Maximum snapshot bytes supplied to the rate-limited FS writer per record. */
#define CFE_TBL_DUMP_BLOCK_SIZE 1024

/*
** Required header files...
*/
#include "cfe_error.h"
#include "cfe_tbl_resource.h"

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Writes fully populated Table File Headers without replacing snapshot
 * metadata
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    FileDescriptor    File Descriptor, as provided by OS_fopen
 * \param[in]    FileHeader        Pointer to buffer containing header data to
 * write
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t
CFE_TBL_WriteHeaders(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor, const CFE_TBL_CombinedFileHdr_t *FileHeader);

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
CFE_Status_t
CFE_TBL_PrepareDumpSnapshotBuffer(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t DumpCtrlId, const char *DumpFilename);

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
CFE_Status_t CFE_TBL_AllocateDumpCtrlBlock(CFE_TBL_TxnState_t         *Txn,
                                           CFE_TBL_DumpCtrlId_t       *DumpCtrlIdOut,
                                           CFE_TBL_BufferSelect_Enum_t BufferSelect);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Send events related to a table dump transaction
 *
 * This sends all deferred events associated with the specified transaction
 *
 * \param[inout] Txn                 Pointer to transaction object
 * \param[in]    FileName            The file name from the original request
 * \param[in]    RequestedTableName  The table name from the original request
 *
 */
void CFE_TBL_SendTableDumpEvents(CFE_TBL_TxnState_t *Txn, const char *FileName, const char *RequestedTableName);

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
 * \brief Queues a table snapshot for asynchronous file output
 *
 * The control block and snapshot buffer remain reserved until FS reports that
 * the request is no longer pending. A successful return means queued, not
 * written. A full queue returns CFE_STATUS_REQUEST_ALREADY_PENDING so the
 * caller can retry without discarding the snapshot.
 *
 * \param[inout] DumpCtlPtr Pointer to the table dump control block
 *
 * \returns CFE Status Code
 * \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_WriteSnapshotToFile(CFE_TBL_DumpControl_t *DumpCtlPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Executes background activity associated with table dump
 *
 * Queues completed snapshots and reclaims requests after the FS writer has
 * released their metadata. No table file content is written in this function.
 * Successful completion updates LastFileDumped here, in the TBL task context.
 *
 * This is invoked periodically by the TBL housekeeping command.
 */
void CFE_TBL_TableDumpExecuteBackground(void);

#endif /* CFE_TBL_DUMP_H */
