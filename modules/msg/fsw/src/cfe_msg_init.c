/******************************************************************************
 * Message initialization
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_msg_defaults.h"
#include "string.h"

/*----------------------------------------------------------------
 *
 * Function: CFE_MSG_Init
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_Init(CFE_MSG_Message_t *MsgPtr, CFE_SB_MsgId_t MsgId, CFE_MSG_Size_t Size)
{

    int32 status;

    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Clear and set defaults */
    memset(MsgPtr, 0, Size);
    CFE_MSG_InitDefaultHdr(MsgPtr);

    /* Set values input */
    status = CFE_MSG_SetMsgId(MsgPtr, MsgId);
    if (status == CFE_SUCCESS)
    {
        status = CFE_MSG_SetSize(MsgPtr, Size);
    }

    return status;
}
