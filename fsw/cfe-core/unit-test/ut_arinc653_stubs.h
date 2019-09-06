/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
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

#ifndef __UT_ARINC653_STUBS_H_
#define __UT_ARINC653_STUBS_H_

#ifdef CFE_ARINC653
/*****************************************************************************/
/**
** \brief Set the library initialization return code
**
** \par Description
**        Set the library initialization return code.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \param[in] LibInitRtn  Library init return code
**
** \returns
**        This function does not return a value.
**
******************************************************************************/
void UT_SetLibInitRtn(int32 LibInitRtn);

/*****************************************************************************/
/**
** \brief Return the library initialization return code
**
** \par Description
**        Return the library initialization return code.
**
** \par Assumptions, External Events, and Notes:
**        None
**
** \returns
**        Returns the library initialization return code.
**
******************************************************************************/
uint32 UT_LibInit(void);

#endif

#endif /* __UT_ARINC653_STUBS_H_ */
