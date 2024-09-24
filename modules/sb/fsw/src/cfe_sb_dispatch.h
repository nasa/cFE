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
 *      definitions for SB internal use.
 *
 * Author:   R.McGraw/SSI
 *
 */

#ifndef CFE_SB_DISPATCH_H
#define CFE_SB_DISPATCH_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_sb_api_typedefs.h"

/*---------------------------------------------------------------------------------------*/
/**
 * Processes a single message buffer that has been received from the command pipe
 *
 * @param SBBufPtr Software bus buffer pointer
 */
void CFE_SB_ProcessCmdPipePkt(const CFE_SB_Buffer_t *SBBufPtr);

#endif /* CFE_SB_DISPATCH_H */
