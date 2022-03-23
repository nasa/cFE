/*
 * Message header field not Zero or F
 */

/*
 * Includes
 */
#include "cfe_msg.h"
#include "test_msg_not.h"

unsigned int Test_MSG_NotZero(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int bits = 0;

    /* Primary */
    bits |= Test_MSG_Pri_NotZero(MsgPtr);

    /* Extended */
    bits |= Test_MSG_Ext_NotZero(MsgPtr);

    return bits;
}

unsigned int Test_MSG_NotF(const CFE_MSG_Message_t *MsgPtr)
{
    unsigned int bits = 0;

    /* Primary */
    bits |= Test_MSG_Pri_NotF(MsgPtr);

    /* Extended */
    bits |= Test_MSG_Ext_NotF(MsgPtr);

    return bits;
}
