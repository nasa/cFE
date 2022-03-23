/**
 * \file
 *   Initialization of variables used by table functional tests and
 *   function definitions for setup and teardown table functions
 */

/*
 * Includes
 */

#include "cfe_test.h"
#include "cfe_test_table.h"

/* Setup function to register a table */
void RegisterTestTable(void)
{

    UtAssert_INT32_EQ(CFE_TBL_Register(&CFE_FT_Global.TblHandle, CFE_FT_Global.TblName, sizeof(TBL_TEST_Table_t),
                                       CFE_TBL_OPT_DEFAULT, NULL),
                      CFE_SUCCESS);
}

/* Teardown function to unregister a table */
void UnregisterTestTable(void)
{
    UtAssert_INT32_EQ(CFE_TBL_Unregister(CFE_FT_Global.TblHandle), CFE_SUCCESS);
}
