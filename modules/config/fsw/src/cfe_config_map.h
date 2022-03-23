/**
 * @file
 *
 * This file contains the CFE configuration registry global data definitions.
 */

#ifndef CFE_CONFIG_MAP_H
#define CFE_CONFIG_MAP_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_config_ids.h"

typedef struct CFE_Config_IdNameEntry
{
    const char *Name;
} CFE_Config_IdNameEntry_t;

extern const CFE_Config_IdNameEntry_t CFE_CONFIG_IDNAME_MAP[];

#endif /* CFE_CONFIG_MAP_H */
