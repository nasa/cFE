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
CFE_Status_t CFE_TBL_TxnInit(CFE_TBL_TxnState_t *Txn, bool CheckContext)
{
    CFE_Status_t Status;

    memset(Txn, 0, sizeof(*Txn));

    /* NOTE: handle and regid are automatically made safe via memset() above */

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
CFE_Status_t CFE_TBL_TxnStartFromHandle(CFE_TBL_TxnState_t *Txn, CFE_TBL_HandleId_t TblHandle, uint32 AllowedContext)
{
    CFE_Status_t                Status;
    uint32                      AccessAllowed;
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;

    AccessAllowed = 0;

    /* Sanity check on handle -- This avoids locking the registry for lookups that will certainly fail */
    if (!CFE_TBL_HandleID_IsDefined(TblHandle))
    {
        Status = CFE_TBL_ERR_INVALID_HANDLE;
    }
    else
    {
        Status = CFE_TBL_TxnInit(Txn, AllowedContext != CFE_TBL_TxnContext_UNDEFINED);
    }

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
                Status = CFE_TBL_ERR_NO_ACCESS;
                CFE_TBL_TxnAddEvent(Txn, CFE_TBL_HANDLE_ACCESS_ERR_EID, Status, AccessAllowed);
            }
        }

        if (Status != CFE_SUCCESS)
        {
            /* If returning with an error, should also unlock the registry */
            CFE_TBL_TxnFinish(Txn);
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

    /* Note that the only entries in the list will be, by definition, access descriptors that
     * point at this regrec entry.  So checking that AccDescPtr->RegIndex matches the transaction
     * subject RegId would result in an uncovered branch, it cannot be false unless the list gets
     * corrupted somehow. */
    if (CFE_TBL_AccDescIsUsed(AccDescPtr) && CFE_RESOURCEID_TEST_EQUAL(AccDescPtr->AppId, CFE_TBL_TxnAppId(Txn)))
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
void CFE_TBL_TxnAddEvent(CFE_TBL_TxnState_t *Txn, uint16 EventId, int32 EventData1, int32 EventData2)
{
    CFE_TBL_TxnEvent_t *EvPtr;

    if (Txn->NumPendingEvents < CFE_TBL_MAX_EVENTS_PER_TXN)
    {
        EvPtr = &Txn->PendingEvents[Txn->NumPendingEvents];

        EvPtr->EventId    = EventId;
        EvPtr->EventData1 = EventData1;
        EvPtr->EventData2 = EventData2;
    }

    /* This always increments the number of pending events, to make it evident if there was an overflow */
    ++Txn->NumPendingEvents;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_TBL_TxnGetEventCount(const CFE_TBL_TxnState_t *Txn)
{
    return Txn->NumPendingEvents;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_TBL_TxnProcessEvents(const CFE_TBL_TxnState_t *Txn, CFE_TBL_TxnEventProcFunc_t EventProc, void *Arg)
{
    uint32 i;
    uint32 NumPending;
    uint32 NumProc;

    NumPending = CFE_TBL_TxnGetEventCount(Txn);
    NumProc    = 0;

    if (NumPending > CFE_TBL_MAX_EVENTS_PER_TXN)
    {
        /* this means there was an overflow */
        NumPending = CFE_TBL_MAX_EVENTS_PER_TXN;
    }

    /* Events should be processed in the same order that CFE_TBL_TxnAddEvent() was called */
    for (i = 0; i < NumPending; ++i)
    {
        if (EventProc(&Txn->PendingEvents[i], Arg))
        {
            ++NumProc;
        }
    }

    return NumProc;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TxnClearEvents(CFE_TBL_TxnState_t *Txn)
{
    Txn->NumPendingEvents = 0;
}
