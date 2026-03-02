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
 * Header definining the table services access descriptor and related methods
 */

#ifndef CFE_TBL_ACCDESC_H
#define CFE_TBL_ACCDESC_H

/*
** Required header files
*/
#include "common_types.h"
#include "cfe_error.h"
#include "cfe_tbl.h"
#include "cfe_platform_cfg.h"
#include "cfe_resourceid.h"

#include "cfe_es_api_typedefs.h"
#include "cfe_tbl_api_typedefs.h"

#include "cfe_tbl_resource.h"
#include "cfe_tbl_handlelink.h"
#include "cfe_tbl_transaction.h"

/*************************************************************************/

/*******************************************************************************/
/**   \brief Application to Table Access Descriptor
**
**     Table Access Descriptor data structure that contains information necessary
**     to access the table without interfering with other threads.  TblHandles are
**     an index into an array of Access Descriptors, thus identifying a specific
**     AccessDescriptor for a particular Application for a table.
*/
struct CFE_TBL_AccessDescriptor
{
    CFE_TBL_HandleId_t HandleId;

    CFE_ES_AppId_t       AppId;       /**< \brief Application ID to verify access */
    CFE_TBL_RegId_t      RegIndex;    /**< \brief Index into Table Registry (a.k.a. - Global Table #) */
    CFE_TBL_HandleLink_t Link;        /**< \brief Linkage into list of access descriptors for the table */
    bool                 LockFlag;    /**< \brief Indicates whether thread is currently accessing table data */
    bool                 Updated;     /**< \brief Indicates table has been updated since last GetAddress call */
    CFE_TBL_LoadBuffId_t BufferIndex; /**< \brief Index of buffer currently being used */
};

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

/*
 * ---------------------------------------------------------------------------------------
 *
 *     ~~~  ACCESS DESCRIPTOR TABLE ACCESSORS ~~~
 *
 * These operate on CFE_TBL_AccessDescriptor_t* and CFE_TBL_HandleId_t types
 *
 * ---------------------------------------------------------------------------------------
 */

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the array index correlating with a table handle/access ID
 *
 * Calculates the array position/index of the global array entry for
 * the given handle.
 *
 * @param[in]  TblHandle the ID/handle of the access descriptor to retrieve
 * @param[out] Idx       Output buffer to store the index
 * @returns    #CFE_SUCCESS if conversion successful. @copydoc CFE_SUCCESS
 *             #CFE_ES_ERR_RESOURCEID_NOT_VALID if ID is outside valid range
 */
CFE_Status_t CFE_TBL_Handle_ToIndex(CFE_TBL_HandleId_t TblHandle, uint32 *Idx);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Locate the access descriptor entry correlating with a given table handle.
 *
 * This only returns a pointer to the table entry where the descriptor
 * should reside, but does _not_ actually check/validate the entry.
 *
 * If the passed-in ID parameter is not within the acceptable range of ID
 * values for tasks, such that it could never be valid under
 * any circumstances, then NULL is returned.  Otherwise, a pointer to the
 * corresponding table entry is returned, indicating the location where
 * that ID should reside, if it is currently in use.
 *
 * @note This only returns where the ID should reside, not that it actually
 * resides there.  If looking up an existing ID, then caller must additionally
 * confirm that the returned descriptor is a match to the expected ID before using
 * or modifying the data within the returned descriptor pointer.
 *
 * The CFE_TBL_AccDescIsMatch() function can be used to check/confirm
 * if the returned table entry is a positive match for the given ID.
 *
 * @sa CFE_TBL_AccDescIsMatch()
 *
 * @param[in]   TblHandle   the table handle to locate
 * @return pointer to decriptor table entry for the given table handle, or NULL if out of range
 */
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateAccDescByHandle(CFE_TBL_HandleId_t TblHandle);

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Get the Handle ID from an an access descriptor pointer
 *
 * This routine converts a pointer to a handle table entry to an ID
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccDescPtr   pointer to access descriptor
 * @returns TblHandle of entry
 */
static inline CFE_TBL_HandleId_t CFE_TBL_AccDescGetHandle(const CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    return AccDescPtr->HandleId;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an Access Descriptor is a match for the given TblHandle
 *
 * This routine confirms that the previously-located record is valid
 * and matches the expected Task ID.
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * This function may be used in conjunction with CFE_TBL_LocateAccDescByHandle()
 * to confirm that the located record is a positive match to the expected ID.
 * As such, the record pointer is also permitted to be NULL, to alleviate the
 * need for the caller to handle this possibility explicitly.
 *
 * Once a record pointer has been successfully validated using this routine,
 * it may be safely passed to all other internal functions.
 *
 * @sa CFE_TBL_LocateAccDescByHandle
 *
 * @param[in]   AccDescPtr   pointer to access descriptor table entry
 * @param[in]   TblHandle       The expected table handle to verify
 * @returns true if the descriptor entry matches the given table handle
 */
static inline bool CFE_TBL_AccDescIsMatch(const CFE_TBL_AccessDescriptor_t *AccDescPtr, CFE_TBL_HandleId_t TblHandle)
{
    return (AccDescPtr != NULL && CFE_TBL_HandleID_IsEqual(AccDescPtr->HandleId, TblHandle));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Check if an access descriptor is in use or free/empty
 *
 * This routine checks if the entry is in use or if it is free
 *
 * As this dereferences fields within the record, global data must be
 * locked prior to invoking this function.
 *
 * @note This internal helper function must only be used on result pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccDescPtr   pointer to descriptor entry
 * @returns true if the entry is in use/configured, or false if it is free/empty
 */
static inline bool CFE_TBL_AccDescIsUsed(const CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    return (CFE_TBL_HandleID_IsDefined(AccDescPtr->HandleId));
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks an access descriptor entry as in use (not avaliable)
 *
 * This sets the internal field(s) within this entry, and marks
 * it as being associated with the given handle ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccDescPtr   pointer to descriptor entry
 * @param[in]   PendingId    the ID of this entry that will be set
 */
static inline void CFE_TBL_AccDescSetUsed(CFE_TBL_AccessDescriptor_t *AccDescPtr, CFE_ResourceId_t PendingId)
{
    AccDescPtr->HandleId = CFE_TBL_HANDLEID_C(PendingId);
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Marks an access descriptor entry as available (not in use)
 *
 * This clears the internal field(s) within this entry, and marks
 * it as not being associated with any table handle ID.
 *
 * @note This internal helper function must only be used on record pointers
 * that are known to refer to an actual table location (i.e. non-null).
 *
 * @param[in]   AccDescPtr   pointer to descriptor entry
 */
static inline void CFE_TBL_AccDescSetFree(CFE_TBL_AccessDescriptor_t *AccDescPtr)
{
    AccDescPtr->HandleId = CFE_TBL_HANDLEID_UNDEFINED;
    AccDescPtr->AppId    = CFE_ES_APPID_UNDEFINED;
}

/*---------------------------------------------------------------------------------------*/
/**
 * @brief Determine the next ID to use for a table handle
 *
 * Obtains an ID value that is usable for a new access descriptor.  If no descriptor
 * result entries are available, then UNDEFINED is returned.
 *
 * @returns ID to use for next result, or UNDEFINED if no slots available
 */
CFE_ResourceId_t CFE_TBL_GetNextTableHandle(void);

/*---------------------------------------------------------------------------------------*/
/**
 * Test if a slot corresponding to a pending ID is used
 *
 * This is an internal helper function for CFE_ResourceId_FindNext(), and not
 * typically called directly. It is prototyped here for unit testing.
 *
 * @returns True if used, False if available
 */
bool CFE_TBL_CheckAccessDescriptorSlotUsed(CFE_ResourceId_t CheckId);

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
CFE_Status_t CFE_TBL_TxnAllocateAccDesc(CFE_TBL_TxnState_t *Txn);

/*---------------------------------------------------------------------------------------*/
/**
 * \brief Releases the Access Descriptor.
 *
 * \par Description
 *        Frees the referenced Access Descriptor and updates all references
 *
 *        If successful, the access descriptor will be returned to the pool
 *        for future use
 *
 * \par Assumptions, External Events, and Notes:
 *        Note: This function assumes the registry has been locked.
 *
 * \param[inout] Txn The transaction object to operate on
 */
void CFE_TBL_TxnReleaseAccDesc(CFE_TBL_TxnState_t *Txn);

#endif /* CFE_TBL_ACCDESC_H */
