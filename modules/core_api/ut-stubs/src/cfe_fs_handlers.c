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
** File: ut_fs_stubs.c
**
** Purpose:
** Unit test stubs for File Service routines
**
** Notes:
** Minimal work is done, only what is required for unit testing
**
*/

/*
** Includes
*/
#include <string.h>
#include "cfe.h"
#include "utstubs.h"

/*
** Functions
*/

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_GetDefaultMountPoint coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_GetDefaultMountPoint(void *UserObj, UT_EntryKey_t FuncKey,
                                                   const UT_StubContext_t *Context)
{
    int32             Status;
    static const char DEFAULT_MOUNTPOINT[] = "/ut";
    void *            TempBuff;
    const char *      Result;

    UT_Stub_GetInt32StatusCode(Context, &Status);
    Result = NULL;

    if (Status == CFE_SUCCESS)
    {
        /* If the test case supplied a buffer, return it, otherwise return fixed value */
        UT_GetDataBuffer(UT_KEY(CFE_FS_GetDefaultMountPoint), &TempBuff, NULL, NULL);
        if (TempBuff == NULL)
        {
            Result = DEFAULT_MOUNTPOINT;
        }
        else
        {
            Result = TempBuff;
        }
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_GetDefaultExtension coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_GetDefaultExtension(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    int32             Status;
    static const char DEFAULT_EXTENSION[] = ".ut";
    void *            TempBuff;
    const char *      Result;

    UT_Stub_GetInt32StatusCode(Context, &Status);
    Result = NULL;

    if (Status == CFE_SUCCESS)
    {
        /* If the test case supplied a buffer, return it, otherwise return fixed value */
        UT_GetDataBuffer(UT_KEY(CFE_FS_GetDefaultExtension), &TempBuff, NULL, NULL);
        if (TempBuff == NULL)
        {
            Result = DEFAULT_EXTENSION;
        }
        else
        {
            Result = TempBuff;
        }
    }

    UT_Stub_SetReturnValue(FuncKey, Result);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_WriteHeader coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_WriteHeader(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_FS_Header_t *Hdr = UT_Hook_GetArgValueByName(Context, "Hdr", CFE_FS_Header_t *);

    int32 status;

    if (!UT_Stub_GetInt32StatusCode(Context, &status))
    {
        status = sizeof(CFE_FS_Header_t);
        UT_Stub_SetReturnValue(FuncKey, status);
    }

    if (status > 0)
    {
        UT_Stub_CopyFromLocal(UT_KEY(CFE_FS_WriteHeader), Hdr, status);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_ReadHeader coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_ReadHeader(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    CFE_FS_Header_t *Hdr = UT_Hook_GetArgValueByName(Context, "Hdr", CFE_FS_Header_t *);

    int32 status;

    if (!UT_Stub_GetInt32StatusCode(Context, &status))
    {
        status = sizeof(CFE_FS_Header_t);
        UT_Stub_SetReturnValue(FuncKey, status);
    }

    if (status > 0)
    {
        UT_Stub_CopyToLocal(UT_KEY(CFE_FS_ReadHeader), Hdr, status);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_ParseInputFileNameEx coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_ParseInputFileNameEx(void *UserObj, UT_EntryKey_t FuncKey,
                                                   const UT_StubContext_t *Context)
{
    char *      OutputBuffer  = UT_Hook_GetArgValueByName(Context, "OutputBuffer", char *);
    size_t      OutputBufSize = UT_Hook_GetArgValueByName(Context, "OutputBufSize", size_t);
    const char *InputBuffer   = UT_Hook_GetArgValueByName(Context, "InputBuffer", const char *);
    const char *DefaultInput  = UT_Hook_GetArgValueByName(Context, "DefaultInput", const char *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    /* Copy any specific output supplied by test case */
    if (status >= 0 && UT_Stub_CopyToLocal(UT_KEY(CFE_FS_ParseInputFileNameEx), OutputBuffer, OutputBufSize) == 0 &&
        OutputBufSize > 0)
    {
        if (DefaultInput != NULL)
        {
            /* Use default if set */
            strncpy(OutputBuffer, DefaultInput, OutputBufSize);
        }
        else
        {
            /* Fall back to copy input to avoid uninitialized output */
            strncpy(OutputBuffer, InputBuffer, OutputBufSize);
        }
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_ParseInputFileName coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_ParseInputFileName(void *UserObj, UT_EntryKey_t FuncKey, const UT_StubContext_t *Context)
{
    char *      OutputBuffer  = UT_Hook_GetArgValueByName(Context, "OutputBuffer", char *);
    size_t      OutputBufSize = UT_Hook_GetArgValueByName(Context, "OutputBufSize", size_t);
    const char *InputName     = UT_Hook_GetArgValueByName(Context, "InputName", const char *);

    int32 status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    /* Copy any specific output supplied by test case */
    if (status >= 0 && UT_Stub_CopyToLocal(UT_KEY(CFE_FS_ParseInputFileName), OutputBuffer, OutputBufSize) == 0 &&
        OutputBufSize > 0)
    {
        /* Otherwise fall back to simple copy */
        strncpy(OutputBuffer, InputName, OutputBufSize);
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_ExtractFilenameFromPath coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_ExtractFilenameFromPath(void *UserObj, UT_EntryKey_t FuncKey,
                                                      const UT_StubContext_t *Context)
{
    const char *OriginalPath = UT_Hook_GetArgValueByName(Context, "OriginalPath", const char *);
    char *      FileNameOnly = UT_Hook_GetArgValueByName(Context, "FileNameOnly", char *);

    int    i, j;
    int    StringLength;
    int    DirMarkIdx;
    int32  status;
    uint32 UserBuffLen;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status >= 0)
    {
        if (OriginalPath == NULL || FileNameOnly == NULL)
        {
            status = CFE_FS_BAD_ARGUMENT;
        }
        else
        {
            UserBuffLen = UT_Stub_CopyToLocal(UT_KEY(CFE_FS_ExtractFilenameFromPath), FileNameOnly, OS_MAX_FILE_NAME);

            if (UserBuffLen >= OS_MAX_FILE_NAME)
            {
                FileNameOnly[OS_MAX_FILE_NAME - 1] = 0;
            }
            else if (UserBuffLen > 0)
            {
                /* Just ensure that the output is null terminated */
                FileNameOnly[UserBuffLen] = 0;
            }
            else
            {
                /* Get the string length of the original file path */
                StringLength = strlen(OriginalPath);

                /* Extract the filename from the Path:
                     Find the last '/' Character */
                DirMarkIdx = -1;

                for (i = 0; i < StringLength; i++)
                {
                    if (OriginalPath[i] == '/')
                    {
                        DirMarkIdx = i;
                    }
                }

                /* Verify the filename isn't too long */
                if ((StringLength - (DirMarkIdx + 1)) < OS_MAX_PATH_LEN)
                {
                    /* Extract the filename portion */
                    if (DirMarkIdx > 0)
                    {
                        /* Extract the filename portion */
                        j = 0;

                        for (i = DirMarkIdx + 1; i < StringLength; i++)
                        {
                            FileNameOnly[j] = OriginalPath[i];
                            j++;
                        }

                        FileNameOnly[j] = '\0';
                    }
                    else
                    {
                        status = CFE_FS_INVALID_PATH;
                    }
                }
                else
                {
                    status = CFE_FS_FNAME_TOO_LONG;
                }
            }
        }
    }
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_BackgroundFileDumpIsPending coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_BackgroundFileDumpIsPending(void *UserObj, UT_EntryKey_t FuncKey,
                                                          const UT_StubContext_t *Context)
{
    int32 status;
    bool  return_value;

    UT_Stub_GetInt32StatusCode(Context, &status);

    return_value = status;

    UT_Stub_SetReturnValue(FuncKey, return_value);
}

/*------------------------------------------------------------
 *
 * Default handler for CFE_FS_BackgroundFileDumpRequest coverage stub function
 *
 *------------------------------------------------------------*/
void UT_DefaultHandler_CFE_FS_BackgroundFileDumpRequest(void *UserObj, UT_EntryKey_t FuncKey,
                                                        const UT_StubContext_t *Context)
{
    CFE_FS_FileWriteMetaData_t *Meta = UT_Hook_GetArgValueByName(Context, "Meta", CFE_FS_FileWriteMetaData_t *);
    int32                       status;

    UT_Stub_GetInt32StatusCode(Context, &status);

    if (status == CFE_SUCCESS)
    {
        /* Snapshot the request, in case the UT test case wants to look */
        UT_Stub_CopyFromLocal(UT_KEY(CFE_FS_BackgroundFileDumpRequest), Meta, sizeof(*Meta));
    }
}
