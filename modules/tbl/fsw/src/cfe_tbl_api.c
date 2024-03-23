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
    CFE_TBL_RegistryRec_t *RegRecPtr     = NULL;
    CFE_TBL_CritRegRec_t * CritRegRecPtr = NULL;
    CFE_Status_t           Status;
    int16                  RegIndx;
    CFE_ES_AppId_t         ThisAppId;
    char                   AppName[OS_MAX_API_NAME]           = {"UNKNOWN"};
    char                   TblName[CFE_TBL_MAX_FULL_NAME_LEN] = {""};

    if (TblHandlePtr == NULL || Name == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Check to make sure calling application is legit */
    Status = CFE_ES_GetAppID(&ThisAppId);

    /* Validate table input parameters (Name, Size and Options) */
    if (Status == CFE_SUCCESS)
    {
        /* Assume we can't make a table and return a bad handle for now */
        *TblHandlePtr = CFE_TBL_BAD_TABLE_HANDLE;

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
        CFE_TBL_LockRegistry();

        Status = CFE_TBL_CheckForDuplicateRegistration(&RegIndx, TblName, RegRecPtr, ThisAppId, Size, TblHandlePtr);

        /* In error conditions or if this is a duplicate registration, no further work is required */
        if (Status == CFE_SUCCESS)
        {
            /* Search Access Descriptor Array for free Descriptor */
            *TblHandlePtr = CFE_TBL_FindFreeHandle();

            /* Check to make sure there was a handle available */
            if (*TblHandlePtr == CFE_TBL_END_OF_LIST)
            {
                Status = CFE_TBL_ERR_HANDLES_FULL;
                CFE_ES_WriteToSysLog("%s: No more free handles\n", __func__);
            }
        }

        /* If no errors, initialize the table registry entry and return the index to the caller as the handle */
        if (Status == CFE_SUCCESS)
        {
            /* Get pointer to Registry Record Entry to speed up processing */
            RegRecPtr = &CFE_TBL_Global.Registry[RegIndx];

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

            CFE_TBL_InitTableAccessDescriptor(TblHandlePtr, ThisAppId, RegRecPtr, RegIndx);

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
        CFE_TBL_UnlockRegistry();
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
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    int16                       RegIndx;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr            = NULL;
    CFE_TBL_RegistryRec_t *     RegRecPtr                = NULL;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    if (TblHandlePtr == NULL || TblName == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Get a valid Application ID for calling App */
    Status = CFE_ES_GetAppID(&ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Lock Registry for update.  This prevents two applications from        */
        /* trying to register/share tables at the same location at the same time */
        CFE_TBL_LockRegistry();

        RegIndx = CFE_TBL_FindTableInRegistry(TblName);

        /* If we found the table, then get a new Access Descriptor and initialize it */
        if (RegIndx != CFE_TBL_NOT_FOUND)
        {
            /* Get pointer to Registry Record Entry to speed up processing */
            RegRecPtr = &CFE_TBL_Global.Registry[RegIndx];

            /* Search Access Descriptor Array for free Descriptor */
            *TblHandlePtr = CFE_TBL_FindFreeHandle();

            /* Check to make sure there was a handle available */
            if (*TblHandlePtr == CFE_TBL_END_OF_LIST)
            {
                Status = CFE_TBL_ERR_HANDLES_FULL;
                CFE_ES_WriteToSysLog("%s: No more free handles\n", __func__);
            }
            else
            {
                /* Initialize the Table Access Descriptor */
                AccessDescPtr = &CFE_TBL_Global.Handles[*TblHandlePtr];

                AccessDescPtr->AppId    = ThisAppId;
                AccessDescPtr->LockFlag = false;
                AccessDescPtr->Updated  = false;

                /* Check current state of table in order to set Notification flags properly */
                if (RegRecPtr->TableLoadedOnce)
                {
                    AccessDescPtr->Updated = true;
                }

                AccessDescPtr->RegIndex = RegIndx;
                AccessDescPtr->UsedFlag = true;

                AccessDescPtr->PrevLink = CFE_TBL_END_OF_LIST; /* We are the new head of the list */
                AccessDescPtr->NextLink = RegRecPtr->HeadOfAccessList;

                /* Make sure the old head of the list now sees this as the head */
                CFE_TBL_Global.Handles[RegRecPtr->HeadOfAccessList].PrevLink = *TblHandlePtr;

                /* Make sure the Registry Record see this as the head of the list */
                RegRecPtr->HeadOfAccessList = *TblHandlePtr;
            }
        }
        else /* Table could not be found in registry */
        {
            Status = CFE_TBL_ERR_INVALID_NAME;

            CFE_ES_WriteToSysLog("%s: Table '%s' not found in Registry\n", __func__, TblName);
        }

        CFE_TBL_UnlockRegistry();
    }
    else /* Application ID was invalid */
    {
        CFE_ES_WriteToSysLog("%s: Bad AppId(%lu)\n", __func__, CFE_RESOURCEID_TO_ULONG(ThisAppId));
    }

    /* On Error conditions, notify ground of screw up */
    if (Status < 0)
    {
        /* Translate AppID of caller into App Name */
        CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

        CFE_EVS_SendEventWithAppID(CFE_TBL_SHARE_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "%s Failed to Share '%s', Status=0x%08X", AppName, TblName, (unsigned int)Status);
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
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_RegistryRec_t *     RegRecPtr                = NULL;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr            = NULL;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get a pointer to the relevant Access Descriptor */
        AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];

        /* Get a pointer to the relevant entry in the registry */
        RegRecPtr = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

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
        CFE_TBL_RemoveAccessLink(TblHandle);
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
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_HANDLE_ACCESS_ERR_EID, CFE_EVS_EventType_ERROR,
                                   CFE_TBL_Global.TableTaskAppId, "%s: No access to Tbl Handle=%d", AppName,
                                   (int)TblHandle);

        return Status;
    }

    AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];
    RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

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
        RegRecPtr->Buffers[0].FileCreateTimeSecs    = 0;
        RegRecPtr->Buffers[0].FileCreateTimeSubSecs = 0;

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
            WorkingBufferPtr->FileCreateTimeSecs    = 0;
            WorkingBufferPtr->FileCreateTimeSubSecs = 0;

            /* Compute the CRC on the specified table buffer */
            WorkingBufferPtr->Crc =
                CFE_ES_CalculateCRC(WorkingBufferPtr->BufferPtr, RegRecPtr->Size, 0, CFE_MISSION_ES_DEFAULT_CRC);

            break;
        default:
            CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_TYPE_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId,
                                       "%s: Attempted to load from illegal source type=%d", AppName, (int)SrcType);

            Status = CFE_TBL_ERR_ILLEGAL_SRC_TYPE;
            break;
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
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_RegistryRec_t *     RegRecPtr                = NULL;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr            = NULL;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    /* Verify access rights and get a valid Application ID for calling App */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];
        RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

        Status = CFE_TBL_UpdateInternal(TblHandle, RegRecPtr, AccessDescPtr);

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
         * warnings, not errors.  Therefore, its impossible to reach this code with RegRegPtr != NULL.
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
    int32          Status;
    CFE_ES_AppId_t ThisAppId;

    if (TblPtr == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Assume failure at returning the table address */
    *TblPtr = NULL;

    /* Validate the calling application's AppID */
    Status = CFE_ES_GetAppID(&ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        Status = CFE_TBL_GetAddressInternal(TblPtr, TblHandle, ThisAppId);

        /* NOTE: GetAddressInternal calls GetNextNotification which may not */
        /*       be equal to CFE_SUCCESS and still not be an error.         */
        /*       Therefore, a write to the SysLog is unnecessary.           */
    }
    else
    {
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
    int32          Status;
    CFE_ES_AppId_t ThisAppId;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Clear the lock flag */
        CFE_TBL_Global.Handles[TblHandle].LockFlag = false;

        /* Return any pending warning or info status indicators */
        Status = CFE_TBL_GetNextNotification(TblHandle);

        /* NOTE: GetNextNotification may not return CFE_SUCCESS  */
        /*       and still not be an error.                      */
        /*       Therefore, a write to the SysLog is unnecessary.*/
    }
    else
    {
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
    uint16         i;
    int32          Status;
    CFE_ES_AppId_t ThisAppId;

    if (TblPtrs == NULL || TblHandles == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    /* Assume failure at returning the table addresses */
    for (i = 0; i < NumTables; i++)
    {
        *TblPtrs[i] = NULL;
    }

    /* Validate the calling application's AppID */
    Status = CFE_ES_GetAppID(&ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        for (i = 0; i < NumTables; i++)
        {
            /* Continue to get the return status until one returns something other than CFE_SUCCESS */
            if (Status == CFE_SUCCESS)
            {
                Status = CFE_TBL_GetAddressInternal(TblPtrs[i], TblHandles[i], ThisAppId);
            }
            else
            {
                /* Don't bother getting the status of other tables once one has returned */
                /* a non CFE_SUCCESS value.                                              */
                CFE_TBL_GetAddressInternal(TblPtrs[i], TblHandles[i], ThisAppId);
            }
        }
    }
    else
    {
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
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;
    char                        AppName[OS_MAX_API_NAME] = {"UNKNOWN"};

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];
        RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

        CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

        /* Identify the image to be validated, starting with the Inactive Buffer */
        if (RegRecPtr->ValidateInactiveIndex != CFE_TBL_NO_VALIDATION_PENDING)
        {
            /* Identify whether the Inactive Buffer is a shared buffer or a dedicated one */
            if (RegRecPtr->DoubleBuffered)
            {
                /* Call the Application's Validation function for the Inactive Buffer */
                Status =
                    (RegRecPtr->ValidationFuncPtr)(RegRecPtr->Buffers[(1U - RegRecPtr->ActiveBufferIndex)].BufferPtr);

                /* Allow buffer to be activated after passing validation */
                if (Status == CFE_SUCCESS)
                {
                    RegRecPtr->Buffers[(1U - RegRecPtr->ActiveBufferIndex)].Validated = true;
                }
            }
            else
            {
                /* Call the Application's Validation function for the appropriate shared buffer */
                Status = (RegRecPtr->ValidationFuncPtr)(CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr);

                /* Allow buffer to be activated after passing validation */
                if (Status == CFE_SUCCESS)
                {
                    CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Validated = true;
                }
            }

            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_INF_EID, CFE_EVS_EventType_INFORMATION,
                                           CFE_TBL_Global.TableTaskAppId, "%s validation successful for Inactive '%s'",
                                           AppName, RegRecPtr->Name);
            }
            else
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_ERR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_TBL_Global.TableTaskAppId,
                                           "%s validation failed for Inactive '%s', Status=0x%08X", AppName,
                                           RegRecPtr->Name, (unsigned int)Status);

                if (Status > CFE_SUCCESS)
                {
                    CFE_ES_WriteToSysLog("%s: App(%lu) Validation func return code invalid (Stat=0x%08X) for '%s'\n",
                                         __func__, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.TableTaskAppId),
                                         (unsigned int)Status, RegRecPtr->Name);
                }
            }

            /* Save the result of the Validation function for the Table Services Task */
            CFE_TBL_Global.ValidationResults[RegRecPtr->ValidateInactiveIndex].Result = Status;

            /* Once validation is complete, set flags to indicate response is ready */
            CFE_TBL_Global.ValidationResults[RegRecPtr->ValidateInactiveIndex].State = CFE_TBL_VALIDATION_PERFORMED;
            RegRecPtr->ValidateInactiveIndex                                         = CFE_TBL_NO_VALIDATION_PENDING;

            /* Since the validation was successfully performed (although maybe not a successful result) */
            /* return a success status */
            Status = CFE_SUCCESS;
        }
        else if (RegRecPtr->ValidateActiveIndex != CFE_TBL_NO_VALIDATION_PENDING)
        {
            /* Perform validation on the currently active table buffer */
            /* Identify whether the Active Buffer is a shared buffer or a dedicated one */
            if (RegRecPtr->DoubleBuffered)
            {
                /* Call the Application's Validation function for the Dedicated Active Buffer */
                Status = (RegRecPtr->ValidationFuncPtr)(RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr);
            }
            else
            {
                /* Call the Application's Validation function for the static buffer */
                Status = (RegRecPtr->ValidationFuncPtr)(RegRecPtr->Buffers[0].BufferPtr);
            }

            if (Status == CFE_SUCCESS)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_INF_EID, CFE_EVS_EventType_INFORMATION,
                                           CFE_TBL_Global.TableTaskAppId, "%s validation successful for Active '%s'",
                                           AppName, RegRecPtr->Name);
            }
            else
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_VALIDATION_ERR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_TBL_Global.TableTaskAppId,
                                           "%s validation failed for Active '%s', Status=0x%08X", AppName,
                                           RegRecPtr->Name, (unsigned int)Status);

                if (Status > CFE_SUCCESS)
                {
                    CFE_ES_WriteToSysLog("%s: App(%lu) Validation func return code invalid (Stat=0x%08X) for '%s'\n",
                                         __func__, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.TableTaskAppId),
                                         (unsigned int)Status, RegRecPtr->Name);
                }
            }

            /* Save the result of the Validation function for the Table Services Task */
            CFE_TBL_Global.ValidationResults[RegRecPtr->ValidateActiveIndex].Result = Status;

            /* Once validation is complete, reset the flags */
            CFE_TBL_Global.ValidationResults[RegRecPtr->ValidateActiveIndex].State = CFE_TBL_VALIDATION_PERFORMED;
            RegRecPtr->ValidateActiveIndex                                         = CFE_TBL_NO_VALIDATION_PENDING;

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
    int32                       Status;
    CFE_ES_AppId_t              ThisAppId;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];
        RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

        /* Perform validations prior to performing any updates */
        if (RegRecPtr->LoadPending)
        {
            Status = CFE_TBL_INFO_UPDATE_PENDING;
        }
        else if ((RegRecPtr->ValidateActiveIndex != CFE_TBL_NO_VALIDATION_PENDING) ||
                 (RegRecPtr->ValidateInactiveIndex != CFE_TBL_NO_VALIDATION_PENDING))
        {
            Status = CFE_TBL_INFO_VALIDATION_PENDING;
        }
        else if (RegRecPtr->DumpControlIndex != CFE_TBL_NO_DUMP_PENDING)
        {
            Status = CFE_TBL_INFO_DUMP_PENDING;
        }
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
CFE_Status_t CFE_TBL_GetInfo(CFE_TBL_Info_t *TblInfoPtr, const char *TblName)
{
    int32                  Status = CFE_SUCCESS;
    int16                  RegIndx;
    int32                  NumAccessDescriptors = 0;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_Handle_t       HandleIterator;

    if (TblInfoPtr == NULL || TblName == NULL)
    {
        return CFE_TBL_BAD_ARGUMENT;
    }

    RegIndx = CFE_TBL_FindTableInRegistry(TblName);

    /* If we found the table, then extract the information from the Registry */
    if (RegIndx != CFE_TBL_NOT_FOUND)
    {
        /* Get pointer to Registry Record Entry to speed up processing */
        RegRecPtr = &CFE_TBL_Global.Registry[RegIndx];

        /* Return table characteristics */
        TblInfoPtr->Size            = RegRecPtr->Size;
        TblInfoPtr->DoubleBuffered  = RegRecPtr->DoubleBuffered;
        TblInfoPtr->DumpOnly        = RegRecPtr->DumpOnly;
        TblInfoPtr->UserDefAddr     = RegRecPtr->UserDefAddr;
        TblInfoPtr->TableLoadedOnce = RegRecPtr->TableLoadedOnce;

        /* Return information on last load and update */
        TblInfoPtr->TimeOfLastUpdate      = RegRecPtr->TimeOfLastUpdate;
        TblInfoPtr->FileCreateTimeSecs    = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSecs;
        TblInfoPtr->FileCreateTimeSubSecs = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileCreateTimeSubSecs;
        TblInfoPtr->Crc                   = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].Crc;
        strncpy(TblInfoPtr->LastFileLoaded, RegRecPtr->LastFileLoaded, sizeof(TblInfoPtr->LastFileLoaded) - 1);
        TblInfoPtr->LastFileLoaded[sizeof(TblInfoPtr->LastFileLoaded) - 1] = 0;

        /* Count the number of Access Descriptors to determine the number of users */
        HandleIterator = RegRecPtr->HeadOfAccessList;
        while (HandleIterator != CFE_TBL_END_OF_LIST)
        {
            NumAccessDescriptors++;
            HandleIterator = CFE_TBL_Global.Handles[HandleIterator].NextLink;
        }

        TblInfoPtr->NumUsers = NumAccessDescriptors;

        TblInfoPtr->Critical = RegRecPtr->CriticalTable;
    }
    else
    {
        Status = CFE_TBL_ERR_INVALID_NAME;
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
    int32                       Status;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    CFE_TBL_RegistryRec_t *     RegRecPtr     = NULL;
    CFE_TBL_DumpControl_t *     DumpCtrlPtr   = NULL;
    CFE_TIME_SysTime_t          DumpTime;

    /* Make sure the table has been requested to be dumped */
    Status = CFE_TBL_GetStatus(TblHandle);
    if (Status == CFE_TBL_INFO_DUMP_PENDING)
    {
        AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];
        RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];
        DumpCtrlPtr   = &CFE_TBL_Global.DumpControlBlocks[RegRecPtr->DumpControlIndex];

        /* Copy the contents of the active buffer to the assigned dump buffer */
        memcpy(DumpCtrlPtr->DumpBufferPtr->BufferPtr, RegRecPtr->Buffers[0].BufferPtr, DumpCtrlPtr->Size);

        /* Save the current time so that the header in the dump file can have the correct time */
        DumpTime                                          = CFE_TIME_GetTime();
        DumpCtrlPtr->DumpBufferPtr->FileCreateTimeSecs    = DumpTime.Seconds;
        DumpCtrlPtr->DumpBufferPtr->FileCreateTimeSubSecs = DumpTime.Subseconds;

        /* Disassociate the dump request from the table */
        RegRecPtr->DumpControlIndex = CFE_TBL_NO_DUMP_PENDING;

        /* Notify the Table Services Application that the dump buffer is ready to be written to a file */
        DumpCtrlPtr->State = CFE_TBL_DUMP_PERFORMED;

        Status = CFE_SUCCESS;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_Modified(CFE_TBL_Handle_t TblHandle)
{
    int32                       Status;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    CFE_TBL_RegistryRec_t *     RegRecPtr     = NULL;
    CFE_TBL_Handle_t            AccessIterator;
    CFE_ES_AppId_t              ThisAppId;
    size_t                      FilenameLen;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];
        RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

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

        AccessIterator = RegRecPtr->HeadOfAccessList;
        while (AccessIterator != CFE_TBL_END_OF_LIST)
        {
            /* Only notify *OTHER* applications that the contents have changed */
            if (!CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.Handles[AccessIterator].AppId, ThisAppId))
            {
                CFE_TBL_Global.Handles[AccessIterator].Updated = true;
            }

            AccessIterator = CFE_TBL_Global.Handles[AccessIterator].NextLink;
        }
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
    int32                       Status;
    CFE_TBL_AccessDescriptor_t *AccessDescPtr = NULL;
    CFE_TBL_RegistryRec_t *     RegRecPtr     = NULL;
    CFE_ES_AppId_t              ThisAppId;

    /* Verify that this application has the right to perform operation */
    Status = CFE_TBL_ValidateAccess(TblHandle, &ThisAppId);

    if (Status == CFE_SUCCESS)
    {
        /* Get pointers to pertinent records in registry and handles */
        AccessDescPtr = &CFE_TBL_Global.Handles[TblHandle];
        RegRecPtr     = &CFE_TBL_Global.Registry[AccessDescPtr->RegIndex];

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
