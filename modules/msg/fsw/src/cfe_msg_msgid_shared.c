/******************************************************************************
 *  Message id access functions, shared cFS implementation
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"

/*----------------------------------------------------------------
 *
 * Function: CFE_MSG_GetTypeFromMsgId
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetTypeFromMsgId(CFE_SB_MsgId_t MsgId, CFE_MSG_Type_t *Type)
{
    CFE_MSG_Message_t msg;
    int32             Status;

    /* Memset to initialize avoids possible GCC bug 53119 */
    memset(&msg, 0, sizeof(msg));

    if (Type == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    Status = CFE_MSG_SetMsgId(&msg, MsgId);
    if (Status == CFE_SUCCESS)
    {
        Status = CFE_MSG_GetType(&msg, Type);
    }

    return Status;
}
