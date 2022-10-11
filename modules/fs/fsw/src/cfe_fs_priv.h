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
 * Purpose:
 *      This header file contains prototypes for private functions and type
 *      definitions for FS internal use.
 *
 * Author:  A. Cudmore/NASA GSFC
 *
 */

#ifndef CFE_FS_PRIV_H
#define CFE_FS_PRIV_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_fs_api_typedefs.h"
#include "cfe_es_api_typedefs.h"

/*
** Macro Definitions
*/

/*
 * Max Number of file write requests that can be queued
 *
 * This needs to be a power of two to simplify the masking/wraparound (bitwise AND).
 */
#define CFE_FS_MAX_BACKGROUND_FILE_WRITES 4

/*
 * Background file credit accumulation rate
 *
 * The background file writer will limit the total bytes written over time.  This
 * controls the amount of "credit" (bytes that can be written) per second
 * of elapsed time.
 *
 * This permits a file writing rate of up to 10kbytes/sec.
 */
#define CFE_FS_BACKGROUND_CREDIT_PER_SECOND 10000

/*
 * Maximum credit that the background write task can accumulate
 *
 * The background file writer will limit the total bytes written over time, and
 * will accumulate credit against this limit while no writes are in progress.
 * This is an upper cap on the amount of credit that can be accumulated.
 *
 * Without this limit, after a long period of inactivity without any file
 * writes, a large credit would essentially bypass the rate limiting for
 * the next file write command(s) once they are issued.
 */
#define CFE_FS_BACKGROUND_MAX_CREDIT 10000

/*
** Type Definitions
*/

/*
 * Background file dump entry structure
 *
 * This structure is stored in global memory and keeps the state
 * of the file dump from one iteration to the next.
 */
typedef struct
{
    CFE_ES_AppId_t              RequestorAppId;
    CFE_FS_FileWriteMetaData_t *Meta;
} CFE_FS_BackgroundFileDumpEntry_t;

typedef struct
{
    osal_id_t Fd;
    int32     Credit;
    uint32    RecordNum;
    size_t    FileSize;
} CFE_FS_CurrentFileState_t;

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Background file dump queue structure
 *
 * This structure is stored in global memory and keeps the state
 * of the file dump from one iteration to the next.
 *
 * Normally when idle the "RequestCount" and "CompleteCount" are the
 * same value.  When an application requests a background file dump,
 * the "RequestCount" is incremented accordingly, and when the background
 * job finishes, the "CompleteCount" is incremented accordingly.
 */
typedef struct
{
    uint32 RequestCount;  /**< Total Number of background file writes requested */
    uint32 CompleteCount; /**< Total Number of background file writes completed */

    /**
     * Data related to each background file write request
     */
    CFE_FS_BackgroundFileDumpEntry_t Entries[CFE_FS_MAX_BACKGROUND_FILE_WRITES];

    /**
     * Persistent storage for the current file write
     * (reused for each file)
     */
    CFE_FS_CurrentFileState_t Current;
} CFE_FS_BackgroundFileDumpState_t;

/******************************************************************************
**  Typedef: CFE_FS_Global_t
**
**  Purpose:
**     This structure contains the FS global variables.
*/
typedef struct
{
    osal_id_t SharedDataMutexId;

    CFE_FS_BackgroundFileDumpState_t FileDump;
} CFE_FS_Global_t;

extern CFE_FS_Global_t CFE_FS_Global;

/*
** FS Function Prototypes
*/

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtains exclusive access to the FS global data structures
 *
 * @param FunctionName The name of the calling function
 */
void CFE_FS_LockSharedData(const char *FunctionName);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Releases exclusive access to the FS global data structures
 *
 * @param FunctionName The name of the calling function
 */
void CFE_FS_UnlockSharedData(const char *FunctionName);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief byte swap cFE file header structure
 *
 * @param Hdr The object to byte swap
 */
void CFE_FS_ByteSwapCFEHeader(CFE_FS_Header_t *Hdr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief byte swap uint32 value
 *
 * @param Uint32ToSwapPtr The buffer to byte swap
 */
void CFE_FS_ByteSwapUint32(uint32 *Uint32ToSwapPtr);

#endif /* CFE_FS_PRIV_H */
