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
 * Declarations and prototypes for cfe_evs_extern_typedefs module
 */

#ifndef EDS_CFE_EVS_EXTERN_TYPEDEFS_H
#define EDS_CFE_EVS_EXTERN_TYPEDEFS_H

#include "cfe_evs_eds_typedefs.h"

#define CFE_EVS_MAKE_BITMASK(pos) (1 << ((pos)-1))

#define CFE_EVS_DEBUG_BIT       CFE_EVS_MAKE_BITMASK(CFE_EVS_EventType_DEBUG)
#define CFE_EVS_INFORMATION_BIT CFE_EVS_MAKE_BITMASK(CFE_EVS_EventType_INFORMATION)
#define CFE_EVS_ERROR_BIT       CFE_EVS_MAKE_BITMASK(CFE_EVS_EventType_ERROR)
#define CFE_EVS_CRITICAL_BIT    CFE_EVS_MAKE_BITMASK(CFE_EVS_EventType_CRITICAL)

#define CFE_EVS_PORT1_BIT CFE_EVS_MAKE_BITMASK(CFE_EVS_EventOutput_PORT1)
#define CFE_EVS_PORT2_BIT CFE_EVS_MAKE_BITMASK(CFE_EVS_EventOutput_PORT2)
#define CFE_EVS_PORT3_BIT CFE_EVS_MAKE_BITMASK(CFE_EVS_EventOutput_PORT3)
#define CFE_EVS_PORT4_BIT CFE_EVS_MAKE_BITMASK(CFE_EVS_EventOutput_PORT4)

#endif /* CFE_EVS_EXTERN_TYPEDEFS_H */
