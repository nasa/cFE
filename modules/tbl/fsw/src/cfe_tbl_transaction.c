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
    CFE_TBL_LockRegistry();
    Txn->RegIsLocked = true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TxnUnlockRegistry(CFE_TBL_TxnState_t *Txn)
{
    CFE_TBL_UnlockRegistry();
    Txn->RegIsLocked = false;
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
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    AccessAllowed = 0;
    Status        = CFE_TBL_TxnInit(Txn, AllowedContext != CFE_TBL_TxnContext_UNDEFINED);

    if (Status == CFE_SUCCESS)
    {
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

        Txn->AccessDescPtr = CFE_TBL_LocateAccessDescriptorByHandle(TblHandle);
        if (!CFE_TBL_AccessDescriptorIsMatch(Txn->AccessDescPtr, TblHandle))
        {
            /* Access descriptor is not good */
            Status = CFE_TBL_ERR_INVALID_HANDLE;
        }
        else
        {
            /* Access descriptor is good - check if caller is the descriptor owner */
            AccessDescPtr = Txn->AccessDescPtr;
            if (CFE_RESOURCEID_TEST_EQUAL(Txn->AppId, AccessDescPtr->AppId))
            {
                /* The calling app owns this access descriptor */
                Txn->CallContext |= CFE_TBL_TxnContext_ACCESSOR_APP;
            }

            /* Now check the underlying registry entry */
            Txn->RegRecPtr = CFE_TBL_LocateRegistryRecordByID(AccessDescPtr->RegIndex);
            if (!CFE_TBL_RegistryRecordIsMatch(Txn->RegRecPtr, AccessDescPtr->RegIndex))
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
    if (Txn->RegIsLocked)
    {
        /* If returning with an error, must also unlock the registry */
        CFE_TBL_TxnUnlockRegistry(Txn);
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

    if (AccDescPtr->UsedFlag && AccDescPtr->RegIndex == CFE_TBL_TxnRegId(Txn) &&
        CFE_RESOURCEID_TEST_EQUAL(AccDescPtr->AppId, CFE_TBL_TxnAppId(Txn)))
    {
        Txn->Handle        = CFE_TBL_AccessDescriptorGetHandle(AccDescPtr);
        Txn->AccessDescPtr = AccDescPtr;
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

    if (Txn->AccessDescPtr == NULL)
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
    if (RegRecPtr->LoadPending)
    {
        Status = CFE_TBL_INFO_UPDATE_PENDING;
    }
    else if (CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->ValidateActiveId) ||
             CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->ValidateInactiveId))
    {
        Status = CFE_TBL_INFO_VALIDATION_PENDING;
    }
    else if (CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->DumpControlId))
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
    int32                       Status        = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr     = CFE_TBL_TxnRegRec(Txn);

    /*
     * NOTE: In all cases where this is invoked, the registry should
     * already be locked under the transaction object
     */
    CFE_TBL_HandleListRemoveLink(RegRecPtr, AccessDescPtr);

    /* Return the Access Descriptor to the pool */
    AccessDescPtr->UsedFlag = false;

    /* If this was the last Access Descriptor for this table, we can free the memory buffers as well */
    if (!CFE_TBL_HandleLinkIsAttached(&RegRecPtr->AccessList))
    {
        /* Only free memory that we have allocated.  If the image is User Defined, then don't bother */
        if (RegRecPtr->UserDefAddr == false)
        {
            /* Free memory allocated to buffers */
            Status = CFE_ES_PutPoolBuf(CFE_TBL_Global.Buf.PoolHdl, RegRecPtr->Buffers[0].BufferPtr);
            RegRecPtr->Buffers[0].BufferPtr = NULL;

            if (Status < 0)
            {
                CFE_ES_WriteToSysLog("%s: PutPoolBuf[0] Fail Stat=0x%08X, Hndl=0x%08lX, Buf=0x%08lX\n", __func__,
                                     (unsigned int)Status, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.Buf.PoolHdl),
                                     (unsigned long)RegRecPtr->Buffers[0].BufferPtr);
            }

            /* If a double buffered table, then free the second buffer as well */
            if (RegRecPtr->DoubleBuffered)
            {
                Status = CFE_ES_PutPoolBuf(CFE_TBL_Global.Buf.PoolHdl, RegRecPtr->Buffers[1].BufferPtr);
                RegRecPtr->Buffers[1].BufferPtr = NULL;

                if (Status < 0)
                {
                    CFE_ES_WriteToSysLog("%s: PutPoolBuf[1] Fail Stat=0x%08X, Hndl=0x%08lX, Buf=0x%08lX\n", __func__,
                                         (unsigned int)Status, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.Buf.PoolHdl),
                                         (unsigned long)RegRecPtr->Buffers[1].BufferPtr);
                }
            }
            else
            {
                /* If a shared buffer has been allocated to the table, then release it as well */
                if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
                {
                    /* Free the working buffer */
                    CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Taken = false;
                    RegRecPtr->LoadInProgress                                 = CFE_TBL_NO_LOAD_IN_PROGRESS;
                }
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
CFE_Status_t CFE_TBL_TxnGetTableAddress(CFE_TBL_TxnState_t *Txn, void **TblPtr)
{
    int32                       Status;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr     = CFE_TBL_TxnRegRec(Txn);
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_Handle_t            TblHandle;

    /* If table is unowned, then owner must have unregistered it when we weren't looking */
    if (CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, CFE_TBL_NOT_OWNED))
    {
        Status = CFE_TBL_ERR_UNREGISTERED;

        ThisAppId = CFE_TBL_TxnAppId(Txn);
        TblHandle = CFE_TBL_TxnHandle(Txn);

        CFE_ES_WriteToSysLog("%s: App(%lu) attempt to access unowned Tbl Handle=%d\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(ThisAppId), (int)TblHandle);
    }
    else /* Table Registry Entry is valid */
    {
        /* Lock the table and return the current pointer */
        AccessDescPtr->LockFlag = true;

        /* Save the buffer we are using in the access descriptor */
        /* This is used to ensure that if the buffer becomes inactive while */
        /* we are using it, no one will modify it until we are done */
        AccessDescPtr->BufferIndex = RegRecPtr->ActiveBufferIndex;

        *TblPtr = RegRecPtr->Buffers[AccessDescPtr->BufferIndex].BufferPtr;

        /* Return any pending warning or info status indicators */
        Status = CFE_TBL_TxnGetNextNotification(Txn);

        /* Clear Table Updated Notify Bit so that caller only gets it once */
        AccessDescPtr->Updated = false;
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
    CFE_Status_t                Status        = CFE_SUCCESS;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr     = CFE_TBL_TxnRegRec(Txn);

    if (!RegRecPtr->TableLoadedOnce)
    {
        /* If the table has never been loaded, return an error code for the address */
        Status = CFE_TBL_ERR_NEVER_LOADED;
    }
    else if (AccessDescPtr->Updated)
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
    CFE_Status_t Status = CFE_TBL_ERR_INVALID_NAME;
    int16        i      = 0;

    while (i < CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        /* Check to see if the record is currently being used */
        if (!CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.Registry[i].OwnerAppId, CFE_TBL_NOT_OWNED))
        {
            /* Perform a case sensitive name comparison */
            if (strcmp(TblName, CFE_TBL_Global.Registry[i].Name) == 0)
            {
                /* If the names match, then return the index */
                Txn->RegId     = i;
                Txn->RegRecPtr = &CFE_TBL_Global.Registry[i];

                Status = CFE_SUCCESS;
                break;
            }
        }

        /* Point to next record in the Table Registry */
        i++;
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
    CFE_Status_t Status = CFE_TBL_ERR_REGISTRY_FULL;
    int16        i      = 0;

    while (i < CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        /* A Table Registry is only "Free" when there isn't an owner AND */
        /* all other applications are not sharing or locking the table   */
        if (CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.Registry[i].OwnerAppId, CFE_TBL_NOT_OWNED) &&
            !CFE_TBL_HandleLinkIsAttached(&CFE_TBL_Global.Registry[i].AccessList))
        {
            Txn->RegId     = i;
            Txn->RegRecPtr = &CFE_TBL_Global.Registry[i];

            Status = CFE_SUCCESS;
            break;
        }

        i++;
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
    CFE_Status_t Status = CFE_TBL_ERR_HANDLES_FULL;
    int16        i      = 0;

    while (i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES)
    {
        if (CFE_TBL_Global.Handles[i].UsedFlag == false)
        {
            Txn->Handle        = i;
            Txn->AccessDescPtr = &CFE_TBL_Global.Handles[i];

            Status = CFE_SUCCESS;
            break;
        }
        i++;
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
            if (Size != RegRecPtr->Size)
            {
                /* If the new size is different, the old table must be deleted but this */
                /* function can't do that because it is probably shared and is probably */
                /* still being accessed. Someone else will need to clean up this mess.  */
                Status = CFE_TBL_ERR_DUPLICATE_DIFF_SIZE;

                CFE_ES_WriteToSysLog("%s: Attempt to register existing table ('%s') with different size(%d!=%d)\n",
                                     __func__, TblName, (int)Size, (int)RegRecPtr->Size);
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
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = CFE_TBL_TxnAccDesc(Txn);
    CFE_TBL_RegistryRec_t *     RegRecPtr     = CFE_TBL_TxnRegRec(Txn);

    AccessDescPtr->AppId    = CFE_TBL_TxnAppId(Txn);
    AccessDescPtr->LockFlag = false;
    AccessDescPtr->Updated  = false;
    AccessDescPtr->UsedFlag = true;
    AccessDescPtr->RegIndex = CFE_TBL_TxnRegId(Txn);

    if ((RegRecPtr->DumpOnly) && (!RegRecPtr->UserDefAddr))
    {
        /* Dump Only Tables are assumed to be loaded at all times unless the address is specified */
        /* by the application. In that case, it isn't loaded until the address is specified       */
        RegRecPtr->TableLoadedOnce = true;
    }

    CFE_TBL_HandleListInsertLink(RegRecPtr, AccessDescPtr);
}
