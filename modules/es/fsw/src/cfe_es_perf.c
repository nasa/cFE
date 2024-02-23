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
** File: cfe_es_perf.c
**
** Purpose: This file contains the functions that implement the software timing
**  performance markers.
**
*/

/*
** Include Section
*/
#include "cfe_es_module_all.h"

#include <string.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
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
    } EndianCheck = {.Word = 0x0100};

    uint32             i;
    CFE_ES_PerfData_t *Perf;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    if (ResetType == CFE_PSP_RST_TYPE_PROCESSOR)
    {
        /*
        ** On a processor reset, just IDLE the data
        ** collection so the ground can dump the data
        */
        Perf->MetaData.State = CFE_ES_PERF_IDLE;
    }
    else
    {
        Perf->MetaData.Version             = 1;
        Perf->MetaData.Endian              = EndianCheck.Endian;
        Perf->MetaData.TimerTicksPerSecond = CFE_PSP_GetTimerTicksPerSecond();
        Perf->MetaData.TimerLow32Rollover  = CFE_PSP_GetTimerLow32Rollover();

        /* set data collection state to waiting for command state */
        Perf->MetaData.State                 = CFE_ES_PERF_IDLE;
        Perf->MetaData.Mode                  = CFE_ES_PerfTrigger_START;
        Perf->MetaData.TriggerCount          = 0;
        Perf->MetaData.DataStart             = 0;
        Perf->MetaData.DataEnd               = 0;
        Perf->MetaData.DataCount             = 0;
        Perf->MetaData.InvalidMarkerReported = false;
        Perf->MetaData.FilterTriggerMaskSize = CFE_ES_PERF_32BIT_WORDS_IN_MASK;

        for (i = 0; i < CFE_ES_PERF_32BIT_WORDS_IN_MASK; i++)
        {
            Perf->MetaData.FilterMask[i]  = CFE_PLATFORM_ES_PERF_FILTMASK_INIT;
            Perf->MetaData.TriggerMask[i] = CFE_PLATFORM_ES_PERF_TRIGMASK_INIT;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_ES_GetPerfLogDumpRemaining(void)
{
    CFE_ES_PerfDumpGlobal_t *PerfDumpState = &CFE_ES_Global.BackgroundPerfDumpState;
    CFE_ES_PerfDumpState_t   CurrentState  = PerfDumpState->CurrentState;
    uint32                   Result;
    CFE_ES_PerfData_t *      Perf;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    /* note this reads the data "live" without exclusion and as such it
     * may change even between checking the state and checking the value.
     * This shouldn't be a big deal, as the result should still be meaningful
     * for a progress report, and the actual 32-bit counters should be atomic */
    if (CurrentState > CFE_ES_PerfDumpState_IDLE && CurrentState < CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES)
    {
        /* dump is requested but not yet to entry writing state,
         * report the entire data count from perf log */
        Result = Perf->MetaData.DataCount;
    }
    else if (CurrentState == CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES)
    {
        /* dump is in active writing state,
         * report the block counter (number remaining) */
        Result = PerfDumpState->StateCounter;
    }
    else
    {
        /* no dump active or dump is complete, report 0 */
        Result = 0;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_StartPerfDataCmd(const CFE_ES_StartPerfDataCmd_t *data)
{
    const CFE_ES_StartPerfCmd_Payload_t *CmdPtr        = &data->Payload;
    CFE_ES_PerfDumpGlobal_t *            PerfDumpState = &CFE_ES_Global.BackgroundPerfDumpState;
    CFE_ES_PerfData_t *                  Perf;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    /* Ensure there is no file write in progress before proceeding */
    if (PerfDumpState->CurrentState == CFE_ES_PerfDumpState_IDLE &&
        PerfDumpState->PendingState == CFE_ES_PerfDumpState_IDLE)
    {
        /* Make sure Trigger Mode is valid */
        if (CmdPtr->TriggerMode <= CFE_ES_PerfTrigger_END)
        {
            CFE_ES_Global.TaskData.CommandCounter++;

            /* Taking lock here as this might be changing states from one active mode to another.
             * In that case, need to make sure that the log is not written to while resetting the counters. */
            OS_MutSemTake(CFE_ES_Global.PerfDataMutex);
            Perf->MetaData.Mode                  = CmdPtr->TriggerMode;
            Perf->MetaData.TriggerCount          = 0;
            Perf->MetaData.DataStart             = 0;
            Perf->MetaData.DataEnd               = 0;
            Perf->MetaData.DataCount             = 0;
            Perf->MetaData.InvalidMarkerReported = false;
            Perf->MetaData.State                 = CFE_ES_PERF_WAITING_FOR_TRIGGER; /* this must be done last */
            OS_MutSemGive(CFE_ES_Global.PerfDataMutex);

            CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_EID, CFE_EVS_EventType_DEBUG,
                              "Start collecting performance data cmd received, trigger mode = %d",
                              (int)CmdPtr->TriggerMode);
        }
        else
        {
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_TRIG_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Cannot start collecting performance data, trigger mode (%d) out of range (%d to %d)",
                              (int)CmdPtr->TriggerMode, (int)CFE_ES_PerfTrigger_START, (int)CFE_ES_PerfTrigger_END);
        }
    }
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_PERF_STARTCMD_ERR_EID, CFE_EVS_EventType_ERROR,
                          "Cannot start collecting performance data,perf data write in progress");
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_StopPerfDataCmd(const CFE_ES_StopPerfDataCmd_t *data)
{
    const CFE_ES_StopPerfCmd_Payload_t *CmdPtr        = &data->Payload;
    CFE_ES_PerfDumpGlobal_t *           PerfDumpState = &CFE_ES_Global.BackgroundPerfDumpState;
    CFE_ES_PerfData_t *                 Perf;
    int32                               Status;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    /* Ensure there is no file write in progress before proceeding */
    /* note - also need to check the PendingState here, in case this command
     * was sent twice in succession and the background task has not awakened yet */
    if (PerfDumpState->CurrentState == CFE_ES_PerfDumpState_IDLE &&
        PerfDumpState->PendingState == CFE_ES_PerfDumpState_IDLE)
    {
        Perf->MetaData.State = CFE_ES_PERF_IDLE;

        /* Copy out the string, using default if unspecified */
        Status = CFE_FS_ParseInputFileNameEx(PerfDumpState->DataFileName, CmdPtr->DataFileName,
                                             sizeof(PerfDumpState->DataFileName), sizeof(CmdPtr->DataFileName),
                                             CFE_PLATFORM_ES_DEFAULT_PERF_DUMP_FILENAME,
                                             CFE_FS_GetDefaultMountPoint(CFE_FS_FileCategory_BINARY_DATA_DUMP),
                                             CFE_FS_GetDefaultExtension(CFE_FS_FileCategory_BINARY_DATA_DUMP));

        if (Status == CFE_SUCCESS)
        {
            PerfDumpState->PendingState = CFE_ES_PerfDumpState_INIT;
            CFE_ES_BackgroundWakeup();

            CFE_ES_Global.TaskData.CommandCounter++;

            CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_EID, CFE_EVS_EventType_DEBUG,
                              "Perf Stop Cmd Rcvd, will write %d entries.%dmS dly every %d entries",
                              (int)Perf->MetaData.DataCount, (int)CFE_PLATFORM_ES_PERF_CHILD_MS_DELAY,
                              (int)CFE_PLATFORM_ES_PERF_ENTRIES_BTWN_DLYS);
        }
        else
        {
            CFE_ES_Global.TaskData.CommandErrorCounter++;
            CFE_EVS_SendEvent(CFE_ES_PERF_LOG_ERR_EID, CFE_EVS_EventType_ERROR, "Error parsing filename, RC = %d",
                              (int)Status);
        }

    } /* if data to write == 0 */
    else
    {
        CFE_ES_Global.TaskData.CommandErrorCounter++;
        CFE_EVS_SendEvent(CFE_ES_PERF_STOPCMD_ERR2_EID, CFE_EVS_EventType_ERROR,
                          "Stop performance data cmd ignored,perf data write in progress");
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_RunPerfLogDump(uint32 ElapsedTime, void *Arg)
{
    CFE_ES_PerfDumpGlobal_t *State = (CFE_ES_PerfDumpGlobal_t *)Arg;
    int32                    OsStatus;
    int32                    Status;
    CFE_FS_Header_t          FileHdr;
    size_t                   BlockSize;
    CFE_ES_PerfData_t *      Perf;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    /*
     * each time this background job is re-entered after a time delay,
     * accumulate a work credit amount based on the elapsed time.
     *
     * This implements work-throttling as a form of cooperative
     * CPU sharing with other low priority background jobs.
     */
    State->WorkCredit += (ElapsedTime * CFE_PLATFORM_ES_PERF_ENTRIES_BTWN_DLYS) / CFE_PLATFORM_ES_PERF_CHILD_MS_DELAY;

    /*
     * do not allow credit to accumulate indefinitely -
     * after a long idle time this would defeat the purpose.
     */
    if (State->WorkCredit > CFE_PLATFORM_ES_PERF_ENTRIES_BTWN_DLYS)
    {
        State->WorkCredit = CFE_PLATFORM_ES_PERF_ENTRIES_BTWN_DLYS;
    }

    while (State->WorkCredit > 0)
    {
        --State->WorkCredit;

        if (State->PendingState != State->CurrentState)
        {
            /*
             * Handle state change/entry logic.
             * Zero the block counter register (may be changed later).
             */
            State->StateCounter = 0;

            switch (State->PendingState)
            {
                case CFE_ES_PerfDumpState_OPEN_FILE:
                    /* Create the file to dump to */
                    OsStatus = OS_OpenCreate(&State->FileDesc, State->DataFileName,
                                             OS_FILE_FLAG_CREATE | OS_FILE_FLAG_TRUNCATE, OS_WRITE_ONLY);
                    if (OsStatus != OS_SUCCESS)
                    {
                        State->FileDesc = OS_OBJECT_ID_UNDEFINED;
                        CFE_EVS_SendEvent(CFE_ES_PERF_LOG_ERR_EID, CFE_EVS_EventType_ERROR,
                                          "Error creating file %s, RC = %ld", State->DataFileName, (long)OsStatus);
                    }
                    State->FileSize = 0;
                    break;

                case CFE_ES_PerfDumpState_DELAY:
                    /*
                     * Add a state entry delay before locking the "Perf" structure to
                     * ensure that any foreground task that may have been writing to this
                     * structure has completed its access.
                     *
                     * Note that the state should already have been set to IDLE, so
                     * no new writes will start, this is just to yield the CPU such that
                     * any already-started writes may finish.
                     *
                     * This can be done by simply zeroing out the current credit,
                     * which will cause this loop to exit for now and resume after
                     * some time delay (does not really matter how much time).
                     */
                    State->WorkCredit = 0;
                    break;

                case CFE_ES_PerfDumpState_LOCK_DATA:
                    OS_MutSemTake(CFE_ES_Global.PerfDataMutex);
                    break;

                case CFE_ES_PerfDumpState_WRITE_FS_HDR:
                case CFE_ES_PerfDumpState_WRITE_PERF_METADATA:
                    State->StateCounter = 1;
                    break;

                case CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES:
                    State->DataPos      = Perf->MetaData.DataStart;
                    State->StateCounter = Perf->MetaData.DataCount;
                    break;

                case CFE_ES_PerfDumpState_UNLOCK_DATA:
                    OS_MutSemGive(CFE_ES_Global.PerfDataMutex);
                    break;

                case CFE_ES_PerfDumpState_CLOSE_FILE:
                    /* close the fd */
                    if (OS_ObjectIdDefined(State->FileDesc))
                    {
                        OS_close(State->FileDesc);
                        State->FileDesc = OS_OBJECT_ID_UNDEFINED;
                    }
                    break;

                default:
                    break;
            }

            State->CurrentState = State->PendingState;
        }

        if (State->CurrentState == CFE_ES_PerfDumpState_IDLE)
        {
            break;
        }

        if (State->StateCounter == 0)
        {
            /*
             * State is finished, do any final error checking and logging
             *
             * Default transition is to the next state by numeric value.
             * This prevent endless looping in the same state.
             *
             * The switch statement can override this transition, however,
             * based on any relevant error checks.
             */
            State->PendingState = 1 + State->CurrentState;
            if (State->PendingState >= CFE_ES_PerfDumpState_MAX)
            {
                State->PendingState = CFE_ES_PerfDumpState_IDLE;
            }
            switch (State->CurrentState)
            {
                case CFE_ES_PerfDumpState_OPEN_FILE:
                    if (!OS_ObjectIdDefined(State->FileDesc))
                    {
                        State->PendingState = CFE_ES_PerfDumpState_IDLE;
                    }
                    break;

                case CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES:
                    CFE_EVS_SendEvent(CFE_ES_PERF_DATAWRITTEN_EID, CFE_EVS_EventType_DEBUG,
                                      "%s written:Size=%lu,EntryCount=%lu", State->DataFileName,
                                      (unsigned long)State->FileSize, (unsigned long)Perf->MetaData.DataCount);
                    break;

                default:
                    break;
            }
        }
        else
        {
            /*
             * State is in progress, perform work item(s) as required
             */
            Status    = 0;
            BlockSize = 0;
            switch (State->CurrentState)
            {
                case CFE_ES_PerfDumpState_WRITE_FS_HDR:
                    /* Zero cFE header, then fill in fields */
                    CFE_FS_InitHeader(&FileHdr, CFE_ES_PERF_LOG_DESC, CFE_FS_SubType_ES_PERFDATA);
                    /* predicted total length of final output */
                    FileHdr.Length =
                        sizeof(CFE_ES_PerfMetaData_t) + (Perf->MetaData.DataCount * sizeof(CFE_ES_PerfDataEntry_t));
                    /* write the cFE header to the file */
                    Status    = CFE_FS_WriteHeader(State->FileDesc, &FileHdr);
                    BlockSize = sizeof(CFE_FS_Header_t);
                    break;

                case CFE_ES_PerfDumpState_WRITE_PERF_METADATA:
                    /* write the performance metadata to the file */
                    BlockSize = sizeof(CFE_ES_PerfMetaData_t);
                    OsStatus  = OS_write(State->FileDesc, &Perf->MetaData, BlockSize);
                    Status    = (long)OsStatus; /* status type conversion (size) */
                    break;

                case CFE_ES_PerfDumpState_WRITE_PERF_ENTRIES:
                    BlockSize = sizeof(CFE_ES_PerfDataEntry_t);
                    OsStatus  = OS_write(State->FileDesc, &Perf->DataBuffer[State->DataPos], BlockSize);
                    Status    = (long)OsStatus; /* status type conversion (size) */

                    ++State->DataPos;
                    if (State->DataPos >= CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE)
                    {
                        State->DataPos = 0;
                    }
                    break;

                default:
                    break;
            }

            if (BlockSize != 0)
            {
                if (Status != BlockSize)
                {
                    CFE_ES_FileWriteByteCntErr(State->DataFileName, BlockSize, Status);

                    State->PendingState = CFE_ES_PerfDumpState_CLEANUP;
                }
                else
                {
                    State->FileSize += BlockSize;
                }
            }

            --State->StateCounter;
        }
    }

    /*
     * Return "true" if activity is ongoing, or "false" if not active
     */
    return (State->CurrentState != CFE_ES_PerfDumpState_IDLE);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SetPerfFilterMaskCmd(const CFE_ES_SetPerfFilterMaskCmd_t *data)
{
    const CFE_ES_SetPerfFilterMaskCmd_Payload_t *cmd = &data->Payload;
    CFE_ES_PerfData_t *                          Perf;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    if (cmd->FilterMaskNum < CFE_ES_PERF_32BIT_WORDS_IN_MASK)
    {
        Perf->MetaData.FilterMask[cmd->FilterMaskNum] = cmd->FilterMask;

        CFE_EVS_SendEvent(CFE_ES_PERF_FILTMSKCMD_EID, CFE_EVS_EventType_DEBUG,
                          "Set Performance Filter Mask Cmd rcvd, num %u, val 0x%08X", (unsigned int)cmd->FilterMaskNum,
                          (unsigned int)cmd->FilterMask);

        CFE_ES_Global.TaskData.CommandCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_PERF_FILTMSKERR_EID, CFE_EVS_EventType_ERROR,
                          "Performance Filter Mask Cmd Error,Index(%u)out of range(%u)",
                          (unsigned int)cmd->FilterMaskNum, (unsigned int)CFE_ES_PERF_32BIT_WORDS_IN_MASK);

        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ES_SetPerfTriggerMaskCmd(const CFE_ES_SetPerfTriggerMaskCmd_t *data)
{
    const CFE_ES_SetPerfTrigMaskCmd_Payload_t *cmd = &data->Payload;
    CFE_ES_PerfData_t *                        Perf;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    if (cmd->TriggerMaskNum < CFE_ES_PERF_32BIT_WORDS_IN_MASK)
    {
        Perf->MetaData.TriggerMask[cmd->TriggerMaskNum] = cmd->TriggerMask;

        CFE_EVS_SendEvent(CFE_ES_PERF_TRIGMSKCMD_EID, CFE_EVS_EventType_DEBUG,
                          "Set Performance Trigger Mask Cmd rcvd,num %u, val 0x%08X", (unsigned int)cmd->TriggerMaskNum,
                          (unsigned int)cmd->TriggerMask);

        CFE_ES_Global.TaskData.CommandCounter++;
    }
    else
    {
        CFE_EVS_SendEvent(CFE_ES_PERF_TRIGMSKERR_EID, CFE_EVS_EventType_ERROR,
                          "Performance Trigger Mask Cmd Error,Index(%u)out of range(%u)",
                          (unsigned int)cmd->TriggerMaskNum, (unsigned int)CFE_ES_PERF_32BIT_WORDS_IN_MASK);

        CFE_ES_Global.TaskData.CommandErrorCounter++;
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_ES_PerfLogAdd(uint32 Marker, uint32 EntryExit)
{
    CFE_ES_PerfDataEntry_t EntryData;
    uint32                 DataEnd;
    CFE_ES_PerfData_t *    Perf;

    /*
    ** Set the pointer to the data area
    */
    Perf = &CFE_ES_Global.ResetDataPtr->Perf;

    /*
     * If the global state is idle, exit immediately without locking or doing anything
     */
    if (Perf->MetaData.State == CFE_ES_PERF_IDLE)
    {
        return;
    }

    /* if marker is out of range... */
    if (Marker >= CFE_MISSION_ES_PERF_MAX_IDS)
    {
        /* if marker has not been reported previously ... */
        if (Perf->MetaData.InvalidMarkerReported == false)
        {
            CFE_ES_WriteToSysLog("%s: Invalid performance marker %d,max is %d\n", __func__, (unsigned int)Marker,
                                 (CFE_MISSION_ES_PERF_MAX_IDS - 1));
            Perf->MetaData.InvalidMarkerReported = true;
        }

        return;
    }

    /*
     * check if this ID is filtered.
     * This is also done outside the lock -
     * normally masks should NOT be changed while perf log is active / non-idle,
     * so although this is reading a global it should be constant, and this avoids
     * locking (and potential task switch) if the data is ultimately not going to
     * be written to the log.
     */
    if (!CFE_ES_TEST_LONG_MASK(Perf->MetaData.FilterMask, Marker))
    {
        return;
    }

    /*
     * prepare the entry data (timestamp) before locking,
     * just in case the locking operation incurs a delay
     */
    EntryData.Data = (Marker | (EntryExit << CFE_MISSION_ES_PERF_EXIT_BIT));
    CFE_PSP_Get_Timebase(&EntryData.TimerUpper32, &EntryData.TimerLower32);

    /*
     * Acquire the perflog mutex before writing into the shared area.
     * Note this lock is held for long periods while a background dump
     * is taking place, but the dump should never be active at the
     * same time that a capture/record is taking place.
     */
    OS_MutSemTake(CFE_ES_Global.PerfDataMutex);

    /*
     * Confirm that the global is still non-idle after lock
     * (state could become idle while getting lock)
     */
    if (Perf->MetaData.State != CFE_ES_PERF_IDLE)
    {
        /* copy data to next perflog slot */
        DataEnd                   = Perf->MetaData.DataEnd;
        Perf->DataBuffer[DataEnd] = EntryData;

        ++DataEnd;
        if (DataEnd >= CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE)
        {
            DataEnd = 0;
        }
        Perf->MetaData.DataEnd = DataEnd;

        /* we have filled up the buffer */
        if (Perf->MetaData.DataCount < CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE)
        {
            Perf->MetaData.DataCount++;
        }
        else
        {
            /* after the buffer fills up start and end point to the same entry since we
               are now overwriting old data */
            Perf->MetaData.DataStart = Perf->MetaData.DataEnd;
        }

        /* waiting for trigger */
        if (Perf->MetaData.State == CFE_ES_PERF_WAITING_FOR_TRIGGER)
        {
            if (CFE_ES_TEST_LONG_MASK(Perf->MetaData.TriggerMask, Marker))
            {
                Perf->MetaData.State = CFE_ES_PERF_TRIGGERED;
            }
        }

        /* triggered */
        if (Perf->MetaData.State == CFE_ES_PERF_TRIGGERED)
        {
            Perf->MetaData.TriggerCount++;
            if (Perf->MetaData.Mode == CFE_ES_PerfTrigger_START)
            {
                if (Perf->MetaData.TriggerCount >= CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE)
                {
                    Perf->MetaData.State = CFE_ES_PERF_IDLE;
                }
            }
            else if (Perf->MetaData.Mode == CFE_ES_PerfTrigger_CENTER)
            {
                if (Perf->MetaData.TriggerCount >= CFE_PLATFORM_ES_PERF_DATA_BUFFER_SIZE / 2)
                {
                    Perf->MetaData.State = CFE_ES_PERF_IDLE;
                }
            }
            else if (Perf->MetaData.Mode == CFE_ES_PerfTrigger_END)
            {
                Perf->MetaData.State = CFE_ES_PERF_IDLE;
            }
        }
    }

    OS_MutSemGive(CFE_ES_Global.PerfDataMutex);
}
