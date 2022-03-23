/*
 * Extended header stubbed out, use when no extended header is defined
 */

/*
 * Includes
 */
#include "cfe_msg.h"
#include "test_msg_not.h"
#include "test_cfe_msg_ccsdsext.h"

unsigned int Test_MSG_Ext_NotZero(const CFE_MSG_Message_t *MsgPtr)
{
    return 0;
}

unsigned int Test_MSG_Ext_NotF(const CFE_MSG_Message_t *MsgPtr)
{
    return 0;
}

void Test_MSG_CCSDSExt(void) {}
