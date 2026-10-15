/* Include Files */
#include "cfe_ta_module_all.h" /* All TA internal definitions and API */

#include <string.h>

/*
** Local function prototypes.
*/
void CFE_TA_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr, CFE_SB_MsgId_t MsgId);
bool CFE_TA_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength);

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * This function validates the length of a command structure, and
 * generates an error event if is not the expected length.
 *
 *-----------------------------------------------------------------*/
bool CFE_TA_VerifyCmdLength(const CFE_MSG_Message_t *MsgPtr, size_t ExpectedLength)
{
    bool              result       = true;
    CFE_MSG_Size_t    ActualLength = 0;
    CFE_MSG_FcnCode_t FcnCode      = 0;
    CFE_SB_MsgId_t    MsgId        = CFE_SB_INVALID_MSG_ID;

    CFE_MSG_GetSize(MsgPtr, &ActualLength);

    /* Verify the command packet length */
    if (ExpectedLength != ActualLength)
    {
        CFE_MSG_GetMsgId(MsgPtr, &MsgId);
        CFE_MSG_GetFcnCode(MsgPtr, &FcnCode);

        CFE_EVS_SendEvent(CFE_TA_LEN_ERR_EID,
                          CFE_EVS_EventType_ERROR,
                          "Invalid msg length: ID = 0x%X,  CC = %u, Len = %u, Expected = %u",
                          (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                          (unsigned int)FcnCode,
                          (unsigned int)ActualLength,
                          (unsigned int)ExpectedLength);

        result = false;
        CFE_TA_Global.CommandErrorCounter++; /* Routing Error */
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Internal helper routine only, not part of API.
 *
 * This function processes a command, verifying that it is valid and of
 * proper length.
 *
 *-----------------------------------------------------------------*/
void CFE_TA_ProcessGroundCommand(const CFE_SB_Buffer_t *SBBufPtr, CFE_SB_MsgId_t MsgId)
{
    CFE_MSG_FcnCode_t FcnCode = 0;

    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &FcnCode);

    /* Process "known" TA task ground commands */
    switch (FcnCode)
    {
        case CFE_TA_NOOP_CC:
            if (CFE_TA_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TA_NoopCmd_t)))
            {
                CFE_TA_NoopCmd((const CFE_TA_NoopCmd_t *)SBBufPtr);
            }
            break;

        case CFE_TA_RESET_COUNTERS_CC:
            if (CFE_TA_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TA_ResetCountersCmd_t)))
            {
                CFE_TA_ResetCountersCmd((const CFE_TA_ResetCountersCmd_t *)SBBufPtr);
            }
            break;

        case CFE_TA_QUERY_ALL_TASKS_CC:
            if (CFE_TA_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TA_QueryAllTasksCmd_t)))
            {
                CFE_TA_QueryAllTasksCmd((const CFE_TA_QueryAllTasksCmd_t *)SBBufPtr);
            }
            break;

        case CFE_TA_SET_TASK_AFFINITY_CC:
            if (CFE_TA_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TA_SetTaskAffinityCmd_t)))
            {
                CFE_TA_SetTaskAffinityCmd((const CFE_TA_SetTaskAffinityCmd_t *)SBBufPtr);
            }
            break;

        case CFE_TA_GET_TASK_AFFINITY_CC:
            if (CFE_TA_VerifyCmdLength(&SBBufPtr->Msg, sizeof(CFE_TA_GetTaskAffinityCmd_t)))
            {
                CFE_TA_GetTaskAffinityCmd((const CFE_TA_GetTaskAffinityCmd_t *)SBBufPtr);
            }
            break;

        default:
            /* Unknown command code */
            CFE_EVS_SendEvent(CFE_TA_ERR_CC_EID,
                              CFE_EVS_EventType_ERROR,
                              "Invalid command code -- ID = 0x%08x, CC = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MsgId),
                              (unsigned int)FcnCode);

            CFE_TA_Global.CommandErrorCounter++; /* Routing Error */
            break;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TA_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    /* Static cache for dynamic Message IDs */
    static CFE_SB_MsgId_t CMD_MID     = CFE_SB_MSGID_RESERVED;
    static CFE_SB_MsgId_t SEND_HK_MID = CFE_SB_MSGID_RESERVED;

    CFE_SB_MsgId_t MessageID = CFE_SB_INVALID_MSG_ID;

    /* cache the local MID Values here, this avoids repeat lookups */
    if (!CFE_SB_IsValidMsgId(CMD_MID))
    {
        CMD_MID     = CFE_SB_ValueToMsgId(CFE_TA_CMD_MID);
        SEND_HK_MID = CFE_SB_ValueToMsgId(CFE_TA_SEND_HK_MID);
    }

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MessageID);

    /* Process all SB messages using the If/Else construct per reviewer request! */
    if (CFE_SB_MsgId_Equal(MessageID, SEND_HK_MID))
    {
        /* Housekeeping request */
        CFE_TA_ReportHousekeepingCmd((const CFE_TA_SendHkCmd_t *)SBBufPtr);
    }
    else if (CFE_SB_MsgId_Equal(MessageID, CMD_MID))
    {
        /* Normal ground command */
        CFE_TA_ProcessGroundCommand(SBBufPtr, MessageID);
    }
    else
    {
        /* Unknown command -- should never occur */
        CFE_EVS_SendEvent(CFE_TA_ERR_MSGID_EID,
                          CFE_EVS_EventType_ERROR,
                          "Invalid command packet, Message ID = 0x%08X",
                          (unsigned int)CFE_SB_MsgIdToValue(MessageID));

        CFE_TA_Global.CommandErrorCounter++; /* Routing Error */
    }
}
