/*
 * Message header unit tests
 */

/*
 * Includes
 */
#include "utassert.h"
#include "ut_support.h"
#include "test_cfe_msg_init.h"
#include "test_cfe_msg_ccsdspri.h"
#include "test_cfe_msg_ccsdsext.h"
#include "test_cfe_msg_msgid_shared.h"
#include "test_cfe_msg_msgid.h"
#include "test_cfe_msg_fc.h"
#include "test_cfe_msg_checksum.h"
#include "test_cfe_msg_time.h"

/*
 * Functions
 */
void UtTest_Setup(void)
{
    UT_Init("msg");
    UtPrintf("Message header coverage test...");

    UT_ADD_TEST(Test_MSG_Init);
    Test_MSG_CCSDSPri();
    Test_MSG_CCSDSExt();
    Test_MSG_MsgId_Shared();
    UT_ADD_TEST(Test_MSG_MsgId);
    UT_ADD_TEST(Test_MSG_Checksum);
    UT_ADD_TEST(Test_MSG_FcnCode);
    UT_ADD_TEST(Test_MSG_Time);
}
