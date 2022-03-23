/**
 * \file
 *   Initialization routine for CFE functional test
 *   Demonstration of how to register and use the UT assert functions.
 */

/*
 * Includes
 */

#include "cfe_assert.h"
#include "cfe_test.h"

CFE_FT_Global_t CFE_FT_Global;

/*
 * Test main function
 * Register this test routine with CFE Assert
 */
void CFE_TestMain(void)
{
    /* Static local so data section is not zero when checking app info */
    static char TestName[] = "CFE API";

    /* Constant Table information used by all table tests */
    CFE_FT_Global.TblName           = "TestTable";
    CFE_FT_Global.RegisteredTblName = "CFE_TEST_APP.TestTable";
    CFE_FT_Global.TblFilename       = "test_tbl.tbl";

    /*
     * Register this test app with CFE assert
     *
     * Note this also waits for the appropriate overall system
     * state and gets ownership of the UtAssert subsystem
     */
    CFE_Assert_RegisterTest(TestName);
    CFE_Assert_OpenLogFile(CFE_ASSERT_LOG_FILE_NAME);

    /*
     * Register test cases in UtAssert
     */
    ESApplicationControlTestSetup();
    ESBehaviorestSetup();
    ESCDSTestSetup();
    ESCounterTestSetup();
    ESInfoTestSetup();
    ESMemPoolTestSetup();
    ESMiscTestSetup();
    ESPerfTestSetup();
    ESResourceIDTestSetup();
    ESTaskTestSetup();
    EVSFiltersTestSetup();
    EVSSendTestSetup();
    FSHeaderTestSetup();
    FSUtilTestSetup();
    MessageIdTestSetup();
    MsgApiTestSetup();
    ResourceIdMiscTestSetup();
    SBPipeMangSetup();
    SBSendRecvTestSetup();
    SBSubscriptionTestSetup();
    TBLContentAccessTestSetup();
    TBLContentMangTestSetup();
    TBLInformationTestSetup();
    TBLRegistrationTestSetup();
    TimeArithmeticTestSetup();
    TimeConversionTestSetup();
    TimeCurrentTestSetup();
    TimeExternalTestSetup();
    TimeMiscTestSetup();

    /*
     * Execute the tests
     *
     * Note this also releases ownership of the UtAssert subsystem when complete
     */
    CFE_Assert_ExecuteTest();

    /* Nothing more for this app to do */
    CFE_ES_ExitApp(CFE_ES_RunStatus_APP_EXIT);
}
