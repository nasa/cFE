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

#ifndef CFE_TBL_TRANSACTION_H
#define CFE_TBL_TRANSACTION_H

/*
 * Required header files...
 */
#include "cfe_es_api_typedefs.h"
#include "cfe_tbl_api_typedefs.h"
#include "cfe_platform_cfg.h"
#include "cfe_tbl_resource.h"
#include "cfe_tbl_eventids.h"

/*********************  Type Definitions   ***************************/

/**
 * Bit mask values for context of a table services transaction
 *
 * An "Accessor" of a table has a handle to it, and a corresponding Access Descriptor.
 * An "Owner" of a table is the one that registered it.
 *
 * Note it is possible for multiple bits to be set.  In particular, the original
 * registrant of a table is generally both the owner and an accessor of the table.
 */
typedef enum CFE_TBL_TxnContext
{
    CFE_TBL_TxnContext_UNDEFINED = 0,

    /**< The caller is table services itself (administrative action) */
    CFE_TBL_TxnContext_TABLE_SERVICES = 0x1,

    /** The caller is the app that originally registered the table (owner) */
    CFE_TBL_TxnContext_OWNER_APP = 0x2,

    /** The caller is app that has an accessor */
    CFE_TBL_TxnContext_ACCESSOR_APP = 0x4,

    /** The caller is an app that is not associated with the table */
    CFE_TBL_TxnContext_OTHER_APP = 0x8,

    /** All context allowed (convenience value) */
    CFE_TBL_TxnContext_ALL = 0x0F

} CFE_TBL_TxnContext_Enum_t;

/**
 * The table transaction object
 *
 * This tracks all the relevant information from the current API request,
 * including the caller context (appID), the table handle/access descriptor
 * and registry entry being acted upon, whether the registry is locked, etc.
 *
 * All public APIs should use fields within this object rather than
 * managing these data items individually on the stack.
 *
 * The object can be extended as necessary.  Ideally, it should track everything
 * that is in the process of being changed, such that changes can be reliably
 * and consistently un-done if a later step in the process fails.  The goal
 * should always be to either make a complete transaction, or leave the global
 * state as it was at the start of the transaction (never something "half-done").
 *
 * Importantly, the transaction object serves as a local snapshot of the relevant
 * values from the registry, so that if they need to be refereced outside of a locked
 * context (e.g. for event or syslog reporting) the copies in this object can still
 * be used after the transaction completes.
 */
typedef struct CFE_TBL_TxnState
{
    CFE_ES_AppId_t   AppId;
    CFE_TBL_Handle_t Handle;
    CFE_TBL_RegId_t  RegId;

    uint32 CallContext;
    uint16 PendingEventId;
    bool   RegIsLocked;

    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

} CFE_TBL_TxnState_t;

/*****************************  Simple Accessors   **********************************/

/*
 * These functions retrieve specific items from the transaction object.  The intent
 * is to (eventually) replace individual local stack variables with these accessors.
 */

/**
 * Gets the table handle
 */
static inline CFE_TBL_Handle_t CFE_TBL_TxnHandle(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->Handle;
}

static inline unsigned long CFE_TBL_TxnHandleAsULong(const CFE_TBL_TxnState_t *Txn)
{
    return (unsigned long)CFE_TBL_TxnHandle(Txn);
}

/**
 * Gets the access descriptor object
 */
static inline CFE_TBL_AccessDescriptor_t *CFE_TBL_TxnAccDesc(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->AccessDescPtr;
}

/**
 * Gets the registry entry ID
 */
static inline CFE_TBL_RegId_t CFE_TBL_TxnRegId(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->RegId;
}

static inline unsigned long CFE_TBL_TxnRegIdAsULong(const CFE_TBL_TxnState_t *Txn)
{
    return (unsigned long)CFE_TBL_TxnRegId(Txn);
}

/**
 * Gets the registry record object
 */
static inline CFE_TBL_RegistryRec_t *CFE_TBL_TxnRegRec(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->RegRecPtr;
}

/**
 * Gets the calling context AppID
 *
 * Otherwise known as "ThisAppId" in many existing functions
 */
static inline CFE_ES_AppId_t CFE_TBL_TxnAppId(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->AppId;
}

static inline unsigned long CFE_TBL_TxnAppIdAsULong(const CFE_TBL_TxnState_t *Txn)
{
    return CFE_RESOURCEID_TO_ULONG(CFE_TBL_TxnAppId(Txn));
}

/*****************************  Function Prototypes   **********************************/

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
 * \brief Set a transaction to a safe initial state
 *
 * \par Description
 *        Clears the transaction object and sets everything to initial state
 *
 *        After this call, all internal object pointers will be NULL and all handles/IDs will
 *        be set the the respective INVALID value.
 *
 *        The "CheckContext" parameter indicates whether the calling context (AppID)
 *        is relevant for the current operation.  If set true, then this will invoke
 *        CFE_ES_GetAppID() to determine the calling context, which can be queried
 *        using CFE_TBL_TxnAppId().  If set to false, this call into ES will be skipped.
 *        This is intended as an optimization as to save the time cost of the call into
 *        ES to determine the app ID (which is small but not zero).
 *
 * \par Assumptions, External Events, and Notes:
 *        This call does _NOT_ lock the registry.  When starting a transaction using this
 *        method, the caller must manage the registry lock via CFE_TBL_TxnLockRegistry() and
 *        CFE_TBL_TxnUnlockRegistry().
 *
 * \param[inout] Txn The transaction object to operate on
 * \param[in]    CheckContext Whether to determine the AppID of the caller (calling context)
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnInit(CFE_TBL_TxnState_t *Txn, bool CheckContext);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set a transaction to operate on an existing table handle
 *
 * \par Description
 *        Clears the transaction object and sets everything to operate on an existing
 *        table handle.
 *
 *        After this call successfully completes, all internal object pointers will be
 *        pointing at the relevant global table entries and the registry will be left
 *        in a __LOCKED__ state.  As this operates on an existing table handle, both
 *        the access descriptor and registry record must map to valid entries.
 *
 *        __IMPORTANT__: If this call returns successfully, it MUST be followed by a call
 *        to CFE_TBL_TxnFinish() to unlock the registry and clean anything else up.
 *
 *        The "AllowedContext" parameter should be passed as a bitmask of values defined
 *        in CFE_TBL_TxnContext_Enum_t, indicating which callers are allowable.  This is
 *        intended to facilitate access controls (i.e. ensuring that the caller matches
 *        the access descriptor, typically).
 *
 * \par Assumptions, External Events, and Notes:
 *        Upon successful return, the table registry will be left in a LOCKED state.  This
 *        call must be followed by a call to CFE_TBL_TxnFinish() to release that lock.
 *
 *        If this call returns with an error, the registry will be left in an UNLOCKED state
 *        and no changes will be made (the caller should report the error and return).
 *
 * \param[inout] Txn The transaction object to operate on
 * \param[in]    TblHandle The table handle to operate on
 * \param[in]    AllowedContext The allowed calling context(s)
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnStartFromHandle(CFE_TBL_TxnState_t *Txn, CFE_TBL_Handle_t TblHandle, uint32 AllowedContext);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Set a transaction to operate on an existing table name
 *
 * \par Description
 *        Clears the transaction object and sets everything to operate on an existing
 *        table which is located by name.
 *
 *        After this call successfully completes, all internal object pointers will be
 *        pointing at the relevant global table entries and the registry will be left
 *        in a __LOCKED__ state.  However, as this operates on a table name (not a handle)
 *        only the registry record will point to a valid registry entry.  The access
 *        descriptor will be left unset (NULL), as there isn't one in this context.
 *
 *        __IMPORTANT__: If this call returns successfully, it MUST be followed by a call
 *        to CFE_TBL_TxnFinish() to unlock the registry and clean anything else up.
 *
 *        The "AllowedContext" parameter should be passed as a bitmask of values defined
 *        in CFE_TBL_TxnContext_Enum_t, indicating which callers are allowable.  This is
 *        intended to facilitate access controls (i.e. ensuring that the caller matches
 *        the access descriptor, typically).
 *
 * \par Assumptions, External Events, and Notes:
 *        Upon successful return, the table registry will be left in a LOCKED state.  This
 *        call must be followed by a call to CFE_TBL_TxnFinish() to release that lock.
 *
 *        If this call returns with an error, the registry will be left in an UNLOCKED state
 *        and no changes will be made (the caller should report the error and return).
 *
 * \param[inout] Txn The transaction object to operate on
 * \param[in]    TblName The table name to find in the registry
 * \param[in]    AllowedContext The allowed calling context(s)
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnStartFromName(CFE_TBL_TxnState_t *Txn, const char *TblName, uint32 AllowedContext);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Completes the referenced transaction
 *
 * \par Description
 *        This function releases any resource(s) that were held as part of the transaction
 *        and performs any related post-transaction cleanup, if needed.
 *
 *        __IMPORTANT__: This function MUST be invoked after any successful call to a
 *        Transaction Start routine (CFE_TBL_TxnStartFromName(), CFE_TBL_TxnStartFromHandle())
 *
 * \par Assumptions, External Events, and Notes:
 *        Identifiers within the transaction object will remain valid, but pointers to table
 *        and dedscriptor records should NOT be used after finishing a transaction.
 *
 * \param[inout] Txn The transaction object to operate on
 */
void CFE_TBL_TxnFinish(CFE_TBL_TxnState_t *Txn);

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
 * \brief Finds the access descriptor associated with the current registry entry, if any
 *
 * \par Description
 *      For a transaction object that is referring to a registry entry, this searches
 *      for an access descriptor that matches this combination of appid (calling context)
 *      and table registry entry.
 *
 *      For example, the CFE_TBL_TxnStartFromName() function will find only a registry
 *      entry (as these have the name) but there could be several access descriptors
 *      pointing at the same table registration entry, as is the case with shared tables.
 *
 * \par Assumptions, External Events, and Notes:
 *      The registry should be locked (as part of the transaction) prior to invoking this
 *
 * \param[inout] Txn The transaction object to operate on
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_FindAccessDescriptorForSelf(CFE_TBL_TxnState_t *Txn);

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

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Returns any pending non-error status code for the specified table.
 *
 * \par Description
 *        Returns any pending non-error status code for the specified table.
 *
 * \par Assumptions, External Events, and Notes:
 *        Note: This function assumes the TblHandle has been validated.
 *
 * \param[inout] Txn The transaction object to operate on
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnGetNextNotification(CFE_TBL_TxnState_t *Txn);

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
 * \brief Locates a free Access Descriptor in the Table Handles Array.
 *
 * \par Description
 *        Locates a free Access Descriptor in the Table Handles Array.
 *
 *        If successful, the internal pointer will be set to the newly allocated
 *        access descriptor.  The accessor functions CFE_TBL_TxnAccDesc() and
 *        CFE_TBL_TxnHandle() may be used to retrieve the pointer and handle,
 *        respectively.
 *
 * \par Assumptions, External Events, and Notes:
 *        Note: This function assumes the registry has been locked.
 *        No association is made between the accessor and the registry object here.  The
 *        association is made via a separate call.  This simply finds an open entry.
 *
 * \param[inout] Txn The transaction object to operate on
 *
 * \returns CFE_SUCCESS normally, or relevent CFE status code
 * \retval #CFE_SUCCESS \copydoc CFE_SUCCESS
 */
CFE_Status_t CFE_TBL_TxnAllocateHandle(CFE_TBL_TxnState_t *Txn);

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

#endif /* CFE_TBL_TRANSACTION_H */
