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
** File: cfe_tbl_task_cmds.c
**
** Subsystem: cFE TBL Task Command Processing Functions
**
** Author: David Kobe (the Hammers Company, Inc.)
**
** Notes:
**
*/

/*
** Required header files
*/
#include "cfe_tbl_module_all.h"
#include "cfe_version.h"
#include "cfe_config.h" /* For version string construction */

#include <string.h>

/*----------------------------------------------------------------
 *
 * Internal helper function
 * Not invoked outside of this unit
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TranslateCmdProcRet(CFE_TBL_CmdProcRet_t ReturnCode)
{
    CFE_Status_t Status;

    /*
     * In the context of command handlers, the status should indicate if there
     * is any notification or cleanup yet to perform.  If all related events have
     * been sent and all counters have been incremented, then the status code
     * should be CFE_SUCCESS because there is nothing more to do - even if the command
     * itself had failed.
     */
    Status = CFE_SUCCESS;

    if (ReturnCode == CFE_TBL_CmdProcRet_INC_CMD_CTR)
    {
        /* No errors detected and increment command counter */
        CFE_TBL_Global.CommandCounter++;
    }
    else if (ReturnCode == CFE_TBL_CmdProcRet_INC_ERR_CTR)
    {
        /* Error detected in (or while processing) message, increment command error counter */
        CFE_TBL_Global.CommandErrorCounter++;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_SendHkCmd(const CFE_TBL_SendHkCmd_t *data)
{
    int32                  Status;
    uint32                 i;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;

    /*
    ** Collect housekeeping data from Table Services
    */
    CFE_TBL_GetHkData();

    /*
    ** Send housekeeping telemetry packet
    */
    CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_TBL_Global.HkPacket.TelemetryHeader));
    Status = CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_TBL_Global.HkPacket.TelemetryHeader), true);

    if (Status != CFE_SUCCESS)
    {
        CFE_EVS_SendEvent(CFE_TBL_FAIL_HK_SEND_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to send Hk Packet (Status=0x%08X)", (unsigned int)Status);
    }

    /* If a table's registry entry has been requested for telemetry, then pack it and send it */
    if (CFE_TBL_REGID_IS_VALID(CFE_TBL_Global.HkTlmTblRegId))
    {
        CFE_TBL_GetTblRegData();

        /*
        ** Send Table Registry Info Packet
        */
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_TBL_Global.TblRegPacket.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_TBL_Global.TblRegPacket.TelemetryHeader), true);

        /* Once the data has been sent, clear the index so that we don't send it again and again */
        CFE_TBL_Global.HkTlmTblRegId = CFE_TBL_REGID_UNDEFINED;
    }

    /* Check to see if there are any dump-only table dumps pending */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[i];

        if (CFE_TBL_DumpCtrlBlockIsUsed(DumpCtrlPtr) && DumpCtrlPtr->State == CFE_TBL_DUMP_PERFORMED)
        {
            CFE_TBL_WriteSnapshotToFile(DumpCtrlPtr);

            /* Free the shared working buffer */
            CFE_TBL_LoadBuffSetFree(DumpCtrlPtr->DumpBufferPtr);

            /* Free the Dump Control Block for later use */
            CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);
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
void CFE_TBL_GetHkData(void)
{
    uint32 i;
    uint16 Count;

    CFE_TBL_ValidationResult_t *ValPtr;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_LoadBuff_t *        LoadBuffPtr;

    /* Copy command counter data */
    CFE_TBL_Global.HkPacket.Payload.CommandCounter      = CFE_TBL_Global.CommandCounter;
    CFE_TBL_Global.HkPacket.Payload.CommandErrorCounter = CFE_TBL_Global.CommandErrorCounter;
    CFE_TBL_Global.HkPacket.Payload.FailedValCounter    = CFE_TBL_Global.FailedValCounter;
    CFE_TBL_Global.HkPacket.Payload.NumLoadPending      = 0;
    CFE_TBL_Global.HkPacket.Payload.MemPoolHandle       = CFE_TBL_Global.Buf.PoolHdl;

    /* Determine the number of tables currently registered and Number of Load Pending Tables */
    Count = 0;
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[i];

        if (CFE_TBL_RegRecIsUsed(RegRecPtr))
        {
            ++Count;

            if (CFE_TBL_RegRecIsLoadInProgress(RegRecPtr))
            {
                CFE_TBL_Global.HkPacket.Payload.NumLoadPending++;
            }
        }
    }
    CFE_TBL_Global.HkPacket.Payload.NumTables = Count;

    /* Determine the number of free shared buffers */
    Count = 0;
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        LoadBuffPtr = &CFE_TBL_Global.LoadBuffs[i];

        if (CFE_TBL_LoadBuffIsUsed(LoadBuffPtr))
        {
            ++Count;
        }
    }
    CFE_TBL_Global.HkPacket.Payload.NumFreeSharedBufs = CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS - Count;

    /* Locate a completed, but unreported, validation request */
    i      = 0;
    ValPtr = NULL;
    while (true)
    {
        if (i >= CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS)
        {
            ValPtr = NULL;
            break;
        }

        ValPtr = &CFE_TBL_Global.ValidationResults[i];
        if (CFE_TBL_ValidationResultIsUsed(ValPtr) && ValPtr->State == CFE_TBL_VALIDATION_PERFORMED)
        {
            break;
        }

        ++i;
    }

    if (ValPtr != NULL)
    {
        CFE_TBL_Global.HkPacket.Payload.LastValCrc    = ValPtr->CrcOfTable;
        CFE_TBL_Global.HkPacket.Payload.LastValStatus = ValPtr->Result;
        CFE_TBL_Global.HkPacket.Payload.ActiveBuffer  = ValPtr->ActiveBuffer;

        /* Keep track of the number of failed and successful validations */
        if (ValPtr->Result == CFE_SUCCESS)
        {
            CFE_TBL_Global.SuccessValCounter++;
        }
        else
        {
            CFE_TBL_Global.FailedValCounter++;
        }

        CFE_SB_MessageStringSet(CFE_TBL_Global.HkPacket.Payload.LastValTableName, ValPtr->TableName,
                                sizeof(CFE_TBL_Global.HkPacket.Payload.LastValTableName), sizeof(ValPtr->TableName));
        CFE_TBL_Global.ValidationCounter++;

        /* Free the Validation Response Block for next time */
        ValPtr->Result       = 0;
        ValPtr->CrcOfTable   = 0;
        ValPtr->TableName[0] = '\0';
        ValPtr->ActiveBuffer = false;

        CFE_TBL_ValidationResultSetFree(ValPtr);
    }

    CFE_TBL_Global.HkPacket.Payload.ValidationCounter = CFE_TBL_Global.ValidationCounter;
    CFE_TBL_Global.HkPacket.Payload.SuccessValCounter = CFE_TBL_Global.SuccessValCounter;
    CFE_TBL_Global.HkPacket.Payload.FailedValCounter  = CFE_TBL_Global.FailedValCounter;
    CFE_TBL_Global.HkPacket.Payload.NumValRequests    = CFE_TBL_Global.NumValRequests;

    RegRecPtr = CFE_TBL_LocateRegRecByID(CFE_TBL_Global.LastTblUpdated);

    /* Check to make sure the Registry Entry is still valid */
    if (CFE_TBL_RegRecIsMatch(RegRecPtr, CFE_TBL_Global.LastTblUpdated))
    {
        /* Get the time at the last table update */
        CFE_TBL_Global.HkPacket.Payload.LastUpdateTime = CFE_TBL_RegRecGetLastUpdateTime(RegRecPtr);

        /* Get the table name used for the last table update */
        CFE_SB_MessageStringSet(CFE_TBL_Global.HkPacket.Payload.LastUpdatedTable, CFE_TBL_RegRecGetName(RegRecPtr),
                                sizeof(CFE_TBL_Global.HkPacket.Payload.LastUpdatedTable), -1);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_GetTblRegData(void)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_LoadBuff_t *   BuffPtr;

    CFE_TBL_LockRegistry();

    RegRecPtr = CFE_TBL_LocateRegRecByID(CFE_TBL_Global.HkTlmTblRegId);
    if (CFE_TBL_RegRecIsMatch(RegRecPtr, CFE_TBL_Global.HkTlmTblRegId))
    {
        CFE_TBL_Global.TblRegPacket.Payload.Size = CFE_ES_MEMOFFSET_C(CFE_TBL_RegRecGetSize(RegRecPtr));

        BuffPtr = CFE_TBL_GetActiveBuffer(RegRecPtr);

        /* The active buffer may be NULL if the table has never been loaded */
        if (BuffPtr != NULL)
        {
            CFE_TBL_Global.TblRegPacket.Payload.ActiveBufferAddr =
                CFE_ES_MEMADDRESS_C(CFE_TBL_LoadBuffGetReadPointer(BuffPtr));
            CFE_TBL_Global.TblRegPacket.Payload.FileTime = BuffPtr->FileTime;
            CFE_TBL_Global.TblRegPacket.Payload.Crc      = BuffPtr->Crc;
        }
        else
        {
            CFE_TBL_Global.TblRegPacket.Payload.ActiveBufferAddr = CFE_ES_MEMADDRESS_C(0);
        }

        BuffPtr = CFE_TBL_GetInactiveBuffer(RegRecPtr);

        /* the inactive buffer is typically NULL in a single-buffer table */
        if (BuffPtr != NULL)
        {
            CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr =
                CFE_ES_MEMADDRESS_C(CFE_TBL_LoadBuffGetReadPointer(BuffPtr));
        }
        else
        {
            CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr = CFE_ES_MEMADDRESS_C(0);
        }

        CFE_TBL_Global.TblRegPacket.Payload.ValidationFuncPtr =
            CFE_ES_MEMADDRESS_C(CFE_TBL_RegRecGetValidationFunc(RegRecPtr));
        CFE_TBL_Global.TblRegPacket.Payload.TimeOfLastUpdate = CFE_TBL_RegRecGetLastUpdateTime(RegRecPtr);
        CFE_TBL_Global.TblRegPacket.Payload.TableLoadedOnce  = CFE_TBL_RegRecIsTableLoaded(RegRecPtr);
        CFE_TBL_Global.TblRegPacket.Payload.LoadPending      = CFE_TBL_RegRecIsPendingActivation(RegRecPtr);
        CFE_TBL_Global.TblRegPacket.Payload.DumpOnly         = CFE_TBL_RegRecGetConfig(RegRecPtr)->DumpOnly;
        CFE_TBL_Global.TblRegPacket.Payload.DoubleBuffered   = CFE_TBL_RegRecGetConfig(RegRecPtr)->DoubleBuffered;
        CFE_TBL_Global.TblRegPacket.Payload.Critical         = CFE_TBL_RegRecGetConfig(RegRecPtr)->Critical;

        CFE_SB_MessageStringSet(CFE_TBL_Global.TblRegPacket.Payload.Name, CFE_TBL_RegRecGetName(RegRecPtr),
                                sizeof(CFE_TBL_Global.TblRegPacket.Payload.Name), -1);
        CFE_SB_MessageStringSet(CFE_TBL_Global.TblRegPacket.Payload.LastFileLoaded,
                                CFE_TBL_RegRecGetLastFileLoaded(RegRecPtr),
                                sizeof(CFE_TBL_Global.TblRegPacket.Payload.LastFileLoaded), -1);
        CFE_ES_GetAppName(CFE_TBL_Global.TblRegPacket.Payload.OwnerAppName, RegRecPtr->OwnerAppId,
                          sizeof(CFE_TBL_Global.TblRegPacket.Payload.OwnerAppName));
    }

    CFE_TBL_UnlockRegistry();
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_NoopCmd(const CFE_TBL_NoopCmd_t *data)
{
    CFE_TBL_CmdProcRet_t CmdProcRet;

    /* No-op commands always succeed */
    CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;

    char VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /* Acknowledge receipt of NOOP with Event Message */
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "CFE_TBL", CFE_SRC_VERSION,
                                CFE_BUILD_CODENAME, CFE_LAST_OFFICIAL);
    CFE_EVS_SendEvent(CFE_TBL_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op Cmd Rcvd: %s", VersionString);

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ResetCountersCmd(const CFE_TBL_ResetCountersCmd_t *data)
{
    CFE_TBL_CmdProcRet_t CmdProcRet;

    /* Reset counter commands never increment the counter */
    CmdProcRet = CFE_TBL_CmdProcRet_DONT_INC_CTR;

    CFE_TBL_Global.CommandCounter      = 0;
    CFE_TBL_Global.CommandErrorCounter = 0;
    CFE_TBL_Global.SuccessValCounter   = 0;
    CFE_TBL_Global.FailedValCounter    = 0;
    CFE_TBL_Global.NumValRequests      = 0;
    CFE_TBL_Global.ValidationCounter   = 0;

    CFE_EVS_SendEvent(CFE_TBL_RESET_INF_EID, CFE_EVS_EventType_DEBUG, "Reset Counters command");

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_LoadCmd(const CFE_TBL_LoadCmd_t *data)
{
    CFE_TBL_CmdProcRet_t             CmdProcRet;
    const CFE_TBL_LoadCmd_Payload_t *CmdPtr;
    CFE_TBL_CombinedFileHdr_t        Header;
    osal_id_t                        FileDescriptor;
    CFE_Status_t                     Status;
    char                             LoadFilename[OS_MAX_PATH_LEN];
    CFE_TBL_TxnState_t               Txn;

    /* just in case, clear out the header */
    memset(&Header, 0, sizeof(Header));

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */
    CmdPtr     = &data->Payload;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(LoadFilename, (char *)CmdPtr->LoadFilename, NULL, sizeof(LoadFilename),
                            sizeof(CmdPtr->LoadFilename));

    CFE_TBL_TxnInit(&Txn, true);

    /* Try to open the specified table file */
    Status = CFE_TBL_TxnOpenTableLoadFile(&Txn, LoadFilename, &FileDescriptor, &Header);
    if (Status == CFE_SUCCESS)
    {
        /* Locate specified table in registry (wrapped in a lock) */
        /* NOTE: The header reading code ensures null term on the table name string, so its OK to pass direct */
        CFE_TBL_TxnLockRegistry(&Txn);
        CFE_TBL_TxnFindRegByName(&Txn, Header.Tbl.TableName);
        CFE_TBL_TxnUnlockRegistry(&Txn);

        Status = CFE_TBL_ValidateFileIsLoadable(&Txn, &Header.Tbl);
    }

    if (Status == CFE_SUCCESS)
    {
        /* Read the file content into the working buffer */
        Status = CFE_TBL_LoadContentFromFile(&Txn, FileDescriptor, Header.Tbl.Offset, Header.Tbl.NumBytes);
    }

    /* If all the above worked out, then set the meta info in the load buffer */
    if (Status == CFE_SUCCESS)
    {
        CFE_TBL_SetMetaDataFromFileHeader(&Txn, LoadFilename, &Header.Std);
    }

    CFE_TBL_TxnFinish(&Txn);

    if (Status == CFE_SUCCESS)
    {
        /* Save file information statistics for housekeeping telemetry */
        CFE_SB_MessageStringSet(CFE_TBL_Global.HkPacket.Payload.LastFileLoaded, LoadFilename,
                                sizeof(CFE_TBL_Global.HkPacket.Payload.LastFileLoaded), sizeof(LoadFilename));
        CFE_SB_MessageStringSet(CFE_TBL_Global.HkPacket.Payload.LastTableLoaded, Header.Tbl.TableName,
                                sizeof(CFE_TBL_Global.HkPacket.Payload.LastTableLoaded), sizeof(Header.Tbl.TableName));

        /* Increment successful command completion counter */
        CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;

        /* Report the successful event */
        CFE_TBL_TxnAddEvent(&Txn, CFE_TBL_FILE_LOADED_INF_EID, 0, 0);
    }

    /* Send any events associated with this table load */
    CFE_TBL_SendTableLoadFileEvents(&Txn, LoadFilename, &Header);

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DumpCmd(const CFE_TBL_DumpCmd_t *data)
{
    CFE_TBL_CmdProcRet_t             CmdProcRet;
    CFE_TBL_TxnState_t               Txn;
    const CFE_TBL_DumpCmd_Payload_t *CmdPtr = &data->Payload;
    char                             DumpFilename[OS_MAX_PATH_LEN];
    char                             TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_Status_t                     Status;
    CFE_TBL_DumpCtrlId_t             DumpCtrlId;

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(DumpFilename, (char *)CmdPtr->DumpFilename, NULL, sizeof(DumpFilename),
                            sizeof(CmdPtr->DumpFilename));

    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    /* Note - if successful, this returns with the registry in a locked state */
    Status = CFE_TBL_TxnStartFromName(&Txn, TableName, CFE_TBL_TxnContext_UNDEFINED);

    if (Status != CFE_SUCCESS)
    {
        /* Table could not be found in Registry */
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }
    else
    {
        /* Reserve a dump control block and populate it with the details */
        Status = CFE_TBL_AllocateDumpCtrlBlock(&Txn, &DumpCtrlId, CmdPtr->ActiveTableFlag);

        /* Done modifying registry for now */
        CFE_TBL_TxnUnlockRegistry(&Txn);

        if (Status == CFE_SUCCESS)
        {
            /* Complete the process */
            Status = CFE_TBL_PrepareDumpSnapshotBuffer(&Txn, DumpCtrlId, DumpFilename);
        }

        CFE_TBL_TxnFinish(&Txn);
    }

    /* Translate return code */
    if (Status == CFE_SUCCESS)
    {
        CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;
    }

    /* Send any events associated with this table dump */
    CFE_TBL_SendTableDumpEvents(&Txn, TableName);

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateCmd(const CFE_TBL_ValidateCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                 CmdProcRet;
    CFE_TBL_TxnState_t                   Txn;
    CFE_Status_t                         Status;
    const CFE_TBL_ValidateCmd_Payload_t *CmdPtr = &data->Payload;
    CFE_TBL_RegistryRec_t *              RegRecPtr;
    CFE_TBL_LoadBuff_t *                 SelectedBufferPtr;
    char                                 TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    uint32                               CrcOfTable;
    CFE_ResourceId_t                     PendingValId;
    CFE_TBL_ValidationResult_t *         ValResultPtr;

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */

    SelectedBufferPtr = NULL;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    Status = CFE_TBL_TxnStartFromName(&Txn, TableName, CFE_TBL_TxnContext_UNDEFINED);
    if (Status == CFE_SUCCESS)
    {
        /* Obtain a pointer to registry information about specified table */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);
        CFE_TBL_TxnFinish(&Txn);

        /* Determine what data is to be validated */
        SelectedBufferPtr = CFE_TBL_GetSelectedBuffer(RegRecPtr, CmdPtr->ActiveTableFlag);

        if (SelectedBufferPtr == NULL)
        {
            CFE_EVS_SendEvent(CFE_TBL_NO_INACTIVE_BUFFER_ERR_EID, CFE_EVS_EventType_ERROR,
                              "No Buffer for Table '%s' present", TableName);
        }
        else
        {
            /* If we have located the data to be validated, then proceed with notifying the application, if */
            /* necessary, and computing the CRC value for the block of memory                               */

            /* Find a free Validation Response Block */
            PendingValId = CFE_TBL_GetNextValResultBlock();
            ValResultPtr = CFE_TBL_LocateValidationResultByID(CFE_TBL_VALRESULTID_C(PendingValId));
            if (ValResultPtr != NULL)
            {
                /* Allocate this Validation Response Block */
                ValResultPtr->State  = CFE_TBL_VALIDATION_PENDING;
                ValResultPtr->Result = 0;
                memcpy(ValResultPtr->TableName, TableName, CFE_TBL_MAX_FULL_NAME_LEN);

                /* Compute the CRC on the specified table buffer */
                CrcOfTable = CFE_ES_CalculateCRC(CFE_TBL_LoadBuffGetReadPointer(SelectedBufferPtr),
                                                 CFE_TBL_LoadBuffGetContentSize(SelectedBufferPtr), 0,
                                                 CFE_MISSION_ES_DEFAULT_CRC);

                ValResultPtr->CrcOfTable   = CrcOfTable;
                ValResultPtr->ActiveBuffer = (CmdPtr->ActiveTableFlag != 0);

                CFE_TBL_ValidationResultSetUsed(ValResultPtr, PendingValId);

                CFE_TBL_Global.LastValidationResultId = PendingValId;

                /* If owner has a validation function, then notify the  */
                /* table owner that there is data to be validated       */
                if (CFE_TBL_RegRecGetValidationFunc(RegRecPtr) != NULL)
                {
                    if (CmdPtr->ActiveTableFlag)
                    {
                        RegRecPtr->ValidateActiveId = CFE_TBL_ValidationResultGetId(ValResultPtr);
                    }
                    else
                    {
                        RegRecPtr->ValidateInactiveId = CFE_TBL_ValidationResultGetId(ValResultPtr);
                    }

                    /* If application requested notification by message, then do so */
                    if (CFE_TBL_SendNotificationMsg(RegRecPtr) == CFE_SUCCESS)
                    {
                        /* Notify ground that validation request has been made */
                        CFE_EVS_SendEvent(CFE_TBL_VAL_REQ_MADE_INF_EID, CFE_EVS_EventType_DEBUG,
                                          "Tbl Services issued validation request for '%s'", TableName);
                    }

                    /* Maintain statistic on number of validation requests given to applications */
                    CFE_TBL_Global.NumValRequests++;
                }
                else
                {
                    /* If there isn't a validation function pointer, then the process is complete  */
                    /* By setting this value, we are letting the Housekeeping process recognize it */
                    /* as data to be sent to the ground in telemetry.                              */
                    ValResultPtr->State = CFE_TBL_VALIDATION_PERFORMED;

                    CFE_EVS_SendEvent(CFE_TBL_ASSUMED_VALID_INF_EID, CFE_EVS_EventType_INFORMATION,
                                      "Tbl Services assumes '%s' is valid. No Validation Function has been registered",
                                      TableName);
                }

                /* Increment Successful Command Counter */
                CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_TOO_MANY_VALIDATIONS_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Too many Table Validations have been requested");
            }
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ActivateCmd(const CFE_TBL_ActivateCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                 CmdProcRet;
    const CFE_TBL_ActivateCmd_Payload_t *CmdPtr = &data->Payload;
    char                                 TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_RegistryRec_t *              RegRecPtr;
    CFE_TBL_LoadBuff_t *                 BufferPtr;
    CFE_TBL_TxnState_t                   Txn;
    int32                                Status;

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    BufferPtr = NULL;
    RegRecPtr = NULL;

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    Status = CFE_TBL_TxnStartFromName(&Txn, TableName, CFE_TBL_TxnContext_UNDEFINED);
    if (Status == CFE_SUCCESS)
    {
        /* Obtain a pointer to registry information about specified table */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);
        CFE_TBL_TxnFinish(&Txn);

        if (CFE_TBL_RegRecGetConfig(RegRecPtr)->DumpOnly)
        {
            CFE_EVS_SendEvent(CFE_TBL_ACTIVATE_DUMP_ONLY_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Illegal attempt to activate dump-only table '%s'", TableName);
        }
        else
        {
            /* This only ever applies to the load in progress, one does not activate the previous buffer */
            BufferPtr = CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);
            if (BufferPtr == NULL)
            {
                CFE_EVS_SendEvent(CFE_TBL_ACTIVATE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Cannot activate table '%s'. No Inactive image available", TableName);
            }
            else if (!BufferPtr->Validated)
            {
                CFE_EVS_SendEvent(CFE_TBL_UNVALIDATED_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Cannot activate table '%s'. Inactive image not Validated", TableName);
            }
            else
            {
                /* If application requested notification by message, then do so */
                if (CFE_TBL_SendNotificationMsg(RegRecPtr) == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CFE_TBL_LOAD_PEND_REQ_INF_EID, CFE_EVS_EventType_DEBUG,
                                      "Tbl Services notifying App that '%s' has a load pending", TableName);
                }

                /* Increment Successful Command Counter */
                CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;
            }
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_DumpRegistryGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    CFE_TBL_RegDumpStateInfo_t *StatePtr = (CFE_TBL_RegDumpStateInfo_t *)Meta;
    CFE_TBL_RegistryRec_t *     RegRecPtr;
    CFE_TBL_LoadBuff_t *        BufferPtr;
    CFE_ES_AppId_t              OwnerAppId;
    uint32                      NumUsers;
    bool                        IsValidEntry;

    IsValidEntry = false;
    OwnerAppId   = CFE_ES_APPID_UNDEFINED;

    if (RecordNum < CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        /* Make a pointer to simplify code look and to remove redundant indexing into registry */
        RegRecPtr = &CFE_TBL_Global.Registry[RecordNum];

        /* should lock registry while copying out data to ensure its in consistent state */
        CFE_TBL_LockRegistry();

        /* Check to see if the Registry entry is empty */
        if (CFE_TBL_RegRecIsUsed(RegRecPtr))
        {
            IsValidEntry = true;
            OwnerAppId   = RegRecPtr->OwnerAppId;

            /* Fill Registry Dump Record with relevant information */
            StatePtr->DumpRecord.Size             = CFE_ES_MEMOFFSET_C(CFE_TBL_RegRecGetSize(RegRecPtr));
            StatePtr->DumpRecord.TimeOfLastUpdate = CFE_TBL_RegRecGetLastUpdateTime(RegRecPtr);
            StatePtr->DumpRecord.ValidationFunc   = (CFE_TBL_RegRecGetValidationFunc(RegRecPtr) != NULL);
            StatePtr->DumpRecord.TableLoadedOnce  = CFE_TBL_RegRecIsTableLoaded(RegRecPtr);
            StatePtr->DumpRecord.LoadPending      = CFE_TBL_RegRecIsPendingActivation(RegRecPtr);
            StatePtr->DumpRecord.DumpOnly         = CFE_TBL_RegRecGetConfig(RegRecPtr)->DumpOnly;
            StatePtr->DumpRecord.DoubleBuffered   = CFE_TBL_RegRecGetConfig(RegRecPtr)->DoubleBuffered;
            StatePtr->DumpRecord.CriticalTable    = CFE_TBL_RegRecGetConfig(RegRecPtr)->Critical;

            /* Convert LoadInProgress ID into historically-compatible integer value */
            StatePtr->DumpRecord.LoadInProgress =
                CFE_TBL_LoadBuffIdToTlmIndex(CFE_TBL_RegRecGetLoadInProgress(RegRecPtr));

            /* Note that the active buffer may be NULL if the table was never loaded */
            BufferPtr = CFE_TBL_GetActiveBuffer(RegRecPtr);
            if (BufferPtr == NULL)
            {
                StatePtr->DumpRecord.FileTime = CFE_TIME_ZERO_VALUE;
                StatePtr->DumpRecord.Crc      = 0;
            }
            else
            {
                StatePtr->DumpRecord.FileTime = BufferPtr->FileTime;
                StatePtr->DumpRecord.Crc      = BufferPtr->Crc;
            }

            CFE_SB_MessageStringSet(StatePtr->DumpRecord.Name, CFE_TBL_RegRecGetName(RegRecPtr),
                                    sizeof(StatePtr->DumpRecord.Name), -1);
            CFE_SB_MessageStringSet(StatePtr->DumpRecord.LastFileLoaded, CFE_TBL_RegRecGetLastFileLoaded(RegRecPtr),
                                    sizeof(StatePtr->DumpRecord.LastFileLoaded), -1);

            /* Walk the access descriptor list to determine the number of users */

            NumUsers = 0;
            CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_CountAccessDescHelper, &NumUsers);
            StatePtr->DumpRecord.NumUsers = NumUsers;
        }

        /* Unlock now - remainder of data gathering uses ES */
        CFE_TBL_UnlockRegistry();
    }

    /*
     * If table record had data, then export now.
     * Need to also get the App name from ES to complete the record.
     */
    if (IsValidEntry)
    {
        /* Determine the name of the owning application */
        if (!CFE_RESOURCEID_TEST_EQUAL(OwnerAppId, CFE_TBL_NOT_OWNED))
        {
            CFE_ES_GetAppName(StatePtr->DumpRecord.OwnerAppName, OwnerAppId, sizeof(StatePtr->DumpRecord.OwnerAppName));
        }
        else
        {
            strncpy(StatePtr->DumpRecord.OwnerAppName, "--UNOWNED--", sizeof(StatePtr->DumpRecord.OwnerAppName) - 1);
            StatePtr->DumpRecord.OwnerAppName[sizeof(StatePtr->DumpRecord.OwnerAppName) - 1] = 0;
        }

        /* export data to caller */
        *Buffer  = &StatePtr->DumpRecord;
        *BufSize = sizeof(StatePtr->DumpRecord);
    }
    else
    {
        /* No data to write for this record */
        *BufSize = 0;
        *Buffer  = NULL;
    }

    /* Check for EOF (last entry) */
    return (RecordNum >= (CFE_PLATFORM_TBL_MAX_NUM_TABLES - 1));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_DumpRegistryEventHandler(void *Meta, CFE_FS_FileWriteEvent_t Event, int32 Status, uint32 RecordNum,
                                      size_t BlockSize, size_t Position)
{
    CFE_TBL_RegDumpStateInfo_t *StatePtr = (CFE_TBL_RegDumpStateInfo_t *)Meta;

    /*
     * Note that this runs in the context of ES background task (file writer background job)
     * It does NOT run in the context of the CFE_TBL app task.
     *
     * Events should use CFE_EVS_SendEventWithAppID() rather than CFE_EVS_SendEvent()
     * to get proper association with TBL task.
     */
    switch (Event)
    {
        case CFE_FS_FileWriteEvent_COMPLETE:
            if (StatePtr->FileExisted)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_OVERWRITE_REG_DUMP_INF_EID, CFE_EVS_EventType_DEBUG,
                                           CFE_TBL_Global.TableTaskAppId,
                                           "Successfully overwrote '%s' with Table Registry:Size=%d,Entries=%d",
                                           StatePtr->FileWrite.FileName, (int)Position, (int)RecordNum);
            }
            else
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_WRITE_REG_DUMP_INF_EID, CFE_EVS_EventType_DEBUG,
                                           CFE_TBL_Global.TableTaskAppId,
                                           "Successfully dumped Table Registry to '%s':Size=%d,Entries=%d",
                                           StatePtr->FileWrite.FileName, (int)Position, (int)RecordNum);
            }
            break;

        case CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR:
            CFE_EVS_SendEventWithAppID(CFE_TBL_WRITE_TBL_REG_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId, "Error writing Registry to '%s', Status=0x%08X",
                                       StatePtr->FileWrite.FileName, (unsigned int)Status);
            break;

        case CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR:
            CFE_EVS_SendEventWithAppID(CFE_TBL_WRITE_CFE_HDR_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId,
                                       "Error writing cFE File Header to '%s', Status=0x%08X",
                                       StatePtr->FileWrite.FileName, (unsigned int)Status);
            break;

        case CFE_FS_FileWriteEvent_CREATE_ERROR:
            CFE_EVS_SendEventWithAppID(CFE_TBL_CREATING_DUMP_FILE_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId, "Error creating dump file '%s', Status=0x%08X",
                                       StatePtr->FileWrite.FileName, (unsigned int)Status);
            break;

        default:
            /* unhandled event - ignore */
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_DumpRegistryCmd(const CFE_TBL_DumpRegistryCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                     CmdProcRet;
    int32                                    Status;
    const CFE_TBL_DumpRegistryCmd_Payload_t *CmdPtr = &data->Payload;
    os_fstat_t                               FileStat;

    CFE_TBL_RegDumpStateInfo_t *StatePtr;

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */

    StatePtr = &CFE_TBL_Global.RegDumpState;

    /* If a reg dump was already pending, do not overwrite the current request */
    if (!CFE_FS_BackgroundFileDumpIsPending(&StatePtr->FileWrite))
    {
        /*
         * Fill out the remainder of meta data.
         * This data is currently the same for every request
         */
        StatePtr->FileWrite.FileSubType = CFE_FS_SubType_TBL_REG;
        snprintf(StatePtr->FileWrite.Description, sizeof(StatePtr->FileWrite.Description), "Table Registry");

        StatePtr->FileWrite.GetData = CFE_TBL_DumpRegistryGetter;
        StatePtr->FileWrite.OnEvent = CFE_TBL_DumpRegistryEventHandler;

        /*
        ** Copy the filename into local buffer with default name/path/extension if not specified
        */
        Status = CFE_FS_ParseInputFileNameEx(StatePtr->FileWrite.FileName, CmdPtr->DumpFilename,
                                             sizeof(StatePtr->FileWrite.FileName), sizeof(CmdPtr->DumpFilename),
                                             CFE_PLATFORM_TBL_DEFAULT_REG_DUMP_FILE,
                                             CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                             CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

        if (Status == CFE_SUCCESS)
        {
            /*
             * Before submitting the background request, use OS_stat() to check if the file exists already.
             *
             * This is needed because TBL services issues a different event ID in some cases if
             * it is overwriting a file vs. creating a new file.
             */
            StatePtr->FileExisted = (OS_stat(StatePtr->FileWrite.FileName, &FileStat) == OS_SUCCESS);

            Status = CFE_FS_BackgroundFileDumpRequest(&StatePtr->FileWrite);
            if (Status == CFE_SUCCESS)
            {
                /* Increment the TBL generic command counter (successfully queued for background job) */
                CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;
            }
        }
    }

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_SendRegistryCmd(const CFE_TBL_SendRegistryCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                     CmdProcRet;
    const CFE_TBL_SendRegistryCmd_Payload_t *CmdPtr = &data->Payload;
    char                                     TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_TxnState_t                       Txn;
    CFE_Status_t                             Status;

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table registry entry that is to be telemetered exists */
    Status = CFE_TBL_TxnStartFromName(&Txn, TableName, CFE_TBL_TxnContext_UNDEFINED);
    if (Status == CFE_SUCCESS)
    {
        /* Change the index used to identify what data is to be telemetered */
        CFE_TBL_Global.HkTlmTblRegId = CFE_TBL_TxnRegId(&Txn);
        CFE_TBL_TxnFinish(&Txn);

        CFE_EVS_SendEvent(CFE_TBL_TLM_REG_CMD_INF_EID, CFE_EVS_EventType_DEBUG,
                          "Table Registry entry for '%s' will be telemetered", TableName);

        /* Increment Successful Command Counter */
        CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_DeleteCDSCmd(const CFE_TBL_DeleteCDSCmd_t *data)
{
    CFE_TBL_CmdProcRet_t               CmdProcRet;
    const CFE_TBL_DelCDSCmd_Payload_t *CmdPtr = &data->Payload;
    char                               TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_CritRegRec_t *             CritRegRecPtr;
    CFE_TBL_CritRegRec_t *             LocalPtr;
    uint32                             i;
    CFE_TBL_RegId_t                    RegIndex;
    int32                              Status;

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table is no longer in the registry */
    /* This would imply that the owning application has been terminated and that it */
    /* is safe to delete the associated critical table image in the CDS. */
    RegIndex      = CFE_TBL_FindTableInRegistry(TableName);
    CritRegRecPtr = NULL;

    if (!CFE_TBL_REGID_IS_VALID(RegIndex))
    {
        /* Find table in the Critical Table Registry */
        for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
        {
            LocalPtr = &CFE_TBL_Global.CritReg[i];

            if (strncmp(LocalPtr->Name, TableName, CFE_TBL_MAX_FULL_NAME_LEN) == 0)
            {
                CritRegRecPtr = LocalPtr;
                break;
            }
        }

        if (CritRegRecPtr != NULL)
        {
            Status = CFE_ES_DeleteCDS(TableName, true);

            if (Status == CFE_ES_CDS_WRONG_TYPE_ERR)
            {
                CFE_EVS_SendEvent(CFE_TBL_NOT_CRITICAL_TBL_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Table '%s' is in Critical Table Registry but CDS is not tagged as a table",
                                  TableName);
            }
            else if (Status == CFE_ES_CDS_OWNER_ACTIVE_ERR)
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "CDS '%s' owning app is still active", TableName);
            }
            else if (Status == CFE_ES_ERR_NAME_NOT_FOUND)
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_NOT_FOUND_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Unable to locate '%s' in CDS Registry", TableName);
            }
            else if (Status != CFE_SUCCESS)
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_DELETE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Error while deleting '%s' from CDS, See SysLog.(Err=0x%08X)", TableName,
                                  (unsigned int)Status);
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_CDS_DELETED_INFO_EID, CFE_EVS_EventType_INFORMATION,
                                  "Successfully removed '%s' from CDS", TableName);

                /* Free the entry in the Critical Table Registry */
                CritRegRecPtr->CDSHandle = CFE_ES_CDS_BAD_HANDLE;

                /* Increment Successful Command Counter */
                CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_NOT_IN_CRIT_REG_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Table '%s' is not found in Critical Table Registry", TableName);
        }
    }
    else /* Table was found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_IN_REGISTRY_ERR_EID, CFE_EVS_EventType_ERROR,
                          "'%s' found in Table Registry. CDS cannot be deleted until table is unregistered", TableName);
    }
    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_AbortLoadCmd(const CFE_TBL_AbortLoadCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                  CmdProcRet;
    CFE_TBL_RegId_t                       RegIndex;
    const CFE_TBL_AbortLoadCmd_Payload_t *CmdPtr = &data->Payload;
    CFE_TBL_RegistryRec_t *               RegRecPtr;
    char                                  TableName[CFE_TBL_MAX_FULL_NAME_LEN];

    CmdProcRet = CFE_TBL_CmdProcRet_INC_ERR_CTR; /* Assume failure */

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table registry entry that is to be telemetered exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (CFE_TBL_REGID_IS_VALID(RegIndex))
    {
        /* Make a pointer to simplify code look and to remove redundant indexing into registry */
        RegRecPtr = CFE_TBL_LocateRegRecByID(RegIndex);

        /* Check to make sure a load was in progress before trying to abort it */
        /* NOTE: NextBufferId contains index of buffer when dumping a dump-only table */
        /* so we must ensure the table is not a dump-only table, otherwise, we would be aborting a dump */
        if (CFE_TBL_RegRecIsLoadInProgress(RegRecPtr) && !CFE_TBL_RegRecGetConfig(RegRecPtr)->DumpOnly)
        {
            CFE_TBL_AbortLoad(RegRecPtr);

            /* Increment Successful Command Counter */
            CmdProcRet = CFE_TBL_CmdProcRet_INC_CMD_CTR;
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_LOAD_ABORT_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Cannot abort load of '%s'. No load started.", TableName);
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }

    return CFE_TBL_TranslateCmdProcRet(CmdProcRet);
}
