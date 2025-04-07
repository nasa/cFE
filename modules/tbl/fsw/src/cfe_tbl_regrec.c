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

/**
 * @file
 *
 * Implementation of table services registry record methods
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
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
    CFE_TBL_LoadBuffId_t BufferIndex;
    CFE_ES_AppId_t       LockingAppId;
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

    if (CFE_TBL_LOADBUFFID_EQ(AccDescPtr->BufferIndex, StatPtr->BufferIndex) && AccDescPtr->LockFlag)
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
CFE_Status_t CFE_TBL_RegId_ToIndex(CFE_TBL_RegId_t RegId, uint32 *Idx)
{
    int32 IdAsInt;

    IdAsInt = CFE_TBL_REGID_INT(RegId);
    if (IdAsInt < 0 || IdAsInt >= CFE_PLATFORM_TBL_MAX_NUM_TABLES)
    {
        return CFE_TBL_ERR_INVALID_HANDLE;
    }

    *Idx = IdAsInt;

    return CFE_SUCCESS;
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
CFE_TBL_RegId_t CFE_TBL_RegRecGetID(const CFE_TBL_RegistryRec_t *RegRecPtr)
{
    /* The pointer should be to an entry within the Registry array */
    int32 IdAsInt;

    IdAsInt = (RegRecPtr - CFE_TBL_Global.Registry);

    return CFE_TBL_REGID_C(IdAsInt);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetActiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuffId_t BufferIndex;

    /*
     * This should be simpler because ActiveBufferIndex always refers to
     * the active buffer, and applies to both single and double buffered
     * (That is, it is always 0 on a single-buffered table).
     *
     * However, legacy code always checked the double buffer flag before
     * using ActiveBufferIndex so this will to (at least for now)
     */
    BufferIndex = RegRecPtr->Status.ActiveBufferIndex;

    return CFE_TBL_LocateLoadBufferByID(RegRecPtr, BufferIndex);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuffId_t CFE_TBL_GetNextLocalBufferId(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    int32 BufferId;

    /* This implements a flip-flop buffer: if active is 1, return 0 and vice versa */
    BufferId = CFE_TBL_LOADBUFFID_INT(RegRecPtr->Status.ActiveBufferIndex);

    BufferId = (BufferId + 1) % 2;

    return CFE_TBL_LOADBUFFID_C(BufferId);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_TBL_LoadBuff_t *CFE_TBL_GetInactiveBuffer(CFE_TBL_RegistryRec_t *RegRecPtr)
{
    CFE_TBL_LoadBuffId_t BufferId;
    CFE_TBL_LoadBuff_t * BuffPtr;

    if (CFE_TBL_RegRecGetConfig(RegRecPtr)->DoubleBuffered)
    {
        /* Determine the index of the Inactive Buffer Pointer */
        BufferId = CFE_TBL_GetNextLocalBufferId(RegRecPtr);
        BuffPtr  = CFE_TBL_LocateLoadBufferByID(RegRecPtr, BufferId);
    }
    else if (!CFE_TBL_RegRecGetConfig(RegRecPtr)->UserDefAddr && CFE_TBL_RegRecIsLoadInProgress(RegRecPtr))
    {
        /*
         * The only time a single buffered table has an inactive buffer is when its loading, and
         * this always refers to a shared load buffer
         */
        BufferId = CFE_TBL_RegRecGetLoadInProgress(RegRecPtr);
        BuffPtr  = CFE_TBL_LocateLoadBufferByID(NULL, BufferId);
    }
    else
    {
        /* Tables with a user-defined address never have an inactive buffer */
        BuffPtr = NULL;
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
    CFE_TBL_LoadBuff_t *LoadBuffPtr;

    /* If a load is already in progress, return the previously allocated working buffer */
    if (!CFE_TBL_RegRecIsLoadInProgress(RegRecPtr))
    {
        LoadBuffPtr = NULL;
    }
    else if (CFE_TBL_RegRecGetConfig(RegRecPtr)->DoubleBuffered)
    {
        /* Interpret the load in progress id as the local (inactive) buffer */
        LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(RegRecPtr, CFE_TBL_RegRecGetLoadInProgress(RegRecPtr));
    }
    else
    {
        /* Interpret the load in progress id as a shared buffer */
        LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(NULL, CFE_TBL_RegRecGetLoadInProgress(RegRecPtr));
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

    memset(&CheckStat, 0, sizeof(CheckStat));

    /* Determine the index of the Inactive Buffer Pointer */
    CheckStat.BufferIndex = CFE_TBL_GetNextLocalBufferId(RegRecPtr);

    /* Scan the access descriptor table to determine if anyone is still using the inactive buffer */
    CFE_TBL_ForeachAccessDescriptor(RegRecPtr, CFE_TBL_CheckInactiveBufferHelper, &CheckStat);

    if (CFE_RESOURCEID_TEST_DEFINED(CheckStat.LockingAppId))
    {
        LoadBuffPtr = NULL;
        CFE_ES_WriteToSysLog("%s: Inactive Dbl Buff Locked for '%s' by AppId=%lu\n", __func__,
                             CFE_TBL_RegRecGetName(RegRecPtr), CFE_RESOURCEID_TO_ULONG(CheckStat.LockingAppId));
    }
    else
    {
        /* If buffer is free, then return the pointer to it */
        LoadBuffPtr = CFE_TBL_LocateLoadBufferByID(RegRecPtr, CheckStat.BufferIndex);
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
    /* Keep note of the app that registered this table */
    RegRecPtr->OwnerAppId = OwnerAppId;

    /* Save the Callback function pointer */
    RegRecPtr->Config.ValidationFuncPtr = ReqCfg->ValidationFuncPtr;

    /* Save Table Name in Registry (note that the string length was already validated) */
    strncpy(RegRecPtr->Config.Name, ReqCfg->Name, sizeof(RegRecPtr->Config.Name));
}
