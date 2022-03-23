/******************************************************************************
 * Message default header initialization - implementation without CCSDS
 * extended header
 */
#include "cfe_msg_hdr.h"
#include "cfe_msg_defaults.h"

/*----------------------------------------------------------------
 *
 * Function: CFE_MSG_InitDefaultHdr
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_MSG_InitDefaultHdr(CFE_MSG_Message_t *MsgPtr)
{
    CFE_MSG_SetDefaultCCSDSPri(MsgPtr);
}
