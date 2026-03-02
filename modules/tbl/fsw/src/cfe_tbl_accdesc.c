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
 * Implementation of table services access descriptor methods
 */

/*
** Includes
*/

#include "cfe_tbl_accdesc.h"
#include "cfe_tbl_regrec.h"
#include "cfe_tbl_registry.h"
#include "cfe_tbl_internal.h"
#include "cfe_core_resourceid_basevalues.h"
#include "cfe_resourceid_api_typedefs.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_InitAccessDescriptor(CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    memset(AccessDescPtr, 0, sizeof(*AccessDescPtr));

    CFE_TBL_HandleLinkInit(&AccessDescPtr->Link);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Handle_ToIndex(CFE_TBL_HandleId_t TblHandle, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(TblHandle), CFE_TBL_HANDLE_BASE,
                                  CFE_PLATFORM_TBL_MAX_NUM_HANDLES, Idx);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_AccessDescriptor_t *CFE_TBL_LocateAccDescByHandle(CFE_TBL_HandleId_t TblHandle)
{
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    uint32                      Idx;

    if (CFE_TBL_Handle_ToIndex(TblHandle, &Idx) == CFE_SUCCESS)
    {
        AccDescPtr = &CFE_TBL_Global.Handles[Idx];
    }
    else
    {
        AccDescPtr = NULL;
    }

    return AccDescPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ResourceId_t CFE_TBL_GetNextTableHandle(void)
{
    return CFE_ResourceId_FindNext(CFE_TBL_Global.LastHandle, CFE_PLATFORM_TBL_MAX_NUM_HANDLES,
                                   CFE_TBL_CheckAccessDescriptorSlotUsed);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_CheckAccessDescriptorSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_TBL_AccessDescriptor_t *AccDescPtr;

    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    AccDescPtr = CFE_TBL_LocateAccDescByHandle(CFE_TBL_HANDLEID_C(CheckId));
    return (AccDescPtr == NULL || CFE_TBL_AccDescIsUsed(AccDescPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnAllocateAccDesc(CFE_TBL_TxnState_t *Txn)
{
    CFE_Status_t                Status;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_ResourceId_t            PendingId;

    CFE_TBL_TxnLockRegistry(Txn);

    /* Search Access Descriptor Array for free Descriptor */
    PendingId  = CFE_TBL_GetNextTableHandle();
    AccDescPtr = CFE_TBL_LocateAccDescByHandle(CFE_TBL_HANDLEID_C(PendingId));

    /* Check to make sure there was a handle available */
    if (AccDescPtr == NULL)
    {
        Txn->AccDescPtr = NULL;
        Txn->Handle     = CFE_TBL_HANDLEID_UNDEFINED;

        Status = CFE_TBL_ERR_HANDLES_FULL;
    }
    else
    {
        /* Initialize the Table Access Descriptor */
        CFE_TBL_InitAccessDescriptor(AccDescPtr);
        CFE_TBL_AccDescSetUsed(AccDescPtr, PendingId);

        Txn->AccDescPtr = AccDescPtr;
        Txn->Handle     = CFE_TBL_AccDescGetHandle(AccDescPtr);

        CFE_TBL_Global.LastHandle = PendingId;

        Status = CFE_SUCCESS;
    }

    CFE_TBL_TxnUnlockRegistry(Txn);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TxnReleaseAccDesc(CFE_TBL_TxnState_t *Txn)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;

    RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    /* Verify that the application unregistering the table owns the table */
    if (CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, CFE_TBL_TxnAppId(Txn)))
    {
        /* Mark table as free, although, technically, it isn't free until the */
        /* linked list of Access Descriptors has no links in it.              */
        /* NOTE: Allocated memory is freed when all Access Links have been    */
        /*       removed.  This allows Applications to continue to use the    */
        /*       data until they acknowledge that the table has been removed. */

        /* Mark it as "un-owned" only -- this is not free yet */
        RegRecPtr->OwnerAppId = CFE_ES_APPID_UNDEFINED;
    }

    /* Remove the Access Descriptor Link from linked list */
    CFE_TBL_TxnRemoveAccessLink(Txn);

    /* Return the Access Descriptor to the pool */
    CFE_TBL_AccDescSetFree(CFE_TBL_TxnAccDesc(Txn));
}
