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

/*
** File: cfe_tbl_transaction.c
**
** Purpose:  cFE Table Services (TBL) utility function source file
**
** Author:   D. Kobe/the Hammers Company, Inc.
**
** Notes:
**
*/

/*
** Required header files...
*/
#include "cfe_tbl_module_all.h"

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
CFE_Status_t CFE_TBL_TxnInit(CFE_TBL_TxnState_t *Txn, bool CheckContext)
{
    CFE_Status_t Status;

    memset(Txn, 0, sizeof(*Txn));

    /*
     * Initialize the refs to a safe value.  Preferably
     * these should be zero but currently they are non-zero
     * and thus "memset()" above does not make them safe.
     */
    Txn->Handle = CFE_TBL_BAD_TABLE_HANDLE;
    Txn->RegId  = CFE_TBL_NOT_FOUND;

    /* Check to make sure App ID is legit */
    if (CheckContext)
    {
        Status = CFE_ES_GetAppID(&Txn->AppId);
    }
    else
    {
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
const char *CFE_TBL_TxnAppNameCaller(CFE_TBL_TxnState_t *Txn)
{
    const char *Result;

    Result = Txn->AppNameBuffer;

    if (Txn->AppNameBuffer[0] == 0)
    {
        /*
         * This should not attempt to get the name while the reg is locked.
         * The typical things that need the name are for purposes like syslog or
         * event sending, and these should only be done after unlocking the reg.
         */
        if (Txn->RegLockCount != 0)
        {
            /* If this is seen in a log, it is a bug in the caller that should be fixed */
            Result = "[!LOCKED!]";
        }
        else
        {
            CFE_ES_GetAppName(Txn->AppNameBuffer, Txn->AppId, sizeof(Txn->AppNameBuffer));
        }
    }

    return Result;
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
CFE_Status_t CFE_TBL_TxnStartFromName(CFE_TBL_TxnState_t *Txn, const char *TblName, uint32 AllowedContext)
{
    CFE_Status_t Status;

    Status = CFE_TBL_TxnInit(Txn, AllowedContext != CFE_TBL_TxnContext_UNDEFINED);

    if (Status == CFE_SUCCESS)
    {
        CFE_TBL_TxnLockRegistry(Txn);

        Status = CFE_TBL_TxnFindRegByName(Txn, TblName);
    }

    if (Status != CFE_SUCCESS)
    {
        /* If returning with an error, should also unlock the registry */
        CFE_TBL_TxnFinish(Txn);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnStartFromHandle(CFE_TBL_TxnState_t *Txn, CFE_TBL_Handle_t TblHandle, uint32 AllowedContext)
{
    CFE_Status_t                Status;
    uint32                      AccessAllowed;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    AccessAllowed = 0;
    Status        = CFE_TBL_TxnInit(Txn, AllowedContext != CFE_TBL_TxnContext_UNDEFINED);

    if (Status == CFE_SUCCESS)
    {
        Txn->Handle = TblHandle;

        /*
         * Check if the caller is actually table services
         * (this is like the "root user" - most/all actions allowed)
         */
        if (CFE_RESOURCEID_TEST_EQUAL(Txn->AppId, CFE_TBL_Global.TableTaskAppId))
        {
            Txn->CallContext |= CFE_TBL_TxnContext_TABLE_SERVICES;
            AccessAllowed |= ~AccessAllowed; /* Set all bits */
        }

        /* Need to lock before actually looking at the descriptor */
        CFE_TBL_TxnLockRegistry(Txn);

        Txn->AccDescPtr = CFE_TBL_LocateAccDescByHandle(TblHandle);
        if (!CFE_TBL_AccDescIsMatch(Txn->AccDescPtr, TblHandle))
        {
            /* Access descriptor is not good */
            Status = CFE_TBL_ERR_INVALID_HANDLE;
        }
        else
        {
            /* Access descriptor is good - check if caller is the descriptor owner */
            AccDescPtr = Txn->AccDescPtr;
            if (CFE_RESOURCEID_TEST_EQUAL(Txn->AppId, AccDescPtr->AppId))
            {
                /* The calling app owns this access descriptor */
                Txn->CallContext |= CFE_TBL_TxnContext_ACCESSOR_APP;
            }

            /* Now check the underlying registry entry */
            Txn->RegId     = AccDescPtr->RegIndex;
            Txn->RegRecPtr = CFE_TBL_LocateRegRecByID(AccDescPtr->RegIndex);
            if (!CFE_TBL_RegRecIsMatch(Txn->RegRecPtr, AccDescPtr->RegIndex))
            {
                /* This means the access descriptor is stale */
                Status = CFE_TBL_ERR_UNREGISTERED;
            }
            else
            {
                /* The registry record is good, check if the caller is the owner */
                RegRecPtr = Txn->RegRecPtr;
                if (CFE_RESOURCEID_TEST_EQUAL(Txn->AppId, RegRecPtr->OwnerAppId))
                {
                    /* The calling app owns this registry entry */
                    Txn->CallContext |= CFE_TBL_TxnContext_OWNER_APP;
                }
            }
        }

        /* If the descriptors all checked out, now check the calling context is whats required */
        if (Status == CFE_SUCCESS)
        {
            AccessAllowed |= Txn->CallContext;

            if ((AccessAllowed & AllowedContext) != AllowedContext)
            {
                Status              = CFE_TBL_ERR_NO_ACCESS;
                Txn->PendingEventId = CFE_TBL_HANDLE_ACCESS_ERR_EID;
            }
        }
    }

    if (Status != CFE_SUCCESS)
    {
        /* If returning with an error, should also unlock the registry */
        CFE_TBL_TxnFinish(Txn);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TxnFinish(CFE_TBL_TxnState_t *Txn)
{
    if (Txn->RegLockCount != 0)
    {
        /* If returning with an error, must also unlock the registry */
        CFE_TBL_TxnUnlockRegistry(Txn);
        Txn->RegLockCount = 0;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function, not invoked outside this unit
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
static void CFE_TBL_FindAccessDescHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg)
{
    CFE_TBL_TxnState_t *Txn = Arg;

    if (AccDescPtr->UsedFlag && CFE_TBL_REGID_EQ(AccDescPtr->RegIndex, CFE_TBL_TxnRegId(Txn)) &&
        CFE_RESOURCEID_TEST_EQUAL(AccDescPtr->AppId, CFE_TBL_TxnAppId(Txn)))
    {
        Txn->Handle     = CFE_TBL_AccDescGetHandle(AccDescPtr);
        Txn->AccDescPtr = AccDescPtr;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_FindAccessDescriptorForSelf(CFE_TBL_TxnState_t *Txn)
{
    CFE_Status_t           Status;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    /* Find the existing access descriptor for the table       */
    RegRecPtr = CFE_TBL_TxnRegRec(Txn);
    CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_FindAccessDescHelper, Txn);

    if (Txn->AccDescPtr == NULL)
    {
        Status = CFE_TBL_ERR_UNREGISTERED;
    }
    else
    {
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
CFE_Status_t CFE_TBL_TxnGetTableStatus(CFE_TBL_TxnState_t *Txn)
{
    int32                  Status;
    CFE_TBL_RegistryRec_t *RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    /* Perform validations prior to performing any updates */
    if (CFE_TBL_RegRecIsLoadPending(RegRecPtr))
    {
        Status = CFE_TBL_INFO_UPDATE_PENDING;
    }
    else if (CFE_TBL_VALRESULTID_IS_VALID(RegRecPtr->ValidateActiveId) ||
             CFE_TBL_VALRESULTID_IS_VALID(RegRecPtr->ValidateInactiveId))
    {
        Status = CFE_TBL_INFO_VALIDATION_PENDING;
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
    if (!CFE_TBL_HandleLinkIsAttached(&RegRecPtr->AccessList))
    {
        /* Only free memory that we have allocated.  If the image is User Defined, then don't bother */
        if (!CFE_TBL_RegRecGetConfig(RegRecPtr)->UserDefAddr)
        {
            /* If there was a working buffer, release it.  This does nothing if there was no working/load buffer. */
            CFE_TBL_DiscardWorkingBuffer(RegRecPtr);

            /* Free memory directly allocated to buffers for this table (non-shared). */
            CFE_TBL_DeallocateAllBuffers(RegRecPtr);
        }
    }

    /* Return the Access Descriptor to the pool */
    CFE_TBL_AccDescSetFree(AccDescPtr);

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
    if (!CFE_TBL_RegRecIsUsed(RegRecPtr))
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

        *TblPtr = ActiveBuffPtr->BufferPtr;

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
CFE_Status_t CFE_TBL_TxnGetNextNotification(CFE_TBL_TxnState_t *Txn)
{
    CFE_Status_t                Status     = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr  = CFE_TBL_TxnRegRec(Txn);

    if (!CFE_TBL_RegRecIsTableLoaded(RegRecPtr))
    {
        /* If the table has never been loaded, return an error code for the address */
        Status = CFE_TBL_ERR_NEVER_LOADED;
    }
    else if (AccDescPtr->Updated)
    {
        /* If the table has been updated recently, return the update status */
        Status = CFE_TBL_INFO_UPDATED;
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
    int16                  i;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    Status = CFE_TBL_ERR_REGISTRY_FULL;
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; ++i)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[i];

        /* A Table Registry is only "Free" when there isn't an owner AND */
        /* all other applications are not sharing or locking the table   */
        if (!CFE_TBL_RegRecIsUsed(RegRecPtr) && !CFE_TBL_HandleLinkIsAttached(&RegRecPtr->AccessList))
        {
            Txn->RegId     = CFE_TBL_REGID_C(i);
            Txn->RegRecPtr = RegRecPtr;

            Status = CFE_SUCCESS;
            break;
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
CFE_Status_t CFE_TBL_TxnAllocateHandle(CFE_TBL_TxnState_t *Txn)
{
    CFE_Status_t                Status;
    int16                       i;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;

    Status = CFE_TBL_ERR_HANDLES_FULL;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES; ++i)
    {
        AccDescPtr = &CFE_TBL_Global.Handles[i];

        if (!CFE_TBL_AccDescIsUsed(AccDescPtr))
        {
            Txn->Handle     = i;
            Txn->AccDescPtr = AccDescPtr;

            Status = CFE_SUCCESS;
            break;
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
                /*
                 * The intent is to fill in the correct handle, but interestingly this
                 * does not detect/propagate the error if it fails
                 */
                CFE_TBL_FindAccessDescriptorForSelf(Txn);

                /* Warn calling application that this is a duplicate registration */
                Status = CFE_TBL_WARN_DUPLICATE;
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

    AccDescPtr->AppId    = CFE_TBL_TxnAppId(Txn);
    AccDescPtr->LockFlag = false;
    AccDescPtr->Updated  = false;
    AccDescPtr->UsedFlag = true;
    AccDescPtr->RegIndex = CFE_TBL_TxnRegId(Txn);

    CFE_TBL_HandleListInsertLink(RegRecPtr, AccDescPtr);
}
