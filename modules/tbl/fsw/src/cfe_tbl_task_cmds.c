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
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_SendHkCmd(const CFE_TBL_SendHkCmd_t *data)
{
    int32                  Status;
    int32                  OsStatus;
    uint32                 i;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;
    CFE_TIME_SysTime_t     DumpTime;
    osal_id_t              FileDescriptor;

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
    if (CFE_TBL_Global.HkTlmTblRegIndex != CFE_TBL_NOT_FOUND)
    {
        CFE_TBL_GetTblRegData();

        /*
        ** Send Table Registry Info Packet
        */
        CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_TBL_Global.TblRegPacket.TelemetryHeader));
        CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_TBL_Global.TblRegPacket.TelemetryHeader), true);

        /* Once the data has been sent, clear the index so that we don't send it again and again */
        CFE_TBL_Global.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND;
    }

    /* Check to see if there are any dump-only table dumps pending */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[i];

        if (CFE_TBL_DumpCtrlBlockIsUsed(DumpCtrlPtr) && DumpCtrlPtr->State == CFE_TBL_DUMP_PERFORMED)
        {
            Status = CFE_TBL_DumpToFile(DumpCtrlPtr->DumpBufferPtr->DataSource, DumpCtrlPtr->TableName,
                                        DumpCtrlPtr->DumpBufferPtr->BufferPtr, DumpCtrlPtr->Size);

            /* If dump file was successfully written, update the file header so that the timestamp */
            /* is the time of the actual capturing of the data, NOT the time when it was written to the file */
            if (Status == CFE_TBL_INC_CMD_CTR)
            {
                DumpTime = DumpCtrlPtr->DumpBufferPtr->FileTime;

                OsStatus = OS_OpenCreate(&FileDescriptor, DumpCtrlPtr->DumpBufferPtr->DataSource, OS_FILE_FLAG_NONE,
                                         OS_READ_WRITE);

                if (OsStatus == OS_SUCCESS)
                {
                    Status = CFE_FS_SetTimestamp(FileDescriptor, DumpTime);

                    if (Status != CFE_SUCCESS)
                    {
                        CFE_ES_WriteToSysLog("%s: Unable to update timestamp in dump file '%s'\n", __func__,
                                             DumpCtrlPtr->DumpBufferPtr->DataSource);
                    }

                    OS_close(FileDescriptor);
                }
                else
                {
                    CFE_ES_WriteToSysLog("%s: Unable to open dump file '%s' to update timestamp\n", __func__,
                                         DumpCtrlPtr->DumpBufferPtr->DataSource);
                }
            }

            /* Free the shared working buffer */
            CFE_TBL_Global.LoadBuffs[DumpCtrlPtr->RegRecPtr->LoadInProgress].Taken = false;
            DumpCtrlPtr->RegRecPtr->LoadInProgress                                 = CFE_TBL_NO_LOAD_IN_PROGRESS;

            /* Free the Dump Control Block for later use */
            CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);
        }
    }

    return CFE_TBL_DONT_INC_CTR;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_GetHkData(void)
{
    uint32                      i;
    uint16                      Count;
    CFE_TBL_ValidationResult_t *ValPtr = NULL;

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
        if (!CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.Registry[i].OwnerAppId, CFE_TBL_NOT_OWNED))
        {
            Count++;

            if (CFE_TBL_Global.Registry[i].LoadPending)
            {
                CFE_TBL_Global.HkPacket.Payload.NumLoadPending++;
            }
        }
    }
    CFE_TBL_Global.HkPacket.Payload.NumTables = Count;

    /* Determine the number of free shared buffers */
    CFE_TBL_Global.HkPacket.Payload.NumFreeSharedBufs = CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS;
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        if (CFE_TBL_Global.LoadBuffs[i].Taken)
        {
            CFE_TBL_Global.HkPacket.Payload.NumFreeSharedBufs--;
        }
    }

    /* Locate a completed, but unreported, validation request */
    i = 0;
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

    /* Validate the index of the last table updated before using it */
    if ((CFE_TBL_Global.LastTblUpdated >= 0) && (CFE_TBL_Global.LastTblUpdated < CFE_PLATFORM_TBL_MAX_NUM_TABLES))
    {
        /* Check to make sure the Registry Entry is still valid */
        if (!CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.Registry[CFE_TBL_Global.LastTblUpdated].OwnerAppId,
                                       CFE_TBL_NOT_OWNED))
        {
            /* Get the time at the last table update */
            CFE_TBL_Global.HkPacket.Payload.LastUpdateTime =
                CFE_TBL_Global.Registry[CFE_TBL_Global.LastTblUpdated].TimeOfLastUpdate;

            /* Get the table name used for the last table update */
            CFE_SB_MessageStringSet(CFE_TBL_Global.HkPacket.Payload.LastUpdatedTable,
                                    CFE_TBL_Global.Registry[CFE_TBL_Global.LastTblUpdated].Name,
                                    sizeof(CFE_TBL_Global.HkPacket.Payload.LastUpdatedTable),
                                    sizeof(CFE_TBL_Global.Registry[CFE_TBL_Global.LastTblUpdated].Name));
        }
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

    RegRecPtr = &CFE_TBL_Global.Registry[CFE_TBL_Global.HkTlmTblRegIndex];

    CFE_TBL_Global.TblRegPacket.Payload.Size = CFE_ES_MEMOFFSET_C(RegRecPtr->Size);
    CFE_TBL_Global.TblRegPacket.Payload.ActiveBufferAddr =
        CFE_ES_MEMADDRESS_C(RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr);

    if (RegRecPtr->DoubleBuffered)
    {
        /* For a double buffered table, the inactive is the other allocated buffer */
        CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr =
            CFE_ES_MEMADDRESS_C(RegRecPtr->Buffers[(1U - RegRecPtr->ActiveBufferIndex)].BufferPtr);
    }
    else
    {
        /* Check to see if an inactive buffer has currently been allocated to the single buffered table */
        if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
        {
            CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr =
                CFE_ES_MEMADDRESS_C(CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr);
        }
        else
        {
            CFE_TBL_Global.TblRegPacket.Payload.InactiveBufferAddr = CFE_ES_MEMADDRESS_C(0);
        }
    }

    CFE_TBL_Global.TblRegPacket.Payload.ValidationFuncPtr = CFE_ES_MEMADDRESS_C(RegRecPtr->ValidationFuncPtr);
    CFE_TBL_Global.TblRegPacket.Payload.TimeOfLastUpdate  = RegRecPtr->TimeOfLastUpdate;
    CFE_TBL_Global.TblRegPacket.Payload.TableLoadedOnce   = RegRecPtr->TableLoadedOnce;
    CFE_TBL_Global.TblRegPacket.Payload.LoadPending       = RegRecPtr->LoadPending;
    CFE_TBL_Global.TblRegPacket.Payload.DumpOnly          = RegRecPtr->DumpOnly;
    CFE_TBL_Global.TblRegPacket.Payload.DoubleBuffered    = RegRecPtr->DoubleBuffered;
    CFE_TBL_Global.TblRegPacket.Payload.FileTime          = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileTime;
    CFE_TBL_Global.TblRegPacket.Payload.Crc               = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].Crc;
    CFE_TBL_Global.TblRegPacket.Payload.Critical          = RegRecPtr->CriticalTable;

    CFE_SB_MessageStringSet(CFE_TBL_Global.TblRegPacket.Payload.Name, RegRecPtr->Name,
                            sizeof(CFE_TBL_Global.TblRegPacket.Payload.Name), sizeof(RegRecPtr->Name));
    CFE_SB_MessageStringSet(CFE_TBL_Global.TblRegPacket.Payload.LastFileLoaded, RegRecPtr->LastFileLoaded,
                            sizeof(CFE_TBL_Global.TblRegPacket.Payload.LastFileLoaded),
                            sizeof(RegRecPtr->LastFileLoaded));
    CFE_ES_GetAppName(CFE_TBL_Global.TblRegPacket.Payload.OwnerAppName, RegRecPtr->OwnerAppId,
                      sizeof(CFE_TBL_Global.TblRegPacket.Payload.OwnerAppName));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_NoopCmd(const CFE_TBL_NoopCmd_t *data)
{
    char VersionString[CFE_CFG_MAX_VERSION_STR_LEN];

    /* Acknowledge receipt of NOOP with Event Message */
    CFE_Config_GetVersionString(VersionString, CFE_CFG_MAX_VERSION_STR_LEN, "cFE", CFE_SRC_VERSION, CFE_BUILD_CODENAME,
                                CFE_LAST_OFFICIAL);
    CFE_EVS_SendEvent(CFE_TBL_NOOP_INF_EID, CFE_EVS_EventType_INFORMATION, "No-op Cmd Rcvd: %s", VersionString);

    return CFE_TBL_INC_CMD_CTR;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_ResetCountersCmd(const CFE_TBL_ResetCountersCmd_t *data)
{
    CFE_TBL_Global.CommandCounter      = 0;
    CFE_TBL_Global.CommandErrorCounter = 0;
    CFE_TBL_Global.SuccessValCounter   = 0;
    CFE_TBL_Global.FailedValCounter    = 0;
    CFE_TBL_Global.NumValRequests      = 0;
    CFE_TBL_Global.ValidationCounter   = 0;

    CFE_EVS_SendEvent(CFE_TBL_RESET_INF_EID, CFE_EVS_EventType_DEBUG, "Reset Counters command");

    return CFE_TBL_DONT_INC_CTR;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_LoadCmd(const CFE_TBL_LoadCmd_t *data)
{
    CFE_TBL_CmdProcRet_t             ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    const CFE_TBL_LoadCmd_Payload_t *CmdPtr     = &data->Payload;
    CFE_FS_Header_t                  StdFileHeader;
    CFE_TBL_File_Hdr_t               TblFileHeader;
    osal_id_t                        FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    int32                            Status;
    int32                            OsStatus;
    int16                            RegIndex;
    CFE_TBL_RegistryRec_t *          RegRecPtr;
    CFE_TBL_LoadBuff_t *             WorkingBufferPtr;
    char                             LoadFilename[OS_MAX_PATH_LEN];
    uint8                            ExtraByte;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(LoadFilename, (char *)CmdPtr->LoadFilename, NULL, sizeof(LoadFilename),
                            sizeof(CmdPtr->LoadFilename));

    /* Try to open the specified table file */
    OsStatus = OS_OpenCreate(&FileDescriptor, LoadFilename, OS_FILE_FLAG_NONE, OS_READ_ONLY);

    if (OsStatus == OS_SUCCESS)
    {
        Status = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, &LoadFilename[0]);

        if (Status == CFE_SUCCESS)
        {
            /* Locate specified table in registry */
            RegIndex = CFE_TBL_FindTableInRegistry(TblFileHeader.TableName);

            if (RegIndex == CFE_TBL_NOT_FOUND)
            {
                CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Unable to locate '%s' in Table Registry", TblFileHeader.TableName);
            }
            else
            {
                /* Translate the registry index into a record pointer */
                RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];

                if (RegRecPtr->DumpOnly)
                {
                    CFE_EVS_SendEvent(CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Attempted to load DUMP-ONLY table '%s' from '%s'", TblFileHeader.TableName,
                                      LoadFilename);
                }
                else if (RegRecPtr->LoadPending)
                {
                    CFE_EVS_SendEvent(CFE_TBL_LOADING_PENDING_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Attempted to load table '%s' while previous load is still pending",
                                      TblFileHeader.TableName);
                }
                else
                {
                    /* Make sure of the following:                                               */
                    /*    1) If table has not been loaded previously, then make sure the current */
                    /*       load starts with the first byte                                     */
                    /*    2) The number of bytes to load is greater than zero                    */
                    /*    3) The offset plus the number of bytes does not exceed the table size  */
                    if (((RegRecPtr->TableLoadedOnce) || (TblFileHeader.Offset == 0)) && (TblFileHeader.NumBytes > 0) &&
                        ((TblFileHeader.NumBytes + TblFileHeader.Offset) <= RegRecPtr->Size))
                    {
                        /* Get a working buffer, either a free one or one allocated with previous load command */
                        Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false);

                        if (Status == CFE_SUCCESS)
                        {
                            /* Copy data from file into working buffer */
                            OsStatus =
                                OS_read(FileDescriptor, ((uint8 *)WorkingBufferPtr->BufferPtr) + TblFileHeader.Offset,
                                        TblFileHeader.NumBytes);

                            /* Make sure the appropriate number of bytes were read */
                            if ((long)OsStatus == TblFileHeader.NumBytes)
                            {
                                /* Check to ensure the file does not have any extra data at the end */
                                OsStatus = OS_read(FileDescriptor, &ExtraByte, 1);

                                /* If another byte was successfully read, then file contains more data than header
                                 * claims */
                                if ((long)OsStatus == 1)
                                {
                                    CFE_EVS_SendEvent(CFE_TBL_FILE_TOO_BIG_ERR_EID, CFE_EVS_EventType_ERROR,
                                                      "File '%s' has more data than Tbl Hdr indicates (%d)",
                                                      LoadFilename, (int)TblFileHeader.NumBytes);
                                }
                                else /* If error reading file or zero bytes read, assume it was the perfect size */
                                {
                                    CFE_EVS_SendEvent(CFE_TBL_FILE_LOADED_INF_EID, CFE_EVS_EventType_INFORMATION,
                                                      "Successful load of '%s' into '%s' working buffer", LoadFilename,
                                                      TblFileHeader.TableName);

                                    /* Save file information statistics for later use in registry */
                                    memcpy(WorkingBufferPtr->DataSource, LoadFilename, OS_MAX_PATH_LEN);

                                    /* Save file creation time for later storage into Registry */
                                    WorkingBufferPtr->FileTime.Seconds    = StdFileHeader.TimeSeconds;
                                    WorkingBufferPtr->FileTime.Subseconds = StdFileHeader.TimeSubSeconds;

                                    /* Compute the CRC on the specified table buffer */
                                    WorkingBufferPtr->Crc = CFE_ES_CalculateCRC(
                                        WorkingBufferPtr->BufferPtr, RegRecPtr->Size, 0, CFE_MISSION_ES_DEFAULT_CRC);

                                    /* Initialize validation flag with true if no Validation Function is required to be
                                     * called */
                                    WorkingBufferPtr->Validated = (RegRecPtr->ValidationFuncPtr == NULL);

                                    /* Save file information statistics for housekeeping telemetry */
                                    strncpy(CFE_TBL_Global.HkPacket.Payload.LastFileLoaded, LoadFilename,
                                            sizeof(CFE_TBL_Global.HkPacket.Payload.LastFileLoaded) - 1);
                                    CFE_TBL_Global.HkPacket.Payload
                                        .LastFileLoaded[sizeof(CFE_TBL_Global.HkPacket.Payload.LastFileLoaded) - 1] =
                                        '\0';
                                    strncpy(CFE_TBL_Global.HkPacket.Payload.LastTableLoaded, TblFileHeader.TableName,
                                            sizeof(CFE_TBL_Global.HkPacket.Payload.LastTableLoaded) - 1);
                                    CFE_TBL_Global.HkPacket.Payload
                                        .LastTableLoaded[sizeof(CFE_TBL_Global.HkPacket.Payload.LastTableLoaded) - 1] =
                                        '\0';

                                    /* Increment successful command completion counter */
                                    ReturnCode = CFE_TBL_INC_CMD_CTR;
                                }
                            }
                            else
                            {
                                /* A file whose header claims has 'x' amount of data but it only has 'y' */
                                /* is considered a fatal error during a load process                     */
                                CFE_EVS_SendEvent(CFE_TBL_FILE_INCOMPLETE_ERR_EID, CFE_EVS_EventType_ERROR,
                                                  "Incomplete load of '%s' into '%s' working buffer", LoadFilename,
                                                  TblFileHeader.TableName);
                            }
                        }
                        else
                        {
                            CFE_EVS_SendEvent(CFE_TBL_NO_WORK_BUFFERS_ERR_EID, CFE_EVS_EventType_ERROR,
                                              "No working buffers available for table '%s'", TblFileHeader.TableName);
                        }
                    }
                    else
                    {
                        if ((TblFileHeader.NumBytes + TblFileHeader.Offset) > RegRecPtr->Size)
                        {
                            CFE_EVS_SendEvent(CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, CFE_EVS_EventType_ERROR,
                                              "Cannot load '%s' (%d) at offset %d in '%s' (%d)", LoadFilename,
                                              (int)TblFileHeader.NumBytes, (int)TblFileHeader.Offset,
                                              TblFileHeader.TableName, (int)RegRecPtr->Size);
                        }
                        else if (TblFileHeader.NumBytes == 0)
                        {
                            CFE_EVS_SendEvent(CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID, CFE_EVS_EventType_ERROR,
                                              "Table Hdr in '%s' indicates no data in file", LoadFilename);
                        }
                        else
                        {
                            CFE_EVS_SendEvent(CFE_TBL_PARTIAL_LOAD_ERR_EID, CFE_EVS_EventType_ERROR,
                                              "'%s' has partial load for uninitialized table '%s'", LoadFilename,
                                              TblFileHeader.TableName);
                        }
                    }
                }
            }
        } /* No need to issue event messages in response to errors reading headers */
          /* because the function that read the headers will generate messages     */

        /* Close the file now that the contents have been read */
        OS_close(FileDescriptor);
    }
    else
    {
        /* Error opening specified file */
        CFE_EVS_SendEvent(CFE_TBL_FILE_ACCESS_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to open file '%s' for table load, Status = %ld", LoadFilename, (long)OsStatus);
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_DumpCmd(const CFE_TBL_DumpCmd_t *data)
{
    CFE_TBL_CmdProcRet_t             ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    CFE_TBL_TxnState_t               Txn;
    const CFE_TBL_DumpCmd_Payload_t *CmdPtr = &data->Payload;
    char                             DumpFilename[OS_MAX_PATH_LEN];
    char                             TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_RegistryRec_t *          RegRecPtr;
    CFE_TBL_LoadBuff_t *             WorkingBufferPtr;
    CFE_TBL_LoadBuff_t *             SelectedBufferPtr;
    CFE_ResourceId_t                 PendingDumpId;
    CFE_Status_t                     Status;
    CFE_TBL_DumpControl_t *          DumpCtrlPtr;

    SelectedBufferPtr = NULL;
    WorkingBufferPtr  = NULL;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(DumpFilename, (char *)CmdPtr->DumpFilename, NULL, sizeof(DumpFilename),
                            sizeof(CmdPtr->DumpFilename));

    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    Status = CFE_TBL_TxnStartFromName(&Txn, TableName, CFE_TBL_TxnContext_UNDEFINED);

    if (Status == CFE_SUCCESS)
    {
        /* Obtain a pointer to registry information about specified table */
        RegRecPtr = CFE_TBL_TxnRegRec(&Txn);

        /* Determine what data is to be dumped */
        SelectedBufferPtr = CFE_TBL_GetSelectedBuffer(RegRecPtr, CmdPtr->ActiveTableFlag);

        CFE_TBL_TxnFinish(&Txn);

        if (SelectedBufferPtr == NULL)
        {
            CFE_EVS_SendEvent(CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Cmd for Table '%s' had illegal buffer parameter (0x%08X)", TableName,
                              (unsigned int)CmdPtr->ActiveTableFlag);
        }
        else
        {
            /* If we have located the data to be dumped, then proceed with creating the file and dumping the data */
            /* If this is not a dump only table, then we can perform the dump immediately */
            if (!RegRecPtr->DumpOnly)
            {
                ReturnCode = CFE_TBL_DumpToFile(DumpFilename, TableName, SelectedBufferPtr->BufferPtr, RegRecPtr->Size);
            }
            else /* Dump Only tables need to synchronize their dumps with the owner's execution */
            {
                /* Make sure a dump is not already in progress */
                if (!CFE_RESOURCEID_TEST_DEFINED(RegRecPtr->DumpControlId))
                {
                    /* Find a free Dump Control Block */
                    PendingDumpId = CFE_TBL_GetNextDumpCtrlBlock();
                    DumpCtrlPtr   = CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DUMPCTRLID_C(PendingDumpId));
                    if (DumpCtrlPtr != NULL)
                    {
                        /* Allocate a shared memory buffer for storing the data to be dumped */
                        Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, false);

                        if (Status == CFE_SUCCESS)
                        {
                            DumpCtrlPtr->State     = CFE_TBL_DUMP_PENDING;
                            DumpCtrlPtr->RegRecPtr = RegRecPtr;

                            /* Save the name of the desired dump filename, table name and size for later */
                            DumpCtrlPtr->DumpBufferPtr = WorkingBufferPtr;
                            memcpy(DumpCtrlPtr->DumpBufferPtr->DataSource, DumpFilename, OS_MAX_PATH_LEN);
                            memcpy(DumpCtrlPtr->TableName, TableName, CFE_TBL_MAX_FULL_NAME_LEN);
                            DumpCtrlPtr->Size = RegRecPtr->Size;

                            /* Notify the owning application that a dump is pending */
                            CFE_TBL_DumpCtrlBlockSetUsed(DumpCtrlPtr, PendingDumpId);
                            RegRecPtr->DumpControlId = CFE_TBL_DumpCtrlBlockGetId(DumpCtrlPtr);

                            /* If application requested notification by message, then do so */
                            CFE_TBL_SendNotificationMsg(RegRecPtr);

                            /* Consider the command completed successfully */
                            ReturnCode = CFE_TBL_INC_CMD_CTR;
                        }
                        else
                        {
                            CFE_EVS_SendEvent(CFE_TBL_NO_WORK_BUFFERS_ERR_EID, CFE_EVS_EventType_ERROR,
                                              "No working buffers available for table '%s'", TableName);
                        }
                    }
                    else
                    {
                        CFE_EVS_SendEvent(CFE_TBL_TOO_MANY_DUMPS_ERR_EID, CFE_EVS_EventType_ERROR,
                                          "Too many Dump Only Table Dumps have been requested");
                    }
                }
                else
                {
                    CFE_EVS_SendEvent(CFE_TBL_DUMP_PENDING_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "A dump for '%s' is already pending", TableName);
                }
            }
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_CmdProcRet_t CFE_TBL_DumpToFile(const char *DumpFilename, const char *TableName, const void *DumpDataAddr,
                                        size_t TblSizeInBytes)
{
    CFE_TBL_CmdProcRet_t ReturnCode      = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    bool                 FileExistedPrev = false;
    CFE_FS_Header_t      StdFileHeader;
    CFE_TBL_File_Hdr_t   TblFileHeader;
    osal_id_t            FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    int32                Status;
    int32                OsStatus;
    int32                EndianCheck = 0x01020304;

    /* Clear Header of any garbage before copying content */
    memset(&TblFileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

    /* Check to see if the dump file already exists */
    OsStatus = OS_OpenCreate(&FileDescriptor, DumpFilename, OS_FILE_FLAG_NONE, OS_READ_ONLY);

    if (OsStatus == OS_SUCCESS)
    {
        FileExistedPrev = true;
        OS_close(FileDescriptor);
    }

    /* Create a new dump file, overwriting anything that may have existed previously */
    OsStatus = OS_OpenCreate(&FileDescriptor, DumpFilename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);

    if (OsStatus == OS_SUCCESS)
    {
        /* Initialize the standard cFE File Header for the Dump File */
        CFE_FS_InitHeader(&StdFileHeader, "Table Dump Image", CFE_FS_SubType_TBL_IMG);

        /* Output the Standard cFE File Header to the Dump File */
        Status = CFE_FS_WriteHeader(FileDescriptor, &StdFileHeader);

        if (Status == sizeof(CFE_FS_Header_t))
        {
            /* Initialize the Table Image Header for the Dump File */
            strncpy(TblFileHeader.TableName, TableName, sizeof(TblFileHeader.TableName) - 1);
            TblFileHeader.TableName[sizeof(TblFileHeader.TableName) - 1] = 0;
            TblFileHeader.Offset                                         = 0;
            TblFileHeader.NumBytes                                       = TblSizeInBytes;
            TblFileHeader.Reserved                                       = 0;

            /* Determine if this is a little endian processor */
            if ((*(char *)&EndianCheck) == 0x04)
            {
                /* If this is a little endian processor, then byte swap the header to a big endian format */
                /* to maintain the cFE Header standards */
                /* NOTE: FOR THE REMAINDER OF THIS FUNCTION, THE CONTENTS OF THE HEADER IS UNREADABLE BY */
                /*       THIS PROCESSOR!  THE DATA WOULD NEED TO BE SWAPPED BACK BEFORE READING.         */
                CFE_TBL_ByteSwapTblHeader(&TblFileHeader);
            }

            /* Output the Table Image Header to the Dump File */
            OsStatus = OS_write(FileDescriptor, &TblFileHeader, sizeof(CFE_TBL_File_Hdr_t));

            /* Make sure the header was output completely */
            if ((long)OsStatus == sizeof(CFE_TBL_File_Hdr_t))
            {
                /* Output the requested data to the dump file */
                /* Output the active table image data to the dump file */
                OsStatus = OS_write(FileDescriptor, DumpDataAddr, TblSizeInBytes);

                if ((long)OsStatus == TblSizeInBytes)
                {
                    if (FileExistedPrev)
                    {
                        CFE_EVS_SendEvent(CFE_TBL_OVERWRITE_DUMP_INF_EID, CFE_EVS_EventType_INFORMATION,
                                          "Successfully overwrote '%s' with Table '%s'", DumpFilename, TableName);
                    }
                    else
                    {
                        CFE_EVS_SendEvent(CFE_TBL_WRITE_DUMP_INF_EID, CFE_EVS_EventType_INFORMATION,
                                          "Successfully dumped Table '%s' to '%s'", TableName, DumpFilename);
                    }

                    /* Save file information statistics for housekeeping telemetry */
                    strncpy(CFE_TBL_Global.HkPacket.Payload.LastFileDumped, DumpFilename,
                            sizeof(CFE_TBL_Global.HkPacket.Payload.LastFileDumped) - 1);
                    CFE_TBL_Global.HkPacket.Payload
                        .LastFileDumped[sizeof(CFE_TBL_Global.HkPacket.Payload.LastFileDumped) - 1] = 0;

                    /* Increment Successful Command Counter */
                    ReturnCode = CFE_TBL_INC_CMD_CTR;
                }
                else
                {
                    CFE_EVS_SendEvent(CFE_TBL_WRITE_TBL_IMG_ERR_EID, CFE_EVS_EventType_ERROR,
                                      "Error writing Tbl image to '%s', Status=%ld", DumpFilename, (long)OsStatus);
                }
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_WRITE_TBL_HDR_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Error writing Tbl image File Header to '%s', Status=%ld", DumpFilename,
                                  (long)OsStatus);
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_WRITE_CFE_HDR_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Error writing cFE File Header to '%s', Status=0x%08X", DumpFilename,
                              (unsigned int)Status);
        }

        /* We are done outputting data to the dump file.  Close it. */
        OS_close(FileDescriptor);
    }
    else
    {
        CFE_EVS_SendEvent(CFE_TBL_CREATING_DUMP_FILE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Error creating dump file '%s', Status=%ld", DumpFilename, (long)OsStatus);
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_ValidateCmd(const CFE_TBL_ValidateCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                 ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    CFE_TBL_TxnState_t                   Txn;
    CFE_Status_t                         Status;
    const CFE_TBL_ValidateCmd_Payload_t *CmdPtr = &data->Payload;
    CFE_TBL_RegistryRec_t *              RegRecPtr;
    CFE_TBL_LoadBuff_t *                 SelectedBufferPtr;
    char                                 TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    uint32                               CrcOfTable;
    CFE_ResourceId_t                     PendingValId;
    CFE_TBL_ValidationResult_t *         ValResultPtr;

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
                CrcOfTable =
                    CFE_ES_CalculateCRC(SelectedBufferPtr->BufferPtr, RegRecPtr->Size, 0, CFE_MISSION_ES_DEFAULT_CRC);

                ValResultPtr->CrcOfTable   = CrcOfTable;
                ValResultPtr->ActiveBuffer = (CmdPtr->ActiveTableFlag != 0);

                CFE_TBL_ValidationResultSetUsed(ValResultPtr, PendingValId);

                /* If owner has a validation function, then notify the  */
                /* table owner that there is data to be validated       */
                if (RegRecPtr->ValidationFuncPtr != NULL)
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
                ReturnCode = CFE_TBL_INC_CMD_CTR;
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

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_ActivateCmd(const CFE_TBL_ActivateCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                 ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    int16                                RegIndex;
    const CFE_TBL_ActivateCmd_Payload_t *CmdPtr = &data->Payload;
    char                                 TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_RegistryRec_t *              RegRecPtr;
    bool                                 ValidationStatus;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table that is to be dumped exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Obtain a pointer to registry information about specified table */
        RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];

        if (RegRecPtr->DumpOnly)
        {
            CFE_EVS_SendEvent(CFE_TBL_ACTIVATE_DUMP_ONLY_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Illegal attempt to activate dump-only table '%s'", TableName);
        }
        else if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
        {
            /* Determine if the inactive buffer has been successfully validated or not */
            if (RegRecPtr->DoubleBuffered)
            {
                ValidationStatus = RegRecPtr->Buffers[(1U - RegRecPtr->ActiveBufferIndex)].Validated;
            }
            else
            {
                ValidationStatus = CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Validated;
            }

            if (ValidationStatus == true)
            {
                CFE_TBL_Global.Registry[RegIndex].LoadPending = true;

                /* If application requested notification by message, then do so */
                if (CFE_TBL_SendNotificationMsg(RegRecPtr) == CFE_SUCCESS)
                {
                    CFE_EVS_SendEvent(CFE_TBL_LOAD_PEND_REQ_INF_EID, CFE_EVS_EventType_DEBUG,
                                      "Tbl Services notifying App that '%s' has a load pending", TableName);
                }

                /* Increment Successful Command Counter */
                ReturnCode = CFE_TBL_INC_CMD_CTR;
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_UNVALIDATED_ERR_EID, CFE_EVS_EventType_ERROR,
                                  "Cannot activate table '%s'. Inactive image not Validated", TableName);
            }
        }
        else
        {
            CFE_EVS_SendEvent(CFE_TBL_ACTIVATE_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Cannot activate table '%s'. No Inactive image available", TableName);
        }
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }

    return ReturnCode;
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
        if (!CFE_RESOURCEID_TEST_EQUAL(RegRecPtr->OwnerAppId, CFE_TBL_NOT_OWNED) ||
            CFE_TBL_HandleLinkIsAttached(&RegRecPtr->AccessList))
        {
            IsValidEntry = true;
            OwnerAppId   = RegRecPtr->OwnerAppId;

            /* Fill Registry Dump Record with relevant information */
            StatePtr->DumpRecord.Size             = CFE_ES_MEMOFFSET_C(RegRecPtr->Size);
            StatePtr->DumpRecord.TimeOfLastUpdate = RegRecPtr->TimeOfLastUpdate;
            StatePtr->DumpRecord.LoadInProgress   = RegRecPtr->LoadInProgress;
            StatePtr->DumpRecord.ValidationFunc   = (RegRecPtr->ValidationFuncPtr != NULL);
            StatePtr->DumpRecord.TableLoadedOnce  = RegRecPtr->TableLoadedOnce;
            StatePtr->DumpRecord.LoadPending      = RegRecPtr->LoadPending;
            StatePtr->DumpRecord.DumpOnly         = RegRecPtr->DumpOnly;
            StatePtr->DumpRecord.DoubleBuffered   = RegRecPtr->DoubleBuffered;
            StatePtr->DumpRecord.FileTime         = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileTime;
            StatePtr->DumpRecord.Crc              = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].Crc;
            StatePtr->DumpRecord.CriticalTable    = RegRecPtr->CriticalTable;

            /* Convert LoadInProgress flag into more meaningful information */
            /* When a load is in progress, identify which buffer is being used as the inactive buffer */
            if (StatePtr->DumpRecord.LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
            {
                if (StatePtr->DumpRecord.DoubleBuffered)
                {
                    /* For double buffered tables, the value of LoadInProgress, when a load is actually in progress, */
                    /* should identify either buffer #0 or buffer #1.  Convert these to enumerated value for ground  */
                    /* display.  LoadInProgress = -2 means Buffer #1, LoadInProgress = -3 means Buffer #0.           */
                    StatePtr->DumpRecord.LoadInProgress = StatePtr->DumpRecord.LoadInProgress - 3;
                }
                /* For single buffered tables, the value of LoadInProgress, when a load is actually in progress,     */
                /* indicates which shared buffer is allocated for the inactive buffer.  Since the number of inactive */
                /* buffers is a platform configuration parameter, then 0 on up merely identifies the buffer number.  */
                /* No translation is necessary for single buffered tables.                                           */
            }

            strncpy(StatePtr->DumpRecord.Name, RegRecPtr->Name, sizeof(StatePtr->DumpRecord.Name) - 1);
            StatePtr->DumpRecord.Name[sizeof(StatePtr->DumpRecord.Name) - 1] = 0;

            strncpy(StatePtr->DumpRecord.LastFileLoaded, RegRecPtr->LastFileLoaded,
                    sizeof(StatePtr->DumpRecord.LastFileLoaded) - 1);
            StatePtr->DumpRecord.LastFileLoaded[sizeof(StatePtr->DumpRecord.LastFileLoaded) - 1] = 0;

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
int32 CFE_TBL_DumpRegistryCmd(const CFE_TBL_DumpRegistryCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                     ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    int32                                    Status;
    const CFE_TBL_DumpRegistryCmd_Payload_t *CmdPtr = &data->Payload;
    os_fstat_t                               FileStat;

    CFE_TBL_RegDumpStateInfo_t *StatePtr;

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
                ReturnCode = CFE_TBL_INC_CMD_CTR;
            }
        }
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_SendRegistryCmd(const CFE_TBL_SendRegistryCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                     ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    int16                                    RegIndex;
    const CFE_TBL_SendRegistryCmd_Payload_t *CmdPtr = &data->Payload;
    char                                     TableName[CFE_TBL_MAX_FULL_NAME_LEN];

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table registry entry that is to be telemetered exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Change the index used to identify what data is to be telemetered */
        CFE_TBL_Global.HkTlmTblRegIndex = RegIndex;

        CFE_EVS_SendEvent(CFE_TBL_TLM_REG_CMD_INF_EID, CFE_EVS_EventType_DEBUG,
                          "Table Registry entry for '%s' will be telemetered", TableName);

        /* Increment Successful Command Counter */
        ReturnCode = CFE_TBL_INC_CMD_CTR;
    }
    else /* Table could not be found in Registry */
    {
        CFE_EVS_SendEvent(CFE_TBL_NO_SUCH_TABLE_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Unable to locate '%s' in Table Registry", TableName);
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_DeleteCDSCmd(const CFE_TBL_DeleteCDSCmd_t *data)
{
    CFE_TBL_CmdProcRet_t               ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    const CFE_TBL_DelCDSCmd_Payload_t *CmdPtr     = &data->Payload;
    char                               TableName[CFE_TBL_MAX_FULL_NAME_LEN];
    CFE_TBL_CritRegRec_t *             CritRegRecPtr = NULL;
    uint32                             i;
    int16                              RegIndex;
    int32                              Status;

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table is no longer in the registry */
    /* This would imply that the owning application has been terminated and that it */
    /* is safe to delete the associated critical table image in the CDS. */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex == CFE_TBL_NOT_FOUND)
    {
        /* Find table in the Critical Table Registry */
        for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
        {
            if (strncmp(CFE_TBL_Global.CritReg[i].Name, TableName, CFE_TBL_MAX_FULL_NAME_LEN) == 0)
            {
                CritRegRecPtr = &CFE_TBL_Global.CritReg[i];
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
                ReturnCode = CFE_TBL_INC_CMD_CTR;
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
    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_AbortLoadCmd(const CFE_TBL_AbortLoadCmd_t *data)
{
    CFE_TBL_CmdProcRet_t                  ReturnCode = CFE_TBL_INC_ERR_CTR; /* Assume failure */
    int16                                 RegIndex;
    const CFE_TBL_AbortLoadCmd_Payload_t *CmdPtr = &data->Payload;
    CFE_TBL_RegistryRec_t *               RegRecPtr;
    char                                  TableName[CFE_TBL_MAX_FULL_NAME_LEN];

    /* Make sure all strings are null terminated before attempting to process them */
    CFE_SB_MessageStringGet(TableName, (char *)CmdPtr->TableName, NULL, sizeof(TableName), sizeof(CmdPtr->TableName));

    /* Before doing anything, lets make sure the table registry entry that is to be telemetered exists */
    RegIndex = CFE_TBL_FindTableInRegistry(TableName);

    if (RegIndex != CFE_TBL_NOT_FOUND)
    {
        /* Make a pointer to simplify code look and to remove redundant indexing into registry */
        RegRecPtr = &CFE_TBL_Global.Registry[RegIndex];

        /* Check to make sure a load was in progress before trying to abort it */
        /* NOTE: LoadInProgress contains index of buffer when dumping a dump-only table */
        /* so we must ensure the table is not a dump-only table, otherwise, we would be aborting a dump */
        if ((RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS) && (!RegRecPtr->DumpOnly))
        {
            CFE_TBL_AbortLoad(RegRecPtr);

            /* Increment Successful Command Counter */
            ReturnCode = CFE_TBL_INC_CMD_CTR;
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

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_AbortLoad(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* The ground has aborted the load, free the working buffer for another attempt */
    if (!RegRecPtr->DoubleBuffered)
    {
        /* For single buffered tables, freeing shared buffer entails resetting flag */
        CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Taken = false;
    }

    /* For double buffered tables, freeing buffer is simple */
    RegRecPtr->LoadInProgress = CFE_TBL_NO_LOAD_IN_PROGRESS;

    /* Make sure the load was not already pending */
    RegRecPtr->LoadPending = false;

    CFE_EVS_SendEvent(CFE_TBL_LOAD_ABORT_INF_EID, CFE_EVS_EventType_INFORMATION, "Table Load Aborted for '%s'",
                      RegRecPtr->Name);
}
