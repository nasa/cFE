/************************************************************************
 * NASA Docket No. GSC-19,200-1, and identified as "cFS Draco"
 *
 * Copyright (c) 2023 United States Government as represented by the
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

/**
 * @file
 *
 * Implementation of table services registry record methods
 */

/*
** Includes
*/
#include "cfe_tbl_regrec.h"
#include "cfe_tbl_internal.h"
#include "cfe_core_resourceid_basevalues.h"

/**
 * Local/private structure used in conjunction with CFE_TBL_CheckInactiveBufferHelper
 */
typedef struct CFE_TBL_CheckInactiveBuffer
{
    CFE_TBL_LoadBuff_t *BufferPtr;
    CFE_ES_AppId_t      LockingAppId;
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
    CFE_TBL_LoadBuff_t *           AccBuffPtr;

    AccBuffPtr = CFE_TBL_LocateLoadBufferByID(AccDescPtr->BufferIndex);

    /* Check if it refers to this memory blob (even if the ID is different) and it is locked */
    if (AccBuffPtr == StatPtr->BufferPtr && AccDescPtr->LockFlag)
    {
        StatPtr->LockingAppId = AccDescPtr->AppId;
    }
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

    CFE_TBL_RegRecClearLoadInProgress(RegRecPtr);

    RegRecPtr->OwnerAppId         = CFE_TBL_NOT_OWNED;
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
CFE_ResourceId_t CFE_TBL_GetNextRegId(void)
{
    return CFE_ResourceId_FindNext(CFE_TBL_Global.LastRegId, CFE_PLATFORM_TBL_MAX_NUM_HANDLES,
                                   CFE_TBL_CheckRegistrySlotUsed);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_CheckRegistrySlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;

    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    RegRecPtr = CFE_TBL_LocateRegRecByID(CFE_TBL_REGID_C(CheckId));
    return (RegRecPtr == NULL || CFE_TBL_RegRecIsUsed(RegRecPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_Status_t CFE_TBL_RegId_ToIndex(CFE_TBL_RegId_t RegId, uint32 *Idx)
{
    return CFE_ResourceId_ToIndex(CFE_RESOURCEID_UNWRAP(RegId), CFE_TBL_REGID_BASE, CFE_PLATFORM_TBL_MAX_NUM_TABLES,
                                  Idx);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByName(const char *Name)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    uint32                 i;

    /*
    ** Search the Registry table for an app with a matching name.
    */
    for (i = 0; i < CFE_PLATFORM_TBL_MAX_NUM_TABLES; ++i)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[i];

        if (CFE_TBL_RegRecIsUsed(RegRecPtr) && strcmp(Name, CFE_TBL_RegRecGetName(RegRecPtr)) == 0)
        {
            break;
        }
    }

    if (i >= CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        RegRecPtr = NULL;
    }

    return RegRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_RegistryRec_t *CFE_TBL_LocateRegRecByID(CFE_TBL_RegId_t RegId)
{
    CFE_TBL_RegistryRec_t *RegRecPtr;
    uint32                 Idx;

    if (CFE_TBL_RegId_ToIndex(RegId, &Idx) == CFE_SUCCESS)
    {
        RegRecPtr = &CFE_TBL_Global.Registry[Idx];
    }
    else
    {
        RegRecPtr = NULL;
    }

    return RegRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetInactiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuff_t *BuffPtr;

    /* In all cases, if the NextBufferId is set and it checks out, use it */
    /* This applies to single and double buffered tables */
    BuffPtr = CFE_TBL_GetLoadInProgressBuffer(RegRecPtr);
    if (BuffPtr == NULL)
    {
        /* If this is a double buffered table, then get the previous buffer */
        BuffPtr = CFE_TBL_GetPreviousBuffer(RegRecPtr);
    }

    /* then return the pointer to it */
    return BuffPtr;
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
                              "Cmd for Table '%s' had illegal buffer parameter (0x%08X)",
                              CFE_TBL_RegRecGetName(RegRecPtr), (unsigned int)BufferSelect);

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
CFE_TBL_LoadBuff_t *CFE_TBL_GetLoadInProgressBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuffId_t BuffId;
    CFE_TBL_LoadBuff_t * LoadBuffPtr;

    BuffId      = CFE_TBL_RegRecGetLoadInProgress(RegRecPtr);
    LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(BuffId);
    if (!CFE_TBL_LoadBuffIsMatch(LoadBuffPtr, BuffId))
    {
        LoadBuffPtr = NULL;

        /*
         * This should not occur, but if the buffers are mis-managed somehow,
         * the reference could be stale.  The situation could occur if the load
         * was started but not activated (i.e. nobody called CFE_TBL_Update)
         * and was somehow the global load buffer was dropped.  Now the reference
         * in the local "load in progress" points to a shared/global load buff
         * that no longer exists.
         */
        if (CFE_TBL_LOADBUFFID_IS_VALID(BuffId))
        {
            /* This will clear the stale ref, restoring normalcy */
            CFE_TBL_RegRecClearLoadInProgress(RegRecPtr);

            /* but it implies something went awry with the load, so report it */
            CFE_ES_WriteToSysLog("WARNING: Cleared stale load in progress on table %s",
                                 CFE_TBL_RegRecGetName(RegRecPtr));
        }
    }

    return LoadBuffPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetInactiveBufferExclusive(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuff_t *          LoadBuffPtr;
    CFE_TBL_CheckInactiveBuffer_t CheckStat;
    CFE_ResourceId_t              PendingId;

    memset(&CheckStat, 0, sizeof(CheckStat));

    PendingId   = CFE_TBL_GetNextLocalBufferId(RegRecPtr);
    LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(CFE_TBL_LOADBUFFID_C(PendingId));

    /* If the load buffer already matches, it means its already reserved */
    if (LoadBuffPtr != NULL && !CFE_TBL_LOADBUFFID_EQ(LoadBuffPtr->LoadBufferId, CFE_TBL_LOADBUFFID_C(PendingId)))
    {
        /* If it does not match, then need to check if it can be reserved now */
        if (CFE_TBL_LoadBuffIsUsed(LoadBuffPtr))
        {
            /* Scan the access descriptor table to determine if anyone is still using the inactive buffer */
            CheckStat.BufferPtr = LoadBuffPtr;

            CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_CheckInactiveBufferHelper, &CheckStat);
        }

        if (CFE_RESOURCEID_TEST_DEFINED(CheckStat.LockingAppId))
        {
            LoadBuffPtr = NULL;
            CFE_ES_WriteToSysLog("%s: Inactive Buff Locked for '%s' by AppId=%lu\n", __func__,
                                 CFE_TBL_RegRecGetName(RegRecPtr), CFE_RESOURCEID_TO_ULONG(CheckStat.LockingAppId));
        }
        else
        {
            /* If buffer is free, then claim it */
            CFE_TBL_LoadBuffSetUsed(LoadBuffPtr, PendingId, CFE_TBL_RegRecGetID(RegRecPtr));
        }
    }

    return LoadBuffPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_RegRecResetLoadInfo(CFE_TBL_RegistryRec_t *RegRecPtr, const char *DataSource,
                                 CFE_TIME_SysTime_t UpdateTime)
{
    strncpy(RegRecPtr->Status.LastFileLoaded, DataSource, sizeof(RegRecPtr->Status.LastFileLoaded) - 1);
    RegRecPtr->Status.LastFileLoaded[sizeof(RegRecPtr->Status.LastFileLoaded) - 1] = 0;

    RegRecPtr->Status.TimeOfLastUpdate = UpdateTime;
    RegRecPtr->Status.IsModified       = false;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_RegRecSetModifiedFlag(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    RegRecPtr->Status.TimeOfLastUpdate = CFE_TIME_GetTime();
    RegRecPtr->Status.IsModified       = true;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_SetupTableRegistryRecord(CFE_TBL_RegistryRec_t *RegRecPtr, CFE_ES_AppId_t OwnerAppId,
                                      const CFE_TBL_TableConfig_t *ReqCfg)
{
    CFE_TBL_LoadBuff_t *InitialBuffPtr;

    /* Keep note of the app that registered this table */
    RegRecPtr->OwnerAppId = OwnerAppId;

    /* Save the Callback function pointer */
    RegRecPtr->Config.ValidationFuncPtr = ReqCfg->ValidationFuncPtr;

    /* Save Table Name in Registry (note that the string length was already validated) */
    strncpy(RegRecPtr->Config.Name, ReqCfg->Name, sizeof(RegRecPtr->Config.Name));

    /* Save the EDS ID */
    RegRecPtr->Config.EdsId = ReqCfg->EdsId;

    /* Set the initial active buffer.  This is only applicable if it is a dump only table
     * that is using an internal buffer - otherwise it needs to be loaded and the 
     * active buffer will be set at that time. */
    if (ReqCfg->DumpOnly && !ReqCfg->UserDefAddr)
    {
        InitialBuffPtr = CFE_TBL_GetInactiveBufferExclusive(RegRecPtr);
    }
    else
    {
        InitialBuffPtr = NULL;
    }

    if (InitialBuffPtr != NULL)
    {
        CFE_TBL_SetActiveBuffer(RegRecPtr, InitialBuffPtr);
    }
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_RegRecIsPendingActivation(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuff_t *LoadBuffPtr;
    bool                Result;

    Result = false;

    /* The next buffer index is set when there is a pending buffer */
    LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(RegRecPtr->Status.NextBufferId);
    if (CFE_TBL_LoadBuffIsMatch(LoadBuffPtr, RegRecPtr->Status.NextBufferId))
    {
        /* it is only pending activation if it is validated */
        Result = LoadBuffPtr->Validated;
    }

    return Result;
}
