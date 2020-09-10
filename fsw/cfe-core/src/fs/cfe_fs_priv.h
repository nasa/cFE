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

/******************************************************************************
** File: cfe_fs_priv.h
**
** Purpose:
**      This header file contains prototypes for private functions and type
**      definitions for FS internal use.
**
** Author:  A. Cudmore/NASA GSFC 
**
**
******************************************************************************/

#ifndef _cfe_fs_priv_
#define _cfe_fs_priv_

/*
** Includes
*/
#include "common_types.h"
#include "cfe_fs.h"
#include "cfe_es.h"

/*
** Macro Definitions
*/


/*
** Type Definitions
*/

/******************************************************************************
**  Typedef: CFE_FS_t 
**
**  Purpose:
**     This structure contains the FS global variables.
*/
typedef struct 
{
    osal_id_t              SharedDataMutexId;

} CFE_FS_t;

/*
** FS Function Prototypes
*/

extern void CFE_FS_LockSharedData(const char *FunctionName );
extern void CFE_FS_UnlockSharedData(const char *FunctionName );
extern void CFE_FS_ByteSwapCFEHeader(CFE_FS_Header_t *Hdr);
extern void CFE_FS_ByteSwapUint32(uint32 *Uint32ToSwapPtr);


#endif /* _cfe_fs_priv_ */
/*****************************************************************************/
