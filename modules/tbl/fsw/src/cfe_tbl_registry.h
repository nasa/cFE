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
 * Header definining the table services registry methods
 */

#ifndef CFE_TBL_REGISTRY_H
#define CFE_TBL_REGISTRY_H

/*
 * Required header files...
 */
#include "cfe_tbl_transaction.h"
#include "cfe_tbl_regrec.h"
#include "cfe_error.h"

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Locks access to the Table Registry
 *
 * \par Description
 *        Locks the Table Registry to prevent multiple tasks/threads
 *        from modifying it at once.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn The transaction object to operate on
 */
void CFE_TBL_TxnLockRegistry(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Unlocks access to the Table Registry
 *
 * \par Description
 *        Unlocks Table Registry to allow other tasks/threads to
 *        modify the Table Registry contents.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn The transaction object to operate on
 */
void CFE_TBL_TxnUnlockRegistry(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Validates the complete table configuration
**
** \par Description
**        The table configuration structure is filled in with all the correct
**        flags and values based on the information passed in, if the validation
**        is successful.
**
**        Takes a given raw table name and combines it with the calling
**        Application's name to make the application specific name of the
**        form: "AppName.BaseName"
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[inout] Txn               The transaction object to operate on
** \param[out]   ReqCfg            Pointer to table configuration structure
** \param[in]    BaseName          Base name for table (local) from caller
** \param[in]    TblOptionFlags    Requested option flags from caller
** \param[in]    Size              Requested size from caller
** \param[in]    ValidationFuncPtr Validation function pointer from caller
**
** \returns CFE_SUCCESS normally, or relevent CFE status code
** \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
*/
CFE_Status_t CFE_TBL_TxnCheckConfig(CFE_TBL_TxnState_t *Txn, CFE_TBL_TableConfig_t *ReqCfg, const char *BaseName,
                                    uint16 TblOptionFlags, size_t Size, CFE_TBL_CallbackFuncPtr_t ValidationFuncPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Removes Access Descriptor from Table's linked list of Access Descriptors
 *
 * \par Description
 *        Removes the given Access Descriptor from the Linked List
 *        of Access Descriptors associated with the table specified
 *        in the Access Descriptor itself.
 *
 * \par Assumptions, External Events, and Notes:
 *
 * \param[inout] Txn The transaction object to operate on
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnRemoveAccessLink(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Returns the Registry Index for the specified Table Name
 *
 * \par Description
 *        Locates given Table Name in the Table Registry and
 *        returns the appropriate Registry Index.
 *
 * \par Assumptions, External Events, and Notes:
 *          None
 *
 * \param[inout] Txn The transaction object to operate on
 * \param[in]  TblName - Pointer to character string containing complete
 *                       Table Name (of the format "AppName.TblName").
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnFindRegByName(CFE_TBL_TxnState_t *Txn, const char *TblName);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Locates a free slot in the Table Registry.
 *
 * \par Description
 *        Locates a free slot in the Table Registry.
 *
 *        If successful, the internal pointer will be set to the newly allocated
 *        registry record.  The accessor functions CFE_TBL_TxnRegRec() and
 *        CFE_TBL_TxnRegId() may be used to retrieve the pointer and handle,
 *        respectively.
 *
 * \par Assumptions, External Events, and Notes:
 *        Note: This function assumes the registry has been locked.
 *
 * \param[inout] Txn The transaction object to operate on
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnAllocateRegistryEntry(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Checks if a table is already registered in the Table Registry
 *
 * \par Description
 *         This routine searches the Table Registry for a table with the specified name,
 *         owning app and size. If a match is found, the same handle is returned. If a
 *         match is not found, the function will locate a free slot in the table registry
 *         (unless it's already full).
 *
 * \par Assumptions, External Events, and Notes:
 *         A return value of CFE_SUCCESS indicates that this is NOT a duplicate registration,
 *         meaning it is OK to proceed with creating a new table entry.  If this is a duplicate,
 *         this may return one of the "warning" status codes indicating the nature of duplication.
 *
 * \param[inout] Txn The transaction object to operate on
 * \param[in]    TblName The name of the table that is the pending in a registration request
 * \param[in]    Size The size of th table that is pending in a registration request
 *
 * \returns CFE_SUCCESS or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnCheckDuplicateRegistration(CFE_TBL_TxnState_t *Txn, const char *TblName, size_t Size);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Connects a Table Access Descriptor to the current Registry record
 *
 * \par Description
 *        As part of setting up a new table, the transaction should have selected a
 *        (pending) access descriptor/handle and a registry record to use for the table.
 *
 *        This call will make the necessary connections to associate the access descriptor
 *        with the registry record selected in the transaction object
 *
 * \par Assumptions, External Events, and Notes:
 *        None
 *
 * \param[inout] Txn The transaction object to operate on
 */
void CFE_TBL_TxnConnectAccessDescriptor(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Gets the table status associated with a transaction
 *
 * \par Description
 *        Returns the current status of the table being referred to in the transaction object
 *
 * \par Assumptions, External Events, and Notes:
 *        None
 *
 * \param[inout] Txn The transaction object to operate on
 *
 * \returns CFE_SUCCESS if nothing is pending, or relevent CFE status code ("info" status)
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnGetTableStatus(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Obtains the data address for the specified table
 *
 * \par Description
 *        Validates the given TblHandle, finds the location of the
 *        Table data and returns the address to the data to the caller.
 *
 * \par Assumptions, External Events, and Notes:
 *        -# It is possible that an Application that was sharing a table
 *           would discover, upon making this call, that the table has
 *           been unregistered by another Application.  In this situation,
 *           this function would return #CFE_TBL_ERR_UNREGISTERED.
 *
 * \param[inout] Txn The transaction object to operate on
 * \param[out]   TblPtr    Pointer to pointer that will hold address of data upon return.
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnGetTableAddress(CFE_TBL_TxnState_t *Txn, void **TblPtr);

#endif /* CFE_TBL_REGISTRY_H */
