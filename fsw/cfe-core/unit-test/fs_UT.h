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
** File:
**    fs_UT.h
**
** Purpose:
**    FS unit test header
**
** References:
**    1. cFE Application Developers Guide
**    2. unit test standard 092503
**    3. C Coding Standard 102904
**
** Notes:
**    1. This is unit test code only, not for use in flight
**
*/
#ifndef _fs_UT_h_
#define _fs_UT_h_

/*
** Includes
*/
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "cfe.h"
#include "cfe_fs.h"
#include "cfe_fs_priv.h"
#include "common_types.h"
#include "ut_support.h"
#include "ut_fs_zipfile.h"

/* FS unit test functions */
/*****************************************************************************/
/**
** \brief Master function for the FS API test functions
**
** \par Description
**        This function calls the FS API test functions.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text
**
******************************************************************************/
void Test_CFE_FS_API(void);

/*****************************************************************************/
/**
** \brief Test FS API initialize header function
**
** \par Description
**        This function tests the initialize header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Report, #CFE_FS_InitHeader
**
******************************************************************************/
void Test_CFE_FS_InitHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API read header function
**
** \par Description
**        This function tests the read header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_ReadHeader,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_FS_ReadHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API write header function
**
** \par Description
**        This function tests the write header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_WriteHeader,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_FS_WriteHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API set time stamp function
**
** \par Description
**        This function tests the set time stamp function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_SetTimestamp,
** \sa #UT_SetRtnCode
**
******************************************************************************/
void Test_CFE_FS_SetTimestamp(void);

/*****************************************************************************/
/**
** \brief Test FS API byte swap cFE header function
**
** \par Description
**        This function tests the byte swap cFE header function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_FS_ByteSwapCFEHeader, #UT_Report
**
******************************************************************************/
void Test_CFE_FS_ByteSwapCFEHeader(void);

/*****************************************************************************/
/**
** \brief Test FS API byte swap uint32 function
**
** \par Description
**        This function tests the byte swap uint32 function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #CFE_FS_ByteSwapUint32, #UT_Report
**
******************************************************************************/
void Test_CFE_FS_ByteSwapUint32(void);

/*****************************************************************************/
/**
** \brief Test FS API is .gz file function
**
** \par Description
**        This function tests the "is .gz file" function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_FS_IsGzFile
**
******************************************************************************/
void Test_CFE_FS_IsGzFile(void);

/*****************************************************************************/
/**
** \brief Test FS API write extract file name from path function
**
** \par Description
**        This function tests the "write extract file name from path" function.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_FS_ExtractFilenameFromPath
**
******************************************************************************/
void Test_CFE_FS_ExtractFileNameFromPath(void);

/*****************************************************************************/
/**
** \brief Tests for FS private functions (cfe_fs_priv.c)
**
** \par Description
**        This function tests the FS private functions (cfe_fs_priv.c).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_Report, #CFE_FS_EarlyInit,
** \sa #CFE_FS_LockSharedData, #UT_SetRtnCode, #CFE_FS_UnlockSharedData
**
******************************************************************************/
void Test_CFE_FS_Private(void);

/*****************************************************************************/
/**
** \brief Tests for FS decompress functions (cfe_fs_decompress.c)
**
** \par Description
**        This function tests the FS decompress functions
**        (cfe_fs_decompress.c).
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
** \sa #UT_Text, #UT_InitData, #UT_SetOSFail, #UT_Report, #CFE_FS_Decompress,
** \sa #UT_SetReadBuffer, #FS_gz_inflate_fixed, #FS_gz_inflate_stored,
** \sa #FS_gz_fill_inbuf, #FS_gz_flush_window
**
******************************************************************************/
void Test_CFE_FS_Decompress(void);

/*****************************************************************************/
/**
** \brief Tests for CFE_FS_GetUncompressedFile function
**
** \par Description
**        This function tests the CFE_FS_GetUncompressedFile() function
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void Test_CFE_FS_GetUncompressedFile(void);

#endif /* _es_ut_h_ */
