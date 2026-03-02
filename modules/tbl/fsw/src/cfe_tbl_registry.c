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
 * Implementation of table services registry methods
 */

/*
** Required header files...
*/
#include "cfe_tbl_registry.h"
#include "cfe_tbl_module_all.h"
#include "cfe_tbl_codec.h"

#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TxnLockRegistry(CFE_TBL_TxnState_t *Txn)
{
    if (Txn->RegLockCount == 0)
    {
        CFE_TBL_LockRegistry();
    }
    ++Txn->RegLockCount;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TxnUnlockRegistry(CFE_TBL_TxnState_t *Txn)
{
    if (Txn->RegLockCount > 0)
    {
        --Txn->RegLockCount;
        if (Txn->RegLockCount == 0)
        {
            CFE_TBL_UnlockRegistry();
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnCheckConfig(CFE_TBL_TxnState_t *Txn, CFE_TBL_TableConfig_t *ReqCfg, const char *BaseName,
                                    uint16 TblOptionFlags, size_t Size, CFE_TBL_CallbackFuncPtr_t ValidationFuncPtr)
{
    CFE_Status_t Status;

    /* There is no way to check the validity of this pointer (NULL is OK) */
    ReqCfg->ValidationFuncPtr = ValidationFuncPtr;

    /* Generate the full application-specific table name (this also detects errors) */
    Status = CFE_TBL_ValidateTableName(ReqCfg, BaseName, CFE_TBL_TxnAppNameCaller(Txn));
    if (Status != CFE_SUCCESS)
    {
        /* Table name failed sanity check */
        CFE_ES_WriteToSysLog("%s: bad table name: '%s' (0x%lx)\n", __func__, BaseName, (unsigned long)Status);
    }
    else
    {
        /*
         * This initializes all the boolean fields in the Requested Options struct.
         * Note that this stage these are only the _requested_ table options.  They
         * will become the _actual_ table options as they are properly configured later.
         */
        Status = CFE_TBL_ValidateTableOptions(ReqCfg, TblOptionFlags);
        if (Status != CFE_SUCCESS)
        {
            /* Table cannot be critial/double buffered or must be dump-only and wasn't specified as so */
            CFE_ES_WriteToSysLog("%s: bad TblOptionFlags combination for '%s' (0x%lx)\n", __func__, BaseName,
                                 (unsigned long)TblOptionFlags);
        }
        else
        {
            /* Table size validation depends on whether double buffered was requested */
            Status = CFE_TBL_ValidateTableSize(ReqCfg, Size);
            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Table '%s' has invalid size (%lu)\n", __func__, BaseName,
                                     (unsigned long)Size);
            }
            else
            {
                Status = CFE_TBL_ValidateCodecConfig(ReqCfg);
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnRemoveAccessLink(CFE_TBL_TxnState_t *Txn)
{
    int32                       Status     = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr  = CFE_TBL_TxnRegRec(Txn);

    /*
     * NOTE: In all cases where this is invoked, the registry should
     * already be locked under the transaction object
     */
    CFE_TBL_HandleListRemoveLink(RegRecPtr, AccDescPtr);

    /* If this was the last Access Descriptor for this table, we can free the memory buffers as well */
    if (!CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->OwnerAppId) && !CFE_TBL_HandleLinkIsAttached(&RegRecPtr->AccessList))
    {
        /* Only free memory that we have allocated.  If the image is User Defined, then don't bother */
        if (!CFE_TBL_RegRecGetConfig(RegRecPtr)->UserDefAddr)
        {
            /* If there was a working buffer, release it.  This does nothing if there was no working/load buffer. */
            CFE_TBL_DiscardWorkingBuffer(RegRecPtr);

            /* Free memory directly allocated to buffers for this table (non-shared). */
            CFE_TBL_DeallocateAllBuffers(RegRecPtr);
        }

        /* Return the Access Descriptor to the pool */
        CFE_TBL_RegRecSetFree(RegRecPtr);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnGetTableAddress(CFE_TBL_TxnState_t *Txn, void **TblPtr)
{
    int32                       Status;
    CFE_TBL_AccessDescriptor_t *AccDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr  = CFE_TBL_TxnRegRec(Txn);
    CFE_TBL_LoadBuff_t *        ActiveBuffPtr;

    /* If table is unowned, then owner must have unregistered it when we weren't looking */
    if (!CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->OwnerAppId))
    {
        Status  = CFE_TBL_ERR_UNREGISTERED;
        *TblPtr = NULL;

        CFE_ES_WriteToSysLog("%s: App(%lu) attempt to access unowned Tbl Handle=%lu\n", __func__,
                             CFE_TBL_TxnAppIdAsULong(Txn), CFE_TBL_TxnHandleAsULong(Txn));
    }
    else if (!CFE_TBL_RegRecIsTableLoaded(RegRecPtr))
    {
        Status  = CFE_TBL_ERR_NEVER_LOADED;
        *TblPtr = NULL;
    }
    else
    {
        /* Table Registry Entry is valid */
        /* Save the buffer we are using in the access descriptor */
        /* This is used to ensure that if the buffer becomes inactive while */
        /* we are using it, no one will modify it until we are done */

        ActiveBuffPtr = CFE_TBL_GetActiveBuffer(RegRecPtr);

        /* Lock the table and return the current pointer */
        AccDescPtr->LockFlag    = true;
        AccDescPtr->BufferIndex = CFE_TBL_LoadBufferGetID(ActiveBuffPtr);

        *TblPtr = (void *)CFE_TBL_LoadBuffGetReadPointer(ActiveBuffPtr);

        /* Return any pending warning or info status indicators */
        Status = CFE_TBL_TxnGetNextNotification(Txn);

        /* Clear Table Updated Notify Bit so that caller only gets it once */
        AccDescPtr->Updated = false;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnFindRegByName(CFE_TBL_TxnState_t *Txn, const char *TblName)
{
    CFE_Status_t           Status;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    RegRecPtr = CFE_TBL_LocateRegRecByName(TblName);
    if (RegRecPtr == NULL)
    {
        Status = CFE_TBL_ERR_INVALID_NAME;
    }
    else
    {
        Txn->RegId     = CFE_TBL_RegRecGetID(RegRecPtr);
        Txn->RegRecPtr = RegRecPtr;

        Status = CFE_SUCCESS;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnAllocateRegistryEntry(CFE_TBL_TxnState_t *Txn)
{
    CFE_Status_t           Status;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_ResourceId_t       PendingId;

    CFE_TBL_TxnLockRegistry(Txn);

    /* Search Registry for free entry */

    PendingId = CFE_TBL_GetNextRegId();
    RegRecPtr = CFE_TBL_LocateRegRecByID(CFE_TBL_REGID_C(PendingId));

    /* Check to make sure there was a handle available */
    if (RegRecPtr == NULL)
    {
        Txn->RegRecPtr = NULL;
        Txn->RegId     = CFE_TBL_REGID_UNDEFINED;

        Status = CFE_TBL_ERR_REGISTRY_FULL;
    }
    else
    {
        /* Initialize the Registry Record */
        CFE_TBL_InitRegistryRecord(RegRecPtr);
        CFE_TBL_RegRecSetUsed(RegRecPtr, PendingId);

        Txn->RegRecPtr = RegRecPtr;
        Txn->RegId     = CFE_TBL_RegRecGetID(RegRecPtr);

        CFE_TBL_Global.LastRegId = PendingId;

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
CFE_Status_t CFE_TBL_TxnCheckDuplicateRegistration(CFE_TBL_TxnState_t *Txn, const char *TblName, size_t Size)
{
    CFE_Status_t           Status = CFE_SUCCESS;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_ES_AppId_t         ThisAppId;

    /* Check for duplicate table name */
    Status = CFE_TBL_TxnFindRegByName(Txn, TblName);

    /* Check to see if table is already in the registry */
    if (Status == CFE_SUCCESS)
    {
        /* Get pointer to Registry Record Entry to speed up processing */
        RegRecPtr = CFE_TBL_TxnRegRec(Txn);
        ThisAppId = CFE_TBL_TxnAppId(Txn);

        /* If this app previously owned the table, then allow them to re-register */
        if (CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, ThisAppId))
        {
            /* If the new table is the same size as the old, then no need to reallocate memory */
            if (Size != CFE_TBL_RegRecGetSize(RegRecPtr))
            {
                /* If the new size is different, the old table must be deleted but this */
                /* function can't do that because it is probably shared and is probably */
                /* still being accessed. Someone else will need to clean up this mess.  */
                Status = CFE_TBL_ERR_DUPLICATE_DIFF_SIZE;

                CFE_ES_WriteToSysLog("%s: Attempt to register existing table ('%s') with different size(%d!=%d)\n",
                                     __func__, TblName, (int)Size, (int)CFE_TBL_RegRecGetSize(RegRecPtr));
            }
            else
            {
                /* Fill in the correct handle (already existing) */
                Status = CFE_TBL_FindAccessDescriptorForSelf(Txn);

                if (Status == CFE_SUCCESS)
                {
                    /* Warn calling application that this is a duplicate registration */
                    Status = CFE_TBL_WARN_DUPLICATE;
                }
            }
        }
        else /* Duplicate named table owned by another Application */
        {
            Status = CFE_TBL_ERR_DUPLICATE_NOT_OWNED;

            CFE_ES_WriteToSysLog("%s: App(%lu) Registering Duplicate Table '%s' owned by App(%lu)\n", __func__,
                                 CFE_RESOURCEID_TO_ULONG(ThisAppId), TblName,
                                 CFE_RESOURCEID_TO_ULONG(RegRecPtr->OwnerAppId));
        }
    }
    else /* Table not already in registry */
    {
        /* Locate empty slot in table registry */
        Status = CFE_TBL_TxnAllocateRegistryEntry(Txn);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TxnConnectAccessDescriptor(CFE_TBL_TxnState_t *Txn)
{
    /* Initialize the Table Access Descriptor */
    CFE_TBL_AccessDescriptor_t *AccDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr  = CFE_TBL_TxnRegRec(Txn);

    AccDescPtr->LockFlag = false;

    AccDescPtr->AppId    = CFE_TBL_TxnAppId(Txn);
    AccDescPtr->RegIndex = CFE_TBL_TxnRegId(Txn);

    CFE_TBL_TxnLockRegistry(Txn);

    /* Check current state of table in order to set Notification flags properly */
    if (CFE_TBL_RegRecIsMatch(RegRecPtr, AccDescPtr->RegIndex))
    {
        AccDescPtr->Updated = !RegRecPtr->Config.DumpOnly && CFE_TBL_RegRecIsTableLoaded(RegRecPtr);
        CFE_TBL_HandleListInsertLink(RegRecPtr, AccDescPtr);
    }
    else
    {
        AccDescPtr->Updated = false;
    }

    CFE_TBL_TxnUnlockRegistry(Txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnGetTableStatus(CFE_TBL_TxnState_t *Txn)
{
    int32                  Status;
    CFE_TBL_RegistryRec_t *RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    /* Perform validations prior to performing any updates */
    if (CFE_TBL_VALRESULTID_IS_VALID(RegRecPtr->ValidateActiveId) ||
        CFE_TBL_VALRESULTID_IS_VALID(RegRecPtr->ValidateInactiveId))
    {
        Status = CFE_TBL_INFO_VALIDATION_PENDING;
    }
    else if (CFE_TBL_RegRecIsPendingActivation(RegRecPtr))
    {
        Status = CFE_TBL_INFO_UPDATE_PENDING;
    }
    else if (CFE_TBL_DUMPCTRLID_IS_VALID(RegRecPtr->DumpControlId))
    {
        Status = CFE_TBL_INFO_DUMP_PENDING;
    }
    else
    {
        Status = CFE_SUCCESS;
    }

    return Status;
}
