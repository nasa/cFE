/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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

/******************************************************************************
** File: cfe_sb_task.c
**
** Purpose:
**      This file contains the source code for the SB task.
**
** Author:   R.McGraw/SSI
**
******************************************************************************/

/* Include Files */

#include "cfe_sb.h"
#include "cfe_sb_events.h"
#include "cfe_evs.h"
#include "cfe_sb_priv.h"
#include "osapi.h"
#include "cfe_version.h"
#include "cfe_msgids.h"
#include "cfe_error.h"
#include "cfe_es.h"
#include "cfe_psp.h"
#include "cfe_es_msg.h"
#include "cfe_sb_verify.h"
#include "cfe_sb_msg_id_util.h"
#include <string.h>

/*  Task Globals */
cfe_sb_t                CFE_SB;
CFE_SB_Qos_t            CFE_SB_Default_Qos;


/******************************************************************************
**  Function:  CFE_SB_TaskMain()
**
**  Purpose:
**    Main loop for Software Bus task, used to process SB commands.
**
**  Arguments:
**    none
**
**  Return:
**    none
*/
void CFE_SB_TaskMain(void)
{
    int32  Status;

    CFE_ES_PerfLogEntry(CFE_MISSION_SB_MAIN_PERF_ID);

    Status = CFE_SB_AppInit();
    
    if(Status != CFE_SUCCESS)
    {
      CFE_ES_WriteToSysLog("SB:Application Init Failed,RC=0x%08X\n", (unsigned int)Status);
      CFE_ES_PerfLogExit(CFE_MISSION_SB_MAIN_PERF_ID);
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

        CFE_ES_PerfLogExit(CFE_MISSION_SB_MAIN_PERF_ID);

        /* Pend on receipt of packet */
        Status = CFE_SB_RcvMsg(&CFE_SB.CmdPipePktPtr,
                                CFE_SB.CmdPipe,
                                CFE_SB_PEND_FOREVER);

        CFE_ES_PerfLogEntry(CFE_MISSION_SB_MAIN_PERF_ID);

        if(Status == CFE_SUCCESS)
        {
            /* Process cmd pipe msg */
            CFE_SB_ProcessCmdPipePkt();
        }else{
            CFE_ES_WriteToSysLog("SB:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
        }/* end if */

    }/* end while */

    /* while loop exits only if CFE_SB_RcvMsg returns error */
    CFE_ES_ExitApp(CFE_ES_RunStatus_CORE_APP_RUNTIME_ERROR);

}/* end CFE_SB_TaskMain */



/******************************************************************************
**  Function:  CFE_SB_AppInit()
**
**  Purpose:
**    Initialization routine for SB application. This routine is executed when
**    the SB application is started by Executive Services.
**
**  Arguments:
**    none
**
**  Return:
**    CFE_SUCCESS if no errors, otherwise this function returns the error code
**    that was received from the function that detected the error.
**    
*/
int32 CFE_SB_AppInit(void){

    uint32 CfgFileEventsToFilter = 0;    
    uint32 *TmpPtr = NULL;
    int32  Status = CFE_SUCCESS;
    
    Status = CFE_ES_RegisterApp();

    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Call to CFE_ES_RegisterApp Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    /* Get the assigned Application ID for the SB Task */
    CFE_ES_GetAppID(&CFE_SB.AppId);

    /* Process the platform cfg file events to be filtered */
    if(CFE_PLATFORM_SB_FILTERED_EVENT1 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT1;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK1;      
      CfgFileEventsToFilter++;
    }/* end if */           

    if(CFE_PLATFORM_SB_FILTERED_EVENT2 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT2;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK2;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_PLATFORM_SB_FILTERED_EVENT3 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT3;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK3;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_PLATFORM_SB_FILTERED_EVENT4 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT4;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK4;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_PLATFORM_SB_FILTERED_EVENT5 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT5;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK5;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_PLATFORM_SB_FILTERED_EVENT6 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT6;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK6;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_PLATFORM_SB_FILTERED_EVENT7 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT7;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK7;      
      CfgFileEventsToFilter++;
    }/* end if */      

    if(CFE_PLATFORM_SB_FILTERED_EVENT8 != 0){
      CFE_SB.EventFilters[CfgFileEventsToFilter].EventID = CFE_PLATFORM_SB_FILTERED_EVENT8;
      CFE_SB.EventFilters[CfgFileEventsToFilter].Mask    = CFE_PLATFORM_SB_FILTER_MASK8;      
      CfgFileEventsToFilter++;
    }/* end if */
    
    /* Be sure the number of events to register for filtering   
    ** does not exceed CFE_PLATFORM_EVS_MAX_EVENT_FILTERS */
    if(CFE_PLATFORM_EVS_MAX_EVENT_FILTERS < CfgFileEventsToFilter){
      CfgFileEventsToFilter = CFE_PLATFORM_EVS_MAX_EVENT_FILTERS;
    }/* end if */


    /* Register event filter table... */
    Status = CFE_EVS_Register(CFE_SB.EventFilters,
                              CfgFileEventsToFilter,
                              CFE_EVS_EventFilter_BINARY);
    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
    
    CFE_ES_WriteToSysLog("SB:Registered %d events for filtering\n",(int)CfgFileEventsToFilter);

    CFE_SB_InitMsg(&CFE_SB.HKTlmMsg,
                   CFE_SB_HK_TLM_MID,
                   sizeof(CFE_SB.HKTlmMsg),
                   true);

    CFE_SB_InitMsg(&CFE_SB.PrevSubMsg,
                   CFE_SB_ALLSUBS_TLM_MID,
                   sizeof(CFE_SB.PrevSubMsg),
                   true);

    CFE_SB_InitMsg(&CFE_SB.SubRprtMsg,
                   CFE_SB_ONESUB_TLM_MID,
                   sizeof(CFE_SB.SubRprtMsg),
                   true);    

    /* Populate the fixed fields in the HK Tlm Msg */
    CFE_SB_SET_MEMADDR(CFE_SB.HKTlmMsg.Payload.MemPoolHandle, CFE_SB.Mem.PoolHdl);
    
    /* Populate the fixed fields in the Stat Tlm Msg */
    CFE_SB.StatTlmMsg.Payload.MaxMsgIdsAllowed = CFE_PLATFORM_SB_MAX_MSG_IDS;
    CFE_SB.StatTlmMsg.Payload.MaxPipesAllowed  = CFE_PLATFORM_SB_MAX_PIPES;
    CFE_SB.StatTlmMsg.Payload.MaxMemAllowed    = CFE_PLATFORM_SB_BUF_MEMORY_BYTES;
    CFE_SB.StatTlmMsg.Payload.MaxPipeDepthAllowed = CFE_PLATFORM_SB_MAX_PIPE_DEPTH;
    CFE_SB.StatTlmMsg.Payload.MaxSubscriptionsAllowed =
                    ((CFE_PLATFORM_SB_MAX_MSG_IDS)*(CFE_PLATFORM_SB_MAX_DEST_PER_PKT));
    
    Status = CFE_SB_CreatePipe(&CFE_SB.CmdPipe,
                                CFE_SB_CMD_PIPE_DEPTH,
                                CFE_SB_CMD_PIPE_NAME);
    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Call to CFE_SB_CreatePipe Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */                                

    Status = CFE_SB_Subscribe(CFE_SB_CMD_MID,CFE_SB.CmdPipe);

    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Subscribe to Cmds Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
        
    Status = CFE_SB_Subscribe(CFE_SB_SEND_HK_MID,CFE_SB.CmdPipe);

    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Subscribe to HK Request Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
     
    /* Ensure a ground commanded reset does not get blocked if SB mem pool  */
    /* becomes fully configured (DCR6772) */
    Status = CFE_ES_GetPoolBuf((uint32 **)&TmpPtr, CFE_SB.Mem.PoolHdl,
                                        sizeof(CFE_ES_Restart_t));

    if(Status < 0){
      CFE_ES_WriteToSysLog("SB:Init error, GetPool Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */

    /* Return mem block used on previous call,the actual memory is not needed.*/
    /* The SB mem pool is now configured with a block size for the reset cmd. */
    Status = CFE_ES_PutPoolBuf(CFE_SB.Mem.PoolHdl, (uint32 *)TmpPtr);

    if(Status < 0){
      CFE_ES_WriteToSysLog("SB:Init error, PutPool Failed:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */    
    
    Status = CFE_EVS_SendEvent(CFE_SB_INIT_EID,
                              CFE_EVS_EventType_INFORMATION,
                              "cFE SB Initialized");
    if(Status != CFE_SUCCESS){
      CFE_ES_WriteToSysLog("SB:Error sending init event:RC=0x%08X\n",(unsigned int)Status);
      return Status;
    }/* end if */
                      
    return CFE_SUCCESS;                      

}/* end CFE_SB_AppInit */


/******************************************************************************
**  Function:  CFE_SB_VerifyCmdLength()
**
**  Purpose:
**    Function to verify the length of incoming SB command packets
**
**  Arguments:
**    Message pointer and expected length
**
**  Return:
**    true if length is acceptable
*/
bool CFE_SB_VerifyCmdLength(CFE_SB_MsgPtr_t Msg, uint16 ExpectedLength)
{
    bool result       = true;
    uint16  ActualLength = CFE_SB_GetTotalMsgLength(Msg);

    /*
    ** Verify the command packet length
    */
    if (ExpectedLength != ActualLength)
    {
        CFE_SB_MsgId_t MessageID = CFE_SB_GetMsgId(Msg);
        uint16 CommandCode = CFE_SB_GetCmdCode(Msg);

        CFE_EVS_SendEvent(CFE_SB_LEN_ERR_EID, CFE_EVS_EventType_ERROR,
                "Invalid cmd length: ID = 0x%X, CC = %d, Exp Len = %d, Len = %d",
                (unsigned int)MessageID, (int)CommandCode, (int)ExpectedLength, (int)ActualLength);
        result = false;
        ++CFE_SB.HKTlmMsg.Payload.CommandErrorCounter;
    }

    return(result);

} /* End of CFE_SB_VerifyCmdLength() */



/******************************************************************************
**  Function:  CFE_SB_ProcessCmdPipePkt()
**
**  Purpose:
**    Function to control actions when an SB command is received.
**
**  Arguments:
**    none
**
**  Return:
**    none
*/
void CFE_SB_ProcessCmdPipePkt(void) {
   switch(CFE_SB_GetMsgId(CFE_SB.CmdPipePktPtr)){

      case CFE_SB_SEND_HK_MID:
         /* Note: Command counter not incremented for this command */
         CFE_SB_SendHKTlmCmd((CCSDS_CommandPacket_t *)CFE_SB.CmdPipePktPtr);
         break;

      case CFE_SB_CMD_MID:
         switch (CFE_SB_GetCmdCode(CFE_SB.CmdPipePktPtr)) {
            case CFE_SB_NOOP_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_Noop_t)))
                {
                    CFE_SB_NoopCmd((CFE_SB_Noop_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_RESET_COUNTERS_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_ResetCounters_t)))
                {
                    /* Note: Command counter not incremented for this command */
                    CFE_SB_ResetCountersCmd((CFE_SB_ResetCounters_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_SEND_SB_STATS_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_SendSbStats_t)))
                {
                    CFE_SB_SendStatsCmd((CFE_SB_SendSbStats_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_SEND_ROUTING_INFO_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_SendRoutingInfo_t)))
                {
                    CFE_SB_SendRoutingInfoCmd((CFE_SB_SendRoutingInfo_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_ENABLE_ROUTE_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_EnableRoute_t)))
                {
                    CFE_SB_EnableRouteCmd((CFE_SB_EnableRoute_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_DISABLE_ROUTE_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_DisableRoute_t)))
                {
                    CFE_SB_DisableRouteCmd((CFE_SB_DisableRoute_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_SEND_PIPE_INFO_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_SendPipeInfo_t)))
                {
                    CFE_SB_SendPipeInfoCmd((CFE_SB_SendPipeInfo_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_SEND_MAP_INFO_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_SendMapInfo_t)))
                {
                    CFE_SB_SendMapInfoCmd((CFE_SB_SendMapInfo_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_SEND_PREV_SUBS_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_SendPrevSubs_t)))
                {
                    CFE_SB_SendPrevSubsCmd((CFE_SB_SendPrevSubs_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_ENABLE_SUB_REPORTING_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_EnableSubReporting_t)))
                {
                    CFE_SB_EnableSubReportingCmd((CFE_SB_EnableSubReporting_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            case CFE_SB_DISABLE_SUB_REPORTING_CC:
                if (CFE_SB_VerifyCmdLength(CFE_SB.CmdPipePktPtr, sizeof(CFE_SB_DisableSubReporting_t)))
                {
                    CFE_SB_DisableSubReportingCmd((CFE_SB_DisableSubReporting_t *)CFE_SB.CmdPipePktPtr);
                }
                break;

            default:
               CFE_EVS_SendEvent(CFE_SB_BAD_CMD_CODE_EID,CFE_EVS_EventType_ERROR,
                     "Invalid Cmd, Unexpected Command Code %d",
                     (int)CFE_SB_GetCmdCode(CFE_SB.CmdPipePktPtr));
               CFE_SB.HKTlmMsg.Payload.CommandErrorCounter++;
               break;
         } /* end switch on cmd code */
         break;

         default:
            CFE_EVS_SendEvent(CFE_SB_BAD_MSGID_EID,CFE_EVS_EventType_ERROR,
                  "Invalid Cmd, Unexpected Msg Id: 0x%04x",
                  (unsigned int)CFE_SB_GetMsgId(CFE_SB.CmdPipePktPtr));
            CFE_SB.HKTlmMsg.Payload.CommandErrorCounter++;
            break;

   } /* end switch on MsgId */

} /* end CFE_SB_ProcessCmdPipePkt */



/******************************************************************************
**  Function:  CFE_SB_NoopCmd()
**
**  Purpose:
**    Handler function the SB command
**
*/
int32 CFE_SB_NoopCmd(const CFE_SB_Noop_t *data)
{
    CFE_EVS_SendEvent(CFE_SB_CMD0_RCVD_EID,CFE_EVS_EventType_INFORMATION,
            "No-op Cmd Rcvd. cFE Version %d.%d.%d.%d",
            CFE_MAJOR_VERSION,CFE_MINOR_VERSION,CFE_REVISION,CFE_MISSION_REV);
    CFE_SB.HKTlmMsg.Payload.CommandCounter++;

    return CFE_SUCCESS;
}

/******************************************************************************
**  Function:  CFE_SB_ResetCountersCmd()
**
**  Purpose:
**    Handler function the SB command
**
*/
int32 CFE_SB_ResetCountersCmd(const CFE_SB_ResetCounters_t *data)
{
    CFE_EVS_SendEvent(CFE_SB_CMD1_RCVD_EID,CFE_EVS_EventType_DEBUG,
            "Reset Counters Cmd Rcvd");

    CFE_SB_ResetCounters();

    return CFE_SUCCESS;
}

/******************************************************************************
**  Function:  CFE_SB_EnableSubReportingCmd()
**
**  Purpose:
**    Handler function the SB command
**
*/
int32 CFE_SB_EnableSubReportingCmd(const CFE_SB_EnableSubReporting_t *data)
{
    CFE_SB_SetSubscriptionReporting(CFE_SB_ENABLE);
    return CFE_SUCCESS;
}

/******************************************************************************
**  Function:  CFE_SB_DisableSubReportingCmd()
**
**  Purpose:
**    Handler function the SB command
**
*/
int32 CFE_SB_DisableSubReportingCmd(const CFE_SB_DisableSubReporting_t *data)
{
    CFE_SB_SetSubscriptionReporting(CFE_SB_DISABLE);
    return CFE_SUCCESS;
}


/******************************************************************************
**  Function:  CFE_SB_SendHKTlmCmd()
**
**  Purpose:
**    Function to send the SB housekeeping packet.
**
**  Arguments:
**    none
**
**  Notes:
**    Command counter not incremented for this command
**
**  Return:
**    none
*/
int32 CFE_SB_SendHKTlmCmd(const CCSDS_CommandPacket_t *data)
{
    CFE_SB.HKTlmMsg.Payload.MemInUse        = CFE_SB.StatTlmMsg.Payload.MemInUse;
    CFE_SB.HKTlmMsg.Payload.UnmarkedMem     = CFE_PLATFORM_SB_BUF_MEMORY_BYTES - CFE_SB.StatTlmMsg.Payload.PeakMemInUse;
    
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_SB.HKTlmMsg);
    CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.HKTlmMsg);

    return CFE_SUCCESS;
}/* end CFE_SB_SendHKTlmCmd */


/******************************************************************************
**  Function:  CFE_SB_ResetCounters()
**
**  Purpose:
**    Function to reset the SB housekeeping counters.
**
**  Arguments:
**    none
**
**  Notes:
**    Command counter not incremented for this command
**
**  Return:
**    none
*/
void CFE_SB_ResetCounters(void){

   CFE_SB.HKTlmMsg.Payload.CommandCounter          = 0;
   CFE_SB.HKTlmMsg.Payload.CommandErrorCounter           = 0;
   CFE_SB.HKTlmMsg.Payload.NoSubscribersCounter    = 0;
   CFE_SB.HKTlmMsg.Payload.DuplicateSubscriptionsCounter = 0;
   CFE_SB.HKTlmMsg.Payload.MsgSendErrorCounter       = 0;
   CFE_SB.HKTlmMsg.Payload.MsgReceiveErrorCounter    = 0;
   CFE_SB.HKTlmMsg.Payload.InternalErrorCounter      = 0;
   CFE_SB.HKTlmMsg.Payload.CreatePipeErrorCounter    = 0;
   CFE_SB.HKTlmMsg.Payload.SubscribeErrorCounter     = 0;
   CFE_SB.HKTlmMsg.Payload.PipeOverflowErrorCounter  = 0;
   CFE_SB.HKTlmMsg.Payload.MsgLimitErrorCounter        = 0;

}/* end CFE_SB_ResetCounters */


/******************************************************************************
**  Function:  CFE_SB_EnableRouteCmd()
**
**  Purpose:
**    SB internal function to enable a specific route. A route is defined as a
**    MsgId/PipeId combination.
**
**  Arguments:
**    MsgPtr  : pointer to the message
**
**  Return:
**    None
*/
int32 CFE_SB_EnableRouteCmd(const CFE_SB_EnableRoute_t *data)
{
    CFE_SB_MsgId_t          MsgId;
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_DestinationD_t   *DestPtr;
    const CFE_SB_RouteCmd_Payload_t      *CmdPtr;

    CmdPtr = &data->Payload;

    MsgId  = CmdPtr->MsgId;
    PipeId = CmdPtr->Pipe;

    /* check cmd parameters */
    if(!CFE_SB_IsValidMsgId(MsgId) ||
       (CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS))
    {
        CFE_EVS_SendEvent(CFE_SB_ENBL_RTE3_EID,CFE_EVS_EventType_ERROR,
                      "Enbl Route Cmd:Invalid Param.Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
        CFE_SB.HKTlmMsg.Payload.CommandErrorCounter++;
        /*
         * returning "success" here as there is no other recourse;
         * the full extent of the error recovery has been done
         */
       return CFE_SUCCESS;
    }/* end if */

    DestPtr = CFE_SB_GetDestPtr(CFE_SB_ConvertMsgIdtoMsgKey(MsgId), PipeId);
    if(DestPtr == NULL){
        CFE_EVS_SendEvent(CFE_SB_ENBL_RTE1_EID,CFE_EVS_EventType_ERROR,
                "Enbl Route Cmd:Route does not exist.Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
        CFE_SB.HKTlmMsg.Payload.CommandErrorCounter++;
        /*
         * returning "success" here as there is no other recourse;
         * the full extent of the error recovery has been done
         */
       return CFE_SUCCESS;
    }/* end if */

    DestPtr->Active = CFE_SB_ACTIVE;
    CFE_EVS_SendEvent(CFE_SB_ENBL_RTE2_EID,CFE_EVS_EventType_DEBUG,
                      "Enabling Route,Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);

    CFE_SB.HKTlmMsg.Payload.CommandCounter++;

    return CFE_SUCCESS;
}/* end CFE_SB_EnableRouteCmd */



/******************************************************************************
**  Function:  CFE_SB_DisableRouteCmd()
**
**  Purpose:
**    SB internal function to disable a specific route. A route is defined as a
**    MsgId/PipeId combination.
**
**  Arguments:
**    MsgPtr  : pointer to the message
**
**  Return:
**    None
*/
int32 CFE_SB_DisableRouteCmd(const CFE_SB_DisableRoute_t *data)
{
    CFE_SB_MsgId_t          MsgId;
    CFE_SB_PipeId_t         PipeId;
    CFE_SB_DestinationD_t   *DestPtr;
    const CFE_SB_RouteCmd_Payload_t      *CmdPtr;

    CmdPtr = &data->Payload;

    MsgId  = CmdPtr->MsgId;
    PipeId = CmdPtr->Pipe;

    /* check cmd parameters */
    if(!CFE_SB_IsValidMsgId(MsgId) ||
       (CFE_SB_ValidatePipeId(PipeId) != CFE_SUCCESS)){
        CFE_EVS_SendEvent(CFE_SB_DSBL_RTE3_EID,CFE_EVS_EventType_ERROR,
                   "Disable Route Cmd:Invalid Param.Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
        CFE_SB.HKTlmMsg.Payload.CommandErrorCounter++;
        /*
         * returning "success" here as there is no other recourse;
         * the full extent of the error recovery has been done
         */
       return CFE_SUCCESS;
    }/* end if */

    DestPtr = CFE_SB_GetDestPtr(CFE_SB_ConvertMsgIdtoMsgKey(MsgId), PipeId);
    if(DestPtr == NULL){
        CFE_EVS_SendEvent(CFE_SB_DSBL_RTE1_EID,CFE_EVS_EventType_ERROR,
            "Disable Route Cmd:Route does not exist,Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
        CFE_SB.HKTlmMsg.Payload.CommandErrorCounter++;
        /*
         * returning "success" here as there is no other recourse;
         * the full extent of the error recovery has been done
         */
       return CFE_SUCCESS;
    }/* end if */

    DestPtr->Active = CFE_SB_INACTIVE;

    CFE_EVS_SendEvent(CFE_SB_DSBL_RTE2_EID,CFE_EVS_EventType_DEBUG,
                      "Route Disabled,Msg 0x%x,Pipe %d",(unsigned int)MsgId,(int)PipeId);
    CFE_SB.HKTlmMsg.Payload.CommandCounter++;

    return CFE_SUCCESS;
}/* end CFE_SB_DisableRouteCmd */


/******************************************************************************
**  Function:  CFE_SB_SendStatsCmd()
**
**  Purpose:
**    SB internal function to send a Software Bus statistics packet
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
int32 CFE_SB_SendStatsCmd(const CFE_SB_SendSbStats_t *data)
{

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &CFE_SB.StatTlmMsg);
    CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.StatTlmMsg);

    CFE_EVS_SendEvent(CFE_SB_SND_STATS_EID,CFE_EVS_EventType_DEBUG,
                      "Software Bus Statistics packet sent");

    CFE_SB.HKTlmMsg.Payload.CommandCounter++;

    return CFE_SUCCESS;
}/* CFE_SB_SendStatsCmd */


/******************************************************************************
**  Function:  CFE_SB_SendRoutingInfoCmd()
**
**  Purpose:
**    SB internal function to handle processing of 'Send Routing Info' Cmd
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
int32 CFE_SB_SendRoutingInfoCmd(const CFE_SB_SendRoutingInfo_t *data)
{
    const CFE_SB_WriteFileInfoCmd_Payload_t *ptr;
    char LocalFilename[OS_MAX_PATH_LEN];
    int32 Stat;

    ptr = &data->Payload;

    CFE_SB_MessageStringGet(LocalFilename, ptr->Filename, CFE_PLATFORM_SB_DEFAULT_ROUTING_FILENAME,
            OS_MAX_PATH_LEN, sizeof(ptr->Filename));

    Stat = CFE_SB_SendRtgInfo(LocalFilename);
    CFE_SB_IncrCmdCtr(Stat);

    return CFE_SUCCESS;
}/* end CFE_SB_SendRoutingInfoCmd */


/******************************************************************************
**  Function:  CFE_SB_SendPipeInfoCmd()
**
**  Purpose:
**    SB internal function to handle processing of 'Send Pipe Info' Cmd
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
int32 CFE_SB_SendPipeInfoCmd(const CFE_SB_SendPipeInfo_t *data)
{
    const CFE_SB_WriteFileInfoCmd_Payload_t *ptr;
    char LocalFilename[OS_MAX_PATH_LEN];
    int32 Stat;

    ptr = &data->Payload;

    CFE_SB_MessageStringGet(LocalFilename, ptr->Filename, CFE_PLATFORM_SB_DEFAULT_PIPE_FILENAME,
            OS_MAX_PATH_LEN, sizeof(ptr->Filename));

    Stat = CFE_SB_SendPipeInfo(LocalFilename);
    CFE_SB_IncrCmdCtr(Stat);

    return CFE_SUCCESS;
}/* end CFE_SB_SendPipeInfoCmd */


/******************************************************************************
**  Function:  CFE_SB_SendMapInfoCmd()
**
**  Purpose:
**    SB internal function to handle processing of 'Send Map Info' Cmd
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
int32 CFE_SB_SendMapInfoCmd(const CFE_SB_SendMapInfo_t *data)
{
    const CFE_SB_WriteFileInfoCmd_Payload_t *ptr;
    char LocalFilename[OS_MAX_PATH_LEN];
    int32 Stat;

    ptr = &data->Payload;

    CFE_SB_MessageStringGet(LocalFilename, ptr->Filename, CFE_PLATFORM_SB_DEFAULT_MAP_FILENAME,
            OS_MAX_PATH_LEN, sizeof(ptr->Filename));

    Stat = CFE_SB_SendMapInfo(LocalFilename);

    CFE_SB_IncrCmdCtr(Stat);

    return CFE_SUCCESS;
}/* end CFE_SB_SendMapInfoCmd */


/******************************************************************************
**  Function:  CFE_SB_SendRoutingInfo()
**
**  Purpose:
**    SB internal function to write the routing information to a file
**
**  Arguments:
**    Pointer to a filename
**
**  Return:
**    CFE_SB_FILE_IO_ERR for file I/O errors or CFE_SUCCESS
*/
int32 CFE_SB_SendRtgInfo(const char *Filename)
{
    CFE_SB_MsgRouteIdx_t        RtgTblIdx;
    const CFE_SB_RouteEntry_t*  RtgTblPtr;
    CFE_SB_MsgKey_Atom_t        MsgKeyVal;
    int32                       fd = 0;
    int32                       WriteStat;
    uint32                      FileSize = 0;
    uint32                      EntryCount = 0;
    CFE_SB_RoutingFileEntry_t   Entry;
    CFE_FS_Header_t             FileHdr;
    CFE_SB_PipeD_t              *pd; 
    CFE_SB_DestinationD_t       *DestPtr;

    fd = OS_creat(Filename, OS_WRITE_ONLY);
    if(fd < OS_FS_SUCCESS){
        CFE_EVS_SendEvent(CFE_SB_SND_RTG_ERR1_EID,CFE_EVS_EventType_ERROR,
                      "Error creating file %s, stat=0x%x",
                      Filename,(unsigned int)fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_FS_InitHeader(&FileHdr, "SB Routing Information", CFE_FS_SubType_SB_ROUTEDATA);

    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    FileSize = WriteStat;

    /* loop through the entire MsgMap */
    for(MsgKeyVal=0; MsgKeyVal < CFE_SB_MAX_NUMBER_OF_MSG_KEYS; ++MsgKeyVal)
    {
        RtgTblIdx = CFE_SB.MsgMap[MsgKeyVal];

        /* Only process table entry if it is used. */
        if(!CFE_SB_IsValidRouteIdx(RtgTblIdx))
        {
            DestPtr = NULL;
            RtgTblPtr = NULL;
        } 
        else 
        {
            RtgTblPtr = CFE_SB_GetRoutePtrFromIdx(RtgTblIdx);
            DestPtr = RtgTblPtr->ListHeadPtr;
        }

        while(DestPtr != NULL){

            pd = CFE_SB_GetPipePtr(DestPtr -> PipeId);
            /* If invalid id, continue on to next entry */
            if (pd != NULL) {
            
                Entry.MsgId     = RtgTblPtr->MsgId;
                Entry.PipeId    = DestPtr -> PipeId;
                Entry.State     = DestPtr -> Active;
                Entry.MsgCnt    = DestPtr -> DestCnt;               
            
                Entry.AppName[0] = 0;
                /* 
                 * NOTE: as long as CFE_ES_GetAppName() returns success, then it 
                 * guarantees null termination of the output.  Return code is not
                 * checked here (bad) but in case of error it does not seem to touch
                 * the buffer, therefore the initialization above will protect for now 
                 */
                CFE_ES_GetAppName(&Entry.AppName[0], pd->AppId, sizeof(Entry.AppName));
                strncpy(&Entry.PipeName[0],CFE_SB_GetPipeName(Entry.PipeId),sizeof(Entry.PipeName));

                WriteStat = OS_write (fd, &Entry, sizeof(CFE_SB_RoutingFileEntry_t));
                if(WriteStat != sizeof(CFE_SB_RoutingFileEntry_t)){
                    CFE_SB_FileWriteByteCntErr(Filename,
                                           sizeof(CFE_SB_RoutingFileEntry_t),
                                           WriteStat);
                    OS_close(fd);
                    return CFE_SB_FILE_IO_ERR;
                }/* end if */

                FileSize += WriteStat;
                EntryCount ++;
            }
            
            DestPtr = DestPtr->Next;

        }/* end while */

    }/* end for */

    OS_close(fd);

    CFE_EVS_SendEvent(CFE_SB_SND_RTG_EID,CFE_EVS_EventType_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                      Filename,(int)FileSize,(int)EntryCount);

    return CFE_SUCCESS;

}/* end CFE_SB_SendRtgInfo */


/******************************************************************************
**  Function:  CFE_SB_SendPipeInfo()
**
**  Purpose:
**    SB internal function to write the Pipe table to a file
**
**  Arguments:
**    Pointer to a filename
**
**  Return:
**    CFE_SB_FILE_IO_ERR for file I/O errors or CFE_SUCCESS
*/
int32 CFE_SB_SendPipeInfo(const char *Filename)
{
    uint16 i;
    int32  fd = 0;
    int32  WriteStat;
    uint32 FileSize = 0;
    uint32 EntryCount = 0;
    CFE_FS_Header_t FileHdr;

    fd = OS_creat(Filename, OS_WRITE_ONLY);

    if(fd < OS_FS_SUCCESS){
        CFE_EVS_SendEvent(CFE_SB_SND_RTG_ERR1_EID,CFE_EVS_EventType_ERROR,
                          "Error creating file %s, stat=0x%x",
                           Filename,(unsigned int)fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_FS_InitHeader(&FileHdr, "SB Pipe Information", CFE_FS_SubType_SB_PIPEDATA);
    
    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    FileSize = WriteStat;

    /* loop through the pipe table */
    for(i=0;i<CFE_PLATFORM_SB_MAX_PIPES;i++){

        if(CFE_SB.PipeTbl[i].InUse==CFE_SB_IN_USE){

            WriteStat = OS_write (fd, &(CFE_SB.PipeTbl[i]), sizeof(CFE_SB_PipeD_t));
            if(WriteStat != sizeof(CFE_SB_PipeD_t)){
                CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_SB_PipeD_t),WriteStat);
                OS_close(fd);
                return CFE_SB_FILE_IO_ERR;
            }/* end if */

            FileSize += WriteStat;
            EntryCount ++;

        }/* end if */

    }/* end for */

    OS_close(fd);

    CFE_EVS_SendEvent(CFE_SB_SND_RTG_EID,CFE_EVS_EventType_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                       Filename,(int)FileSize,(int)EntryCount);

    return CFE_SUCCESS;

}/* end CFE_SB_SendPipeInfo */


/******************************************************************************
**  Function:  CFE_SB_SendMapInfo()
**
**  Purpose:
**    SB internal function to write the Message Map to a file
**
**  Arguments:
**    Pointer to a filename
**
**  Return:
**    CFE_SB_FILE_IO_ERR for file I/O errors or CFE_SUCCESS
*/
int32 CFE_SB_SendMapInfo(const char *Filename)
{
    const CFE_SB_RouteEntry_t*  RtgTblPtr;
    CFE_SB_MsgRouteIdx_t        RtgTblIdx;
    CFE_SB_MsgKey_Atom_t        MsgKeyVal;
    int32  fd = 0;
    int32  WriteStat;
    uint32 FileSize = 0;
    uint32 EntryCount = 0;
    CFE_SB_MsgMapFileEntry_t Entry;
    CFE_FS_Header_t FileHdr;

    fd = OS_creat(Filename, OS_WRITE_ONLY);

    if (fd < OS_FS_SUCCESS){
        CFE_EVS_SendEvent(CFE_SB_SND_RTG_ERR1_EID,CFE_EVS_EventType_ERROR,
                          "Error creating file %s, stat=0x%x",
                           Filename,(unsigned int)fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    /* clear out the cfe file header fields, then populate description and subtype */
    CFE_FS_InitHeader(&FileHdr, "SB Message Map Information", CFE_FS_SubType_SB_MAPDATA);

    WriteStat = CFE_FS_WriteHeader(fd, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t)){
        CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_FS_Header_t),WriteStat);
        OS_close(fd);
        return CFE_SB_FILE_IO_ERR;
    }/* end if */

    FileSize = WriteStat;

    /* loop through the entire MsgMap */
    for(MsgKeyVal=0; MsgKeyVal < CFE_SB_MAX_NUMBER_OF_MSG_KEYS; ++MsgKeyVal)
    {
        RtgTblIdx = CFE_SB_GetRoutingTblIdx(CFE_SB_ValueToMsgKey(MsgKeyVal));

        if(CFE_SB_IsValidRouteIdx(RtgTblIdx))
        {
            RtgTblPtr = CFE_SB_GetRoutePtrFromIdx(RtgTblIdx);

            Entry.MsgId = RtgTblPtr->MsgId;
            Entry.Index = CFE_SB_RouteIdxToValue(RtgTblIdx);

            WriteStat = OS_write (fd, &Entry, sizeof(CFE_SB_MsgMapFileEntry_t));
            if(WriteStat != sizeof(CFE_SB_MsgMapFileEntry_t)){
                CFE_SB_FileWriteByteCntErr(Filename,sizeof(CFE_SB_MsgMapFileEntry_t),WriteStat);
                OS_close(fd);
                return CFE_SB_FILE_IO_ERR;
            }/* end if */

            FileSize += WriteStat;
            EntryCount ++;

        }/* end for */
    }/* end for */

    OS_close(fd);

    CFE_EVS_SendEvent(CFE_SB_SND_RTG_EID,CFE_EVS_EventType_DEBUG,
                      "%s written:Size=%d,Entries=%d",
                      Filename,(int)FileSize,(int)EntryCount);

    return CFE_SUCCESS;

}/* end CFE_SB_SendMapInfo */



/******************************************************************************
**  Function:  CFE_SB_SendPrevSubsCmd()
**
**  Purpose:
**    SB function to build and send an SB packet containing a complete list of
**    current subscriptions.Intended to be used primarily for the Software Bus
**    Networking Application (SBN).
**
**  Arguments:
**    None
**
**  Return:
**    None
*/
int32 CFE_SB_SendPrevSubsCmd(const CFE_SB_SendPrevSubs_t *data)
{
  CFE_SB_MsgRouteIdx_Atom_t i;
  const CFE_SB_RouteEntry_t* RoutePtr;
  uint32 EntryNum = 0;
  uint32 SegNum = 1;
  int32  Stat;
  CFE_SB_DestinationD_t *DestPtr = NULL;

  /* Take semaphore to ensure data does not change during this function */
  CFE_SB_LockSharedData(__func__,__LINE__);

  /* seek msgids that are in use */
  for(i=0;i<CFE_PLATFORM_SB_MAX_MSG_IDS;++i)
  {
      RoutePtr = CFE_SB_GetRoutePtrFromIdx(CFE_SB_ValueToRouteIdx(i));
      if(!CFE_SB_IsValidMsgId(RoutePtr->MsgId))
      {
          DestPtr = NULL;
      }
      else
      {
          DestPtr = CFE_SB.RoutingTbl[i].ListHeadPtr;
      }
        
        while(DestPtr != NULL){

            if(DestPtr->Scope == CFE_SB_GLOBAL){
            
                /* ...add entry into pkt */
                CFE_SB.PrevSubMsg.Payload.Entry[EntryNum].MsgId = RoutePtr->MsgId;
                CFE_SB.PrevSubMsg.Payload.Entry[EntryNum].Qos.Priority = 0;
                CFE_SB.PrevSubMsg.Payload.Entry[EntryNum].Qos.Reliability = 0;
                EntryNum++;
        
                /* send pkt if full */
                if(EntryNum >= CFE_SB_SUB_ENTRIES_PER_PKT){
                  CFE_SB.PrevSubMsg.Payload.PktSegment = SegNum;
                  CFE_SB.PrevSubMsg.Payload.Entries = EntryNum;
                  CFE_SB_UnlockSharedData(__func__,__LINE__);
                  Stat = CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.PrevSubMsg);
                  CFE_SB_LockSharedData(__func__,__LINE__);
                  CFE_EVS_SendEvent(CFE_SB_FULL_SUB_PKT_EID,CFE_EVS_EventType_DEBUG,
                      "Full Sub Pkt %d Sent,Entries=%d,Stat=0x%x\n",(int)SegNum,(int)EntryNum,(unsigned int)Stat);
                  EntryNum = 0;
                  SegNum++;
                }/* end if */
        
                /* break while loop through destinations, onto next CFE_SB.RoutingTbl index */
                /* This is done because we want only one network subscription per msgid */
                /* Later when Qos is used, we may want to take just the highest priority */
                /* subscription if there are more than one */
                break;
                
            }/* end if */
            
            /* Check next destination (if another exists) for global scope */
            DestPtr = DestPtr -> Next;
        
        }/* end while */
  
  }/* end for */ 

  /* if pkt has any number of entries, send it as a partial pkt */
  if(EntryNum > 0){
    CFE_SB.PrevSubMsg.Payload.PktSegment = SegNum;
    CFE_SB.PrevSubMsg.Payload.Entries = EntryNum;
    CFE_SB_UnlockSharedData(__func__,__LINE__);
    Stat = CFE_SB_SendMsg((CFE_SB_Msg_t *)&CFE_SB.PrevSubMsg);
    CFE_SB_LockSharedData(__func__,__LINE__);
    CFE_EVS_SendEvent(CFE_SB_PART_SUB_PKT_EID,CFE_EVS_EventType_DEBUG,
        "Partial Sub Pkt %d Sent,Entries=%d,Stat=0x%x",(int)SegNum,(int)EntryNum,(unsigned int)Stat);
  }/* end if */

  CFE_SB_UnlockSharedData(__func__,__LINE__);

  return CFE_SUCCESS;
}/* end CFE_SB_SendPrevSubsCmd */


/******************************************************************************
**  Function:  CFE_SB_FindGlobalMsgIdCnt()
**
**  Purpose:
**    SB internal function to get a count of the global message ids in use.
**
**  Notes:
**    Subscriptions made with CFE_SB_SubscribeLocal would not be counted.
**    Subscription made with a subscribe API other than CFE_SB_SubscribeLocal are
**    considerd to be global subscriptions. MsgIds with both global and local
**    subscriptions would be counted.
**
**  Arguments:
**
**  Return:
**    None
*/
uint32 CFE_SB_FindGlobalMsgIdCnt(void){

    CFE_SB_MsgRouteIdx_Atom_t i;
    uint32 cnt = 0;
    const CFE_SB_RouteEntry_t* RoutePtr;
    CFE_SB_DestinationD_t *DestPtr = NULL;
    
    for(i=0;i<CFE_PLATFORM_SB_MAX_MSG_IDS;i++)
    {
        RoutePtr = CFE_SB_GetRoutePtrFromIdx(CFE_SB_ValueToRouteIdx(i));
        if(!CFE_SB_IsValidMsgId(RoutePtr->MsgId))
        {
            DestPtr = NULL;
        }
        else
        {
            DestPtr = RoutePtr->ListHeadPtr;
        }
        
        while(DestPtr != NULL){
    
            if(DestPtr->Scope == CFE_SB_GLOBAL){

                cnt++;
                break;

            }/* end if */
            
            /* Check next destination (if another exists) for global scope */
            DestPtr = DestPtr -> Next;
            
        }/* end while */

    }/* end for */

  return cnt;

}/* end CFE_SB_FindGlobalMsgIdCnt */




/******************************************************************************
**  Function:  CFE_SB_IncrCmdCtr()
**
**  Purpose:
**    SB internal function to increment the proper cmd counter based on the
**    status input. This small utility was written to eliminate duplicate code.
**
**  Arguments:
**    status - typically CFE_SUCCESS or an SB error code
**
**  Return:
**    None
*/
void CFE_SB_IncrCmdCtr(int32 status){

    if(status==CFE_SUCCESS){
      CFE_SB.HKTlmMsg.Payload.CommandCounter++;
    }else{
      CFE_SB.HKTlmMsg.Payload.CommandErrorCounter++;
    }/* end if */

}/* end CFE_SB_IncrCmdCtr */



/******************************************************************************
**  Function:  CFE_SB_FileWriteByteCntErr()
**
**  Purpose:
**    SB internal function to report a file write error
**
**  Arguments:
**
**
**  Return:
**    None
*/
void CFE_SB_FileWriteByteCntErr(const char *Filename,uint32 Requested,uint32 Actual){

    CFE_EVS_SendEvent(CFE_SB_FILEWRITE_ERR_EID,CFE_EVS_EventType_ERROR,
                      "File write,byte cnt err,file %s,request=%d,actual=%d",
                       Filename,(int)Requested,(int)Actual);

}/* end CFE_SB_FileWriteByteCntErr() */


/******************************************************************************
**  Function:  CFE_SB_SetSubscriptionReporting()
**
**  Purpose:
**    SB internal function to enable and disable subscription reporting.
**
**  Arguments:
**
**
**  Return:
**    None
*/
void CFE_SB_SetSubscriptionReporting(uint32 state){

    CFE_SB.SubscriptionReporting = state;

}/* end CFE_SB_SetSubscriptionReporting */





