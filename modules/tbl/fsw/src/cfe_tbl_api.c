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
** File: cfe_tbl_api.c
**
** Purpose:  cFE Table Services (TBL) library API source file
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

#include <string.h>

/*
** Local Macros
*/

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Register(CFE_TBL_Handle_t *TblHandlePtr, const char *Name, size_t Size, uint16 TblOptionFlags,
                              CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr)
{
    CFE_TBL_TxnState_t     Txn;
    CFE_TBL_RegistryRec_t *RegRecPtr     = NULL;
    CFE_TBL_CritRegRec_t * CritRegRecPtr = NULL;
    CFE_Status_t           Status;
    CFE_ES_AppId_t         ThisAppId;
    char                   AppName[OS_MAX_API_NAME]           = {"UNKNOWN"};
    char                   TblName[CFE_TBL_MAX_FULL_NAME_LEN] = {""};

    if (TblHandlePtr == NULL || Name == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Check to make sure calling application is legit */
    Status = CFE_TBL_TxnInit(&Txn, true);

    ThisAppId = CFE_TBL_TxnAppId(&Txn);

    if (Status == CFE_SUCCESS)
    {
        /* Validate table input parameters (Name, Size and Options) */
        Status = CFE_TBL_ValidateTableName(Name);

        if (Status == CFE_SUCCESS)
        {
            /* Generate the full application-specific table name if name validation was successful */
            CFE_TBL_FormTableName(TblName, Name, ThisAppId);

            Status = CFE_TBL_ValidateTableSize(Name, Size, TblOptionFlags);

            if (Status == CFE_SUCCESS)
            {
                Status = CFE_TBL_ValidateTableOptions(Name, TblOptionFlags);
            }
        }
    }
    else /* Application ID was invalid */
    {
        CFE_ES_WriteToSysLog("%s: Bad AppId(%lu)\n", __func__, CFE_RESOURCEID_TO_ULONG(ThisAppId));
    }

    /* If input parameters appear acceptable, register the table */
    if (Status == CFE_SUCCESS)
    {
        /* Lock Registry for update.  This prevents two applications from        */
        /* trying to register/share tables at the same location at the same time */
        CFE_TBL_TxnLockRegistry(&Txn);

        Status = CFE_TBL_TxnCheckDuplicateRegistration(&Txn, TblName, Size);

        /* In error conditions or if this is a duplicate registration, no further work is required */
        if (Status == CFE_SUCCESS)
        {
            /* Search Access Descriptor Array for free Descriptor */
            Status = CFE_TBL_TxnAllocateHandle(&Txn);
        }

        /* If no errors, initialize the table registry entry and return the index to the caller as the handle */
        if (Status == CFE_SUCCESS)
        {
            /* Get pointer to Registry Record Entry to speed up processing */
            RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

            /* Initialize Registry Record to default settings */
            CFE_TBL_InitRegistryRecord(RegRecPtr);

            if ((TblOptionFlags & CFE_TBL_OPT_USR_DEF_MSK) != (CFE_TBL_OPT_USR_DEF_ADDR & CFE_TBL_OPT_USR_DEF_MSK))
            {
                RegRecPtr->UserDefAddr = false;

                Status = CFE_TBL_AllocateTableBuffer(RegRecPtr, Size);
            }
            else
            {
                /* Set buffer pointer to NULL for user-defined address tables */
                RegRecPtr->Buffers[0].BufferPtr = NULL;
                RegRecPtr->UserDefAddr          = true;
            }

            if (((TblOptionFlags & CFE_TBL_OPT_DBL_BUFFER) == CFE_TBL_OPT_DBL_BUFFER) && (Status == CFE_SUCCESS))
            {
                Status = CFE_TBL_AllocateSecondaryBuffer(RegRecPtr, Size);
            }
            else /* Single-Buffered Table */
            {
                RegRecPtr->DoubleBuffered    = false;
                RegRecPtr->ActiveBufferIndex = 0;
            }
        }

        /* If memory allocation was successful, register with table services and the CDS (if applicable) */
        if (Status == CFE_SUCCESS)
        {
            CFE_TBL_InitTableRegistryEntry(RegRecPtr, Size, TblValidationFuncPtr, TblName, TblOptionFlags);

            CFE_TBL_TxnConnectAccessDescriptor(&Txn);

            /* If the table is a critical table, allocate space for it in the Critical Data Store */
            /* OR locate its previous incarnation there and extract its previous contents */
            if ((TblOptionFlags & CFE_TBL_OPT_CRITICAL_MSK) == CFE_TBL_OPT_CRITICAL)
            {
                /* Register a CDS under the table name and determine if the table already exists there */
                Status = CFE_ES_RegisterCDSEx(&RegRecPtr->CDSHandle, Size, TblName, true);

                CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

                if (Status == CFE_ES_CDS_ALREADY_EXISTS)
                {
                    Status = CFE_TBL_RestoreTableDataFromCDS(RegRecPtr, AppName, Name, CritRegRecPtr);
                }

                /* Don't need to register with Critical Table Registry if table data was recovered */
                if (Status == CFE_SUCCESS)
                {
                    CFE_TBL_RegisterWithCriticalTableRegistry(CritRegRecPtr, RegRecPtr, TblName);
                }
                else if (Status != CFE_TBL_INFO_RECOVERED_TBL)
                {
                    CFE_ES_WriteToSysLog("%s: Failed to register '%s.%s' as a CDS (ErrCode=0x%08X)\n", __func__,
                                         AppName, Name, (unsigned int)Status);

                    /* Notify caller that although they asked for it to be critical, it isn't */
                    Status = CFE_TBL_WARN_NOT_CRITICAL;
                }
            }

            /* The last step of the registration process is claiming ownership.    */
            /* By making it the last step, other APIs do not have to lock registry */
            /* to share the table or get its address because registry entries that */
            /* are unowned are not checked to see if they match names, etc.        */
            RegRecPtr->OwnerAppId = ThisAppId;
        }

        /* Unlock Registry for update */
        CFE_TBL_TxnUnlockRegistry(&Txn);
    }

    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Make sure the returned handle is invalid when an error occurs */
        *TblHandlePtr = CFE_TBL_BAD_TABLE_HANDLE;

        CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName)); /* Get app name in case not avilable from earlier */
        CFE_EVS_SendEventWithAppID(CFE_TBL_REGISTER_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "%s Failed to Register '%s', Status=0x%08X", AppName, TblName, (unsigned int)Status);
    }
    else
    {
        *TblHandlePtr = CFE_TBL_TxnHandle(&Txn);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Share(CFE_TBL_Handle_t *TblHandlePtr, const char *TblName)
{
    CFE_TBL_TxnState_t          Txn;
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr            = NULL;
    CFE_TBL_RegistryRec_t *     RegRecPtr                = NULL;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    if (TblHandlePtr == NULL || TblName == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    Status = CFE_TBL_TxnStartFromName(&Txn, TblName, CFE_TBL_TxnContext_OTHER_APP);

    ThisAppId = CFE_TBL_TxnAppId(&Txn);

    if (Status == CFE_SUCCESS)
    {
        /* Search Access Descriptor Array for free Descriptor */
        Status = CFE_TBL_TxnAllocateHandle(&Txn);

        /* Check to make sure there was a handle available */
        if (Status == CFE_SUCCESS)
        {
            /* Initialize the Table Access Descriptor */
            AccessDescPtr = CFE_TBL_TxnAccDesc(&Txn);
            RegRecPtr     = CFE_TBL_TxnRegRec(&Txn);

            AccessDescPtr->AppId    = ThisAppId;
            AccessDescPtr->LockFlag = false;
            AccessDescPtr->Updated  = false;

            /* Check current state of table in order to set Notification flags properly */
            if (RegRecPtr->TableLoadedOnce)
            {
                AccessDescPtr->Updated = true;
            }

            AccessDescPtr->RegIndex = CFE_TBL_TxnRegId(&Txn);
            AccessDescPtr->UsedFlag = true;

            CFE_TBL_HandleLinkInit(&AccessDescPtr->Link);
            CFE_TBL_HandleListInsertLink(RegRecPtr, AccessDescPtr);
        }

        CFE_TBL_TxnFinish(&Txn);
    }

    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        *TblHandlePtr = CFE_TBL_BAD_TABLE_HANDLE;

        if (Status == CFE_TBL_ERR_INVALID_NAME)
        {
            CFE_ES_WriteToSysLog("%s: Table '%s' not found in Registry\n", __func__, TblName);
        }

        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

        CFE_EVS_SendEventWithAppID(CFE_TBL_SHARE_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "%s Failed to Share '%s', Status=0x%08X", AppName, TblName, (unsigned int)Status);
    }
    else
    {
        /* Export handle to caller */
        *TblHandlePtr = CFE_TBL_TxnHandle(&Txn);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Unregister(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t     Txn;
    int32                  Status;
    CFE_ES_AppId_t         ThisAppId;
    CFE_TBL_RegistryRec_t *RegRecPtr                = NULL;
    char                   AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_ACCESSOR_APP);

    ThisAppId = CFE_TBL_TxnAppId(&Txn);

    if (Status == CFE_SUCCESS)
    {
        /* Get a pointer to the relevant entry in the registry */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

        /* Verify that the application unregistering the table owns the table */
        if (CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, ThisAppId))
        {
            /* Mark table as free, although, technically, it isn't free until the */
            /* linked list of Access Descriptors has no links in it.              */
            /* NOTE: Allocated memory is freed when all Access Links have been    */
            /*       removed.  This allows Applications to continue to use the    */
            /*       data until they acknowledge that the table has been removed. */
            RegRecPtr->OwnerAppId = CFE_TBL_NOT_OWNED;

            /* Remove Table Name */
            RegRecPtr->Name[0] = '\0';
        }

        /* Remove the Access Descriptor Link from linked list */
        /* NOTE: If this removes the last access link, then   */
        /*       memory buffers are set free as well.         */
        CFE_TBL_TxnRemoveAccessLink(&Txn);

        CFE_TBL_TxnFinish(&Txn);
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: App(%lu) does not have access to Tbl Handle=%d\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(ThisAppId), (int)TblHandle);
    }

    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

        CFE_EVS_SendEventWithAppID(CFE_TBL_UNREGISTER_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "%s Failed to Unregister '?', Status=0x%08X", AppName, (unsigned int)Status);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Load(CFE_TBL_Handle_t TblHandle, CFE_TBL_SrcEnum_t SrcType, const void *SrcDataPtr)
{
    CFE_TBL_TxnState_t          Txn;
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_LoadBuff_t *        WorkingBufferPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};
    bool                        FirstTime                = false;

    if (SrcDataPtr == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Verify access rights and get a valid Application ID for calling App */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_OWNER_APP);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_HANDLE_ACCESS_ERR_EID, CFE_EVS_EventType_ERROR,
                                   CFE_TBL_Global.TableTaskAppId, "%s: No access to Tbl Handle=%d", AppName,
                                   (int)TblHandle);

        return Status;
    }

    AccessDescPtr = CFE_TBL_TxnAccDesc(&Txn);
    RegRecPtr     = CFE_TBL_TxnRegRec(&Txn);
    ThisAppId     = CFE_TBL_TxnAppId(&Txn);

    /*
     * This is not the end of the transaction - this is just put here for now
     * until the many inline "return" statements in this function can be cleaned up.
     *
     * This means nearly everything is subject to race conditions, but it is no worse
     * than it had been before.
     */
    CFE_TBL_TxnFinish(&Txn);

    /* Translate AppID of caller into App Name */
    CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

    /* Initialize return pointer to NULL */
    WorkingBufferPtr = NULL;

    /* Check to see if this is a dump only table */
    if (RegRecPtr->DumpOnly)
    {
        if ((!RegRecPtr->UserDefAddr) || (RegRecPtr->TableLoadedOnce))
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId, "%s: Attempted to load Dump Only Tbl '%s'",
                                       AppName, RegRecPtr->Name);

            return CFE_TBL_ERR_DUMP_ONLY;
        }

        /* The Application is allowed to call Load once when the address  */
        /* of the dump only table is being defined by the application.    */
        RegRecPtr->Buffers[0].BufferPtr = (void *)SrcDataPtr;
        RegRecPtr->TableLoadedOnce      = true;

        snprintf(RegRecPtr->Buffers[0].DataSource, sizeof(RegRecPtr->Buffers[0].DataSource), "Addr 0x%08lX",
                 (unsigned long)SrcDataPtr);

        RegRecPtr->Buffers[0].FileTime = CFE_TIME_ZERO_VALUE;

        CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_SUCCESS_INF_EID, CFE_EVS_EventType_DEBUG, CFE_TBL_Global.TableTaskAppId,
                                   "Successfully loaded '%s' from '%s'", RegRecPtr->Name,
                                   RegRecPtr->Buffers[0].DataSource);

        return CFE_SUCCESS;
    }

    /* Loads by an Application are not allowed if a table load is already in progress */
    if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_IN_PROGRESS_ERR_EID, CFE_EVS_EventType_ERROR,
                                   CFE_TBL_Global.TableTaskAppId, "%s: Load already in progress for '%s'", AppName,
                                   RegRecPtr->Name);

        return CFE_TBL_ERR_LOAD_IN_PROGRESS;
    }

    /* Obtain a working buffer (either the table's dedicated buffer or one of the shared buffers) */
    Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, true);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_NO_WORK_BUFFERS_ERR_EID, CFE_EVS_EventType_ERROR,
                                   CFE_TBL_Global.TableTaskAppId, "%s: Failed to get Working Buffer (Stat=%u)", AppName,
                                   (unsigned int)Status);

        return Status;
    }

    /* Perform appropriate update to working buffer */
    /* Determine whether the load is to occur from a file or from a block of memory */
    switch (SrcType)
    {
        case CFE_TBL_SRC_FILE:
            /* Load the data from the file into the specified buffer */
            Status = CFE_TBL_LoadFromFile(AppName, WorkingBufferPtr, RegRecPtr, (const char *)SrcDataPtr);

            if ((Status == CFE_TBL_WARN_PARTIAL_LOAD) && (!RegRecPtr->TableLoadedOnce))
            {
                /* Uninitialized tables cannot be loaded with partial table loads */
                /* Partial loads can only occur on previously loaded tables.      */
                CFE_EVS_SendEventWithAppID(CFE_TBL_PARTIAL_LOAD_ERR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_TBL_Global.TableTaskAppId,
                                           "%s: Attempted to load from partial Tbl '%s' from '%s' (Stat=%u)", AppName,
                                           RegRecPtr->Name, (const char *)SrcDataPtr, (unsigned int)Status);

                Status = CFE_TBL_ERR_PARTIAL_LOAD;
            }

            break;
        case CFE_TBL_SRC_ADDRESS:
            /* When the source is a block of memory, it is assumed to be a complete load */
            memcpy(WorkingBufferPtr->BufferPtr, (uint8 *)SrcDataPtr, RegRecPtr->Size);

            snprintf(WorkingBufferPtr->DataSource, sizeof(WorkingBufferPtr->DataSource), "Addr 0x%08lX",
                     (unsigned long)SrcDataPtr);

            WorkingBufferPtr->FileTime = CFE_TIME_ZERO_VALUE;

            /* Compute the CRC on the specified table buffer */
            WorkingBufferPtr->Crc =
                CFE_ES_CalculateCRC(WorkingBufferPtr->BufferPtr, RegRecPtr->Size, 0, CFE_MISSION_ES_DEFAULT_CRC);

            break;
        default:
            CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_TYPE_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId,
                                       "%s: Attempted to load from illegal source type=%d", AppName, (int)SrcType);

            Status = CFE_TBL_ERR_ILLEGAL_SRC_TYPE;
    }

    /* If the data was successfully loaded, then validate its contents */
    if ((Status >= CFE_SUCCESS) && (RegRecPtr->ValidationFuncPtr != NULL))
    {
        Status = (RegRecPtr->ValidationFuncPtr)(WorkingBufferPtr->BufferPtr);

        if (Status > CFE_SUCCESS)
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_VAL_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                       "%s: Validation func return code invalid (Stat=%u) for '%s'", AppName,
                                       (unsigned int)Status, RegRecPtr->Name);

            Status = -1;
        }

        if (Status < 0)
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId,
                                       "%s: Validation func reports table invalid (Stat=%u) for '%s'", AppName,
                                       (unsigned int)Status, RegRecPtr->Name);

            /* Zero out the buffer to remove any bad data */
            memset(WorkingBufferPtr->BufferPtr, 0, RegRecPtr->Size);
        }
    }

    /* Perform the table update to complete the load */
    if (Status < CFE_SUCCESS)
    {
        /* The load has had a problem, free the working buffer for another attempt */
        if ((!RegRecPtr->DoubleBuffered) && (RegRecPtr->TableLoadedOnce == true))
        {
            /* For single-buffered tables, freeing entails resetting flag */
            CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Taken = false;
        }

        /* For double-buffered tables, freeing buffer is simple */
        RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;

        return Status;
    }

    FirstTime = !RegRecPtr->TableLoadedOnce;

    /* If this is not the first load, then the data must be moved from the inactive buffer      */
    /* to the active buffer to complete the load.  First loads are done directly to the active. */
    if (!FirstTime)
    {
        /* Force the table update */
        RegRecPtr->LoadPending = true;

        Status = CFE_TBL_UpdateInternal(TblHandle, RegRecPtr, AccessDescPtr);

        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                       "%s: Failed to update '%s' (Stat=%u)", AppName, RegRecPtr->Name,
                                       (unsigned int)Status);
        }
    }
    else
    {
        /* On initial loads, make sure registry is given file/address of data source */
        strncpy(RegRecPtr->LastFileLoaded, WorkingBufferPtr->DataSource, sizeof(RegRecPtr->LastFileLoaded) - 1);
        RegRecPtr->LastFileLoaded[sizeof(RegRecPtr->LastFileLoaded) - 1] = '\0';

        CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);

        /* If the table is a critical table, update the appropriate CDS with the new data */
        if (RegRecPtr->CriticalTable == true)
        {
            CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
        }

        Status = CFE_SUCCESS;
    }

    if (Status == CFE_SUCCESS)
    {
        /* The first time a table is loaded, the event message is DEBUG */
        /* to help eliminate a flood of events during a startup         */
        CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_SUCCESS_INF_EID,
                                   FirstTime ? CFE_EVS_EventType_DEBUG : CFE_EVS_EventType_INFORMATION,
                                   CFE_TBL_Global.TableTaskAppId, "Successfully loaded '%s' from '%s'", RegRecPtr->Name,
                                   RegRecPtr->LastFileLoaded);

        /* Save the index of the table for housekeeping telemetry */
        CFE_TBL_Global.LastTblUpdated = AccessDescPtr->RegIndex;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Update(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t          Txn;
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_RegistryRec_t *     RegRecPtr                = NULL;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr            = NULL;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    /* Verify access rights and get a valid Application ID for calling App */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_OWNER_APP);

    ThisAppId = CFE_TBL_TxnAppId(&Txn);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = CFE_TBL_TxnAccDesc(&Txn);
        RegRecPtr     = CFE_TBL_TxnRegRec(&Txn);

        Status = CFE_TBL_UpdateInternal(TblHandle, RegRecPtr, AccessDescPtr);

        CFE_TBL_TxnFinish(&Txn);

        if (Status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: App(%lu) fail to update Tbl '%s' (Stat=0x%08X)\n", __func__,
                                 CFE_RESOURCEID_TO_ULONG(ThisAppId), RegRecPtr->Name, (unsigned int)Status);
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: App(%lu) does not have access to Tbl Handle=%d\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(ThisAppId), (int)TblHandle);
    }

    if (Status != CFE_ES_ERR_RESOURCEID_NOT_VALID)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));
    }

    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /*
         * Note that (Status < 0) specifically matches ERROR, not WARNING codes. The CFE_TBL_UpdateInternal() function
         * currently only produces two possible codes (aside from CFE_SUCCESS) and both of these are defined as
         * warnings, not errors.  Therefore, its impossible to reach this code with RegRecPtr != NULL.
         */
        CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "%s Failed to update table, Status=0x%08X", AppName, (unsigned int)Status);
    }
    else
    {
        /* If there was a warning (ie - Table is currently locked), then do not issue a message */
        if (Status == CFE_SUCCESS)
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_SUCCESS_INF_EID, CFE_EVS_EventType_INFORMATION,
                                       CFE_TBL_Global.TableTaskAppId, "%s Successfully Updated '%s'", AppName,
                                       RegRecPtr->Name);

            /* Save the index of the table for housekeeping telemetry */
            CFE_TBL_Global.LastTblUpdated = AccessDescPtr->RegIndex;
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_GetAddress(void **TblPtr, CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t Txn;
    int32              Status;
    CFE_ES_AppId_t     ThisAppId;

    if (TblPtr == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Assume failure at returning the table address */
    *TblPtr = NULL;

    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_ACCESSOR_APP);

    if (Status == CFE_SUCCESS)
    {
        Status = CFE_TBL_TxnGetTableAddress(&Txn, TblPtr);

        /* NOTE: GetAddressInternal calls GetNextNotification which may not */
        /*       be equal to CFE_SUCCESS and still not be an error.         */
        /*       Therefore, a write to the SysLog is unnecessary.           */

        CFE_TBL_TxnFinish(&Txn);
    }
    else
    {
        ThisAppId = CFE_TBL_TxnAppId(&Txn);

        CFE_ES_WriteToSysLog("%s: Bad AppId=%lu\n", __func__, CFE_RESOURCEID_TO_ULONG(ThisAppId));
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ReleaseAddress(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t Txn;
    int32              Status;
    CFE_ES_AppId_t     ThisAppId;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_ACCESSOR_APP);

    if (Status == CFE_SUCCESS)
    {
        /* Clear the lock flag */
        CFE_TBL_Global.Handles[TblHandle].LockFlag = false;

        /* Return any pending warning or info status indicators */
        Status = CFE_TBL_TxnGetNextNotification(&Txn);

        /* NOTE: GetNextNotification may not return CFE_SUCCESS  */
        /*       and still not be an error.                      */
        /*       Therefore, a write to the SysLog is unnecessary.*/

        CFE_TBL_TxnFinish(&Txn);
    }
    else
    {
        ThisAppId = CFE_TBL_TxnAppId(&Txn);

        CFE_ES_WriteToSysLog("%s: App(%lu) does not have access to Tbl Handle=%u\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(ThisAppId), (unsigned int)TblHandle);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_GetAddresses(void **TblPtrs[], uint16 NumTables, const CFE_TBL_Handle_t TblHandles[])
{
    CFE_TBL_TxnState_t Txn;
    CFE_Status_t       FinalStatus;
    uint16             i;
    int32              Status;
    CFE_ES_AppId_t     ThisAppId;

    if (TblPtrs == NULL || TblHandles == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Assume failure at returning the table addresses */
    for (i = 0; i < NumTables; i++)
    {
        *TblPtrs[i] = NULL;
    }

    FinalStatus = CFE_SUCCESS;
    for (i = 0; i < NumTables; i++)
    {
        Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandles[i], CFE_TBL_TxnContext_ACCESSOR_APP);
        if (Status == CFE_SUCCESS)
        {
            Status = CFE_TBL_TxnGetTableAddress(&Txn, TblPtrs[i]);

            CFE_TBL_TxnFinish(&Txn);
        }

        if (FinalStatus == CFE_SUCCESS)
        {
            FinalStatus = Status;
        }

        if (Status == CFE_ES_ERR_RESOURCEID_NOT_VALID)
        {
            ThisAppId = CFE_TBL_TxnAppId(&Txn);
            CFE_ES_WriteToSysLog("%s: Bad AppId=%lu\n", __func__, CFE_RESOURCEID_TO_ULONG(ThisAppId));
            break;
        }
    }

    return FinalStatus;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ReleaseAddresses(uint16 NumTables, const CFE_TBL_Handle_t TblHandles[])
{
    CFE_Status_t Status = CFE_SUCCESS;
    uint16       i;

    if (TblHandles == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    for (i = 0; i < NumTables; i++)
    {
        /* Continue to get the return status until one returns something other than CFE_SUCCESS */
        if (Status == CFE_SUCCESS)
        {
            Status = CFE_TBL_ReleaseAddress(TblHandles[i]);
        }
        else
        {
            /* Don't bother getting the status of other tables once one has returned */
            /* a non CFE_SUCCESS value.                                              */
            CFE_TBL_ReleaseAddress(TblHandles[i]);
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Validate(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t          Txn;
    int32                       Status;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};
    CFE_TBL_LoadBuff_t *        BuffPtr;
    CFE_TBL_ValidationResult_t *ResultPtr;
    const char *                LogTagStr;

    ResultPtr = NULL;
    BuffPtr   = NULL;
    LogTagStr = "(none)";

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_OWNER_APP);
    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

        /* Identify the image to be validated, starting with the Inactive Buffer */
        ResultPtr = CFE_TBL_CheckValidationRequest(&RegRecPtr->ValidateInactiveId);
        if (ResultPtr != NULL)
        {
            LogTagStr = "inactive";
            BuffPtr   = CFE_TBL_GetInactiveBuffer(RegRecPtr);
        }
        else
        {
            ResultPtr = CFE_TBL_CheckValidationRequest(&RegRecPtr->ValidateActiveId);
            if (ResultPtr != NULL)
            {
                LogTagStr = "active";
                BuffPtr   = CFE_TBL_GetActiveBuffer(RegRecPtr);
            }
        }

        CFE_TBL_TxnFinish(&Txn);

        if (ResultPtr != NULL)
        {
            if (BuffPtr == NULL)
            {
                /* No buffer, it cannot be valid */
                ResultPtr->Result = -1;
            }
            else if (RegRecPtr->ValidationFuncPtr == NULL)
            {
                /* no validation function, assume its OK */
                ResultPtr->Result = 0;
            }
            else
            {
                /* Save the result of the Validation function for the Table Services Task */
                ResultPtr->Result = (RegRecPtr->ValidationFuncPtr)(BuffPtr->BufferPtr);
            }

            /* Get the app name for logging */
            CFE_ES_GetAppName(AppName, CFE_TBL_TxnAppId(&Txn), sizeof(AppName));

            /* Allow buffer to be activated after passing validation */
            if (ResultPtr->Result == 0)
            {
                BuffPtr->Validated = true;
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_INF_EID, CFE_EVS_EventType_INFORMATION,
                                           CFE_TBL_Global.TableTaskAppId, "%s validation successful for %s '%s'",
                                           AppName, LogTagStr, RegRecPtr->Name);
            }
            else
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_ERR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_TBL_Global.TableTaskAppId,
                                           "%s validation failed for %s '%s', Status=0x%08X", AppName, RegRecPtr->Name,
                                           LogTagStr, (unsigned int)Status);

                if (ResultPtr->Result > 0)
                {
                    CFE_ES_WriteToSysLog("%s: App(%lu) Validation func return code invalid (Stat=0x%08X) for '%s'\n",
                                         __func__, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.TableTaskAppId),
                                         (unsigned int)ResultPtr->Result, RegRecPtr->Name);
                }
            }

            /* Once validation is complete, set flags to indicate response is ready */
            ResultPtr->State = CFE_TBL_VALIDATION_PERFORMED;

            /* Since the validation was successfully performed (although maybe not a successful result) */
            /* return a success status */
            Status = CFE_SUCCESS;
        }
        else
        {
            Status = CFE_TBL_INFO_NO_VALIDATION_PENDING;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: App(%lu) does not have access to Tbl Handle=%lu\n", __func__,
                             CFE_TBL_TxnAppIdAsULong(&Txn), CFE_TBL_TxnHandleAsULong(&Txn));
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Manage(CFE_TBL_Handle_t TblHandle)
{
    int32 Status           = CFE_SUCCESS;
    bool  FinishedManaging = false;

    while (!FinishedManaging)
    {
        /* Determine if the table has a validation or update that needs to be performed */
        Status = CFE_TBL_GetStatus(TblHandle);

        if (Status == CFE_TBL_INFO_VALIDATION_PENDING)
        {
            /* Validate the specified Table */
            Status = CFE_TBL_Validate(TblHandle);

            if (Status != CFE_SUCCESS)
            {
                /* If an error occurred during Validate, then do not perform any more managing */
                FinishedManaging = true;
            }
        }
        else if (Status == CFE_TBL_INFO_DUMP_PENDING)
        {
            /* Dump the specified Table */
            Status = CFE_TBL_DumpToBuffer(TblHandle);

            /* After a Dump, always assume we are done (Dumps are on DumpOnly tables and cannot be "Updated") */
            FinishedManaging = true;
        }
        else if (Status == CFE_TBL_INFO_UPDATE_PENDING)
        {
            /* Update the specified Table */
            Status = CFE_TBL_Update(TblHandle);

            /* If the update performed nominally, let the caller know the table has changed */
            if (Status == CFE_SUCCESS)
            {
                Status = CFE_TBL_INFO_UPDATED;
            }

            /* After an Update, always assume we are done and return Update Status */
            FinishedManaging = true;
        }
        else
        {
            FinishedManaging = true;
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_GetStatus(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t Txn;
    int32              Status;
    CFE_ES_AppId_t     ThisAppId;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_ACCESSOR_APP);

    if (Status == CFE_SUCCESS)
    {
        Status = CFE_TBL_TxnGetTableStatus(&Txn);

        CFE_TBL_TxnFinish(&Txn);
    }
    else
    {
        ThisAppId = CFE_TBL_TxnAppId(&Txn);

        CFE_ES_WriteToSysLog("%s: App(%lu) does not have access to Tbl Handle=%d\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(ThisAppId), (int)TblHandle);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_GetInfo(CFE_TBL_Info_t *TblInfoPtr, const char *TblName)
{
    CFE_TBL_TxnState_t     Txn;
    int32                  Status               = CFE_SUCCESS;
    int32                  NumAccessDescriptors = 0;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    if (TblInfoPtr == NULL || TblName == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    Status = CFE_TBL_TxnStartFromName(&Txn, TblName, CFE_TBL_TxnContext_UNDEFINED);

    /* If we found the table, then extract the information from the Registry */
    if (Status == CFE_SUCCESS)
    {
        /* Get pointer to Registry Record Entry to speed up processing */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

        /* Return table characteristics */
        TblInfoPtr->Size            = RegRecPtr->Size;
        TblInfoPtr->DoubleBuffered  = RegRecPtr->DoubleBuffered;
        TblInfoPtr->DumpOnly        = RegRecPtr->DumpOnly;
        TblInfoPtr->UserDefAddr     = RegRecPtr->UserDefAddr;
        TblInfoPtr->TableLoadedOnce = RegRecPtr->TableLoadedOnce;

        /* Return information on last load and update */
        TblInfoPtr->TimeOfLastUpdate = RegRecPtr->TimeOfLastUpdate;
        TblInfoPtr->FileTime         = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileTime;
        TblInfoPtr->Crc              = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].Crc;
        strncpy(TblInfoPtr->LastFileLoaded, RegRecPtr->LastFileLoaded, sizeof(TblInfoPtr->LastFileLoaded) - 1);
        TblInfoPtr->LastFileLoaded[sizeof(TblInfoPtr->LastFileLoaded) - 1] = 0;

        CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_CountAccessDescHelper, &NumAccessDescriptors);

        TblInfoPtr->NumUsers = NumAccessDescriptors;

        TblInfoPtr->Critical = RegRecPtr->CriticalTable;

        CFE_TBL_TxnFinish(&Txn);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DumpToBuffer(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t     Txn;
    int32                  Status;
    CFE_TBL_RegistryRec_t *RegRecPtr   = NULL;
    CFE_TBL_DumpControl_t *DumpCtrlPtr = NULL;
    CFE_TBL_LoadBuff_t *   ActiveBufPtr;

    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_ACCESSOR_APP);

    if (Status == CFE_SUCCESS)
    {
        Status = CFE_TBL_TxnGetTableStatus(&Txn);

        /* Make sure the table has been requested to be dumped */
        if (Status == CFE_TBL_INFO_DUMP_PENDING)
        {
            RegRecPtr    = CFE_TBL_TxnRegRec(&Txn);
            DumpCtrlPtr  = CFE_TBL_LocateDumpCtrlByID(RegRecPtr->DumpControlId);
            ActiveBufPtr = CFE_TBL_GetActiveBuffer(RegRecPtr);

            /* Copy the contents of the active buffer to the assigned dump buffer */
            memcpy(DumpCtrlPtr->DumpBufferPtr->BufferPtr, ActiveBufPtr->BufferPtr, DumpCtrlPtr->Size);

            /* Save the current time so that the header in the dump file can have the correct time */
            DumpCtrlPtr->DumpBufferPtr->FileTime = CFE_TIME_GetTime();

            /* Disassociate the dump request from the table */
            RegRecPtr->DumpControlId = CFE_TBL_NO_DUMP_PENDING;

            /* Notify the Table Services Application that the dump buffer is ready to be written to a file */
            DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;

            Status = CFE_SUCCESS;
        }

        CFE_TBL_TxnFinish(&Txn);
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: App(%lu) does not have access to Tbl Handle=%lu\n", __func__,
                             CFE_TBL_TxnAppIdAsULong(&Txn), CFE_TBL_TxnHandleAsULong(&Txn));
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function, not invoked outside this unit
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
static void CFE_TBL_NotifyOtherAppHelper(CFE_TBL_AccessDescriptor_t *AccessDescPtr, void *Arg)
{
    CFE_TBL_TxnState_t *Txn = Arg;

    /* Only notify *OTHER* applications that the contents have changed */
    if (!CFE_RESOURCEID_TEST_EQUAL(AccessDescPtr->AppId, CFE_TBL_TxnAppId(Txn)))
    {
        AccessDescPtr->Updated = true;
    }
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Modified(CFE_TBL_Handle_t TblHandle)
{
    CFE_TBL_TxnState_t     Txn;
    int32                  Status;
    CFE_TBL_RegistryRec_t *RegRecPtr = NULL;
    CFE_ES_AppId_t         ThisAppId;
    size_t                 FilenameLen;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_ACCESSOR_APP);

    ThisAppId = CFE_TBL_TxnAppId(&Txn);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

        CFE_TBL_TxnFinish(&Txn);

        /* If the table is a critical table, update the appropriate CDS with the new data */
        if (RegRecPtr->CriticalTable == true)
        {
            CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
        }

        /* Keep a record of change for the ground operators reference */
        RegRecPtr->TimeOfLastUpdate                                      = CFE_TIME_GetTime();
        RegRecPtr->LastFileLoaded[sizeof(RegRecPtr->LastFileLoaded) - 1] = '\0';

        /* Update CRC on contents of table */
        RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].Crc = CFE_ES_CalculateCRC(
            RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr, RegRecPtr->Size, 0, CFE_MISSION_ES_DEFAULT_CRC);

        FilenameLen = strlen(RegRecPtr->LastFileLoaded);
        if (FilenameLen < (sizeof(RegRecPtr->LastFileLoaded) - 4))
        {
            strncpy(&RegRecPtr->LastFileLoaded[FilenameLen], "(*)", 4);
        }
        else
        {
            strncpy(&RegRecPtr->LastFileLoaded[sizeof(RegRecPtr->LastFileLoaded) - 4], "(*)", 4);
        }

        /* Only notify *OTHER* applications that the contents have changed */
        CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_NotifyOtherAppHelper, &Txn);
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: App(%lu) does not have access to Tbl Handle=%d\n", __func__,
                             CFE_RESOURCEID_TO_ULONG(ThisAppId), (int)TblHandle);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_NotifyByMessage(CFE_TBL_Handle_t TblHandle, CFE_SB_MsgId_t MsgId, CFE_MSG_FcnCode_t CommandCode,
                                     uint32 Parameter)
{
    CFE_TBL_TxnState_t     Txn;
    int32                  Status;
    CFE_TBL_RegistryRec_t *RegRecPtr = NULL;
    CFE_ES_AppId_t         ThisAppId;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_TxnStartFromHandle(&Txn, TblHandle, CFE_TBL_TxnContext_ACCESSOR_APP);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);
        ThisAppId = CFE_TBL_TxnAppId(&Txn);

        CFE_TBL_TxnFinish(&Txn);

        /* Verify that the calling application is the table owner */
        if (CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, ThisAppId))
        {
            RegRecPtr->NotificationMsgId = MsgId;
            RegRecPtr->NotificationCC    = CommandCode;
            RegRecPtr->NotificationParam = Parameter;
            RegRecPtr->NotifyByMsg       = true;
        }
        else
        {
            Status = CFE_TBL_ERR_NO_ACCESS;
            CFE_ES_WriteToSysLog("%s: App(%lu) does not own Tbl Handle=%d\n", __func__,
                                 CFE_RESOURCEID_TO_ULONG(ThisAppId), (int)TblHandle);
        }
    }

    return Status;
}
