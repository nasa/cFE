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
 *  Title:   Table Services API Application Library Header File
 *
 *  Purpose:
 *     Unit specification for Table services library functions and macros.
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 *  Notes:
 *
 */

#ifndef CFE_TBL_API_TYPEDEFS_H
#define CFE_TBL_API_TYPEDEFS_H

/********************* Include Files  ************************/
#include "common_types.h" /* Basic Data Types */
#include "cfe_mission_cfg.h"
#include "cfe_tbl_extern_typedefs.h"
#include "cfe_time_extern_typedefs.h"

/** @defgroup CFETBLTypeOptions cFE Table Type Defines
 * @{
 */
#define CFE_TBL_OPT_BUFFER_MSK  (0x0001) /**< \brief Table buffer mask */
#define CFE_TBL_OPT_SNGL_BUFFER (0x0000) /**< \brief Single buffer table */
#define CFE_TBL_OPT_DBL_BUFFER  (0x0001) /**< \brief Double buffer table */

#define CFE_TBL_OPT_LD_DMP_MSK (0x0002) /**< \brief Table load/dump mask */
#define CFE_TBL_OPT_LOAD_DUMP  (0x0000) /**< \brief Load/Dump table */
#define CFE_TBL_OPT_DUMP_ONLY  (0x0002) /**< \brief Dump only table */

#define CFE_TBL_OPT_USR_DEF_MSK (0x0004) /**< \brief Table user defined mask */
#define CFE_TBL_OPT_NOT_USR_DEF (0x0000) /**< \brief Not user defined table */
#define CFE_TBL_OPT_USR_DEF_ADDR \
    (0x0006) /**< \brief User Defined table, @note Automatically includes #CFE_TBL_OPT_DUMP_ONLY option */

#define CFE_TBL_OPT_CRITICAL_MSK (0x0008) /**< \brief Table critical mask */
#define CFE_TBL_OPT_NOT_CRITICAL (0x0000) /**< \brief Not critical table */
#define CFE_TBL_OPT_CRITICAL     (0x0008) /**< \brief Critical table */

/** @brief Default table options */
#define CFE_TBL_OPT_DEFAULT (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP)
/**@}*/

/**
 * \brief Table maximum full name length
 *
 * The full length of table names is defined at the mission scope.
 * This is defined here to support applications that depend on cfe_tbl.h
 * providing this value.
 */
#define CFE_TBL_MAX_FULL_NAME_LEN (CFE_MISSION_TBL_MAX_FULL_NAME_LEN)

/** \brief Bad table handle */
#define CFE_TBL_BAD_TABLE_HANDLE (CFE_TBL_Handle_t)0xFFFF

/******************  Data Type Definitions *********************/

/** \brief Table Callback Function */
typedef int32 (*CFE_TBL_CallbackFuncPtr_t)(void *TblPtr);

/** \brief Table Handle primitive */
typedef int16 CFE_TBL_Handle_t;

/** \brief Table Source */
typedef enum CFE_TBL_SrcEnum
{
    CFE_TBL_SRC_FILE = 0, /**< \brief File source
                               When this option is selected, the \c SrcDataPtr
                               will be interpreted as a pointer to a null
                               terminated character string.  The string should
                               specify the full path and filename of the file
                               containing the initial data contents of the table. */
    CFE_TBL_SRC_ADDRESS   /**< \brief Address source
                               When this option is selected, the \c SrcDataPtr will
                               be interpreted as a pointer to a memory location
                               that is the beginning of the initialization data
                               for loading the table OR, in the case of a "user defined"
                               dump only table, the address of the active table itself.
                               The block of memory is assumed to be of the same size
                               specified in the #CFE_TBL_Register function Size parameter. */
} CFE_TBL_SrcEnum_t;

/** \brief Table Info */
typedef struct CFE_TBL_Info
{
    size_t             Size;             /**< \brief Size, in bytes, of Table */
    uint32             NumUsers;         /**< \brief Number of Apps with access to the table */
    CFE_TIME_SysTime_t FileTime;         /**< \brief File creation time from last file loaded into table */
    uint32             Crc;              /**< \brief Most recently calculated CRC by TBL services on table contents */
    CFE_TIME_SysTime_t TimeOfLastUpdate; /**< \brief Time when Table was last updated */
    bool               TableLoadedOnce;  /**< \brief Flag indicating whether table has been loaded once or not */
    bool               DumpOnly;         /**< \brief Flag indicating Table is NOT to be loaded */
    bool               DoubleBuffered;   /**< \brief Flag indicating Table has a dedicated inactive buffer */
    bool               UserDefAddr;      /**< \brief Flag indicating Table address was defined by Owner Application */
    bool               Critical;         /**< \brief Flag indicating Table contents are maintained in a CDS */
    char               LastFileLoaded[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Filename of last file loaded into table */
} CFE_TBL_Info_t;

#endif /* CFE_TBL_API_TYPEDEFS_H */
