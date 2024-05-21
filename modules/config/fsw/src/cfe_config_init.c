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
 * Initialization for CFE configuration registry
 */

#include "cfe_config_priv.h"
#include "cfe_error.h"
#include "cfe_version.h"
#include "target_config.h"

#include "cfe_config_nametable.h"
#include "cfe_config_ids.h"

#include "cfe_config_external.h"

#include <string.h>
#include <ctype.h>
#include <stdio.h>

/* The define names are uppercase while the module names are likely lowercase */
/* C99 does not have a standard "strcasecmp" - must do it manually */
bool CFE_Config_StrCaseEq(const char *c1, const char *c2)
{
    while (*c1 != 0 && *c2 != 0)
    {
        if (toupper((unsigned char)*c1) != toupper((unsigned char)*c2))
        {
            break;
        }
        ++c1;
        ++c2;
    }

    return (*c1 == 0 && *c2 == 0);
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * Find value for given config key
 *
 *-----------------------------------------------------------------*/
const char *CFE_Config_FindTargetKeyValue(CFE_ConfigKeyValue_t *ConfigList, const char *KeyName)
{
    const char *ValuePtr;

    ValuePtr = NULL;
    if (KeyName != NULL && ConfigList != NULL)
    {
        while (ConfigList->Key != NULL)
        {
            if (CFE_Config_StrCaseEq(KeyName, ConfigList->Key))
            {
                ValuePtr = ConfigList->Value;
                break;
            }
            ++ConfigList;
        }
    }

    return ValuePtr;
}

const char *CFE_Config_IsModuleInList(CFE_ConfigName_t *ModuleListPtr, const char *ModuleName)
{
    const char *Result;

    Result = NULL;
    while (ModuleListPtr != NULL && ModuleListPtr->Name != NULL)
    {
        if (CFE_Config_StrCaseEq(ModuleListPtr->Name, ModuleName))
        {
            Result = ModuleListPtr->Name;
            break;
        }
        ++ModuleListPtr;
    }

    return Result;
}

const char *CFE_Config_IsPspModule(CFE_StaticModuleLoadEntry_t *PspModuleList, const char *ModuleName)
{
    const char *                 Result;
    CFE_StaticModuleLoadEntry_t *ModuleListPtr;

    ModuleListPtr = PspModuleList;
    Result        = NULL;
    while (ModuleListPtr != NULL && ModuleListPtr->Name != NULL)
    {
        if (CFE_Config_StrCaseEq(ModuleListPtr->Name, ModuleName))
        {
            Result = ModuleListPtr->Name;
            break;
        }
        ++ModuleListPtr;
    }

    return Result;
}

const char *CFE_Config_FindStaticModName(CFE_ConfigName_t *ModuleListSet[], size_t SetLength, const char *ModuleName)
{
    const char *Result;
    size_t      i;

    Result = NULL;
    for (i = 0; i < SetLength; ++i)
    {
        Result = CFE_Config_IsModuleInList(ModuleListSet[i], ModuleName);
        if (Result != NULL)
        {
            break;
        }
    }

    return Result;
}

void CFE_Config_SetupModuleVersions(CFE_ConfigName_t *ModuleListSet[], size_t SetLength,
                                    CFE_ConfigKeyValue_t *ActiveList)
{
    const char *                    ModName;
    const char *                    KeyVal;
    uint32                          OffsetVal;
    const CFE_Config_IdNameEntry_t *NamePtr;

    static const char IDNAME_PREFIX[] = "MOD_SRCVER_";

    NamePtr = CFE_CONFIGID_NAMETABLE;
    for (OffsetVal = 0; OffsetVal < CFE_ConfigIdOffset_MAX; ++OffsetVal)
    {
        if (NamePtr->Name != NULL && strncmp(NamePtr->Name, IDNAME_PREFIX, sizeof(IDNAME_PREFIX) - 1) == 0)
        {
            ModName = NamePtr->Name + sizeof(IDNAME_PREFIX) - 1;

            /* skip modules which are not statically linked (version depends on dynamic load) */
            ModName = CFE_Config_FindStaticModName(ModuleListSet, SetLength, ModName);
            if (ModName != NULL)
            {
                KeyVal = CFE_Config_FindTargetKeyValue(ActiveList, ModName);
                if (KeyVal != NULL)
                {
                    CFE_Config_SetString(CFE_Config_OffsetToId(OffsetVal), KeyVal);
                }
            }
        }
        ++NamePtr;
    }
}

void CFE_Config_SetupBasicBuildInfo(void)
{
    const char *KeyVal;
    char        VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /* Global mission name */
    CFE_Config_SetString(CFE_CONFIGID_MISSION_NAME, GLOBAL_CONFIGDATA.MissionName);
    KeyVal = CFE_Config_FindTargetKeyValue(GLOBAL_CONFIGDATA.ModuleVersionList, "MISSION");
    CFE_Config_SetString(CFE_CONFIGID_MISSION_SRCVER, KeyVal);

    /* Global mission EDS runtime DB */
    CFE_Config_SetObjPointer(CFE_CONFIGID_MISSION_EDS_DB, GLOBAL_CONFIGDATA.EdsDb);
    CFE_Config_SetObjPointer(CFE_CONFIGID_MISSION_SBINTF_DB, GLOBAL_CONFIGDATA.SbIntfDb);

    /* propagate the version numbers from version.h */
    CFE_Config_SetValue(CFE_CONFIGID_CORE_VERSION_MAJOR, CFE_MAJOR_VERSION);
    CFE_Config_SetValue(CFE_CONFIGID_CORE_VERSION_MINOR, CFE_MINOR_VERSION);
    CFE_Config_SetValue(CFE_CONFIGID_CORE_VERSION_REVISION, CFE_REVISION);
    CFE_Config_SetValue(CFE_CONFIGID_CORE_VERSION_MISSION_REV, CFE_MISSION_REV);
    CFE_Config_SetValue(CFE_CONFIGID_CORE_VERSION_BUILDNUM, CFE_BUILD_NUMBER);

    CFE_Config_SetString(CFE_CONFIGID_CORE_VERSION_BASELINE, CFE_BUILD_BASELINE);
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE", CFE_SRC_VERSION, CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);
    CFE_Config_SetString(CFE_CONFIGID_CORE_VERSION_DESCRIPTION, VersionString);

    /*
     * Initialize values from the "target_config" CFE internal object
     */
    KeyVal = CFE_Config_FindTargetKeyValue(GLOBAL_CONFIGDATA.BuildEnvironment, "BUILDDATE");
    CFE_Config_SetString(CFE_CONFIGID_CORE_BUILDINFO_DATE, KeyVal);
    KeyVal = CFE_Config_FindTargetKeyValue(GLOBAL_CONFIGDATA.BuildEnvironment, "BUILDUSER");
    CFE_Config_SetString(CFE_CONFIGID_CORE_BUILDINFO_USER, KeyVal);
    KeyVal = CFE_Config_FindTargetKeyValue(GLOBAL_CONFIGDATA.BuildEnvironment, "BUILDHOST");
    CFE_Config_SetString(CFE_CONFIGID_CORE_BUILDINFO_HOST, KeyVal);
}

/*----------------------------------------------------------------
 *
 * CFE core internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_Config_Init(void)
{
    /* Clear the table, just in case it was not already cleared from initial program loading */
    memset(&CFE_Config_Global, 0, sizeof(CFE_Config_Global));

    CFE_ConfigName_t *ModuleListSet[2] = {GLOBAL_CONFIGDATA.CoreModuleList, GLOBAL_CONFIGDATA.StaticAppList};

    /*
     * Create a fixed string that can be returned in place of NULL for string returns,
     * this makes it safer/easier to use with printf-style calls
     */
    CFE_Config_Global.UnknownString = "[unknown]";

    CFE_Config_SetupBasicBuildInfo();
    CFE_Config_SetupModuleVersions(ModuleListSet, 2, GLOBAL_CONFIGDATA.ModuleVersionList);
    CFE_Config_SetupPlatformConfigInfo();

    return CFE_SUCCESS;
}
