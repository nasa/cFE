/******************************************************************************
 *  Time field access functions - cFS default 32 bit seconds, 16 bit subseconds
 *  in big endian format
 */
#include "cfe_msg.h"
#include "cfe_msg_priv.h"
#include "cfe_error.h"
#include <string.h>

/*----------------------------------------------------------------
 *
 * Function: CFE_MSG_SetMsgTime
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_SetMsgTime(CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t NewTime)
{

    CFE_Status_t               status;
    CFE_MSG_Type_t             type;
    bool                       hassechdr = false;
    CFE_MSG_TelemetryHeader_t *tlm       = (CFE_MSG_TelemetryHeader_t *)MsgPtr;

    if (MsgPtr == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Tlm || !hassechdr)
    {
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    /* Set big endian time field with default 32/16 layout */
    tlm->Sec.Time[0] = (NewTime.Seconds >> 24) & 0xFF;
    tlm->Sec.Time[1] = (NewTime.Seconds >> 16) & 0xFF;
    tlm->Sec.Time[2] = (NewTime.Seconds >> 8) & 0xFF;
    tlm->Sec.Time[3] = NewTime.Seconds & 0xFF;
    tlm->Sec.Time[4] = (NewTime.Subseconds >> 24) & 0xFF;
    tlm->Sec.Time[5] = (NewTime.Subseconds >> 16) & 0xFF;

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Function: CFE_MSG_GetMsgTime
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_MSG_GetMsgTime(const CFE_MSG_Message_t *MsgPtr, CFE_TIME_SysTime_t *Time)
{

    CFE_Status_t               status;
    CFE_MSG_Type_t             type;
    bool                       hassechdr = false;
    CFE_MSG_TelemetryHeader_t *tlm       = (CFE_MSG_TelemetryHeader_t *)MsgPtr;

    if (MsgPtr == NULL || Time == NULL)
    {
        return CFE_MSG_BAD_ARGUMENT;
    }

    /* Ignore return, pointer already checked */
    CFE_MSG_GetHasSecondaryHeader(MsgPtr, &hassechdr);

    status = CFE_MSG_GetType(MsgPtr, &type);
    if (status != CFE_SUCCESS || type != CFE_MSG_Type_Tlm || !hassechdr)
    {
        memset(Time, 0, sizeof(*Time));
        return CFE_MSG_WRONG_MSG_TYPE;
    }

    /* Get big endian time fields with default 32/16 layout */
    Time->Subseconds = (tlm->Sec.Time[4] << 24) + (tlm->Sec.Time[5] << 16);
    Time->Seconds    = (tlm->Sec.Time[0] << 24) + (tlm->Sec.Time[1] << 16) + (tlm->Sec.Time[2] << 8) + tlm->Sec.Time[3];

    return CFE_SUCCESS;
}
