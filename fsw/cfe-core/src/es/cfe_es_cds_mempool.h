/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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

/*
** Macro Definitions
*/
#define CFE_ES_CDS_NUM_BLOCK_SIZES     17

/*
** Type Definitions
*/

typedef uint32 CFE_ES_CDSBlockHandle_t;

typedef struct
{
  uint16    CheckBits;
  uint16    AllocatedFlag;
  uint32    SizeUsed;
  uint32    ActualSize;
  uint32    CRC;
  uint32    Next;
} CFE_ES_CDSBlockDesc_t;

typedef struct
{
   uint32   Top;
   uint32   NumCreated;
   uint32   MaxSize;
} CFE_ES_CDSBlockSizeDesc_t;
/*
** Memory Pool Type
*/
typedef struct {
   uint32   Start;
   uint32   Size;
   uint32   End;
   uint32   Current;
   int32    SizeIndex;
   uint16   CheckErrCntr;
   uint16   RequestCntr;
   uint32   MutexId;
   uint32   MinBlockSize;
   CFE_ES_CDSBlockSizeDesc_t SizeDesc[CFE_ES_CDS_NUM_BLOCK_SIZES];
} CFE_ES_CDSPool_t;

/*
 * External variables
 *
 * Note - these globals should not be modified outside of this module,
 * however the unit test code does tweak them directly in order to test specific code paths
 */
extern CFE_ES_CDSPool_t      CFE_ES_CDSMemPool;
extern CFE_ES_CDSBlockDesc_t CFE_ES_CDSBlockDesc;


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
int32 CFE_ES_CreateCDSPool(uint32 CDSPoolSize, uint32 StartOffset);


int32 CFE_ES_RebuildCDSPool(uint32 CDSPoolSize, uint32 StartOffset);

int32 CFE_ES_GetCDSBlock(CFE_ES_CDSBlockHandle_t *BlockHandle, uint32  BlockSize);

int32 CFE_ES_PutCDSBlock(CFE_ES_CDSBlockHandle_t BlockHandle);

int32 CFE_ES_CDSBlockWrite(CFE_ES_CDSBlockHandle_t BlockHandle, void *DataToWrite);

int32 CFE_ES_CDSBlockRead(void *DataRead, CFE_ES_CDSBlockHandle_t BlockHandle);

uint32 CFE_ES_CDSReqdMinSize(uint32 MaxNumBlocksToSupport);

#endif  /* _cfe_es_cds_mempool_ */
