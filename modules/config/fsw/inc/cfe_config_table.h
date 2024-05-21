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
    CFE_ConfigType_ARRAY,   /**< Value is a combination of length and pointer */
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
