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
#include "cfe_fs_core_internal.h"
#include "cfe_psp.h"

#include <stdio.h>
#include <string.h>

typedef struct CFE_TBL_DumpContext
{
    const char *FileName;
    const char *RequestedTableName;
} CFE_TBL_DumpContext_t;

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t
CFE_TBL_WriteHeaders(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor, const CFE_TBL_CombinedFileHdr_t *FileHeader)
{
    CFE_Status_t Status;

    /* The core serializer preserves snapshot metadata and leaves the caller's
     * header unchanged. The table-specific codec writes the second header. */
    Status = CFE_FS_WriteHeaderFromBuffer(FileDescriptor, &FileHeader->Std);

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

/* The optional FS header callback preserves the snapshot timestamp and uses
 * the existing native/EDS table-header codec. Only fixed-size headers are
 * written here; table content is supplied in bounded records below. */
static int32 CFE_TBL_WriteSnapshotHeader(void *Meta, osal_id_t FileDescriptor)
{
    CFE_TBL_DumpControl_t    *DumpCtlPtr = Meta;
    CFE_TBL_CombinedFileHdr_t FileHeader;
    CFE_TBL_TxnState_t        Txn;
    CFE_Status_t              Status;
    int32                     HeaderSize;

    CFE_TBL_TxnInit(&Txn, false);
    memset(&FileHeader, 0, sizeof(FileHeader));
    CFE_FS_InitHeader(&FileHeader.Std, "Table Dump Image", CFE_FS_SubType_TBL_IMG);
    FileHeader.Std.ContentType    = CFE_FS_FILE_CONTENT_ID;
    FileHeader.Std.Length         = sizeof(CFE_FS_Header_t);
    FileHeader.Std.SpacecraftID   = CFE_PSP_GetSpacecraftId();
    FileHeader.Std.ProcessorID    = CFE_PSP_GetProcessorId();
    FileHeader.Std.ApplicationID  = CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.TableTaskAppId);
    FileHeader.Std.TimeSeconds    = DumpCtlPtr->DumpBufferPtr->FileTime.Seconds;
    FileHeader.Std.TimeSubSeconds = DumpCtlPtr->DumpBufferPtr->FileTime.Subseconds;
    strncpy(FileHeader.Tbl.TableName, DumpCtlPtr->TableName, sizeof(FileHeader.Tbl.TableName) - 1);
    FileHeader.Tbl.NumBytes = CFE_TBL_LoadBuffGetContentSize(DumpCtlPtr->DumpBufferPtr);

    Status = CFE_TBL_WriteHeaders(&Txn, FileDescriptor, &FileHeader);
    if (Status == CFE_SUCCESS)
    {
        /* Obtain the encoded size rather than assuming native and EDS headers
         * have the same representation. Table files support seeking. */
        HeaderSize = OS_lseek(FileDescriptor, 0, OS_SEEK_CUR);
        if (HeaderSize > 0)
        {
            return HeaderSize;
        }
        CFE_TBL_TxnAddEvent(&Txn, CFE_TBL_WRITE_TBL_HDR_ERR_EID, HeaderSize, 0);
        Status = CFE_TBL_ERR_ACCESS;
    }

    CFE_TBL_SendTableDumpEvents(&Txn, DumpCtlPtr->FileWrite.FileName, DumpCtlPtr->TableName);
    return Status;
}

static bool CFE_TBL_SnapshotDataGetter(void *Meta, uint32 RecordNum, void **Buffer, size_t *BufSize)
{
    CFE_TBL_DumpControl_t *DumpCtlPtr = Meta;
    size_t                 DataSize   = CFE_TBL_LoadBuffGetContentSize(DumpCtlPtr->DumpBufferPtr);

    (void)RecordNum;
    *Buffer  = NULL;
    *BufSize = 0;
    if (DumpCtlPtr->NextOffset < DataSize)
    {
        *BufSize = DataSize - DumpCtlPtr->NextOffset;
        if (*BufSize > CFE_TBL_DUMP_BLOCK_SIZE)
        {
            *BufSize = CFE_TBL_DUMP_BLOCK_SIZE;
        }
        *Buffer = (uint8 *)CFE_TBL_LoadBuffGetReadPointer(DumpCtlPtr->DumpBufferPtr) + DumpCtlPtr->NextOffset;
        DumpCtlPtr->NextOffset += *BufSize;
    }
    return DumpCtlPtr->NextOffset >= DataSize;
}

static void CFE_TBL_SnapshotEventHandler(void                   *Meta,
                                         CFE_FS_FileWriteEvent_t Event,
                                         int32                   Status,
                                         uint32                  RecordNum,
                                         size_t                  BlockSize,
                                         size_t                  Position)
{
    CFE_TBL_DumpControl_t *DumpCtlPtr = Meta;
    CFE_TBL_TxnState_t     Txn;

    (void)RecordNum;
    (void)BlockSize;
    (void)Position;
    CFE_TBL_TxnInit(&Txn, false);
    switch (Event)
    {
        case CFE_FS_FileWriteEvent_COMPLETE:
            DumpCtlPtr->WriteStatus = CFE_SUCCESS;
            CFE_TBL_TxnAddEvent(&Txn,
                                DumpCtlPtr->FileExisted ? CFE_TBL_OVERWRITE_DUMP_INF_EID : CFE_TBL_WRITE_DUMP_INF_EID,
                                0,
                                0);
            break;
        case CFE_FS_FileWriteEvent_CREATE_ERROR:
            DumpCtlPtr->WriteStatus = CFE_TBL_ERR_ACCESS;
            CFE_TBL_TxnAddEvent(&Txn, CFE_TBL_CREATING_DUMP_FILE_ERR_EID, Status, 0);
            break;
        case CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR:
            DumpCtlPtr->WriteStatus = CFE_TBL_ERR_ACCESS;
            CFE_TBL_TxnAddEvent(&Txn, CFE_TBL_WRITE_TBL_IMG_ERR_EID, Status, 0);
            break;
        case CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR:
            /* The header callback already recorded the precise codec/write error. */
            DumpCtlPtr->WriteStatus = CFE_TBL_ERR_ACCESS;
            break;
        default:
            break;
    }

    /* This helper attributes events to TBL even in the ES background task.
     * Do not release buffers here: FS still accesses Meta after this callback. */
    CFE_TBL_SendTableDumpEvents(&Txn, DumpCtlPtr->FileWrite.FileName, DumpCtlPtr->TableName);
}

CFE_Status_t CFE_TBL_WriteSnapshotToFile(CFE_TBL_DumpControl_t *DumpCtlPtr)
{
    CFE_Status_t Status;
    os_fstat_t   FileStat;
    int          NameLength;

    if (CFE_FS_BackgroundFileDumpIsPending(&DumpCtlPtr->FileWrite))
    {
        return CFE_STATUS_REQUEST_ALREADY_PENDING;
    }

    NameLength = snprintf(DumpCtlPtr->FileWrite.FileName,
                          sizeof(DumpCtlPtr->FileWrite.FileName),
                          "%.*s",
                          (int)sizeof(DumpCtlPtr->DumpBufferPtr->DataSource),
                          DumpCtlPtr->DumpBufferPtr->DataSource);
    if (NameLength < 0 || NameLength >= sizeof(DumpCtlPtr->FileWrite.FileName)
        || NameLength >= sizeof(DumpCtlPtr->DumpBufferPtr->DataSource))
    {
        CFE_TBL_SnapshotEventHandler(DumpCtlPtr, CFE_FS_FileWriteEvent_CREATE_ERROR, CFE_FS_FNAME_TOO_LONG, 0, 0, 0);
        return CFE_FS_FNAME_TOO_LONG;
    }
    DumpCtlPtr->FileWrite.GetData     = CFE_TBL_SnapshotDataGetter;
    DumpCtlPtr->FileWrite.OnEvent     = CFE_TBL_SnapshotEventHandler;
    DumpCtlPtr->FileWrite.WriteHeader = CFE_TBL_WriteSnapshotHeader;
    DumpCtlPtr->FileExisted           = (OS_stat(DumpCtlPtr->FileWrite.FileName, &FileStat) == OS_SUCCESS);
    DumpCtlPtr->NextOffset            = 0;
    DumpCtlPtr->WriteStatus           = CFE_TBL_ERR_ACCESS;
    DumpCtlPtr->State                 = CFE_TBL_DUMP_WRITING;

    Status = CFE_FS_BackgroundFileDumpRequest(&DumpCtlPtr->FileWrite);
    if (Status != CFE_SUCCESS)
    {
        DumpCtlPtr->State = CFE_TBL_DUMP_PERFORMED;
        if (Status != CFE_STATUS_REQUEST_ALREADY_PENDING)
        {
            CFE_TBL_SnapshotEventHandler(DumpCtlPtr, CFE_FS_FileWriteEvent_CREATE_ERROR, Status, 0, 0, 0);
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
        /* NOTE: In a "passthru" implementation, it is not possible for the encoding to fail,
         * this always succeeds.  In an implementation where it does do an encode, then it
         * is not the type of failure where trying again later might improve the result.
         * We need to just accept the error and move on to eventually free the dump ctrl buffer. */
        DumpCtrlPtr->EncodeStatus = CFE_TBL_EncodeOutputData(&Txn, SourceBufPtr, DumpCtrlPtr->DumpBufferPtr);

        /* Save the current time so that the header in the dump file can have the correct time */
        DumpCtrlPtr->DumpBufferPtr->FileTime = CFE_TIME_GetTime();

        /* Notify the Table Services Application that the dump buffer is ready to be written to a file */
        DumpCtrlPtr->State        = CFE_TBL_DUMP_PERFORMED;
        DumpCtrlPtr->SourceBuffId = CFE_TBL_LOADBUFFID_UNDEFINED;

        Status = CFE_SUCCESS;
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
CFE_Status_t
CFE_TBL_PrepareDumpSnapshotBuffer(CFE_TBL_TxnState_t *Txn, CFE_TBL_DumpCtrlId_t DumpCtrlId, const char *DumpFilename)
{
    CFE_Status_t           Status;
    CFE_TBL_LoadBuff_t    *WorkingBufferPtr;
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
CFE_Status_t CFE_TBL_AllocateDumpCtrlBlock(CFE_TBL_TxnState_t         *Txn,
                                           CFE_TBL_DumpCtrlId_t       *DumpCtrlIdOut,
                                           CFE_TBL_BufferSelect_Enum_t BufferSelect)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_LoadBuff_t    *SelectedBufferPtr;
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
            CFE_SB_MessageStringSet(DumpCtrlPtr->TableName,
                                    CFE_TBL_RegRecGetName(RegRecPtr),
                                    sizeof(DumpCtrlPtr->TableName),
                                    CFE_TBL_MAX_FULL_NAME_LEN);

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
bool CFE_TBL_SendDumpEventHelper(const CFE_TBL_TxnEvent_t *Event, CFE_TBL_TxnEventContext_t *Ctxt)
{
    const CFE_TBL_DumpContext_t *DumpCtxt;
    uint16                       EventType;
    char                         EventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];
    char                         CallerString[CFE_MISSION_MAX_API_LEN + 8];

    DumpCtxt = Ctxt->OperationDataPtr;

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
            snprintf(EventString,
                     sizeof(EventString),
                     "Cmd had illegal buffer parameter (0x%08X)",
                     (unsigned int)Event->EventData2);
            break;

        case CFE_TBL_DUMP_PENDING_ERR_EID:
            snprintf(EventString, sizeof(EventString), "A dump is already pending");
            break;

        case CFE_TBL_WRITE_CFE_HDR_ERR_EID:
            snprintf(EventString,
                     sizeof(EventString),
                     "Error writing cFE File Header, Status=0x%08X",
                     (unsigned int)Event->EventData1);
            break;

        case CFE_TBL_WRITE_TBL_HDR_ERR_EID:
            snprintf(EventString,
                     sizeof(EventString),
                     "Error writing cFE TBL Header, Status=%d",
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

    if (Ctxt->CallerName[0] == 0)
    {
        /* Empty name indicates it was initiated via ground command */
        snprintf(CallerString, sizeof(CallerString), "command");
    }
    else
    {
        /* non-Empty name means it was initiated by another app via API */
        snprintf(CallerString, sizeof(CallerString), "app=%s", Ctxt->CallerName);
    }

    /* Finally send the actual event by appending all the info we have */
    CFE_EVS_SendEventWithAppID(Event->EventId,
                               EventType,
                               CFE_TBL_Global.TableTaskAppId,
                               "%s by %s,table=%s,file=%s:%s",
                               Ctxt->Operation,
                               CallerString,
                               DumpCtxt->RequestedTableName,
                               DumpCtxt->FileName,
                               EventString);

    return true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_SendTableDumpEvents(CFE_TBL_TxnState_t *Txn, const char *FileName, const char *RequestedTableName)
{
    CFE_TBL_DumpContext_t Ctxt;

    memset(&Ctxt, 0, sizeof(Ctxt));

    if (FileName != NULL)
    {
        Ctxt.FileName = FileName;
    }
    else
    {
        /* do not leave it null/blank */
        Ctxt.FileName = "[none]";
    }

    if (RequestedTableName != NULL)
    {
        Ctxt.RequestedTableName = RequestedTableName;
    }
    else
    {
        /* do not leave it null/blank */
        Ctxt.RequestedTableName = "[unknown]";
    }

    CFE_TBL_SendTransactionEvents(Txn, "Dump", CFE_TBL_SendDumpEventHelper, &Ctxt);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TableDumpExecuteBackground(void)
{
    uint32                 i;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;
    CFE_TBL_TxnState_t     Txn;
    CFE_Status_t           Status;

    CFE_TBL_TxnInit(&Txn, false);
    CFE_TBL_TxnLockRegistry(&Txn);
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[i];
        if (!CFE_TBL_DumpCtrlBlockIsUsed(DumpCtrlPtr))
        {
            continue;
        }

        if (DumpCtrlPtr->State == CFE_TBL_DUMP_WRITING)
        {
            if (CFE_FS_BackgroundFileDumpIsPending(&DumpCtrlPtr->FileWrite))
            {
                continue;
            }
            /* FS has released the metadata. Update HK only in the TBL task. */
            if (DumpCtrlPtr->WriteStatus == CFE_SUCCESS)
            {
                CFE_SB_MessageStringSet(CFE_TBL_Global.HkPacket.Payload.LastFileDumped,
                                        DumpCtrlPtr->FileWrite.FileName,
                                        sizeof(CFE_TBL_Global.HkPacket.Payload.LastFileDumped),
                                        -1);
            }
        }
        else if (DumpCtrlPtr->State == CFE_TBL_DUMP_PERFORMED)
        {
            if (DumpCtrlPtr->EncodeStatus == CFE_SUCCESS)
            {
                Status = CFE_TBL_WriteSnapshotToFile(DumpCtrlPtr);
                if (Status == CFE_SUCCESS || Status == CFE_STATUS_REQUEST_ALREADY_PENDING)
                {
                    /* Retain accepted requests, and retry a full queue on the next HK
                     * cycle. */
                    continue;
                }
            }
            else
            {
                CFE_EVS_SendEvent(CFE_TBL_DUMP_ENCODE_FAIL_EID,
                                  CFE_EVS_EventType_ERROR,
                                  "Table Dump encoding failed, status=%d",
                                  (int)DumpCtrlPtr->EncodeStatus);
            }
        }
        else
        {
            continue;
        }

        CFE_TBL_LoadBuffSetFree(DumpCtrlPtr->DumpBufferPtr);
        CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);
    }
    CFE_TBL_TxnFinish(&Txn);
}
