/**
 * @file
 *
 * This file contains the CFE configuration registry global data definitions.
 */

#ifndef CFE_CONFIG_TABLE_H
#define CFE_CONFIG_TABLE_H

/*
** Includes
*/
#include "common_types.h"
#include "cfe_config_ids.h"

typedef enum CFE_ConfigType
{
    CFE_ConfigType_UNDEFINED,
    CFE_ConfigType_VALUE,   /**< Value is an unsigned int */
    CFE_ConfigType_STRING,  /**< Value is a string pointer */
    CFE_ConfigType_POINTER, /**< Value is a non-string object pointer */
} CFE_ConfigType_t;

typedef union CFE_Config_ValueBuffer
{
    uint32      AsInteger;
    const void *AsPointer;
} CFE_Config_ValueBuffer_t;

typedef struct CFE_Config_ValueEntry
{
    CFE_ConfigType_t         ActualType;
    CFE_Config_ValueBuffer_t Datum;
} CFE_Config_ValueEntry_t;

#endif /* CFE_CONFIG_TABLE_H */
