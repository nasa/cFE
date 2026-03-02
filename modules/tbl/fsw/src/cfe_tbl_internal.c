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
** File: cfe_tbl_internal.c
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
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_EarlyInit(void)
{
    uint16 i;
    uint32 j;
    int32  OsStatus;
    int32  Status;

    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_LoadBuff_t *        LoadBuffPtr;
    CFE_TBL_CritRegRec_t *      CritRegRecPtr;

    /* Clear task global */
    memset(&CFE_TBL_Global, 0, sizeof(CFE_TBL_Global));

    /* Initialize the Table Registry */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[i];

        CFE_TBL_InitRegistryRecord(RegRecPtr);
    }

    /* Initialize the Table Access Descriptors nonzero values */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES; i++)
    {
        AccessDescPtr = &CFE_TBL_Global.Handles[i];

        CFE_TBL_InitAccessDescriptor(AccessDescPtr);
    }

    CFE_TBL_Global.HkTlmTblRegId  = CFE_TBL_REGID_UNDEFINED;
    CFE_TBL_Global.LastTblUpdated = CFE_TBL_REGID_UNDEFINED;

    /*
    ** Create table registry access mutex
    */
    OsStatus = OS_MutSemCreate(&CFE_TBL_Global.RegistryMutex, CFE_TBL_MUT_REG_NAME, CFE_TBL_MUT_REG_VALUE);
    if (OsStatus != OS_SUCCESS)
    {
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /*
    ** Create working buffer access mutex
    */
    OsStatus = OS_MutSemCreate(&CFE_TBL_Global.WorkBufMutex, CFE_TBL_MUT_WORK_NAME, CFE_TBL_MUT_WORK_VALUE);
    if (OsStatus != OS_SUCCESS)
    {
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /* Initialize memory partition and allocate shared table buffers. */
    Status = CFE_ES_PoolCreate(&CFE_TBL_Global.Buf.PoolHdl, CFE_TBL_Global.Buf.Partition.Data,
                               sizeof(CFE_TBL_Global.Buf.Partition));

    if (Status < 0)
    {
        return Status;
    }
    else
    {
        /* Initialize each of the shared load buffers */
        for (j = 0; j < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; j++)
        {
            LoadBuffPtr = &CFE_TBL_Global.LoadBuffs[j];

            /* Allocate memory for shared load buffers */
            Status = CFE_TBL_AllocateTableLoadBuffer(LoadBuffPtr, CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE);

            if (Status != CFE_SUCCESS)
            {
                return Status;
            }
        };
    }

    /* Try to obtain a previous image of the Critical Table Registry from the Critical Data Store */
    Status = CFE_ES_RegisterCDSEx(&CFE_TBL_Global.CritRegHandle,
                                  (sizeof(CFE_TBL_CritRegRec_t) * CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES),
                                  "CFE_TBL.CritReg", true);

    /* Assume for the moment that nothing is already in the CDS and zero out the Critical Table Registry */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CritRegRecPtr = &CFE_TBL_Global.CritReg[i];

        CritRegRecPtr->CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    }

    if (Status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* Try to recover the Critical Table Registry from the CDS */
        Status = CFE_ES_RestoreFromCDS(CFE_TBL_Global.CritReg, CFE_TBL_Global.CritRegHandle);

        if (Status != CFE_SUCCESS)
        {
            /* Note if we were unable to recover error free Critical Table Registry from the CDS */
            CFE_ES_WriteToSysLog("%s: Failed to recover Critical Table Registry (Err=0x%08X)\n", __func__,
                                 (unsigned int)Status);
        }

        /* Whether we recovered the Critical Table Registry or not, we are successful with initialization */
        Status = CFE_SUCCESS;
    }
    else if (Status != CFE_SUCCESS)
    {
        /* Not being able to support Critical Tables is not the end of the world */
        /* Note the problem and move on */
        CFE_ES_WriteToSysLog("%s: Failed to create Critical Table Registry (Err=0x%08X)\n", __func__,
                             (unsigned int)Status);

        /* Failure to support critical tables is not a good enough reason to exit the cFE on start up */
        Status = CFE_SUCCESS;
    }
    else
    {
        /* Save the initial version of the Critical Table Registry in the CDS */
        Status = CFE_ES_CopyToCDS(CFE_TBL_Global.CritRegHandle, CFE_TBL_Global.CritReg);

        if (Status != CFE_SUCCESS)
        {
            /* Not being able to support Critical Tables is not the end of the world */
            /* Note the problem and move on */
            CFE_ES_WriteToSysLog("%s: Failed to save Critical Table Registry (Err=0x%08X)\n", __func__,
                                 (unsigned int)Status);

            /* Failure to support critical tables is not a good enough reason to exit the cFE on start up */
            Status = CFE_SUCCESS;
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
CFE_Status_t CFE_TBL_ValidateTableName(CFE_TBL_TableConfig_t *ReqCfg, const char *TblName, const char *AppName)
{
    CFE_Status_t Status;
    int          Result;

    /* Make sure the specified table name is not too long or too short */
    if (TblName[0] == 0 || memchr(TblName, 0, CFE_MISSION_TBL_MAX_NAME_LENGTH) == NULL)
    {
        Status = CFE_TBL_ERR_INVALID_NAME;
    }
    else
    {
        /* Complete formation of application specific table name */
        Result = snprintf(ReqCfg->Name, sizeof(ReqCfg->Name), "%s.%s", AppName, TblName);
        if (Result > sizeof(ReqCfg->Name))
        {
            /* This means the name was truncated in the snprintf() call, the buffer is too small */
            Status = CFE_TBL_ERR_INVALID_NAME;
        }
        else
        {
            Status = CFE_SUCCESS;
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
void CFE_TBL_DiscardWorkingBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* If the buffer is NOT one of the buffers "owned" by this table, then it must be
     * from a shared buffer pool so it needs to be released */
    CFE_TBL_LoadBuff_t * LoadBuffPtr;
    CFE_TBL_LoadBuffId_t LoadInProgressId;

    if (!CFE_TBL_RegRecGetConfig(RegRecPtr)->DoubleBuffered && CFE_TBL_RegRecIsLoadInProgress(RegRecPtr))
    {
        LoadInProgressId = CFE_TBL_RegRecGetLoadInProgress(RegRecPtr);
        LoadBuffPtr      = CFE_TBL_LocateLoadBufferByID(LoadInProgressId);
    }
    else
    {
        LoadBuffPtr = NULL;
    }

    CFE_TBL_RegRecClearLoadInProgress(RegRecPtr);

    if (LoadBuffPtr != NULL)
    {
        CFE_TBL_LoadBuffSetFree(LoadBuffPtr);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_DeallocateBuffer(CFE_TBL_LoadBuff_t *BuffPtr)
{
    CFE_Status_t Status;
    void *       MemPtr;

    MemPtr = CFE_TBL_LoadBuffGetWritePointer(BuffPtr);

    Status = CFE_ES_PutPoolBuf(CFE_TBL_Global.Buf.PoolHdl, MemPtr);

    if (Status < 0)
    {
        CFE_ES_WriteToSysLog("%s: PutPoolBuf() Fail Stat=0x%08X, Hndl=0x%08lX, Buf=0x%08lX\n", __func__,
                             (unsigned int)Status, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.Buf.PoolHdl),
                             (unsigned long)MemPtr);
    }

    CFE_TBL_LoadBuffSetAllocatedBlock(BuffPtr, NULL, 0);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_DeallocateAllBuffers(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuff_t *BuffPtr;
    int                 i;

    for (i = 0; i < 2; ++i)
    {
        BuffPtr = &RegRecPtr->Buffers[i];

        if (CFE_TBL_LoadBuffIsAllocated(BuffPtr))
        {
            /* Free memory allocated to buffers */
            CFE_TBL_DeallocateBuffer(BuffPtr);
        }

        CFE_TBL_LoadBuffSetFree(BuffPtr);
    }

    /* If a shared buffer has been allocated to the table, then release it as well */
    CFE_TBL_DiscardWorkingBuffer(RegRecPtr);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegId_t CFE_TBL_FindTableInRegistry(const char *TblName)
{
    CFE_TBL_TxnState_t Txn;

    /* Note: there is no way for transaction setup to fail when passing false for context check */
    CFE_TBL_TxnInit(&Txn, false);
    CFE_TBL_TxnFindRegByName(&Txn, TblName);
    CFE_TBL_TxnFinish(&Txn);

    /* The transaction mechanism will set this to CFE_TBL_NOT_FOUND if anything failed */
    return CFE_TBL_TxnRegId(&Txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_LockRegistry(void)
{
    int32 OsStatus;
    int32 Status;

    OsStatus = OS_MutSemTake(CFE_TBL_Global.RegistryMutex);

    if (OsStatus == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_UnlockRegistry(void)
{
    int32 OsStatus;
    int32 Status;

    OsStatus = OS_MutSemGive(CFE_TBL_Global.RegistryMutex);

    if (OsStatus == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_AcquireGlobalLoadBuff(CFE_TBL_RegId_t PendingOwnerId)
{
    int32               OsStatus;
    CFE_ResourceId_t    PendingId;
    CFE_TBL_LoadBuff_t *LoadBuffPtr;

    LoadBuffPtr = NULL;

    /* Take Mutex to make sure we are not trying to grab a working buffer that some */
    /* other application is also trying to grab. */
    OsStatus = OS_MutSemTake(CFE_TBL_Global.WorkBufMutex);

    /* Make note of any errors but continue and hope for the best */
    if (OsStatus != OS_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Internal error taking WorkBuf Mutex (Status=%ld)\n", __func__, (long)OsStatus);
    }

    /* Determine if there are any common buffers available */
    PendingId   = CFE_TBL_FindNextSharedBufferId();
    LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(CFE_TBL_LOADBUFFID_C(PendingId));
    if (LoadBuffPtr != NULL)
    {
        /* Claim it as ours */
        CFE_TBL_LoadBuffSetUsed(LoadBuffPtr, PendingId, PendingOwnerId);

        CFE_TBL_Global.LastLoadBuffId = PendingId;
    }

    /* Allow others to obtain a shared working buffer */
    OS_MutSemGive(CFE_TBL_Global.WorkBufMutex);

    return LoadBuffPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_GetWorkingBuffer(CFE_TBL_LoadBuff_t **WorkingBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuff_t *LoadBuffPtr;

    /* If a load is already in progress, return the previously allocated working buffer */
    LoadBuffPtr = CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);
    if (LoadBuffPtr == NULL)
    {
        /* load was not in progress */
        if (!CFE_TBL_RegRecIsTableLoaded(RegRecPtr))
        {
            /* If the table is uninitialized and the function is called by an application (rather than       */
            /* by the Table Services application), then use the current active buffer as the working buffer. */
            /* This allows many tasks with many tables to perform the initialization without conflict        */
            /* over the accessibility of the shared working buffers.                                         */
            LoadBuffPtr = CFE_TBL_GetInactiveBufferExclusive(RegRecPtr);
        }
        else
        {
            LoadBuffPtr = CFE_TBL_PrepareNewLoadBuff(RegRecPtr);
        }

        if (LoadBuffPtr != NULL)
        {
            /*
             * store the resource ID (generic form) in NextBufferId - this allows
             * it to refer to either a shared buffer or a local table buffer
             */
            CFE_TBL_RegRecSetLoadInProgress(RegRecPtr, CFE_TBL_LoadBufferGetID(LoadBuffPtr));
        }
    }

    if (WorkingBufferPtr != NULL)
    {
        *WorkingBufferPtr = LoadBuffPtr;
    }

    if (LoadBuffPtr == NULL)
    {
        return CFE_TBL_ERR_NO_BUFFER_AVAIL;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_UpdateInternal(CFE_TBL_HandleId_t TblHandle, CFE_TBL_RegistryRec_t *RegRecPtr,
                             CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    int32               Status = CFE_SUCCESS;
    CFE_TBL_LoadBuff_t *NextBuffPtr;
    CFE_TBL_LoadBuff_t *LoadBuffPtr;

    NextBuffPtr = NULL;
    LoadBuffPtr = CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);
    if (LoadBuffPtr == NULL)
    {
        /* Question: Should calling CFE_TBL_Update on a table with no load pending */
        /* be considered an error?  Currently assuming it is not an error.         */
        Status = CFE_TBL_INFO_NO_UPDATE_PENDING;
    }
    else
    {
        /*
         * If the load buffer is a DEDICATED buffer for this table registry (not a shared buf),
         * then it can be simply activated by setting it active buffer ref to it.  This will
         * always be true for double buffered tables, because they just flip-flop between two
         * dedicated table buffers; they never use shared buffers for loading.  For single
         * buffered tables this is also true during the first initial load, where a temp buffer
         * is not necessary
         */
        if (CFE_TBL_LoadBuffIsPrivate(CFE_TBL_LoadBufferGetID(LoadBuffPtr), CFE_TBL_RegRecGetID(RegRecPtr)))
        {
            NextBuffPtr = LoadBuffPtr;
        }
        else
        {
            /*
             * This gets a new identifier and also makes sure nobody is using the buffer
             * via a shared table handle at this time.  The only way this can fail is
             * if there is an active shared table reference to this buffer.
             */
            NextBuffPtr = CFE_TBL_GetInactiveBufferExclusive(RegRecPtr);
            if (NextBuffPtr == NULL)
            {
                Status = CFE_TBL_INFO_TABLE_LOCKED;
                CFE_ES_WriteToSysLog("%s: Unable to update locked table Handle=%lu\n", __func__,
                                     CFE_TBL_HandleID_AsInt(CFE_TBL_AccDescGetHandle(AccessDescPtr)));
            }
        }
    }

    if (NextBuffPtr != NULL)
    {
        /*
         * To update a single buffered table requires a memcpy from working buffer,
         * but in a double buffered table they will point to the same LoadBuff, so
         * all the copies can be skipped (this is the advantage of double-buffering)
         */
        if (NextBuffPtr != LoadBuffPtr)
        {
            CFE_TBL_LoadBuffCopyData(NextBuffPtr, CFE_TBL_LoadBuffGetReadPointer(LoadBuffPtr),
                                     CFE_TBL_LoadBuffGetContentSize(LoadBuffPtr));

            /* Save source description with active buffer (Note - structs are same type, length already checked) */
            strncpy(NextBuffPtr->DataSource, LoadBuffPtr->DataSource, sizeof(NextBuffPtr->DataSource));

            /* Save the file creation time from the loaded file into the Table Registry */
            NextBuffPtr->FileTime = LoadBuffPtr->FileTime;

            /* Save the previously computed CRC into the new buffer */
            NextBuffPtr->Crc = LoadBuffPtr->Crc;
        }

        CFE_TBL_SetActiveBuffer(RegRecPtr, NextBuffPtr);

        CFE_TBL_RegRecResetLoadInfo(RegRecPtr, NextBuffPtr->DataSource, CFE_TIME_GetTime());

        CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);

        /* If the table is a critical table, update the appropriate CDS with the new data */
        if (CFE_TBL_RegRecGetConfig(RegRecPtr)->Critical)
        {
            CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
        }

        CFE_TBL_DiscardWorkingBuffer(RegRecPtr);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function, not invoked outside this unit
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
static void CFE_TBL_SetUpdatedHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg)
{
    AccDescPtr->Updated = true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_NotifyTblUsersOfUpdate(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_SetUpdatedHelper, NULL);
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_CleanUpApp(CFE_ES_AppId_t AppId)
{
    uint32                      i;
    CFE_TBL_TxnState_t          Txn;
    CFE_TBL_DumpControl_t *     DumpCtrlPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_Status_t                Status;

    CFE_TBL_TxnInit(&Txn, false);

    /* Scan Dump Requests to determine if any of the tables that */
    /* were to be dumped will be deleted */
    CFE_TBL_TxnLockRegistry(&Txn);
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[i];

        /* Check to see if the table to be dumped is owned by the App to be deleted */
        if (CFE_TBL_DumpCtrlBlockIsUsed(DumpCtrlPtr))
        {
            RegRecPtr = CFE_TBL_LoadBuffGetRegRecFromId(DumpCtrlPtr->SourceBuffId);
            if (RegRecPtr != NULL && CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, AppId))
            {
                /* If so, then remove the dump request */
                CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);
            }
        }
    }
    CFE_TBL_TxnFinish(&Txn);

    /* Scan Access Descriptors to determine if the Application had access to any tables */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES; i++)
    {
        AccessDescPtr = &CFE_TBL_Global.Handles[i];

        Status =
            CFE_TBL_TxnStartFromHandle(&Txn, CFE_TBL_AccDescGetHandle(AccessDescPtr), CFE_TBL_TxnContext_UNDEFINED);
        if (Status == CFE_SUCCESS)
        {
            /* This transaction is being performed on behalf of the appid that is now gone away */
            Txn.AppId = AppId;

            /* Check to see if the Handle belongs to the Application being deleted */
            if (CFE_RESOURCEID_TEST_EQUAL(AccessDescPtr->AppId, Txn.AppId))
            {
                /* Release the access descriptor and clean up refs */
                CFE_TBL_TxnReleaseAccDesc(&Txn);
            }
            CFE_TBL_TxnFinish(&Txn);
        }
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_FindCriticalTblInfo(CFE_TBL_CritRegRec_t **CritRegRecPtr, CFE_ES_CDSHandle_t CDSHandleToFind)
{
    uint32                i;
    CFE_TBL_CritRegRec_t *LocalPtr;

    /* Assume the record is never found */
    *CritRegRecPtr = NULL;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        LocalPtr = &CFE_TBL_Global.CritReg[i];

        if (CFE_RESOURCEID_TEST_EQUAL(LocalPtr->CDSHandle, CDSHandleToFind))
        {
            *CritRegRecPtr = LocalPtr;
            break;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_UpdateCriticalTblCDS(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_CritRegRec_t *CritRegRecPtr = NULL;
    CFE_TBL_LoadBuff_t *  ActiveBufPtr;

    int32 Status;

    /* Copy an image of the updated table to the CDS for safekeeping */
    ActiveBufPtr = CFE_TBL_GetActiveBuffer(RegRecPtr);
    if (ActiveBufPtr == NULL)
    {
        /* cannot write a table to CDS if it was never loaded in the first place */
        Status = CFE_TBL_ERR_NEVER_LOADED;
    }
    else
    {
        Status = CFE_ES_CopyToCDS(RegRecPtr->CDSHandle, CFE_TBL_LoadBuffGetReadPointer(ActiveBufPtr));
    }

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Unable to update Critical Table '%s' in CDS (Err=0x%08X)\n", __func__,
                             CFE_TBL_RegRecGetName(RegRecPtr), (unsigned int)Status);
    }
    else
    {
        /* Locate entry in Critical Table Registry */
        CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, RegRecPtr->CDSHandle);
        if (CritRegRecPtr != NULL)
        {
            /* Save information related to the source of the data stored in the table in Critical Table Registry */
            CritRegRecPtr->FileTime = ActiveBufPtr->FileTime;
            strncpy(CritRegRecPtr->LastFileLoaded, CFE_TBL_RegRecGetLastFileLoaded(RegRecPtr),
                    sizeof(CritRegRecPtr->LastFileLoaded) - 1);
            CritRegRecPtr->LastFileLoaded[sizeof(CritRegRecPtr->LastFileLoaded) - 1] = '\0';

            CritRegRecPtr->TimeOfLastUpdate = CFE_TBL_RegRecGetLastUpdateTime(RegRecPtr);
            CritRegRecPtr->TableLoadedOnce  = CFE_TBL_RegRecIsTableLoaded(RegRecPtr);

            /* Update copy of Critical Table Registry in the CDS */
            Status = CFE_ES_CopyToCDS(CFE_TBL_Global.CritRegHandle, CFE_TBL_Global.CritReg);

            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Unable to update Critical Table Registry in CDS (Err=0x%08X)\n", __func__,
                                     (unsigned int)Status);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Error finding '%s' in Critical Table Registry\n", __func__,
                                 CFE_TBL_RegRecGetName(RegRecPtr));
        }
    }

    /* Don't bother notifying the caller of the problem since the active table is still legitimate */
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_SendNotificationMsg(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    int32 Status = CFE_SUCCESS;

    /* First, determine if a message should be sent */
    if (RegRecPtr->Notify.Enabled)
    {
        /* Set the message ID */
        CFE_MSG_SetMsgId(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader), RegRecPtr->Notify.MsgId);

        /* Set the command code */
        CFE_MSG_SetFcnCode(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader), RegRecPtr->Notify.FcnCode);

        /* Set the command parameter */
        CFE_TBL_Global.NotifyMsg.Payload.Parameter = RegRecPtr->Notify.Param;

        CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader));
        Status = CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader), true);

        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_TBL_FAIL_NOTIFY_SEND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Manage Notification Pkt Error(Status=0x%08X)", (unsigned int)Status);
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
CFE_Status_t CFE_TBL_ValidateTableSize(CFE_TBL_TableConfig_t *TableCfg, size_t Size)
{
    CFE_Status_t Status;
    size_t       SizeLimit;

    /* Single-buffered tables are allowed to be up to CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE   */
    /* Double-buffered tables are allowed to be up to CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE    */
    if (TableCfg->DoubleBuffered)
    {
        SizeLimit = CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE;
    }
    else
    {
        SizeLimit = CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE;
    }

    /* Check if the specified table size is zero, or above the maximum allowed               */
    if (Size == 0 || Size > SizeLimit)
    {
        Status = CFE_TBL_ERR_INVALID_SIZE;
    }
    else
    {
        Status         = CFE_SUCCESS;
        TableCfg->Size = Size;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateTableOptions(CFE_TBL_TableConfig_t *TableCfg, uint16 TblOptionFlags)
{
    CFE_Status_t Status;

    /* This will be changed if validation fails */
    Status = CFE_SUCCESS;

    TableCfg->DoubleBuffered = ((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_DBL_BUFFER);
    TableCfg->UserDefAddr =
        ((TblOptionFlags & CFE_TBL_OPT_USR_DEF_MSK) == (CFE_TBL_OPT_USR_DEF_ADDR & CFE_TBL_OPT_USR_DEF_MSK));
    TableCfg->DumpOnly = ((TblOptionFlags & CFE_TBL_OPT_LD_DMP_MSK) == CFE_TBL_OPT_DUMP_ONLY);
    TableCfg->Critical = ((TblOptionFlags & CFE_TBL_OPT_CRITICAL_MSK) == CFE_TBL_OPT_CRITICAL);

    /* Now check the specific combination of options that are not valid */
    if (TableCfg->DumpOnly)
    {
        /* Dump Only tables cannot be double-buffered, nor critical (this also applies to user-defined address) */
        if (TableCfg->DoubleBuffered || TableCfg->Critical)
        {
            Status = CFE_TBL_ERR_INVALID_OPTIONS;
        }
    }
    else if (TableCfg->UserDefAddr)
    {
        /* User-defined table addresses are only legal for dump-only tables */
        Status = CFE_TBL_ERR_INVALID_OPTIONS;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_AllocateTableLoadBuffer(CFE_TBL_LoadBuff_t *LoadBuffPtr, size_t Size)
{
    CFE_Status_t Status;
    void *       MemPtr;

    /* Allocate the memory buffer(s) for the table and inactive table, if necessary */
    Status = CFE_ES_GetPoolBuf(&MemPtr, CFE_TBL_Global.Buf.PoolHdl, Size);

    if (Status < 0)
    {
        CFE_ES_WriteToSysLog("%s: Buf Alloc GetPool fail Stat=0x%08X MemPoolHndl=0x%08lX\n", __func__,
                             (unsigned int)Status, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.Buf.PoolHdl));
    }
    else
    {
        Status = CFE_SUCCESS;

        CFE_TBL_LoadBuffSetAllocatedBlock(LoadBuffPtr, MemPtr, Size);
        CFE_TBL_LoadBuffClearData(LoadBuffPtr);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_MarkNameAsModified(char *NameBufPtr, size_t NameBufSize)
{
    char *EndPtr;

    EndPtr = memchr(NameBufPtr, 0, NameBufSize);
    if ((EndPtr - NameBufPtr) > (NameBufSize - 4))
    {
        EndPtr = &NameBufPtr[NameBufSize - 4];
    }

    strcpy(EndPtr, "(*)");
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_SetupTableBuffers(CFE_TBL_RegistryRec_t *RegRecPtr, const CFE_TBL_TableConfig_t *ReqCfg)
{
    CFE_Status_t Status;

    /* If requested a dump-only table, set that flag now.
     * NOTE: other option flags are set when that feature is actually enabled,
     * but this one does not require any special enablement other than the flag */
    RegRecPtr->Config.DumpOnly = ReqCfg->DumpOnly;

    if (ReqCfg->UserDefAddr)
    {
        RegRecPtr->Config.UserDefAddr = true;

        /* nothing more to do when using user-defined address (no local buffer) */
        Status = CFE_SUCCESS;
    }
    else
    {
        /* Allocate the local buffer for storing table content */
        Status = CFE_TBL_AllocateTableLoadBuffer(&RegRecPtr->Buffers[0], ReqCfg->Size);

        /* Secondary only needed on double buffered tables */
        if (Status == CFE_SUCCESS && ReqCfg->DoubleBuffered)
        {
            Status = CFE_TBL_AllocateTableLoadBuffer(&RegRecPtr->Buffers[1], ReqCfg->Size);
            if (Status == CFE_SUCCESS)
            {
                /* double buffering was set up successfully */
                RegRecPtr->Config.DoubleBuffered = true;
            }
        }
    }

    if (Status == CFE_SUCCESS)
    {
        /* Save the size of the table */
        RegRecPtr->Config.Size = ReqCfg->Size;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_RestoreTableDataFromCDS(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_Status_t          Status = CFE_SUCCESS;
    CFE_TBL_CritRegRec_t *CritRegRecPtr;
    CFE_TBL_LoadBuff_t *  WorkingBufferPtr;

    Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr);

    if (Status != CFE_SUCCESS)
    {
        /* Unable to get a working buffer - this error is not really */
        /* possible at this point during table registration.  But we */
        /* do need to handle the error case because if the function */
        /* call did fail, WorkingBufferPtr would be a NULL pointer. */
        CFE_ES_WriteToSysLog("%s: Failed to get work buffer for '%s' (ErrCode=0x%08X)\n", __func__,
                             CFE_TBL_RegRecGetName(RegRecPtr), (unsigned int)Status);
    }
    else
    {
        /* CDS exists for this table - try to restore the data */
        Status = CFE_ES_RestoreFromCDS(CFE_TBL_LoadBuffGetWritePointer(WorkingBufferPtr), RegRecPtr->CDSHandle);

        if (Status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: Failed to recover '%s' from CDS (ErrCode=0x%08X)\n", __func__,
                                 CFE_TBL_RegRecGetName(RegRecPtr), (unsigned int)Status);

            /*
             * Treat a restore from existing CDS error the same as
             * after a power-on reset (CDS was created but is empty)
             */
            Status = CFE_SUCCESS;
        }
        else
        {
            /* Table was fully restored from existing CDS... */
            /* Try to locate the associated information in the Critical Table Registry */
            CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, RegRecPtr->CDSHandle);

            if ((CritRegRecPtr != NULL) && (CritRegRecPtr->TableLoadedOnce))
            {
                strncpy(WorkingBufferPtr->DataSource, CritRegRecPtr->LastFileLoaded,
                        sizeof(WorkingBufferPtr->DataSource) - 1);
                WorkingBufferPtr->DataSource[sizeof(WorkingBufferPtr->DataSource) - 1] = '\0';

                WorkingBufferPtr->FileTime = CritRegRecPtr->FileTime;

                CFE_TBL_SetActiveBuffer(RegRecPtr, WorkingBufferPtr);

                CFE_TBL_RegRecResetLoadInfo(RegRecPtr, CritRegRecPtr->LastFileLoaded, CritRegRecPtr->TimeOfLastUpdate);

                /* Compute the CRC on the specified table buffer */
                CFE_TBL_LoadBuffRecomputeCRC(WorkingBufferPtr);

                /* Make sure everyone who sees the table knows that it has been updated */
                CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);

                /* Make sure the caller realizes the contents have been initialized */
                Status = CFE_TBL_INFO_RECOVERED_TBL;
            }
            else
            {
                /* If an error occurred while trying to get the previous contents registry info, */
                /* Log the error in the System Log and pretend like we created a new CDS */
                CFE_ES_WriteToSysLog("%s: Failed to recover '%s' info from CDS TblReg\n", __func__,
                                     CFE_TBL_RegRecGetName(RegRecPtr));
                Status = CFE_SUCCESS;
            }
        }
    }

    /* Mark the table as critical for future reference */
    RegRecPtr->Config.Critical = true;

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_RegisterWithCriticalTableRegistry(CFE_TBL_CritRegRec_t *CritRegRecPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                                               const char *TblName)
{
    /* Find and initialize a free entry in the Critical Table Registry */
    CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, CFE_ES_CDS_BAD_HANDLE);

    if (CritRegRecPtr != NULL)
    {
        CritRegRecPtr->CDSHandle = RegRecPtr->CDSHandle;
        strncpy(CritRegRecPtr->Name, TblName, sizeof(CritRegRecPtr->Name) - 1);
        CritRegRecPtr->Name[sizeof(CritRegRecPtr->Name) - 1] = '\0';
        CritRegRecPtr->LastFileLoaded[0]                     = '\0';

        CritRegRecPtr->FileTime         = CFE_TIME_ZERO_VALUE;
        CritRegRecPtr->TimeOfLastUpdate = CFE_TIME_ZERO_VALUE;

        CFE_ES_CopyToCDS(CFE_TBL_Global.CritRegHandle, CFE_TBL_Global.CritReg);
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: Failed to find a free Crit Tbl Reg Rec for '%s'\n", __func__,
                             CFE_TBL_RegRecGetName(RegRecPtr));
    }

    /* Mark the table as critical for future reference */
    RegRecPtr->Config.Critical = true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
static inline CFE_TBL_AccessDescriptor_t *CFE_TBL_HandleListGetNext(const CFE_TBL_HandleLink_t *Link)
{
    return CFE_TBL_LocateAccDescByHandle(Link->Next);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
static inline CFE_TBL_AccessDescriptor_t *CFE_TBL_HandleListGetPrev(const CFE_TBL_HandleLink_t *Link)
{
    return CFE_TBL_LocateAccDescByHandle(Link->Prev);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleListGetSafeLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccDescPtr,
                                   CFE_TBL_HandleLink_t **PtrOut, CFE_TBL_HandleId_t *HandleOut)
{
    if (AccDescPtr == NULL)
    {
        /* Instead of returning NULL, return a pointer to the head node linkage */
        *PtrOut    = &RegRecPtr->AccessList;
        *HandleOut = CFE_TBL_HANDLEID_UNDEFINED;
    }
    else
    {
        /* Return a pointer to this linkage */
        *PtrOut    = &AccDescPtr->Link;
        *HandleOut = CFE_TBL_AccDescGetHandle(AccDescPtr);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleListRemoveLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    CFE_TBL_HandleLink_t *LocalLink;
    CFE_TBL_HandleLink_t *LocalPrevPtr;
    CFE_TBL_HandleLink_t *LocalNextPtr;
    CFE_TBL_HandleId_t    LocalHandle;
    CFE_TBL_HandleId_t    PrevHandle;
    CFE_TBL_HandleId_t    NextHandle;

    CFE_TBL_HandleListGetSafeLink(RegRecPtr, AccessDescPtr, &LocalLink, &LocalHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, CFE_TBL_HandleListGetNext(LocalLink), &LocalNextPtr, &NextHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, CFE_TBL_HandleListGetPrev(LocalLink), &LocalPrevPtr, &PrevHandle);

    LocalPrevPtr->Next = NextHandle;
    LocalNextPtr->Prev = PrevHandle;

    /* now that it is removed, reset the link */
    CFE_TBL_HandleLinkInit(LocalLink);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleListInsertLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    CFE_TBL_HandleLink_t *LocalLink;
    CFE_TBL_HandleLink_t *LocalPrevPtr;
    CFE_TBL_HandleLink_t *LocalNextPtr;
    CFE_TBL_HandleId_t    LocalHandle;
    CFE_TBL_HandleId_t    PrevHandle;
    CFE_TBL_HandleId_t    NextHandle;

    /* inserting at the front, so the "previous" will always be the head node (NULL) */
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, AccessDescPtr, &LocalLink, &LocalHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, NULL, &LocalPrevPtr, &PrevHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, CFE_TBL_HandleListGetNext(LocalPrevPtr), &LocalNextPtr, &NextHandle);

    LocalLink->Next = NextHandle;
    LocalLink->Prev = PrevHandle;

    LocalPrevPtr->Next = LocalHandle;
    LocalNextPtr->Prev = LocalHandle;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ForeachAccessDescriptor(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescFunc_t Func, void *Arg)
{
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    const CFE_TBL_HandleLink_t *LinkPtr;

    LinkPtr = &RegRecPtr->AccessList;
    while (true)
    {
        AccDescPtr = CFE_TBL_HandleListGetNext(LinkPtr);

        if (AccDescPtr == NULL)
        {
            break;
        }

        Func(AccDescPtr, Arg);

        LinkPtr = &AccDescPtr->Link;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_CountAccessDescHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg)
{
    uint32 *Count = Arg;

    ++(*Count);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_ValidationResult_t *CFE_TBL_CheckValidationRequest(CFE_TBL_ValidationResultId_t *ValIdPtr)
{
    CFE_TBL_ValidationResult_t * ResultPtr;
    CFE_TBL_ValidationResultId_t ValId;

    ValId = *ValIdPtr;

    /*
     * always clear the flag, regardless of "IsMatch" above.  If it was not a match,
     * that means the ID was stale, and it will never be a match (ie. it was aborted somehow)
     *
     * However, because this also acts as a flag, only write to the global if it was set to a value,
     * do not unconditionally write undefined value here.
     */
    if (CFE_TBL_VALRESULTID_IS_VALID(ValId))
    {
        *ValIdPtr = CFE_TBL_VALRESULTID_UNDEFINED;

        ResultPtr = CFE_TBL_LocateValidationResultByID(ValId);
    }
    else
    {
        ResultPtr = NULL;
    }

    if (!CFE_TBL_ValidationResultIsMatch(ResultPtr, ValId))
    {
        ResultPtr = NULL;
    }

    return ResultPtr;
}
