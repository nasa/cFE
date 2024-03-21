/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/*
** File: cfe_tbl_task.c
**
** Subsystem: cFE TBL Task
**
** Author: David Kobe (the Hammers Company, Inc.)
**
** Notes:
**
*/

/*
** Required header files
*/
#include "cfe_tbl_module_all.h"

#include <string.h>

/**
** Data structure of a single record in #CFE_TBL_CmdHandlerTbl
*/
typedef struct
{
    CFE_SB_MsgId_t           MsgId;          /**< \brief Acceptable Message ID */
    CFE_MSG_FcnCode_t        CmdCode;        /**< \brief Acceptable Command Code (if necessary) */
    size_t                   ExpectedLength; /**< \brief Expected Message Length (in bytes) including message header */
    CFE_TBL_MsgProcFuncPtr_t MsgProcFuncPtr; /**< \brief Pointer to function to handle message  */
    CFE_TBL_MsgType_t        MsgTypes;       /**< \brief Message Type (i.e. - with/without Cmd Code)   */
} CFE_TBL_CmdHandlerTblRec_t;

/*
 * Macros to assist in building the CFE_TBL_CmdHandlerTbl -
 *  For command handler entries, which have a command code, payload type, and a handler function
 */
#define CFE_TBL_ENTRY(mid, ccode, paramtype, handlerfunc, msgtype)                                             \
    {                                                                                                          \
        CFE_SB_MSGID_WRAP_VALUE(mid), ccode, sizeof(paramtype), (CFE_TBL_MsgProcFuncPtr_t)handlerfunc, msgtype \
    }

/* Constant Data */

const CFE_TBL_CmdHandlerTblRec_t CFE_TBL_CmdHandlerTbl[] = {
    /* SEND_HK Entry */
    CFE_TBL_ENTRY(CFE_TBL_SEND_HK_MID, 0, CFE_TBL_SendHkCmd_t, CFE_TBL_SendHkCmd, CFE_TBL_MSG_MSGTYPE),

    /* Everything else */
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_NOOP_CC, CFE_TBL_NoopCmd_t, CFE_TBL_NoopCmd, CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_RESET_COUNTERS_CC, CFE_TBL_ResetCountersCmd_t, CFE_TBL_ResetCountersCmd,
                  CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_LOAD_CC, CFE_TBL_LoadCmd_t, CFE_TBL_LoadCmd, CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_DUMP_CC, CFE_TBL_DumpCmd_t, CFE_TBL_DumpCmd, CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_VALIDATE_CC, CFE_TBL_ValidateCmd_t, CFE_TBL_ValidateCmd,
                  CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_ACTIVATE_CC, CFE_TBL_ActivateCmd_t, CFE_TBL_ActivateCmd,
                  CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_DUMP_REGISTRY_CC, CFE_TBL_DumpRegistryCmd_t, CFE_TBL_DumpRegistryCmd,
                  CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_SEND_REGISTRY_CC, CFE_TBL_SendRegistryCmd_t, CFE_TBL_SendRegistryCmd,
                  CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_DELETE_CDS_CC, CFE_TBL_DeleteCDSCmd_t, CFE_TBL_DeleteCDSCmd,
                  CFE_TBL_CMD_MSGTYPE),
    CFE_TBL_ENTRY(CFE_TBL_CMD_MID, CFE_TBL_ABORT_LOAD_CC, CFE_TBL_AbortLoadCmd_t, CFE_TBL_AbortLoadCmd,
                  CFE_TBL_CMD_MSGTYPE),

    /* list terminator (keep last) */
    {CFE_SB_MSGID_RESERVED, 0, 0, NULL, CFE_TBL_TERM_MSGTYPE}};

/******************************************************************************/

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr)
{
    CFE_SB_MsgId_t       MessageID   = CFE_SB_INVALID_MSG_ID;
    CFE_MSG_FcnCode_t    CommandCode = 0;
    int16                CmdIndx;
    CFE_MSG_Size_t       ActualLength = 0;
    CFE_TBL_CmdProcRet_t CmdStatus    = CFE_TBL_INC_ERR_CTR; /* Assume a failed command */

    CFE_MSG_GetMsgId(&SBBufPtr->Msg, &MessageID);
    CFE_MSG_GetFcnCode(&SBBufPtr->Msg, &CommandCode);

    /* Search the Command Handler Table for a matching message */
    CmdIndx = CFE_TBL_SearchCmdHndlrTbl(MessageID, CommandCode);

    /* Check to see if a matching command was found */
    if (CmdIndx >= 0)
    {
        /* Verify Message Length before processing */
        CFE_MSG_GetSize(&SBBufPtr->Msg, &ActualLength);
        if (ActualLength == CFE_TBL_CmdHandlerTbl[CmdIndx].ExpectedLength)
        {
            /* All checks have passed, call the appropriate message handler */
            CmdStatus = (CFE_TBL_CmdHandlerTbl[CmdIndx].MsgProcFuncPtr)(SBBufPtr);
        }
        else /* Bad Message Length */
        {
            CFE_EVS_SendEvent(CFE_TBL_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid msg length -- ID = 0x%X, CC = %u, Len = %u, Expected = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MessageID), (unsigned int)CommandCode,
                              (unsigned int)ActualLength, (unsigned int)CFE_TBL_CmdHandlerTbl[CmdIndx].ExpectedLength);
        }

        /* Only update command counters when message has a command code */
        if (CFE_TBL_CmdHandlerTbl[CmdIndx].MsgTypes == CFE_TBL_CMD_MSGTYPE)
        {
            if (CmdStatus == CFE_TBL_INC_CMD_CTR)
            {
                CFE_TBL_Global.CommandCounter++;
            }
            else if (CmdStatus == CFE_TBL_INC_ERR_CTR)
            {
                CFE_TBL_Global.CommandErrorCounter++;
            }
        }
    }
    else
    {
        /* Determine whether event message should be */
        /* "Bad Command Code" or "Bad Message ID"    */
        if (CmdIndx == CFE_TBL_BAD_CMD_CODE)
        {
            CFE_EVS_SendEvent(CFE_TBL_CC1_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Invalid command code -- ID = 0x%X, CC = %u",
                              (unsigned int)CFE_SB_MsgIdToValue(MessageID), (unsigned int)CommandCode);

            /* Update the command error counter */
            CFE_TBL_Global.CommandErrorCounter++;
        }
        else /* CmdIndx == CFE_TBL_BAD_MSG_ID */
        {
            CFE_EVS_SendEvent(CFE_TBL_MID_ERR_EID, CFE_EVS_EventType_ERROR, "Invalid message ID -- ID = 0x%X",
                              (unsigned int)CFE_SB_MsgIdToValue(MessageID));
            /*
            ** Note: we only increment the command error counter when
            **    processing messages with command codes
            */
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int16 CFE_TBL_SearchCmdHndlrTbl(CFE_SB_MsgId_t MessageID, uint16 CommandCode)
{
    int16 TblIndx    = CFE_TBL_BAD_CMD_CODE;
    bool  FoundMsg   = false;
    bool  FoundMatch = false;

    do
    {
        /* Point to next entry in Command Handler Table */
        TblIndx++;

        /* Check to see if we found a matching Message ID */
        if (CFE_SB_MsgId_Equal(CFE_TBL_CmdHandlerTbl[TblIndx].MsgId, MessageID) &&
            (CFE_TBL_CmdHandlerTbl[TblIndx].MsgTypes != CFE_TBL_TERM_MSGTYPE))
        {
            /* Flag any found message IDs so that if there is an error,        */
            /* we can determine if it was a bad message ID or bad command code */
            FoundMsg = true;

            /* If entry in the Command Handler Table is a command entry, */
            /* then check for a matching command code                    */
            if (CFE_TBL_CmdHandlerTbl[TblIndx].MsgTypes == CFE_TBL_CMD_MSGTYPE)
            {
                if (CFE_TBL_CmdHandlerTbl[TblIndx].CmdCode == CommandCode)
                {
                    /* Found matching message ID and Command Code */
                    FoundMatch = true;
                }
            }
            else /* Message is not a command message with specific command code */
            {
                /* Automatically assume a match when legit */
                /* Message ID is all that is required      */
                FoundMatch = true;
            }
        }
    } while ((!FoundMatch) && (CFE_TBL_CmdHandlerTbl[TblIndx].MsgTypes != CFE_TBL_TERM_MSGTYPE));

    /* If we failed to find a match, return a negative index */
    if (!FoundMatch)
    {
        /* Determine if the message ID was bad or the command code */
        if (FoundMsg)
        {
            /* A matching message ID was found, so the command code must be bad */
            TblIndx = CFE_TBL_BAD_CMD_CODE;
        }
        else /* No matching message ID was found */
        {
            TblIndx = CFE_TBL_BAD_MSG_ID;
        }
    }

    return TblIndx;
}
