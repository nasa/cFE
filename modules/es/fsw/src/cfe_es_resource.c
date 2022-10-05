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
#include "cfe_es_module_all.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
osal_id_t CFE_ES_TaskId_ToOSAL(CFE_ES_TaskId_t id)
{
    osal_id_t     Result;
    unsigned long Val;

    Val    = CFE_ResourceId_ToInteger(CFE_RESOURCEID_UNWRAP(id));
    Result = OS_ObjectIdFromInteger(Val ^ CFE_RESOURCEID_MARK);

    return Result;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_TaskId_t CFE_ES_TaskId_FromOSAL(osal_id_t id)
{
    CFE_ResourceId_t Result;
    unsigned long    Val;

    Val    = OS_ObjectIdToInteger(id);
    Result = CFE_ResourceId_FromInteger(Val ^ CFE_RESOURCEID_MARK);

    return CFE_ES_TASKID_C(Result);
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByName(const char *Name)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    uint32              Count;

    /*
    ** Search the Application table for an app with a matching name.
    */
    AppRecPtr = CFE_ES_Global.AppTable;
    Count     = CFE_PLATFORM_ES_MAX_APPLICATIONS;
    while (true)
    {
        if (Count == 0)
        {
            AppRecPtr = NULL;
            break;
        }
        if (CFE_ES_AppRecordIsUsed(AppRecPtr) && strcmp(Name, CFE_ES_AppRecordGetName(AppRecPtr)) == 0)
        {
            break;
        }

        ++AppRecPtr;
        --Count;
    }

    return AppRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_LibRecord_t *CFE_ES_LocateLibRecordByName(const char *Name)
{
    CFE_ES_LibRecord_t *LibRecPtr;
    uint32              Count;

    /*
    ** Search the Library table for a library with a matching name.
    */
    LibRecPtr = CFE_ES_Global.LibTable;
    Count     = CFE_PLATFORM_ES_MAX_LIBRARIES;
    while (true)
    {
        if (Count == 0)
        {
            LibRecPtr = NULL;
            break;
        }
        if (CFE_ES_LibRecordIsUsed(LibRecPtr) && strcmp(Name, CFE_ES_LibRecordGetName(LibRecPtr)) == 0)
        {
            break;
        }

        ++LibRecPtr;
        --Count;
    }

    return LibRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_GenCounterRecord_t *CFE_ES_LocateCounterRecordByName(const char *Name)
{
    CFE_ES_GenCounterRecord_t *CounterRecPtr;
    uint32                     Count;

    /*
    ** Search the Counter table for a matching name.
    */
    CounterRecPtr = CFE_ES_Global.CounterTable;
    Count         = CFE_PLATFORM_ES_MAX_GEN_COUNTERS;
    while (true)
    {
        if (Count == 0)
        {
            CounterRecPtr = NULL;
            break;
        }
        if (CFE_ES_CounterRecordIsUsed(CounterRecPtr) && strcmp(Name, CFE_ES_CounterRecordGetName(CounterRecPtr)) == 0)
        {
            break;
        }

        ++CounterRecPtr;
        --Count;
    }

    return CounterRecPtr;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_AppRecord_t *CFE_ES_LocateAppRecordByID(CFE_ES_AppId_t AppID)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    uint32              Idx;

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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_LibRecord_t *CFE_ES_LocateLibRecordByID(CFE_ES_LibId_t LibID)
{
    CFE_ES_LibRecord_t *LibRecPtr;
    uint32              Idx;

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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_TaskRecord_t *CFE_ES_LocateTaskRecordByID(CFE_ES_TaskId_t TaskID)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    uint32               Idx;

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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_GenCounterRecord_t *CFE_ES_LocateCounterRecordByID(CFE_ES_CounterId_t CounterID)
{
    CFE_ES_GenCounterRecord_t *CounterRecPtr;
    uint32                     Idx;

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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_TaskRecord_t *CFE_ES_GetTaskRecordByContext(void)
{
    CFE_ES_TaskRecord_t *TaskRecPtr;
    CFE_ES_TaskId_t      TaskID;

    /*
    ** Use the OS task ID to get the ES task record
    */
    TaskID     = CFE_ES_TaskId_FromOSAL(OS_TaskGetId());
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ES_AppRecord_t *CFE_ES_GetAppRecordByContext(void)
{
    CFE_ES_AppRecord_t * AppRecPtr;
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

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_CheckCounterIdSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_ES_GenCounterRecord_t *GenCounterRecPtr;
    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    GenCounterRecPtr = CFE_ES_LocateCounterRecordByID(CFE_ES_COUNTERID_C(CheckId));
    return (GenCounterRecPtr == NULL || CFE_ES_CounterRecordIsUsed(GenCounterRecPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_CheckAppIdSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_ES_AppRecord_t *AppRecPtr;
    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    AppRecPtr = CFE_ES_LocateAppRecordByID(CFE_ES_APPID_C(CheckId));
    return (AppRecPtr == NULL || CFE_ES_AppRecordIsUsed(AppRecPtr));
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_ES_CheckLibIdSlotUsed(CFE_ResourceId_t CheckId)
{
    CFE_ES_LibRecord_t *LibRecPtr;
    /*
     * Note - The pointer here should never be NULL because the ID should always be
     * within the expected range, but if it ever is NULL, this should return true
     * such that the caller will _not_ attempt to use the record.
     */
    LibRecPtr = CFE_ES_LocateLibRecordByID(CFE_ES_LIBID_C(CheckId));
    return (LibRecPtr == NULL || CFE_ES_LibRecordIsUsed(LibRecPtr));
}
