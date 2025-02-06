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
 * Purpose:  cFE Table Services (TBL) utility function interface file
 *
 * Author:   D. Kobe/the Hammers Company, Inc.
 *
 * Notes:
 *
 */

#ifndef CFE_TBL_INTERNAL_H
#define CFE_TBL_INTERNAL_H

/*
** Required header files...
*/
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "cfe_msgids.h"
#include "cfe_perfids.h"
#include "cfe_tbl_task.h"
#include "cfe_tbl_task_cmds.h"
#include "cfe_tbl_eventids.h"
#include "cfe_tbl_msg.h"

/*********************  Macro and Constant Type Definitions   ***************************/

#define CFE_TBL_NOT_OWNED   CFE_ES_APPID_UNDEFINED
#define CFE_TBL_NOT_FOUND   (-1)
#define CFE_TBL_END_OF_LIST (CFE_TBL_Handle_t)0xFFFF

/**
 * Function type used with access descriptor iterator
 *
 * The access descriptor iterator will invoke the supplied function
 * for every descriptor associated with the table registry entry
 *
 * \param AccDescPtr Pointer to the current access descriptor
 * \param Arg Opaque argument from caller (passed through)
 */
typedef void (*const CFE_TBL_AccessDescFunc_t)(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg);

/*****************************  Function Prototypes   **********************************/

/*
 * Inline accessors for fields within the registry record struct
 *
 * Use of these accessors allows for greater independence between the logic and the
 * specific names and heirarchy of the data structure(s) that the values are stored in.
 *
 * They should all be trivial in nature.  All of these accessors are for internal use only
 * and only operate on pointers that are known to be valid (i.e. no additional checking here).
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the name associated with the Application record
 *
 * Returns the name field from within the Application record
 *
 * @note The name buffer in the registry contains the fully-qualified name (AppName.TableName)
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @returns Pointer to Table name
 */
static inline const char *CFE_TBL_RegRecGetName(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->Name;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the table size associated with the registry entry
 *
 * Returns the size field from within the registry entry
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @returns Table size
 */
static inline size_t CFE_TBL_RegRecGetSize(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->Size;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a load is in progress
 *
 * Checks if the table is currently being loaded
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @returns true if a load is in progress
 */
static inline size_t CFE_TBL_RegRecIsLoadInProgress(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Obtain the load in progress identifier
 *
 * Returns the identifier associated with the buffer being loaded
 * This is only valid for registry entries for which CFE_TBL_RegRecIsLoadInProgress() is true
 *
 * @param[inout]   RegRecPtr   pointer to Registry table entry
 * @returns Identifier for buffer being loaded
 */
static inline CFE_TBL_LoadBuffId_t CFE_TBL_RegRecGetLoadInProgress(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->LoadInProgress;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set the load in progress identifier
 *
 * Marks the registry entry as having a load in progress
 *
 * @param[inout] RegRecPtr      pointer to Registry table entry
 * @param[in]    LoadInProgress identifier of the buffer being loaded
 */
static inline void CFE_TBL_RegRecSetLoadInProgress(CFE_TBL_RegistryRec_t *RegRecPtr,
                                                   CFE_TBL_LoadBuffId_t   LoadInProgress)
{
    RegRecPtr->LoadInProgress = LoadInProgress;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Clear the load in progress identifier
 *
 * Marks the registry entry as not having a load in progress
 *
 * @param[inout]   RegRecPtr   pointer to Registry table entry
 */
static inline void CFE_TBL_RegRecClearLoadInProgress(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the last file loaded string
 *
 * Gets the last file loaded from the registry record
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @returns string indicating the last file loaded
 */
static inline const char *CFE_TBL_RegRecGetLastFileLoaded(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->LastFileLoaded;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the last update time
 *
 * Gets the last update time from the registry record
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @returns time of last update
 */
static inline CFE_TIME_SysTime_t CFE_TBL_RegRecGetLastUpdateTime(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->TimeOfLastUpdate;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if the table has initially loaded
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @retval false if the table is registered but not yet loaded
 * @retval true if the table has been loaded at least once
 */
static inline bool CFE_TBL_RegRecIsTableLoaded(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->TableLoadedOnce;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Sets the table loaded flag
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 */
static inline void CFE_TBL_RegRecSetTableLoadedFlag(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->TableLoadedOnce = true;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a table load is pending
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @retval false if there is no load pending
 * @retval true if there is a load pending
 */
static inline bool CFE_TBL_RegRecIsLoadPending(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->LoadPending;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Sets the load pending flag
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 */
static inline void CFE_TBL_RegRecSetLoadPendingFlag(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->LoadPending = true;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Clears the load pending flag
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 */
static inline void CFE_TBL_RegRecClearLoadPendingFlag(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->LoadPending = false;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a table has been modified from its original form
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @retval false if the table has not been modified since the last load
 * @retval true if the table is modified since the last load
 */
static inline bool CFE_TBL_RegRecIsModified(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->IsModified;
}

/*---------------------------------------------------------------------------------------*/
/**
** \brief Returns the Registry Index for the specified Table Name
**
** \par Description
**        Locates given Table Name in the Table Registry and
**        returns the appropriate Registry Index.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  TblName - Pointer to character string containing complete
**                       Table Name (of the format "AppName.TblName").
**
** \retval #CFE_TBL_NOT_FOUND or the Index into Registry for Table with specified name
**
*/
CFE_TBL_RegId_t CFE_TBL_FindTableInRegistry(const char *TblName);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Locks access to the Table Registry
**
** \par Description
**        Locks the Table Registry to prevent multiple tasks/threads
**        from modifying it at once.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
*/
int32 CFE_TBL_LockRegistry(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Unlocks access to the Table Registry
**
** \par Description
**        Unlocks Table Registry to allow other tasks/threads to
**        modify the Table Registry contents.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**
*/
int32 CFE_TBL_UnlockRegistry(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Finds the address of a buffer compatible with the specified table
**
** \par Description
**        Checks to see if the specified table has a dedicated working
**        buffer (i.e. - is a double buffered table) or requires one
**        from the common table buffer pool.  If it requires one from
**        the pool, it locates, locks and returns its address.  If the
**        table is double buffered, the access list is scanned to ensure
**        that nobody is currently using the inactive buffer.
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes the TblHandle and MinBufferSize values
**           are legitimate.
**
** \param[in, out]  WorkingBufferPtr  Pointer to variable that will contain the
**                                    address of the first byte of the working buffer. *WorkingBufferPtr is the address
**                                    of the first byte of the working buffer
**
** \param[in]  RegRecPtr         Pointer to Table Registry Entry for Table for whom
**                               a working buffer is to be obtained
**
** \param[in]  CalledByApp       Boolean that identifies whether this internal API
**                               function is being called by a user Application (true)
**                               or by the Table Services Application (false)
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_NO_BUFFER_AVAIL     \copydoc CFE_TBL_ERR_NO_BUFFER_AVAIL
**
*/
int32 CFE_TBL_GetWorkingBuffer(CFE_TBL_LoadBuff_t **WorkingBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                               bool CalledByApp);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Loads a table buffer with data from a specified file
**
** \par Description
**        Locates the specified filename in the onboard filesystem
**        and loads its contents into the specified working buffer.
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes parameters have been verified.
**
** \param[in]  AppName          The name of the application loading the table.
**
** \param[in]  WorkingBufferPtr Pointer to a working buffer that is to be loaded
**                              with the contents of the specified file
**
** \param[in]  RegRecPtr        Pointer to Table Registry record for table whose
**                              buffer is to filled with data from the specified file
**
** \param[in]  Filename         Pointer to ASCII string containing full path and filename
**                              of table image file to be loaded
**
** \retval #CFE_SUCCESS                      \copydoc CFE_SUCCESS
** \retval #OS_INVALID_POINTER               \copydoc OS_INVALID_POINTER
** \retval #OS_FS_ERR_PATH_TOO_LONG          \copydoc OS_FS_ERR_PATH_TOO_LONG
** \retval #OS_FS_ERR_PATH_INVALID           \copydoc OS_FS_ERR_PATH_INVALID
** \retval #OS_FS_ERR_NAME_TOO_LONG          \copydoc OS_FS_ERR_NAME_TOO_LONG
** \retval #OS_ERR_NO_FREE_IDS               \copydoc OS_ERR_NO_FREE_IDS
** \retval #OS_ERROR                         \copydoc OS_ERROR
** \retval #CFE_TBL_ERR_FILE_TOO_LARGE       \copydoc CFE_TBL_ERR_FILE_TOO_LARGE
** \retval #CFE_TBL_WARN_SHORT_FILE          \copydoc CFE_TBL_WARN_SHORT_FILE
** \retval #CFE_TBL_WARN_PARTIAL_LOAD        \copydoc CFE_TBL_WARN_PARTIAL_LOAD
** \retval #CFE_TBL_ERR_FILENAME_TOO_LONG    \copydoc CFE_TBL_ERR_FILENAME_TOO_LONG
** \retval #CFE_TBL_ERR_FILE_FOR_WRONG_TABLE \copydoc CFE_TBL_ERR_FILE_FOR_WRONG_TABLE
** \retval #CFE_TBL_ERR_NO_STD_HEADER        \copydoc CFE_TBL_ERR_NO_STD_HEADER
** \retval #CFE_TBL_ERR_NO_TBL_HEADER        \copydoc CFE_TBL_ERR_NO_TBL_HEADER
** \retval #CFE_TBL_ERR_BAD_CONTENT_ID       \copydoc CFE_TBL_ERR_BAD_CONTENT_ID
** \retval #CFE_TBL_ERR_BAD_SUBTYPE_ID       \copydoc CFE_TBL_ERR_BAD_SUBTYPE_ID
**
*/
int32 CFE_TBL_LoadFromFile(const char *AppName, CFE_TBL_LoadBuff_t *WorkingBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                           const char *Filename);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Updates the active table buffer with contents of inactive buffer
**
** \par Description
**        Copies pertinent data from working buffer (inactive buffer)
**        to the active buffer (for single buffered tables) or just
**        changes index to identifying the active buffer (for double
**        buffered tables).
**
** \par Assumptions, External Events, and Notes:
**        -# All parameters are assumed to be verified before function
**           is called.
**
** \param[in]  TblHandle      Handle of Table to be updated.
**
** \param[in]  RegRecPtr      Pointer to Table Registry Entry for table to be updated
**
** \param[in]  AccessDescPtr  Pointer to appropriate access descriptor for table-application interface
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
*/
int32 CFE_TBL_UpdateInternal(CFE_TBL_Handle_t TblHandle, CFE_TBL_RegistryRec_t *RegRecPtr,
                             CFE_TBL_AccessDescriptor_t *AccessDescPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Sets flags in access descriptors associated with specified table
**
** \par Description
**        Sets the flag in each access descriptor for a table to indicate the
**        contents of the table have been updated.
**
** \par Assumptions, External Events, and Notes:
**        -# All parameters are assumed to be verified before function
**           is called.
**
** \param[in]  RegRecPtr      Pointer to Table Registry Entry for table to be updated
*/
void CFE_TBL_NotifyTblUsersOfUpdate(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Reads Table File Headers
**
** \par Description
**        Reads Table File Headers and performs rudimentary checks
**        on header contents to ensure the acceptability of the
**        file format.
**
** \par Assumptions, External Events, and Notes:
**        -# FileDescriptor is assumed to be valid
**
** \param[in]  FileDescriptor    File Descriptor, as provided by OS_fopen
**
** \param[in, out]  StdFileHeaderPtr  Pointer to buffer to be filled with the contents
**                                    of the file's standard cFE Header. *StdFileHeaderPtr is the contents of the
**                                    standard cFE File Header
**
** \param[in, out]  TblFileHeaderPtr  Pointer to buffer to be filled with the contents
**                                    of the file's standard cFE Table Header. *TblFileHeaderPtr is the contents of the
**                                    standard cFE Table File Header
**
** \param[in]  LoadFilename      Pointer to character string containing full path
**                               and filename of table image to be loaded
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_NO_STD_HEADER       \copydoc CFE_TBL_ERR_NO_STD_HEADER
** \retval #CFE_TBL_ERR_NO_TBL_HEADER       \copydoc CFE_TBL_ERR_NO_TBL_HEADER
** \retval #CFE_TBL_ERR_BAD_CONTENT_ID      \copydoc CFE_TBL_ERR_BAD_CONTENT_ID
** \retval #CFE_TBL_ERR_BAD_SUBTYPE_ID      \copydoc CFE_TBL_ERR_BAD_SUBTYPE_ID
** \retval #CFE_TBL_ERR_BAD_SPACECRAFT_ID   \copydoc CFE_TBL_ERR_BAD_SPACECRAFT_ID
** \retval #CFE_TBL_ERR_BAD_PROCESSOR_ID    \copydoc CFE_TBL_ERR_BAD_PROCESSOR_ID
**
*/
int32 CFE_TBL_ReadHeaders(osal_id_t FileDescriptor, CFE_FS_Header_t *StdFileHeaderPtr,
                          CFE_TBL_File_Hdr_t *TblFileHeaderPtr, const char *LoadFilename);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes the entries of a single Table Registry Record
**
** \par Description
**        Initializes the contents of a single Table Registry Record to default values
**
** \par Assumptions, External Events, and Notes:
**        -# This function is intended to be called before populating a table registry record
**
** \param[out]  RegRecPtr         Pointer to Registry Entry to initialize
*/
void CFE_TBL_InitRegistryRecord(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes the entries of a single Access Descriptor
**
** \par Description
**        Initializes the contents of a single Access Descriptor to default values
**
** \par Assumptions, External Events, and Notes:
**        -# This function is intended to be called before populating the Access Descriptor
**
** \param[out]  AccessDescPtr         Pointer to Access Descriptor to initialize
*/
void CFE_TBL_InitAccessDescriptor(CFE_TBL_AccessDescriptor_t *AccessDescPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Byte swaps a CFE_TBL_File_Hdr_t structure
**
** \par Description
**        Converts a big-endian version of a CFE_TBL_File_Hdr_t structure to
**        a little-endian version and vice-versa.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in, out]  HdrPtr   Pointer to table header that needs to be swapped. *HdrPtr provides the swapped header
**
*/
void CFE_TBL_ByteSwapTblHeader(CFE_TBL_File_Hdr_t *HdrPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Searches the Critical Table Registry for the given handle
**
** \par Description
**        This function scans the Critical Table Registry to find the specified
**        handle.  Once located, the function returns a pointer to the appropriate
**        Critical Table Registry Record that contains information on where the
**        contents of the Table came from and when.  If a matching record is not
**        found, the pointer returned is NULL.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in, out] **CritRegRecPtr    Pointer to a pointer that should be initialized with
**                               the start address of the located Critical Table Registry
**                               Record. *CritRegRecPtr is the pointer to the start address of the located Critical
**                               Table Registry Record.  \c NULL if the record is not
**                               found.
**
** \param[in]  CDSHandleToFind   CDS Handle to be located in Critical Table Registry.
**
*/
void CFE_TBL_FindCriticalTblInfo(CFE_TBL_CritRegRec_t **CritRegRecPtr, CFE_ES_CDSHandle_t CDSHandleToFind);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Updates a CDS associated with a Critical Table
**
** \par Description
**        Copies the contents of the active buffer into a previously allocated
**        CDS associated with the table.  The Critical Table Registry is also
**        updated and copied into the CDS to keep relevant information on the
**        source of the data contained in the table.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  RegRecPtr Pointer to Registry Record of Critical Table whose CDS
**                       needs to be updated.
**
*/
void CFE_TBL_UpdateCriticalTblCDS(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief When enabled, will send a manage notification command message
**
** \par Description
**        Whenever an application uses the #CFE_TBL_NotifyByMessage API, Table services
**        will call this routine whenever a table requires management by the owning
**        Application.  This routine will then issue the appropriate message to the
**        software bus.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in]  RegRecPtr Pointer to Registry Record of Table whose owner needs notifying.
**
*/
int32 CFE_TBL_SendNotificationMsg(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Performs a byte swap on a uint32 integer
**
** \par Description
**        Converts a big-endian uint32 integer to a little-endian uint32 integer
**        and vice-versa.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \param[in, out]  Uint32ToSwapPtr Pointer to uint32 value to be swapped. *Uint32ToSwapPtr is the swapped uint32 value
**
*/
void CFE_TBL_ByteSwapUint32(uint32 *Uint32ToSwapPtr);

/*
 * Internal helper functions for Table Registry dump
 *
 * These callbacks are used with the FS background write request API
 * and are implemented per that specification.
 */
void CFE_TBL_DumpRegistryEventHandler(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum,
                                      size_t BlockSize, size_t Position);
bool CFE_TBL_DumpRegistryGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize);

/*
 * Internal helper functions for CFE_TBL_Register()
 *
 * These functions execute the table registration process but are
 * separated out into the most cohesive logical blocks of work.
 *
 */

/*---------------------------------------------------------------------------------------*/
/**
** \brief Validates the size of the table to be registered
**
** \par Description
**         This function validates the size of the table that is being registered. It
**         checks that the size is not zero, and that single/double-buffered tables are
**         not requested to be of a size larger than their respective limits
**         (CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE & CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE).
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_INVALID_SIZE        \copydoc CFE_TBL_ERR_INVALID_SIZE
**
*/
CFE_Status_t CFE_TBL_ValidateTableSize(const char *Name, size_t Size, uint16 TblOptionFlags);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Validates the selected table options
**
** \par Description
**         Validates the selected table options for a table that is being registered.
**         User-defined table addresses cannot be double-buffered, load/dump or critical.
**         Dump-only tables cannot be double-buffered or critical.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_INVALID_OPTIONS     \copydoc CFE_TBL_ERR_INVALID_OPTIONS
**
*/
CFE_Status_t CFE_TBL_ValidateTableOptions(const char *Name, uint16 TblOptionFlags);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Allocates memory for a table buffer
**
** \par Description
**         Allocates a memory buffer for the table buffer of a table that is being registered.
**         If successful, the buffer is zeroed out.  This is a helper routine that is applicable
**         to any table buffer including temporary working/load buffers, primary or secondary buffers
**         of double-buffered tables.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**
*/
CFE_Status_t CFE_TBL_AllocateTableLoadBuffer(CFE_TBL_LoadBuff_t *LoadBuff, size_t Size);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Allocates the primary memory buffer for a table
**
** \par Description
**         Allocates the memory buffer for a single-buffered table, or the first buffer for a
**         double-buffered table that is being registered. If successful, the buffer is zeroed out.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**
*/
CFE_Status_t CFE_TBL_AllocatePrimaryBuffer(CFE_TBL_RegistryRec_t *RegRecPtr, size_t Size);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Allocates the secondary memory buffer for a double-buffered table
**
** \par Description
**         Allocates the secondary memory buffer for a double-buffered table that is
**         being registered. If successful, the buffer is zeroed out, and the
**         DoubleBuffered flag is set to true.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**
*/
CFE_Status_t CFE_TBL_AllocateSecondaryBuffer(CFE_TBL_RegistryRec_t *RegRecPtr, size_t Size);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes a Table Registry Entry
**
** \par Description
**         Initializes a Table Registry Entry for a table that is being registered
**
** \par Assumptions, External Events, and Notes:
**          None
**
*/
void CFE_TBL_InitTableRegistryEntry(CFE_TBL_RegistryRec_t *RegRecPtr, size_t Size,
                                    CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr, const char *TblName,
                                    uint16 TblOptionFlags);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Restore the contents of a table from the Critical Data Store (if it exists)
**
** \par Description
**          This function restores the contents of the specified table from the Critical
**          Data Store (CDS), if a copy of the table contents exists there.
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_INFO_RECOVERED_TBL      \copydoc CFE_TBL_INFO_RECOVERED_TBL
**
*/
CFE_Status_t CFE_TBL_RestoreTableDataFromCDS(CFE_TBL_RegistryRec_t *RegRecPtr, const char *AppName, const char *Name,
                                             CFE_TBL_CritRegRec_t *CritRegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Register a table with the Critical Table Registry
**
** \par Description
**        This function registers a table with the Critical Table Registry. The fields of
**        the Critical Table Registry Record are initialized and then the data is copied
**        to the Critical Data Store (CDS).
**
** \par Assumptions, External Events, and Notes:
**          None
**
*/
void CFE_TBL_RegisterWithCriticalTableRegistry(CFE_TBL_CritRegRec_t *CritRegRecPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                                               const char *TblName);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Generic iterator for access descriptors associated with a Table Registry
**
** \par Description
**        This function iterates through the list of access descriptors that are associated
**        with the given table registry entry.  The user-supplied callback function will be
**        invoked for each access descriptor, with the opaque pointer argument passed as a
**        parameter.
**
** \par Assumptions, External Events, and Notes:
**        The caller should ensure that the list is not modified by other threads during this call
**
** \param RegRecPtr The pointer to the registry record
** \param Func The function to invoke for each access descriptor
** \param Arg Opaque argument, passed through to function
*/
void CFE_TBL_ForeachAccessDescriptor(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescFunc_t Func, void *Arg);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Handle iterator function that increments a counter
**
** \par Description
**        When used with CFE_TBL_ForeachAccessDescriptor() this will count the number of entries
**
** \par Assumptions, External Events, and Notes:
**        This is declared here so it can be used in several places that count descriptors
**
** \param AccessDescPtr Pointer to descriptor entry, not used
** \param Arg a pointer to a uint32 value that is incremented on each call
*/
void CFE_TBL_CountAccessDescHelper(CFE_TBL_AccessDescriptor_t *AccessDescPtr, void *Arg);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes a handle link
**
** \par Description
**        Sets the handle link to initial condition, where it is not a member of any list
**        After this call, CFE_TBL_HandleLinkIsAttached() on this link will always return false
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param LinkPtr Pointer to link entry to initialize
*/
void CFE_TBL_HandleLinkInit(CFE_TBL_HandleLink_t *LinkPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Checks if a handle link is attached to another entry
**
** \par Description
**        This will return true if the passed-in link is attached to another list node,
**        indicating it is part of a list.  Conversely, this will return false if the
**        link is not attached to another node, indicating a singleton or empty list.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param LinkPtr Pointer to link entry to check
** \retval true if the link node is part of a list (attached)
** \retval false if the link node is not part of a list (detached)
*/
bool CFE_TBL_HandleLinkIsAttached(CFE_TBL_HandleLink_t *LinkPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Removes the given access descriptor from the registry list
**
** \par Description
**        This will disassociate the access descriptor from the table registry record by
**        removing/extracting the access descriptor from the linked list
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param RegRecPtr The table registry record that is associated with the access descriptor
** \param AccessDescPtr The access descriptor that is to be removed from the list
*/
void CFE_TBL_HandleListRemoveLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccessDescPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Inserts the given access descriptor into the registry list
**
** \par Description
**        This will associate the access descriptor with the table registry record by
**        inserting the access descriptor into the linked list
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param RegRecPtr The table registry record that should be associated with the access descriptor
** \param AccessDescPtr The access descriptor that is to be added to the list
*/
void CFE_TBL_HandleListInsertLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccessDescPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Gets the ID of the next buffer to use on a double-buffered table
**
** \par Description
**        This returns the identifier for the local table buffer that should be
**        loaded next.
**
** \par Assumptions, External Events, and Notes:
**        This is not applicable to single-buffered tables.
**
** \param RegRecPtr The table registry record
** \returns Identifier of next buffer to use
*/
CFE_TBL_LoadBuffId_t CFE_TBL_GetNextLocalBufferId(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Gets the currently-active buffer pointer for a table
**
** \par Description
**        This returns a pointer to the currently active table buffer.  On a single-buffered
**        table, this is always the first/only buffer.  This function never returns NULL, as
**        all tables have at least one buffer.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param RegRecPtr The table registry record
** \returns Pointer to the active table buffer
*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetActiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Gets the inactive buffer pointer for a table
**
** \par Description
**        This returns a pointer to inactive table buffer.  On a double-buffered table
**        this refers to whichever buffer is _not_ currently active (that is, the opposite
**        buffer from what is returned by CFE_TBL_GetActiveBuffer()).
**
**        On a single-buffered, if there is a load in progress that is utilizing one of the
**        global/shared load buffers, then this returns a pointer to that buffer.  If there
**        is no load in progress, this returns NULL to indicate there is no inactive buffer.
**
** \par Assumptions, External Events, and Notes:
**        This funtion may return NULL if there is no inactive buffer associated with the table
**
** \param RegRecPtr The table registry record
** \returns Pointer to the inactive table buffer
*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetInactiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Gets the buffer pointer for a table based on the selection enum
**
** \par Description
**        Gets either the active buffer (see CFE_TBL_GetActiveBuffer()) or the inactive
**        buffer (see CFE_TBL_GetInactiveBuffer()) based on the BufferSelect parameter.
**
** \par Assumptions, External Events, and Notes:
**        This funtion may return NULL if there is no buffer associated with the table
**        This will send an event if the BufferSelect parameter is not valid
**
** \param RegRecPtr The table registry record
** \param BufferSelect The buffer to obtain (active or inactive)
** \returns Pointer to the selected table buffer
*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetSelectedBuffer(CFE_TBL_RegistryRec_t *     RegRecPtr,
                                              CFE_TBL_BufferSelect_Enum_t BufferSelect);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Checks if a validation request is pending and clears the request
**
** \par Description
**        This checks the given flag (which is a request ID) to determine if a table validation
**        request is pending.
**
**        If no validation request is pending, this returns NULL and nothing else is done.
**
**        If a validation request is pending, then this clears the request (by writing
**        #CFE_TBL_VALRESULTID_UNDEFINED to the request flag) and returns a pointer to the
**        corresponding Validation Result buffer that refers to that request.  The request
**        should be in the PENDING state.
**
** \par Assumptions, External Events, and Notes:
**        This will clear the flag if there was a pending request, as it is expected that the
**        caller will be performing the validation at this time.
**
** \param ValIdPtr Pointer to the table validation request flag (from the table registry entry)
** \returns Pointer to the request, if a request was pending
** \retval NULL if no request was pending
*/
CFE_TBL_ValidationResult_t *CFE_TBL_CheckValidationRequest(CFE_TBL_ValidationResultId_t *ValIdPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Drops the working buffer associated with this registry entry
**
** \par Description
**        If the registry entry had a working (i.e. load in progress) buffer associated with it,
**        this drops the association and returns the buffer to the pool (if applicable).
**
**        On a single-buffered table, the working buffers come from a small set of shared/global
**        temporary buffers.  This function will reset the temporary buffer back to the available state.
**
**        On a double-buffered table, the working buffer is the inactive buffer, and it does not get
**        deleted.  This function only marks the inactive buffer as _not_ being used for loading.
**
** \par Assumptions, External Events, and Notes:
**        This always clears the "load in progress" status, if it was set.
**
** \param[inout] RegRecPtr Pointer to the registry entry to operate on
*/
void CFE_TBL_DiscardWorkingBuffer(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Gets the working buffer associated with this registry entry
**
** \par Description
**        If the registry entry had a working (i.e. load in progress) buffer associated with it,
**        this gets the buffer and returns a pointer to it.
**
**        On a single-buffered table, the working buffers come from a small set of shared/global
**        temporary buffers.  On a double-buffered table, the working buffer is the inactive buffer.
**
** \par Assumptions, External Events, and Notes:
**        This is only used to recover a buffer to load that was already started.  It does not
**        start a new load (i.e. it will _not_ allocate a working buffer, if there was none associated
**        with the table).  If a table load was not already in progress, this returns NULL.
**
** \param[inout] RegRecPtr Pointer to the registry entry to operate on
** \returns Pointer to working buffer
** \retval NULL if no table load is in progress
*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetLoadInProgressBuffer(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Acquires a temporary working buffer from the shared pool
**
** \par Description
**        This finds an unused buffer within the set of shared buffers intended to facilitate loading tables
**
**        This type of buffer is used as a temporary holding location for table data, so it can be validated
**        before copying it into the actual table buffer.  The buffer must be returned to the pool when the
**        loading is complete.
**
** \par Assumptions, External Events, and Notes:
**        These temporary buffers are a limited resource, constrained by #CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS
**
** \returns Pointer to working buffer
** \retval NULL if no working buffers are available
*/
CFE_TBL_LoadBuff_t *CFE_TBL_AcquireGlobalLoadBuff(void);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Acquires a the inactive buffer from a double buffered table
**
** \par Description
**        Double buffered table have a dedicated (inactive) buffer that is used for loading, rather than using
**        a temporary buffer from the shared pool.  This function gets a pointer to the inactive buffer and also
**        confirms that no other tasks are referencing the buffer (i.e. it has exclusive access) such that it
**        is safe to load new data into it.
**
** \par Assumptions, External Events, and Notes:
**        If there is a task/app that is holding a reference to the buffer, loading is not possible.  Table users
**        must release the address when they are not actively accessing the table data to permit reloading.
**
** \param[inout] RegRecPtr Pointer to the registry entry to operate on
** \returns Pointer to working buffer
** \retval NULL if no working buffers are available
*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetInactiveBufferExclusive(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Deallocates a single table buffer
**
** \par Description
**        Returns the block of memory associated with the table load buffer into the TBL memory pool.
**        This is only intended to be used when un-registering a table.
**
** \par Assumptions, External Events, and Notes:
**        This function de-allocates the buffer completely, and should only be used when un-registering
**        a table completely from the system.
**
**        In normal usage, where tables remain registered, buffers are marked as unused but remain
**        allocated for future use.
**
** \param[inout] BuffPtr Pointer to the buffer to deallocate
*/
void CFE_TBL_DeallocateBuffer(CFE_TBL_LoadBuff_t *BuffPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Deallocates all table buffers assocated with a table registry entry
**
** \par Description
**        Returns all the block of memory associated with the table buffers into the TBL memory pool.
**        This is only intended to be used when un-registering a table.
**
**        If the table is single-buffered, this de-allocates the first (and only) buffer
**        If the table is double-buffered, this de-allocates both buffers.
**
** \par Assumptions, External Events, and Notes:
**        This function de-allocates the buffers completely, and should only be used when un-registering
**        a table completely from the system.
**
** \param[inout] RegRecPtr Pointer to the registry entry to operate on
*/
void CFE_TBL_DeallocateAllBuffers(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set the metadata for the last table update
 *
 * Stores the given string as the most recent source of data, and
 * sets the last update time to the indicated time.  Note that in some
 * cases (e.g. restore from CDS) the real timestamp of the data may be
 * earlier than the current time which is why it is passed in.
 *
 * @param[inout]   RegRecPtr   pointer to Registry table entry
 * @param[in] DataSource string indicating to the source of data
 * @param[in] UpdateTime time stamp of the source of data
 */
void CFE_TBL_RegRecResetLoadInfo(CFE_TBL_RegistryRec_t *RegRecPtr, const char *DataSource,
                                 CFE_TIME_SysTime_t UpdateTime);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks the given string buffer with a modified tag
 *
 * Adds an indicator -- traditionally "(*)" -- at the end of the string
 * that conveys the table has been modified since it was last loaded.  This
 * indicator shows the operator that the contents in memory are not identical to
 * what was in the file that was loaded.
 *
 * @param[inout]   NameBufPtr   pointer to table name buffer
 * @param[in] NameBufSize Size if name buffer
 */
void CFE_TBL_MarkNameAsModified(char *NameBufPtr, size_t NameBufSize);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Flags the table as being modified from its original data
 *
 * This marks the internal metadata to indicate the the table data
 * has been changed by the application since the initial load from the file.
 *
 * @param[inout]   RegRecPtr   pointer to Registry table entry
 */
void CFE_TBL_RegRecSetModifiedFlag(CFE_TBL_RegistryRec_t *RegRecPtr);

/*
** Globals specific to the TBL module
*/
extern CFE_TBL_Global_t CFE_TBL_Global;

#endif /* CFE_TBL_INTERNAL_H */
