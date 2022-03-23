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
 * cFE Internal configuration APIs
 *
 * The internal APIs prototyped within this file are only intended to be invoked from
 * other CFE core apps.  They still need to be prototyped in the shared header such that
 * they can be called from other core modules, but applications should not call these.
 */

#ifndef CFE_CONFIG_CORE_INTERNAL_H
#define CFE_CONFIG_CORE_INTERNAL_H

#include "cfe_config_api_typedefs.h"

#include "common_types.h"

/** @defgroup CFEAPIConfigCoreInternal cFE Internal configuration APIs, internal to CFE core
 * @{
 */

int32 CFE_Config_Init(void);

/*
 * The "Set" API is only used during init phase
 */
void CFE_Config_SetValue(CFE_ConfigId_t ConfigId, uint32 Value);
void CFE_Config_SetString(CFE_ConfigId_t ConfigId, const char *Ptr);
void CFE_Config_SetObjPointer(CFE_ConfigId_t ConfigId, const void *Ptr);

/**@}*/

#endif /* CFE_CONFIG_CORE_INTERNAL_H */
