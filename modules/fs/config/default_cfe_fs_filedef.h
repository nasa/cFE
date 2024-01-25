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
 * Declarations and prototypes for cfe_fs_extern_typedefs module
 */

#ifndef CFE_FS_FILEHDR_H
#define CFE_FS_FILEHDR_H

#include "common_types.h"
#include "cfe_fs_interface_cfg.h" /* to define CFE_FS_HDR_DESC_MAX_LEN */

/**
 * @brief File subtypes used within cFE
 *
 * This defines all the file subtypes used by cFE.
 * Note apps can extend as needed but need to
 * avoid conflicts (app context not currently included
 * in the file header).
 */
enum CFE_FS_SubType
{
    /**
     * @brief Executive Services Exception/Reset Log Type
     *
     * Executive Services Exception/Reset Log File which is generated in response to a
     * \link #CFE_ES_WRITE_ER_LOG_CC \ES_WRITEERLOG2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_ES_ERLOG = 1,

    /**
     * @brief Executive Services System Log Type
     *
     * Executive Services System Log File which is generated in response to a
     * \link #CFE_ES_WRITE_SYS_LOG_CC \ES_WRITESYSLOG2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_ES_SYSLOG = 2,

    /**
     * @brief Executive Services Information on All Applications File
     *
     * Executive Services Information on All Applications File which is generated in response to a
     * \link #CFE_ES_QUERY_ALL_CC \ES_WRITEAPPINFO2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_ES_QUERYALL = 3,

    /**
     * @brief Executive Services Performance Data File
     *
     * Executive Services Performance Analyzer Data File which is generated in response to a
     * \link #CFE_ES_STOP_PERF_DATA_CC \ES_STOPLADATA \endlink
     * command.
     *
     */
    CFE_FS_SubType_ES_PERFDATA = 4,

    /**
     * @brief Executive Services Critical Data Store Registry Dump File
     *
     * Executive Services Critical Data Store Registry Dump File which is generated in response to a
     * \link #CFE_ES_DUMP_CDS_REGISTRY_CC \ES_DUMPCDSREG \endlink
     * command.
     *
     */
    CFE_FS_SubType_ES_CDS_REG = 6,

    /**
     * @brief Table Services Registry Dump File
     *
     * Table Services Registry Dump File which is generated in response to a
     * \link #CFE_TBL_DUMP_REGISTRY_CC \TBL_WRITEREG2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_TBL_REG = 9,

    /**
     * @brief Table Services Table Image File
     *
     * Table Services Table Image File which is generated either on the ground or in response to a
     * \link #CFE_TBL_DUMP_CC \TBL_DUMP \endlink command.
     *
     */
    CFE_FS_SubType_TBL_IMG = 8,

    /**
     * @brief Event Services Application Data Dump File
     *
     * Event Services Application Data Dump File which is generated in response to a
     * \link #CFE_EVS_WRITE_APP_DATA_FILE_CC \EVS_WRITEAPPDATA2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_EVS_APPDATA = 15,

    /**
     * @brief Event Services Local Event Log Dump File
     *
     * Event Services Local Event Log Dump File which is generated in response to a
     * \link  #CFE_EVS_WRITE_LOG_DATA_FILE_CC \EVS_WRITELOG2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_EVS_EVENTLOG = 16,

    /**
     * @brief Software Bus Pipe Data Dump File
     *
     * Software Bus Pipe Data Dump File which is generated in response to a
     * \link #CFE_SB_WRITE_PIPE_INFO_CC \SB_WRITEPIPE2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_SB_PIPEDATA = 20,

    /**
     * @brief Software Bus Message Routing Data Dump File
     *
     * Software Bus Message Routing Data Dump File which is generated in response to a
     * \link #CFE_SB_WRITE_ROUTING_INFO_CC \SB_WRITEROUTING2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_SB_ROUTEDATA = 21,

    /**
     * @brief Software Bus Message Mapping Data Dump File
     *
     * Software Bus Message Mapping Data Dump File which is generated in response to a
     * \link #CFE_SB_WRITE_MAP_INFO_CC \SB_WRITEMAP2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_SB_MAPDATA = 22,

    /**
     * @brief Executive Services Query All Tasks Data File
     *
     * Executive Services Query All Tasks Data File which is generated in response to a
     * \link #CFE_ES_QUERY_ALL_TASKS_CC \ES_WRITETASKINFO2FILE \endlink
     * command.
     *
     */
    CFE_FS_SubType_ES_QUERYALLTASKS = 23
};

/**
 * @brief Content descriptor for File Headers
 *
 * @sa enum CFE_FS_SubType
 */
typedef uint32 CFE_FS_SubType_Enum_t;

/**
** \brief Standard cFE File header structure definition
*/
typedef struct CFE_FS_Header
{
    uint32 ContentType;   /**< \brief Identifies the content type (='cFE1'=0x63464531)*/
    uint32 SubType;       /**< \brief Type of \c ContentType, if necessary */
                          /**< Standard SubType definitions can be found
                               \link #CFE_FS_SubType_ES_ERLOG here \endlink */
    uint32 Length;        /**< \brief Length of this header to support external processing */
    uint32 SpacecraftID;  /**< \brief Spacecraft that generated the file */
    uint32 ProcessorID;   /**< \brief Processor that generated the file */
    uint32 ApplicationID; /**< \brief Application that generated the file */

    uint32 TimeSeconds;    /**< \brief File creation timestamp (seconds) */
    uint32 TimeSubSeconds; /**< \brief File creation timestamp (sub-seconds) */

    char Description[CFE_FS_HDR_DESC_MAX_LEN]; /**< \brief File description */
} CFE_FS_Header_t;

#endif /* CFE_FS_EXTERN_TYPEDEFS_H */
