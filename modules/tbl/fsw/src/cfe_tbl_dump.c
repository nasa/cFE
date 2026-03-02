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
 * Implementation of table services table dump (write to file) methods
 */

/*
** Required header files...
*/
#include "cfe_tbl_module_all.h"
#include "cfe_tbl_codec.h"
#include "cfe_config.h"

#include <stdio.h>
#include <string.h>

typedef struct CFE_TBL_DumpContext
{
    const char *Operation;
    const char *Tablename;
    const char *DumpFilename;
    const char *CallerName;

    const CFE_TBL_RegistryRec_t *RegRecPtr;
} CFE_TBL_DumpContext_t;

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_WriteHeaders(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                  const CFE_TBL_CombinedFileHdr_t *FileHeader)
{
    CFE_Status_t Status;

    union
    {
        CFE_FS_Header_t    Std;
        CFE_TBL_File_Hdr_t Tbl;
    } Buffer;

    /* Initialize the standard cFE File Header for the Dump File */
    /* This is needed because the FS API call (possibly) modifies the buffer during write */
    Buffer.Std = FileHeader->Std;

    /* Output the Standard cFE File Header to the Dump File */
    Status = CFE_FS_WriteHeader(FileDescriptor, &Buffer.Std);

    if (Status != sizeof(CFE_FS_Header_t))
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_WRITE_CFE_HDR_ERR_EID, Status, sizeof(CFE_FS_Header_t));
        Status = CFE_TBL_ERR_ACCESS;
    }
    else
    {
        Status = CFE_TBL_EncodeHeadersToFile(Txn, FileDescriptor, &FileHeader->Tbl);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnOpenTableDumpFile(CFE_TBL_TxnState_t *Txn, const char *Filename, osal_id_t *FileDescOut,
                                          const CFE_TBL_CombinedFileHdr_t *FileHeader)
{
    CFE_Status_t ReturnCode;
    int32        OsStatus;
    osal_id_t    FileDescriptor;

    /* Create a new dump file, overwriting anything that may have existed previously */
    OsStatus = OS_OpenCreate(&FileDescriptor, Filename, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);

    if (OsStatus != OS_SUCCESS)
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_CREATING_DUMP_FILE_ERR_EID, OsStatus, 0);
        ReturnCode     = CFE_TBL_ERR_ACCESS;
        FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    }
    else
    {
        ReturnCode = CFE_TBL_WriteHeaders(Txn, FileDescriptor, FileHeader);
        if (ReturnCode != CFE_SUCCESS)
        {
            OS_close(FileDescriptor);
            FileDescriptor = OS_OBJECT_ID_UNDEFINED;
        }
    }

    *FileDescOut = FileDescriptor;

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_WriteSnapshotToFile(const CFE_TBL_DumpControl_t *DumpCtlPtr)
{
    CFE_Status_t              Status;
    int32                     OsStatus;
    bool                      FileExistedPrev;
    CFE_TBL_CombinedFileHdr_t FileHeader;
    osal_id_t                 FileDescriptor;
    const char *              DumpFilename;
    const void *              DumpDataAddr;
    size_t                    DumpDataSize;
    CFE_TBL_TxnState_t        Txn;

    CFE_TBL_TxnInit(&Txn, false);

    DumpFilename = DumpCtlPtr->DumpBufferPtr->DataSource;
    DumpDataAddr = CFE_TBL_LoadBuffGetReadPointer(DumpCtlPtr->DumpBufferPtr);
    DumpDataSize = CFE_TBL_LoadBuffGetContentSize(DumpCtlPtr->DumpBufferPtr);

    /* Clear Header of any garbage before copying content */
    memset(&FileHeader, 0, sizeof(CFE_TBL_File_Hdr_t));

    /* Initialize the standard cFE File Header for the Dump File */
    CFE_FS_InitHeader(&FileHeader.Std, "Table Dump Image", CFE_FS_SubType_TBL_IMG);
    FileHeader.Std.TimeSeconds    = DumpCtlPtr->DumpBufferPtr->FileTime.Seconds;
    FileHeader.Std.TimeSubSeconds = DumpCtlPtr->DumpBufferPtr->FileTime.Subseconds;

    /* Initialize the Table Image Header for the Dump File */
    strncpy(FileHeader.Tbl.TableName, DumpCtlPtr->TableName, sizeof(FileHeader.Tbl.TableName) - 1);
    FileHeader.Tbl.NumBytes = DumpDataSize;

    /* Check to see if the dump file already exists */
    /* NOTE: this is only relevant for sending a success event later, it changes the EID */
    OsStatus = OS_OpenCreate(&FileDescriptor, DumpFilename, OS_FILE_FLAG_NONE, OS_READ_ONLY);

    if (OsStatus == OS_SUCCESS)
    {
        FileExistedPrev = true;
        OS_close(FileDescriptor);
    }
    else
    {
        FileExistedPrev = false;
    }

    /* Create a new dump file, overwriting anything that may have existed previously */
    Status = CFE_TBL_TxnOpenTableDumpFile(&Txn, DumpFilename, &FileDescriptor, &FileHeader);

    if (Status == CFE_SUCCESS)
    {
        /* Output the requested data to the dump file */
        /* Output the active table image data to the dump file */
        OsStatus = OS_write(FileDescriptor, DumpDataAddr, DumpDataSize);

        if (OsStatus != DumpDataSize)
        {
            CFE_TBL_TxnAddEvent(&Txn, CFE_TBL_WRITE_TBL_IMG_ERR_EID, OsStatus, 0);
            Status = CFE_TBL_ERR_ACCESS;
        }

        /* We are done outputting data to the dump file.  Close it. */
        OS_close(FileDescriptor);
    }

    /* If everything went well, report happiness and update global TLM data */
    if (Status == CFE_SUCCESS)
    {
        /* The whole check of file existence is to give a different EID for overwrite vs first write */
        if (FileExistedPrev)
        {
            CFE_TBL_TxnAddEvent(&Txn, CFE_TBL_OVERWRITE_DUMP_INF_EID, 0, 0);
        }
        else
        {
            CFE_TBL_TxnAddEvent(&Txn, CFE_TBL_WRITE_DUMP_INF_EID, 0, 0);
        }

        /* Save file information statistics for housekeeping telemetry */
        CFE_SB_MessageStringSet(CFE_TBL_Global.HkPacket.Payload.LastFileDumped, DumpFilename,
                                sizeof(CFE_TBL_Global.HkPacket.Payload.LastFileDumped), -1);
    }

    CFE_TBL_SendTableDumpEvents(&Txn, NULL);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ExecuteDumpSnapshot(CFE_TBL_DumpControl_t *DumpCtrlPtr)
{
    CFE_Status_t        Status;
    CFE_TBL_TxnState_t  Txn;
    CFE_TBL_LoadBuff_t *SourceBufPtr;

    CFE_TBL_TxnInit(&Txn, false);

    SourceBufPtr = CFE_TBL_LocateLoadBufferByID(DumpCtrlPtr->SourceBuffId);
    if (CFE_TBL_LoadBuffIsMatch(SourceBufPtr, DumpCtrlPtr->SourceBuffId))
    {
        Txn.RegRecPtr = CFE_TBL_LoadBuffGetRegRecFromId(DumpCtrlPtr->SourceBuffId);
        Txn.RegId     = CFE_TBL_RegRecGetID(Txn.RegRecPtr);

        /* Copy the contents of the active buffer to the assigned dump buffer */
        Status = CFE_TBL_EncodeOutputData(&Txn, SourceBufPtr, DumpCtrlPtr->DumpBufferPtr);

        /* NOTE: In a "passthru" implementation, it is not possible for the encoding to fail,
         * this always succeeds.  Therefore this will show up as an uncovered branch in the
         * passthrough configuration.  It is not possible to fix this without adding stubs
         * for the codec layer. */
        if (Status == CFE_SUCCESS)
        {
            /* Save the current time so that the header in the dump file can have the correct time */
            DumpCtrlPtr->DumpBufferPtr->FileTime = CFE_TIME_GetTime();

            /* Notify the Table Services Application that the dump buffer is ready to be written to a file */
            DumpCtrlPtr->State        = CFE_TBL_DUMP_PERFORMED;
            DumpCtrlPtr->SourceBuffId = CFE_TBL_LOADBUFFID_UNDEFINED;
        }
    }
    else
    {
        Status = CFE_TBL_ERR_INVALID_HANDLE;
    }

    CFE_TBL_TxnFinish(&Txn);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_PrepareDumpSnapshotBuffer(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t DumpCtrlId,
                                               const char *DumpFilename)
{
    CFE_Status_t           Status;
    CFE_TBL_LoadBuff_t *   WorkingBufferPtr;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;

    /* Allocate a shared memory buffer for storing the data to be dumped */
    /* Note this uses a different lock than the registry lock */
    WorkingBufferPtr = CFE_TBL_AcquireGlobalLoadBuff(CFE_TBL_TxnRegId(Txn));

    if (WorkingBufferPtr == NULL)
    {
        Status = CFE_TBL_ERR_NO_BUFFER_AVAIL;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_NO_WORK_BUFFERS_ERR_EID, Status, 0);
    }
    else
    {
        strncpy(WorkingBufferPtr->DataSource, DumpFilename, sizeof(WorkingBufferPtr->DataSource));

        /* Now associate it with the Dump Control block */
        RegRecPtr   = CFE_TBL_TxnRegRec(Txn);
        DumpCtrlPtr = CFE_TBL_LocateDumpCtrlByID(DumpCtrlId);
        if (!CFE_TBL_DumpCtrlBlockIsMatch(DumpCtrlPtr, DumpCtrlId))
        {
            /* This is unexpected, it appears the dump was aborted in the meantime */
            Status = CFE_TBL_ERR_INVALID_HANDLE;
        }
        else
        {
            DumpCtrlPtr->DumpBufferPtr = WorkingBufferPtr;

            if (CFE_TBL_RegRecGetConfig(RegRecPtr)->DumpOnly)
            {
                /* Dump Only tables need to synchronize their dumps with the owner's execution */
                DumpCtrlPtr->State       = CFE_TBL_DUMP_PENDING;
                RegRecPtr->DumpControlId = CFE_TBL_DumpCtrlBlockGetId(DumpCtrlPtr);

                /* Consider the command completed successfully */
                Status = CFE_SUCCESS;
            }
            else
            {
                /* Do the dump right now (note this is only the memcpy, write to file still deferred) */
                Status = CFE_TBL_ExecuteDumpSnapshot(DumpCtrlPtr);
            }

            /* If application requested notification by message, then do so */
            CFE_TBL_SendNotificationMsg(RegRecPtr);
        }

        /* If leaving here unsuccessfully, then release the buffer that was obtained */
        if (Status != CFE_SUCCESS)
        {
            CFE_TBL_LoadBuffSetFree(WorkingBufferPtr);
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
CFE_Status_t CFE_TBL_AllocateDumpCtrlBlock(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t *DumpCtrlIdOut,
                                           CFE_TBL_BufferSelect_Enum_t BufferSelect)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_LoadBuff_t *   SelectedBufferPtr;
    CFE_ResourceId_t       PendingDumpId;
    CFE_Status_t           Status;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;

    DumpCtrlPtr       = NULL;
    SelectedBufferPtr = NULL;

    /* Obtain a pointer to registry information about specified table */
    RegRecPtr = CFE_TBL_TxnRegRec(Txn);
    if (RegRecPtr == NULL)
    {
        Status = CFE_TBL_ERR_INVALID_HANDLE;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_NO_SUCH_TABLE_ERR_EID, Status, 0);
    }
    else if (CFE_TBL_DUMPCTRLID_IS_VALID(RegRecPtr->DumpControlId))
    {
        /* A dump is already in progress */
        Status = CFE_TBL_INFO_DUMP_PENDING;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_DUMP_PENDING_ERR_EID, Status, 0);
    }
    else
    {
        /* Determine what data is to be dumped */
        SelectedBufferPtr = CFE_TBL_GetSelectedBuffer(RegRecPtr, BufferSelect);

        if (SelectedBufferPtr == NULL)
        {
            /* Use deferred event reporting while registry is locked */
            Status = CFE_TBL_BAD_ARGUMENT;
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID, Status, BufferSelect);
        }
    }

    /* If there is a valid source buffer to dump, find a dump ctrl block to track progress */
    if (SelectedBufferPtr != NULL)
    {
        /* Find a free Dump Control Block */
        PendingDumpId = CFE_TBL_GetNextDumpCtrlBlock();
        DumpCtrlPtr   = CFE_TBL_LocateDumpCtrlByID(CFE_TBL_DUMPCTRLID_C(PendingDumpId));
        if (DumpCtrlPtr == NULL)
        {
            Status = CFE_TBL_ERR_NO_BUFFER_AVAIL;
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_TOO_MANY_DUMPS_ERR_EID, Status, 0);
        }
        else
        {
            /* Reserve the block */
            memset(DumpCtrlPtr, 0, sizeof(*DumpCtrlPtr));

            /* Capture important context info to the dump control block */
            DumpCtrlPtr->SourceBuffId = CFE_TBL_LoadBufferGetID(SelectedBufferPtr);
            CFE_SB_MessageStringSet(DumpCtrlPtr->TableName, CFE_TBL_RegRecGetName(RegRecPtr),
                                    sizeof(DumpCtrlPtr->TableName), CFE_TBL_MAX_FULL_NAME_LEN);

            CFE_TBL_DumpCtrlBlockSetUsed(DumpCtrlPtr, PendingDumpId);

            CFE_TBL_Global.LastDumpCtrlBlockId = PendingDumpId;

            Status = CFE_SUCCESS;
        }
    }

    if (DumpCtrlPtr != NULL)
    {
        *DumpCtrlIdOut = CFE_TBL_DumpCtrlBlockGetId(DumpCtrlPtr);
    }
    else
    {
        *DumpCtrlIdOut = CFE_TBL_DUMPCTRLID_UNDEFINED;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function for sending events
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_SendDumpEventHelper(const CFE_TBL_TxnEvent_t *Event, void *Arg)
{
    const CFE_TBL_DumpContext_t *Ctxt;
    uint16                       EventType;
    char                         EventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    Ctxt = Arg;

    /* The majority of the events are errors, but this can be reset later to demote to info/debug */
    EventType      = CFE_EVS_EventType_ERROR;
    EventString[0] = 0;

    switch (Event->EventId)
    {
        case CFE_TBL_NO_WORK_BUFFERS_ERR_EID:
            snprintf(EventString, sizeof(EventString), "No working buffers available");
            break;

        case CFE_TBL_TOO_MANY_DUMPS_ERR_EID:
            snprintf(EventString, sizeof(EventString), "Too many table dumps have been requested");
            break;

        case CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID:
            snprintf(EventString, sizeof(EventString), "Cmd had illegal buffer parameter (0x%08X)",
                     (unsigned int)Event->EventData2);
            break;

        case CFE_TBL_DUMP_PENDING_ERR_EID:
            snprintf(EventString, sizeof(EventString), "A dump is already pending");
            break;

        case CFE_TBL_WRITE_CFE_HDR_ERR_EID:
            snprintf(EventString, sizeof(EventString), "Error writing cFE File Header, Status=0x%08X",
                     (unsigned int)Event->EventData1);
            break;

        case CFE_TBL_WRITE_TBL_HDR_ERR_EID:
            snprintf(EventString, sizeof(EventString), "Error writing cFE TBL Header, Status=%d",
                     (int)Event->EventData1);
            break;

        case CFE_TBL_CREATING_DUMP_FILE_ERR_EID:
            snprintf(EventString, sizeof(EventString), "Error creating dump file, Status=%d", (int)Event->EventData1);
            break;

        case CFE_TBL_WRITE_TBL_IMG_ERR_EID:
            snprintf(EventString, sizeof(EventString), "Error writing Tbl image, Status=%d", (int)Event->EventData1);
            break;

        case CFE_TBL_OVERWRITE_DUMP_INF_EID:
            EventType = CFE_EVS_EventType_INFORMATION;
            snprintf(EventString, sizeof(EventString), "Successfully overwrote table dump");
            break;

        case CFE_TBL_WRITE_DUMP_INF_EID:
            EventType = CFE_EVS_EventType_INFORMATION;
            snprintf(EventString, sizeof(EventString), "Successfully dumped table");
            break;
    }

    if (EventString[0] == 0)
    {
        return false;
    }

    /* Finally send the actual event by appending all the info we have */
    CFE_EVS_SendEventWithAppID(Event->EventId, EventType, CFE_TBL_Global.TableTaskAppId,
                               "%s,table=%s,app=%s,file=%s:%s", Ctxt->Operation, Ctxt->Tablename, Ctxt->CallerName,
                               Ctxt->DumpFilename, EventString);

    return true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_SendTableDumpEvents(CFE_TBL_TxnState_t *Txn, const char *RequestedTableName)
{
    CFE_TBL_DumpContext_t Ctxt;

    memset(&Ctxt, 0, sizeof(Ctxt));

    Ctxt.Operation  = "dump";
    Ctxt.CallerName = CFE_TBL_TxnAppNameCaller(Txn);
    Ctxt.RegRecPtr  = CFE_TBL_TxnRegRec(Txn);

    if (RequestedTableName != NULL)
    {
        Ctxt.Tablename = RequestedTableName;
    }
    else
    {
        /* do not leave it null/blank */
        Ctxt.Tablename = "[unknown]";
    }

    CFE_TBL_TxnProcessEvents(Txn, CFE_TBL_SendDumpEventHelper, &Ctxt);
}
