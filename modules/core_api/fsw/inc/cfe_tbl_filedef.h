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
 *  Title:   ELF2CFETBL Utility Header File for Table Images
 *
 *  Purpose:
 *     This header file provides a data structure definition and macro definition
 *     required in source code that is intended to be compiled into a cFE compatible
 *     Table Image file.
 *
 *  Design Notes:
 *
 *     Typically, a user would include this file in a ".c" file that contains nothing
 *     but a desired instantiation of values for a table image along with the macro
 *     defined below.  After compilation, the resultant elf file can be processed using
 *     the 'elf2cfetbl' utility to generate a file that can be loaded onto a cFE flight
 *     system and successfully loaded into a table using the cFE Table Services.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 *  Notes:
 *
 */

#ifndef CFE_TBL_FILEDEF_H
#define CFE_TBL_FILEDEF_H

#include "cfe_mission_cfg.h"
#include "common_types.h"
#include "cfe_tbl_extern_typedefs.h" /* for "CFE_TBL_FileHdr_t" definition */
#include "cfe_fs_extern_typedefs.h"  /* for "CFE_FS_HDR_DESC_MAX_LEN" definition */

/**
 * \brief Table File summary object
 *
 * The definition of the file definition metadata that can be used by
 * external tools (e.g. elf2cfetbl) to generate CFE table data files.
 */
typedef struct CFE_TBL_FileDef
{
    /**
     * \brief Name of instantiated variable that contains desired table image
     *
     * \note For consistency and future compatibility with auto-generated table files
     * and table definitions, the "ObjectName" should match the table struct typedef
     * name without the "_t" suffix.  For example, the limit checker action table (ADT)
     * is defined by a type called "LC_ADT_t", the ObjectName should be "LC_ADT".
     *
     * This naming convention allows the type name to be inferred from the ObjectName
     * (and vice-versa) without having to directly specify both the type name and object
     * name here.
     *
     * Although the traditional elf2cfetbl tool does not currently do any type checking,
     * future tool versions may add more robust type verification and therefore need to
     * know the type name as well as the object name.
     */
    char ObjectName[64];

    /**
     * \brief Name of Table as defined onboard
     *
     * This should be in the form of "APP_NAME.TABLE_NAME" where APP_NAME matches what
     * the app is named at runtime (the 4th column of cfe_es_startup.scr) and TABLE_NAME
     * matches the 2nd parameter of the call to CFE_TBL_Register().  Preferably the
     * TABLE_NAME should also match the ObjectName here in this structure, although this
     * is not strictly required, it helps keep things consistent.
     */
    char TableName[CFE_MISSION_TBL_MAX_FULL_NAME_LEN];

    /**
     * \brief Description of table image that is included in cFE File Header
     *
     * This is a free-form text string that can be any meaningful value
     */
    char Description[CFE_FS_HDR_DESC_MAX_LEN];

    /**
     * \brief Default filename to be used for output of elf2cfetbl utility
     *
     * This must match the expected table file name, which is the name of the source file but
     * the ".c" extension replaced with ".tbl".  This is the filename only - do not include a
     * directory/path name here, it can be copied to any runtime directory on the target by
     * external scripts, but should not be renamed.
     */
    char TgtFilename[CFE_MISSION_MAX_FILE_LEN];

    /**
     * \brief Size, in bytes, of instantiated object
     *
     * This may be used by tools to check for consistency between the actual defined table size
     * and the expected table size.  This is set automatically via the #CFE_TBL_FILEDEF macro.
     */
    uint32 ObjectSize;
} CFE_TBL_FileDef_t;

/**
 * \brief Macro to assist in with table definition object declaration
 *
 * See notes in the #CFE_TBL_FileDef_t structure type about naming conventions and
 * recommended practices for the various fields.
 *
 * The CFE_TBL_FILEDEF macro can be used to simplify the declaration of a table image
 * when using the elf2cfetbl utility.
 *
 *  Note that the macro adds a NULL at the end to ensure that it is null-terminated. (C allows
 *  a struct to be statically initialized with a string exactly the length of the array, which
 *  loses the null terminator.) This means the actual length limit of the fields are the above
 *  LEN - 1.
 *
 *  An example of the source code and how this macro would be used is as follows:
\code

    #include "cfe_tbl_filedef.h"

   typedef struct MyTblStruct
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

#define CFE_TBL_FILEDEF(ObjName, TblName, Desc, Filename) \
    CFE_TBL_FileDef_t CFE_TBL_FileDef = {#ObjName "\0", #TblName "\0", #Desc "\0", #Filename "\0", sizeof(ObjName)};

/*************************************************************************/

#endif /* CFE_TBL_FILEDEF_H */
