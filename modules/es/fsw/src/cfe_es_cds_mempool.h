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
 *  This file contains the Internal interface for the cFE Critical Data Store
 *  memory pool functions.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *  Notes:
 *
 */

#ifndef CFE_ES_CDS_MEMPOOL_H
#define CFE_ES_CDS_MEMPOOL_H

/*
** Include Files
*/
#include "cfe_es_cds.h"

/*
** Macro Definitions
*/
#define CFE_ES_CDS_NUM_BLOCK_SIZES 17

/*****************************************************************************/
/*
** Function prototypes
*/

/*---------------------------------------------------------------------------------------*/
/**
** \brief Creates a CDS memory pool from scratch
**
** \par Description
**        Creates a memory pool of the specified size starting at the specified
**        offset into the CDS memory.
**
** \par Assumptions, External Events, and Notes:
**          This function must only be called during "Early Init" phase.
**
** \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**
******************************************************************************/
int32 CFE_ES_CreateCDSPool(size_t CDSPoolSize, size_t StartOffset);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Rebuilds an existing CDS memory pool from contents
 *
 * \par Description
 *        Reconstructs pool data structures from an existing CDS
 *
 * \par Assumptions, External Events, and Notes:
 *          This function is only ever called during "Early Init" phase.
 *
 * \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
 */
int32 CFE_ES_RebuildCDSPool(size_t CDSPoolSize, size_t StartOffset);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Writes a block of data to CDS
 */
int32 CFE_ES_CDSBlockWrite(CFE_ES_CDSHandle_t Handle, const void *DataToWrite);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Reads a block of data from CDS
 */
int32 CFE_ES_CDSBlockRead(void *DataRead, CFE_ES_CDSHandle_t Handle);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Computes the minimum required size for a CDS pool
 */
size_t CFE_ES_CDSReqdMinSize(uint32 MaxNumBlocksToSupport);

#endif /* CFE_ES_CDS_MEMPOOL_H */
