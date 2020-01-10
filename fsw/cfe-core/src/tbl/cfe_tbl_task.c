/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

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
#include "private/cfe_private.h"
#include "cfe_version.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_events.h"
#include "cfe_tbl_msg.h"
#include "cfe_tbl_task_cmds.h"
#include "cfe_tbl_verify.h"
#include "cfe_msgids.h"
#include <string.h>


/*
** Table task global data
*/
CFE_TBL_TaskData_t    CFE_TBL_TaskData;

/*
 * Macros to assist in building the CFE_TBL_CmdHandlerTbl -
 *  For generic message entries, which only have a MID and a handler function (no command payload)
 */
#define CFE_TBL_MESSAGE_ENTRY(mid,handlerfunc) \
        { mid, 0, sizeof(CCSDS_CommandPacket_t), (CFE_TBL_MsgProcFuncPtr_t)handlerfunc, CFE_TBL_MSG_MSGTYPE }

/*
 * Macros to assist in building the CFE_TBL_CmdHandlerTbl -
 *  For command handler entries, which have a command code, payload type, and a handler function
 */
#define CFE_TBL_COMMAND_ENTRY(ccode,paramtype,handlerfunc) \
        { CFE_TBL_CMD_MID, ccode, sizeof(paramtype), (CFE_TBL_MsgProcFuncPtr_t)handlerfunc, CFE_TBL_CMD_MSGTYPE }

/* Constant Data */

const CFE_TBL_CmdHandlerTblRec_t CFE_TBL_CmdHandlerTbl[] =
{
        /* message entries (SEND_HK) */
        CFE_TBL_MESSAGE_ENTRY(CFE_TBL_SEND_HK_MID,  CFE_TBL_HousekeepingCmd),

        /* command entries (everything else) */
        CFE_TBL_COMMAND_ENTRY(          CFE_TBL_NOOP_CC,         CFE_TBL_Noop_t,         CFE_TBL_NoopCmd),
        CFE_TBL_COMMAND_ENTRY(CFE_TBL_RESET_COUNTERS_CC,CFE_TBL_ResetCounters_t,CFE_TBL_ResetCountersCmd),
        CFE_TBL_COMMAND_ENTRY(          CFE_TBL_LOAD_CC,         CFE_TBL_Load_t,         CFE_TBL_LoadCmd),
        CFE_TBL_COMMAND_ENTRY(          CFE_TBL_DUMP_CC,         CFE_TBL_Dump_t,         CFE_TBL_DumpCmd),
        CFE_TBL_COMMAND_ENTRY(      CFE_TBL_VALIDATE_CC,     CFE_TBL_Validate_t,     CFE_TBL_ValidateCmd),
        CFE_TBL_COMMAND_ENTRY(      CFE_TBL_ACTIVATE_CC,     CFE_TBL_Activate_t,     CFE_TBL_ActivateCmd),
        CFE_TBL_COMMAND_ENTRY( CFE_TBL_DUMP_REGISTRY_CC, CFE_TBL_DumpRegistry_t, CFE_TBL_DumpRegistryCmd),
        CFE_TBL_COMMAND_ENTRY( CFE_TBL_SEND_REGISTRY_CC, CFE_TBL_SendRegistry_t, CFE_TBL_SendRegistryCmd),
        CFE_TBL_COMMAND_ENTRY(    CFE_TBL_DELETE_CDS_CC,    CFE_TBL_DeleteCDS_t,    CFE_TBL_DeleteCDSCmd),
        CFE_TBL_COMMAND_ENTRY(    CFE_TBL_ABORT_LOAD_CC,    CFE_TBL_AbortLoad_t,    CFE_TBL_AbortLoadCmd),

        /* list terminator (keep last) */
        {  0,   0,   0,  NULL, CFE_TBL_TERM_MSGTYPE }
};


/******************************************************************************/

void CFE_TBL_TaskMain(void)
{
    int32  Status;

    CFE_ES_PerfLogEntry(CFE_MISSION_TBL_MAIN_PERF_ID);

    Status = CFE_TBL_TaskInit();
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      CFE_ES_PerfLogExit(CFE_MISSION_TBL_MAIN_PERF_ID);      
      /* Note: CFE_ES_ExitApp will not return */
      CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_INIT_ERROR);
    }/* end if */

    /*
     * Wait for other apps to start.
     * It is important that the core apps are present before this starts receiving
     * messages from the command pipe, as some of those handlers might depend on
     * the other core apps.
     */
    CFE_ES_WaitForSystemState(CFE_ES_SystemState_CORE_READY, CFE_PLATFORM_CORE_MAX_STARTUP_MSEC);

    /* Main loop */
    while (Status == CFE_SUCCESS)
    {
        /* Increment the Main task Execution Counter */
        CFE_ES_IncrementTaskCounter();

        CFE_ES_PerfLogExit(CFE_MISSION_TBL_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_RcvMsg( &CFE_TBL_TaskData.MsgPtr,
                                CFE_TBL_TaskData.CmdPipe,
                                CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_MISSION_TBL_MAIN_PERF_ID);

        if (Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_TBL_TaskPipe(CFE_TBL_TaskData.MsgPtr);
        }else{
            CFE_ES_WriteToSysLog("TBL:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
        }/* end if */

    }/* end while */

    /* while loop exits only if CFE_SB_RcvMsg returns error */
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);

} /* end CFE_TBL_TaskMain() */


/******************************************************************************/

int32 CFE_TBL_TaskInit(void)
{
    int32 Status = CFE_SUCCESS;

    /*
    ** Register Table Services with ES
    */
    Status = CFE_ES_RegisterApp();

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Call to CFE_ES_RegisterApp Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    /*
    ** Initialize global Table Services data
    */
    CFE_TBL_InitData();

    /*
    ** Register event filter table
    */
    Status = CFE_EVS_Register(NULL, 0, 0);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    /*
    ** Create Software Bus message pipe
    */
    Status = CFE_SB_CreatePipe(&CFE_TBL_TaskData.CmdPipe,
                                CFE_TBL_TaskData.PipeDepth,
                                CFE_TBL_TaskData.PipeName);
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error creating cmd pipe:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */                                                                

    /*
    ** Subscribe to Housekeeping request commands
    */
    Status = CFE_SB_Subscribe(CFE_TBL_SEND_HK_MID, CFE_TBL_TaskData.CmdPipe);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error subscribing to HK Request:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    /*
    ** Subscribe to Table task ground command packets
    */
    Status = CFE_SB_Subscribe(CFE_TBL_CMD_MID, CFE_TBL_TaskData.CmdPipe);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error subscribing to gnd cmds:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    /*
    ** Task startup event message
    */
    Status = CFE_EVS_SendEvent(CFE_TBL_INIT_INF_EID, CFE_EVS_EventType_INFORMATION, "cFE TBL Initialized.  cFE Version %d.%d.%d.%d",
                               CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);

    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("TBL:Error sending init event:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    return CFE_SUCCESS;

} /* End of CFE_TBL_TaskInit() */


/******************************************************************************/

void CFE_TBL_InitData(void)
{
    /* Initialize Counters */
    CFE_TBL_TaskData.CommandCounter = 0;
    CFE_TBL_TaskData.CommandErrorCounter = 0;
    CFE_TBL_TaskData.SuccessValCounter = 0;
    CFE_TBL_TaskData.FailedValCounter = 0;

    /* Get the assigned Application ID for the Table Services Task */
    CFE_ES_GetAppID(&CFE_TBL_TaskData.TableTaskAppId);

    /* Initialize Command Pipe Parameters */
    CFE_TBL_TaskData.PipeDepth = CFE_TBL_TASK_PIPE_DEPTH;
    strncpy(CFE_TBL_TaskData.PipeName, CFE_TBL_TASK_PIPE_NAME, 16);

    /* Initialize Packet Headers */
    CFE_SB_InitMsg(&CFE_TBL_TaskData.HkPacket,
                   CFE_TBL_HK_TLM_MID,
                   sizeof(CFE_TBL_TaskData.HkPacket),
                   true);

    CFE_SB_InitMsg(&CFE_TBL_TaskData.TblRegPacket,
                   CFE_TBL_REG_TLM_MID,
                   sizeof(CFE_TBL_TaskData.TblRegPacket),
                   true);

} /* End of CFE_TBL_InitData() */


/******************************************************************************/

void CFE_TBL_TaskPipe(CFE_SB_Msg_t *MessagePtr)
{
    CFE_SB_MsgId_t       MessageID = CFE_SB_GetMsgId(MessagePtr);
    uint16               CommandCode = CFE_SB_GetCmdCode(MessagePtr);
    int16                CmdIndx;
    uint32               ActualLength;
    CFE_TBL_CmdProcRet_t CmdStatus = CFE_TBL_INC_ERR_CTR; /* Assume a failed command */

    /* Search the Command Handler Table for a matching message */
    CmdIndx = CFE_TBL_SearchCmdHndlrTbl(MessageID, CommandCode);

    /* Check to see if a matching command was found */
    if (CmdIndx >= 0)
    {
        /* Verify Message Length before processing */
        ActualLength = CFE_SB_GetTotalMsgLength(MessagePtr);
        if (ActualLength == CFE_TBL_CmdHandlerTbl[CmdIndx].ExpectedLength)
        {
            /* All checks have passed, call the appropriate message handler */
            CmdStatus = (CFE_TBL_CmdHandlerTbl[CmdIndx].MsgProcFuncPtr)(MessagePtr);
        }
        else /* Bad Message Length */
        {
            CFE_EVS_SendEvent( CFE_TBL_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                               "Invalid msg length -- ID = 0x%04X, CC = %d, Len = %d (!= %d)",
                               (unsigned int)MessageID, (int)CommandCode, (int)ActualLength,
                               (int)CFE_TBL_CmdHandlerTbl[CmdIndx].ExpectedLength );
        }

        /* Only update command counters when message has a command code */
        if (CFE_TBL_CmdHandlerTbl[CmdIndx].MsgTypes == CFE_TBL_CMD_MSGTYPE)
        {
            if (CmdStatus == CFE_TBL_INC_CMD_CTR)
            {
                CFE_TBL_TaskData.CommandCounter++;
            }
            else if (CmdStatus == CFE_TBL_INC_ERR_CTR)
            {
                CFE_TBL_TaskData.CommandErrorCounter++;
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
                              "Invalid command code -- ID = 0x%04X, CC = %d",
                              (unsigned int)MessageID, (int)CommandCode);

            /* Update the command error counter */
            CFE_TBL_TaskData.CommandErrorCounter++;
        }
        else  /* CmdIndx == CFE_TBL_BAD_MSG_ID */
        {
            CFE_EVS_SendEvent(CFE_TBL_MID_ERR_EID, CFE_EVS_EventType_ERROR,
                             "Invalid message ID -- ID = 0x%04X",
                              (unsigned int)MessageID);
            /*
            ** Note: we only increment the command error counter when
            **    processing messages with command codes
            */
        }
    }

    return;

} /* End of CFE_TBL_TaskPipe() */


/******************************************************************************/

int16 CFE_TBL_SearchCmdHndlrTbl( CFE_SB_MsgId_t MessageID, uint16 CommandCode )
{
    int16     TblIndx = CFE_TBL_BAD_CMD_CODE;
    bool      FoundMsg = false;
    bool      FoundMatch = false;

    do
    {
        /* Point to next entry in Command Handler Table */
        TblIndx++;

        /* Check to see if we found a matching Message ID */
        if ((CFE_TBL_CmdHandlerTbl[TblIndx].MsgId == MessageID) &&
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
}   /* End of CFE_TBL_SearchCmdHndlrTbl() */


/************************/
/*  End of File Comment */
/************************/

