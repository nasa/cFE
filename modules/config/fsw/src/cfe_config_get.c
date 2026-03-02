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
 * API definition for configuration registry
 *
 * This defines the "getter" functions, which are publicly available
 *
 * @note The declaration for all functions in this file is in the "core_api"
 * module, not here This file constitutes the entire externally-callable API for
 * the config module.
 */

/*
** Required header files.
*/
#include "cfe_config_eds.h"
#include "cfe_config_nametable.h"
#include "cfe_config_priv.h"
#include "cfe_version.h"

#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_Config_GetValue(CFE_ConfigId_t ConfigId)
{
    const CFE_Config_ValueEntry_t *Entry;

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry == NULL || Entry->ActualType != CFE_ConfigType_VALUE)
    {
        return 0;
    }

    return Entry->Datum.AsInteger;
}

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
const void *CFE_Config_GetObjPointer(CFE_ConfigId_t ConfigId)
{
    const CFE_Config_ValueEntry_t *Entry;

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry == NULL || (Entry->ActualType != CFE_ConfigType_POINTER && Entry->ActualType != CFE_ConfigType_STRING))
    {
        return NULL;
    }

    return Entry->Datum.AsPointer;
}

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Config_ArrayValue_t CFE_Config_GetArrayValue(CFE_ConfigId_t ConfigId)
{
    const CFE_Config_ValueEntry_t *Entry;
    static CFE_Config_ArrayValue_t NULL_ARRAY = {0, NULL};

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry == NULL || Entry->ActualType != CFE_ConfigType_ARRAY)
    {
        return NULL_ARRAY;
    }

    return *((const CFE_Config_ArrayValue_t *)Entry->Datum.AsPointer);
}

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
const char *CFE_Config_GetString(CFE_ConfigId_t ConfigId)
{
    const CFE_Config_ValueEntry_t *Entry;

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry == NULL || Entry->ActualType != CFE_ConfigType_STRING)
    {
        return CFE_Config_Global.UnknownString;
    }

    return Entry->Datum.AsPointer;
}

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
const char *CFE_Config_GetName(CFE_ConfigId_t ConfigId)
{
    uint32 OffsetVal;

    OffsetVal = CFE_Config_IdToOffset(ConfigId);

    if (OffsetVal >= CFE_ConfigIdOffset_MAX)
    {
        return CFE_Config_Global.UnknownString;
    }

    return CFE_CONFIGID_NAMETABLE[OffsetVal].Name;
}

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ConfigId_t CFE_Config_GetIdByName(const char *Name)
{
    const CFE_Config_IdNameEntry_t *NamePtr;
    uint32                          OffsetVal;

    NamePtr = CFE_CONFIGID_NAMETABLE;
    for (OffsetVal = 0; OffsetVal < CFE_ConfigIdOffset_MAX; ++OffsetVal)
    {
        if (NamePtr->Name != NULL && strcmp(NamePtr->Name, Name) == 0)
        {
            break;
        }
        ++NamePtr;
    }

    if (OffsetVal >= CFE_ConfigIdOffset_MAX)
    {
        return CFE_CONFIGID_UNDEFINED;
    }

    return CFE_Config_OffsetToId(OffsetVal);
}

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_Config_IterateAll(void *Arg, CFE_Config_Callback_t Callback)
{
    const CFE_Config_IdNameEntry_t *NamePtr;
    uint32                          OffsetVal;

    NamePtr = CFE_CONFIGID_NAMETABLE;
    for (OffsetVal = 0; OffsetVal < CFE_ConfigIdOffset_MAX; ++OffsetVal)
    {
        if (CFE_Config_Global.Table[OffsetVal].ActualType != CFE_ConfigType_UNDEFINED)
        {
            Callback(Arg, CFE_Config_OffsetToId(OffsetVal), NamePtr->Name);
        }
        ++NamePtr;
    }
}

/*----------------------------------------------------------------
 *
 * Internal helper routine, not called outside of this unit
 *
 *-----------------------------------------------------------------*/
void CFE_Config_GetMissionRevString(char *Buf, size_t Size, uint8 MissionRev, const char *LastOffcRel)
{
    if (MissionRev == 0)
    {
        /*
         * Mission Revision 0 is reserved only for official releases
         * from the CFS development team that have gone through the
         * full release process
         */
        snprintf(Buf, Size, "%s Official Release", LastOffcRel);
    }
    else if (MissionRev == 0xFF)
    {
        /*
         * Mission Revision 255 (0xFF) is reserved for use by the CFS
         * development team for evaluation builds (refer to git commit ID
         * for more specific information about the build).
         */
        snprintf(Buf, Size, "DEV BUILD, based on %s", LastOffcRel);
    }
    else
    {
        /*
         * Mission Rev values 1-254 are for user customizations.  CFS users are free
         * to set this value in any way that is meaningful to them.
         */
        snprintf(Buf, Size, "M%u, based on %s", MissionRev, LastOffcRel);
    }
}

/*----------------------------------------------------------------
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_Config_GetVersionString(char *Buf, size_t Size, const char *Component, const char *SrcVersion,
                                 const char *CodeName, const char *LastOffcRel)
{
    char RevInfo[32];

    CFE_Config_GetMissionRevString(RevInfo, sizeof(RevInfo), CFE_MISSION_REV, LastOffcRel);

    snprintf(Buf, Size, "%s %s (%s) %s, EDS %s", Component, SrcVersion, CodeName, RevInfo,
             CFE_Config_EdsState(Component));
}
