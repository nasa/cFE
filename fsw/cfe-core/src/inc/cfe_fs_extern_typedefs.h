/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

#ifndef _CFE_FS_EXTERN_TYPEDEFS_H_
#define _CFE_FS_EXTERN_TYPEDEFS_H_

/* This header may be generated from an EDS file, 
 * tools are available and the feature is enabled */
#ifdef CFE_EDS_ENABLED_BUILD

/* Use the EDS generated version of these types */
#include "cfe_fs_eds_typedefs.h"

#else
/* Use the local definitions of these types */

#include "common_types.h"

/******************* Macro Definitions ***********************/

/*
 * NOTE: the value of CFE_FS_HDR_DESC_MAX_LEN, if modified, should
 * be constrained to multiples of 4, as it is used within a structure that
 * also contains uint32 types.  This ensures that the entire structure
 * remains 32-bit aligned without the need for implicit padding bytes.
 */

#define CFE_FS_HDR_DESC_MAX_LEN 32          /**< \brief Max length of description field in a standard cFE File Header */

#define CFE_FS_FILE_CONTENT_ID  0x63464531  /**< \brief Magic Number for cFE compliant files (= 'cFE1') */



/**
 * @brief Label definitions associated with CFE_FS_SubType_Enum_t
 */
enum CFE_FS_SubType
{

   /**
    * @brief Executive Services Exception/Reset Log Type
    *
    *
    * Executive Services Exception/Reset Log File which is generated in response to a
    * \link #CFE_ES_WRITE_ER_LOG_CC \ES_WRITEERLOG2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_ES_ERLOG                            = 1,

   /**
    * @brief Executive Services System Log Type
    *
    *
    * Executive Services System Log File which is generated in response to a
    * \link #CFE_ES_WRITE_SYSLOG_CC \ES_WRITESYSLOG2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_ES_SYSLOG                           = 2,

   /**
    * @brief Executive Services Information on All Applications File
    *
    *
    * Executive Services Information on All Applications File which is generated in response to a
    * \link #CFE_ES_QUERY_ALL_CC \ES_WRITEAPPINFO2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_ES_QUERYALL                         = 3,

   /**
    * @brief Executive Services Performance Data File
    *
    *
    * Executive Services Performance Analyzer Data File which is generated in response to a
    * \link #CFE_ES_STOP_PERF_DATA_CC \ES_STOPLADATA \endlink
    * command.
    *
    */
   CFE_FS_SubType_ES_PERFDATA                         = 4,

   /**
    * @brief Executive Services Shell Response File
    *
    *
    * Executive Services Shell Response Data File which is generated in response to a
    * \link #CFE_ES_SHELL_CC \ES_SHELL \endlink
    * command.
    *
    */
   CFE_FS_SubType_ES_SHELL                            = 5,

   /**
    * @brief Executive Services Critical Data Store Registry Dump File
    *
    *
    * Executive Services Critical Data Store Registry Dump File which is generated in response to a
    * \link #CFE_ES_DUMP_CDS_REGISTRY_CC \ES_DUMPCDSREG \endlink
    * command.
    *
    */
   CFE_FS_SubType_ES_CDS_REG                          = 6,

   /**
    * @brief Table Services Registry Dump File
    *
    *
    * Table Services Registry Dump File which is generated in response to a
    * \link #CFE_TBL_DUMP_REGISTRY_CC \TBL_WRITEREG2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_TBL_REG                             = 9,

   /**
    * @brief Table Services Table Image File
    *
    *
    * Table Services Table Image File which is generated either on the ground or in response to a
    * \link #CFE_TBL_DUMP_CC \TBL_DUMP \endlink command.
    *
    */
   CFE_FS_SubType_TBL_IMG                             = 8,

   /**
    * @brief Event Services Application Data Dump File
    *
    *
    * Event Services Application Data Dump File which is generated in response to a
    * \link #CFE_EVS_WRITE_APP_DATA_FILE_CC \EVS_WRITEAPPDATA2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_EVS_APPDATA                         = 15,

   /**
    * @brief Event Services Local Event Log Dump File
    *
    *
    * Event Services Local Event Log Dump File which is generated in response to a
    * \link  #CFE_EVS_WRITE_LOG_DATA_FILE_CC \EVS_WRITELOG2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_EVS_EVENTLOG                        = 16,

   /**
    * @brief Software Bus Pipe Data Dump File
    *
    *
    * Software Bus Pipe Data Dump File which is generated in response to a
    * \link #CFE_SB_SEND_PIPE_INFO_CC \SB_WRITEPIPE2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_SB_PIPEDATA                         = 20,

   /**
    * @brief Software Bus Message Routing Data Dump File
    *
    *
    * Software Bus Message Routing Data Dump File which is generated in response to a
    * \link #CFE_SB_SEND_ROUTING_INFO_CC \SB_WRITEROUTING2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_SB_ROUTEDATA                        = 21,

   /**
    * @brief Software Bus Message Mapping Data Dump File
    *
    *
    * Software Bus Message Mapping Data Dump File which is generated in response to a
    * \link #CFE_SB_SEND_MAP_INFO_CC \SB_WRITEMAP2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_SB_MAPDATA                          = 22,

   /**
    * @brief Executive Services Query All Tasks Data File
    *
    *
    * Executive Services Query All Tasks Data File which is generated in response to a
    * \link #CFE_ES_QUERY_ALL_TASKS_CC \ES_WRITETASKINFO2FILE \endlink
    * command.
    *
    */
   CFE_FS_SubType_ES_QUERYALLTASKS                    = 23
};

/**
 * @brief Content descriptor for File Headers
 *
 *
 * @sa enum CFE_FS_SubType
 */
typedef uint32                                           CFE_FS_SubType_Enum_t;


/**
** \brief Standard cFE File header structure definition
*/
typedef struct
{
    uint32  ContentType;           /**< \brief Identifies the content type (='cFE1'=0x63464531)*/
    uint32  SubType;               /**< \brief Type of \c ContentType, if necessary */
                                   /**< Standard SubType definitions can be found
                                        \link #CFE_FS_SubType_ES_ERLOG here \endlink */
    uint32  Length;                /**< \brief Length of primary header */
    uint32  SpacecraftID;          /**< \brief Spacecraft that generated the file */
    uint32  ProcessorID;           /**< \brief Processor that generated the file */
    uint32  ApplicationID;         /**< \brief Application that generated the file */

    uint32  TimeSeconds;           /**< \brief File creation timestamp (seconds) */
    uint32  TimeSubSeconds;        /**< \brief File creation timestamp (sub-seconds) */

    char    Description[CFE_FS_HDR_DESC_MAX_LEN];       /**< \brief File description */

} CFE_FS_Header_t;




#endif /* CFE_EDS_ENABLED_BUILD */

#endif /* _CFE_FS_EXTERN_TYPEDEFS_H_ */

