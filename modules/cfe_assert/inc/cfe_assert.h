/*************************************************************************
**
**      GSC-18128-1, "Core Flight Executive Version 6.7"
**
**      Copyright (c) 2006-2019 United States Government as represented by
**      the Administrator of the National Aeronautics and Space Administration.
**      All Rights Reserved.
**
**      Licensed under the Apache License, Version 2.0 (the "License");
**      you may not use this file except in compliance with the License.
**      You may obtain a copy of the License at
**
**        http://www.apache.org/licenses/LICENSE-2.0
**
**      Unless required by applicable law or agreed to in writing, software
**      distributed under the License is distributed on an "AS IS" BASIS,
**      WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**      See the License for the specific language governing permissions and
**      limitations under the License.
**
** File: cfe_assert.h
**
** Purpose:
**   Specification for the CFE assert (UT assert wrapper) functions.
**
*************************************************************************/
#ifndef cfe_assert_h_
#define cfe_assert_h_

/************************************************************************
** Includes
*************************************************************************/
#include <common_types.h>

/************************************************************************
** Type Definitions
*************************************************************************/

/*************************************************************************
** Exported Functions
*************************************************************************/

/************************************************************************/
/** \brief Application Entry Point Function
**
**  \par Description
**        This function should be specified in the cfe_es_startup.scr file
**        as part of starting this application.
**
**  \par Assumptions, External Events, and Notes:
**        None
**
**  \return Execution status, see \ref CFEReturnCodes
**
**
*************************************************************************/
void CFE_Assert_AppMain(void);

#endif /* cfe_assert_h_ */

/************************/
/*  End of File Comment */
/************************/
