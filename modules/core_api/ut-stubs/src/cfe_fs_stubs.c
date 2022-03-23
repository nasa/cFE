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

/**
 * @file
 *
 * Auto-Generated stub implementations for functions defined in cfe_fs header
 */

#include "cfe_fs.h"
#include "utgenstub.h"

void UT_DefaultHandler_CFE_FS_BackgroundFileDumpIsPending(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_BackgroundFileDumpRequest(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_ExtractFilenameFromPath(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_GetDefaultExtension(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_GetDefaultMountPoint(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_ParseInputFileName(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_ParseInputFileNameEx(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_ReadHeader(void *, UT_EntryKey_t, const UT_StubContext_t *);
void UT_DefaultHandler_CFE_FS_WriteHeader(void *, UT_EntryKey_t, const UT_StubContext_t *);

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_BackgroundFileDumpIsPending()
 * ----------------------------------------------------
 */
bool CFE_FS_BackgroundFileDumpIsPending(const CFE_FS_FileWriteMetaData_t *Meta)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_BackgroundFileDumpIsPending, bool);

    UT_GenStub_AddParam(CFE_FS_BackgroundFileDumpIsPending, const CFE_FS_FileWriteMetaData_t *, Meta);

    UT_GenStub_Execute(CFE_FS_BackgroundFileDumpIsPending, Basic, UT_DefaultHandler_CFE_FS_BackgroundFileDumpIsPending);

    return UT_GenStub_GetReturnValue(CFE_FS_BackgroundFileDumpIsPending, bool);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_BackgroundFileDumpRequest()
 * ----------------------------------------------------
 */
int32 CFE_FS_BackgroundFileDumpRequest(CFE_FS_FileWriteMetaData_t *Meta)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_BackgroundFileDumpRequest, int32);

    UT_GenStub_AddParam(CFE_FS_BackgroundFileDumpRequest, CFE_FS_FileWriteMetaData_t *, Meta);

    UT_GenStub_Execute(CFE_FS_BackgroundFileDumpRequest, Basic, UT_DefaultHandler_CFE_FS_BackgroundFileDumpRequest);

    return UT_GenStub_GetReturnValue(CFE_FS_BackgroundFileDumpRequest, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_ExtractFilenameFromPath()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_ExtractFilenameFromPath, CFE_Status_t);

    UT_GenStub_AddParam(CFE_FS_ExtractFilenameFromPath, const char *, OriginalPath);
    UT_GenStub_AddParam(CFE_FS_ExtractFilenameFromPath, char *, FileNameOnly);

    UT_GenStub_Execute(CFE_FS_ExtractFilenameFromPath, Basic, UT_DefaultHandler_CFE_FS_ExtractFilenameFromPath);

    return UT_GenStub_GetReturnValue(CFE_FS_ExtractFilenameFromPath, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_GetDefaultExtension()
 * ----------------------------------------------------
 */
const char *CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_t FileCategory)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_GetDefaultExtension, const char *);

    UT_GenStub_AddParam(CFE_FS_GetDefaultExtension, CFE_FS_FileCategory_t, FileCategory);

    UT_GenStub_Execute(CFE_FS_GetDefaultExtension, Basic, UT_DefaultHandler_CFE_FS_GetDefaultExtension);

    return UT_GenStub_GetReturnValue(CFE_FS_GetDefaultExtension, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_GetDefaultMountPoint()
 * ----------------------------------------------------
 */
const char *CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_t FileCategory)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_GetDefaultMountPoint, const char *);

    UT_GenStub_AddParam(CFE_FS_GetDefaultMountPoint, CFE_FS_FileCategory_t, FileCategory);

    UT_GenStub_Execute(CFE_FS_GetDefaultMountPoint, Basic, UT_DefaultHandler_CFE_FS_GetDefaultMountPoint);

    return UT_GenStub_GetReturnValue(CFE_FS_GetDefaultMountPoint, const char *);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_InitHeader()
 * ----------------------------------------------------
 */
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType)
{
    UT_GenStub_AddParam(CFE_FS_InitHeader, CFE_FS_Header_t *, Hdr);
    UT_GenStub_AddParam(CFE_FS_InitHeader, const char *, Description);
    UT_GenStub_AddParam(CFE_FS_InitHeader, uint32, SubType);

    UT_GenStub_Execute(CFE_FS_InitHeader, Basic, NULL);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_ParseInputFileName()
 * ----------------------------------------------------
 */
int32 CFE_FS_ParseInputFileName(char *OutputBuffer, const char *InputName, size_t OutputBufSize,
                                CFE_FS_FileCategory_t FileCategory)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_ParseInputFileName, int32);

    UT_GenStub_AddParam(CFE_FS_ParseInputFileName, char *, OutputBuffer);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileName, const char *, InputName);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileName, size_t, OutputBufSize);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileName, CFE_FS_FileCategory_t, FileCategory);

    UT_GenStub_Execute(CFE_FS_ParseInputFileName, Basic, UT_DefaultHandler_CFE_FS_ParseInputFileName);

    return UT_GenStub_GetReturnValue(CFE_FS_ParseInputFileName, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_ParseInputFileNameEx()
 * ----------------------------------------------------
 */
int32 CFE_FS_ParseInputFileNameEx(char *OutputBuffer, const char *InputBuffer, size_t OutputBufSize,
                                  size_t InputBufSize, const char *DefaultInput, const char *DefaultPath,
                                  const char *DefaultExtension)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_ParseInputFileNameEx, int32);

    UT_GenStub_AddParam(CFE_FS_ParseInputFileNameEx, char *, OutputBuffer);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileNameEx, const char *, InputBuffer);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileNameEx, size_t, OutputBufSize);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileNameEx, size_t, InputBufSize);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileNameEx, const char *, DefaultInput);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileNameEx, const char *, DefaultPath);
    UT_GenStub_AddParam(CFE_FS_ParseInputFileNameEx, const char *, DefaultExtension);

    UT_GenStub_Execute(CFE_FS_ParseInputFileNameEx, Basic, UT_DefaultHandler_CFE_FS_ParseInputFileNameEx);

    return UT_GenStub_GetReturnValue(CFE_FS_ParseInputFileNameEx, int32);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_ReadHeader()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, osal_id_t FileDes)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_ReadHeader, CFE_Status_t);

    UT_GenStub_AddParam(CFE_FS_ReadHeader, CFE_FS_Header_t *, Hdr);
    UT_GenStub_AddParam(CFE_FS_ReadHeader, osal_id_t, FileDes);

    UT_GenStub_Execute(CFE_FS_ReadHeader, Basic, UT_DefaultHandler_CFE_FS_ReadHeader);

    return UT_GenStub_GetReturnValue(CFE_FS_ReadHeader, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_SetTimestamp()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_FS_SetTimestamp(osal_id_t FileDes, CFE_TIME_SysTime_t NewTimestamp)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_SetTimestamp, CFE_Status_t);

    UT_GenStub_AddParam(CFE_FS_SetTimestamp, osal_id_t, FileDes);
    UT_GenStub_AddParam(CFE_FS_SetTimestamp, CFE_TIME_SysTime_t, NewTimestamp);

    UT_GenStub_Execute(CFE_FS_SetTimestamp, Basic, NULL);

    return UT_GenStub_GetReturnValue(CFE_FS_SetTimestamp, CFE_Status_t);
}

/*
 * ----------------------------------------------------
 * Generated stub function for CFE_FS_WriteHeader()
 * ----------------------------------------------------
 */
CFE_Status_t CFE_FS_WriteHeader(osal_id_t FileDes, CFE_FS_Header_t *Hdr)
{
    UT_GenStub_SetupReturnBuffer(CFE_FS_WriteHeader, CFE_Status_t);

    UT_GenStub_AddParam(CFE_FS_WriteHeader, osal_id_t, FileDes);
    UT_GenStub_AddParam(CFE_FS_WriteHeader, CFE_FS_Header_t *, Hdr);

    UT_GenStub_Execute(CFE_FS_WriteHeader, Basic, UT_DefaultHandler_CFE_FS_WriteHeader);

    return UT_GenStub_GetReturnValue(CFE_FS_WriteHeader, CFE_Status_t);
}
