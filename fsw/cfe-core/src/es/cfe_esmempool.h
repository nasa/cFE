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

/**
 * \file cfe_esmempool.h
 *
 *  Created on: Jan 21, 2015
 *      Author: joseph.p.hickey@nasa.gov
 *
 * Contains data structure definitions used by the ES mempool implementation.
 * These had previously been defined in cfe_esmempool.c.  The definitions are
 * moved into this header file so they can be shared with the unit test.
 */

#ifndef _CFE_ES_MEMPOOL_H_
#define _CFE_ES_MEMPOOL_H_

/*
** Include Files
*/
#include "common_types.h"

typedef struct BD BD_t;

struct BD
{
  uint16    CheckBits;
  uint16    Allocated;
  uint32    Size;
  BD_t     *Next;
};

typedef struct
{
  BD_t    *Top;
  uint32   NumCreated;
  uint32   NumFree;
  uint32   MaxSize;
} BlockSizeDesc_t;

/*
** Memory Pool Type
*/
typedef struct
{
   cpuaddr          PoolHandle;
   cpuaddr          Size;
   cpuaddr          End;
   cpuaddr          CurrentAddr;
   cpuaddr          AlignMask;
   BlockSizeDesc_t *SizeDescPtr;
   uint16           CheckErrCntr;
   uint16           RequestCntr;
   uint32           MutexId;
   uint32           UseMutex;
   BlockSizeDesc_t  SizeDesc[CFE_ES_MAX_MEMPOOL_BLOCK_SIZES];
} Pool_t;



#endif  /* _cfe_esmempool_ */
