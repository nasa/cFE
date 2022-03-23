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
 * Purpose:  cFE File Services (FS) library API header file
 *
 * Author:   S.Walling/Microtel
 *
 */

#ifndef CFE_FS_H
#define CFE_FS_H

/*
** Required header files...
*/
#include "common_types.h"
#include "osconfig.h"
#include "cfe_platform_cfg.h"
#include "cfe_error.h"
#include "cfe_fs_api_typedefs.h"
#include "cfe_fs_extern_typedefs.h"
#include "cfe_time_api_typedefs.h"

/** @defgroup CFEAPIFSHeader cFE File Header Management APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Read the contents of the Standard cFE File Header
**
** \par Description
**        This API will fill the specified #CFE_FS_Header_t variable with the
**        contents of the Standard cFE File Header of the file identified by
**        the given File Descriptor.
**
** \par Assumptions, External Events, and Notes:
**        -# The File has already been successfully opened using #OS_OpenCreate and
**           the caller has a legitimate File Descriptor.
**        -# File offset behavior: Agnostic on entry since it will move the offset to the start of the file,
**           on success the offset will be at the end of the header, undefined offset behavior for error cases.
**
** \param[out] Hdr         Pointer to a variable of type #CFE_FS_Header_t @nonnull that will be
**                         filled with the contents of the Standard cFE File Header. *Hdr is the contents of the
**                         Standard cFE File Header for the specified file.
** \param[in] FileDes      File Descriptor obtained from a previous call to #OS_OpenCreate
**                         that is associated with the file whose header is to be read.
**
** \return Bytes read or error status from OSAL
** \retval #CFE_FS_BAD_ARGUMENT  \copybrief CFE_FS_BAD_ARGUMENT
**
** \note This function invokes OSAL API routines and the current implementation may return
**       OSAL error codes to the caller if failure occurs.  In a future version of CFE, the
**       status codes will be converted to a value in \ref CFEReturnCodes.
**
** \sa #CFE_FS_WriteHeader
**
******************************************************************************/
CFE_Status_t CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, osal_id_t FileDes);

/*****************************************************************************/
/**
** \brief Initializes the contents of the Standard cFE File Header
**
** \par Description
**        This API will clear the specified #CFE_FS_Header_t variable and
**        initialize the description field with the specified value
**
** \param[in] Hdr          Pointer to a variable of type #CFE_FS_Header_t that will be
**                         cleared and initialized
**
** \param[in]  Description Initializes Header's Description @nonnull
**
** \param[in]  SubType     Initializes Header's SubType
**
** \sa #CFE_FS_WriteHeader
**
******************************************************************************/
void CFE_FS_InitHeader(CFE_FS_Header_t *Hdr, const char *Description, uint32 SubType);

/*****************************************************************************/
/**
** \brief Write the specified Standard cFE File Header to the specified file
**
** \par Description
**        This API will output the specified #CFE_FS_Header_t variable, with some
**        fields automatically updated, to the specified file as the Standard cFE
**        File Header. This API will automatically populate the following fields
**        in the specified #CFE_FS_Header_t:
**
**      -# \link #CFE_FS_Header_t::ContentType \c ContentType \endlink - Filled with 0x63464531 ('cFE1')
**      -# \link #CFE_FS_Header_t::Length \c Length \endlink - Filled with the sizeof(CFE_FS_Header_t)
**      -# \link #CFE_FS_Header_t::SpacecraftID \c SpacecraftID \endlink - Filled with the Spacecraft ID
**      -# \link #CFE_FS_Header_t::ProcessorID \c ProcessorID \endlink - Filled with the Processor ID
**      -# \link #CFE_FS_Header_t::ApplicationID \c ApplicationID \endlink -  Filled with the Application ID
**      -# \link #CFE_FS_Header_t::TimeSeconds \c TimeSeconds \endlink - Filled with the Time, in seconds, as obtained
**                                                                       by #CFE_TIME_GetTime
**      -# \link #CFE_FS_Header_t::TimeSubSeconds \c TimeSubSeconds \endlink - Filled with the Time, subseconds, as
**                                                                             obtained by #CFE_TIME_GetTime
**
** \par Assumptions, External Events, and Notes:
**        -# The File has already been successfully opened using #OS_OpenCreate and
**           the caller has a legitimate File Descriptor.
**        -# The \c SubType field has been filled appropriately by the Application.
**        -# The \c Description field has been filled appropriately by the Application.
**        -# File offset behavior: Agnostic on entry since it will move the offset to the start of the file,
**           on success the offset will be at the end of the header, undefined offset behavior for error cases.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_OpenCreate
**                    that is associated with the file whose header is to be read.
**
** \param[out] Hdr    Pointer to a variable of type #CFE_FS_Header_t @nonnull that will be
**                    filled with the contents of the Standard cFE File Header. *Hdr is the contents of the
**                    Standard cFE File Header for the specified file.
**
** \return Bytes read or error status from OSAL
** \retval #CFE_FS_BAD_ARGUMENT  \copybrief CFE_FS_BAD_ARGUMENT
**
** \note This function invokes OSAL API routines and the current implementation may return
**       OSAL error codes to the caller if failure occurs.  In a future version of CFE, the
**       status codes will be converted to a value in \ref CFEReturnCodes.
**
** \sa #CFE_FS_ReadHeader
**
******************************************************************************/
CFE_Status_t CFE_FS_WriteHeader(osal_id_t FileDes, CFE_FS_Header_t *Hdr);

/*****************************************************************************/
/**
** \brief Modifies the Time Stamp field in the Standard cFE File Header for the specified file
**
** \par Description
**        This API will modify the \link #CFE_FS_Header_t::TimeSeconds timestamp \endlink found
**        in the Standard cFE File Header of the specified file.  The timestamp will be replaced
**        with the time specified by the caller.
**
** \par Assumptions, External Events, and Notes:
**        -# The File has already been successfully opened using #OS_OpenCreate and
**           the caller has a legitimate File Descriptor.
**        -# The \c NewTimestamp field has been filled appropriately by the Application.
**        -# File offset behavior: Agnostic on entry since it will move the offset,
**           on success the offset will be at the end of the time stamp, undefined offset behavior for error cases.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_OpenCreate
**                    that is associated with the file whose header is to be read.
**
** \param[in] NewTimestamp A #CFE_TIME_SysTime_t data structure containing the desired time
**                         to be put into the file's Standard cFE File Header.
**
** \return Execution status, see \ref CFEReturnCodes, or OSAL status
** \retval #CFE_STATUS_EXTERNAL_RESOURCE_FAIL  \covtest \copybrief CFE_STATUS_EXTERNAL_RESOURCE_FAIL
** \retval #CFE_SUCCESS                        \copybrief CFE_SUCCESS
**
** \note This function invokes OSAL API routines and the current implementation may return
**       OSAL error codes to the caller if failure occurs.  In a future version of CFE, the
**       status codes will be converted to a value in \ref CFEReturnCodes.
**
******************************************************************************/
CFE_Status_t CFE_FS_SetTimestamp(osal_id_t FileDes, CFE_TIME_SysTime_t NewTimestamp);
/**@}*/

/** @defgroup CFEAPIFSUtil cFE File Utility APIs
 * @{
 */

/*****************************************************************************/
/**
** \brief Get the default virtual mount point for a file category
**
** Certain classes of files generally reside in a common directory, mainly
** either the persistent storage (/cf typically) or ram disk (/ram typically).
**
** Ephemeral status files are generally in the ram disk while application
** modules and scripts are generally in the persistent storage.
**
** This returns the expected directory for a given class of files in the form
** of a virtual OSAL mount point string.
**
** \returns String containing the mount point
** \retval  NULL if no mount point is known for the given file category
*/
const char *CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_t FileCategory);

/*****************************************************************************/
/**
** \brief Get the default filename extension for a file category
**
** Certain file types may have an extension that varies from system to system.
** This is primarily an issue for application modules which are ".so" on
** Linux systems, ".dll" on Windows, ".o" on VxWorks, ".obj" on RTEMS, and so on.
**
** This uses a combination of compile-time configuration and hints from the
** build environment to get the default/expected extension for a given file
** category.
**
** \returns String containing the extension
** \retval  NULL if no default extension is known for the given file category
*/
const char *CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_t FileCategory);

/*****************************************************************************/
/**
** \brief Parse a filename input from an input buffer into a local buffer
**
** \par Description
**        This provides a more user friendly way to specify file names,
**        using default values for the path and extension, which can
**        vary from system to system.
**
**        If InputBuffer is null or its length is zero, then DefaultInput
**        is used as if it was the content of the input buffer.
**
**        If either the pathname or extension is missing from the input,
**        it will be added from defaults, with the complete fully-qualified
**        filename stored in the output buffer.
**
** \par Assumptions, External Events, and Notes:
**        -# The paths and filenames used here are the standard unix style
**            filenames separated by "/" (path) and "." (extension) characters.
**        -# Input Buffer has a fixed max length.  Parsing will not exceed InputBufSize,
**            and does not need to be null terminated.  However parsing will stop
**            at the first null char, when the input is shorter than the maximum.
**
** \param[out] OutputBuffer     Buffer to store result @nonnull.
** \param[in]  InputBuffer      A input buffer that may contain a file name (e.g. from command) @nonnull.
** \param[in]  OutputBufSize    Maximum Size of output buffer @nonzero.
** \param[in]  InputBufSize     Maximum Size of input buffer.
** \param[in]  DefaultInput     Default value to use for input if InputBffer is empty
** \param[in]  DefaultPath      Default value to use for pathname if omitted from input
** \param[in]  DefaultExtension Default value to use for extension if omitted from input
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_FS_BAD_ARGUMENT    \copybrief CFE_FS_BAD_ARGUMENT
** \retval #CFE_FS_FNAME_TOO_LONG  \copybrief CFE_FS_FNAME_TOO_LONG
** \retval #CFE_FS_INVALID_PATH    \copybrief CFE_FS_INVALID_PATH
** \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
**
******************************************************************************/
int32 CFE_FS_ParseInputFileNameEx(char *OutputBuffer, const char *InputBuffer, size_t OutputBufSize,
                                  size_t InputBufSize, const char *DefaultInput, const char *DefaultPath,
                                  const char *DefaultExtension);

/*****************************************************************************/
/**
** \brief Parse a filename string from the user into a local buffer
**
** \par Description
**        Simplified API for CFE_FS_ParseInputFileNameEx() where input is
**        always known to be a non-empty, null terminated string and the fixed-length
**        input buffer not needed.  For instance this may be used where
**        the input is a fixed string from cfe_platform_cfg.h or similar.
**
** \par Assumptions, External Events, and Notes:
**        The parameters are organized such that this is basically like strncpy() with an
**        extra argument, and existing file name accesses which use a direct copy can
**        easily change to use this instead.
**
** \sa CFE_FS_ParseInputFileNameEx()
**
** \param[out] OutputBuffer     Buffer to store result @nonnull.
** \param[in]  InputName        A null terminated input string @nonnull.
** \param[in]  OutputBufSize    Maximum Size of output buffer @nonzero.
** \param[in]  FileCategory     The generalized category of file (implies default path/extension)
**
** \return Execution status, see \ref CFEReturnCodes
**
**---------------------------------------------------------------------------------------
*/
int32 CFE_FS_ParseInputFileName(char *OutputBuffer, const char *InputName, size_t OutputBufSize,
                                CFE_FS_FileCategory_t FileCategory);

/*****************************************************************************/
/**
** \brief Extracts the filename from a unix style path and filename string.
**
** \par Description
**        This API will take the original unix path/filename combination and
**        extract the base filename. Example: Given the path/filename : "/cf/apps/myapp.o.gz"
**        this function will return the filename: "myapp.o.gz".
**
** \par Assumptions, External Events, and Notes:
**        -# The paths and filenames used here are the standard unix style
**            filenames separated by "/" characters.
**        -# The extracted filename (including terminator) is no longer than #OS_MAX_PATH_LEN
**
** \param[in] OriginalPath The original path @nonnull
** \param[out] FileNameOnly The filename that is extracted from the path @nonnull
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_FS_BAD_ARGUMENT    \copybrief CFE_FS_BAD_ARGUMENT
** \retval #CFE_FS_FNAME_TOO_LONG  \copybrief CFE_FS_FNAME_TOO_LONG
** \retval #CFE_FS_INVALID_PATH    \copybrief CFE_FS_INVALID_PATH
** \retval #CFE_SUCCESS            \copybrief CFE_SUCCESS
**
******************************************************************************/
CFE_Status_t CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly);

/*****************************************************************************/
/**
** \brief Register a background file dump request
**
** \par Description
**        Puts the previously-initialized metadata into the pending request queue
**
** \par Assumptions, External Events, and Notes:
**        Metadata structure should be stored in a persistent memory area (not on stack) as
**        it must remain accessible by the file writer task throughout the asynchronous
**        job operation.
**
** \param[inout] Meta        The background file write persistent state object @nonnull
**
** \return Execution status, see \ref CFEReturnCodes
** \retval #CFE_FS_BAD_ARGUMENT                 \copybrief CFE_FS_BAD_ARGUMENT
** \retval #CFE_FS_INVALID_PATH                 \copybrief CFE_FS_INVALID_PATH
** \retval #CFE_STATUS_REQUEST_ALREADY_PENDING  \copybrief CFE_STATUS_REQUEST_ALREADY_PENDING
** \retval #CFE_SUCCESS                         \copybrief CFE_SUCCESS
**
******************************************************************************/
int32 CFE_FS_BackgroundFileDumpRequest(CFE_FS_FileWriteMetaData_t *Meta);

/*****************************************************************************/
/**
** \brief Query if a background file write request is currently pending
**
** \par Description
**        This returns "true" while the request is on the background work queue
**        This returns "false" once the request is complete and removed from the queue.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[inout] Meta        The background file write persistent state object @nonnull
**
** \return boolean value indicating if request is already pending
** \retval true if request is pending
** \retval false if request is not pending
**
******************************************************************************/
bool CFE_FS_BackgroundFileDumpIsPending(const CFE_FS_FileWriteMetaData_t *Meta);

/**@}*/

#endif /* CFE_FS_H */
