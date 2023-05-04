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
 * Declarations and prototypes for cfe_tbl_extern_typedefs module
 */

#ifndef CFE_TBL_EXTERN_TYPEDEFS_H
#define CFE_TBL_EXTERN_TYPEDEFS_H

#include "common_types.h"
#include "cfe_es_extern_typedefs.h"
#include "cfe_mission_cfg.h" /* for CFE_MISSION_TBL_MAX_FULL_NAME_LEN */

/**
 * @brief Label definitions associated with CFE_TBL_BufferSelect_Enum_t
 */
enum CFE_TBL_BufferSelect
{
    /**
     * @brief Select the Inactive buffer for validate or dump
     */
    CFE_TBL_BufferSelect_INACTIVE = 0,

    /**
     * @brief Select the Active buffer for validate or dump
     */
    CFE_TBL_BufferSelect_ACTIVE = 1
};

/**
 * @brief Selects the buffer to operate on for validate or dump commands
 *
 * @sa enum CFE_TBL_BufferSelect
 */
typedef uint16 CFE_TBL_BufferSelect_Enum_t;

/**
 * @brief The definition of the header fields that are included in CFE Table Data files.
 *
 * This header follows the CFE_FS header and precedes the actual table data.
 *
 * @note The Offset and NumBytes fields in the table header are to 32 bits for
 * backward compatibility with existing CFE versions.  This means that even on
 * 64-bit CPUs, individual table files will be limited to 4GiB in size.
 */
typedef struct CFE_TBL_File_Hdr
{
    uint32 Reserved;                                     /**< Future Use: NumTblSegments in File?   */
    uint32 Offset;                                       /**< Byte Offset at which load should commence */
    uint32 NumBytes;                                     /**< Number of bytes to load into table */
    char   TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< Fully qualified name of table to load */
} CFE_TBL_File_Hdr_t;

#endif /* CFE_TBL_EXTERN_TYPEDEFS_H */
