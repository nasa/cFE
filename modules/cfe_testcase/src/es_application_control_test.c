/**
 * \file
 *   Functional test of ES Application Control APIs
 *
 *   Tests only invalid calls to the application control functions.
 *
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_test.h"

void TestApplicationControl(void)
{
    UtPrintf("Testing: CFE_ES_RestartApp, CFE_ES_ReloadApp, CFE_ES_DeleteApp");
    CFE_ES_AppId_t TestAppId;
    UtAssert_INT32_EQ(CFE_ES_GetAppID(&TestAppId), CFE_SUCCESS);

    UtAssert_UINT32_EQ(CFE_ES_RestartApp(CFE_ES_APPID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
    /*
     * This seems a bit strange that it throws a file io error
     * CFE_ES_ReloadApp calls OS_stat with the null filename
     * OS_stat should return OS_INVALID_POINTER, but the exact
     * error is ignored in CFE_ES_ReloadApp and file io error is returned
     * most other functions return a CFE_ES_BAD_ARGUMENT in this situation
     */
    UtAssert_UINT32_EQ(CFE_ES_ReloadApp(TestAppId, NULL), CFE_ES_FILE_IO_ERR);
    UtAssert_UINT32_EQ(CFE_ES_ReloadApp(TestAppId, "/cf/NOT_cfe_testcase.so"), CFE_ES_FILE_IO_ERR);
    UtAssert_UINT32_EQ(CFE_ES_ReloadApp(CFE_ES_APPID_UNDEFINED, "/cf/cfe_testcase.so"),
                       CFE_ES_ERR_RESOURCEID_NOT_VALID);
    UtAssert_UINT32_EQ(CFE_ES_DeleteApp(CFE_ES_APPID_UNDEFINED), CFE_ES_ERR_RESOURCEID_NOT_VALID);
}

void ESApplicationControlTestSetup(void)
{
    UtTest_Add(TestApplicationControl, NULL, NULL, "Test Application Control API");
}
