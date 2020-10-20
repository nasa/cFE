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
**  File:
**  cfe_es_resource.c
**
**  Purpose:
**      Function definitions related to CFE resource management
**
**  References:
**     Flight Software Branch C Coding Standard Version 1.0a
**     cFE Flight Software Application Developers Guide
*/

/*
** Includes
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "cfe_platform_cfg.h"
#include "cfe_es_resource.h"


/*********************************************************************/
/*
 * CFE_ES_ResourceID_ToIndex_Internal
 *
 * For complete API information, see prototype in header
 */
int32 CFE_ES_ResourceID_ToIndex_Internal(uint32 Serial, uint32 TableSize, uint32 *Idx)
{
    if (Idx == NULL)
    {
        return CFE_ES_ERR_BUFFER;
    }

    if (Serial > CFE_ES_RESOURCEID_MAX)
    {
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    *Idx = Serial % TableSize;
    return CFE_SUCCESS;
}


/*********************************************************************/
/*
 * CFE_ES_ResourceID_ToOSAL
 *
 * For complete API information, see prototype in header
 */
osal_id_t CFE_ES_ResourceID_ToOSAL(CFE_ES_ResourceID_t id)
{
    unsigned long val = CFE_ES_ResourceID_ToInteger(id);
    return OS_ObjectIdFromInteger(val);
}

/*********************************************************************/
/*
 * CFE_ES_ResourceID_FromOSAL
 *
 * For complete API information, see prototype in header
 */
CFE_ES_ResourceID_t CFE_ES_ResourceID_FromOSAL(osal_id_t id)
{
    unsigned long val = OS_ObjectIdToInteger(id);
    return CFE_ES_ResourceID_FromInteger(val);
}

/*********************************************************************/
/*
 * CFE_ES_FindNextAvailableId
 *
 * For complete API information, see prototype in header
 */
CFE_ES_ResourceID_t CFE_ES_FindNextAvailableId(CFE_ES_ResourceID_t StartId, uint32 TableSize)
{
    uint32 Serial;
    uint32 Count;
    uint32 ResourceType;
    CFE_ES_ResourceID_t CheckId;
    bool IsTaken;

    ResourceType = CFE_ES_ResourceID_ToInteger(StartId);
    Serial = ResourceType & CFE_ES_RESOURCEID_MAX;
    ResourceType -= Serial;
    Count = TableSize;
    IsTaken = true;

    do
    {
        if (Count == 0)
        {
            CheckId = CFE_ES_RESOURCEID_UNDEFINED;
            break;
        }

        --Count;
        ++Serial;
        if (Serial >= CFE_ES_RESOURCEID_MAX)
        {
            Serial %= TableSize;
        }
        CheckId = CFE_ES_ResourceID_FromInteger(ResourceType + Serial);

        switch (ResourceType)
        {
        case CFE_ES_APPID_BASE:
            IsTaken = CFE_ES_AppRecordIsUsed(CFE_ES_LocateAppRecordByID(CheckId));
            break;
        case CFE_ES_LIBID_BASE:
            IsTaken = CFE_ES_LibRecordIsUsed(CFE_ES_LocateLibRecordByID(CheckId));
            break;
        case CFE_ES_COUNTID_BASE:
            IsTaken = CFE_ES_CounterRecordIsUsed(CFE_ES_LocateCounterRecordByID(CheckId));
            break;
        case CFE_ES_POOLID_BASE:
            IsTaken = CFE_ES_MemPoolRecordIsUsed(CFE_ES_LocateMemPoolRecordByID(CheckId));
            break;
        case CFE_ES_CDSBLOCKID_BASE:
            IsTaken = CFE_ES_CDSBlockRecordIsUsed(CFE_ES_LocateCDSBlockRecordByID(CheckId));
            break;
        default:
            /* do nothing, should never happen */
            break;
        }
    }
    while (IsTaken);

    return CheckId;
}

/*********************************************************************/
/*
 * CFE_ES_LocateAppRecordByName
 *
 * For complete API information, see prototype in header
 */
CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByName(const char *Name)
{
   CFE_ES_AppRecord_t *AppRecPtr;
   uint32 Count;

   /*
   ** Search the Application table for an app with a matching name.
   */
   AppRecPtr = CFE_ES_Global.AppTable;
   Count = CFE_PLATFORM_ES_MAX_APPLICATIONS;
   while ( true )
   {
       if ( Count == 0 )
       {
           AppRecPtr = NULL;
           break;
       }
       if ( CFE_ES_AppRecordIsUsed(AppRecPtr) &&
               strcmp(Name, CFE_ES_AppRecordGetName(AppRecPtr)) == 0 )
       {
           break;
       }

      ++AppRecPtr;
      --Count;
   }

   return AppRecPtr;

} /* End of CFE_ES_LocateAppRecordByName() */

/*********************************************************************/
/*
 * CFE_ES_LocateLibRecordByName
 *
 * For complete API information, see prototype in header
 */
CFE_ES_LibRecord_t *CFE_ES_LocateLibRecordByName(const char *Name)
{
   CFE_ES_LibRecord_t *LibRecPtr;
   uint32 Count;

   /*
   ** Search the Library table for a library with a matching name.
   */
   LibRecPtr = CFE_ES_Global.LibTable;
   Count = CFE_PLATFORM_ES_MAX_LIBRARIES;
   while ( true )
   {
       if ( Count == 0 )
       {
           LibRecPtr = NULL;
           break;
       }
       if ( CFE_ES_LibRecordIsUsed(LibRecPtr) &&
               strcmp(Name, CFE_ES_LibRecordGetName(LibRecPtr)) == 0 )
       {
           break;
       }

      ++LibRecPtr;
      --Count;
   }

   return LibRecPtr;

} /* End of CFE_ES_LocateLibRecordByName() */


/*********************************************************************/
/*
 * CFE_ES_LocateCounterRecordByName
 *
 * For complete API information, see prototype in header
 */
CFE_ES_GenCounterRecord_t *CFE_ES_LocateCounterRecordByName(const char *Name)
{
    CFE_ES_GenCounterRecord_t *CounterRecPtr;
    uint32 Count;

    /*
    ** Search the Counter table for a matching name.
    */
    CounterRecPtr = CFE_ES_Global.CounterTable;
    Count = CFE_PLATFORM_ES_MAX_GEN_COUNTERS;
    while ( true )
    {
        if ( Count == 0 )
        {
            CounterRecPtr = NULL;
            break;
        }
        if ( CFE_ES_CounterRecordIsUsed(CounterRecPtr) &&
                strcmp(Name, CFE_ES_CounterRecordGetName(CounterRecPtr)) == 0 )
        {
            break;
        }

       ++CounterRecPtr;
       --Count;
    }

    return CounterRecPtr;
}


/*********************************************************************/
/*
 * CFE_ES_LocateAppRecordByID
 *
 * For complete API information, see prototype in header
 */
CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByID(CFE_ES_ResourceID_t AppID)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    uint32 Idx;

    if (CFE_ES_AppID_ToIndex(AppID, &Idx) == CFE_SUCCESS)
    {
        AppRecPtr = &CFE_ES_Global.AppTable[Idx];
    }
    else
    {
        AppRecPtr = NULL;
    }

    return AppRecPtr;
}

/*********************************************************************/
/*
 * CFE_ES_LocateLibRecordByID
 *
 * For complete API information, see prototype in header
 */
CFE_ES_LibRecord_t* CFE_ES_LocateLibRecordByID(CFE_ES_ResourceID_t LibID)
{
    CFE_ES_LibRecord_t *LibRecPtr;
    uint32 Idx;

    if (CFE_ES_LibID_ToIndex(LibID, &Idx) == CFE_SUCCESS)
    {
        LibRecPtr = &CFE_ES_Global.LibTable[Idx];
    }
    else
    {
        LibRecPtr = NULL;
    }

    return LibRecPtr;
}

/*********************************************************************/
/*
 * CFE_ES_LocateTaskRecordByID
 *
 * For complete API information, see prototype in header
 */
CFE_ES_TaskRecord_t *CFE_ES_LocateTaskRecordByID(CFE_ES_ResourceID_t TaskID)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    uint32 Idx;

    if (CFE_ES_TaskID_ToIndex(TaskID, &Idx) == CFE_SUCCESS)
    {
        TaskRecPtr = &CFE_ES_Global.TaskTable[Idx];
    }
    else
    {
        TaskRecPtr = NULL;
    }

    return TaskRecPtr;
}

/*********************************************************************/
/*
 * CFE_ES_LocateCounterRecordByID
 *
 * For complete API information, see prototype in header
 */
CFE_ES_GenCounterRecord_t* CFE_ES_LocateCounterRecordByID(CFE_ES_ResourceID_t CounterID)
{
    CFE_ES_GenCounterRecord_t *CounterRecPtr;
    uint32 Idx;

    if (CFE_ES_CounterID_ToIndex(CounterID, &Idx) == CFE_SUCCESS)
    {
        CounterRecPtr = &CFE_ES_Global.CounterTable[Idx];
    }
    else
    {
        CounterRecPtr = NULL;
    }

    return CounterRecPtr;
}

/*********************************************************************/
/*
 * CFE_ES_GetTaskRecordByContext
 *
 * For complete API information, see prototype in header
 *
 * This function does additional validation on the task record
 * and should only be called when global data is locked.
 */
CFE_ES_TaskRecord_t *CFE_ES_GetTaskRecordByContext(void)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    CFE_ES_ResourceID_t TaskID;

    /*
    ** Use the OS task ID to get the ES task record
    */
    TaskID = CFE_ES_ResourceID_FromOSAL(OS_TaskGetId());
    TaskRecPtr = CFE_ES_LocateTaskRecordByID(TaskID);

    /*
     * Confirm that the entry is actually a match (this requires/assumes
     * the global data is locked).
     *
     * If not a match, return NULL.
     */
    if (!CFE_ES_TaskRecordIsMatch(TaskRecPtr, TaskID))
    {
        TaskRecPtr = NULL;
    }

    return TaskRecPtr;
}

/*********************************************************************/
/*
 * CFE_ES_GetAppRecordByContext
 *
 * For complete API information, see prototype in header
 */
CFE_ES_AppRecord_t *CFE_ES_GetAppRecordByContext(void)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    CFE_ES_TaskRecord_t *TaskRecPtr;

    /*
    ** Step 1: Get the task record
    */
    TaskRecPtr = CFE_ES_GetTaskRecordByContext();
    if (TaskRecPtr != NULL)
    {
        /*
        ** Step 2: get the Application ID for the current task
        */
        AppRecPtr = CFE_ES_LocateAppRecordByID(TaskRecPtr->AppId);

        /*
         * Confirm that the entry is actually a match (this requires/assumes
         * the global data is locked).
         *
         * If not a match, return NULL.
         */
        if (!CFE_ES_AppRecordIsMatch(AppRecPtr, TaskRecPtr->AppId))
        {
            AppRecPtr = NULL;
        }
    }
    else
    {
        AppRecPtr = NULL;
    }

    return AppRecPtr;
}


