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
 * Header definining the table services transaction methods
 */

#ifndef CFE_TBL_TRANSACTION_H
#define CFE_TBL_TRANSACTION_H

/*
 * Required header files...
 */
#include "cfe_error.h"
#include "cfe_es_api_typedefs.h"
#include "cfe_tbl_api_typedefs.h"
#include "cfe_platform_cfg.h"
#include "cfe_tbl_resource.h"
#include "cfe_tbl_eventids.h"

/*********************  Type Definitions   ***************************/

#define CFE_TBL_MAX_EVENTS_PER_TXN 8

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

typedef struct CFE_TBL_TxnEvent
{
    /* The ID that should be broadcast via EVS */
    uint16 EventId;

    /*
     * Two general-purpose integer values to further identify the cause - these are
     * context dependent as to what they mean.  These may be status codes.
     *
     * As a general rule of thumb, if the event was triggered from a comparison
     * between two values, then register the actual observed value (or status code)
     * as event data 1, and the expected value as event data 2, if applicable.
     *
     * These are only used when assembling the event message.
     */
    int32 EventData1; /**< Actual value observed */
    int32 EventData2; /**< Expected value, if applicable, or other relevant value */

} CFE_TBL_TxnEvent_t;

/**
 * Callback function for event processing
 */
typedef bool (*CFE_TBL_TxnEventProcFunc_t)(const CFE_TBL_TxnEvent_t *, void *);

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
    CFE_ES_AppId_t     AppId;
    CFE_TBL_HandleId_t Handle;
    CFE_TBL_RegId_t    RegId;

    uint32 RegLockCount;
    uint32 CallContext;

    char AppNameBuffer[CFE_MISSION_MAX_API_LEN];

    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    uint32             NumPendingEvents;
    CFE_TBL_TxnEvent_t PendingEvents[CFE_TBL_MAX_EVENTS_PER_TXN];

} CFE_TBL_TxnState_t;

/*****************************  Simple Accessors   **********************************/

/*
 * These functions retrieve specific items from the transaction object.  The intent
 * is to (eventually) replace individual local stack variables with these accessors.
 */

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the table handle.
 *
 * This is internal (preferred) form of the table handle which is unique and type safe.
 */
static inline CFE_TBL_HandleId_t CFE_TBL_TxnHandle(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->Handle;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the table handle as an unsigned long (for logging/events)
 */
static inline unsigned long CFE_TBL_TxnHandleAsULong(const CFE_TBL_TxnState_t *Txn)
{
    return CFE_RESOURCEID_TO_ULONG(CFE_TBL_TxnHandle(Txn));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the access descriptor object
 */
static inline CFE_TBL_AccessDescriptor_t *CFE_TBL_TxnAccDesc(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->AccDescPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the registry entry ID
 */
static inline CFE_TBL_RegId_t CFE_TBL_TxnRegId(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->RegId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the table registry entry ID as an unsigned long (for logging/events)
 */
static inline unsigned long CFE_TBL_TxnRegIdAsULong(const CFE_TBL_TxnState_t *Txn)
{
    return CFE_TBL_REGID_INT(CFE_TBL_TxnRegId(Txn));
}

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the registry record object
 */
static inline CFE_TBL_RegistryRec_t *CFE_TBL_TxnRegRec(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->RegRecPtr;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the calling context AppID
 *
 * Otherwise known as "ThisAppId" in many existing functions
 */
static inline CFE_ES_AppId_t CFE_TBL_TxnAppId(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->AppId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * Gets the calling context application ID as an unsigned long (for logging/events)
 */
static inline unsigned long CFE_TBL_TxnAppIdAsULong(const CFE_TBL_TxnState_t *Txn)
{
    return CFE_RESOURCEID_TO_ULONG(CFE_TBL_TxnAppId(Txn));
}

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Returns the calling application name
 *
 * \par Description
 *        Returns a pointer to a buffer containing the name of the currently-running application
 *        The buffer is local to the transaction object and is initialized on the first invocation.
 *
 * \par Assumptions, External Events, and Notes:
 *        None
 *
 * \param[inout] Txn The transaction object to operate on
 */
const char *CFE_TBL_TxnAppNameCaller(CFE_TBL_TxnState_t *Txn);

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
CFE_Status_t CFE_TBL_TxnStartFromHandle(CFE_TBL_TxnState_t *Txn, CFE_TBL_HandleId_t TblHandle, uint32 AllowedContext);

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
 * \brief Adds an event to the transaction for deferred reporting
 *
 * \par Description
 *        Adds an event to the list of events that need to be reported as part of this transaction
 *        Events will be handled in a future call to CFE_TBL_TxnProcessEvents() once everything
 *        is completed and nothing is in a locked or intermediate state.
 *
 * \par Assumptions, External Events, and Notes:
 *        EventData1 and EventData2 are aribitrary integers to capture any relevant detail about
 *        the event that occurred.  If the event is the result of a status code from CFE, PSP, OSAL,
 *        or any other library that uses integer status codes, it should be passed as EventData1.  If
 *        the event was triggered as the result of a comparison, then the reference/expected value
 *        should be passed as EventData2.  If there is no relevant detail, pass 0 for these values.
 *
 *        The suggestions above are just guidelines for consistency -- the ultimate meaning/use of the
 *        event context data is governed by the callback function provided to CFE_TBL_TxnProcessEvents().
 *
 * \param[inout] Txn     The transaction object to operate on
 * \param[in]    EventId The transaction object to operate on
 * \param[in]    EventData1 Arbitrary integer context data, may be a status code or actual value observed
 * \param[in]    EventData2 Second integer context data, may be the expected/needed value in comparison
 */
void CFE_TBL_TxnAddEvent(CFE_TBL_TxnState_t *Txn, uint16 EventId, int32 EventData1, int32 EventData2);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Gets the number of events pending in the transaction
 *
 * \par Description
 *        Gets the number of events currently pending for deferred reporting
 *
 * \par Assumptions, External Events, and Notes:
 *        None
 *
 * \param[in] Txn       The transaction object to operate on
 *
 * \returns the number of events that were successfully processed
 */
uint32 CFE_TBL_TxnGetEventCount(const CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Process the list of events for deferred reporting
 *
 * \par Description
 *        Iterates over the set of deferred events, invoking the given routine for each event.
 *        The passed-in EventProc routine will be called for each event, which in turn should
 *        propagate the event detail to the user via the appropriate call to CFE_EVS or sys log.
 *
 * \par Assumptions, External Events, and Notes:
 *        The opaque argument is not modified or used directly, it is passed through to EventProc.
 *
 * \param[in]  Txn       The transaction object to operate on
 * \param[in]  EventProc User-defined routine to call for each event, which does the reporting
 * \param[in]  Arg       Opaque argument to pass to EventProc
 *
 * \returns the number of events that were successfully processed
 */
uint32 CFE_TBL_TxnProcessEvents(const CFE_TBL_TxnState_t *Txn, CFE_TBL_TxnEventProcFunc_t EventProc, void *Arg);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Clear the list of events in the transaction
 *
 * \par Description
 *        Resets the transaction deferred event list to an empty state, as it is in a newly
 *        initialized transaction object.
 *
 * \par Assumptions, External Events, and Notes:
 *        This is only needed if events are reported in multiple sets or stages in a transaction,
 *        allowing the list to be reset after each stage.  For simple transactions this is not
 *        necessary.
 *
 * \param[inout] Txn       The transaction object to operate on
 */
void CFE_TBL_TxnClearEvents(CFE_TBL_TxnState_t *Txn);

#endif /* CFE_TBL_TRANSACTION_H */
