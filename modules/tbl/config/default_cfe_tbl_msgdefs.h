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
 *   Specification for the CFE Event Services (CFE_TBL) command and telemetry
 *   message constant definitions.
 *
 *  For CFE_TBL this is only the function/command code definitions
 */
#ifndef CFE_TBL_MSGDEFS_H
#define CFE_TBL_MSGDEFS_H

#include "common_types.h"
#include "cfe_mission_cfg.h"
#include "cfe_es_extern_typedefs.h"
#include "cfe_time_extern_typedefs.h"
#include "cfe_tbl_extern_typedefs.h"
#include "cfe_tbl_fcncodes.h"

/********************************/
/* Command Message Data Formats */
/********************************/

/**
** \brief Load Table Command Payload
**
** For command details, see #CFE_TBL_LOAD_CC
**
**/
typedef struct CFE_TBL_LoadCmd_Payload
{
    char LoadFilename[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Filename (and path) of data to be loaded */
} CFE_TBL_LoadCmd_Payload_t;

/**
** \brief Dump Table Command Payload
**
** For command details, see #CFE_TBL_DUMP_CC
*/
typedef struct CFE_TBL_DumpCmd_Payload
{
    CFE_TBL_BufferSelect_Enum_t ActiveTableFlag;       /**< \brief #CFE_TBL_BufferSelect_INACTIVE=Inactive Table,
                                                            #CFE_TBL_BufferSelect_ACTIVE=Active Table */
                                                       /**< Selects either the "Inactive"
                                                            (#CFE_TBL_BufferSelect_INACTIVE) buffer or the
                                                            "Active" (#CFE_TBL_BufferSelect_ACTIVE) buffer
                                                            to be dumped */
    char TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full name of table to be dumped */
                                                       /**< ASCII string containing full table name
                                                            identifier of table to be dumped */
    char DumpFilename[CFE_MISSION_MAX_PATH_LEN];       /**< \brief Full Filename where data is to be written */
                                                       /**< ASCII string containing full path of filename
                                                            where data is to be dumped */
} CFE_TBL_DumpCmd_Payload_t;

/**
** \brief Validate Table Command Payload
**
** For command details, see #CFE_TBL_VALIDATE_CC
*/
typedef struct CFE_TBL_ValidateCmd_Payload
{
    CFE_TBL_BufferSelect_Enum_t ActiveTableFlag;       /**< \brief #CFE_TBL_BufferSelect_INACTIVE=Inactive Table,
                                                            #CFE_TBL_BufferSelect_ACTIVE=Active Table */
                                                       /**< Selects either the "Inactive"
                                                            (#CFE_TBL_BufferSelect_INACTIVE) buffer or the
                                                            "Active" (#CFE_TBL_BufferSelect_ACTIVE) buffer
                                                            to be validated */
    char TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full Name of Table to be validated */
                                                       /**< ASCII string containing full table name
                                                            identifier of table to be validated */
} CFE_TBL_ValidateCmd_Payload_t;

/**
** \brief Activate Table Command Payload
**
** For command details, see #CFE_TBL_ACTIVATE_CC
*/
typedef struct CFE_TBL_ActivateCmd_Payload
{
    char TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full Name of Table to be activated */
                                                       /**< ASCII string containing full table name
                                                            identifier of table to be activated */
} CFE_TBL_ActivateCmd_Payload_t;

/**
** \brief Dump Registry Command Payload
**
** For command details, see #CFE_TBL_DUMP_REGISTRY_CC
*/
typedef struct CFE_TBL_DumpRegistryCmd_Payload
{
    char DumpFilename[CFE_MISSION_MAX_PATH_LEN]; /**< \brief Full Filename where dumped data is to be
                                             written */
                                                 /**< ASCII string containing full path of filename
                                                      where registry is to be dumped */
} CFE_TBL_DumpRegistryCmd_Payload_t;

/**
** \brief Send Table Registry Command Payload
**
** For command details, see #CFE_TBL_SEND_REGISTRY_CC
*/
typedef struct CFE_TBL_SendRegistryCmd_Payload
{
    char TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full Name of Table whose registry entry
                                                    is to be telemetered */
                                                       /**< ASCII string containing full table name
                                                            identifier of table whose registry entry is
                                                            to be telemetered via #CFE_TBL_TableRegistryTlm_t */
} CFE_TBL_SendRegistryCmd_Payload_t;

/**
** \brief Delete Critical Table CDS Command Payload
**
** For command details, see #CFE_TBL_DELETE_CDS_CC
*/
typedef struct CFE_TBL_DelCDSCmd_Payload
{
    char TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full Name of Table whose CDS is
                                                    to be deleted */
                                                       /**< ASCII string containing full table name
                                                            identifier of a critical table whose
                                                            CDS is to be deleted */
} CFE_TBL_DelCDSCmd_Payload_t;

/**
** \brief Abort Load Command Payload
**
** For command details, see #CFE_TBL_ABORT_LOAD_CC
*/
typedef struct CFE_TBL_AbortLoadCmd_Payload
{
    char TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \brief Full Name of Table whose load is to be aborted */
                                                       /**< ASCII string containing full table name
                                                            identifier of a table whose load is to be aborted */
} CFE_TBL_AbortLoadCmd_Payload_t;

/*************************************************************************/

/**
** \brief Table Management Notification Command Payload
**
** \par Description
**      Whenever an application that owns a table calls the #CFE_TBL_NotifyByMessage API
**      following the table registration, Table services will generate the following
**      command message with the application specified message ID, command code and
**      parameter whenever the table requires management (e.g. - loads and validations).
*/
typedef struct CFE_TBL_NotifyCmd_Payload
{
    uint32 Parameter; /**< \brief Application specified command parameter */
} CFE_TBL_NotifyCmd_Payload_t;

/*************************************************************************/

/**********************************/
/* Telemetry Message Data Formats */
/**********************************/
/**
**  \cfetbltlm Table Services Housekeeping Packet
**/
typedef struct CFE_TBL_HousekeepingTlm_Payload
{
    /*
    ** Task command interface counters...
    */
    uint8 CommandCounter;      /**< \cfetlmmnemonic \TBL_CMDPC
                                \brief Count of valid commands received */
    uint8 CommandErrorCounter; /**< \cfetlmmnemonic \TBL_CMDEC
                           \brief Count of invalid commands received */

    /*
    ** Table Registry Statistics
    */
    uint16 NumTables;      /**< \cfetlmmnemonic \TBL_NUMTABLES
                                \brief Number of Tables Registered */
    uint16 NumLoadPending; /**< \cfetlmmnemonic \TBL_NUMUPDATESPEND
                                \brief Number of Tables pending on Applications for their update */

    /*
    ** Last Table Validation Results
    */
    uint16 ValidationCounter;                                 /**< \cfetlmmnemonic \TBL_VALCOMPLTDCTR
                                                               \brief Number of completed table validations */
    uint32 LastValCrc;                                        /**< \cfetlmmnemonic \TBL_LASTVALCRC
                                                                   \brief Data Integrity Value computed for last table validated */
    int32 LastValStatus;                                      /**< \cfetlmmnemonic \TBL_LASTVALS
                                                                   \brief Returned status from validation function for last table validated */
    bool ActiveBuffer;                                        /**< \cfetlmmnemonic \TBL_LASTVALBUF
                                                                   \brief Indicator of whether table buffer validated was 0=Inactive, 1=Active */
    char LastValTableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTVALTBLNAME
                                                           \brief Name of last table validated */
    uint8 SuccessValCounter;                                  /**< \cfetlmmnemonic \TBL_VALSUCCESSCTR
                                                               \brief Total number of successful table validations */
    uint8 FailedValCounter;                                   /**< \cfetlmmnemonic \TBL_VALFAILEDCTR
                                                               \brief Total number of unsuccessful table validations */
    uint8 NumValRequests;                                     /**< \cfetlmmnemonic \TBL_VALREQCTR
                                                                   \brief Number of times Table Services has requested validations from Apps */

    /*
    ** Ground system interface information
    */
    uint8 NumFreeSharedBufs;                                  /**< \cfetlmmnemonic \TBL_NUMFREESHRBUF
                                                                   \brief Number of free Shared Working Buffers */
    uint8 ByteAlignPad1;                                      /**< \cfetlmmnemonic \TBL_BYTEALIGNPAD1
                                                                   \brief Spare byte to ensure longword alignment */
    CFE_ES_MemHandle_t MemPoolHandle;                         /**< \cfetlmmnemonic \TBL_MEMPOOLHANDLE
                                                                   \brief Handle to TBL's memory pool */
    CFE_TIME_SysTime_t LastUpdateTime;                        /**< \cfetlmmnemonic \TBL_LASTUPDTIME
                                                                   \brief Time of last table update */
    char LastUpdatedTable[CFE_MISSION_TBL_MAX_FULL_NAME_LEN]; /**< \cfetlmmnemonic \TBL_LASTUPDTBLNAME
                                                         \brief Name of the last table updated */
    char LastFileLoaded[CFE_MISSION_MAX_PATH_LEN];            /**< \cfetlmmnemonic \TBL_LASTFILELOADED
                                                          \brief Path and Name of last table image file loaded */
    char LastFileDumped[CFE_MISSION_MAX_PATH_LEN];            /**< \cfetlmmnemonic \TBL_LASTFILEDUMPED
                                                          \brief Path and Name of last file dumped to */
    char LastTableLoaded[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];  /**< \cfetlmmnemonic \TBL_LASTTABLELOADED
                                                          \brief Name of the last table loaded */
} CFE_TBL_HousekeepingTlm_Payload_t;

/**
**  \cfetbltlm Table Registry Info Packet
**/
typedef struct CFE_TBL_TblRegPacket_Payload
{
    CFE_ES_MemOffset_t Size;                       /**< \cfetlmmnemonic \TBL_SIZE
                                                        \brief Size, in bytes, of Table */
    uint32 Crc;                                    /**< \cfetlmmnemonic \TBL_CRC
                                                        \brief Most recently calculated CRC of Table */
    CFE_ES_MemAddress_t ActiveBufferAddr;          /**< \cfetlmmnemonic \TBL_ACTBUFADD
                                                        \brief Address of Active Buffer */
    CFE_ES_MemAddress_t InactiveBufferAddr;        /**< \cfetlmmnemonic \TBL_IACTBUFADD
                                                        \brief Address of Inactive Buffer */
    CFE_ES_MemAddress_t ValidationFuncPtr;         /**< \cfetlmmnemonic \TBL_VALFUNCPTR
                                                        \brief Ptr to Owner App's function that validates tbl contents */
    CFE_TIME_SysTime_t TimeOfLastUpdate;           /**< \cfetlmmnemonic \TBL_TIMELASTUPD
                                                        \brief Time when Table was last updated */
    CFE_TIME_SysTime_t FileTime;                   /**< \cfetlmmnemonic \TBL_FILECTIME
                                                        \brief File creation time from last file loaded into table */
    bool TableLoadedOnce;                          /**< \cfetlmmnemonic \TBL_LOADEDONCE
                                                        \brief Flag indicating whether table has been loaded once or not */
    bool LoadPending;                              /**< \cfetlmmnemonic \TBL_UPDATEPNDNG
                                                        \brief Flag indicating an inactive buffer is ready to be copied */
    bool DumpOnly;                                 /**< \cfetlmmnemonic \TBL_DUMPONLY
                                                        \brief Flag indicating Table is NOT to be loaded */
    bool DoubleBuffered;                           /**< \cfetlmmnemonic \TBL_DBLBUFFERED
                                                     \brief Flag indicating Table has a dedicated inactive buffer */
    char Name[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];  /**< \cfetlmmnemonic \TBL_NAME
                                                \brief Processor specific table name */
    char LastFileLoaded[CFE_MISSION_MAX_PATH_LEN]; /**< \cfetlmmnemonic \TBL_LASTFILEUPD
                                               \brief Filename of last file loaded into table */
    char OwnerAppName[CFE_MISSION_MAX_API_LEN];    /**< \cfetlmmnemonic \TBL_OWNERAPP
                                                \brief Name of owning application */
    bool Critical;                                 /**< \cfetlmmnemonic \TBL_CRITICAL
                                                        \brief Indicates whether table is Critical or not */
    uint8 ByteAlign4;                              /**< \cfetlmmnemonic \TBL_SPARE4
                                                        \brief Spare byte to maintain byte alignment */
} CFE_TBL_TblRegPacket_Payload_t;

#endif
