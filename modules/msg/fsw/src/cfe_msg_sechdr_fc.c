/******************************************************************************
 *  Function code field access functions
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"

#define CFE_MSG_FC_MASK 0x7F /**< \brief Function code mask */

/*----------------------------------------------------------------
 *
 * Function: CFE_MSG_GetFcnCode
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetFcnCode(const CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t *FcnCode)
{
    CFE_Status_t             status;
    CFE_MSG_Type_t           type;
    bool                     hassechdr = false;
    CFE_MSG_CommandHeader_t *cmd       = (CFE_MSG_CommandHeader_t *)MsgPtr;

    if (MsgPtr == NULL || FcnCode == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Cmd || !hassechdr)
    {
        *FcnCode = 0;
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    *FcnCode = cmd->Sec.FunctionCode & CFE_MSG_FC_MASK;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: CFE_MSG_SetFcnCode
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetFcnCode(CFE_MSG_Message_t *MsgPtr, CFE_MSG_FcnCode_t FcnCode)
{
    CFE_Status_t             status;
    CFE_MSG_Type_t           type;
    bool                     hassechdr = false;
    CFE_MSG_CommandHeader_t *cmd       = (CFE_MSG_CommandHeader_t *)MsgPtr;

    if (MsgPtr == NULL || (FcnCode > CFE_MSG_FC_MASK))
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Cmd || !hassechdr)
    {
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    cmd->Sec.FunctionCode = FcnCode;

    return CFE_SUCCESS;
}
