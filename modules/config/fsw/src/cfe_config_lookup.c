/*
**  File:
**    cfe_es_api.c
**
**  Purpose:
**    This file implements the cFE Executive Services API functions.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
**
**  Notes:
**
*/

/*
** Required header files.
*/
#include "cfe_config_priv.h"

#include "cfe_resourceid.h"
#include "cfe_core_resourceid_basevalues.h"

/*
 * Instantiate the global table
 */
CFE_Config_Global_t CFE_Config_Global;

/*----------------------------------------------------------------
 *
 * Function: CFE_Config_LocateConfigRecordByID
 *
 * Application scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Config_ValueEntry_t *CFE_Config_LocateConfigRecordByID(CFE_ConfigId_t ConfigId)
{
    uint32 OffsetVal;

    OffsetVal = CFE_Config_IdToOffset(ConfigId);

    if (OffsetVal >= CFE_ConfigIdOffset_MAX)
    {
        return NULL;
    }

    return &CFE_Config_Global.Table[OffsetVal];
}
