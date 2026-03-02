/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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
 *   CFE File Services (CFE_FS) Module Public Definitions
 *
 * This provides default values for configurable items that affect
 * the interface(s) of this module.
 *
 * @note This file may be overridden/superceded by mission-provided definitionsm
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */

#ifndef CFE_FS_INTERFACE_CFG_H
#define CFE_FS_INTERFACE_CFG_H

#include "cfe_fs_interface_cfg_values.h"

/******************* Macro Definitions ***********************/

/**
 * \brief Max length of description field in a standard cFE File Header
 *
 * \note the value of CFE_FS_HDR_DESC_MAX_LEN, if modified, should
 * be constrained to multiples of 4, as it is used within a structure that
 * also contains uint32 types.  This ensures that the entire structure
 * remains 32-bit aligned without the need for implicit padding bytes.
 */
#define CFE_FS_HDR_DESC_MAX_LEN         CFE_MISSION_FS_CFGVAL(HDR_DESC_MAX_LEN)
#define DEFAULT_CFE_FS_HDR_DESC_MAX_LEN 32

/**
 * \brief Magic Number for cFE compliant files (= 'cFE1')
 */
#define CFE_FS_FILE_CONTENT_ID         CFE_MISSION_FS_CFGVAL(FILE_CONTENT_ID)
#define DEFAULT_CFE_FS_FILE_CONTENT_ID 0x63464531

#endif /* CFE_FS_INTERFACE_CFG_H */
