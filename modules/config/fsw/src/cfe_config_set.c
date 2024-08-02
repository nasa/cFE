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
 * API definition for configuration registry
 *
 * This defines the "setter" functions, which are reserved for CFE core internal use during startup.
 */

/*
** Required header files.
*/
#include "cfe_config_priv.h"

/*----------------------------------------------------------------
 *
 * CFE core internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_Config_SetValue(CFE_ConfigId_t ConfigId, uint32 Value)
{
    CFE_Config_ValueEntry_t *Entry;

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry != NULL)
    {
        Entry->ActualType      = CFE_ConfigType_VALUE;
        Entry->Datum.AsInteger = Value;
    }
}

/*----------------------------------------------------------------
 *
 * CFE core internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_Config_SetObjPointer(CFE_ConfigId_t ConfigId, const void *Ptr)
{
    CFE_Config_ValueEntry_t *Entry;

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry != NULL)
    {
        Entry->ActualType      = CFE_ConfigType_POINTER;
        Entry->Datum.AsPointer = Ptr;
    }
}

/*----------------------------------------------------------------
 *
 * CFE core internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_Config_SetArrayValue(CFE_ConfigId_t ConfigId, const CFE_Config_ArrayValue_t *ArrayPtr)
{
    CFE_Config_ValueEntry_t *Entry;

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry != NULL)
    {
        Entry->ActualType      = CFE_ConfigType_ARRAY;
        Entry->Datum.AsPointer = ArrayPtr;
    }
}

/*----------------------------------------------------------------
 *
 * CFE core internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_Config_SetString(CFE_ConfigId_t ConfigId, const char *Ptr)
{
    CFE_Config_ValueEntry_t *Entry;

    Entry = CFE_Config_LocateConfigRecordByID(ConfigId);
    if (Entry != NULL)
    {
        Entry->ActualType      = CFE_ConfigType_STRING;
        Entry->Datum.AsPointer = Ptr;
    }
}
