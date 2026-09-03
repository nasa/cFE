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

/**
 * States of the input file name parser
 *
 * The filename consists of a pathname, filename, and extension component.
 */
typedef enum
{
    CFE_FS_PARSE_PROCESS_INIT,
    CFE_FS_PARSE_PATHNAME_COMPONENT,
    CFE_FS_PARSE_PATHNAME_SEPARATOR,
    CFE_FS_PARSE_FILENAME_COMPONENT,
    CFE_FS_PARSE_EXTENSION_SEPARATOR,
    CFE_FS_PARSE_EXTENSION_COMPONENT,
    CFE_FS_PARSE_END_COMPONENT
} CFE_FS_ParseComponent_Enum_t;

/**
 * Context information used while parsing an input file name
 *
 * This keeps the important state info so it can be easily
 * passed to helper functions.
 */
typedef struct
{
    /* Fixed for the duration of the parse */
    char       *OutputBuffer;
    size_t      OutputBufSize;
    const char *DefaultPath;
    const char *DefaultExtension;

    /* Updated as each component is processed */
    CFE_FS_ParseComponent_Enum_t Component;
    const char                  *InputPtr;
    size_t                       InputLen;
    const char                  *ComponentPtr;
    size_t                       ComponentLen;
    char                         ComponentTerm;
    size_t                       OutputLen;
    bool                         LastPathReached;
    int32                        Status;
} CFE_FS_ParseContext_t;

/*----------------------------------------------------------------
 *
 * Local helper function
 * Selects the input source for the parse operation
 * Uses the caller-supplied input buffer if it is non-empty, otherwise
 * falls back to the DefaultInput string, otherwise there is no input.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_SelectInput(CFE_FS_ParseContext_t *Ctx,
                                                    const char            *InputBuffer,
                                                    size_t                 InputBufSize,
                                                    const char            *DefaultInput)
{
    /* If input buffer is not empty, then use it, otherwise use DefaultInput */
    if (InputBuffer != NULL && InputBufSize > 0 && InputBuffer[0] != 0)
    {
        Ctx->InputPtr = InputBuffer;
        Ctx->InputLen = InputBufSize;
    }
    else if (DefaultInput != NULL)
    {
        /* This must be a normal null terminated string */
        Ctx->InputPtr = DefaultInput;
        Ctx->InputLen = strlen(DefaultInput);
    }
    else
    {
        /* No input */
        Ctx->InputPtr = NULL;
        Ctx->InputLen = 0;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Selects the next component to process
 * The pathname component repeats until the last path separator is reached,
 * all other components are visited exactly once, in order.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_AdvanceComponent(CFE_FS_ParseContext_t *Ctx)
{
    /* Move to next component */
    if (Ctx->Component == CFE_FS_PARSE_PATHNAME_SEPARATOR && !Ctx->LastPathReached)
    {
        /* repeat until LastPathReached */
        Ctx->Component = CFE_FS_PARSE_PATHNAME_COMPONENT;
    }
    else
    {
        ++Ctx->Component;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Splits the remaining input at the next occurrence of the given terminator
 * If found, the component is set to the part of the input preceding the
 * terminator, and the input is advanced to the terminator.  Returns whether
 * the terminator was found.  ComponentTerm is always set to Term.
 *
 *-----------------------------------------------------------------*/
static bool CFE_FS_ParseInputFileNameEx_SplitAtTerm(CFE_FS_ParseContext_t *Ctx, char Term)
{
    Ctx->ComponentTerm = Term;
    Ctx->ComponentPtr  = memchr(Ctx->InputPtr, Ctx->ComponentTerm, Ctx->InputLen);
    if (Ctx->ComponentPtr == NULL)
    {
        return false;
    }

    /* use the part before the terminator, advance InputPtr to the next part */
    Ctx->ComponentLen  = Ctx->ComponentPtr - Ctx->InputPtr;
    Ctx->ComponentPtr  = Ctx->InputPtr;
    Ctx->InputPtr     += Ctx->ComponentLen;
    Ctx->InputLen     -= Ctx->ComponentLen;

    return true;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Selects the pathname component
 * The path part ends with the last / char, which begins the filename.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_PathnameComponent(CFE_FS_ParseContext_t *Ctx)
{
    /* has path: use pathname from input, advance InputPtr to next part (filename) */
    if (CFE_FS_ParseInputFileNameEx_SplitAtTerm(Ctx, '/'))
    {
        return;
    }

    Ctx->LastPathReached = true;

    /* no path: if no output at all yet, use default pathname, otherwise move on. */
    if (Ctx->DefaultPath != NULL && Ctx->OutputLen == 0)
    {
        Ctx->ComponentLen = strlen(Ctx->DefaultPath);
        Ctx->ComponentPtr = Ctx->DefaultPath;
    }
    else
    {
        /* use no pathname at all */
        Ctx->ComponentLen = 0;
        Ctx->ComponentPtr = NULL;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Selects the filename component
 * The filename ends with a . char, which begins the extension.  If there is
 * no extension in the input, this switches the input over to the default
 * extension, or ends the parse if there is no default extension.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_FilenameComponent(CFE_FS_ParseContext_t *Ctx)
{
    /* has ext: use filename from input, advance InputPtr to next part (extension) */
    if (!CFE_FS_ParseInputFileNameEx_SplitAtTerm(Ctx, '.'))
    {
        /* no ext: use remainder of input here - then use default extension for next part */
        Ctx->ComponentLen = Ctx->InputLen;
        Ctx->ComponentPtr = Ctx->InputPtr;
        if (Ctx->DefaultExtension != NULL)
        {
            Ctx->InputPtr = Ctx->DefaultExtension;
            Ctx->InputLen = strlen(Ctx->DefaultExtension);
        }
        else
        {
            /* Use no extension */
            Ctx->Component = CFE_FS_PARSE_END_COMPONENT;
            Ctx->InputLen  = 0;
        }
    }

    if (Ctx->ComponentLen > 0 && *Ctx->ComponentPtr != 0)
    {
        /*
         * If the filename part is non-empty, then consider the conversion successful
         * (note that extension is not really needed for an acceptable filename)
         */
        Ctx->Status = CFE_SUCCESS;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Selects a single separator char as the component
 * Duplicate separators are removed from both the tail of the output and the
 * head of the remaining input.  ComponentTerm holds the separator char, as
 * set by the preceding pathname or filename component.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_SeparatorComponent(CFE_FS_ParseContext_t *Ctx)
{
    /* Remove duplicate terminators that may have been in the input */
    while (Ctx->OutputLen > 0 && Ctx->OutputBuffer[Ctx->OutputLen - 1] == Ctx->ComponentTerm)
    {
        --Ctx->OutputLen;
    }

    Ctx->ComponentLen = 1;
    Ctx->ComponentPtr = &Ctx->ComponentTerm;

    /* advance past any separators in input to get to the next content */
    while (*Ctx->InputPtr == Ctx->ComponentTerm && Ctx->InputLen > 0)
    {
        ++Ctx->InputPtr;
        --Ctx->InputLen;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Selects all remaining input as the component
 * This consumes the rest of the input, ending the parse.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_RemainderComponent(CFE_FS_ParseContext_t *Ctx)
{
    /* Just consume the rest of input -
     * should already be pointing to correct data */
    Ctx->ComponentTerm = 0;
    Ctx->ComponentLen  = Ctx->InputLen;
    Ctx->ComponentPtr  = Ctx->InputPtr;
    Ctx->InputPtr      = NULL; /* no more input */
    Ctx->InputLen      = 0;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Selects the component content for the current parse state
 * On return the ComponentPtr/ComponentLen pair describes the text to append.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_SelectComponent(CFE_FS_ParseContext_t *Ctx)
{
    switch (Ctx->Component)
    {
        case CFE_FS_PARSE_PATHNAME_COMPONENT:
            CFE_FS_ParseInputFileNameEx_PathnameComponent(Ctx);
            break;

        case CFE_FS_PARSE_FILENAME_COMPONENT:
            CFE_FS_ParseInputFileNameEx_FilenameComponent(Ctx);
            break;

        case CFE_FS_PARSE_PATHNAME_SEPARATOR:
        case CFE_FS_PARSE_EXTENSION_SEPARATOR:
            CFE_FS_ParseInputFileNameEx_SeparatorComponent(Ctx);
            break;

        case CFE_FS_PARSE_EXTENSION_COMPONENT:
            /* Intentional fall through to default case */

        default:
            CFE_FS_ParseInputFileNameEx_RemainderComponent(Ctx);
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Appends the currently selected component to the output buffer
 * If the component does not fit, the parse is ended with FNAME_TOO_LONG.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_ParseInputFileNameEx_AppendComponent(CFE_FS_ParseContext_t *Ctx)
{
    while (Ctx->ComponentLen > 0 && *Ctx->ComponentPtr != 0)
    {
        Ctx->OutputBuffer[Ctx->OutputLen] = *Ctx->ComponentPtr;
        ++Ctx->ComponentPtr;
        ++Ctx->OutputLen;
        --Ctx->ComponentLen;

        if (Ctx->OutputLen >= Ctx->OutputBufSize)
        {
            /* name is too long to fit in output buffer */
            Ctx->Status   = CFE_FS_FNAME_TOO_LONG;
            Ctx->InputPtr = NULL; /* no more input */
            Ctx->InputLen = 0;
            --Ctx->OutputLen; /* back up one char for term */
            break;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_FS_ParseInputFileNameEx(char       *OutputBuffer,
                                  const char *InputBuffer,
                                  size_t      OutputBufSize,
                                  size_t      InputBufSize,
                                  const char *DefaultInput,
                                  const char *DefaultPath,
                                  const char *DefaultExtension)
{
    CFE_FS_ParseContext_t Ctx;

    /* Sanity check buffer input */
    if (OutputBuffer == NULL || OutputBufSize == 0)
    {
        return CFE_FS_BAD_ARGUMENT;
    }

    memset(&Ctx, 0, sizeof(Ctx));

    Ctx.OutputBuffer     = OutputBuffer;
    Ctx.OutputBufSize    = OutputBufSize;
    Ctx.DefaultPath      = DefaultPath;
    Ctx.DefaultExtension = DefaultExtension;
    Ctx.Status           = CFE_FS_INVALID_PATH;
    Ctx.ComponentPtr     = NULL;
    Ctx.InputPtr         = NULL;
    Ctx.Component        = CFE_FS_PARSE_PROCESS_INIT;

    CFE_FS_ParseInputFileNameEx_SelectInput(&Ctx, InputBuffer, InputBufSize, DefaultInput);

    while (Ctx.InputPtr != NULL && Ctx.Component < CFE_FS_PARSE_END_COMPONENT)
    {
        CFE_FS_ParseInputFileNameEx_AdvanceComponent(&Ctx);
        CFE_FS_ParseInputFileNameEx_SelectComponent(&Ctx);
        CFE_FS_ParseInputFileNameEx_AppendComponent(&Ctx);
    }

    /*
     * Always add a final terminating NUL char.
     *
     * Note that the loop above should never entirely fill
     * buffer (length check includes extra char).
     */
    OutputBuffer[Ctx.OutputLen] = 0;

    return Ctx.Status;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_FS_ParseInputFileName(char                 *OutputBuffer,
                                const char           *InputName,
                                size_t                OutputBufSize,
                                CFE_FS_FileCategory_t FileCategory)
{
    return CFE_FS_ParseInputFileNameEx(OutputBuffer,
                                       NULL,
                                       OutputBufSize,
                                       0,
                                       InputName,
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
 * Local helper function
 * Opens the output file and writes the cFE header for a pending entry
 * Does nothing if the file is already open or the entry is not pending.
 * On any failure the file descriptor is left undefined and the
 * corresponding error event is generated.
 *
 *-----------------------------------------------------------------*/
static void CFE_FS_RunBackgroundFileDump_OpenFile(CFE_FS_CurrentFileState_t *State, CFE_FS_FileWriteMetaData_t *Meta)
{
    int32           OsStatus;
    int32           Status;
    CFE_FS_Header_t FileHdr;

    if (OS_ObjectIdDefined(State->Fd) || !Meta->IsPending)
    {
        return;
    }

    /* First time processing this entry - open the file */
    OsStatus = OS_OpenCreate(&State->Fd, Meta->FileName, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
    if (OsStatus != OS_SUCCESS)
    {
        State->Fd = OS_OBJECT_ID_UNDEFINED;
        /* NOTE: This converts the OSAL status directly into a CFE status for logging */
        Meta->OnEvent(Meta, CFE_FS_FileWriteEvent_CREATE_ERROR, (long)OsStatus, 0, 0, 0);
        return;
    }

    CFE_FS_InitHeader(&FileHdr, Meta->Description, Meta->FileSubType);

    /* write the cFE header to the file */
    Status = CFE_FS_WriteHeader(State->Fd, &FileHdr);
    if (Status != sizeof(CFE_FS_Header_t))
    {
        OS_close(State->Fd);
        State->Fd = OS_OBJECT_ID_UNDEFINED;
        Meta->OnEvent(Meta,
                      CFE_FS_FileWriteEvent_HEADER_WRITE_ERROR,
                      Status,
                      State->RecordNum,
                      sizeof(CFE_FS_Header_t),
                      State->FileSize);
        return;
    }

    State->FileSize   = sizeof(CFE_FS_Header_t);
    State->Credit    -= sizeof(CFE_FS_Header_t);
    State->RecordNum  = 0;
}

/*----------------------------------------------------------------
 *
 * Local helper function
 * Writes records to the output file while credit remains
 * Returns whether the end of the data set was reached.  A write error
 * ends the file early without indicating EOF, so that the completion
 * event is not generated for it.
 *
 *-----------------------------------------------------------------*/
static bool CFE_FS_RunBackgroundFileDump_WriteRecords(CFE_FS_CurrentFileState_t  *State,
                                                      CFE_FS_FileWriteMetaData_t *Meta)
{
    int32  OsStatus;
    void  *RecordPtr;
    size_t RecordSize;
    bool   IsEOF;

    IsEOF      = false;
    RecordPtr  = NULL;
    RecordSize = 0;

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
                Meta->OnEvent(Meta,
                              CFE_FS_FileWriteEvent_RECORD_WRITE_ERROR,
                              (long)OsStatus,
                              State->RecordNum,
                              RecordSize,
                              State->FileSize);
                break;
            }
            else
            {
                State->FileSize += RecordSize;
            }
        }

        ++State->RecordNum;
    }

    return IsEOF;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_FS_RunBackgroundFileDump(uint32 ElapsedTime, void *Arg)
{
    CFE_FS_CurrentFileState_t        *State;
    CFE_FS_BackgroundFileDumpEntry_t *Curr;
    CFE_FS_FileWriteMetaData_t       *Meta;
    bool                              IsEOF;

    State = &CFE_FS_Global.FileDump.Current;
    Curr  = NULL;

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

    CFE_FS_RunBackgroundFileDump_OpenFile(State, Meta);

    IsEOF = CFE_FS_RunBackgroundFileDump_WriteRecords(State, Meta);

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
