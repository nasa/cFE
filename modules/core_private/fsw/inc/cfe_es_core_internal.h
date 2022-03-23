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
 *  Purpose:
 *	Unit specification for Executive Services library functions and macros.
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *     cFE Flight Software Application Developers Guide
 *
 *	Notes:
 *
 */

#ifndef CFE_ES_CORE_INTERNAL_H
#define CFE_ES_CORE_INTERNAL_H

#include "common_types.h"
#include "cfe_es_extern_typedefs.h"

/*
 * The internal APIs prototyped within this block are only intended to be invoked from
 * other CFE core apps.  They still need to be prototyped in the shared header such that
 * they can be called from other core modules, but applications should not call these.
 */

/** @defgroup CFEAPIESCoreInternal cFE Internal Executive Service APIs, internal to CFE core
 * @{
 */

/*****************************************************************************/
/**
** \brief Entry Point for cFE Core Application
**
** \par Description
**        This is the entry point to the cFE ES Core Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
******************************************************************************/
void CFE_ES_TaskMain(void);

/*****************************************************************************/
/**
** \brief Initializes the cFE core module API Library
**
** \par Description
**        Initializes the cFE core module API Library
**
** \par Assumptions, External Events, and Notes:
**        -# This function MUST be called before any module API's are called.
**
******************************************************************************/
int32 CFE_ES_CDS_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Reserve space (or re-obtain previously reserved space) in the Critical Data Store (CDS)
**
** \par Description
**        This routine is identical to #CFE_ES_RegisterCDS except it identifies the contents
**        of the CDS as a critical table.  This is crucial because a critical table CDS must
**        only be deleted by cFE Table Services, not via an ES delete CDS command.  Otherwise,
**        Table Services may be out of sync with the contents of the CDS.
**
** \par Assumptions, External Events, and Notes:
**        -# This function assumes input parameters are error free and have met size/value restrictions.
**        -# The calling function is responsible for issuing any event messages associated with errors.
**
** \param[in, out]   HandlePtr   Pointer Application's variable that will contain the CDS Memory Block Handle.
**                               HandlePtr is the handle of the CDS block that can be used in #CFE_ES_CopyToCDS and
**                               #CFE_ES_RestoreFromCDS.
**
** \param[in]   UserBlockSize   The number of bytes needed in the CDS.
**
** \param[in]   Name        Pointer to character string containing the Application's local name for
**                          the CDS.
**
** \param[in]   CriticalTbl   Indicates whether the CDS is to be used as a Critical Table or not
**
** \return See return codes for #CFE_ES_RegisterCDS
**
******************************************************************************/
int32 CFE_ES_RegisterCDSEx(CFE_ES_CDSHandle_t *HandlePtr, size_t UserBlockSize, const char *Name, bool CriticalTbl);

/*****************************************************************************/
/**
** \brief Deletes the specified CDS from the CDS Registry and frees CDS Memory
**
** \par Description
**        Removes the record of the specified CDS from the CDS Registry and
**        frees the associated CDS memory for future use.
**
**        This operation invalidates the registry entry and returns
**        the underlying data storage back to the CDS pool for re-use.
**
** \par Assumptions, External Events, and Notes:
**        The actual data block is not modified by this call.  Specifically, this does not
**        "wipe" or otherwise overwrite the data block.  If the application needs to ensure
**        that the data is actually erased, it should explicitly do so (by e.g. writing all
**        zeros or all ones) before deleting the block.
**
** \param[in]  CDSName - Pointer to character string containing complete
**                       CDS Name (of the format "AppName.CDSName").
**
** \param[in]  CalledByTblServices - Flag that identifies whether the CDS is supposed to
**                       be a Critical Table Image or not.
**
** \return #CFE_SUCCESS                     \copydoc CFE_SUCCESS
** \return #CFE_ES_CDS_WRONG_TYPE_ERR       \copydoc CFE_ES_CDS_WRONG_TYPE_ERR
** \return #CFE_ES_CDS_OWNER_ACTIVE_ERR     \copydoc CFE_ES_CDS_OWNER_ACTIVE_ERR
** \return #CFE_ES_ERR_NAME_NOT_FOUND       \copydoc CFE_ES_ERR_NAME_NOT_FOUND
** \return Any of the return values from CFE_ES_UpdateCDSRegistry
** \return Any of the return values from CFE_ES_GenPoolPutBlock
**
******************************************************************************/
int32 CFE_ES_DeleteCDS(const char *CDSName, bool CalledByTblServices);

/**@}*/

#endif /* CFE_ES_CORE_INTERNAL_H */
