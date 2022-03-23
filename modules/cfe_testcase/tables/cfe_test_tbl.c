/**
 * \file
 *   Create a file containing a CFE Test Table
 */

/*
 * Includes
 */

#include "cfe_tbl_filedef.h"
#include "cfe_test_tbl.h"

/*
 * The test table data should contain some identifiable numeric values,
 * so any issues with paritial loading/byteswapping are morely likely
 * to be detected.
 */
TBL_TEST_Table_t TestTable = {0xf007, 0xba11};

CFE_TBL_FILEDEF(TestTable, CFE_TEST_APP.TestTable, Table Test Table, cfe_test_tbl.tbl)
