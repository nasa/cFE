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

/*
**  File: cfe_tbl.h
**
**  Title:   Table Services API Application Library Header File
**
**  Purpose:
**     Unit specification for Table services library functions and macros.
**
**  Design Notes:
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**  Notes:
**
**/

#ifndef _cfe_tbl_
#define _cfe_tbl_

/********************* Include Files  ************************/
#include "cfe_tbl_extern_typedefs.h"
#include "cfe_sb_extern_typedefs.h"
#include "cfe_error.h"
#include "common_types.h"  /* Basic Data Types */
#include "cfe_time.h"
#include "osconfig.h"
#include "cfe_msg_typedefs.h"

/******************* Macro Definitions ***********************/

/** @defgroup CFETBLTypeOptions cFE Table Type Defines
 * @{
 */
#define CFE_TBL_OPT_BUFFER_MSK   (0x0001) /**< \brief Table buffer mask */
#define CFE_TBL_OPT_SNGL_BUFFER  (0x0000) /**< \brief Single buffer table */
#define CFE_TBL_OPT_DBL_BUFFER   (0x0001) /**< \brief Double buffer table */

#define CFE_TBL_OPT_LD_DMP_MSK   (0x0002) /**< \brief Table load/dump mask */
#define CFE_TBL_OPT_LOAD_DUMP    (0x0000) /**< \brief Load/Dump table */
#define CFE_TBL_OPT_DUMP_ONLY    (0x0002) /**< \brief Dump only table */

#define CFE_TBL_OPT_USR_DEF_MSK  (0x0004) /**< \brief Table user defined mask */
#define CFE_TBL_OPT_NOT_USR_DEF  (0x0000) /**< \brief Not user defined table */
#define CFE_TBL_OPT_USR_DEF_ADDR (0x0006) /**< \brief User Defined table, @note Automatically includes #CFE_TBL_OPT_DUMP_ONLY option */

#define CFE_TBL_OPT_CRITICAL_MSK (0x0008) /**< \brief Table critical mask */
#define CFE_TBL_OPT_NOT_CRITICAL (0x0000) /**< \brief Not critical table */
#define CFE_TBL_OPT_CRITICAL     (0x0008) /**< \brief Critical table */

/** @brief Default table options */
#define CFE_TBL_OPT_DEFAULT      (CFE_TBL_OPT_SNGL_BUFFER | CFE_TBL_OPT_LOAD_DUMP)
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
#define CFE_TBL_BAD_TABLE_HANDLE  (CFE_TBL_Handle_t) 0xFFFF




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
    size_t                Size;                             /**< \brief Size, in bytes, of Table */
    uint32                NumUsers;                         /**< \brief Number of Apps with access to the table */
    uint32                FileCreateTimeSecs;               /**< \brief File creation time from last file loaded into table */
    uint32                FileCreateTimeSubSecs;            /**< \brief File creation time from last file loaded into table */
    uint32                Crc;                              /**< \brief Most recently calculated CRC by TBL services on table contents */
    CFE_TIME_SysTime_t    TimeOfLastUpdate;                 /**< \brief Time when Table was last updated */
    bool                  TableLoadedOnce;                  /**< \brief Flag indicating whether table has been loaded once or not */
    bool                  DumpOnly;                         /**< \brief Flag indicating Table is NOT to be loaded */
    bool                  DoubleBuffered;                      /**< \brief Flag indicating Table has a dedicated inactive buffer */
    bool                  UserDefAddr;                      /**< \brief Flag indicating Table address was defined by Owner Application */
    bool                  Critical;                         /**< \brief Flag indicating Table contents are maintained in a CDS */
    char                  LastFileLoaded[CFE_MISSION_MAX_PATH_LEN];  /**< \brief Filename of last file loaded into table */
} CFE_TBL_Info_t;

/*************************** Function Prototypes ******************************/

/** \defgroup CFEAPITBLRegistration cFE Registration APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Register a table with cFE to obtain Table Management Services
**
** \par Description
**          When an application is created and initialized, it is responsible for creating its table images via 
**          the TBL API.  The application must inform the Table Service of the table name, table size and selection 
**          of optional table features.
**
** \par Assumptions, External Events, and Notes:
**          Note: This function call can block.  Therefore, interrupt service routines should NOT create 
**                their own tables.  An application should create any table(s) and provide the handle(s) 
**                to the interrupt service routine.
**
** \param[in, out] TblHandlePtr         a pointer to a #CFE_TBL_Handle_t type variable that will be assigned the 
**                                 table's handle.  The table handle is required for other API calls when 
**                                 accessing the data contained in the table. *TblHandlePtr is the handle used to identify table to cFE when performing Table operations.
**                                 This value is returned at the address specified by TblHandlePtr.
**
** \param[in] Name                 The application-specific name.  This name will be combined with the name of the 
**                                 application to produce a processor specific name of the form 
**                                 "ApplicationName.TableName".  The processor specific name will be used in commands 
**                                 for modifying or viewing the contents of the table.
**
** \param[in] Size                 The size, in bytes, of the table to be created.  This is the size that will be 
**                                 allocated as a shared memory resource between the Table Management Service and 
**                                 the calling application.
**
** \param[in] TblOptionFlags       Flag bits indicating selected options for table.  A bitwise OR of the following 
**                                 option flags:
**                                 \arg #CFE_TBL_OPT_DEFAULT     - The default setting for table options is a 
**                                                                 combination of #CFE_TBL_OPT_SNGL_BUFFER and 
**                                                                 #CFE_TBL_OPT_LOAD_DUMP.  See below for a 
**                                                                 description of these two options.  This option 
**                                                                 is mutually exclusive with the 
**                                                                 #CFE_TBL_OPT_DBL_BUFFER, #CFE_TBL_OPT_DUMP_ONLY 
**                                                                 and #CFE_TBL_OPT_USR_DEF_ADDR options.
**                                 \arg #CFE_TBL_OPT_SNGL_BUFFER - When this option is selected, the table will use 
**                                                                 a shared session table for performing table 
**                                                                 modifications and a memory copy from the session 
**                                                                 table to the "active" table buffer will occur 
**                                                                 when the table is updated.  This is the preferred 
**                                                                 option since it will minimize memory usage.  
**                                                                 This option is mutually exclusive with the 
**                                                                 #CFE_TBL_OPT_DBL_BUFFER option
**                                 \arg #CFE_TBL_OPT_DBL_BUFFER  - When this option is selected, two instances of 
**                                                                 the table are created.   One is considered the 
**                                                                 "active" table and the other the "inactive" table.
**                                                                 Whenever table modifications occur, they do not 
**                                                                 require the use of a common session table.  
**                                                                 Modifications occur in the "inactive" buffer.  
**                                                                 Then, when it is time to update the table, 
**                                                                 the pointer to the "active" table is changed to 
**                                                                 point to the "inactive" buffer thus making it 
**                                                                 the new "active" buffer.  This feature is most 
**                                                                 useful for time critical applications (ie - 
**                                                                 interrupt service routines, etc).  This option is 
**                                                                 mutually exclusive with the 
**                                                                 #CFE_TBL_OPT_SNGL_BUFFER and #CFE_TBL_OPT_DEFAULT 
**                                                                 option.
**                                 \arg #CFE_TBL_OPT_LOAD_DUMP   - When this option is selected, the Table Service 
**                                                                 is allowed to perform all operations on the 
**                                                                 specified table.  This option is mutually 
**                                                                 exclusive with the #CFE_TBL_OPT_DUMP_ONLY option.
**                                 \arg #CFE_TBL_OPT_DUMP_ONLY   - When this option is selected, the Table Service 
**                                                                 will not perform table loads to this table.  This 
**                                                                 does not prevent, however, a task from writing 
**                                                                 to the table via an address obtained with the 
**                                                                 #CFE_TBL_GetAddress API function.  This option is 
**                                                                 mutually exclusive with the #CFE_TBL_OPT_LOAD_DUMP
**                                                                 and #CFE_TBL_OPT_DEFAULT options. If the Application wishes
**                                                                 to specify their own block of memory as the Dump Only table,
**                                                                 they need to also include the #CFE_TBL_OPT_USR_DEF_ADDR option
**                                                                 explained below.
**                                 \arg #CFE_TBL_OPT_NOT_USR_DEF - When this option is selected, Table Services allocates
**                                                                 memory for the table and, in the case of a double buffered
**                                                                 table, it allocates the same amount of memory again for the
**                                                                 second buffer.  This option is mutually exclusive with
**                                                                 the #CFE_TBL_OPT_USR_DEF_ADDR option.
**                                 \arg #CFE_TBL_OPT_USR_DEF_ADDR- When this option is selected, the Table Service 
**                                                                 will not allocate memory for the table.  Table Services
**                                                                 will require the Application to identify the location of the
**                                                                 active table buffer via the #CFE_TBL_Load function. This 
**                                                                 option implies the #CFE_TBL_OPT_DUMP_ONLY and the 
**                                                                 #CFE_TBL_OPT_SNGL_BUFFER options and is mutually 
**                                                                 exclusive of the #CFE_TBL_OPT_DBL_BUFFER option.
**                                 \arg #CFE_TBL_OPT_CRITICAL-     When this option is selected, the Table Service 
**                                                                 will automatically allocate space in the Critical Data Store (CDS)
**                                                                 for the table and insure that the contents in the CDS are the same
**                                                                 as the contents of the currently active buffer for the table.
**                                                                 This option is mutually exclusive of the #CFE_TBL_OPT_USR_DEF_ADDR
**                                                                 and #CFE_TBL_OPT_DUMP_ONLY options.  It should also be noted that
**                                                                 the use of this option with double buffered tables will prevent
**                                                                 the update of the double buffered table from being quick and
**                                                                 it could be blocked.  Therefore, critical tables should not be
**                                                                 updated by Interrupt Service Routines.
**
** \param[in] TblValidationFuncPtr is a pointer to a function that will be executed in the context of the Table 
**                                 Management Service when the contents of a table need to be validated.  If set 
**                                 to NULL, then the Table Management Service will assume any data is valid.  If 
**                                 the value is not NULL, it must be a pointer to a function with the following 
**                                 prototype: <BR>
**                                 <B> int32 CallbackFunc(void *TblPtr);</B> <BR>
**                                 where  <BR>
**                                 <B>TblPtr </B> will be a pointer to the table data that is to be verified.  When the 
**                                 function returns #CFE_SUCCESS, the data is considered valid and ready for a commit.  
**                                 When the function returns a negative value, the data is considered invalid and an 
**                                 Event Message will be issued containing the returned value.  If the function should
**                                 return a positive number, the table is considered invalid and the return code is
**                                 considered invalid.  Validation functions \b must return either #CFE_SUCCESS or a negative
**                                 number (whose value is at the developer's discretion).  The validation function 
**                                 will be executed in the Application's context so that Event Messages describing the 
**                                 validation failure are possible from within the function.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                      \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_RECOVERED_TBL       \copybrief CFE_TBL_INFO_RECOVERED_TBL
** \retval #CFE_TBL_ERR_DUPLICATE_DIFF_SIZE  \copybrief CFE_TBL_ERR_DUPLICATE_DIFF_SIZE
** \retval #CFE_TBL_ERR_DUPLICATE_NOT_OWNED  \copybrief CFE_TBL_ERR_DUPLICATE_NOT_OWNED
** \retval #CFE_TBL_ERR_REGISTRY_FULL        \copybrief CFE_TBL_ERR_REGISTRY_FULL
** \retval #CFE_TBL_ERR_HANDLES_FULL         \copybrief CFE_TBL_ERR_HANDLES_FULL
** \retval #CFE_TBL_ERR_INVALID_SIZE         \copybrief CFE_TBL_ERR_INVALID_SIZE
** \retval #CFE_TBL_ERR_INVALID_NAME         \copybrief CFE_TBL_ERR_INVALID_NAME
** \retval #CFE_TBL_ERR_BAD_APP_ID           \copybrief CFE_TBL_ERR_BAD_APP_ID
**
** \sa #CFE_TBL_Unregister, #CFE_TBL_Share
**/
CFE_Status_t CFE_TBL_Register(CFE_TBL_Handle_t *TblHandlePtr,                  /* Returned Handle */
                          const char   *Name,                              /* Application specific name  */
                          size_t  Size,                                    /* Size, in bytes, of table   */
                          uint16  TblOptionFlags,                          /* Tbl Options Settings     */
                          CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr); /* Ptr to func that validates tbl */

/*****************************************************************************/
/**
** \brief Obtain handle of table registered by another application
**
** \par Description
**        After a table has been created, other applications can gain access 
**        to that table via the table handle.  In order for two or more 
**        applications to share a table, the applications that do not create 
**        the table must obtain the handle using this function.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in, out]  TblHandlePtr  A pointer to a #CFE_TBL_Handle_t type variable 
**                           that will be assigned the table's handle.  The 
**                           table handle is required for other API calls 
**                           when accessing the data contained in the table. *TblHandlePtr is the handle used to identify table to cFE when performing Table operations.
**                           This value is returned at the address specified by TblHandlePtr.
**
** \param[in]  TblName       The processor specific name of the table.  It is important to note 
**                           that the processor specific table name is different from the table 
**                           name specified in the #CFE_TBL_Register API call.  The processor 
**                           specific table name includes the name of the application that created 
**                           the table.  The name would be of the form "ApplicationName.TableName".  
**                           An example of this would be "ACS.TamParams" for a table called "TamParams" 
**                           that was registered by the application called "ACS".
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS              \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_HANDLES_FULL \copybrief CFE_TBL_ERR_HANDLES_FULL
** \retval #CFE_TBL_ERR_INVALID_NAME \copybrief CFE_TBL_ERR_INVALID_NAME
** \retval #CFE_TBL_ERR_BAD_APP_ID   \copybrief CFE_TBL_ERR_BAD_APP_ID
**
** \sa #CFE_TBL_Unregister, #CFE_TBL_Register
**
******************************************************************************/
CFE_Status_t CFE_TBL_Share(CFE_TBL_Handle_t *TblHandlePtr, const char *TblName);

/*****************************************************************************/
/**
** \brief Unregister a previously registered table and free associated resources
**
** \par Description
**        When an application is being removed from the system, it should 
**        unregister those tables that it created.  The application should 
**        call this function as a part of its cleanup process.  The table 
**        will be removed from memory once all table addresses referencing 
**        it have been released.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                      that identifies the Table to be unregistered.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Share, #CFE_TBL_Register
** 
******************************************************************************/
CFE_Status_t CFE_TBL_Unregister (CFE_TBL_Handle_t TblHandle);
/**@}*/

/** @defgroup CFEAPITBLManage cFE Manage Table Content APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Load a specified table with data from specified source
**
** \par Description
**        Once an application has created a table (#CFE_TBL_Register), it must 
**        provide the values that initialize the contents of that table.  The 
**        application accomplishes this with one of two different TBL API calls.  
**        This function call initializes the table with values that are held 
**        in a data structure.  
**
** \par Assumptions, External Events, and Notes:
**        This function call can block.  Therefore, interrupt service routines 
**        should NOT initialize their own tables.  An application should initialize 
**        any table(s) prior to providing the handle(s) to the interrupt service routine.
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be loaded.
**
** \param[in] SrcType    Flag indicating the nature of the given \c SrcDataPtr below.
**                       This value can be any one of the following:
**                          \arg #CFE_TBL_SRC_FILE    - \copybrief CFE_TBL_SRC_FILE
**                          \arg #CFE_TBL_SRC_ADDRESS - \copybrief CFE_TBL_SRC_ADDRESS 
**                                                      
** \param[in] SrcDataPtr Pointer to either a character string specifying a filename or
**                       a memory address of a block of binary data to be loaded into a table or,
**                       if the table was registered with the #CFE_TBL_OPT_USR_DEF_ADDR option,
**                       the address of the active table buffer.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                   \copybrief CFE_SUCCESS
** \retval #CFE_TBL_WARN_SHORT_FILE       \copybrief CFE_TBL_WARN_SHORT_FILE
** \retval #CFE_TBL_WARN_PARTIAL_LOAD     \copybrief CFE_TBL_WARN_PARTIAL_LOAD
** \retval #CFE_TBL_ERR_BAD_APP_ID        \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS         \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE    \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_DUMP_ONLY         \copybrief CFE_TBL_ERR_DUMP_ONLY
** \retval #CFE_TBL_ERR_ILLEGAL_SRC_TYPE  \copybrief CFE_TBL_ERR_ILLEGAL_SRC_TYPE
** \retval #CFE_TBL_ERR_LOAD_IN_PROGRESS  \copybrief CFE_TBL_ERR_LOAD_IN_PROGRESS
** \retval #CFE_TBL_ERR_NO_BUFFER_AVAIL   \copybrief CFE_TBL_ERR_NO_BUFFER_AVAIL
** \retval #CFE_TBL_ERR_FILE_NOT_FOUND    \copybrief CFE_TBL_ERR_FILE_NOT_FOUND
** \retval #CFE_TBL_ERR_FILE_TOO_LARGE    \copybrief CFE_TBL_ERR_FILE_TOO_LARGE
** \retval #CFE_TBL_ERR_BAD_CONTENT_ID    \copybrief CFE_TBL_ERR_BAD_CONTENT_ID
** \retval #CFE_TBL_ERR_PARTIAL_LOAD      \copybrief CFE_TBL_ERR_PARTIAL_LOAD
**
** \sa #CFE_TBL_Update, #CFE_TBL_Validate, #CFE_TBL_Manage 
**
******************************************************************************/
CFE_Status_t CFE_TBL_Load(CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr);

/*****************************************************************************/
/**
** \brief Update contents of a specified table, if an update is pending
**
** \par Description
**        An application is \b required to perform a periodic check for an update 
**        for all the tables that it creates.  Typically, the application that 
**        created the table would call this function at the start or conclusion 
**        of any routine processing cycle or at regular intervals.  To determine 
**        whether an update is pending prior to making this call, the Application 
**        can use the #CFE_TBL_GetStatus API first.  If a table update is pending, 
**        it will take place during this function call.
** 
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be updated.
**                                      
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                    \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_NO_UPDATE_PENDING \copybrief CFE_TBL_INFO_NO_UPDATE_PENDING
** \retval #CFE_TBL_ERR_BAD_APP_ID         \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS          \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE     \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Load, #CFE_TBL_Validate, #CFE_TBL_Manage
**
******************************************************************************/
CFE_Status_t CFE_TBL_Update(CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Perform steps to validate the contents of a table image.
**
** \par Description
**        An application is \b required to perform a periodic check for an update
**        or a validation request for all the tables that it creates.  Typically,
**        the application that created the table would call this function at the
**        start or conclusion of any routine processing cycle.  To determine whether
**        a validation request is pending prior to making this call, the Application
**        can use the #CFE_TBL_GetStatus API first.  If a table validation is pending,
**        the Application would call this function to perform the necessary actions.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be managed.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                        \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_NO_VALIDATION_PENDING \copybrief CFE_TBL_INFO_NO_VALIDATION_PENDING
** \retval #CFE_TBL_ERR_BAD_APP_ID             \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS              \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE         \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Update, #CFE_TBL_Manage, #CFE_TBL_Load
**
******************************************************************************/
CFE_Status_t CFE_TBL_Validate(CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Perform standard operations to maintain a table.
**
** \par Description
**        An application is \b required to perform a periodic check for an update
**        or a validation request for all the tables that it creates.  Typically,
**        the application that created the table would call this function at the
**        start or conclusion of any routine processing cycle.  If a table update
**        or validation request is pending, this function would perform either or
**        both before returning.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be managed.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED       \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Update, #CFE_TBL_Validate, #CFE_TBL_Load, #CFE_TBL_DumpToBuffer
**
******************************************************************************/
CFE_Status_t CFE_TBL_Manage(CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Copies the contents of a Dump Only Table to a shared buffer
**
** \par Description
**        Copies contents of a Dump Only table to a shared buffer so that it
**        can be written to a file by the Table Services routine.  This function
**        is called by the Application that owns the table in response to a #CFE_TBL_INFO_DUMP_PENDING
**        status obtained via #CFE_TBL_GetStatus.
**
** \par Assumptions, External Events, and Notes:
**        -# If the table does not have a dump pending status, nothing will occur (no error, no dump)
**        -# Applications may wish to use this function in lieu of #CFE_TBL_Manage for their Dump Only tables
**
** \param[in]  TblHandle      Handle of Table to be dumped.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Manage
**
******************************************************************************/
CFE_Status_t   CFE_TBL_DumpToBuffer(CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Notify cFE Table Services that table contents have been modified by the Application
**
** \par Description
**        This API notifies Table Services that the contents of the specified table has been
**        modified by the Application.  This notification is important when a table has been
**        registered as "Critical" because Table Services can then update the contents of the
**        table kept in the Critical Data Store.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in]  TblHandle      Handle of Table that was modified.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Manage
**
******************************************************************************/
CFE_Status_t   CFE_TBL_Modified(CFE_TBL_Handle_t TblHandle);
/**@}*/

/** @defgroup CFEAPITBLAccess cFE Access Table Content APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Obtain the current address of the contents of the specified table
**
** \par Description
**        When a table has been created and initialized, it is available to 
**        any application that can identify it with its unique handle.  In 
**        order to view the data contained in the table, an application must 
**        call this function or #CFE_TBL_GetAddresses.
**
** \par Assumptions, External Events, and Notes:
**        -# This call can be a blocking call when the table is not double buffered 
**           and is shared with another application of lower priority that just happens 
**           to be in the middle of a table update of the specific table.  If this occurs, 
**           the application performing the table update will automatically have its 
**           priority elevated in order to release the resource as soon as possible.
**        -# An application must always release the returned table address using the 
**           #CFE_TBL_ReleaseAddress or #CFE_TBL_ReleaseAddresses function prior to 
**           either a #CFE_TBL_Update call or any blocking call (e.g. - pending on software 
**           bus message, etc).  Table updates cannot occur while table addresses have not 
**           been released.
**        -# #CFE_TBL_ERR_NEVER_LOADED will be returned if the table has never been
**           loaded (either from file or from a block of memory), but the function
**           will still return a valid table pointer to a table with all zero content.
**           This pointer mush be released with the #CFE_TBL_ReleaseAddress API before
**           the table can be loaded with data.
**
** \param[in, out]  TblPtr     The address of a pointer that will be loaded with the address of 
**                        the first byte of the table.  This pointer can then be typecast 
**                        by the calling application to the appropriate table data structure. *TblPtr is the address of the first byte of data associated with the specified table.
**
** \param[in]  TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                        identifies the Table whose address is to be returned.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED       \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_UNREGISTERED   \copybrief CFE_TBL_ERR_UNREGISTERED
** \retval #CFE_TBL_ERR_NEVER_LOADED   \copybrief CFE_TBL_ERR_NEVER_LOADED
**
** \sa #CFE_TBL_ReleaseAddress, #CFE_TBL_GetAddresses, #CFE_TBL_ReleaseAddresses
**
******************************************************************************/
CFE_Status_t CFE_TBL_GetAddress(void **TblPtr, CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Release previously obtained pointer to the contents of the specified table
**
** \par Description
**        Each application is \b required to release a table address obtained through 
**        the #CFE_TBL_GetAddress function.
**
** \par Assumptions, External Events, and Notes:
**        An application must always release the returned table address using the 
**        #CFE_TBL_ReleaseAddress function prior to either a #CFE_TBL_Update call 
**        or any blocking call (e.g. - pending on software bus message, etc).  
**        Table updates cannot occur while table addresses have not been released.
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table whose address is to be released.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED       \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_NEVER_LOADED   \copybrief CFE_TBL_ERR_NEVER_LOADED
**
** \sa #CFE_TBL_GetAddress, #CFE_TBL_GetAddresses, #CFE_TBL_ReleaseAddresses
**
******************************************************************************/
CFE_Status_t CFE_TBL_ReleaseAddress(CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Obtain the current addresses of an array of specified tables
**
** \par Description
**        When a table has been created and initialized, it is available to 
**        any application that can identify it with its unique handle.  In 
**        order to view the data contained in the table, an application must 
**        call this function or #CFE_TBL_GetAddresses.
**
** \par Assumptions, External Events, and Notes:
**        -# This call can be a blocking call when the table is not double buffered 
**           and is shared with another application of lower priority that just happens 
**           to be in the middle of a table update of the specific table.  If this occurs, 
**           the application performing the table update will automatically have its 
**           priority elevated in order to release the resource as soon as possible.
**        -# An application must always release the returned table address using the 
**           #CFE_TBL_ReleaseAddress or #CFE_TBL_ReleaseAddresses function prior to 
**           either a #CFE_TBL_Update call or any blocking call (e.g. - pending on software 
**           bus message, etc).  Table updates cannot occur while table addresses have not 
**           been released.
**        -# #CFE_TBL_ERR_NEVER_LOADED will be returned if the table has never been
**           loaded (either from file or from a block of memory), but the function
**           will still return a valid table pointer to a table with all zero content.
**           This pointer mush be released with the #CFE_TBL_ReleaseAddress API before
**           the table can be loaded with data.
**
** \param[in, out] TblPtrs    Array of Pointers to variables that calling Application
**                       wishes to hold the start addresses of the Tables. *TblPtrs is an array of addresses of the first byte of data associated with the
**                       specified tables.
**
** \param[in] NumTables  Size of TblPtrs and TblHandles arrays.
**
** \param[in] TblHandles Array of Table Handles, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                       of those tables whose start addresses are to be obtained.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED       \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_UNREGISTERED   \copybrief CFE_TBL_ERR_UNREGISTERED
** \retval #CFE_TBL_ERR_NEVER_LOADED   \copybrief CFE_TBL_ERR_NEVER_LOADED
**
** \sa #CFE_TBL_GetAddress, #CFE_TBL_ReleaseAddress, #CFE_TBL_ReleaseAddresses
**
******************************************************************************/
CFE_Status_t CFE_TBL_GetAddresses(void **TblPtrs[], uint16 NumTables, const CFE_TBL_Handle_t TblHandles[]);

/*****************************************************************************/
/**
** \brief Release the addresses of an array of specified tables
**
** \par Description
**        Each application is \b required to release a table address obtained through 
**        the #CFE_TBL_GetAddress function.
**
** \par Assumptions, External Events, and Notes:
**        An application must always release the returned table address using the 
**        #CFE_TBL_ReleaseAddress function prior to either a #CFE_TBL_Update call 
**        or any blocking call (e.g. - pending on software bus message, etc).  
**        Table updates cannot occur while table addresses have not been released.
**
** \param[in] NumTables  Size of TblHandles array.
**
** \param[in] TblHandles Array of Table Handles, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                       of those tables whose start addresses are to be released.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED       \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_NEVER_LOADED   \copybrief CFE_TBL_ERR_NEVER_LOADED
**
** \sa #CFE_TBL_GetAddress, #CFE_TBL_ReleaseAddress, #CFE_TBL_GetAddresses
**
******************************************************************************/
CFE_Status_t CFE_TBL_ReleaseAddresses(uint16 NumTables, const CFE_TBL_Handle_t TblHandles[]);
/**@}*/

/** @defgroup CFEAPITBLInfo cFE Get Table Information APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Obtain current status of pending actions for a table.
**
** \par Description
**        An application is \b required to perform a periodic check for an update 
**        or a validation request for all the tables that it creates.  Typically, 
**        the application that created the table would call this function at the 
**        start or conclusion of any routine processing cycle.  If a table update 
**        or validation request is pending, the Application should follow up with 
**        a call to #CFE_TBL_Update or #CFE_TBL_Validate respectively.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be managed.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATE_PENDING     \copybrief CFE_TBL_INFO_UPDATE_PENDING
** \retval #CFE_TBL_INFO_VALIDATION_PENDING \copybrief CFE_TBL_INFO_VALIDATION_PENDING
** \retval #CFE_TBL_INFO_DUMP_PENDING       \copybrief CFE_TBL_INFO_DUMP_PENDING
** \retval #CFE_TBL_ERR_BAD_APP_ID          \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \note Some status return codes are "success" while being non-zero. This
**       behavior will change in the future.
**
** \sa #CFE_TBL_Manage, #CFE_TBL_Update, #CFE_TBL_Validate, #CFE_TBL_GetInfo
**
******************************************************************************/
CFE_Status_t CFE_TBL_GetStatus(CFE_TBL_Handle_t TblHandle);

/*****************************************************************************/
/**
** \brief Obtain characteristics/information of/about a specified table.
**
** \par Description
**        This API provides the registry information associated with the specified
**        table.  The function fills the given data structure with the data found
**        in the Table Registry.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in, out]  TblInfoPtr    A pointer to a CFE_TBL_Info_t data structure that is to be populated
**                           with table characteristics and information. *TblInfoPtr is the description of the tables characteristics and registry information stored in
**                           the #CFE_TBL_Info_t data structure format.
** 
** \param[in]  TblName       The processor specific name of the table.  It is important to note 
**                           that the processor specific table name is different from the table 
**                           name specified in the #CFE_TBL_Register API call.  The processor 
**                           specific table name includes the name of the application that created 
**                           the table.  The name would be of the form "ApplicationName.TableName".  
**                           An example of this would be "ACS.TamParams" for a table called "TamParams" 
**                           that was registered by the application called "ACS".
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS              \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_INVALID_NAME \copybrief CFE_TBL_ERR_INVALID_NAME
**
** \sa #CFE_TBL_GetStatus
**
******************************************************************************/
CFE_Status_t CFE_TBL_GetInfo(CFE_TBL_Info_t *TblInfoPtr, const char *TblName);

/*****************************************************************************/
/**
** \brief Instruct cFE Table Services to notify Application via message when table requires management
**
** \par Description
**        This API instructs Table Services to send a message to the calling Application
**        whenever the specified table requires management by the application.  This feature
**        allows applications to avoid polling table services via the #CFE_TBL_Manage call
**        to determine whether a table requires updates, validation, etc.  This API should
**        be called following the #CFE_TBL_Register API whenever the owning application requires
**        this feature.
**
** \par Assumptions, External Events, and Notes:
**        - Only the application that owns the table is allowed to register a notification message
**        - Recommend \b NOT using the ground command MID which typically impacts command counters.
**          The typical approach is to use a unique MID for inter-task communications
**          similar to how schedulers typically trigger application housekeeping messages.
**
** \param[in]  TblHandle      Handle of Table with which the message should be associated.
** 
** \param[in]  MsgId          Message ID to be used in notification message sent by Table Services.
** 
** \param[in]  CommandCode    Command Code value to be placed in secondary header of message 
**                            sent by Table Services.
** 
** \param[in]  Parameter      Application defined value to be passed as a parameter in the 
**                            message sent by Table Services.  Suggested use includes an application's
**                            table index that allows the same MsgId and Command Code to be used for
**                            all table management notifications.
** 
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_BAD_APP_ID     \copybrief CFE_TBL_ERR_BAD_APP_ID
** \retval #CFE_TBL_ERR_NO_ACCESS      \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Register
**
******************************************************************************/
CFE_Status_t CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, CFE_SB_MsgId_t MsgId, CFE_MSG_FcnCode_t CommandCode, uint32 Parameter);
/**@}*/

#endif  /* _cfe_tbl_ */
