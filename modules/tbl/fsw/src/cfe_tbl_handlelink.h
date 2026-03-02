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
 * Header definining the table services handle link and related methods
 */

#ifndef CFE_TBL_HANDLELINK_H
#define CFE_TBL_HANDLELINK_H

/*
** Required header files
*/
#include "common_types.h"
#include "cfe_tbl_api_typedefs.h"
#include "cfe_tbl_resource.h"

/**
 * A structure to facilitate a linked-list of table handles
 */
typedef struct CFE_TBL_HandleLink
{
    CFE_TBL_HandleId_t Next; /**< Next table handle in list */
    CFE_TBL_HandleId_t Prev; /**< Previous table handle in list */
} CFE_TBL_HandleLink_t;

/*---------------------------------------------------------------------------------------*/
/**
** \brief Initializes a handle link
**
** \par Description
**        Sets the handle link to initial condition, where it is not a member of any list
**        After this call, CFE_TBL_HandleLinkIsAttached() on this link will always return false
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param LinkPtr Pointer to link entry to initialize
*/
void CFE_TBL_HandleLinkInit(CFE_TBL_HandleLink_t *LinkPtr);

/*---------------------------------------------------------------------------------------*/
/**
** \brief Checks if a handle link is attached to another entry
**
** \par Description
**        This will return true if the passed-in link is attached to another list node,
**        indicating it is part of a list.  Conversely, this will return false if the
**        link is not attached to another node, indicating a singleton or empty list.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param LinkPtr Pointer to link entry to check
** \retval true if the link node is part of a list (attached)
** \retval false if the link node is not part of a list (detached)
*/
bool CFE_TBL_HandleLinkIsAttached(CFE_TBL_HandleLink_t *LinkPtr);

#endif /* CFE_TBL_TASK_H */
