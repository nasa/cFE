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
 *  Title:    cFE Status Code Definition Header File
 *
 *  Purpose:
 *            Common source of cFE API return status codes.
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 */

#ifndef CFE_CONFIG_API_TYPEDEFS_H
#define CFE_CONFIG_API_TYPEDEFS_H

/* Include Files */
#include "common_types.h"
#include "cfe_resourceid_api_typedefs.h"

/**
 * @brief A type for Configuration IDs
 *
 * This is the type that is used for any API accepting or returning a configuration key ID
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ConfigId_t;

#define CFE_CONFIGID_C(val)    ((CFE_ConfigId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_CONFIGID_UNDEFINED CFE_CONFIGID_C(CFE_RESOURCEID_UNDEFINED)

typedef void (*CFE_Config_Callback_t)(void *Arg, CFE_ConfigId_t Id, const char *Name);

/**
 * @brief Wrapper type for array configuration
 *
 * This is a pair containing a size and pointer that is get/set
 * via a single config table entry
 */
typedef struct CFE_Config_ArrayValue
{
    size_t      NumElements;
    const void *ElementPtr;
} CFE_Config_ArrayValue_t;

#endif /* CFE_CONFIG_API_TYPEDEFS_H */
