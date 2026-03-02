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

#define CFE_TBL_NOT_FOUND CFE_TBL_REGID_UNDEFINED
#define CFE_TBL_NOT_OWNED CFE_ES_APPID_UNDEFINED

/*
 * Compatibility helpers: provide a bridge from the traditional table handles to the safer definition
 *
 * The external handle type (CFE_TBL_Handle_t) still exists only for the public API.  These wrappers
 * are intended to provide a conversion to facilitate transition of apps to use the improved typedef.
 *
 * In the preferred mode, CFE_TBL_Handle_t (external) and CFE_TBL_HandleId_t (internal) are a direct
 * typedef - so they are simply two different names for the same thing. (in the future this may be
 * consolidated again back to a single typedef once transition in CFS apps has been accomplished).
 *
 * In backward-compatible mode, the CFE_TBL_Handle_t (external) type is a simple int16 as it has always
 * been.  This will contain only the lower 16 bits of the full handle ID.  It is not as safe but
 * this is fully transparent to apps because it is still a simple 16 bit integer.
 */

#ifdef CFE_OMIT_DEPRECATED_6_8
/* Preferred versions (passthru/noop) */
#define CFE_TBL_HANDLE_EXPORT(x) (x)
#define CFE_TBL_HANDLE_IMPORT(x) (x)
#else
/* Backward compatible versions */
#define CFE_TBL_HANDLE_EXPORT(x) ((CFE_TBL_Handle_t)(CFE_TBL_HandleID_AsInt(x) - CFE_TBL_HANDLE_BASE))
#define CFE_TBL_HANDLE_IMPORT(x) CFE_TBL_HANDLEID_C(CFE_ResourceId_FromInteger((unsigned long)x + CFE_TBL_HANDLE_BASE))
#endif

/*****************************  Function Prototypes   **********************************/

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
** \brief Creates a Full Table name from application name and table name
**
** \par Description
**        Takes a given raw table name and combines it with the calling
**        Application's name to make the application specific name of the
**        form: "AppName.RawTableName"
**
**        The resulting string will be stored into the "Name" of the supplied table config
**
** \par Assumptions, External Events, and Notes:
**        AppName portion will be truncated to OS_MAX_API_NAME.
**
** \param[in, out] ReqCfg  Pointer to table configuration structure
** \param[in] TblName           Pointer to character string containing the table name.
** \param[in] AppName           Name of the application that owns the table
*/
CFE_Status_t CFE_TBL_ValidateTableName(CFE_TBL_TableConfig_t *ReqCfg, const char *TblName, const char *AppName);

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
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \retval #CFE_TBL_ERR_NO_BUFFER_AVAIL     \copydoc CFE_TBL_ERR_NO_BUFFER_AVAIL
**
*/
int32 CFE_TBL_GetWorkingBuffer(CFE_TBL_LoadBuff_t **WorkingBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr);

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
int32 CFE_TBL_UpdateInternal(CFE_TBL_HandleId_t TblHandle, CFE_TBL_RegistryRec_t *RegRecPtr,
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
CFE_Status_t CFE_TBL_ValidateTableSize(CFE_TBL_TableConfig_t *TableCfg, size_t Size);

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
CFE_Status_t CFE_TBL_ValidateTableOptions(CFE_TBL_TableConfig_t *TableCfg, uint16 TblOptionFlags);

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
CFE_Status_t CFE_TBL_AllocateTableLoadBuffer(CFE_TBL_LoadBuff_t *LoadBuffPtr, size_t Size);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes a Table Registry Entry
**
** \par Description
**         Initializes a Table Registry Entry for a table that is being registered
**
**         Specifically this handles the following aspects of the registration, which
**         are propagated to the table registry.
**           - Table buffer size
**           - User defined address flag (indicates there are NO local buffers if set)
**           - Double buffer flag (indicates there are TWO local buffers if set)

**         This also takes care of allocating the buffers as indicated
**
** \par Assumptions, External Events, and Notes:
**          None
**
** \retval #CFE_SUCCESS                     \copydoc CFE_SUCCESS
**
*/
CFE_Status_t CFE_TBL_SetupTableBuffers(CFE_TBL_RegistryRec_t *RegRecPtr, const CFE_TBL_TableConfig_t *ReqCfg);

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
CFE_Status_t CFE_TBL_RestoreTableDataFromCDS(CFE_TBL_RegistryRec_t *RegRecPtr);

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
** \param AccDescPtr Pointer to descriptor entry, not used
** \param Arg a pointer to a uint32 value that is incremented on each call
*/
void CFE_TBL_CountAccessDescHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg);

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
CFE_TBL_LoadBuff_t *CFE_TBL_AcquireGlobalLoadBuff(CFE_TBL_RegId_t PendingOwnerId);

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

/*
** Globals specific to the TBL module
*/
extern CFE_TBL_Global_t CFE_TBL_Global;

#endif /* CFE_TBL_INTERNAL_H */
