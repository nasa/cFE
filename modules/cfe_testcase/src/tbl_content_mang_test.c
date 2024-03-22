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
 * \file
 *   Functional test of Table Manage Content APIs
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_test_table.h"

static const char TESTTBL_OTHER_NAME[] = "OtherTbl";

/* A set of additional (generated) table images to test permutations of CFE_TBL_Load() */
static const char TESTTBL_NOMINAL_FILE[] =
    "/cf/cfe_test_tbl.tbl"; /**< nominal image file, from table tool (elf2cfetbl or similar) */
static const char TESTTBL_NOTEXIST_FILE[] =
    "/ram/notexist.tbl"; /**< a valid filename, but file does not actually exist */
static const char TESTTBL_BAD_STDHDR_FILE[]  = "/ram/testtbl_shdr.tbl"; /**< bad image without a complete FS header */
static const char TESTTBL_BAD_TBLHDR_FILE[]  = "/ram/testtbl_thdr.tbl"; /**< bad image without a complete TBL header */
static const char TESTTBL_BAD_CONTENT_FILE[] = "/ram/testtbl_cid.tbl";  /**< image with incorrect "content ID" field */
static const char TESTTBL_BAD_SUBTYPE_FILE[] = "/ram/testtbl_st.tbl";   /**< image with incorrect "subtype" field */
static const char TESTTBL_ALTERNATE_FILE[] = "/ram/testtbl_alt.tbl"; /**< good/complete image with different content */
static const char TESTTBL_OTHERTBL_FILE[] =
    "/ram/testtbl_other.tbl"; /**< good/complete image with different content for a different table */
static const char TESTTBL_TRUNCATED_FILE[] =
    "/ram/testtbl_trunc.tbl"; /**< truncated version (header info has more bytes than file) */
static const char TESTTBL_LONG_FILE[] =
    "/ram/testtbl_long.tbl"; /**< long version (file has more bytes than header info) */
static const char TESTTBL_SHORT_FILE[] =
    "/ram/testtbl_short.tbl"; /**< short version (header info matches file but smaller than tbl) */
static const char TESTTBL_PARTIAL_FILE[] =
    "/ram/testtbl_part.tbl"; /**< partial (offset nonzero, remainder of data from short file) */

void TestLoad(void)
{
    CFE_TBL_Handle_t      BadTblHandle;
    const char *          BadTblName = "BadTableName";
    CFE_TBL_Handle_t      DumpTblHandle;
    const char *          DumpTblName = "DumpOnlyTable";
    CFE_TBL_Handle_t      SharedTblHandle;
    const char *          SharedTblName = CFE_FT_Global.RegisteredTblName;
    CFE_TEST_TestTable_t  TestTable     = {0xd00d, 0xdad};
    CFE_TEST_TestTable_t *TablePtr;
    CFE_TBL_Handle_t      OtherHandle;
    void *                TempPtr;

    UtPrintf("Testing: CFE_TBL_Load");

    UtAssert_INT32_EQ(
        CFE_TBL_Register(&BadTblHandle, BadTblName, sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DBL_BUFFER, NULL),
        CFE_SUCCESS);

    /* Create a second table handle, to keep things interesting */
    UtAssert_INT32_EQ(CFE_TBL_Register(&OtherHandle, TESTTBL_OTHER_NAME, sizeof(CFE_TEST_TestTable_t), 0, NULL),
                      CFE_SUCCESS);

    /* Some basic failure checks */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_BAD_STDHDR_FILE),
                      CFE_TBL_ERR_NO_STD_HEADER);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_BAD_TBLHDR_FILE),
                      CFE_TBL_ERR_NO_TBL_HEADER);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_BAD_CONTENT_FILE),
                      CFE_TBL_ERR_BAD_CONTENT_ID);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_BAD_SUBTYPE_FILE),
                      CFE_TBL_ERR_BAD_SUBTYPE_ID);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, ~CFE_TBL_SRC_FILE, TESTTBL_NOMINAL_FILE),
                      CFE_TBL_ERR_ILLEGAL_SRC_TYPE);

    /* Load from partial file (offset nonzero, before any successful load) -
     * This should be restricted and return an error  */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_PARTIAL_FILE),
                      CFE_TBL_ERR_PARTIAL_LOAD);

    /* Load from short file (offset 0, but incomplete, also before any successful load) */
    /* In the current TBL implementation, this actually returns SUCCESS here (which is misleading) */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_SHORT_FILE), CFE_SUCCESS);

    /* NOTE: _NOT_ checking content after above; although it returned a success code, it is not well defined
     * as to what the content will be because the data was never fully loaded yet */

    /* Load from file (nominal) - first full data load */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_NOMINAL_FILE), CFE_SUCCESS);

    /* confirm content (football) */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0xf007);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0xba11);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* Load from file too big */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_LONG_FILE),
                      CFE_TBL_ERR_FILE_TOO_LARGE);

    /* confirm content again (note content should not have been updated) */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, CFE_FT_Global.TblHandle), CFE_SUCCESS);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0xf007);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0xba11);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* Load again with alternate content */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_ALTERNATE_FILE), CFE_SUCCESS);

    /* confirm content again (changed to alternate data) */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0xdead);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0xbeef);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* Load from file truncated */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_TRUNCATED_FILE),
                      CFE_TBL_ERR_LOAD_INCOMPLETE);

    /* confirm content again (should not be changed) */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, CFE_FT_Global.TblHandle), CFE_SUCCESS);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0xdead);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0xbeef);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* Load the other table (nominal data) */
    UtAssert_INT32_EQ(CFE_TBL_Load(OtherHandle, CFE_TBL_SRC_FILE, TESTTBL_OTHERTBL_FILE), CFE_SUCCESS);

    /* confirm content of first table again (should not be changed) */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, CFE_FT_Global.TblHandle), CFE_SUCCESS);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0xdead);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0xbeef);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* confirm content of other table (boatload) */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, OtherHandle), CFE_TBL_INFO_UPDATED);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0xb0a7);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0x10ad);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(OtherHandle), CFE_SUCCESS);

    /* Load from short file again (different response after successful load) */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_SHORT_FILE), CFE_SUCCESS);

    /* confirm content again (reported as updated from partial load) */
    /* Should have updated the first word only */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0x5555);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0xbeef);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* Load from short file again (different response after successful load) */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_PARTIAL_FILE), CFE_SUCCESS);

    /* confirm content again (reported as updated from partial load) */
    /* Should have updated the second word only */
    UtAssert_INT32_EQ(CFE_TBL_GetAddress(&TempPtr, CFE_FT_Global.TblHandle), CFE_TBL_INFO_UPDATED);
    TablePtr = TempPtr;
    UtAssert_UINT32_EQ(TablePtr->Int1, 0x5555);
    UtAssert_UINT32_EQ(TablePtr->Int2, 0x6666);
    UtAssert_INT32_EQ(CFE_TBL_ReleaseAddress(CFE_FT_Global.TblHandle), CFE_SUCCESS);

    /* Table name mismatches */
    UtAssert_INT32_EQ(CFE_TBL_Load(BadTblHandle, CFE_TBL_SRC_FILE, TESTTBL_NOMINAL_FILE),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_OTHERTBL_FILE),
                      CFE_TBL_ERR_FILE_FOR_WRONG_TABLE);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_FILE, TESTTBL_NOTEXIST_FILE),
                      CFE_TBL_ERR_ACCESS);

    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_TBL_BAD_TABLE_HANDLE, CFE_TBL_SRC_FILE, TESTTBL_NOMINAL_FILE),
                      CFE_TBL_ERR_INVALID_HANDLE);

    /* Load from memory */
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_ADDRESS, &TestTable), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_FT_Global.TblHandle, CFE_TBL_SRC_ADDRESS, NULL), CFE_TBL_BAD_ARGUMENT);
    UtAssert_INT32_EQ(CFE_TBL_Load(CFE_TBL_BAD_TABLE_HANDLE, CFE_TBL_SRC_ADDRESS, &TestTable),
                      CFE_TBL_ERR_INVALID_HANDLE);

    /* Attempt to load a dump only table */
    UtAssert_INT32_EQ(
        CFE_TBL_Register(&DumpTblHandle, DumpTblName, sizeof(CFE_TEST_TestTable_t), CFE_TBL_OPT_DUMP_ONLY, NULL),
        CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Load(DumpTblHandle, CFE_TBL_SRC_FILE, TESTTBL_NOMINAL_FILE), CFE_TBL_ERR_DUMP_ONLY);

    /* Load a shared table */
    UtAssert_INT32_EQ(CFE_TBL_Share(&SharedTblHandle, SharedTblName), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Load(SharedTblHandle, CFE_TBL_SRC_FILE, TESTTBL_NOMINAL_FILE), CFE_SUCCESS);
}

void TestUpdate(void)
{
    UtPrintf("Testing: CFE_TBL_Update");
    /* Haven't figured out how to get an update pending */
    UtAssert_INT32_EQ(CFE_TBL_Update(CFE_FT_Global.TblHandle), CFE_TBL_INFO_NO_UPDATE_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_Update(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestValidate(void)
{
    UtPrintf("Testing: CFE_TBL_Validate");
    /* Haven't figured out how to get a validation pending */
    UtAssert_INT32_EQ(CFE_TBL_Validate(CFE_FT_Global.TblHandle), CFE_TBL_INFO_NO_VALIDATION_PENDING);
    UtAssert_INT32_EQ(CFE_TBL_Validate(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestManage(void)
{
    UtPrintf("Testing: CFE_TBL_Manage");
    UtAssert_INT32_EQ(CFE_TBL_Manage(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Manage(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestDumpToBuffer(void)
{
    UtPrintf("Testing: CFE_TBL_DumpToBuffer");
    /* This should at least return an info code such as CFE_TBL_INFO_NO_UPDATE_PENDING when CFE_TBL_Update is called
     * with no pending update instead of returning CFE_SUCCESS whether or not it actually dumped*/
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_DumpToBuffer(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

void TestModified(void)
{
    UtPrintf("Testing: CFE_TBL_Modified");
    UtAssert_INT32_EQ(CFE_TBL_Modified(CFE_FT_Global.TblHandle), CFE_SUCCESS);
    UtAssert_INT32_EQ(CFE_TBL_Modified(CFE_TBL_BAD_TABLE_HANDLE), CFE_TBL_ERR_INVALID_HANDLE);
}

/* Helper function to set a 32-bit table offset value (must be big-endian) */
void TblTest_UpdateOffset(uint32 *TgtVal, size_t SetVal)
{
    size_t i;
    union
    {
        uint32 offset;
        uint8  bytes[sizeof(uint32)];
    } offsetbuf;

    i = sizeof(offsetbuf.bytes);
    while (i > 0)
    {
        --i;
        offsetbuf.bytes[i] = SetVal & 0xFF;
        SetVal >>= 8;
    }

    *TgtVal = offsetbuf.offset;
}

/*
 * A helper function that intentionally creates flawed table image files -
 * This takes the good image file produced during the build, and creates
 * variants with certain header fields modified and data truncated, to
 * validate the error detection logic in CFE_TBL_Load().
 */
void TblTest_GenerateTblFiles(void)
{
    osal_id_t fh1 = OS_OBJECT_ID_UNDEFINED;
    osal_id_t fh2 = OS_OBJECT_ID_UNDEFINED;
    size_t    PartialOffset;
    size_t    PartialSize;
    union
    {
        uint8                u8;
        uint16               u16;
        uint32               u32;
        CFE_FS_Header_t      FsHdr;
        CFE_TBL_File_Hdr_t   TblHdr;
        CFE_TEST_TestTable_t Content;
    } buf;

    /* Open the original (correct) table image file for reference */
    UtAssert_INT32_EQ(OS_OpenCreate(&fh1, TESTTBL_NOMINAL_FILE, 0, OS_READ_ONLY), OS_SUCCESS);

    /* create a file which does not have a valid FS header */
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_BAD_STDHDR_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);
    buf.u32 = 0x12345678;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.u32)), sizeof(buf.u32));
    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* create a file which has an FS header but not a valid TBL header */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_BAD_TBLHDR_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    buf.u32 = 0x12345678;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.u32)), sizeof(buf.u32));
    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Create a tbl image that has the wrong content ID field */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_BAD_CONTENT_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    /* copy headers and modify */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    buf.FsHdr.ContentType = 0x09abcdef;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* Identifiable content, different from original */
    buf.Content.Int1 = 0x7777;
    buf.Content.Int2 = 0x8888;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.Content)), sizeof(buf.Content));

    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Create a tbl image that has the wrong content ID field */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_BAD_SUBTYPE_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    /* copy headers as-is */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    buf.FsHdr.SubType = 0x09abcdef;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* Identifiable content, different from original */
    buf.Content.Int1 = 0x9999;
    buf.Content.Int2 = 0xaaaa;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.Content)), sizeof(buf.Content));

    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Create a tbl image that is complete but with different content */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_ALTERNATE_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    /* copy headers as-is */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* Identifiable content, different from original */
    buf.Content.Int1 = 0xdead;
    buf.Content.Int2 = 0xbeef;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.Content)), sizeof(buf.Content));

    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Create a tbl image that is complete but for the OTHER table (also different content) */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_OTHERTBL_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    /* copy headers as-is */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    snprintf(buf.TblHdr.TableName, sizeof(buf.TblHdr.TableName), "%s.%s", "CFE_TEST_APP", TESTTBL_OTHER_NAME);
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* Identifiable content, different from original */
    buf.Content.Int1 = 0xb0a7;
    buf.Content.Int2 = 0x10ad;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.Content)), sizeof(buf.Content));

    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);
    /* Create a tbl image that is too big */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_LONG_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY), OS_SUCCESS);

    /* copy headers as-is */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* write content, but make it identifiable/unique */
    buf.Content.Int1 = 0x1111;
    buf.Content.Int1 = 0x2222;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.Content)), sizeof(buf.Content));

    /* Write extra byte at the end */
    buf.u8 = 0x33;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.u8)), sizeof(buf.u8));
    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Create a tbl image that is truncated; header byte count is larger than file */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_TRUNCATED_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    /* copy headers as-is */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* But write only one byte of data into the content part (so will be too small) */
    buf.u8 = 0x44;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.u8)), sizeof(buf.u8));

    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Make a file that is "short" (byte count is correct, just not enough bytes to fill table) */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_SHORT_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    /* copy headers, but modify TBL header */
    /* NOTE: headers must be in big-endian/network byte order! */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    PartialOffset = 0;
    PartialSize   = offsetof(CFE_TEST_TestTable_t, Int2);
    TblTest_UpdateOffset(&buf.TblHdr.Offset, PartialOffset);
    TblTest_UpdateOffset(&buf.TblHdr.NumBytes, PartialSize);
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* write partial content */
    buf.Content.Int1 = 0x5555;
    UtAssert_INT32_EQ(OS_write(fh2, &buf, PartialSize), PartialSize);
    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Make a file that is "partial" (contains remainder of bytes from above) */
    OS_lseek(fh1, 0, OS_SEEK_SET);
    UtAssert_INT32_EQ(
        OS_OpenCreate(&fh2, TESTTBL_PARTIAL_FILE, OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY),
        OS_SUCCESS);

    /* copy headers, but modify TBL header */
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.FsHdr)), sizeof(buf.FsHdr));
    UtAssert_INT32_EQ(OS_read(fh1, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));
    PartialOffset = offsetof(CFE_TEST_TestTable_t, Int2);
    PartialSize   = sizeof(buf.Content) - offsetof(CFE_TEST_TestTable_t, Int2);
    TblTest_UpdateOffset(&buf.TblHdr.Offset, PartialOffset);
    TblTest_UpdateOffset(&buf.TblHdr.NumBytes, PartialSize);
    UtAssert_INT32_EQ(OS_write(fh2, &buf, sizeof(buf.TblHdr)), sizeof(buf.TblHdr));

    /* Copy partial content */
    buf.Content.Int2 = 0x6666;
    UtAssert_INT32_EQ(OS_write(fh2, &buf.Content.Int2, PartialSize), PartialSize);
    UtAssert_INT32_EQ(OS_close(fh2), OS_SUCCESS);

    /* Close the source file */
    UtAssert_INT32_EQ(OS_close(fh1), OS_SUCCESS);
}

void TBLContentMangTestSetup(void)
{
    TblTest_GenerateTblFiles();

    UtTest_Add(TestLoad, RegisterTestTable, UnregisterTestTable, "Test Table Load");
    UtTest_Add(TestUpdate, RegisterTestTable, UnregisterTestTable, "Test Table Update");
    UtTest_Add(TestValidate, RegisterTestTable, UnregisterTestTable, "Test Table Validate");
    UtTest_Add(TestManage, RegisterTestTable, UnregisterTestTable, "Test Table Manage");
    UtTest_Add(TestDumpToBuffer, RegisterTestTable, UnregisterTestTable, "Test Table Dump to Buffer");
    UtTest_Add(TestModified, RegisterTestTable, UnregisterTestTable, "Test Table Modified");
}
