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
 * Header definining the table services registry record and related methods
 */

#ifndef CFE_TBL_REGREC_H
#define CFE_TBL_REGREC_H

/*
** Required header files...
*/
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_platform_cfg.h"
#include "cfe_es_api_typedefs.h"
#include "cfe_sb_api_typedefs.h"
#include "cfe_time_api_typedefs.h"

#include "cfe_tbl_resource.h"

#include "cfe_tbl_handlelink.h"
#include "cfe_tbl_dumpctl.h"
#include "cfe_tbl_valresult.h"
#include "cfe_tbl_loadbuff.h"

/*********************  Macro and Constant Type Definitions   ***************************/

/**
 * A structure that encapsulates all of the optional table features
 *
 * Options are the set of booleans that are defined/set at the time
 * of registration and remain constant thereafter, for the duration
 * of the table lifetime.
 */
typedef struct CFE_TBL_TableConfig
{
    char Name[CFE_TBL_MAX_FULL_NAME_LEN]; /**< \brief Processor specific table name */

    size_t Size; /**< \brief Size of table in bytes */

    CFE_TBL_CallbackFuncPtr_t ValidationFuncPtr; /**< \brief Ptr to Owner App's function that validates tbl contents */

    bool DoubleBuffered; /**< \brief Flag indicating Table has a dedicated inactive buffer */
    bool DumpOnly;       /**< \brief Flag indicating Table is NOT to be loaded */
    bool UserDefAddr;    /**< \brief Flag indicating Table address was defined by Owner Application */
    bool Critical;       /**< \brief Flag indicating whether table is a Critical Table */

    /* The EDS ID is only used in EDS builds, it is stored here as a uint32. */
    uint32 EdsId;

} CFE_TBL_TableConfig_t;

/**
 * A structure that encapsulates all of the table status flags
 *
 * Status flags are the set of booleans that reflect the current state
 * and can be updated throughout the table lifetime.
 */
typedef struct CFE_TBL_TableStatus
{
    CFE_TBL_LoadBuffId_t ActiveBufferId; /**< \brief Identifier of the currently active buffer */
    CFE_TBL_LoadBuffId_t PrevBufferId;   /**< \brief Identifier of the previously active buffer */
    CFE_TBL_LoadBuffId_t NextBufferId;   /**< \brief Identifier of the next buffer (pending activation) */

    CFE_TIME_SysTime_t TimeOfLastUpdate; /**< \brief Time when Table was last updated */

    bool IsModified; /**< \brief Indicates if this table is modified since loading */

    char LastFileLoaded[OS_MAX_PATH_LEN]; /**< \brief Filename of last file loaded into table */
} CFE_TBL_TableStatus_t;

/**
 * A structure that encapsulates the update notification configuration
 *
 * This is the configuration associated with update notifications.  These
 * are an optional table feature, but are configured after registration.
 */
typedef struct CFE_TBL_TableUpdateNotify
{
    bool Enabled; /**< \brief Flag indicating Table Services should notify owning App via message
                                  when table requires management */

    CFE_MSG_FcnCode_t FcnCode; /**< \brief Command Code of an associated management notification message */
    CFE_SB_MsgId_t    MsgId;   /**< \brief Message ID of an associated management notification message */
    uint32            Param;   /**< \brief Opaque parameter of an associated management notification message */

} CFE_TBL_TableUpdateNotify_t;

/*******************************************************************************/
/**   \brief Table Registry Record
**
**     Table Registry Record that contains all information associated
**     with a particular table.
*/
struct CFE_TBL_RegistryRec
{
    CFE_TBL_RegId_t RegId;

    CFE_ES_AppId_t       OwnerAppId; /**< \brief Application ID of App that Registered Table */
    CFE_TBL_LoadBuff_t   Buffers[2]; /**< \brief Active and Inactive Buffer Pointers */
    CFE_TBL_HandleLink_t AccessList; /**< \brief Linked List of associated access descriptors */
    CFE_TBL_ValidationResultId_t
        ValidateActiveId; /**< \brief Index to Validation Request on Active Table Result data */
    CFE_TBL_ValidationResultId_t
                         ValidateInactiveId; /**< \brief Index to Validation Request on Inactive Table Result data */
    CFE_TBL_DumpCtrlId_t DumpControlId;      /**< \brief Index to Dump Control Block */
    CFE_ES_CDSHandle_t   CDSHandle;          /**< \brief Handle to Critical Data Store for Critical Tables */

    CFE_TBL_TableConfig_t       Config;
    CFE_TBL_TableStatus_t       Status;
    CFE_TBL_TableUpdateNotify_t Notify;
};

/*****************************  Function Prototypes   **********************************/

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  REGISTRY RECORD TABLE ACCESSORS ~~~
 *
 * These operate on CFE_TBL_RegistryRec_t* and CFE_TBL_RegId_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a Registry Record ID
 *
 * Calculates the array position/index of the global array entry for
 * the given registry ID.
 *
 * @param[in]  RegId the ID/handle of the registry record to retrieve
 * @param[out] Idx     Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_RegId_ToIndex(CFE_TBL_RegId_t RegId, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the registry table entry correlating with a given registry ID.
 *
 * This only returns a pointer to the table entry where the record
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for applications, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned record is a match to the expected ID before using
 * or modifying the data within the returned record pointer.
 *
 * The CFE_TBL_RegRecIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_RegRecIsMatch()
 *
 * @param[in]   RegId   the registry ID to locate
 * @return pointer to Registry Table entry for the given registry ID, or NULL if out of range
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByID(CFE_TBL_RegId_t RegId);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate a Registry Record ID by name
 *
 * Returns a pointer to the registry table with a matching name
 *
 * @param[in] Name the name to search for (fully-qualified as AppName.TableNam)
 * @returns   Pointer to table entry with matching name
 * @retval    NULL if no matching entry was found
 */
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByName(const char *Name);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a registry record is a match for the given RegId
 *
 * This routine confirms that the previously-located registry record is valid
 * and matches the expected registry ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateRegRecByID()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateRegRecByID
 *
 * @param[in]   RegRecPtr   pointer to registry table entry, or NULL
 * @param[in]   RegId       expected registry ID
 * @returns true if the entry matches the given registry ID
 */
static inline bool CFE_TBL_RegRecIsMatch(const CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_RegId_t RegId)
{
    return (RegRecPtr != NULL && CFE_TBL_REGID_EQ(RegRecPtr->RegId, RegId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the ID value from a registry record
 *
 * This routine converts the registry record pointer to its corresponding ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to table entry
 * @returns ID of entry
 */
static inline CFE_TBL_RegId_t CFE_TBL_RegRecGetID(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->RegId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a registry entry is in use or free/empty
 *
 * This routine checks if the entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to registry entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_RegRecIsUsed(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return CFE_TBL_REGID_IS_VALID(RegRecPtr->RegId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a registry record as in use (not avaliable)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the registry ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to registry entry
 * @param[in]   PendingId   the ID of this entry that will be set
 */
static inline void CFE_TBL_RegRecSetUsed(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_ResourceId_t PendingId)
{
    RegRecPtr->RegId = CFE_TBL_REGID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks a registry entry as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any registry ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   RegRecPtr   pointer to registry entry
 */
static inline void CFE_TBL_RegRecSetFree(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->RegId = CFE_TBL_REGID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the configuration associated with this table
 *
 * Returns a pointer to the configuration structure, which has all user-selectable options.
 * @note  this table config is set during registration and does not change thereafter
 *
 * @param[in]   RegRecPtr   pointer to registry entry
 * @returns   Pointer to config struct
 */
static inline const CFE_TBL_TableConfig_t *CFE_TBL_RegRecGetConfig(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return &RegRecPtr->Config;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the validation function associated with this table
 *
 * Returns a pointer to the validation function
 *
 * @note  this configuration is set during registration and does not change thereafter
 *
 * @param[in]   RegRecPtr   pointer to registry entry
 * @returns   Pointer to validation function
 */
static inline CFE_TBL_CallbackFuncPtr_t CFE_TBL_RegRecGetValidationFunc(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return CFE_TBL_RegRecGetConfig(RegRecPtr)->ValidationFuncPtr;
}

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
 * @brief Obtain the name associated with the registry entry
 *
 * Returns the name field from within the registry entry
 *
 * @note The name buffer in the registry contains the fully-qualified name (AppName.TableName)
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @returns Pointer to Table name
 */
static inline const char *CFE_TBL_RegRecGetName(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->Config.Name;
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
    return RegRecPtr->Config.Size;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if a load is in progress
 *
 * Checks if the table is currently being loaded
 *
 * A load in progress is simply that there is a "next" buffer associated with this registry,
 * it will be true even if the buffer is not valid
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @returns true if a load is in progress
 */
static inline size_t CFE_TBL_RegRecIsLoadInProgress(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return CFE_TBL_LOADBUFFID_IS_VALID(RegRecPtr->Status.NextBufferId);
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
static inline CFE_TBL_LoadBuffId_t CFE_TBL_RegRecGetLoadInProgress(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    return RegRecPtr->Status.NextBufferId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Set the load in progress identifier
 *
 * Marks the registry entry as having a load in progress
 *
 * @param[inout] RegRecPtr      pointer to Registry table entry
 * @param[in]    NextBufferId identifier of the pending buffer being loaded
 */
static inline void CFE_TBL_RegRecSetLoadInProgress(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_LoadBuffId_t NextBufferId)
{
    RegRecPtr->Status.NextBufferId = NextBufferId;
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
    RegRecPtr->Status.NextBufferId = CFE_TBL_LOADBUFFID_UNDEFINED;
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
    return RegRecPtr->Status.LastFileLoaded;
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
    return RegRecPtr->Status.TimeOfLastUpdate;
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
    /* The active buffer index is not set until the first load */
    return CFE_TBL_LOADBUFFID_IS_VALID(RegRecPtr->Status.ActiveBufferId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Checks if a table load is pending activation
 *
 * Pending activation means that there is a load in progress that has been fully
 * validated and is ready to invoke the activate table command to make it active
 *
 * @param[in]   RegRecPtr   pointer to Registry table entry
 * @retval false if there is no activation pending
 * @retval true if there is a activation pending
 */
bool CFE_TBL_RegRecIsPendingActivation(const CFE_TBL_RegistryRec_t *RegRecPtr);

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
    return RegRecPtr->Status.IsModified;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Determine the next ID to use for a table registry entry
 *
 * Obtains an ID value that is usable for a new registry.  If no registry entries
 * are available, then UNDEFINED is returned.
 *
 * @returns ID to use for next result, or UNDEFINED if no slots available
 */
CFE_ResourceId_t CFE_TBL_GetNextRegId(void);

/*---------------------------------------------------------------------------------------*/
/**
 * Test if a slot corresponding to a pending ID is used
 *
 * This is an internal helper function for CFE_ResourceId_FindNext(), and not
 * typically called directly. It is prototyped here for unit testing.
 *
 * @returns True if used, False if available
 */
bool CFE_TBL_CheckRegistrySlotUsed(CFE_ResourceId_t CheckId);

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
static inline CFE_TBL_LoadBuff_t *CFE_TBL_GetActiveBuffer(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* This will return NULL if the index is not valid, as in a non-loaded table */
    return CFE_TBL_LocateLoadBufferByID(RegRecPtr->Status.ActiveBufferId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Sets the active buffer of the table
 *
 * @param[inout] RegRecPtr  pointer to Registry table entry
 * @param[in]    BuffPtr    pointer to working buffer that becomes the active buffer
 */
static inline void CFE_TBL_SetActiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_LoadBuff_t *BuffPtr)
{
    RegRecPtr->Status.PrevBufferId   = RegRecPtr->Status.ActiveBufferId;
    RegRecPtr->Status.ActiveBufferId = CFE_TBL_LoadBufferGetID(BuffPtr);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Gets the previous buffer of the table
 *
 * @param[inout] RegRecPtr  pointer to Registry table entry
 * @returns Pointer to the active table buffer
 */
static inline CFE_TBL_LoadBuff_t *CFE_TBL_GetPreviousBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* This will return NULL if the index is not valid */
    return CFE_TBL_LocateLoadBufferByID(RegRecPtr->Status.PrevBufferId);
}

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
 * @brief Flags the table as being modified from its original data
 *
 * This marks the internal metadata to indicate the the table data
 * has been changed by the application since the initial load from the file.
 *
 * @param[inout]   RegRecPtr   pointer to Registry table entry
 */
void CFE_TBL_RegRecSetModifiedFlag(CFE_TBL_RegistryRec_t *RegRecPtr);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Sets up the registry record according to the supplied configuration
 *
 * This is intended to be called only on new table registrations, after allocating a registry
 * entry, to configure the entry according to the user-defined options.
 *
 * Note that this does not validate the options, it simply copies them verbatim.  It is
 * expected that the options have already been validated for consistency.
 *
 * @param[inout] RegRecPtr   pointer to Registry table entry
 * @param[in]    OwnerAppId  application ID of the table owner
 * @param[in]    ReqCfg      Requested table configuration
 */
void CFE_TBL_SetupTableRegistryRecord(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_ES_AppId_t OwnerAppId,
                                      const CFE_TBL_TableConfig_t *ReqCfg);

#endif /* CFE_TBL_REGREC_H */
