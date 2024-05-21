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
 * This file contains the CFE configuration registry global data definitions.
 */

#ifndef CFE_CONFIG_PRIV_H
#define CFE_CONFIG_PRIV_H

/*
** Includes
*/
#include "cfe_config_core_internal.h"
#include "cfe_config_table.h"
#include "cfe_config.h"
#include "target_config.h"

#include "cfe_config_init.h"
#include "cfe_config_lookup.h"
#include "cfe_config_set.h"

/*
 * Actual definition of the config registry table
 *
 * Note this object is instantiated by a bit of generated code from CMake -
 * The final length depends on the number of config keys in use in the mission
 */
typedef struct
{
    const char *            UnknownString;
    CFE_Config_ValueEntry_t Table[CFE_ConfigIdOffset_MAX];
} CFE_Config_Global_t;

extern CFE_Config_Global_t CFE_Config_Global;

/*
 ******************************
 * INTERNAL FUNCTION CALLS
 ******************************
 */

bool        CFE_Config_StrCaseEq(const char *c1, const char *c2);
const char *CFE_Config_FindTargetKeyValue(CFE_ConfigKeyValue_t *ConfigList, const char *KeyName);
const char *CFE_Config_IsModuleInList(CFE_ConfigName_t *ModuleListPtr, const char *ModuleName);
const char *CFE_Config_IsPspModule(CFE_StaticModuleLoadEntry_t *PspModuleList, const char *ModuleName);
const char *CFE_Config_FindStaticModName(CFE_ConfigName_t *ModuleListSet[], size_t SetLength, const char *ModuleName);
void        CFE_Config_SetupModuleVersions(CFE_ConfigName_t *ModuleListSet[], size_t SetLength,
                                           CFE_ConfigKeyValue_t *ActiveList);

/**
 * @brief Converts a configuration ID to a table index
 *
 * This function is intended for internal use only, and does not perform
 * any error checks.  It uses a simpler conversion than other ID conversions.
 */
static inline uint32 CFE_Config_IdToOffset(CFE_ConfigId_t ConfigId)
{
    uint32 OffsetVal;

    OffsetVal = CFE_RESOURCEID_TO_ULONG(ConfigId);
    OffsetVal -= CFE_CONFIGID_BASE;

    return OffsetVal;
}

/**
 * @brief Converts a table index to a configuration ID
 *
 * This function is intended for internal use only, and does not perform
 * any error checks.  It uses a simpler conversion than other ID conversions.
 */
static inline CFE_ConfigId_t CFE_Config_OffsetToId(uint32 OffsetVal)
{
    return CFE_CONFIGID_C(CFE_ResourceId_FromInteger(CFE_CONFIGID_BASE + OffsetVal));
}

#endif /* CFE_CONFIG_PRIV_H */
