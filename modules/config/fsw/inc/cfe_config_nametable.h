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

#ifndef CFE_CONFIG_NAMETABLE_H
#define CFE_CONFIG_NAMETABLE_H

/*
** Includes
*/
#include "cfe_configid_offset.h"

typedef struct CFE_Config_IdNameEntry
{
    const char *Name;
} CFE_Config_IdNameEntry_t;

extern const CFE_Config_IdNameEntry_t CFE_CONFIGID_NAMETABLE[];

#endif /* CFE_CONFIG_MAP_H */
