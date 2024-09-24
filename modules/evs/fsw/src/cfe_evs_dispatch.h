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
 *  Event Services API - Dispatch API
 */

#ifndef CFE_EVS_DISPATCH_H
#define CFE_EVS_DISPATCH_H

/********************************** Include Files  ************************************/
#include "common_types.h"
#include "cfe_evs_api_typedefs.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_msg_api_typedefs.h"

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Command Pipe Processing
 *
 * This function processes packets received on the EVS command pipe.
 */
void CFE_EVS_ProcessCommandPacket(const CFE_SB_Buffer_t *SBBufPtr);

#endif /* CFE_EVS_DISPATCH_H */
