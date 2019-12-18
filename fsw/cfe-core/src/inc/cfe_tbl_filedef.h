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
**  File: cfe_tbl_filedef.h
**
**  Title:   ELF2CFETBL Utility Header File for Table Images
**
**  Purpose:
**     This header file provides a data structure definition and macro definition
**     required in source code that is intended to be compiled into a cFE compatible
**     Table Image file.
**
**  Design Notes:
**
**     Typically, a user would include this file in a ".c" file that contains nothing
**     but a desired instantiation of values for a table image along with the macro
**     defined below.  After compilation, the resultant elf file can be processed using
**     the 'elf2cfetbl' utility to generate a file that can be loaded onto a cFE flight
**     system and successfully loaded into a table using the cFE Table Services.
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**
**  Notes:
**
*/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_tbl_filedef_
#define _cfe_tbl_filedef_

#include "cfe.h"

/* CFE_TBL_MAX_FULL_NAME_LEN is defined in cfe_tbl.h and includes alignment bytes */
#define CFE_TBL_FILDEF_MAX_NAME_LEN       (CFE_TBL_MAX_FULL_NAME_LEN)

/* Compute number of additional bytes needed to make               */
/* CFE_FS_HDR_DESC_MAX_LEN rounded up to nearest longword boundary */
#define CFE_TBL_FILEDEF_FS_HDR_ALIGN32    (CFE_FS_HDR_DESC_MAX_LEN - ((CFE_FS_HDR_DESC_MAX_LEN/4)*4))

/* Allocate enough space for maximum table description length plus alignment bytes */
#define CFE_TBL_FILDEF_FS_HDR_LEN         (CFE_FS_HDR_DESC_MAX_LEN + CFE_TBL_FILEDEF_FS_HDR_ALIGN32)

/* Compute number of additional bytes needed to make        */
/* OS_MAX_FILE_NAME rounded up to nearest longword boundary */
#define CFE_TBL_FILEDEF_OS_FILE_ALIGN32   (OS_MAX_FILE_NAME - ((OS_MAX_FILE_NAME/4)*4))

/* Allocate enough space for maximum file name length plus alignment bytes */
#define CFE_TBL_FILDEF_OS_FILE_LEN        (OS_MAX_FILE_NAME + CFE_TBL_FILEDEF_OS_FILE_ALIGN32)

typedef struct
{
    uint32                   Reserved;                             /**< Future Use: NumTblSegments in File?   */
    uint32                   Offset;                               /**< Byte Offset at which load should commence */
    uint32                   NumBytes;                             /**< Number of bytes to load into table */
    char                     TableName[CFE_TBL_MAX_FULL_NAME_LEN]; /**< Fully qualified name of table to load */
} CFE_TBL_File_Hdr_t;


typedef struct
{
    char        ObjectName[64];                           /**< \brief Name of instantiated variable that contains desired table image */
    char        TableName[CFE_TBL_FILDEF_MAX_NAME_LEN];   /**< \brief Name of Table as defined onboard */
    char        Description[CFE_TBL_FILDEF_FS_HDR_LEN];   /**< \brief Description of table image that is included in cFE File Header */
    char        TgtFilename[CFE_TBL_FILDEF_OS_FILE_LEN];  /**< \brief Default filename to be used for output of elf2cfetbl utility  */
    uint32      ObjectSize;                               /**< \brief Size, in bytes, of instantiated object */
} CFE_TBL_FileDef_t;

/** The CFE_TBL_FILEDEF macro can be used to simplify the declaration of a table image when using the elf2cfetbl utility.
**  An example of the source code and how this macro would be used is as follows: \code
 
    #include "cfe_tbl_filedef.h"

   typedef struct
   {
       int     Int1;
       int     Int2;
       int     Int3;
       char    Char1;
   } MyTblStruct_t;

   MyTblStruct_t MyTblStruct = { 0x01020304, 0x05060708, 0x090A0B0C, 0x0D };

   CFE_TBL_FILEDEF(MyTblStruct, MyApp.TableName, Table Utility Test Table, MyTblDefault.bin )
   
\endcode
*/
  
#define CFE_TBL_FILEDEF(ObjName, TblName, Desc, Filename) static OS_USED CFE_TBL_FileDef_t CFE_TBL_FileDef={#ObjName, #TblName, #Desc, #Filename, sizeof(ObjName)};

/*************************************************************************/

#endif /* _cfe_tbl_filedef_ */

/************************/
/*  End of File Comment */
/************************/
