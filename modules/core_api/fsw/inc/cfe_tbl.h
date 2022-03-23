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

#ifndef CFE_TBL_H
#define CFE_TBL_H

/********************* Include Files  ************************/
#include "common_types.h" /* Basic Data Types */
#include "cfe_error.h"
#include "cfe_tbl_api_typedefs.h"
#include "cfe_sb_api_typedefs.h"

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
** \param[out] TblHandlePtr    a pointer to a #CFE_TBL_Handle_t type variable @nonnull that will be assigned the table's
**                                 handle.  The table handle is required for other API calls when accessing the data
**                                 contained in the table. *TblHandlePtr is the handle used to identify table to cFE
**                                 when performing Table operations. This value is returned at address specified by
**                                 TblHandlePtr.
**
** \param[in] Name                 The raw table name.  This name will be combined with the name of the
**                                 application to produce a name of the form "AppName.RawTableName".
**                                 This application specific name will be used in commands
**                                 for modifying or viewing the contents of the table.
**
** \param[in] Size                 The size, in bytes, of the table to be created @nonzero.  This is the size that will
*be
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
**                                                                 and #CFE_TBL_OPT_DEFAULT options. If the Application
**                                                                 wishes to specify their own block of memory as the
**                                                                 Dump Only table, they need to also include the
**                                                                 #CFE_TBL_OPT_USR_DEF_ADDR option explained below.
**                                 \arg #CFE_TBL_OPT_NOT_USR_DEF - When this option is selected, Table Services
**                                                                 allocates memory for the table and, in the case of a
**                                                                 double buffered table, it allocates the same amount
**                                                                 of memory again for the second buffer.  This option
**                                                                 is mutually exclusive with the
**                                                                 #CFE_TBL_OPT_USR_DEF_ADDR option.
**                                 \arg #CFE_TBL_OPT_USR_DEF_ADDR- When this option is selected, the Table Service
**                                                                 will not allocate memory for the table.  Table
**                                                                 Services will require the Application to identify
**                                                                 the location of the active table buffer via the
**                                                                 #CFE_TBL_Load function. This option implies the
**                                                                 #CFE_TBL_OPT_DUMP_ONLY and the
**                                                                 #CFE_TBL_OPT_SNGL_BUFFER options and is mutually
**                                                                 exclusive of the #CFE_TBL_OPT_DBL_BUFFER option.
**                                 \arg #CFE_TBL_OPT_CRITICAL-     When this option is selected, the Table Service
**                                                                 will automatically allocate space in the Critical
**                                                                 Data Store (CDS) for the table and ensure that the
**                                                                 contents in the CDS are the same
**                                                                 as the contents of the currently active buffer for
**                                                                 the table. This option is mutually exclusive of the
**                                                                 #CFE_TBL_OPT_USR_DEF_ADDR and #CFE_TBL_OPT_DUMP_ONLY
**                                                                 options.  It should also be noted that the use of
**                                                                 this option with double buffered tables will prevent
**                                                                 the update of the double buffered table from being
**                                                                 quick and it could be blocked.  Therefore, critical
**                                                                 tables should not be updated by Interrupt Service
**                                                                 Routines.
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
**                                 considered invalid.  Validation functions \b must return either #CFE_SUCCESS or a
**                                 negative number (whose value is at the developer's discretion).  The validation
**                                 function will be executed in the Application's context so that Event Messages
**                                 describing the validation failure are possible from within the function.
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
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID  \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_BAD_ARGUMENT             \copybrief CFE_TBL_BAD_ARGUMENT
** \retval #CFE_TBL_ERR_INVALID_OPTIONS      \copybrief CFE_TBL_ERR_INVALID_OPTIONS
** \retval #CFE_TBL_WARN_DUPLICATE           \copybrief CFE_TBL_WARN_DUPLICATE
** \retval #CFE_TBL_WARN_NOT_CRITICAL        \copybrief CFE_TBL_WARN_NOT_CRITICAL
**
** \sa #CFE_TBL_Unregister, #CFE_TBL_Share
**/
CFE_Status_t CFE_TBL_Register(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, size_t Size, uint16 TblOptionFlags,
                              CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr);

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
** \param[out]  TblHandlePtr  A pointer to a #CFE_TBL_Handle_t type variable @nonnull
**                           that will be assigned the table's handle.  The
**                           table handle is required for other API calls
**                           when accessing the data contained in the table. *TblHandlePtr is the handle used to
**                           identify table to cFE when performing Table operations.
**                           This value is returned at the address specified by TblHandlePtr.
**
** \param[in]  TblName       The application specific name of the table of the form "AppName.RawTableName",
**                           where RawTableName is the name specified in the #CFE_TBL_Register API call.
**                           Example: "ACS.TamParams" for a table called "TamParams"
**                           that was registered by the application called "ACS".
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_HANDLES_FULL        \copybrief CFE_TBL_ERR_HANDLES_FULL
** \retval #CFE_TBL_ERR_INVALID_NAME        \copybrief CFE_TBL_ERR_INVALID_NAME
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_BAD_ARGUMENT            \copybrief CFE_TBL_BAD_ARGUMENT
**
** \sa #CFE_TBL_Unregister, #CFE_TBL_Register
**
******************************************************************************/
CFE_Status_t CFE_TBL_Share(CFE_TBL_Handle_t *TblHandlePtr, const char *TblName);

/*****************************************************************************/
/**
** \brief Unregister a table
**
** \par Description
**        When an application is being removed from the system, ES will
**        clean up/free all the application related resources including tables
**        so apps are not required to call this function.
**
**        A valid use-case for this API is to unregister a shared table if
**        access is no longer needed or the owning application was removed from
**        the system (CS app is an example).
**
**        Typically apps should only register tables during initialization and
**        registration/unregistration by the owning application during operation
**        should be avoided. If unavoidable, special care needs to be taken
**        (especially for shared tables) to avoid race conditions due to
**        competing requests from multiple tasks.
**
**        Note the table will not be removed from memory until all table access
**        links have been removed (registration and all shared access).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                      that identifies the Table to be unregistered.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Share, #CFE_TBL_Register
**
******************************************************************************/
CFE_Status_t CFE_TBL_Unregister(CFE_TBL_Handle_t TblHandle);
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
** \param[in] SrcDataPtr Pointer @nonnull to either a character string specifying a filename or
**                       a memory address of a block of binary data to be loaded into a table or,
**                       if the table was registered with the #CFE_TBL_OPT_USR_DEF_ADDR option,
**                       the address of the active table buffer.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_DUMP_ONLY           \copybrief CFE_TBL_ERR_DUMP_ONLY
** \retval #CFE_TBL_ERR_ILLEGAL_SRC_TYPE    \copybrief CFE_TBL_ERR_ILLEGAL_SRC_TYPE
** \retval #CFE_TBL_ERR_LOAD_IN_PROGRESS    \copybrief CFE_TBL_ERR_LOAD_IN_PROGRESS
** \retval #CFE_TBL_ERR_LOAD_INCOMPLETE     \copybrief CFE_TBL_ERR_LOAD_INCOMPLETE
** \retval #CFE_TBL_ERR_NO_BUFFER_AVAIL     \copybrief CFE_TBL_ERR_NO_BUFFER_AVAIL
** \retval #CFE_TBL_ERR_ACCESS              \copybrief CFE_TBL_ERR_ACCESS
** \retval #CFE_TBL_ERR_FILE_TOO_LARGE      \copybrief CFE_TBL_ERR_FILE_TOO_LARGE
** \retval #CFE_TBL_ERR_BAD_CONTENT_ID      \copybrief CFE_TBL_ERR_BAD_CONTENT_ID
** \retval #CFE_TBL_ERR_BAD_SUBTYPE_ID      \copybrief CFE_TBL_ERR_BAD_SUBTYPE_ID
** \retval #CFE_TBL_ERR_NO_STD_HEADER       \copybrief CFE_TBL_ERR_NO_STD_HEADER
** \retval #CFE_TBL_ERR_NO_TBL_HEADER       \copybrief CFE_TBL_ERR_NO_TBL_HEADER
** \retval #CFE_TBL_ERR_PARTIAL_LOAD        \copybrief CFE_TBL_ERR_PARTIAL_LOAD
** \retval #CFE_TBL_BAD_ARGUMENT            \copybrief CFE_TBL_BAD_ARGUMENT
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
**        Typically, apps should just call #CFE_TBL_Manage as part
**        of routine processing which will perform validation, update, or dump
**        if pending. This API is provided for the case where just an
**        update should be performed.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be updated.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_NO_UPDATE_PENDING  \copybrief CFE_TBL_INFO_NO_UPDATE_PENDING
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
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
**        Typically, apps should just call #CFE_TBL_Manage as part
**        of routine processing which will perform validation, update, or dump
**        if pending. This API is provided for the case where just a
**        validation should be performed.
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
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID    \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
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
**        Applications should call this API periodically to process pending
**        requests for update, validation, or dump to buffer.  Typically,
**        the application that created the table would call this function at the
**        start or conclusion of any routine processing cycle.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in] TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                       identifies the Table to be managed.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED            \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_INFO_DUMP_PENDING       \copybrief CFE_TBL_INFO_DUMP_PENDING
** \retval #CFE_TBL_INFO_UPDATE_PENDING     \copybrief CFE_TBL_INFO_UPDATE_PENDING
** \retval #CFE_TBL_INFO_VALIDATION_PENDING \copybrief CFE_TBL_INFO_VALIDATION_PENDING
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
**        Typically, apps should just call #CFE_TBL_Manage as part
**        of routine processing which will perform validation, update, or dump
**        if pending. This API is provided for the case where just a
**        dump should be performed.
**
** \par Assumptions, External Events, and Notes:
**        If the table does not have a dump pending status, nothing will occur (no error, no dump)
**
** \param[in]  TblHandle      Handle of Table to be dumped.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_INFO_DUMP_PENDING       \copybrief CFE_TBL_INFO_DUMP_PENDING
**
** \sa #CFE_TBL_Manage
**
******************************************************************************/
CFE_Status_t CFE_TBL_DumpToBuffer(CFE_TBL_Handle_t TblHandle);

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
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Manage
**
******************************************************************************/
CFE_Status_t CFE_TBL_Modified(CFE_TBL_Handle_t TblHandle);
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
**           This pointer must be released with the #CFE_TBL_ReleaseAddress API before
**           the table can be loaded with data.
**
** \param[out]  TblPtr    The address of a pointer @nonnull that will be loaded with the address of
**                        the first byte of the table.  This pointer can then be typecast
**                        by the calling application to the appropriate table data structure. *TblPtr is the address of
**                        the first byte of data associated with the specified table.
**
** \param[in]  TblHandle  Handle, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share, that
**                        identifies the Table whose address is to be returned.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED            \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_UNREGISTERED        \copybrief CFE_TBL_ERR_UNREGISTERED
** \retval #CFE_TBL_ERR_NEVER_LOADED        \copybrief CFE_TBL_ERR_NEVER_LOADED
** \retval #CFE_TBL_BAD_ARGUMENT            \copybrief CFE_TBL_BAD_ARGUMENT
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
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED            \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_NEVER_LOADED        \copybrief CFE_TBL_ERR_NEVER_LOADED
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
**        call this function or #CFE_TBL_GetAddress.
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
**           This pointer must be released with the #CFE_TBL_ReleaseAddress API before
**           the table can be loaded with data.
**
** \param[out] TblPtrs   Array of Pointers @nonnull to variables that calling Application
**                       wishes to hold the start addresses of the Tables. *TblPtrs is an array of addresses of the
**                       first byte of data associated with the specified tables.
**
** \param[in] NumTables  Size of TblPtrs and TblHandles arrays.
**
** \param[in] TblHandles Array of Table Handles, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                       of those tables whose start addresses are to be obtained.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED            \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_UNREGISTERED        \copybrief CFE_TBL_ERR_UNREGISTERED
** \retval #CFE_TBL_ERR_NEVER_LOADED        \copybrief CFE_TBL_ERR_NEVER_LOADED
** \retval #CFE_TBL_BAD_ARGUMENT            \copybrief CFE_TBL_BAD_ARGUMENT
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
** \param[in] TblHandles Array of Table Handles @nonnull, previously obtained from #CFE_TBL_Register or #CFE_TBL_Share,
**                       of those tables whose start addresses are to be released.
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_TBL_INFO_UPDATED            \copybrief CFE_TBL_INFO_UPDATED
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
** \retval #CFE_TBL_ERR_NEVER_LOADED        \copybrief CFE_TBL_ERR_NEVER_LOADED
** \retval #CFE_TBL_BAD_ARGUMENT            \copybrief CFE_TBL_BAD_ARGUMENT
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
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
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
** \param[out]  TblInfoPtr   A pointer to a CFE_TBL_Info_t data structure @nonnull that is to be populated
**                           with table characteristics and information. *TblInfoPtr is the description of the tables
**                           characteristics and registry information stored in the #CFE_TBL_Info_t data structure
**                           format.
** \param[in]  TblName       The application specific name @nonnull of the table of the form "AppName.RawTableName",
**                           where RawTableName is the name specified in the #CFE_TBL_Register API call.
**                           Example: "ACS.TamParams" for a table called "TamParams"
**                           that was registered by the application called "ACS".
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_SUCCESS              \copybrief CFE_SUCCESS
** \retval #CFE_TBL_ERR_INVALID_NAME \copybrief CFE_TBL_ERR_INVALID_NAME
** \retval #CFE_TBL_BAD_ARGUMENT     \copybrief CFE_TBL_BAD_ARGUMENT
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
** \retval #CFE_SUCCESS                     \copybrief CFE_SUCCESS
** \retval #CFE_ES_ERR_RESOURCEID_NOT_VALID \copybrief CFE_ES_ERR_RESOURCEID_NOT_VALID
** \retval #CFE_TBL_ERR_NO_ACCESS           \copybrief CFE_TBL_ERR_NO_ACCESS
** \retval #CFE_TBL_ERR_INVALID_HANDLE      \copybrief CFE_TBL_ERR_INVALID_HANDLE
**
** \sa #CFE_TBL_Register
**
******************************************************************************/
CFE_Status_t CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, CFE_SB_MsgId_t MsgId, CFE_MSG_FcnCode_t CommandCode,
                                     uint32 Parameter);
/**@}*/

#endif /* CFE_TBL_H */
