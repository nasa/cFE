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
 * Function: CFE_Config_SetValue
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
 * Function: CFE_Config_SetObjPointer
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
 * Function: CFE_Config_SetString
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
