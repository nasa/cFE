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

/*
**  File:
**      cfe_resource_api.c
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

#include "cfe.h"
#include "cfe_resourceid.h"
#include "cfe_resourceid_basevalue.h"

/*
 * The "CFE_RESOURCEID_MAX" limit is used as both a numeric maximum as well
 * as a mask to separate the serial number bits from the base value bits.
 *
 * This sanity checks that the value is one less than a power of two so it
 * works as a mask and the logic in this file works as expected.
 */
CompileTimeAssert(((CFE_RESOURCEID_MAX + 1) & CFE_RESOURCEID_MAX) == 0, CFE_RESOURCEID_MAX_BITMASK);

/**
 * State structure for use with CFE_ResourceId_DefaultIncrementSerial
 */
typedef struct
{
    uint32 ResourceType;
    uint32 TableSize;
    uint32 RemainingCount;
} CFE_ResourceId_DefaultIncrementState_t;

/*----------------------------------------------------------------
 *
 * Local helper routine, not invoked outside of this unit
 * Default implementation of serial increment, treats all as equal
 *
 *-----------------------------------------------------------------*/
static CFE_ResourceId_t CFE_ResourceId_DefaultIncrementSerial(CFE_ResourceId_t Id, void *Arg)
{
    CFE_ResourceId_DefaultIncrementState_t *State = Arg;
    CFE_ResourceId_t                        Result;
    uint32                                  Serial;

    if (State->RemainingCount == 0)
    {
        Result = CFE_RESOURCEID_UNDEFINED;
    }
    else
    {
        Serial = CFE_ResourceId_GetSerial(Id);

        ++Serial;
        if (Serial >= CFE_RESOURCEID_MAX)
        {
            Serial %= State->TableSize;
        }

        Result = CFE_ResourceId_FromInteger(State->ResourceType + Serial);

        --State->RemainingCount;
    }

    return Result;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_ResourceId_GetBase(CFE_ResourceId_t ResourceId)
{
    return (CFE_ResourceId_ToInteger(ResourceId) & ~((uint32)CFE_RESOURCEID_MAX));
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
uint32 CFE_ResourceId_GetSerial(CFE_ResourceId_t ResourceId)
{
    return (CFE_ResourceId_ToInteger(ResourceId) & ((uint32)CFE_RESOURCEID_MAX));
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
int32 CFE_ResourceId_ToIndex(CFE_ResourceId_t Id, uint32 BaseValue, uint32 TableSize, uint32 *Idx)
{
    uint32 Serial;

    if (Idx == NULL)
    {
        return CFE_ES_BAD_ARGUMENT;
    }

    Serial = CFE_ResourceId_ToInteger(Id) - BaseValue;

    if (Serial > CFE_RESOURCEID_MAX || TableSize == 0)
    {
        return CFE_ES_ERR_RESOURCEID_NOT_VALID;
    }

    *Idx = Serial % TableSize;
    return CFE_SUCCESS;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ResourceId_t CFE_ResourceId_FindNextEx(CFE_ResourceId_t StartId, CFE_ResourceId_IncrementFunc_t IncrFunc,
                                           void *IncrArg, CFE_ResourceId_CheckFunc_t CheckFunc)
{
    CFE_ResourceId_t CheckId;

    if (CheckFunc == NULL || IncrFunc == NULL)
    {
        CheckId = CFE_RESOURCEID_UNDEFINED;
    }
    else
    {
        CheckId = StartId;
        do
        {
            CheckId = IncrFunc(CheckId, IncrArg);
            if (!CFE_ResourceId_IsDefined(CheckId))
            {
                break;
            }

        } while (CheckFunc(CheckId));
    }

    return CheckId;
}

/*----------------------------------------------------------------
 *
 * Implemented per public API
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
CFE_ResourceId_t CFE_ResourceId_FindNext(CFE_ResourceId_t StartId, uint32 TableSize,
                                         CFE_ResourceId_CheckFunc_t CheckFunc)
{
    CFE_ResourceId_DefaultIncrementState_t State;

    memset(&State, 0, sizeof(State));

    State.ResourceType   = CFE_ResourceId_GetBase(StartId);
    State.TableSize      = TableSize;
    State.RemainingCount = TableSize;

    return CFE_ResourceId_FindNextEx(StartId, CFE_ResourceId_DefaultIncrementSerial, &State, CheckFunc);
}
