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
**  File: 
**  cfe_es_cds_mempool.h
**
**  Purpose:
**  This file contains the Internal interface for the cFE Critical Data Store 
**  memory pool functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/


#ifndef _cfe_es_cds_mempool_
#define _cfe_es_cds_mempool_

/*
** Include Files
*/
#include "private/cfe_private.h"
#include "cfe_es_cds.h"

/*
** Macro Definitions
*/
#define CFE_ES_CDS_NUM_BLOCK_SIZES     17


/*****************************************************************************/
/*
** Function prototypes
*/

/*****************************************************************************/
/**
** \brief Creates a CDS memory pool from scratch
**
** \par Description
**        Creates a memory pool of the specified size starting at the specified
**        offset into the CDS memory.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**                     
******************************************************************************/
int32 CFE_ES_CreateCDSPool(size_t CDSPoolSize, size_t StartOffset);

int32 CFE_ES_RebuildCDSPool(size_t CDSPoolSize, size_t StartOffset);

int32 CFE_ES_CDSBlockWrite(CFE_ES_CDSHandle_t Handle, const void *DataToWrite);

int32 CFE_ES_CDSBlockRead(void *DataRead, CFE_ES_CDSHandle_t Handle);

size_t CFE_ES_CDSReqdMinSize(uint32 MaxNumBlocksToSupport);

#endif  /* _cfe_es_cds_mempool_ */
