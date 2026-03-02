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
 * Implementation of table services handle link methods
 */

/*
** Required header files...
*/
#include "cfe_tbl.h"
#include "cfe_tbl_handlelink.h"

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
void CFE_TBL_HandleLinkInit(CFE_TBL_HandleLink_t *LinkPtr)
{
    LinkPtr->Prev = CFE_TBL_HANDLEID_UNDEFINED;
    LinkPtr->Next = CFE_TBL_HANDLEID_UNDEFINED;
}

/*----------------------------------------------------------------
 *
 * Application-scope internal function
 * See description in header file for argument/return detail
 *
 *-----------------------------------------------------------------*/
bool CFE_TBL_HandleLinkIsAttached(CFE_TBL_HandleLink_t *LinkPtr)
{
    return (CFE_TBL_HandleID_IsDefined(LinkPtr->Next));
}
