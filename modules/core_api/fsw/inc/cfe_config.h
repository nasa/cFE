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

#ifndef CFE_CONFIG_H
#define CFE_CONFIG_H

/* Include Files */
#include "common_types.h"
#include "cfe_config_api_typedefs.h"
#include "cfe_config_ids.h"

/**
 * @brief Obtain an integer value correlating to an CFE configuration ID
 *
 * Retreives the integer value associated with the specified key.
 *
 * If no value has been set, or the key is not valid, this returns 0.
 *
 * @param[in]   ConfigId  Configuration ID/Key to look up
 *
 * @return Value associated with key
 * @retval 0 if key is not defined or not set
 */
uint32 CFE_Config_GetValue(CFE_ConfigId_t ConfigId);

/**
 * @brief Obtain a pointer value correlating to an CFE configuration ID
 *
 * Retreives the pointer value associated with the specified key.
 *
 * If no value has been set, or the key is not valid, this returns NULL.
 *
 * @param[in]   ConfigId  Configuration ID/Key to look up
 *
 * @return Value associated with key
 * @retval NULL if key is not defined or not set
 */
const void *CFE_Config_GetObjPointer(CFE_ConfigId_t ConfigId);

/**
 * @brief Obtain a string value correlating to an CFE configuration ID
 *
 * Retreives the string value associated with the specified key.
 *
 * If no value has been set, or the key is not valid, this returns the
 * special string "UNDEFINED"
 *
 * @note This function does not return NULL, so it can be used directly
 * in printf-style calls.
 *
 * @param[in]   ConfigId  Configuration ID/Key to look up
 *
 * @return String value associated with key
 */
const char *CFE_Config_GetString(CFE_ConfigId_t ConfigId);

/**
 * @brief Obtain the name of a CFE configuration ID
 *
 * Retreives the printable name associated with the specified key.
 *
 * @note  This function does not return NULL.
 *
 * If the ID is not valid/known, then the implementation returns the
 * special string '[unknown]' rather than NULL, so this function may
 * be more easily used in printf() style calls.
 *
 * @param[in]   ConfigId  Configuration ID/Key to look up
 *
 * @return Name associated with key
 */
const char *CFE_Config_GetName(CFE_ConfigId_t ConfigId);

/**
 * @brief Obtain the ID value associated with a configuration name
 *
 * @param[in]   Name  The name of the ID to look up
 *
 * @return ID associated with name
 * @retval CFE_CONFIGID_UNDEFINED if the name did not correspond to a key
 */
CFE_ConfigId_t CFE_Config_GetIdByName(const char *Name);

/**
 * @brief Iterate all known name/ID value pairs
 *
 * @param[in]   Arg       User-supplied opaque argument to pass to callback
 * @param[in]   Callback  User-supplied callback function to invoke for each ID
 */
void CFE_Config_IterateAll(void *Arg, CFE_Config_Callback_t Callback);

#endif /* CFE_CONFIG_H */
