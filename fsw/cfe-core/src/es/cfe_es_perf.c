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

/*
** File: cfe_es_perf.c
**
** Purpose: This file contains the functions that implement the software timing
**  performance markers.
**
*/

/*
** Include Section
*/

#include "osapi.h"
#include "private/cfe_private.h"
#include "cfe_es_perf.h"
#include "cfe_es_log.h"
#include "cfe_es_global.h"
#include "cfe_es_start.h"
#include "cfe_es_events.h"
#include "cfe_es_task.h"
#include "cfe_fs.h"
#include "cfe_psp.h"
#include <string.h>


/*
** Pointer to performance log in the reset area
*/
CFE_ES_PerfData_t      *Perf;
CFE_ES_PerfLogDump_t    CFE_ES_PerfLogDumpStatus;

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Name: CFE_ES_SetupPerfVariables                                               */
/*                                                                               */
/* Purpose:This function initializes filter mask,trigger mask, data & state vals */
/*                                                                               */
/* Assumptions and Notes: This gets called from CFE_ES_Main() at startup         */
/* This code must be called before any other task or code that would use         */
/* CFE_ES_PerfLogEntry() / CFE_ES_PerfLogExit() functions                        */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_SetupPerfVariables(uint32 ResetType)
{
    /* Create a constant union -
     * The "Endian" field will have "0x01" on a big endian processor
     * and will have value "0x00" on a little endian processor.
     */
    const union
    {
        uint16 Word;
        uint8  Endian;
    } EndianCheck = { .Word = 0x0100 };


    uint32      i;

    /*
    ** Set the pointer to the data area
    */
    Perf = (CFE_ES_PerfData_t *)&(CFE_ES_ResetDataPtr->Perf);

    if ( ResetType == CFE_PSP_RST_TYPE_PROCESSOR )
    {
       /*
       ** On a processor reset, just IDLE the data
       ** collection so the ground can dump the data
       */
       Perf->MetaData.State = CFE_ES_PERF_IDLE;
    }
    else
    {

       Perf->MetaData.Version = 1;
       Perf->MetaData.Endian = EndianCheck.Endian;
       Perf->MetaData.TimerTicksPerSecond = CFE_PSP_GetTimerTicksPerSecond();
       Perf->MetaData.TimerLow32Rollover = CFE_PSP_GetTimerLow32Rollover();

       /* set data collection state to waiting for command state */
       Perf->MetaData.State = CFE_ES_PERF_IDLE;
       Perf->MetaData.Mode = CFE_ES_PERF_TRIGGER_START;
       Perf->MetaData.TriggerCount = 0;
       Perf->MetaData.DataStart = 0;
       Perf->MetaData.DataEnd = 0;
       Perf->MetaData.DataCount = 0;
       Perf->MetaData.InvalidMarkerReported = false;
       Perf->MetaData.FilterTriggerMaskSize = CFE_ES_PERF_32BIT_WORDS_IN_MASK;

       for (i=0; i < CFE_ES_PERF_32BIT_WORDS_IN_MASK; i++)
       {
          Perf->MetaData.FilterMask[i]  = CFE_PLATFORM_ES_PERF_FILTMASK_INIT;
          Perf->MetaData.TriggerMask[i] = CFE_PLATFORM_ES_PERF_TRIGMASK_INIT;
       }

    }

    CFE_ES_PerfLogDumpStatus.DataToWrite = 0;
    CFE_ES_PerfLogDumpStatus.ChildID = 0;
    CFE_ES_PerfLogDumpStatus.DataFileName[0] = '\0';
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_StartPerfDataCmd() --                                                  */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_StartPerfDataCmd(const CFE_ES_StartPerfData_t *data)
{
    const CFE_ES_StartPerfCmd_Payload_t *CmdPtr = &data->Payload;

    /* Ensure there is no file write in progress before proceeding */
    if(CFE_ES_PerfLogDumpStatus.DataToWrite == 0)
    {
        /* Make sure Trigger Mode is valid */
        /* cppcheck-suppress unsignedPositive */
        if ((CmdPtr->TriggerMode >= CFE_ES_PERF_TRIGGER_START) && (CmdPtr->TriggerMode < CFE_ES_PERF_MAX_MODES))
        {

            CFE_ES_TaskData.CommandCounter++;

            Perf->MetaData.Mode = CmdPtr->TriggerMode;
            Perf->MetaData.TriggerCount = 0;
            Perf->MetaData.DataStart = 0;
            Perf->MetaData.DataEnd = 0;
            Perf->MetaData.DataCount = 0;
            Perf->MetaData.InvalidMarkerReported = false;
            Perf->MetaData.State = CFE_ES_PERF_WAITING_FOR_TRIGGER; /* this must be done last */

            CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_EID, CFE_EVS_EventType_DEBUG,
                    "Start collecting performance data cmd received, trigger mode = %d",
                    (int)CmdPtr->TriggerMode);
        }
        else
        {
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_TRIG_ERR_EID, CFE_EVS_EventType_ERROR,
                    "Cannot start collecting performance data, trigger mode (%d) out of range (%d to %d)",
                    (int)CmdPtr->TriggerMode, (int)CFE_ES_PERF_TRIGGER_START, (int)CFE_ES_PERF_TRIGGER_END);
        }/* end if */
    }
    else
    {
        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_ERR_EID, CFE_EVS_EventType_ERROR,
                "Cannot start collecting performance data,perf data write in progress");
    } /* end if */

    return CFE_SUCCESS;
} /* End of CFE_ES_StartPerfDataCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_StopPerfDataCmd() --                                                   */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_StopPerfDataCmd(const CFE_ES_StopPerfData_t *data)
{
    const CFE_ES_StopPerfCmd_Payload_t *CmdPtr = &data->Payload;
    int32 Stat;

    /* Ensure there is no file write in progress before proceeding */
    if(CFE_ES_PerfLogDumpStatus.DataToWrite == 0)
    {
        Perf->MetaData.State = CFE_ES_PERF_IDLE;

        /* Copy out the string, using default if unspecified */
        CFE_SB_MessageStringGet(CFE_ES_PerfLogDumpStatus.DataFileName, CmdPtr->DataFileName,
                CFE_PLATFORM_ES_DEFAULT_PERF_DUMP_FILENAME, OS_MAX_PATH_LEN, sizeof(CmdPtr->DataFileName));

        /* Create the file to dump to */
        CFE_ES_PerfLogDumpStatus.DataFileDescriptor = OS_creat(&CFE_ES_PerfLogDumpStatus.DataFileName[0], OS_WRITE_ONLY);


        if(CFE_ES_PerfLogDumpStatus.DataFileDescriptor < 0)
        {
            CFE_ES_TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_PERF_LOG_ERR_EID,CFE_EVS_EventType_ERROR,
                    "Error creating file %s, RC = 0x%08X",
                    &CFE_ES_PerfLogDumpStatus.DataFileName[0], (unsigned int)CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
        }
        else
        {

            /* Spawn a task to write the performance data to a file */
            Stat = CFE_ES_CreateChildTask(&CFE_ES_PerfLogDumpStatus.ChildID,
                    CFE_ES_PERF_CHILD_NAME,
                    CFE_ES_PerfLogDump,
                    CFE_ES_PERF_CHILD_STACK_PTR,
                    CFE_PLATFORM_ES_PERF_CHILD_STACK_SIZE,
                    CFE_PLATFORM_ES_PERF_CHILD_PRIORITY,
                    CFE_ES_PERF_CHILD_FLAGS);

            if(Stat == CFE_SUCCESS)
            {
                /* Note: the file gets closed in the child task */
                CFE_ES_TaskData.CommandCounter++;
                CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_EID,CFE_EVS_EventType_DEBUG,
                        "Perf Stop Cmd Rcvd,%s will write %d entries.%dmS dly every %d entries",
                        CFE_ES_PERF_CHILD_NAME,(int)Perf->MetaData.DataCount,
                        (int)CFE_PLATFORM_ES_PERF_CHILD_MS_DELAY,(int)CFE_PLATFORM_ES_PERF_ENTRIES_BTWN_DLYS);
            }
            else
            {
                /* close the fd */
                OS_close( CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
                CFE_ES_TaskData.CommandErrorCounter++;
                CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_ERR1_EID, CFE_EVS_EventType_ERROR,
                        "Stop performance data cmd,Error creating child task RC=0x%08X",(unsigned int)Stat);
            }/* end if */

        }/* end if fd < 0 */

    }/* if data to write == 0 */
    else
    {

        CFE_ES_TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_ERR2_EID, CFE_EVS_EventType_ERROR,
                "Stop performance data cmd ignored,perf data write in progress");
    }/* end if */

    return CFE_SUCCESS;
} /* End of CFE_ES_StopPerfDataCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*  Function:  CFE_ES_PerfLogDump()                                              */
/*                                                                               */
/*  Purpose:                                                                     */
/*    Write performance data to a file                                           */
/*                                                                               */
/*  Arguments:                                                                   */
/*    None                                                                       */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfLogDump(void){

    int32               WriteStat;
    uint32              i;
    uint32              FileSize;
    CFE_FS_Header_t     FileHdr;

    CFE_ES_RegisterChildTask();


    /* Zero cFE header, then fill in fields */
    CFE_FS_InitHeader(&FileHdr, CFE_ES_PERF_LOG_DESC, CFE_FS_SubType_ES_PERFDATA);

    /* write the cFE header to the file */
    WriteStat = CFE_FS_WriteHeader( CFE_ES_PerfLogDumpStatus.DataFileDescriptor, &FileHdr);
    if(WriteStat != sizeof(CFE_FS_Header_t))
    {
        CFE_ES_FileWriteByteCntErr(&CFE_ES_PerfLogDumpStatus.DataFileName[0],
                                   sizeof(CFE_FS_Header_t),WriteStat);
        
        OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
        CFE_ES_ExitChildTask();
        /* normally ExitChildTask() does not return, but it DOES under UT */
        return;
    }/* end if */
    FileSize = WriteStat;

    /* write the performance metadata to the file */
    WriteStat = OS_write(CFE_ES_PerfLogDumpStatus.DataFileDescriptor,&Perf->MetaData,sizeof(CFE_ES_PerfMetaData_t));
    if(WriteStat != sizeof(CFE_ES_PerfMetaData_t))
    {
        CFE_ES_FileWriteByteCntErr(&CFE_ES_PerfLogDumpStatus.DataFileName[0],
                                   sizeof(CFE_ES_PerfMetaData_t),WriteStat);
        OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
        CFE_ES_ExitChildTask();
        /* normally ExitChildTask() does not return, but it DOES under UT */
        return;
    }/* end if */
    FileSize += WriteStat;

    CFE_ES_PerfLogDumpStatus.DataToWrite = Perf->MetaData.DataCount;

    /* write the collected data to the file */
    for(i=0; i < Perf->MetaData.DataCount; i++){
      WriteStat = OS_write (CFE_ES_PerfLogDumpStatus.DataFileDescriptor, &Perf->DataBuffer[i], sizeof(CFE_ES_PerfDataEntry_t));
      if(WriteStat != sizeof(CFE_ES_PerfDataEntry_t))
      {
        CFE_ES_FileWriteByteCntErr(&CFE_ES_PerfLogDumpStatus.DataFileName[0],
                                   sizeof(CFE_ES_PerfDataEntry_t),WriteStat);
        OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);
        /* Reset the DataToWrite variable, so a new file can be written */
        CFE_ES_PerfLogDumpStatus.DataToWrite = 0;
        CFE_ES_ExitChildTask();
        /* normally ExitChildTask() does not return, but it DOES under UT */
        return;
      }/* end if */
      FileSize += WriteStat;
      CFE_ES_PerfLogDumpStatus.DataToWrite--;
      if((i % CFE_PLATFORM_ES_PERF_ENTRIES_BTWN_DLYS) == 0){
        OS_TaskDelay(CFE_PLATFORM_ES_PERF_CHILD_MS_DELAY);
      }/* end if */

    }/* end for */

    OS_close(CFE_ES_PerfLogDumpStatus.DataFileDescriptor);

    CFE_EVS_SendEvent(CFE_ES_PERF_DATAWRITTEN_EID,CFE_EVS_EventType_DEBUG,
                      "%s written:Size=%d,EntryCount=%d",
                       &CFE_ES_PerfLogDumpStatus.DataFileName[0],(int)FileSize,
                       (int)Perf->MetaData.DataCount);

    CFE_ES_ExitChildTask();

}/* end CFE_ES_PerfLogDump */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_SetPerfFilterMaskCmd() --                                              */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_SetPerfFilterMaskCmd(const CFE_ES_SetPerfFilterMask_t *data)
{
    const CFE_ES_SetPerfFilterMaskCmd_Payload_t *cmd = &data->Payload;

    if(cmd->FilterMaskNum < CFE_ES_PERF_32BIT_WORDS_IN_MASK ){

        Perf->MetaData.FilterMask[cmd->FilterMaskNum] = cmd->FilterMask;

        CFE_EVS_SendEvent(CFE_ES_PERF_FILTMSKCMD_EID, CFE_EVS_EventType_DEBUG,
                "Set Performance Filter Mask Cmd rcvd, num %d, val 0x%x",
                (int)cmd->FilterMaskNum,(unsigned int)cmd->FilterMaskNum);

        CFE_ES_TaskData.CommandCounter++;
    }else{
        CFE_EVS_SendEvent(CFE_ES_PERF_FILTMSKERR_EID, CFE_EVS_EventType_ERROR,
                "Performance Filter Mask Cmd Error,Index(%d)out of range(%d)",
                (int)cmd->FilterMaskNum,(int)CFE_ES_PERF_32BIT_WORDS_IN_MASK);

        CFE_ES_TaskData.CommandErrorCounter++;

    }

    return CFE_SUCCESS;
} /* End of CFE_ES_SetPerfFilterMaskCmd() */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                               */
/* CFE_ES_SetPerfTriggerMaskCmd() --                                             */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 CFE_ES_SetPerfTriggerMaskCmd(const CFE_ES_SetPerfTriggerMask_t *data)
{
    const CFE_ES_SetPerfTrigMaskCmd_Payload_t *cmd = &data->Payload;

    if(cmd->TriggerMaskNum < CFE_ES_PERF_32BIT_WORDS_IN_MASK ){

        Perf->MetaData.TriggerMask[cmd->TriggerMaskNum] = cmd->TriggerMask;

        CFE_EVS_SendEvent(CFE_ES_PERF_TRIGMSKCMD_EID, CFE_EVS_EventType_DEBUG,
                "Set Performance Trigger Mask Cmd rcvd,num %d, val 0x%x",
                (int)cmd->TriggerMaskNum,(int)cmd->TriggerMaskNum);

        CFE_ES_TaskData.CommandCounter++;

    }else{
        CFE_EVS_SendEvent(CFE_ES_PERF_TRIGMSKERR_EID, CFE_EVS_EventType_ERROR,
                "Performance Trigger Mask Cmd Error,Index(%d)out of range(%d)",
                (int)cmd->TriggerMaskNum,(int)CFE_ES_PERF_32BIT_WORDS_IN_MASK);

        CFE_ES_TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
} /* End of CFE_ES_SetPerfTriggerMaskCmd() */


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Name: CFE_ES_PerfLogAdd                                                       */
/*                                                                               */
/* Purpose: This function adds a new entry to the data buffer.                   */
/*                                                                               */
/* Assumptions and Notes:                                                        */
/*                                                                               */
/*  This function implements a circular buffer using an array.                   */
/*      DataStart points to first stored entry                                   */
/*      DataEnd points to next available entry                                   */
/*      if DataStart == DataEnd then the buffer is either empty or full          */
/*      depending on the value of the DataCount                                  */
/*                                                                               */
/*  Time is stored as 2 32 bit integers, (TimerLower32, TimerUpper32):           */
/*      TimerLower32 is the curent value of the hardware timer register.         */
/*      TimerUpper32 is the number of times the timer has rolled over.           */
/*                                                                               */
/*  Time is stored as a absolute time instead of a relative time between log     */
/*  entries. This will yield better accuracy since storing relative time between */
/*  entries will accumulate (rounding/sampling) errors over time.  It also is    */
/*  faster since the time does not need to be calculated.                        */
/*                                                                               */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
    int32         IntFlags;

    if (Perf->MetaData.State != CFE_ES_PERF_IDLE) {

        /* if marker is out of range... */
        if(Marker >= CFE_MISSION_ES_PERF_MAX_IDS){

          /* if marker has not been reported previously ... */
          if(Perf->MetaData.InvalidMarkerReported == false){
            CFE_ES_WriteToSysLog("ES PERF:Invalid performance marker %d,max is %d\n",
                    (unsigned int)Marker,(CFE_MISSION_ES_PERF_MAX_IDS - 1));
            Perf->MetaData.InvalidMarkerReported = true;
          }/* end if */

          return;

        }/* end if */


        /* is this id filtered */
        if (CFE_ES_TEST_LONG_MASK(Perf->MetaData.FilterMask, Marker)) {

           /* disable interrupts to guarentee exclusive access to the data structures */
            IntFlags = OS_IntLock();

            Perf->DataBuffer[Perf->MetaData.DataEnd].Data = (Marker | (EntryExit << CFE_MISSION_ES_PERF_EXIT_BIT));
            CFE_PSP_Get_Timebase((uint32*)&Perf->DataBuffer[Perf->MetaData.DataEnd].TimerUpper32,(uint32*)&Perf->DataBuffer[Perf->MetaData.DataEnd].TimerLower32);

            Perf->MetaData.DataEnd++;
            if (Perf->MetaData.DataEnd >= CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE) {
                Perf->MetaData.DataEnd = 0;
            }

            /* we have filled up the buffer */
            Perf->MetaData.DataCount++;
            if (Perf->MetaData.DataCount > CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE) {

                Perf->MetaData.DataCount = CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE;

                /* after the buffer fills up start and end point to the same entry since we
                   are now overwriting old data */
                Perf->MetaData.DataStart = Perf->MetaData.DataEnd;
            }

            /* waiting for trigger */
            if (Perf->MetaData.State == CFE_ES_PERF_WAITING_FOR_TRIGGER) {

                if (CFE_ES_TEST_LONG_MASK(Perf->MetaData.TriggerMask, Marker)) {
                    Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
                }
            }
            /* triggered */
            if (Perf->MetaData.State == CFE_ES_PERF_TRIGGERED) {

                Perf->MetaData.TriggerCount++;
                if (Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_START) {

                    if (Perf->MetaData.TriggerCount >= CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE) {
                        Perf->MetaData.State = CFE_ES_PERF_IDLE;
                    }
                }
                else if (Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_CENTER) {

                    if (Perf->MetaData.TriggerCount >= CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE / 2) {
                        Perf->MetaData.State = CFE_ES_PERF_IDLE;
                    }
                }
                else if (Perf->MetaData.Mode == CFE_ES_PERF_TRIGGER_END) {

                    Perf->MetaData.State = CFE_ES_PERF_IDLE;
                }
            }

            /* enable interrupts */
            OS_IntUnlock(IntFlags);
        }
    }
}/* end CFE_ES_PerfLogAdd */


