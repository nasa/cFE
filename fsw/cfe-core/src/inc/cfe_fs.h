/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/*
** File: cfe_fs.h
**
** Purpose:  cFE File Services (FS) library API header file
**
** Author:   S.Walling/Microtel
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_fs_
#define _cfe_fs_


/*
** Required header files...
*/
#include "cfe_fs_extern_typedefs.h"
#include "cfe_error.h"
#include "common_types.h"
#include "cfe_time.h"


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
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_OpenCreate
**                    that is associated with the file whose header is to be read.
**
** \param[in, out] Hdr     Pointer to a variable of type #CFE_FS_Header_t that will be
**                    filled with the contents of the Standard cFE File Header. *Hdr is the contents of the Standard cFE File Header for the specified file.
**
** \return Execution status, see \ref CFEReturnCodes
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
** \param[in] *Description Initializes Header's Description
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
**      -# \link #CFE_FS_Header_t::TimeSeconds \c TimeSeconds \endlink - Filled with the Time, in seconds, as obtained by #CFE_TIME_GetTime
**      -# \link #CFE_FS_Header_t::TimeSubSeconds \c TimeSubSeconds \endlink - Filled with the Time, subseconds, as obtained by #CFE_TIME_GetTime
**        
**        
** \par Assumptions, External Events, and Notes:
**        -# The File has already been successfully opened using #OS_OpenCreate and
**           the caller has a legitimate File Descriptor.
**        -# The \c SubType field has been filled appropriately by the Application.
**        -# The \c Description field has been filled appropriately by the Application.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_OpenCreate
**                    that is associated with the file whose header is to be read.
**
** \param[in, out] Hdr     Pointer to a variable of type #CFE_FS_Header_t that will be
**                    filled with the contents of the Standard cFE File Header. *Hdr is the contents of the Standard cFE File Header for the specified file.
**
** \return Execution status, see \ref CFEReturnCodes
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
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_OpenCreate
**                    that is associated with the file whose header is to be read.
**
** \param[in] NewTimestamp A #CFE_TIME_SysTime_t data structure containing the desired time
**                         to be put into the file's Standard cFE File Header.
**
** \return Execution status, see \ref CFEReturnCodes
**               
******************************************************************************/
CFE_Status_t CFE_FS_SetTimestamp(osal_id_t FileDes, CFE_TIME_SysTime_t NewTimestamp);
/**@}*/


/** @defgroup CFEAPIFSUtil cFE File Utility APIs
 * @{
 */

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
** \param[in] OriginalPath The original path.
** \param[out] FileNameOnly The filename that is extracted from the path.
**
** \return Execution status, see \ref CFEReturnCodes
**
******************************************************************************/
CFE_Status_t CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly);
/**@}*/

#endif /* _cfe_fs_ */

/************************/
/*  End of File Comment */
/************************/
