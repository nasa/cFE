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
** File: cfe_tbl_internal.c
**
** Purpose:  cFE Table Services (TBL) utility function source file
**
** Author:   D. Kobe/the Hammers Company, Inc.
**
** Notes:
**
*/

/*
** Required header files...
*/
#include "cfe_tbl_module_all.h"

#include <stdio.h>
#include <string.h>

/**
 * Local/private structure used in conjunction with CFE_TBL_CheckInactiveBufferHelper
 */
typedef struct CFE_TBL_CheckInactiveBuffer
{
    int32          BufferIndex;
    CFE_ES_AppId_t LockingAppId;
} CFE_TBL_CheckInactiveBuffer_t;

/*----------------------------------------------------------------
 *
 * Local helper function, not invoked outside this unit
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
static void CFE_TBL_CheckInactiveBufferHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg)
{
    CFE_TBL_CheckInactiveBuffer_t *StatPtr = Arg;

    if (AccDescPtr->BufferIndex == StatPtr->BufferIndex && AccDescPtr->LockFlag)
    {
        StatPtr->LockingAppId = AccDescPtr->AppId;
    }
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_EarlyInit(void)
{
    uint16 i;
    uint32 j;
    int32  OsStatus;
    int32  Status;

    /* Clear task global */
    memset(&CFE_TBL_Global, 0, sizeof(CFE_TBL_Global));

    /* Initialize the Table Registry */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; i++)
    {
        CFE_TBL_InitRegistryRecord(&CFE_TBL_Global.Registry[i]);
    }

    /* Initialize the Table Access Descriptors nonzero values */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES; i++)
    {
        CFE_TBL_Global.Handles[i].AppId = CFE_TBL_NOT_OWNED;
        CFE_TBL_HandleLinkInit(&CFE_TBL_Global.Handles[i].Link);
    }

    /* Initialize the Dump-Only Table Dump Control Blocks nonzero values */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        /* Prevent Shared Buffers from being used until successfully allocated */
        CFE_TBL_Global.LoadBuffs[i].Taken = true;
    }

    CFE_TBL_Global.HkTlmTblRegIndex = CFE_TBL_NOT_FOUND;
    CFE_TBL_Global.LastTblUpdated   = CFE_TBL_NOT_FOUND;

    /*
    ** Create table registry access mutex
    */
    OsStatus = OS_MutSemCreate(&CFE_TBL_Global.RegistryMutex, CFE_TBL_MUT_REG_NAME, CFE_TBL_MUT_REG_VALUE);
    if (OsStatus != OS_SUCCESS)
    {
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /*
    ** Create working buffer access mutex
    */
    OsStatus = OS_MutSemCreate(&CFE_TBL_Global.WorkBufMutex, CFE_TBL_MUT_WORK_NAME, CFE_TBL_MUT_WORK_VALUE);
    if (OsStatus != OS_SUCCESS)
    {
        return CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    /* Initialize memory partition and allocate shared table buffers. */
    Status = CFE_ES_PoolCreate(&CFE_TBL_Global.Buf.PoolHdl, CFE_TBL_Global.Buf.Partition.Data,
                               sizeof(CFE_TBL_Global.Buf.Partition));

    if (Status < 0)
    {
        return Status;
    }
    else
    {
        /* Initialize each of the shared load buffers */
        j = 0;
        do
        {
            /* Allocate memory for shared load buffers */
            Status = CFE_ES_GetPoolBuf(&CFE_TBL_Global.LoadBuffs[j].BufferPtr, CFE_TBL_Global.Buf.PoolHdl,
                                       CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE);

            if (Status < CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE)
            {
                return Status;
            }
            else
            {
                /* The buffer is successfully created, so allow it to be used */
                CFE_TBL_Global.LoadBuffs[j].Taken = false;
            }

            j++;
        } while (j < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS);
    }

    /* Try to obtain a previous image of the Critical Table Registry from the Critical Data Store */
    Status = CFE_ES_RegisterCDSEx(&CFE_TBL_Global.CritRegHandle,
                                  (sizeof(CFE_TBL_CritRegRec_t) * CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES),
                                  "CFE_TBL.CritReg", true);

    /* Assume for the moment that nothing is already in the CDS and zero out the Critical Table Registry */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        CFE_TBL_Global.CritReg[i].CDSHandle = CFE_ES_CDS_BAD_HANDLE;
    }

    if (Status == CFE_ES_CDS_ALREADY_EXISTS)
    {
        /* Try to recover the Critical Table Registry from the CDS */
        Status = CFE_ES_RestoreFromCDS(CFE_TBL_Global.CritReg, CFE_TBL_Global.CritRegHandle);

        if (Status != CFE_SUCCESS)
        {
            /* Note if we were unable to recover error free Critical Table Registry from the CDS */
            CFE_ES_WriteToSysLog("%s: Failed to recover Critical Table Registry (Err=0x%08X)\n", __func__,
                                 (unsigned int)Status);
        }

        /* Whether we recovered the Critical Table Registry or not, we are successful with initialization */
        Status = CFE_SUCCESS;
    }
    else if (Status != CFE_SUCCESS)
    {
        /* Not being able to support Critical Tables is not the end of the world */
        /* Note the problem and move on */
        CFE_ES_WriteToSysLog("%s: Failed to create Critical Table Registry (Err=0x%08X)\n", __func__,
                             (unsigned int)Status);

        /* Failure to support critical tables is not a good enough reason to exit the cFE on start up */
        Status = CFE_SUCCESS;
    }
    else
    {
        /* Save the initial version of the Critical Table Registry in the CDS */
        Status = CFE_ES_CopyToCDS(CFE_TBL_Global.CritRegHandle, CFE_TBL_Global.CritReg);

        if (Status != CFE_SUCCESS)
        {
            /* Not being able to support Critical Tables is not the end of the world */
            /* Note the problem and move on */
            CFE_ES_WriteToSysLog("%s: Failed to save Critical Table Registry (Err=0x%08X)\n", __func__,
                                 (unsigned int)Status);

            /* Failure to support critical tables is not a good enough reason to exit the cFE on start up */
            Status = CFE_SUCCESS;
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_InitRegistryRecord(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    memset(RegRecPtr, 0, sizeof(*RegRecPtr));

    RegRecPtr->OwnerAppId         = CFE_TBL_NOT_OWNED;
    RegRecPtr->NotificationMsgId  = CFE_SB_INVALID_MSG_ID;
    RegRecPtr->LoadInProgress     = CFE_TBL_NO_LOAD_IN_PROGRESS;
    RegRecPtr->ValidateActiveId   = CFE_TBL_NO_VALIDATION_PENDING;
    RegRecPtr->ValidateInactiveId = CFE_TBL_NO_VALIDATION_PENDING;
    RegRecPtr->CDSHandle          = CFE_ES_CDS_BAD_HANDLE;
    RegRecPtr->DumpControlId      = CFE_TBL_NO_DUMP_PENDING;

    CFE_TBL_HandleLinkInit(&RegRecPtr->AccessList);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int16 CFE_TBL_FindTableInRegistry(const char *TblName)
{
    CFE_TBL_TxnState_t Txn;

    /* Note: there is no way for transaction setup to fail when passing false for context check */
    CFE_TBL_TxnInit(&Txn, false);
    CFE_TBL_TxnFindRegByName(&Txn, TblName);
    CFE_TBL_TxnFinish(&Txn);

    /* The transaction mechanism will set this to CFE_TBL_NOT_FOUND if anything failed */
    return CFE_TBL_TxnRegId(&Txn);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_FormTableName(char *FullTblName, const char *TblName, CFE_ES_AppId_t ThisAppId)
{
    char AppName[OS_MAX_API_NAME];

    CFE_ES_GetAppName(AppName, ThisAppId, sizeof(AppName));

    /* Ensure that AppName is null terminated */
    AppName[OS_MAX_API_NAME - 1] = '\0';

    /* Complete formation of application specific table name */
    sprintf(FullTblName, "%s.%s", AppName, TblName);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_LockRegistry(void)
{
    int32 OsStatus;
    int32 Status;

    OsStatus = OS_MutSemTake(CFE_TBL_Global.RegistryMutex);

    if (OsStatus == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_UnlockRegistry(void)
{
    int32 OsStatus;
    int32 Status;

    OsStatus = OS_MutSemGive(CFE_TBL_Global.RegistryMutex);

    if (OsStatus == OS_SUCCESS)
    {
        Status = CFE_SUCCESS;
    }
    else
    {
        Status = CFE_STATUS_EXTERNAL_RESOURCE_FAIL;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetActiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    int32 BufferIndex;

    /*
     * This should be simpler because ActiveBufferIndex always refers to
     * the active buffer, and applies to both single and double buffered
     * (That is, it is always 0 on a single-buffered table).
     *
     * However, legacy code always checked the double buffer flag before
     * using ActiveBufferIndex so this will to (at least for now)
     */
    if (RegRecPtr->DoubleBuffered)
    {
        BufferIndex = RegRecPtr->ActiveBufferIndex;
    }
    else
    {
        BufferIndex = 0;
    }

    return &RegRecPtr->Buffers[BufferIndex];
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_GetNextLocalBufferId(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* This implements a flip-flop buffer: if active is 1, return 0 and vice versa */
    return (1 - (RegRecPtr->ActiveBufferIndex & 1));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetInactiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    int32               BufferIndex;
    CFE_TBL_LoadBuff_t *Result;

    if (RegRecPtr->DoubleBuffered)
    {
        /* Determine the index of the Inactive Buffer Pointer */
        BufferIndex = CFE_TBL_GetNextLocalBufferId(RegRecPtr);

        /* then return the pointer to it */
        Result = &RegRecPtr->Buffers[BufferIndex];
    }
    else if (!RegRecPtr->UserDefAddr && RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
    {
        /*
         * The only time a single buffered table has an inactive buffer is when its loading, and
         * this always refers to a shared load buffer
         */
        Result = &CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress];
    }
    else
    {
        /* Tables with a user-defined address never have an inactive buffer */
        Result = NULL;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetSelectedBuffer(CFE_TBL_RegistryRec_t *     RegRecPtr,
                                              CFE_TBL_BufferSelect_Enum_t BufferSelect)
{
    CFE_TBL_LoadBuff_t *Result;

    switch (BufferSelect)
    {
        case CFE_TBL_BufferSelect_INACTIVE:
            Result = CFE_TBL_GetInactiveBuffer(RegRecPtr);
            break;
        case CFE_TBL_BufferSelect_ACTIVE:
            Result = CFE_TBL_GetActiveBuffer(RegRecPtr);
            break;
        default:
            CFE_EVS_SendEvent(CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Cmd for Table '%s' had illegal buffer parameter (0x%08X)", RegRecPtr->Name,
                              (unsigned int)BufferSelect);

            Result = NULL;
            break;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_GetWorkingBuffer(CFE_TBL_LoadBuff_t **WorkingBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                               bool CalledByApp)
{
    int32                         Status = CFE_SUCCESS;
    int32                         OsStatus;
    int32                         i;
    CFE_TBL_CheckInactiveBuffer_t CheckStat;

    /* Initialize return pointer to NULL */
    *WorkingBufferPtr = NULL;

    /* If a load is already in progress, return the previously allocated working buffer */
    if (RegRecPtr->LoadInProgress != CFE_TBL_NO_LOAD_IN_PROGRESS)
    {
        if (RegRecPtr->DoubleBuffered)
        {
            *WorkingBufferPtr = &RegRecPtr->Buffers[(1U - RegRecPtr->ActiveBufferIndex)];
        }
        else
        {
            *WorkingBufferPtr = &CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress];
        }
    }
    else
    {
        /* If the table is uninitialized and the function is called by an application (rather than       */
        /* by the Table Services application), then use the current active buffer as the working buffer. */
        /* This allows many tasks with many tables to perform the initialization without conflict        */
        /* over the accessibility of the shared working buffers.                                         */
        if ((RegRecPtr->TableLoadedOnce == false) && (CalledByApp == true))
        {
            if (RegRecPtr->DoubleBuffered)
            {
                *WorkingBufferPtr = &RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex];
            }
            else
            {
                *WorkingBufferPtr = &RegRecPtr->Buffers[0];
            }
        }
        else
        {
            /* If the table is a double buffered table, then check to make sure the */
            /* inactive buffer has been freed by any Applications that may have been using it */
            if (RegRecPtr->DoubleBuffered)
            {
                memset(&CheckStat, 0, sizeof(CheckStat));

                /* Determine the index of the Inactive Buffer Pointer */
                CheckStat.BufferIndex = 1 - RegRecPtr->ActiveBufferIndex;

                /* Scan the access descriptor table to determine if anyone is still using the inactive buffer */
                CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_CheckInactiveBufferHelper, &CheckStat);

                if (CFE_RESOURCEID_TEST_DEFINED(CheckStat.LockingAppId))
                {
                    Status = CFE_TBL_ERR_NO_BUFFER_AVAIL;

                    CFE_ES_WriteToSysLog("%s: Inactive Dbl Buff Locked for '%s' by AppId=%lu\n", __func__,
                                         RegRecPtr->Name, CFE_RESOURCEID_TO_ULONG(CheckStat.LockingAppId));
                }

                /* If buffer is free, then return the pointer to it */
                if (Status == CFE_SUCCESS)
                {
                    *WorkingBufferPtr         = &RegRecPtr->Buffers[CheckStat.BufferIndex];
                    RegRecPtr->LoadInProgress = CheckStat.BufferIndex;
                }
            }
            else /* Single Buffered Table */
            {
                /* Take Mutex to make sure we are not trying to grab a working buffer that some */
                /* other application is also trying to grab. */
                OsStatus = OS_MutSemTake(CFE_TBL_Global.WorkBufMutex);

                /* Make note of any errors but continue and hope for the best */
                if (OsStatus != OS_SUCCESS)
                {
                    CFE_ES_WriteToSysLog("%s: Internal error taking WorkBuf Mutex (Status=%ld)\n", __func__,
                                         (long)OsStatus);
                }

                /* Determine if there are any common buffers available */
                i = 0;
                while ((i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS) && (CFE_TBL_Global.LoadBuffs[i].Taken == true))
                {
                    i++;
                }

                /* If a free buffer was found, then return the address to the associated shared buffer */
                if (i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS)
                {
                    CFE_TBL_Global.LoadBuffs[i].Taken = true;
                    *WorkingBufferPtr                 = &CFE_TBL_Global.LoadBuffs[i];
                    RegRecPtr->LoadInProgress         = i;

                    /* Translate OS_SUCCESS into CFE_SUCCESS */
                    Status = CFE_SUCCESS;
                }
                else
                {
                    Status = CFE_TBL_ERR_NO_BUFFER_AVAIL;

                    CFE_ES_WriteToSysLog("%s: All shared buffers are locked\n", __func__);
                }

                /* Allow others to obtain a shared working buffer */
                OS_MutSemGive(CFE_TBL_Global.WorkBufMutex);
            }

            if ((*WorkingBufferPtr) != NULL &&
                (*WorkingBufferPtr)->BufferPtr != RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr)
            {
                /* In case the file contains a partial table load, get the active buffer contents first */
                memcpy((*WorkingBufferPtr)->BufferPtr, RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr,
                       RegRecPtr->Size);
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_LoadFromFile(const char *AppName, CFE_TBL_LoadBuff_t *WorkingBufferPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                           const char *Filename)
{
    int32              Status = CFE_SUCCESS;
    int32              OsStatus;
    CFE_FS_Header_t    StdFileHeader;
    CFE_TBL_File_Hdr_t TblFileHeader;
    osal_id_t          FileDescriptor = OS_OBJECT_ID_UNDEFINED;
    size_t             FilenameLen    = strlen(Filename);
    uint32             NumBytes;
    uint8              ExtraByte;

    if (FilenameLen > (OS_MAX_PATH_LEN - 1))
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_FILENAME_LONG_ERR_EID, CFE_EVS_EventType_ERROR,
                                   CFE_TBL_Global.TableTaskAppId, "%s: Filename is too long ('%s' (%lu) > %lu)",
                                   AppName, Filename, (long unsigned int)FilenameLen,
                                   (long unsigned int)OS_MAX_PATH_LEN - 1);

        return CFE_TBL_ERR_FILENAME_TOO_LONG;
    }

    /* Try to open the specified table file */
    OsStatus = OS_OpenCreate(&FileDescriptor, Filename, OS_FILE_FLAG_NONE, OS_READ_ONLY);

    if (OsStatus != OS_SUCCESS)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_ACCESS_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "%s: Unable to open file (Status=%ld)", AppName, (long)OsStatus);

        return CFE_TBL_ERR_ACCESS;
    }

    Status = CFE_TBL_ReadHeaders(FileDescriptor, &StdFileHeader, &TblFileHeader, Filename);

    if (Status != CFE_SUCCESS)
    {
        /* CFE_TBL_ReadHeaders() generates its own events */

        OS_close(FileDescriptor);
        return Status;
    }

    /* Verify that the specified file has compatible data for specified table */
    if (strcmp(RegRecPtr->Name, TblFileHeader.TableName) != 0)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID, CFE_EVS_EventType_ERROR,
                                   CFE_TBL_Global.TableTaskAppId, "%s: Table name mismatch (exp=%s, tblfilhdr=%s)",
                                   AppName, RegRecPtr->Name, TblFileHeader.TableName);

        OS_close(FileDescriptor);
        return CFE_TBL_ERR_FILE_FOR_WRONG_TABLE;
    }

    if ((TblFileHeader.Offset + TblFileHeader.NumBytes) > RegRecPtr->Size)
    {
        CFE_EVS_SendEventWithAppID(
            CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
            "%s: File reports size larger than expected (file=%lu, exp=%lu)", AppName,
            (long unsigned int)(TblFileHeader.Offset + TblFileHeader.NumBytes), (long unsigned int)RegRecPtr->Size);

        OS_close(FileDescriptor);
        return CFE_TBL_ERR_FILE_TOO_LARGE;
    }

    /* Any Table load that starts beyond the first byte is a "partial load" */
    /* But a file that starts with the first byte and ends before filling   */
    /* the whole table is just considered "short".                          */
    if (TblFileHeader.Offset > 0)
    {
        Status = CFE_TBL_WARN_PARTIAL_LOAD;
    }
    else if (TblFileHeader.NumBytes < RegRecPtr->Size)
    {
        Status = CFE_TBL_WARN_SHORT_FILE;
    }

    OsStatus =
        OS_read(FileDescriptor, ((uint8 *)WorkingBufferPtr->BufferPtr) + TblFileHeader.Offset, TblFileHeader.NumBytes);
    if (OsStatus >= OS_SUCCESS)
    {
        NumBytes = OsStatus; /* status code conversion (size) */
    }
    else
    {
        NumBytes = 0;
    }

    if (NumBytes != TblFileHeader.NumBytes)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_INCOMPLETE_ERR_EID, CFE_EVS_EventType_ERROR,
                                   CFE_TBL_Global.TableTaskAppId, "%s: File load incomplete (exp=%lu, read=%lu)",
                                   AppName, (long unsigned int)TblFileHeader.NumBytes, (long unsigned int)NumBytes);

        OS_close(FileDescriptor);
        return CFE_TBL_ERR_LOAD_INCOMPLETE;
    }

    /* Check to see if the file is too large (ie - more data than header claims) */
    OsStatus = OS_read(FileDescriptor, &ExtraByte, 1);
    if (OsStatus >= OS_SUCCESS)
    {
        NumBytes = OsStatus; /* status code conversion (size) */
    }
    else
    {
        NumBytes = 0;
    }

    /* If successfully read another byte, then file must have too much data */
    if (NumBytes == 1)
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_TOO_BIG_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "%s: File load too long (file length > %lu)", AppName,
                                   (long unsigned int)TblFileHeader.NumBytes);

        OS_close(FileDescriptor);
        return CFE_TBL_ERR_FILE_TOO_LARGE;
    }

    strncpy(WorkingBufferPtr->DataSource, Filename, sizeof(WorkingBufferPtr->DataSource) - 1);
    WorkingBufferPtr->DataSource[sizeof(WorkingBufferPtr->DataSource) - 1] = '\0';

    /* Save file creation time for later storage into Registry */
    WorkingBufferPtr->FileTime.Seconds    = StdFileHeader.TimeSeconds;
    WorkingBufferPtr->FileTime.Subseconds = StdFileHeader.TimeSubSeconds;

    /* Compute the CRC on the specified table buffer */
    WorkingBufferPtr->Crc =
        CFE_ES_CalculateCRC(WorkingBufferPtr->BufferPtr, RegRecPtr->Size, 0, CFE_MISSION_ES_DEFAULT_CRC);

    OS_close(FileDescriptor);

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function, not invoked outside this unit
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
static void CFE_TBL_CheckLockHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg)
{
    bool *LockStatus = Arg;

    if (AccDescPtr->LockFlag)
    {
        *LockStatus = true;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_UpdateInternal(CFE_TBL_Handle_t TblHandle, CFE_TBL_RegistryRec_t *RegRecPtr,
                             CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    int32 Status     = CFE_SUCCESS;
    bool  LockStatus = false;

    if ((!RegRecPtr->LoadPending) || (RegRecPtr->LoadInProgress == CFE_TBL_NO_LOAD_IN_PROGRESS))
    {
        /* Question: Should calling CFE_TBL_Update on a table with no load pending */
        /* be considered an error?  Currently assuming it is not an error.         */
        Status = CFE_TBL_INFO_NO_UPDATE_PENDING;
    }
    else
    {
        if (RegRecPtr->DoubleBuffered)
        {
            /* To update a double buffered table only requires a pointer swap */
            RegRecPtr->ActiveBufferIndex = (uint8)RegRecPtr->LoadInProgress;

            /* Source description in buffer should already have been updated by either */
            /* the LoadFromFile function or the Load function (when a memory load).    */
            /* However, we need to copy it into active registry area */
            strncpy(RegRecPtr->LastFileLoaded, RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].DataSource,
                    sizeof(RegRecPtr->LastFileLoaded) - 1);
            RegRecPtr->LastFileLoaded[sizeof(RegRecPtr->LastFileLoaded) - 1] = '\0';

            CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);

            /* If the table is a critical table, update the appropriate CDS with the new data */
            if (RegRecPtr->CriticalTable == true)
            {
                CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
            }
        }
        else
        {
            /* Check to see if the Table is locked by anyone */
            CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_CheckLockHelper, &LockStatus);

            if (LockStatus)
            {
                Status = CFE_TBL_INFO_TABLE_LOCKED;

                CFE_ES_WriteToSysLog("%s: Unable to update locked table Handle=%d\n", __func__, TblHandle);
            }
            else
            {
                /* To update a single buffered table requires a memcpy from working buffer */
                if (RegRecPtr->Buffers[0].BufferPtr != CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr)
                {
                    memcpy(RegRecPtr->Buffers[0].BufferPtr,
                           CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].BufferPtr, RegRecPtr->Size);
                }

                /* Save source description with active buffer */
                strncpy(RegRecPtr->Buffers[0].DataSource,
                        CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].DataSource,
                        sizeof(RegRecPtr->Buffers[0].DataSource) - 1);
                RegRecPtr->Buffers[0].DataSource[sizeof(RegRecPtr->Buffers[0].DataSource) - 1] = 0;
                strncpy(RegRecPtr->LastFileLoaded, CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].DataSource,
                        sizeof(RegRecPtr->LastFileLoaded) - 1);
                RegRecPtr->LastFileLoaded[sizeof(RegRecPtr->LastFileLoaded) - 1] = 0;

                /* Save the file creation time from the loaded file into the Table Registry */
                RegRecPtr->Buffers[0].FileTime = CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].FileTime;

                /* Save the previously computed CRC into the new buffer */
                RegRecPtr->Buffers[0].Crc = CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Crc;

                /* Free the working buffer */
                CFE_TBL_Global.LoadBuffs[RegRecPtr->LoadInProgress].Taken = false;

                CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);

                /* If the table is a critical table, update the appropriate CDS with the new data */
                if (RegRecPtr->CriticalTable == true)
                {
                    CFE_TBL_UpdateCriticalTblCDS(RegRecPtr);
                }
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Local helper function, not invoked outside this unit
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
static void CFE_TBL_SetUpdatedHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg)
{
    AccDescPtr->Updated = true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_NotifyTblUsersOfUpdate(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* Reset Load in Progress Values */
    RegRecPtr->LoadInProgress   = CFE_TBL_NO_LOAD_IN_PROGRESS;
    RegRecPtr->TimeOfLastUpdate = CFE_TIME_GetTime();

    /* Clear notification of pending load (as well as NO LOAD) and notify everyone of update */
    RegRecPtr->LoadPending     = false;
    RegRecPtr->TableLoadedOnce = true;

    CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_SetUpdatedHelper, NULL);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_ReadHeaders(osal_id_t FileDescriptor, CFE_FS_Header_t *StdFileHeaderPtr,
                          CFE_TBL_File_Hdr_t *TblFileHeaderPtr, const char *LoadFilename)
{
    int32 Status;
    int32 OsStatus;
    int32 EndianCheck = 0x01020304;

#if (CFE_PLATFORM_TBL_VALID_SCID_COUNT > 0)
    static uint32 ListSC[2] = {CFE_PLATFORM_TBL_VALID_SCID_1, CFE_PLATFORM_TBL_VALID_SCID_2};
    uint32        IndexSC;
#endif

#if (CFE_PLATFORM_TBL_VALID_PRID_COUNT > 0)
    static uint32 ListPR[4] = {CFE_PLATFORM_TBL_VALID_PRID_1, CFE_PLATFORM_TBL_VALID_PRID_2,
                               CFE_PLATFORM_TBL_VALID_PRID_3, CFE_PLATFORM_TBL_VALID_PRID_4};
    uint32        IndexPR;
#endif

    /* Once the file is open, read the headers to determine the target Table */
    Status = CFE_FS_ReadHeader(StdFileHeaderPtr, FileDescriptor);

    /* Verify successful read of standard cFE File Header */
    if (Status != sizeof(CFE_FS_Header_t))
    {
        CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_STD_HDR_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                                   "Unable to read std header for '%s', Status = 0x%08X", LoadFilename,
                                   (unsigned int)Status);

        Status = CFE_TBL_ERR_NO_STD_HEADER;
    }
    else
    {
        /* Verify the file type is a cFE compatible file */
        if (StdFileHeaderPtr->ContentType != CFE_FS_FILE_CONTENT_ID)
        {
            CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_TYPE_ERR_EID, CFE_EVS_EventType_ERROR,
                                       CFE_TBL_Global.TableTaskAppId,
                                       "File '%s' is not a cFE file type, ContentType = 0x%08X", LoadFilename,
                                       (unsigned int)StdFileHeaderPtr->ContentType);

            Status = CFE_TBL_ERR_BAD_CONTENT_ID;
        }
        else
        {
            /* Verify the SubType to ensure that it is a Table Image File */
            if (StdFileHeaderPtr->SubType != CFE_FS_SubType_TBL_IMG)
            {
                CFE_EVS_SendEventWithAppID(CFE_TBL_FILE_SUBTYPE_ERR_EID, CFE_EVS_EventType_ERROR,
                                           CFE_TBL_Global.TableTaskAppId,
                                           "File subtype for '%s' is wrong. Subtype = 0x%08X", LoadFilename,
                                           (unsigned int)StdFileHeaderPtr->SubType);

                Status = CFE_TBL_ERR_BAD_SUBTYPE_ID;
            }
            else
            {
                OsStatus = OS_read(FileDescriptor, TblFileHeaderPtr, sizeof(CFE_TBL_File_Hdr_t));

                /* Verify successful read of cFE Table File Header */
                if (OsStatus != sizeof(CFE_TBL_File_Hdr_t))
                {
                    CFE_EVS_SendEventWithAppID(
                        CFE_TBL_FILE_TBL_HDR_ERR_EID, CFE_EVS_EventType_ERROR, CFE_TBL_Global.TableTaskAppId,
                        "Unable to read tbl header for '%s', Status = %ld", LoadFilename, (long)OsStatus);

                    Status = CFE_TBL_ERR_NO_TBL_HEADER;
                }
                else
                {
                    /* All "required" checks have passed and we are pointing at the data */
                    Status = CFE_SUCCESS;

                    /* cppcheck-suppress knownConditionTrueFalse */
                    if ((*(char *)&EndianCheck) == 0x04)
                    {
                        /* If this is a little endian processor, then the standard cFE Table Header,   */
                        /* which is in big endian format, must be swapped so that the data is readable */
                        CFE_TBL_ByteSwapTblHeader(TblFileHeaderPtr);
                    }

                    /*
                     * Ensure termination of all local strings. These were read from a file, so they
                     * must be treated with appropriate care.  This could happen in case the file got
                     * damaged in transit or simply was not written properly to begin with.
                     *
                     * Since the "TblFileHeaderPtr" is a local buffer, this can be done directly.
                     */
                    TblFileHeaderPtr->TableName[sizeof(TblFileHeaderPtr->TableName) - 1] = '\0';

/* Verify Spacecraft ID contained in table file header [optional] */
#if (CFE_PLATFORM_TBL_VALID_SCID_COUNT > 0)
                    if (Status == CFE_SUCCESS)
                    {
                        Status = CFE_TBL_ERR_BAD_SPACECRAFT_ID;
                        for (IndexSC = 0; IndexSC < CFE_PLATFORM_TBL_VALID_SCID_COUNT; IndexSC++)
                        {
                            if (StdFileHeaderPtr->SpacecraftID == ListSC[IndexSC])
                            {
                                Status = CFE_SUCCESS;
                            }
                        }

                        if (Status == CFE_TBL_ERR_BAD_SPACECRAFT_ID)
                        {
                            CFE_EVS_SendEventWithAppID(CFE_TBL_SPACECRAFT_ID_ERR_EID, CFE_EVS_EventType_ERROR,
                                                       CFE_TBL_Global.TableTaskAppId,
                                                       "Unable to verify Spacecraft ID for '%s', ID = 0x%08X",
                                                       LoadFilename, (unsigned int)StdFileHeaderPtr->SpacecraftID);
                        }
                    }
#endif

/* Verify Processor ID contained in table file header [optional] */
#if (CFE_PLATFORM_TBL_VALID_PRID_COUNT > 0)
                    if (Status == CFE_SUCCESS)
                    {
                        Status = CFE_TBL_ERR_BAD_PROCESSOR_ID;
                        for (IndexPR = 0; IndexPR < CFE_PLATFORM_TBL_VALID_PRID_COUNT; IndexPR++)
                        {
                            if (StdFileHeaderPtr->ProcessorID == ListPR[IndexPR])
                            {
                                Status = CFE_SUCCESS;
                            }
                        }

                        if (Status == CFE_TBL_ERR_BAD_PROCESSOR_ID)
                        {
                            CFE_EVS_SendEventWithAppID(CFE_TBL_PROCESSOR_ID_ERR_EID, CFE_EVS_EventType_ERROR,
                                                       CFE_TBL_Global.TableTaskAppId,
                                                       "Unable to verify Processor ID for '%s', ID = 0x%08X",
                                                       LoadFilename, (unsigned int)StdFileHeaderPtr->ProcessorID);
                        }
                    }
#endif
                }
            }
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ByteSwapTblHeader(CFE_TBL_File_Hdr_t *HdrPtr)
{
    CFE_TBL_ByteSwapUint32(&HdrPtr->Reserved);
    CFE_TBL_ByteSwapUint32(&HdrPtr->Offset);
    CFE_TBL_ByteSwapUint32(&HdrPtr->NumBytes);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ByteSwapUint32(uint32 *Uint32ToSwapPtr)
{
    int32 Temp   = *Uint32ToSwapPtr;
    char *InPtr  = (char *)&Temp;
    char *OutPtr = (char *)Uint32ToSwapPtr;

    /* SAD: Safe access to InPtr[0-3] and OutPtr[0-3] as both manipulate bytes within 4-byte integers. */
    OutPtr[0] = InPtr[3];
    OutPtr[1] = InPtr[2];
    OutPtr[2] = InPtr[1];
    OutPtr[3] = InPtr[0];
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_CleanUpApp(CFE_ES_AppId_t AppId)
{
    uint32                 i;
    CFE_TBL_TxnState_t     Txn;
    CFE_TBL_DumpControl_t *DumpCtrlPtr;

    /* Scan Dump Requests to determine if any of the tables that */
    /* were to be dumped will be deleted */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS; i++)
    {
        DumpCtrlPtr = &CFE_TBL_Global.DumpControlBlocks[i];

        /* Check to see if the table to be dumped is owned by the App to be deleted */
        if (CFE_TBL_DumpCtrlBlockIsUsed(DumpCtrlPtr) &&
            CFE_RESOURCEID_TEST_EQUAL(DumpCtrlPtr->RegRecPtr->OwnerAppId, AppId))
        {
            /* If so, then remove the dump request */
            CFE_TBL_DumpCtrlBlockSetFree(DumpCtrlPtr);
        }
    }

    /* Scan Access Descriptors to determine if the Application had access to any tables */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_HANDLES; i++)
    {
        /* Check to see if the Handle belongs to the Application being deleted */
        if (CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.Handles[i].AppId, AppId) &&
            CFE_TBL_Global.Handles[i].UsedFlag == true)
        {
            CFE_TBL_TxnStartFromHandle(&Txn, i, CFE_TBL_TxnContext_UNDEFINED);

            /* Determine if the Application owned this particular table */
            if (CFE_RESOURCEID_TEST_EQUAL(Txn.RegRecPtr->OwnerAppId, AppId))
            {
                /* Mark table as free, although, technically, it isn't free until the */
                /* linked list of Access Descriptors has no links in it.              */
                /* NOTE: Allocated memory is freed when all Access Links have been    */
                /*       removed.  This allows Applications to continue to use the    */
                /*       data until they acknowledge that the table has been removed. */
                Txn.RegRecPtr->OwnerAppId = CFE_TBL_NOT_OWNED;

                /* Remove Table Name */
                Txn.RegRecPtr->Name[0] = '\0';
            }

            /* Remove the Access Descriptor Link from linked list */
            /* NOTE: If this removes the last access link, then   */
            /*       memory buffers are set free as well.         */
            CFE_TBL_TxnRemoveAccessLink(&Txn);

            CFE_TBL_Global.Handles[i].AppId = CFE_TBL_NOT_OWNED;

            CFE_TBL_TxnFinish(&Txn);
        }
    }

    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_FindCriticalTblInfo(CFE_TBL_CritRegRec_t **CritRegRecPtr, CFE_ES_CDSHandle_t CDSHandleToFind)
{
    uint32 i;

    /* Assume the record is never found */
    *CritRegRecPtr = NULL;

    for (i = 0; i < CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES; i++)
    {
        if (CFE_RESOURCEID_TEST_EQUAL(CFE_TBL_Global.CritReg[i].CDSHandle, CDSHandleToFind))
        {
            *CritRegRecPtr = &CFE_TBL_Global.CritReg[i];
            break;
        }
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_UpdateCriticalTblCDS(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_CritRegRec_t *CritRegRecPtr = NULL;

    int32 Status;

    /* Copy an image of the updated table to the CDS for safekeeping */
    Status = CFE_ES_CopyToCDS(RegRecPtr->CDSHandle, RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].BufferPtr);

    if (Status != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("%s: Unable to update Critical Table '%s' in CDS (Err=0x%08X)\n", __func__,
                             RegRecPtr->Name, (unsigned int)Status);
    }
    else
    {
        /* Locate entry in Critical Table Registry */
        CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, RegRecPtr->CDSHandle);
        if (CritRegRecPtr != NULL)
        {
            /* Save information related to the source of the data stored in the table in Critical Table Registry */
            CritRegRecPtr->FileTime = RegRecPtr->Buffers[RegRecPtr->ActiveBufferIndex].FileTime;
            strncpy(CritRegRecPtr->LastFileLoaded, RegRecPtr->LastFileLoaded,
                    sizeof(CritRegRecPtr->LastFileLoaded) - 1);
            CritRegRecPtr->LastFileLoaded[sizeof(CritRegRecPtr->LastFileLoaded) - 1] = '\0';
            CritRegRecPtr->TimeOfLastUpdate                                          = RegRecPtr->TimeOfLastUpdate;
            CritRegRecPtr->TableLoadedOnce                                           = RegRecPtr->TableLoadedOnce;

            /* Update copy of Critical Table Registry in the CDS */
            Status = CFE_ES_CopyToCDS(CFE_TBL_Global.CritRegHandle, CFE_TBL_Global.CritReg);

            if (Status != CFE_SUCCESS)
            {
                CFE_ES_WriteToSysLog("%s: Unable to update Critical Table Registry in CDS (Err=0x%08X)\n", __func__,
                                     (unsigned int)Status);
            }
        }
        else
        {
            CFE_ES_WriteToSysLog("%s: Error finding '%s' in Critical Table Registry\n", __func__, RegRecPtr->Name);
        }
    }

    /* Don't bother notifying the caller of the problem since the active table is still legitimate */
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_TBL_SendNotificationMsg(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    int32 Status = CFE_SUCCESS;

    /* First, determine if a message should be sent */
    if (RegRecPtr->NotifyByMsg)
    {
        /* Set the message ID */
        CFE_MSG_SetMsgId(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader), RegRecPtr->NotificationMsgId);

        /* Set the command code */
        CFE_MSG_SetFcnCode(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader), RegRecPtr->NotificationCC);

        /* Set the command parameter */
        CFE_TBL_Global.NotifyMsg.Payload.Parameter = RegRecPtr->NotificationParam;

        CFE_SB_TimeStampMsg(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader));
        Status = CFE_SB_TransmitMsg(CFE_MSG_PTR(CFE_TBL_Global.NotifyMsg.CommandHeader), true);

        if (Status != CFE_SUCCESS)
        {
            CFE_EVS_SendEvent(CFE_TBL_FAIL_NOTIFY_SEND_ERR_EID, CFE_EVS_EventType_ERROR,
                              "Manage Notification Pkt Error(Status=0x%08X)", (unsigned int)Status);
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateTableName(const char *Name)
{
    CFE_Status_t Status     = CFE_SUCCESS;
    size_t       NameLength = strlen(Name);
    char         TempTblName[CFE_TBL_MAX_FULL_NAME_LEN];

    /* Make sure the specified table name is not too long or too short */
    if (NameLength > CFE_MISSION_TBL_MAX_NAME_LENGTH || NameLength == 0)
    {
        Status = CFE_TBL_ERR_INVALID_NAME;

        /* Perform a buffer overrun safe copy of name for debug log message */
        strncpy(TempTblName, Name, sizeof(TempTblName) - 1);
        TempTblName[sizeof(TempTblName) - 1] = '\0';
        CFE_ES_WriteToSysLog("%s: Table Name (%s) is bad length (%d)", __func__, TempTblName, (int)NameLength);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateTableSize(const char *Name, size_t Size, uint16 TblOptionFlags)
{
    CFE_Status_t Status;
    size_t       SizeLimit;

    /* Single-buffered tables are allowed to be up to CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE   */
    /* Double-buffered tables are allowed to be up to CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE    */
    if ((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_DBL_BUFFER)
    {
        SizeLimit = CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE;
    }
    else
    {
        SizeLimit = CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE;
    }

    /* Check if the specified table size is zero, or above the maximum allowed               */
    if (Size == 0 || Size > SizeLimit)
    {
        Status = CFE_TBL_ERR_INVALID_SIZE;

        CFE_ES_WriteToSysLog("%s: Table '%s' has invalid size (%d)\n", __func__, Name, (int)Size);
    }
    else
    {
        Status = CFE_SUCCESS;
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_ValidateTableOptions(const char *Name, uint16 TblOptionFlags)
{
    CFE_Status_t Status = CFE_SUCCESS;

    /* User-defined table addresses are only legal for single-buffered, dump-only, non-critical tables */
    if ((TblOptionFlags & CFE_TBL_OPT_USR_DEF_MSK) == (CFE_TBL_OPT_USR_DEF_ADDR & CFE_TBL_OPT_USR_DEF_MSK))
    {
        if (((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_DBL_BUFFER) ||
            ((TblOptionFlags & CFE_TBL_OPT_LD_DMP_MSK) == CFE_TBL_OPT_LOAD_DUMP) ||
            ((TblOptionFlags & CFE_TBL_OPT_CRITICAL_MSK) == CFE_TBL_OPT_CRITICAL))
        {
            Status = CFE_TBL_ERR_INVALID_OPTIONS;

            CFE_ES_WriteToSysLog("%s: User Def tbl '%s' cannot be dbl buff, load/dump or critical\n", __func__, Name);
        }
    }
    else if ((TblOptionFlags & CFE_TBL_OPT_LD_DMP_MSK) == CFE_TBL_OPT_DUMP_ONLY)
    {
        /* Dump Only tables cannot be double-buffered, nor critical */
        if (((TblOptionFlags & CFE_TBL_OPT_BUFFER_MSK) == CFE_TBL_OPT_DBL_BUFFER) ||
            ((TblOptionFlags & CFE_TBL_OPT_CRITICAL_MSK) == CFE_TBL_OPT_CRITICAL))
        {
            Status = CFE_TBL_ERR_INVALID_OPTIONS;

            CFE_ES_WriteToSysLog("%s: Dump Only tbl '%s' cannot be double-buffered or critical\n", __func__, Name);
        }
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_AllocateTableBuffer(CFE_TBL_RegistryRec_t *RegRecPtr, size_t Size)
{
    CFE_Status_t Status;

    /* Allocate the memory buffer(s) for the table and inactive table, if necessary */
    Status = CFE_ES_GetPoolBuf(&RegRecPtr->Buffers[0].BufferPtr, CFE_TBL_Global.Buf.PoolHdl, Size);

    if (Status < 0)
    {
        CFE_ES_WriteToSysLog("%s: 1st Buf Alloc GetPool fail Stat=0x%08X MemPoolHndl=0x%08lX\n", __func__,
                             (unsigned int)Status, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.Buf.PoolHdl));
    }
    else
    {
        Status = CFE_SUCCESS;

        /* Zero the memory buffer */
        memset(RegRecPtr->Buffers[0].BufferPtr, 0x0, Size);
    }

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_AllocateSecondaryBuffer(CFE_TBL_RegistryRec_t *RegRecPtr, size_t Size)
{
    CFE_Status_t Status;

    /* Allocate memory for the dedicated secondary buffer */
    Status = CFE_ES_GetPoolBuf(&RegRecPtr->Buffers[1].BufferPtr, CFE_TBL_Global.Buf.PoolHdl, Size);

    if (Status < 0)
    {
        CFE_ES_WriteToSysLog("%s: 2nd Buf Alloc GetPool fail Stat=0x%08X MemPoolHndl=0x%08lX\n", __func__,
                             (unsigned int)Status, CFE_RESOURCEID_TO_ULONG(CFE_TBL_Global.Buf.PoolHdl));
    }
    else
    {
        Status = CFE_SUCCESS;

        /* Zero the dedicated secondary buffer */
        memset(RegRecPtr->Buffers[1].BufferPtr, 0x0, Size);
    }

    RegRecPtr->ActiveBufferIndex = 0;
    RegRecPtr->DoubleBuffered    = true;

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_InitTableRegistryEntry(CFE_TBL_RegistryRec_t *RegRecPtr, size_t Size,
                                    CFE_TBL_CallbackFuncPtr_t TblValidationFuncPtr, const char *TblName,
                                    uint16 TblOptionFlags)
{
    /* Save the size of the table */
    RegRecPtr->Size = Size;

    /* Save the Callback function pointer */
    RegRecPtr->ValidationFuncPtr = TblValidationFuncPtr;

    /* Save Table Name in Registry */
    strncpy(RegRecPtr->Name, TblName, sizeof(RegRecPtr->Name) - 1);
    RegRecPtr->Name[sizeof(RegRecPtr->Name) - 1] = '\0';

    /* Set the "Dump Only" flag to true/false based upon selected option */
    RegRecPtr->DumpOnly = ((TblOptionFlags & CFE_TBL_OPT_LD_DMP_MSK) == CFE_TBL_OPT_DUMP_ONLY);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_RestoreTableDataFromCDS(CFE_TBL_RegistryRec_t *RegRecPtr, const char *AppName, const char *Name,
                                             CFE_TBL_CritRegRec_t *CritRegRecPtr)
{
    CFE_Status_t        Status = CFE_SUCCESS;
    CFE_TBL_LoadBuff_t *WorkingBufferPtr;

    Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, true);

    if (Status != CFE_SUCCESS)
    {
        /* Unable to get a working buffer - this error is not really */
        /* possible at this point during table registration.  But we */
        /* do need to handle the error case because if the function */
        /* call did fail, WorkingBufferPtr would be a NULL pointer. */
        CFE_ES_WriteToSysLog("%s: Failed to get work buffer for '%s.%s' (ErrCode=0x%08X)\n", __func__, AppName, Name,
                             (unsigned int)Status);
    }
    else
    {
        /* CDS exists for this table - try to restore the data */
        Status = CFE_ES_RestoreFromCDS(WorkingBufferPtr->BufferPtr, RegRecPtr->CDSHandle);

        if (Status != CFE_SUCCESS)
        {
            CFE_ES_WriteToSysLog("%s: Failed to recover '%s.%s' from CDS (ErrCode=0x%08X)\n", __func__, AppName, Name,
                                 (unsigned int)Status);

            /*
             * Treat a restore from existing CDS error the same as
             * after a power-on reset (CDS was created but is empty)
             */
            Status = CFE_SUCCESS;
        }
        else
        {
            /* Table was fully restored from existing CDS... */
            /* Try to locate the associated information in the Critical Table Registry */
            CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, RegRecPtr->CDSHandle);

            if ((CritRegRecPtr != NULL) && (CritRegRecPtr->TableLoadedOnce))
            {
                strncpy(WorkingBufferPtr->DataSource, CritRegRecPtr->LastFileLoaded,
                        sizeof(WorkingBufferPtr->DataSource) - 1);
                WorkingBufferPtr->DataSource[sizeof(WorkingBufferPtr->DataSource) - 1] = '\0';

                WorkingBufferPtr->FileTime = CritRegRecPtr->FileTime;

                strncpy(RegRecPtr->LastFileLoaded, CritRegRecPtr->LastFileLoaded,
                        sizeof(RegRecPtr->LastFileLoaded) - 1);
                RegRecPtr->LastFileLoaded[sizeof(RegRecPtr->LastFileLoaded) - 1] = '\0';

                RegRecPtr->TimeOfLastUpdate = CritRegRecPtr->TimeOfLastUpdate;
                RegRecPtr->TableLoadedOnce  = CritRegRecPtr->TableLoadedOnce;

                /* Compute the CRC on the specified table buffer */
                WorkingBufferPtr->Crc =
                    CFE_ES_CalculateCRC(WorkingBufferPtr->BufferPtr, RegRecPtr->Size, 0, CFE_MISSION_ES_DEFAULT_CRC);

                /* Make sure everyone who sees the table knows that it has been updated */
                CFE_TBL_NotifyTblUsersOfUpdate(RegRecPtr);

                /* Make sure the caller realizes the contents have been initialized */
                Status = CFE_TBL_INFO_RECOVERED_TBL;
            }
            else
            {
                /* If an error occurred while trying to get the previous contents registry info, */
                /* Log the error in the System Log and pretend like we created a new CDS */
                CFE_ES_WriteToSysLog("%s: Failed to recover '%s.%s' info from CDS TblReg\n", __func__, AppName, Name);
                Status = CFE_SUCCESS;
            }
        }
    }

    /* Mark the table as critical for future reference */
    RegRecPtr->CriticalTable = true;

    return Status;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_RegisterWithCriticalTableRegistry(CFE_TBL_CritRegRec_t *CritRegRecPtr, CFE_TBL_RegistryRec_t *RegRecPtr,
                                               const char *TblName)
{
    /* Find and initialize a free entry in the Critical Table Registry */
    CFE_TBL_FindCriticalTblInfo(&CritRegRecPtr, CFE_ES_CDS_BAD_HANDLE);

    if (CritRegRecPtr != NULL)
    {
        CritRegRecPtr->CDSHandle = RegRecPtr->CDSHandle;
        strncpy(CritRegRecPtr->Name, TblName, sizeof(CritRegRecPtr->Name) - 1);
        CritRegRecPtr->Name[sizeof(CritRegRecPtr->Name) - 1] = '\0';
        CritRegRecPtr->LastFileLoaded[0]                     = '\0';
        CritRegRecPtr->TableLoadedOnce                       = false;

        CritRegRecPtr->FileTime         = CFE_TIME_ZERO_VALUE;
        CritRegRecPtr->TimeOfLastUpdate = CFE_TIME_ZERO_VALUE;

        CFE_ES_CopyToCDS(CFE_TBL_Global.CritRegHandle, CFE_TBL_Global.CritReg);
    }
    else
    {
        CFE_ES_WriteToSysLog("%s: Failed to find a free Crit Tbl Reg Rec for '%s'\n", __func__, RegRecPtr->Name);
    }

    /* Mark the table as critical for future reference */
    RegRecPtr->CriticalTable = true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
static inline CFE_TBL_AccessDescriptor_t *CFE_TBL_HandleListGetNext(const CFE_TBL_HandleLink_t *Link)
{
    return CFE_TBL_LocateAccessDescriptorByHandle(Link->Next);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
static inline CFE_TBL_AccessDescriptor_t *CFE_TBL_HandleListGetPrev(const CFE_TBL_HandleLink_t *Link)
{
    return CFE_TBL_LocateAccessDescriptorByHandle(Link->Prev);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleListGetSafeLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccDescPtr,
                                   CFE_TBL_HandleLink_t **PtrOut, CFE_TBL_Handle_t *HandleOut)
{
    if (AccDescPtr == NULL)
    {
        /* Instead of returning NULL, return a pointer to the head node linkage */
        *PtrOut    = &RegRecPtr->AccessList;
        *HandleOut = CFE_TBL_END_OF_LIST;
    }
    else
    {
        /* Return a pointer to this linkage */
        *PtrOut    = &AccDescPtr->Link;
        *HandleOut = CFE_TBL_AccessDescriptorGetHandle(AccDescPtr);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleListRemoveLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    CFE_TBL_HandleLink_t *LocalLink;
    CFE_TBL_HandleLink_t *LocalPrevPtr;
    CFE_TBL_HandleLink_t *LocalNextPtr;
    CFE_TBL_Handle_t      LocalHandle;
    CFE_TBL_Handle_t      PrevHandle;
    CFE_TBL_Handle_t      NextHandle;

    CFE_TBL_HandleListGetSafeLink(RegRecPtr, AccessDescPtr, &LocalLink, &LocalHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, CFE_TBL_HandleListGetNext(LocalLink), &LocalNextPtr, &NextHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, CFE_TBL_HandleListGetPrev(LocalLink), &LocalPrevPtr, &PrevHandle);

    LocalPrevPtr->Next = NextHandle;
    LocalNextPtr->Prev = PrevHandle;

    /* now that it is removed, reset the link */
    CFE_TBL_HandleLinkInit(LocalLink);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleListInsertLink(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescriptor_t *AccessDescPtr)
{
    CFE_TBL_HandleLink_t *LocalLink;
    CFE_TBL_HandleLink_t *LocalPrevPtr;
    CFE_TBL_HandleLink_t *LocalNextPtr;
    CFE_TBL_Handle_t      LocalHandle;
    CFE_TBL_Handle_t      PrevHandle;
    CFE_TBL_Handle_t      NextHandle;

    /* inserting at the front, so the "previous" will always be the head node (NULL) */
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, AccessDescPtr, &LocalLink, &LocalHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, NULL, &LocalPrevPtr, &PrevHandle);
    CFE_TBL_HandleListGetSafeLink(RegRecPtr, CFE_TBL_HandleListGetNext(LocalPrevPtr), &LocalNextPtr, &NextHandle);

    LocalLink->Next = NextHandle;
    LocalLink->Prev = PrevHandle;

    LocalPrevPtr->Next = LocalHandle;
    LocalNextPtr->Prev = LocalHandle;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_ForeachAccessDescriptor(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_TBL_AccessDescFunc_t Func, void *Arg)
{
    CFE_TBL_AccessDescriptor_t *AccDescPtr;
    const CFE_TBL_HandleLink_t *LinkPtr;

    LinkPtr = &RegRecPtr->AccessList;
    while (true)
    {
        AccDescPtr = CFE_TBL_HandleListGetNext(LinkPtr);

        if (AccDescPtr == NULL)
        {
            break;
        }

        Func(AccDescPtr, Arg);

        LinkPtr = &AccDescPtr->Link;
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleLinkInit(CFE_TBL_HandleLink_t *LinkPtr)
{
    LinkPtr->Prev = CFE_TBL_END_OF_LIST;
    LinkPtr->Next = CFE_TBL_END_OF_LIST;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_HandleLinkIsAttached(CFE_TBL_HandleLink_t *LinkPtr)
{
    return (LinkPtr->Next != CFE_TBL_END_OF_LIST);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * Intended to be used with CFE_TBL_ForeachAccessDescriptor()
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_CountAccessDescHelper(CFE_TBL_AccessDescriptor_t *AccDescPtr, void *Arg)
{
    uint32 *Count = Arg;

    ++(*Count);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_ValidationResult_t *CFE_TBL_CheckValidationRequest(CFE_TBL_ValidationResultId_t *ValIdPtr)
{
    CFE_TBL_ValidationResult_t * ResultPtr;
    CFE_TBL_ValidationResultId_t ValId;

    ValId = *ValIdPtr;

    /*
     * always clear the flag, regardless of "IsMatch" above.  If it was not a match,
     * that means the ID was stale, and it will never be a match (ie. it was aborted somehow)
     *
     * However, because this also acts as a flag, only write to the global if it was set to a value,
     * do not unconditionally write undefined value here.
     */
    if (CFE_RESOURCEID_TEST_DEFINED(ValId))
    {
        *ValIdPtr = CFE_TBL_VALRESULTID_UNDEFINED;

        ResultPtr = CFE_TBL_LocateValidationResultByID(ValId);
    }
    else
    {
        ResultPtr = NULL;
    }

    if (!CFE_TBL_ValidationResultIsMatch(ResultPtr, ValId))
    {
        ResultPtr = NULL;
    }

    return ResultPtr;
}
