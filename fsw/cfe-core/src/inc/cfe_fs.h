/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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
#include "common_types.h"
#include "cfe_time.h"



/*
 * To preserve source-code compatibility with existing code,
 * this allows the old enum names to still work.  This should
 * be turned off after the new names are established.
 */
#ifndef CFE_OMIT_DEPRECATED_6_6

/*
 * Compatibility Macros for the SubType enumeration
 */
#define CFE_FS_ES_ERLOG_SUBTYPE             CFE_FS_SubType_ES_ERLOG
#define CFE_FS_ES_SYSLOG_SUBTYPE            CFE_FS_SubType_ES_SYSLOG
#define CFE_FS_ES_QUERYALL_SUBTYPE          CFE_FS_SubType_ES_QUERYALL
#define CFE_FS_ES_PERFDATA_SUBTYPE          CFE_FS_SubType_ES_PERFDATA
#define CFE_FS_ES_SHELL_SUBTYPE             CFE_FS_SubType_ES_SHELL
#define CFE_FS_ES_CDS_REG_SUBTYPE           CFE_FS_SubType_ES_CDS_REG
#define CFE_FS_TBL_REG_SUBTYPE              CFE_FS_SubType_TBL_REG
#define CFE_FS_TBL_IMG_SUBTYPE              CFE_FS_SubType_TBL_IMG
#define CFE_FS_EVS_APPDATA_SUBTYPE          CFE_FS_SubType_EVS_APPDATA
#define CFE_FS_EVS_EVENTLOG_SUBTYPE         CFE_FS_SubType_EVS_EVENTLOG
#define CFE_FS_SB_PIPEDATA_SUBTYPE          CFE_FS_SubType_SB_PIPEDATA
#define CFE_FS_SB_ROUTEDATA_SUBTYPE         CFE_FS_SubType_SB_ROUTEDATA
#define CFE_FS_SB_MAPDATA_SUBTYPE           CFE_FS_SubType_SB_MAPDATA
#define CFE_FS_ES_QUERYALLTASKS_SUBTYPE     CFE_FS_SubType_ES_QUERYALLTASKS


#endif  /* CFE_OMIT_DEPRECATED_6_6 */

/*
** File header access functions...
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
**        -# The File has already been successfully opened using #OS_open and
**           the caller has a legitimate File Descriptor.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_open
**                    that is associated with the file whose header is to be read.
**
** \param[in] Hdr     Pointer to a variable of type #CFE_FS_Header_t that will be
**                    filled with the contents of the Standard cFE File Header.
**
** \param[out] *Hdr   Contents of the Standard cFE File Header for the specified file.
**
** \returns
** \retstmt Any of the return codes specified for #OS_lseek or #OS_read  \endstmt
** \endreturns
**
** \sa #CFE_FS_WriteHeader
**
******************************************************************************/
int32 CFE_FS_ReadHeader(CFE_FS_Header_t *Hdr, int32 FileDes);

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
**        -# The File has already been successfully opened using #OS_open and
**           the caller has a legitimate File Descriptor.
**        -# The \c SubType field has been filled appropriately by the Application.
**        -# The \c Description field has been filled appropriately by the Application.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_open
**                    that is associated with the file whose header is to be read.
**
** \param[in] Hdr     Pointer to a variable of type #CFE_FS_Header_t that will be
**                    filled with the contents of the Standard cFE File Header.
**
** \param[out] *Hdr   Contents of the Standard cFE File Header for the specified file.
**
** \returns
** \retstmt Any of the return codes specified for #OS_lseek or #OS_write       \endstmt
** \endreturns
**
** \sa #CFE_FS_ReadHeader
**
******************************************************************************/
int32 CFE_FS_WriteHeader(int32 FileDes, CFE_FS_Header_t *Hdr);

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
**        -# The File has already been successfully opened using #OS_open and
**           the caller has a legitimate File Descriptor.
**        -# The \c NewTimestamp field has been filled appropriately by the Application.
**
** \param[in] FileDes File Descriptor obtained from a previous call to #OS_open
**                    that is associated with the file whose header is to be read.
**
** \param[in] NewTimestamp A #CFE_TIME_SysTime_t data structure containing the desired time
**                         to be put into the file's Standard cFE File Header.
**
** \returns
** \retstmt Any of the return codes specified for #OS_lseek or #OS_write   \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
int32 CFE_FS_SetTimestamp(int32 FileDes, CFE_TIME_SysTime_t NewTimestamp);


/*****************************************************************************/
/**
** \brief Determines if a file is a Gzip/compressed file.
**
** \par Description
**        This API will check the filename and return true if the file is 
**        a gzip file. The check is currently based on the filename, so the 
**        zipped files should use the ".gz" extention. 
**
** \par Assumptions, External Events, and Notes:
**        -# A gzipped file will use the ".gz" filename extention.
**
** \param[in] FileName The name of the file.
**
** \returns
** \retstmt true if the file has the ".gz" extention and false otherwise.   \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
bool CFE_FS_IsGzFile(const char *FileName);

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
**        -# The extracted filename is no longer than #OS_MAX_PATH_LEN 
**
** \param[in] OriginalPath The original path.
** \param[out] FileNameOnly The filename that is extracted from the path.
**
** \returns
** \retstmt CFE_SUCCESS if the filename was extracted from the path  \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
int32 CFE_FS_ExtractFilenameFromPath(const char *OriginalPath, char *FileNameOnly);

/*****************************************************************************/
/**
** \brief Decompresses the source file to the destination file.
**
** \par Description
**        This API will decompress the source file to the file specified by
**        the destination file. The file must be compressed using the "gzip"
**        utility. This utility is available on most unix workstations, Mac OS X, 
**        Cygwin, and MinGW for Windows. More information can be found at
**         http://www.gzip.org/
**
** \par Assumptions, External Events, and Notes:
**        -# The paths and filenames used here are cfe compliant file names.
**        -# The source file is compressed with the "gzip" utility.
**        -# The destination file does not exist, or can be overwritten. 
**
** \param[in] SourceFile The "gzipped" file to decompress.
** \param[out] DestinationFile The path/filename to write the decompressed or
**             "gunzipped" file to.
**
** \returns
** \retstmt CFE_SUCCESS if the file was decompressed sucessfully.  \endstmt
** \endreturns
**
** \sa
**               
******************************************************************************/
int32 CFE_FS_Decompress( const char * SourceFile, const char * DestinationFile );


/*****************************************************************************/
/**
** \brief Decompresses the source file to a temporary file created in the temp dir
**
** \par Description
**        This is a wrapper around the CFE_FS_Decompress() function that
**        formulates a temporary file name based on the gzip file name, saving
**        the caller from needing to do this.  The temporary file name is
**        created in the given temp directory.
**
** \par Assumptions, External Events, and Notes:
**        The name passed in as "GzipFileName" is not checked again, it is assumed to
**        have passed the criteria in CFE_FS_IsGzFile().  If this is not true then
**        the conversion to a temporary file name may produce incorrect results.
**
** \param[in] OutputNameBuffer  A caller-supplied buffer for storing the temp file name
** \param[in] OutputNameBufferSize  The size of OutputNameBuffer
** \param[in] GzipFileName  The "gzipped" file to decompress.
** \param[in] TempDir   The directory in which the temporary file should be created
**
** \returns
** \retstmt CFE_SUCCESS if the file was decompressed sucessfully.  \endstmt
** \endreturns
**
** \sa
**
******************************************************************************/
int32 CFE_FS_GetUncompressedFile(char *OutputNameBuffer, uint32 OutputNameBufferSize,
        const char *GzipFileName, const char *TempDir);

#endif /* _cfe_fs_ */

/************************/
/*  End of File Comment */
/************************/
