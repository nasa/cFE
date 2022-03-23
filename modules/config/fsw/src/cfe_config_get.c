/**
 * @file
 *
 * API definition for configuration registry
 *
 * This defines the "getter" functions, which are publicly available
 */

/*
** Required header files.
*/
#include "cfe_config_priv.h"
#include "cfe_config_map.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Function: CFE_Config_GetValue
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
 * Function: CFE_Config_GetObjPointer
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
 * Function: CFE_Config_GetObjPointer
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
 * Function: CFE_Config_GetName
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

    return CFE_CONFIG_IDNAME_MAP[OffsetVal].Name;
}

/*----------------------------------------------------------------
 *
 * Function: CFE_Config_GetIdByName
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ConfigId_t CFE_Config_GetIdByName(const char *Name)
{
    const CFE_Config_IdNameEntry_t *NamePtr;
    uint32                          OffsetVal;

    NamePtr = CFE_CONFIG_IDNAME_MAP;
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
 * Function: CFE_Config_GetIdByName
 *
 * Defined per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_Config_IterateAll(void *Arg, CFE_Config_Callback_t Callback)
{
    const CFE_Config_IdNameEntry_t *NamePtr;
    uint32                          OffsetVal;

    NamePtr = CFE_CONFIG_IDNAME_MAP;
    for (OffsetVal = 0; OffsetVal < CFE_ConfigIdOffset_MAX; ++OffsetVal)
    {
        if (CFE_Config_Global.Table[OffsetVal].ActualType != CFE_ConfigType_UNDEFINED)
        {
            Callback(Arg, CFE_Config_OffsetToId(OffsetVal), NamePtr->Name);
        }
        ++NamePtr;
    }
}
