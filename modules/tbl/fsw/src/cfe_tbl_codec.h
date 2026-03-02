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
 * API calls associated with encoding and decoding of table files
 *
 * One version of these routines must be selected at compile time, depending
 * on user configuration
 */

#ifndef CFE_TBL_CODEC_H
#define CFE_TBL_CODEC_H

#include "common_types.h"
#include "cfe_error.h"
#include "cfe_fs_extern_typedefs.h"
#include "cfe_tbl_extern_typedefs.h"
#include "cfe_tbl_resource.h"

/******************************************************************************/

/*
** Prototypes Section
*/

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Byte swaps a CFE_TBL_File_Hdr_t structure
 *
 * \par Description
 *        Converts a big-endian version of a CFE_TBL_File_Hdr_t structure to
 *        a little-endian version and vice-versa.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in, out]  HdrPtr   Pointer to table header that needs to be swapped. *HdrPtr provides the swapped header
 *
 */
void CFE_TBL_ByteSwapTblHeader(CFE_TBL_File_Hdr_t *HdrPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Performs a byte swap on a uint32 integer
 *
 * \par Description
 *        Converts a big-endian uint32 integer to a little-endian uint32 integer
 *        and vice-versa.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[in, out]  Uint32ToSwapPtr Pointer to uint32 value to be swapped. *Uint32ToSwapPtr is the swapped uint32 value
 *
 */
void CFE_TBL_ByteSwapUint32(uint32 *Uint32ToSwapPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Checks that the codec layer can work with the requested table config
 *
 * \par Description
 *        Confirms that the table binary format is able to be encoded/decoded by
 *        the codec layer, and set configuration flags accordingly to facilitate
 *        the process.
 *
 *        For a passthrough codec implementation this does nothing always succeeds.
 *        For the EDS implementation it confirms that there is an entry in the EDS
 *        data type database that corresponds to this table.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout]  ReqCfg   Pointer to requested configuration
 * \returns CFE Status code, #CFE_SUCCESS if successful
 */
CFE_Status_t CFE_TBL_ValidateCodecConfig(CFE_TBL_TableConfig_t *ReqCfg);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Checks that the codec layer can load a given table file, based on size data
 *
 * \par Description
 *        Confirms that the table binary file is within the size limits and expectations
 *        of the codec layer.  This implements a size check of the header data against
 *        the expected (encoded) size of the table data.
 *
 *        For a passthrough codec implementation this simply checks against the registered
 *        size of the table, because the binary data in the file will be bitwise identical
 *        to the data in the file, and the same size limits apply.
 *
 *        For the EDS implementation this checks against the binary size for the table data
 *        type in the EDS database.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn        Pointer to transaction object
 * \param[in]    HeaderPtr  Pointer to header data from table load file
 *
 * \returns CFE Status code, #CFE_SUCCESS if successful
 */
CFE_Status_t CFE_TBL_ValidateCodecLoadSize(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Gets the status code to return to the caller
 *
 * \par Description
 *        After completing the table load opertion, this checks the status of the
 *        table load to return to the caller.
 *
 *        Historically, CFE_TBL_Load() would return one of several possible alternate codes
 *        after a successful load, if the file being loaded was not complete.  The concept
 *        of completeness is only known inside the codec layer, as it involves the encoded data.
 *
 *        For a passthrough codec implementation this simply checks against the registered
 *        size of the table, because the binary data in the file will be bitwise identical
 *        to the data in the file, and the same size limits apply.
 *
 *        For the EDS implementation this checks against the binary size for the table data
 *        type in the EDS database.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn        Pointer to transaction object
 * \param[in]    HeaderPtr  Pointer to header data from table load file
 *
 * \returns CFE Status code, #CFE_SUCCESS if successful
 */
CFE_Status_t CFE_TBL_CodecGetFinalStatus(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *HeaderPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Read and Decode the table header from the file
 *
 * \par Description
 *        Reads the file content from the file descriptor and interpret it as a CFE table header
 *        The output buffer will be filled with the decoded header data
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    FileDescriptor    An open file ID positioned at the table header
 * \param[out]   HeaderPtr          Pointer to buffer that will hold header data
 * \returns CFE Status code, #CFE_SUCCESS if successful
 */
CFE_Status_t CFE_TBL_DecodeHeadersFromFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                           CFE_TBL_File_Hdr_t *HeaderPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Encode and write the table header to the file
 *
 * \par Description
 *        Encode the CFE table header data into the format required, and write
 *        the encoded data into the file.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn               Pointer to transaction object
 * \param[in]    FileDescriptor    An open file ID positioned at the table header
 * \param[in]    HeaderPtr         Pointer to buffer that holds header data
 * \returns CFE Status code, #CFE_SUCCESS if successful
 */
CFE_Status_t CFE_TBL_EncodeHeadersToFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                         const CFE_TBL_File_Hdr_t *HeaderPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Encode the table data to the destination buffer
 *
 * \par Description
 *        Encode the CFE table content data according to the defined format, and writes the encoded
 *        data into the destination buffer, if applicable.
 *
 *        In a passthrough implementation, the source and destination buffers may be the same,
 *        and this function is a no-op which always succeeds.  If an actual encode operation
 *        is performed, the destination buffer must be different/separate from the source.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn             Pointer to transaction object
 * \param[in]    SourceBuffer    Table buffer containing the native (C struct) data
 * \param[out]   DestBuffer      Table buffer to hold encoded data
 * \returns CFE Status code, #CFE_SUCCESS if successful
 */
CFE_Status_t CFE_TBL_EncodeOutputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
                                      CFE_TBL_LoadBuff_t *DestBuffer);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Decode the table data from the destination buffer
 *
 * \par Description
 *        Decode the CFE table content data according to the defined format, and write the decoded
 *        data into the destination buffer, if applicable.
 *
 *        In a passthrough implementation, the source and destination buffers may be the same,
 *        and this function is a no-op which always succeeds.  If an actual decode operation
 *        is performed, the destination buffer must be different/separate from the source.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn             Pointer to transaction object
 * \param[in]    SourceBuffer    Table buffer containing the encoded data
 * \param[out]   DestBuffer      Table buffer to hold native (C struct) data
 * \returns CFE Status code, #CFE_SUCCESS if successful
 */
CFE_Status_t CFE_TBL_DecodeInputData(CFE_TBL_TxnState_t *Txn, const CFE_TBL_LoadBuff_t *SourceBuffer,
                                     CFE_TBL_LoadBuff_t *DestBuffer);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Acquire a temporary buffer for holding encoded data
 *
 * \par Description
 *        Gets a buffer for holding the encoded data, which is necessary for holding the data
 *        until it can be written to a file (encode) or after reading from a file (decode).
 *
 *        In a passthrough implementation, the source and destination buffers may be the same,
 *        and this function may directly return the "load in progress" buffer associated with the table.
 *        If an actual encode/decode operation is necessary, this allocates a separate buffer.
 *
 * \par Assumptions, External Events, and Notes:
 *        Must be followed by a call to CFE_TBL_ReleaseCodecBuffer()
 *
 * \param[in]   RegRecPtr      Pointer to table registry record
 * \returns Pointer to temporary buffer
 */
CFE_TBL_LoadBuff_t *CFE_TBL_AcquireCodecBuffer(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Release a temporary buffer for holding encoded data
 *
 * \par Description
 *        When the encode/decode operation is complete, this returns the temporary buffer
 *        to the pool for future use.
 *
 *        In a passthrough implementation, this may be a no-op.
 *
 * \par Assumptions, External Events, and Notes:
 *        Must follow every call to CFE_TBL_AcquireCodecBuffer()
 *
 * \param[in]   BufferPtr      Pointer to temporary buffer
 */
void CFE_TBL_ReleaseCodecBuffer(CFE_TBL_LoadBuff_t *BufferPtr);

/******************************************************************************/

#endif
