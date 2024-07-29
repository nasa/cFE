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
** File: cfe_fs_api.c
**
** Purpose:  cFE File Services (FS) library API source file
**
** Author:   S.Walling/Microtel
**
** Notes:
**
*/

/*
** Required header files...
*/
#include "cfe_fs_module_all.h"

#include <string.h>

/* The target config allows refs into global CONFIGDATA object(s) */
#include "target_config.h"

/*
 * Fixed default file system extensions (not platform dependent)
 */
const char CFE_FS_DEFAULT_SCRIPT_EXTENSION[]    = ".scr";
const char CFE_FS_DEFAULT_TEMP_FILE_EXTENSION[] = ".tmp";
const char CFE_FS_DEFAULT_LOG_FILE_EXTENSION[]  = ".log";

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
const char *CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_t FileCategory)
{
    const char *Result;

    switch (FileCategory)
    {
        case CFE_FS_FileCategory_SCRIPT:
        case CFE_FS_FileCategory_DYNAMIC_MODULE:
            /* scripts and app/lib modules reside in the non-volatile/CF mount by default */
            Result = GLOBAL_CFE_CONFIGDATA.NonvolMountPoint;
            break;
        case CFE_FS_FileCategory_TEMP:
        case CFE_FS_FileCategory_BINARY_DATA_DUMP:
        case CFE_FS_FileCategory_TEXT_LOG:
            /* temporary and data dump files are put in the RAM DISK mount by default */
            Result = GLOBAL_CFE_CONFIGDATA.RamdiskMountPoint;
            break;
        default:
            Result = NULL; /* Should not be used */
            break;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
const char *CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_t FileCategory)
{
    const char *Result;

    switch (FileCategory)
    {
        case CFE_FS_FileCategory_SCRIPT:
            Result = CFE_FS_DEFAULT_SCRIPT_EXTENSION;
            break;
        case CFE_FS_FileCategory_DYNAMIC_MODULE:
            /* app/lib modules use a platform-specific extension, and the
             * default is derived from the build system */
            Result = GLOBAL_CONFIGDATA.Default_ModuleExtension;
            break;
        case CFE_FS_FileCategory_TEMP:
            Result = CFE_FS_DEFAULT_TEMP_FILE_EXTENSION;
            break;
        case CFE_FS_FileCategory_TEXT_LOG:
            Result = CFE_FS_DEFAULT_LOG_FILE_EXTENSION;
            break;
        default:
            Result = NULL; /* Should not be used */
            break;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, osal_id_t FileDes)
{
    int32 OsStatus;
    int32 Result;
    int32 EndianCheck = 0x01020304;

    if (Hdr == NULL)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    /*
    ** Ensure that we are at the start of the file...
    */
    OsStatus = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (OsStatus >= OS_SUCCESS)
    {
        /*
        ** Read header structure into callers buffer...
        */
        OsStatus = OS_read(FileDes, Hdr, sizeof(CFE_FS_Header_t));

        /* Check if the read was successful */
        if (OsStatus != sizeof(CFE_FS_Header_t)) 
        {
            return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }

        /* Determine if this processor is a little endian processor */
        /* cppcheck-suppress knownConditionTrueFalse */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this is a little endian processor, then convert the header data structure from */
            /* its standard big-endian format into a little endian format to ease user access    */
            CFE_FS_ByteSwapCFEHeader(Hdr);
        }

        Result = (long)OsStatus;
    }
    else
    {
        Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType)
{
    if (Hdr == NULL || Description == NULL)
    {
        CFE_ES_WriteToSysLog("%s: Failed invalid arguments\n", __func__);
    }
    else
    {
        memset(Hdr, 0, sizeof(CFE_FS_Header_t));
        strncpy((char *)Hdr->Description, Description, sizeof(Hdr->Description) - 1);
        Hdr->SubType = SubType;
    }
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_FS_WriteHeader(osal_id_t FileDes, CFE_FS_Header_t *Hdr)
{
    CFE_TIME_SysTime_t Time;
    int32              OsStatus;
    int32              Result;
    int32              EndianCheck = 0x01020304;
    CFE_ES_AppId_t     AppID;

    if (Hdr == NULL)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    /*
    ** Ensure that we are at the start of the file...
    */
    OsStatus = OS_lseek(FileDes, 0, OS_SEEK_SET);

    if (OsStatus >= OS_SUCCESS)
    {
        /*
        ** Fill in the ID fields...
        */
        Hdr->SpacecraftID = CFE_PSP_GetSpacecraftId();
        Hdr->ProcessorID  = CFE_PSP_GetProcessorId();
        CFE_ES_GetAppID(&AppID);
        Hdr->ApplicationID = CFE_RESOURCEID_TO_ULONG(AppID);

        /* Fill in length field */

        Hdr->Length = sizeof(CFE_FS_Header_t);

        /* put the header, 'cfe1' in hex, in to the content type */
        Hdr->ContentType = CFE_FS_FILE_CONTENT_ID;

        /*
        ** Fill in the timestamp fields...
        */
        Time                = CFE_TIME_GetTime();
        Hdr->TimeSeconds    = Time.Seconds;
        Hdr->TimeSubSeconds = Time.Subseconds;

        /*
        ** Determine if this is a little endian processor
        */
        /* cppcheck-suppress knownConditionTrueFalse */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this is a little endian processor, then convert the header data structure from */
            /* the native little endian format to the required CFE standard big endian format    */
            CFE_FS_ByteSwapCFEHeader(Hdr);
        }

        /*
        ** Write header structure from callers buffer...
        */
        OsStatus = OS_write(FileDes, Hdr, sizeof(CFE_FS_Header_t));

        /*
        ** Determine if this is a little endian processor
        */
        /* cppcheck-suppress knownConditionTrueFalse */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this is a little endian processor, then convert the header data structure back */
            /* from the required CFE standard big endian format to the little endian format      */
            CFE_FS_ByteSwapCFEHeader(Hdr);
        }
    }

    if (OsStatus >= OS_SUCCESS)
    {
        /* The "OsStatus" reflects size actually written */
        Result = (long)OsStatus;
    }
    else
    {
        Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_FS_SetTimestamp(osal_id_t FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    int32              OsStatus;
    int32              Result;
    CFE_FS_Header_t    TempHdr;
    int32              EndianCheck  = 0x01020304;
    CFE_TIME_SysTime_t OutTimestamp = NewTimestamp;
    int32              FileOffset   = 0;

    FileOffset = ((char *)&TempHdr.TimeSeconds - (char *)&TempHdr.ContentType);
    OsStatus   = OS_lseek(FileDes, FileOffset, OS_SEEK_SET);

    if (OsStatus == FileOffset)
    {
        /*
        **  Determine if this is a little endian processor
        */
        /* cppcheck-suppress knownConditionTrueFalse */
        if ((*(char *)(&EndianCheck)) == 0x04)
        {
            /* If this processor is a little endian processor, then convert the timestamp to a big */
            /* endian format so that it is compatible with the standard cFE File Header format     */
            CFE_FS_ByteSwapUint32(&OutTimestamp.Seconds);
            CFE_FS_ByteSwapUint32(&OutTimestamp.Subseconds);
        }

        OsStatus = OS_write(FileDes, &OutTimestamp, sizeof(OutTimestamp));

        /* On a good write, the value returned will equal the number of bytes written */
        if (OsStatus == sizeof(OutTimestamp))
        {
            Result = CFE_SUCCESS;
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Failed to write timestamp (Status=%ld)\n", __func__, (long)OsStatus);
            Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
        }
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: Failed to lseek time fields (Status=%ld)\n", __func__, (long)OsStatus);
        Result = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_FS_ByteSwapCFEHeader(CFE_FS_Header_t *Hdr)
{
    CFE_FS_ByteSwapUint32(&Hdr->ContentType);
    CFE_FS_ByteSwapUint32(&Hdr->SubType);
    CFE_FS_ByteSwapUint32(&Hdr->Length);
    CFE_FS_ByteSwapUint32(&Hdr->SpacecraftID);
    CFE_FS_ByteSwapUint32(&Hdr->ProcessorID);
    CFE_FS_ByteSwapUint32(&Hdr->ApplicationID);
    CFE_FS_ByteSwapUint32(&Hdr->TimeSeconds);
    CFE_FS_ByteSwapUint32(&Hdr->TimeSubSeconds);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_FS_ByteSwapUint32(uint32 *Uint32ToSwapPtr)
{
    int32 Temp   = *Uint32ToSwapPtr;
    char *InPtr  = (char *)&Temp;
    char *OutPtr = (char *)Uint32ToSwapPtr;

    /* SAD: Safe access to InPtr[0-3] and OutPtr[0-3] as both manipulate bytes within 4-byte integers. */
    OutPtr[0] = InPtr[3];
    OutPtr[1] = InPtr[2];
    OutPtr[2] = InPtr[1];
    OutPtr[3] = InPtr[0];
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_FS_ParseInputFileNameEx(char *OutputBuffer, const char *InputBuffer, size_t OutputBufSize,
                                  size_t InputBufSize, const char *DefaultInput, const char *DefaultPath,
                                  const char *DefaultExtension)
{
    int32       Status;
    const char *InputPtr;
    const char *ComponentPtr;
    size_t      ComponentLen;
    char        ComponentTerm;
    size_t      OutputLen;
    size_t      InputLen;
    bool        LastPathReached;

    /* The filename consists of a pathname, filename, and extension component. */
    enum
    {
        PROCESS_INIT,
        PATHNAME_COMPONENT,
        PATHNAME_SEPARATOR,
        FILENAME_COMPONENT,
        EXTENSION_SEPARATOR,
        EXTENSION_COMPONENT,
        END_COMPONENT
    } Component;

    /* Sanity check buffer input */
    if (OutputBuffer == NULL || OutputBufSize == 0)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    Status          = CFE_FS_INVALID_PATH;
    OutputLen       = 0;
    ComponentTerm   = 0;
    LastPathReached = false;

    /* If input buffer is not empty, then use it, otherwise use DefaultInput */
    if (InputBuffer != NULL && InputBufSize > 0 && InputBuffer[0] != 0)
    {
        InputPtr = InputBuffer;
        InputLen = InputBufSize;
    }
    else if (DefaultInput != NULL)
    {
        /* This must be a normal null terminated string */
        InputPtr = DefaultInput;
        InputLen = strlen(DefaultInput);
    }
    else
    {
        /* No input */
        InputPtr = NULL;
        InputLen = 0;
    }

    Component = PROCESS_INIT;
    while (InputPtr != NULL && Component < END_COMPONENT)
    {
        /* Move to next component */
        if (Component == PATHNAME_SEPARATOR && !LastPathReached)
        {
            /* repeat until LastPathReached */
            Component = PATHNAME_COMPONENT;
        }
        else
        {
            ++Component;
        }

        switch (Component)
        {
            case PATHNAME_COMPONENT:
                /* path part ends with the last / char, which begins the filename */
                ComponentTerm = '/';
                ComponentPtr  = memchr(InputPtr, ComponentTerm, InputLen);
                if (ComponentPtr != NULL)
                {
                    /* has path: use pathname from input, advance InputPtr to next part (filename) */
                    ComponentLen = ComponentPtr - InputPtr;
                    ComponentPtr = InputPtr;
                    InputPtr += ComponentLen;
                    InputLen -= ComponentLen;
                }
                else
                {
                    LastPathReached = true;

                    /* no path: if no output at all yet, use default pathname, otherwise move on. */
                    if (DefaultPath != NULL && OutputLen == 0)
                    {
                        ComponentLen = strlen(DefaultPath);
                        ComponentPtr = DefaultPath;
                    }
                    else
                    {
                        /* use no pathname at all */
                        ComponentLen = 0;
                        ComponentPtr = NULL;
                    }
                }
                break;

            case FILENAME_COMPONENT:
                /* filename ends with a . char, which begins the extension */
                ComponentTerm = '.';
                ComponentPtr  = memchr(InputPtr, ComponentTerm, InputLen);
                if (ComponentPtr != NULL)
                {
                    /* has ext: use pathname from input, advance InputPtr to next part (extension) */
                    ComponentLen = ComponentPtr - InputPtr;
                    ComponentPtr = InputPtr;
                    InputPtr += ComponentLen;
                    InputLen -= ComponentLen;
                }
                else
                {
                    /* no ext: use remainder of input here - then use default extension for next part */
                    ComponentLen = InputLen;
                    ComponentPtr = InputPtr;
                    if (DefaultExtension != NULL)
                    {
                        InputPtr = DefaultExtension;
                        InputLen = strlen(DefaultExtension);
                    }
                    else
                    {
                        /* Use no extension */
                        InputPtr = NULL;
                        InputLen = 0;
                    }
                }

                if (ComponentLen > 0 && *ComponentPtr != 0)
                {
                    /*
                     * If the filename part is non-empty, then consider the conversion successful
                     * (note that extension is not really needed for an acceptable filename)
                     */
                    Status = CFE_SUCCESS;
                }

                break;

            case PATHNAME_SEPARATOR:
            case EXTENSION_SEPARATOR:
                /* Remove duplicate terminators that may have been in the input */
                while (OutputLen > 0 && OutputBuffer[OutputLen - 1] == ComponentTerm)
                {
                    --OutputLen;
                }

                ComponentLen = 1;
                ComponentPtr = &ComponentTerm;

                /* advance past any separators in input to get to the next content */
                while (*InputPtr == ComponentTerm && InputLen > 0)
                {
                    ++InputPtr;
                    --InputLen;
                }
                break;

            case EXTENSION_COMPONENT:
                /* Intentional fall through to default case */

            default:
                /* Just consume the rest of input -
                 * should already be pointing to correct data */
                ComponentTerm = 0;
                ComponentLen  = InputLen;
                ComponentPtr  = InputPtr;
                InputPtr      = NULL; /* no more input */
                InputLen      = 0;
                break;
        }

        /* Append component */
        while (ComponentLen > 0 && *ComponentPtr != 0)
        {
            OutputBuffer[OutputLen] = *ComponentPtr;
            ++ComponentPtr;
            ++OutputLen;
            --ComponentLen;

            if (OutputLen >= OutputBufSize)
            {
                /* name is too long to fit in output buffer */
                Status   = CFE_FS_FNAME_TOO_LONG;
                InputPtr = NULL; /* no more input */
                InputLen = 0;
                --OutputLen; /* back up one char for term */
                break;
            }
        }
    }

    /*
     * Always add a final terminating NUL char.
     *
     * Note that the loop above should never entirely fill
     * buffer (length check includes extra char).
     */
    OutputBuffer[OutputLen] = 0;

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_FS_ParseInputFileName(char *OutputBuffer, const char *InputName, size_t OutputBufSize,
                                CFE_FS_FileCategory_t FileCategory)
{
    return CFE_FS_ParseInputFileNameEx(OutputBuffer, NULL, OutputBufSize, 0, InputName,
                                       CFE_FS_GetDefaultMountPoint(FileCategory),
                                       CFE_FS_GetDefaultExtension(FileCategory));
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
    uint32 i, j;
    int    StringLength;
    int    DirMarkIdx;
    int32  ReturnCode;

    if (OriginalPath == NULL || FileNameOnly == NULL)
    {
        ReturnCode = CFE_FS_BAD_ARGUMENT;
    }
    else
    {
        /*
        ** Get the string length of the original file path
        */
        StringLength = strlen(OriginalPath);

        /*
        ** Extract the filename from the Path
        */

        /*
        ** Find the last '/' Character
        */
        DirMarkIdx = -1;
        for (i = 0; i < StringLength; i++)
        {
            if (OriginalPath[i] == '/')
            {
                DirMarkIdx = i;
            }
        }

        /*
        ** Verify the filename isn't too long
        */
        if ((StringLength - (DirMarkIdx + 1)) < OS_MAX_PATH_LEN)
        {
            /*
            ** Extract the filename portion
            */
            if (DirMarkIdx > 0)
            {
                /*
                ** Extract the filename portion
                */
                j = 0;
                for (i = DirMarkIdx + 1; i < StringLength; i++)
                {
                    FileNameOnly[j] = OriginalPath[i];
                    j++;
                }
                FileNameOnly[j] = '\0';

                ReturnCode = CFE_SUCCESS;
            }
            else
            {
                ReturnCode = CFE_FS_INVALID_PATH;
            }
        }
        else
        {
            ReturnCode = CFE_FS_FNAME_TOO_LONG;
        }
    }

    return ReturnCode;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_FS_RunBackgroundFileDump(uint32 ElapsedTime, void *Arg)
{
    CFE_FS_CurrentFileState_t *       State;
    CFE_FS_BackgroundFileDumpEntry_t *Curr;
    CFE_FS_FileWriteMetaData_t *      Meta;
    int32                             OsStatus;
    int32                             Status;
    CFE_FS_Header_t                   FileHdr;
    void *                            RecordPtr;
    size_t                            RecordSize;
    bool                              IsEOF;

    State      = &CFE_FS_Global.FileDump.Current;
    Curr       = NULL;
    IsEOF      = false;
    RecordPtr  = NULL;
    RecordSize = 0;

    State->Credit += (ElapsedTime * CFE_FS_BACKGROUND_CREDIT_PER_SECOND) / 1000;
    if (State->Credit > CFE_FS_BACKGROUND_MAX_CREDIT)
    {
        State->Credit = CFE_FS_BACKGROUND_MAX_CREDIT;
    }

    /*
     * Lock shared data.
     * Not strictly necessary as the "CompleteCount" is only updated
     * by this task but this helps in case the access isn't atomic.
     */
    CFE_FS_LockSharedData(__func__);

    if (CFE_FS_Global.FileDump.CompleteCount != CFE_FS_Global.FileDump.RequestCount)
    {
        Curr = &CFE_FS_Global.FileDump
                    .Entries[CFE_FS_Global.FileDump.CompleteCount & (CFE_FS_MAX_BACKGROUND_FILE_WRITES - 1)];
    }

    CFE_FS_UnlockSharedData(__func__);

    if (Curr == NULL)
    {
        return false;
    }

    Meta = Curr->Meta;

    if (!OS_ObjectIdDefined(State->Fd) && Meta->IsPending)
    {
        /* First time processing this entry - open the file */
        OsStatus =
            OS_OpenCreate(&State->Fd, Meta->FileName, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
        if (OsStatus != OS_SUCCESS)
        {
            State->Fd = OS_OBJECT_ID_UNDEFINED;
            /* NOTE: This converts the OSAL status directly into a CFE status for logging */
            Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_CREATE_ERROR, (long)OsStatus, 0, 0, 0);
        }
        else
        {
            CFE_FS_InitHeader(&FileHdr, Meta->Description, Meta->FileSubType);

            /* write the cFE header to the file */
            Status = CFE_FS_WriteHeader(State->Fd, &FileHdr);
            if (Status != sizeof(CFE_FS_Header_t))
            {
                OS_close(State->Fd);
                State->Fd = OS_OBJECT_ID_UNDEFINED;
                Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR, Status, State->RecordNum,
                              sizeof(CFE_FS_Header_t), State->FileSize);
            }
            else
            {
                State->FileSize = sizeof(CFE_FS_Header_t);
                State->Credit -= sizeof(CFE_FS_Header_t);
                State->RecordNum = 0;
            }
        }
    }

    while (OS_ObjectIdDefined(State->Fd) && State->Credit > 0 && !IsEOF)
    {
        /*
         * Getter should return false on EOF (last record), true if more data is still waiting
         */
        IsEOF = Meta->GetData(Meta, State->RecordNum, &RecordPtr, &RecordSize);

        /*
         * if the getter outputs a record size of 0, this means there is no data for
         * this entry, but the cycle keeps going (in case of "holes" or unused table entries
         * in the database).
         */
        if (RecordSize > 0)
        {
            State->Credit -= RecordSize;

            /*
             * Now write to file
             */
            OsStatus = OS_write(State->Fd, RecordPtr, RecordSize);

            if (OsStatus != RecordSize)
            {
                /* end the file early (cannot set "IsEOF" as this would cause the complete event to be generated too) */
                OS_close(State->Fd);
                State->Fd = OS_OBJECT_ID_UNDEFINED;

                /* generate write error event */
                /* NOTE: This converts the OSAL status directly into a CFE status for logging */
                Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR, (long)OsStatus, State->RecordNum,
                              RecordSize, State->FileSize);
                break;
            }
            else
            {
                State->FileSize += RecordSize;
            }
        }

        ++State->RecordNum;
    }

    /* On normal EOF close the file and generate the complete event */
    if (IsEOF)
    {
        OS_close(State->Fd);
        State->Fd = OS_OBJECT_ID_UNDEFINED;

        /* generate complete event */
        Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_COMPLETE, CFE_SUCCESS, State->RecordNum, 0, State->FileSize);
    }

    /*
     * if the file is not open, consider this file complete, and advance the head position.
     * (done this way so it also catches the case where the file failed to create, not just EOF)
     */
    if (!OS_ObjectIdDefined(State->Fd))
    {
        CFE_FS_LockSharedData(__func__);

        /* Wipe the entry structure, as it will be reused */
        memset(Curr, 0, sizeof(*Curr));
        ++CFE_FS_Global.FileDump.CompleteCount;

        /* Set the "IsPending" flag to false - this indicates that the originator may re-post now */
        Meta->IsPending = false;

        CFE_FS_UnlockSharedData(__func__);
    }

    return !IsEOF;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_FS_BackgroundFileDumpRequest(CFE_FS_FileWriteMetaData_t *Meta)
{
    CFE_FS_BackgroundFileDumpEntry_t *Curr;
    int32                             Status;
    uint32                            PendingRequestCount;

    /* Pre-validate inputs */
    if (Meta == NULL)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    /* getter and event functions must be set */
    if (Meta->GetData == NULL || Meta->OnEvent == NULL)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    /* filename cannot be empty */
    if (Meta->FileName[0] == 0)
    {
        return CFE_FS_INVALID_PATH;
    }

    /* request must not already be pending */
    if (Meta->IsPending)
    {
        return CFE_STATUS_REQUEST_ALREADY_PENDING;
    }

    CFE_FS_LockSharedData(__func__);

    PendingRequestCount = CFE_FS_Global.FileDump.RequestCount + 1;

    /* Check if queue is full before writing to tail position */
    if (PendingRequestCount == (CFE_FS_Global.FileDump.CompleteCount + CFE_FS_MAX_BACKGROUND_FILE_WRITES))
    {
        Status = CFE_STATUS_REQUEST_ALREADY_PENDING;
    }
    else
    {
        Curr = &CFE_FS_Global.FileDump
                    .Entries[CFE_FS_Global.FileDump.RequestCount & (CFE_FS_MAX_BACKGROUND_FILE_WRITES - 1)];

        /*
         * store the meta object - note this retains the pointer that was submitted
         * (caller must not reuse/change this object until request is completed)
         */
        Curr->Meta = Meta;

        /*
         * The "IsPending" Flag will be set true whenever while this is waiting in the request queue.
         * It will be set false when the file is done.
         *
         * The requester can check this flag to determine if/when the request is complete
         */
        Meta->IsPending = true;

        /* update tail position */
        CFE_FS_Global.FileDump.RequestCount = PendingRequestCount;

        Status = CFE_SUCCESS;
    }

    CFE_FS_UnlockSharedData(__func__);

    if (Status == CFE_SUCCESS)
    {
        /*
         * If successfully added to write queue, then wake the ES background task to get started.
         *
         * This may reduce the overall latency between request and completion (depending on other
         * background task work).  If this is the only pending job, this should get it started faster.
         */
        CFE_ES_BackgroundWakeup();
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_FS_BackgroundFileDumpIsPending(const CFE_FS_FileWriteMetaData_t *Meta)
{
    if (Meta == NULL)
    {
        return false;
    }

    return Meta->IsPending;
}
