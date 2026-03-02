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
 * Implementation of table services table load (read from file) methods
 */

/*
** Required header files...
*/
#include "cfe_tbl_module_all.h"
#include "cfe_tbl_load.h"
#include "cfe_tbl_codec.h"
#include "cfe_config.h"

#include <stdio.h>
#include <string.h>

typedef struct CFE_TBL_LoadContext
{
    const char *Operation;
    const char *Tablename;
    const char *LoadFilename;
    const char *CallerName;

    const CFE_TBL_CombinedFileHdr_t *FileHdr;
    const CFE_TBL_RegistryRec_t *    RegRecPtr;
} CFE_TBL_LoadContext_t;

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnOpenTableLoadFile(CFE_TBL_TxnState_t *Txn, const char *Filename, osal_id_t *FileDescOut,
                                          CFE_TBL_CombinedFileHdr_t *TblFileHeader)
{
    CFE_Status_t Status;
    int32        OsStatus;
    osal_id_t    FileDescriptor;

    FileDescriptor = OS_OBJECT_ID_UNDEFINED;

    /* Try to open the specified table file */
    OsStatus = OS_OpenCreate(&FileDescriptor, Filename, OS_FILE_FLAG_NONE, OS_READ_ONLY);
    if (OsStatus == OS_FS_ERR_PATH_TOO_LONG)
    {
        /* Traditionally there was a dedicated EID for long file names, keeping it */
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_FILENAME_LONG_ERR_EID, OsStatus, 0);
        Status = CFE_TBL_ERR_FILENAME_TOO_LONG;
    }
    else if (OsStatus != OS_SUCCESS)
    {
        /* Catch any other OSAL error, probably related to file existence or access */
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_ACCESS_ERR_EID, OsStatus, 0);
        Status = CFE_TBL_ERR_ACCESS;
    }
    else
    {
        /* CFE_TBL_ReadHeaders() - if successful, it leaves the file desc positioned at the table data */
        Status = CFE_TBL_ReadHeaders(Txn, FileDescriptor, TblFileHeader);

        /* If unsuccessful, then close the file descriptor */
        if (Status != CFE_SUCCESS)
        {
            OS_close(FileDescriptor);
            FileDescriptor = OS_OBJECT_ID_UNDEFINED;
        }
    }

    /* Always export the file descriptor.  It will be undefined on failure,
     * or a valid file descriptor on success, positioned at the file data */
    *FileDescOut = FileDescriptor;

    return Status;
}

/*----------------------------------------------------------------
 *
 * Internal helper function
 * Checks a user-supplied ID against the configured acceptable list
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_ValidateTableHeaderId(CFE_ConfigId_t ConfigId, uint32 RefId)
{
    CFE_Config_ArrayValue_t AcceptList;
    const uint32 *          ListPtr;
    uint32                  Idx;

    AcceptList = CFE_Config_GetArrayValue(ConfigId);

    /* Verify ID contained in table file header [optional] */
    ListPtr = AcceptList.ElementPtr;
    for (Idx = 0; Idx < AcceptList.NumElements; ++Idx)
    {
        if (RefId == *ListPtr)
        {
            break;
        }

        ++ListPtr;
    }

    return (AcceptList.NumElements == 0 || Idx < AcceptList.NumElements);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ReadHeaders(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor,
                                 CFE_TBL_CombinedFileHdr_t *FileHeader)
{
    CFE_Status_t Status;

    /* Once the file is open, read the headers to determine the target Table */
    Status = CFE_FS_ReadHeader(&FileHeader->Std, FileDescriptor);

    /* Verify successful read of standard cFE File Header */
    if (Status != sizeof(CFE_FS_Header_t))
    {
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_STD_HDR_ERR_EID, Status, sizeof(CFE_FS_Header_t));
        Status = CFE_TBL_ERR_NO_STD_HEADER;
    }
    else if (FileHeader->Std.ContentType != CFE_FS_FILE_CONTENT_ID)
    {
        Status = CFE_TBL_ERR_BAD_CONTENT_ID;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_TYPE_ERR_EID, Status, FileHeader->Std.ContentType);
    }
    else if (FileHeader->Std.SubType != CFE_FS_SubType_TBL_IMG)
    {
        Status = CFE_TBL_ERR_BAD_SUBTYPE_ID;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_SUBTYPE_ERR_EID, Status, FileHeader->Std.SubType);
    }
    else if (!CFE_TBL_ValidateTableHeaderId(CFE_CONFIGID_PLATFORM_TBL_VALID_SCID, FileHeader->Std.SpacecraftID))
    {
        /* Verify Spacecraft ID contained in table file header [optional] */
        Status = CFE_TBL_ERR_BAD_SPACECRAFT_ID;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_SPACECRAFT_ID_ERR_EID, Status, FileHeader->Std.SpacecraftID);
    }
    else if (!CFE_TBL_ValidateTableHeaderId(CFE_CONFIGID_PLATFORM_TBL_VALID_PRID, FileHeader->Std.ProcessorID))
    {
        /* Verify Processor ID contained in table file header [optional] */
        Status = CFE_TBL_ERR_BAD_PROCESSOR_ID;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_PROCESSOR_ID_ERR_EID, Status, FileHeader->Std.ProcessorID);
    }
    else
    {
        Status = CFE_TBL_DecodeHeadersFromFile(Txn, FileDescriptor, &FileHeader->Tbl);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateFileIsLoadable(CFE_TBL_TxnState_t *Txn, const CFE_TBL_File_Hdr_t *TblFileHeader)
{
    CFE_Status_t           Status;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    /*
     * Note, this function only needs to validate the file itself.  The config+state of the table (RegRecPtr)
     * has already been checked and it is already known to be in a state that permits loading in general.
     */

    RegRecPtr = CFE_TBL_TxnRegRec(Txn);
    if (RegRecPtr == NULL)
    {
        /* it should not be possible to get here in FSW, but UT can trigger this, so handle it */
        Status = CFE_TBL_ERR_INVALID_HANDLE;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_NO_SUCH_TABLE_ERR_EID, Status, 0);
    }
    else if (strcmp(CFE_TBL_RegRecGetName(RegRecPtr), TblFileHeader->TableName) != 0)
    {
        Status = CFE_TBL_ERR_FILE_FOR_WRONG_TABLE;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID, Status, 0);
    }
    else if (TblFileHeader->NumBytes == 0)
    {
        /* Loading an empty file is not valid; this suggests some form of data error / missing header */
        Status = CFE_TBL_ERR_NO_TBL_HEADER;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID, Status, 0);
    }
    else if (TblFileHeader->Offset > 0 && !CFE_TBL_RegRecIsTableLoaded(RegRecPtr))
    {
        /* Partial loads can only occur on previously loaded tables. */
        Status = CFE_TBL_ERR_PARTIAL_LOAD;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_PARTIAL_LOAD_ERR_EID, Status, 0);
    }
    else
    {
        Status = CFE_TBL_ValidateCodecLoadSize(Txn, TblFileHeader);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_LoadContentFromFile(CFE_TBL_TxnState_t *Txn, osal_id_t FileDescriptor, size_t Offset,
                                         size_t NumBytes)
{
    size_t                 ActualBytes;
    uint8                  ExtraByte;
    uint8 *                DestPtr;
    int32                  OsStatus;
    CFE_Status_t           Status;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_LoadBuff_t *   WorkingBufferPtr;
    size_t                 LoadTailSize;

    RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    WorkingBufferPtr = CFE_TBL_AcquireCodecBuffer(RegRecPtr);
    if (WorkingBufferPtr == NULL)
    {
        Status = CFE_TBL_ERR_NO_BUFFER_AVAIL;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_NO_WORK_BUFFERS_ERR_EID, Status, 0);
        return Status;
    }

    /* Confirm that the data about to be loaded will fit */
    LoadTailSize = Offset + NumBytes;
    if (LoadTailSize > CFE_TBL_LoadBuffGetAllocSize(WorkingBufferPtr))
    {
        Status = CFE_TBL_ERR_FILE_TOO_LARGE;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_TOO_BIG_ERR_EID, LoadTailSize,
                            CFE_TBL_LoadBuffGetAllocSize(WorkingBufferPtr));
    }
    else
    {
        /* Find where this goes in the memory buffer */
        DestPtr = CFE_TBL_LoadBuffGetWritePointer(WorkingBufferPtr);
        DestPtr += Offset;

        /* Now actually read the data, and confirm that the end of the file data lines up as expected */
        OsStatus = OS_read(FileDescriptor, DestPtr, NumBytes);
        if (OsStatus >= OS_SUCCESS)
        {
            ActualBytes = OsStatus; /* status code conversion (size) */
        }
        else
        {
            ActualBytes = 0;
        }

        if (ActualBytes != NumBytes)
        {
            Status = CFE_TBL_ERR_LOAD_INCOMPLETE;
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_INCOMPLETE_ERR_EID, OsStatus, NumBytes);
        }
        else
        {
            /* Check to see if the file is too large (ie - more data than header claims) */
            OsStatus = OS_read(FileDescriptor, &ExtraByte, 1);
            if (OsStatus >= OS_SUCCESS)
            {
                ActualBytes = OsStatus; /* status code conversion (size) */
            }
            else
            {
                ActualBytes = 0;
            }

            /* If successfully read another byte, then file must have too much data */
            if (ActualBytes == 1)
            {
                Status = CFE_TBL_ERR_FILE_TOO_LARGE;
                CFE_TBL_TxnAddEvent(Txn, CFE_TBL_FILE_TOO_BIG_ERR_EID, OsStatus, 0);
            }
            else
            {
                CFE_TBL_LoadBuffSetContentSize(WorkingBufferPtr, LoadTailSize);
                Status = CFE_TBL_DecodeInputData(Txn, WorkingBufferPtr, CFE_TBL_GetLoadInProgressBuffer(RegRecPtr));
            }
        }
    }

    CFE_TBL_ReleaseCodecBuffer(WorkingBufferPtr);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_SetMetaDataFromFileHeader(CFE_TBL_TxnState_t *Txn, const char *Filename, const CFE_FS_Header_t *StdHeader)
{
    CFE_TBL_LoadBuff_t *   WorkingBufferPtr;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    RegRecPtr        = CFE_TBL_TxnRegRec(Txn);
    WorkingBufferPtr = CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);

    /* This function should not be called unless there is a working buffer, to do so
     * is a calling error.  This verification avoids warnings from static analysis */
    if (WorkingBufferPtr != NULL)
    {
        strncpy(WorkingBufferPtr->DataSource, Filename, sizeof(WorkingBufferPtr->DataSource) - 1);
        WorkingBufferPtr->DataSource[sizeof(WorkingBufferPtr->DataSource) - 1] = '\0';

        /* Save file creation time for later storage into Registry */
        WorkingBufferPtr->FileTime.Seconds    = StdHeader->TimeSeconds;
        WorkingBufferPtr->FileTime.Subseconds = StdHeader->TimeSubSeconds;

        /* Compute the CRC on the specified table buffer */
        CFE_TBL_LoadBuffRecomputeCRC(WorkingBufferPtr);

        /* Initialize validation flag with true if no Validation Function is required to be
         * called */
        WorkingBufferPtr->Validated = (CFE_TBL_RegRecGetValidationFunc(RegRecPtr) == NULL);
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function for sending events
 * This function handles general events that are _not_ related to a file header
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_SendLoadBasicEventHelper(const CFE_TBL_TxnEvent_t *Event, void *Arg)
{
    const CFE_TBL_LoadContext_t *Ctxt;
    uint16                       EventType;
    char                         EventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    Ctxt = Arg;

    /* The majority of the events are errors, but this can be reset later to demote to info/debug */
    EventType      = CFE_EVS_EventType_ERROR;
    EventString[0] = 0;

    switch (Event->EventId)
    {
        case CFE_TBL_NO_SUCH_TABLE_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Unable to locate in Table Registry");
            break;
        }
        case CFE_TBL_HANDLE_ACCESS_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "No access to Tbl Handle");
            break;
        }
        case CFE_TBL_NO_WORK_BUFFERS_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "No working buffers available, stat=0x%lx",
                     (unsigned long)Event->EventData1);
            break;
        }
        case CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Attempted to load DUMP-ONLY table");
            break;
        }
        case CFE_TBL_LOAD_IN_PROGRESS_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Load already in progress");
            break;
        }
        case CFE_TBL_LOAD_TYPE_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Attempted to load from illegal source type=%d",
                     (int)Event->EventData2);
            break;
        }
        case CFE_TBL_LOAD_VAL_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Validation func return code invalid (Stat=%lx)",
                     (unsigned long)Event->EventData1);
            break;
        }
        case CFE_TBL_VALIDATION_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Validation func reports table invalid (Stat=%lx)",
                     (unsigned long)Event->EventData1);
            break;
        }
    }

    if (EventString[0] == 0)
    {
        return false;
    }

    /* Finally send the actual event by appending all the info we have */
    CFE_EVS_SendEventWithAppID(Event->EventId, EventType, CFE_TBL_Global.TableTaskAppId, "%s,app=%s,tbl=%s:%s",
                               Ctxt->Operation, Ctxt->CallerName, Ctxt->Tablename, EventString);

    return true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_SendTableLoadEvents(CFE_TBL_TxnState_t *Txn)
{
    CFE_TBL_LoadContext_t Ctxt;

    memset(&Ctxt, 0, sizeof(Ctxt));

    Ctxt.Operation  = "Load";
    Ctxt.CallerName = CFE_TBL_TxnAppNameCaller(Txn);
    Ctxt.RegRecPtr  = CFE_TBL_TxnRegRec(Txn);

    if (Ctxt.RegRecPtr != NULL)
    {
        Ctxt.Tablename = CFE_TBL_RegRecGetName(Ctxt.RegRecPtr);
    }
    else
    {
        /* do not leave it null/blank */
        Ctxt.Tablename = "[unknown]";
    }

    CFE_TBL_TxnProcessEvents(Txn, CFE_TBL_SendLoadBasicEventHelper, &Ctxt);
    CFE_TBL_TxnClearEvents(Txn);
}

/*----------------------------------------------------------------
 *
 * Local helper function for sending events
 * This function handles events that may be related to a file header
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_SendLoadFileEventHelper(const CFE_TBL_TxnEvent_t *Event, void *Arg)
{
    const CFE_TBL_LoadContext_t *Ctxt;
    uint16                       EventType;
    char                         EventString[CFE_MISSION_EVS_MAX_MESSAGE_LENGTH];

    /* A file load can generate some of the basic events too */
    if (CFE_TBL_SendLoadBasicEventHelper(Event, Arg))
    {
        /* Handled -- Nothing else to do here */
        return true;
    }

    Ctxt = Arg;

    /* The majority of the events are errors, but this can be reset later to demote to info/debug */
    EventType      = CFE_EVS_EventType_ERROR;
    EventString[0] = 0;

    switch (Event->EventId)
    {
        case CFE_TBL_FILE_TOO_BIG_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "File has more data than Tbl Hdr indicates (%d)",
                     (int)Ctxt->FileHdr->Tbl.NumBytes);
            break;
        }
        case CFE_TBL_FILE_INCOMPLETE_ERR_EID:
        {
            /* A file whose header claims has 'x' amount of data but it only has 'y' */
            /* is considered a fatal error during a load process                     */
            snprintf(EventString, sizeof(EventString), "Incomplete load into working buffer");
            break;
        }
        case CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "File for wrong table '%s'", Ctxt->FileHdr->Tbl.TableName);
            break;
        }
        case CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Cannot load %u at off %u",
                     (unsigned int)Ctxt->FileHdr->Tbl.NumBytes, (unsigned int)Ctxt->FileHdr->Tbl.Offset);
            break;
        }
        case CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Table header indicates no data in file");
            break;
        }
        case CFE_TBL_PARTIAL_LOAD_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "partial load for uninitialized table");
            break;
        }
        case CFE_TBL_FILE_STD_HDR_ERR_EID:
        {
            /* unsuccessful read of standard cFE File Header */
            snprintf(EventString, sizeof(EventString), "Unable to read std header, Status = 0x%08x",
                     (unsigned int)Event->EventData1);
            break;
        }
        case CFE_TBL_FILE_TBL_HDR_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Unable to read tbl header, Status = 0x%08x",
                     (unsigned int)Event->EventData1);
            break;
        }
        case CFE_TBL_FILE_TYPE_ERR_EID:
        {
            /* file type is not a cFE compatible file */
            snprintf(EventString, sizeof(EventString), "not a cFE file type, ContentType = 0x%08x",
                     (unsigned int)Ctxt->FileHdr->Std.ContentType);
            break;
        }
        case CFE_TBL_FILE_SUBTYPE_ERR_EID:
        {
            /* The SubType is not a Table Image File */
            snprintf(EventString, sizeof(EventString), "File subtype is wrong. Subtype = 0x%08x",
                     (unsigned int)Ctxt->FileHdr->Std.SubType);

            break;
        }
        case CFE_TBL_SPACECRAFT_ID_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Invalid Spacecraft ID (0x%08X)",
                     (unsigned int)Ctxt->FileHdr->Std.SpacecraftID);
            break;
        }

        case CFE_TBL_PROCESSOR_ID_ERR_EID:
        {
            snprintf(EventString, sizeof(EventString), "Invalid Processor ID (0x%08X)",
                     (unsigned int)Ctxt->FileHdr->Std.ProcessorID);
            break;
        }

        case CFE_TBL_FILE_ACCESS_ERR_EID:
        {
            /* Error opening specified file */
            snprintf(EventString, sizeof(EventString), "Unable to open, Status = %ld", (long)Event->EventData1);
            break;
        }

        case CFE_TBL_FILE_LOADED_INF_EID:
        {
            /* This one is success, not an error */
            EventType = CFE_EVS_EventType_INFORMATION;
            snprintf(EventString, sizeof(EventString), "Successful load into '%s' working buffer", Ctxt->Tablename);
            break;
        }
    }

    if (EventString[0] == 0)
    {
        return false;
    }

    /* Finally send the actual event by appending all the info we have */
    CFE_EVS_SendEventWithAppID(Event->EventId, EventType, CFE_TBL_Global.TableTaskAppId, "%s,app=%s,file=%s,tbl=%s:%s",
                               Ctxt->Operation, Ctxt->CallerName, Ctxt->LoadFilename, Ctxt->Tablename, EventString);

    return true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_SendTableLoadFileEvents(CFE_TBL_TxnState_t *Txn, const char *Filename, const CFE_TBL_CombinedFileHdr_t *FileHdr)
{
    CFE_TBL_LoadContext_t Ctxt;

    memset(&Ctxt, 0, sizeof(Ctxt));

    Ctxt.Operation  = "LoadFile";
    Ctxt.FileHdr    = FileHdr;
    Ctxt.CallerName = CFE_TBL_TxnAppNameCaller(Txn);
    Ctxt.RegRecPtr  = CFE_TBL_TxnRegRec(Txn);

    /* In command context, the table name comes from the file */
    if (Ctxt.FileHdr->Tbl.TableName[0])
    {
        Ctxt.Tablename = Ctxt.FileHdr->Tbl.TableName;
    }
    else
    {
        /* do not leave it null/blank */
        Ctxt.Tablename = "[unknown]";
    }

    if (Filename != NULL)
    {
        Ctxt.LoadFilename = Filename;
    }
    else
    {
        Ctxt.LoadFilename = "[none]";
    }

    CFE_TBL_TxnProcessEvents(Txn, CFE_TBL_SendLoadFileEventHelper, &Ctxt);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnLoadFromFile(CFE_TBL_TxnState_t *Txn, const char *Filename)
{
    CFE_Status_t              Status;
    CFE_TBL_CombinedFileHdr_t Header;
    osal_id_t                 FileDescriptor;

    /* just in case, clear out the header */
    memset(&Header, 0, sizeof(Header));

    /* open the file and sanity check it.  If successful, this will output a valid
     * file descriptor (still open) that is positioned at the file data, along with
     * populating the fields within Header.  If unsuccessful, nothing else to do. */
    Status = CFE_TBL_TxnOpenTableLoadFile(Txn, Filename, &FileDescriptor, &Header);
    if (Status == CFE_SUCCESS)
    {
        /* NOTE: the FileDescriptor is valid and must be closed before leaving */

        /* check the sanity of the headers against the registry record */
        Status = CFE_TBL_ValidateFileIsLoadable(Txn, &Header.Tbl);

        if (Status == CFE_SUCCESS)
        {
            Status = CFE_TBL_LoadContentFromFile(Txn, FileDescriptor, Header.Tbl.Offset, Header.Tbl.NumBytes);
        }

        /* Done with the file now -- must always close the file regardless of what happened */
        OS_close(FileDescriptor);
    }

    /* If all the above worked out, then set the meta info in the load buffer */
    if (Status == CFE_SUCCESS)
    {
        CFE_TBL_SetMetaDataFromFileHeader(Txn, Filename, &Header.Std);

        /* Determine the final status code to return to the caller.  This may be an "alternate success" code,
         * in the case that the file was not complete.  These should be removed/deprecated because they are
         * confusing, but preserving for now for backward compatibility.  */
        Status = CFE_TBL_CodecGetFinalStatus(Txn, &Header.Tbl);
    }

    /* Send any events associated with this table load */
    /* Note that many of these require the Header context, which is local,
     * so send events before it goes out of scope */
    CFE_TBL_SendTableLoadFileEvents(Txn, Filename, &Header);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_TxnLoadFromSourceAddr(CFE_TBL_TxnState_t *Txn, const void *Address)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    CFE_TBL_LoadBuff_t *   WorkingBufferPtr;

    RegRecPtr        = CFE_TBL_TxnRegRec(Txn);
    WorkingBufferPtr = CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);

    /*
     * Note, on dump only tables, the buffer pointer isn't set until the first load,
     * whereas on normal (non-dump only) tables, the buffer pointer is always set as
     * part of registration and thus can never be NULL at this point (in fact, checking
     * for NULL would result in a non-reachable branch in the test cases).
     */
    if (CFE_TBL_RegRecGetConfig(RegRecPtr)->DumpOnly)
    {
        /* for dump-only, the data is not copied - just use the user-supplied addr directly */
        CFE_TBL_LoadBuffSetExternalBlock(WorkingBufferPtr, (void *)Address, CFE_TBL_RegRecGetSize(RegRecPtr));
    }
    else
    {
        /* When the source is a block of memory, it is assumed to be a complete load */
        CFE_TBL_LoadBuffCopyData(WorkingBufferPtr, Address, CFE_TBL_RegRecGetSize(RegRecPtr));
    }

    /* If success, then initialize the rest of the pending buffer info */
    snprintf(WorkingBufferPtr->DataSource, sizeof(WorkingBufferPtr->DataSource), "Addr 0x%08lX",
             (unsigned long)Address);

    WorkingBufferPtr->FileTime = CFE_TIME_ZERO_VALUE;

    /* Compute the CRC on the specified table buffer */
    CFE_TBL_LoadBuffRecomputeCRC(WorkingBufferPtr);

    return CFE_SUCCESS;
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
    CFE_TBL_DiscardWorkingBuffer(RegRecPtr);

    CFE_EVS_SendEvent(CFE_TBL_LOAD_ABORT_INF_EID, CFE_EVS_EventType_INFORMATION, "Table Load Aborted for '%s'",
                      CFE_TBL_RegRecGetName(RegRecPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateLoadRequest(CFE_TBL_TxnState_t *Txn, CFE_TBL_SrcEnum_t SrcType)
{
    CFE_Status_t           Status;
    CFE_TBL_RegistryRec_t *RegRecPtr;

    Status    = CFE_SUCCESS;
    RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    /* Loads by an Application are not allowed if a table load is already in progress */
    if (CFE_TBL_RegRecIsLoadInProgress(RegRecPtr))
    {
        Status = CFE_TBL_ERR_LOAD_IN_PROGRESS;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_IN_PROGRESS_ERR_EID, Status, 0);
    }
    else if (CFE_TBL_RegRecGetConfig(RegRecPtr)->DumpOnly)
    {
        /* Dump-only tables have additional restrictions */
        if (!CFE_TBL_RegRecGetConfig(RegRecPtr)->UserDefAddr || CFE_TBL_RegRecIsTableLoaded(RegRecPtr))
        {
            Status = CFE_TBL_ERR_DUMP_ONLY;
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID, Status, 0);
        }
        else if (SrcType != CFE_TBL_SRC_ADDRESS)
        {
            /* dump only tables can only be loaded from an address, not a file */
            Status = CFE_TBL_ERR_ILLEGAL_SRC_TYPE;
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_TYPE_ERR_EID, Status, SrcType);
        }
    }
    else if (SrcType != CFE_TBL_SRC_ADDRESS && SrcType != CFE_TBL_SRC_FILE)
    {
        /* For normal loads the source must be an address or a file */
        Status = CFE_TBL_ERR_ILLEGAL_SRC_TYPE;
        CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_TYPE_ERR_EID, Status, SrcType);
    }

    if (Status == CFE_SUCCESS)
    {
        /* Obtain a working buffer (either the table's dedicated buffer or one of the shared buffers) */
        /* Note -- this is just to reserve the buffer right now.  It stores the ID of the reserved buffer
         * as the "load in progress" buffer -- we do not need a direct pointer to it here, only confirm
         * that it has been reserved so when a future step needs it, it is ready to go.
         */
        Status = CFE_TBL_GetWorkingBuffer(NULL, RegRecPtr);
        if (Status != CFE_SUCCESS)
        {
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_NO_WORK_BUFFERS_ERR_EID, Status, 0);
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
CFE_Status_t CFE_TBL_ValidateLoadInProgress(CFE_TBL_TxnState_t *Txn, CFE_Status_t LoadStatus)
{
    CFE_Status_t              Status;
    CFE_TBL_CallbackFuncPtr_t ValidationFunc;
    CFE_TBL_RegistryRec_t *   RegRecPtr;
    CFE_TBL_LoadBuff_t *      WorkingBufferPtr;

    RegRecPtr        = CFE_TBL_TxnRegRec(Txn);
    WorkingBufferPtr = CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);
    ValidationFunc   = CFE_TBL_RegRecGetValidationFunc(RegRecPtr);

    /* If the data was successfully loaded, then validate its contents */
    if (WorkingBufferPtr == NULL)
    {
        /* Not expected -- this is a problem */
        LoadStatus = CFE_TBL_ERR_NEVER_LOADED;
    }
    else if (ValidationFunc != NULL)
    {
        Status = ValidationFunc((void *)CFE_TBL_LoadBuffGetReadPointer(WorkingBufferPtr));

        /*
         * Anything other than CFE_SUCCESS (0) is considered a failure to validate
         *
         * It is not entirely clear why there are two different EIDs here,
         * depending on whether the validation function returned a positive or negative value.
         * This is historical behavior and it has been preserved.
         */
        if (Status > CFE_SUCCESS)
        {
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_LOAD_VAL_ERR_EID, Status, 0);
            Status = CFE_STATUS_VALIDATION_FAILURE;
        }

        if (Status < CFE_SUCCESS)
        {
            CFE_TBL_TxnAddEvent(Txn, CFE_TBL_VALIDATION_ERR_EID, Status, 0);

            /* Zero out the buffer to remove any bad data */
            CFE_TBL_LoadBuffClearData(WorkingBufferPtr);
            LoadStatus                  = Status;
            WorkingBufferPtr->Validated = false;
        }
        else
        {
            WorkingBufferPtr->Validated = true;
        }
    }
    else
    {
        /* tables without a validation function are considered valid */
        WorkingBufferPtr->Validated = true;
    }

    return LoadStatus;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_LoadFinish(CFE_TBL_TxnState_t *Txn, CFE_Status_t LoadStatus)
{
    CFE_Status_t           Status;
    CFE_TBL_RegistryRec_t *RegRecPtr;
    bool                   FirstTime;

    RegRecPtr = CFE_TBL_TxnRegRec(Txn);

    if (RegRecPtr != NULL)
    {
        /*
         * Note that the load status can be an INFO code, which is not CFE_SUCCESS but
         * still is considered successful (these are positive values)
         */
        if (LoadStatus < CFE_SUCCESS)
        {
            /* If something failed during the load, then drop the working buffer */
            CFE_TBL_DiscardWorkingBuffer(RegRecPtr);
        }
        else
        {
            /* Check if this is the initial table load */
            FirstTime = !CFE_TBL_RegRecIsTableLoaded(RegRecPtr);

            /* Note - this call to will also discard the working buffer if it is successful */
            Status = CFE_TBL_UpdateInternal(CFE_TBL_TxnHandle(Txn), RegRecPtr, CFE_TBL_TxnAccDesc(Txn));

            if (Status != CFE_SUCCESS)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_ERR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_TBL_Global.TableTaskAppId, "%s: Failed to update '%s' (Stat=%u)",
                                           CFE_TBL_TxnAppNameCaller(Txn), CFE_TBL_RegRecGetName(RegRecPtr),
                                           (unsigned int)Status);

                /* Something went wrong - Use this status code */
                LoadStatus = Status;
            }
            else
            {
                /* The first time a table is loaded, the event message is DEBUG */
                /* to help eliminate a flood of events during a startup         */
                CFE_EVS_SendEventWithAppID(
                    CFE_TBL_LOAD_SUCCESS_INF_EID, FirstTime ? CFE_EVS_EventType_DEBUG : CFE_EVS_EventType_INFORMATION,
                    CFE_TBL_Global.TableTaskAppId, "Successfully loaded '%s' from '%s'",
                    CFE_TBL_RegRecGetName(RegRecPtr), CFE_TBL_RegRecGetLastFileLoaded(RegRecPtr));

                /* Save the index of the table for housekeeping telemetry */
                CFE_TBL_Global.LastTblUpdated = CFE_TBL_TxnRegId(Txn);
            }
        }
    }

    return LoadStatus;
}
