/**
 * @file
 *
 * Purpose:  cFE Time Services (TIME) library API header file
 *
 * Author:   S.Walling/Microtel
 *
 * Notes:
 *
 */

#ifndef CFE_TIME_CORE_INTERNAL_H
#define CFE_TIME_CORE_INTERNAL_H

#include "common_types.h"
#include "cfe_es_extern_typedefs.h"

/*
 * The internal APIs prototyped within this block are only intended to be invoked from
 * other CFE core apps.  They still need to be prototyped in the shared header such that
 * they can be called from other core modules, but applications should not call these.
 */

/** @defgroup CFEAPITIMECoreInternal cFE Internal Time APIs, internal to CFE core
 * @{
 */

/*****************************************************************************/
/**
** \brief Entry Point for cFE Core Application
**
** \par Description
**        This is the entry point to the cFE TIME Core Application.
**
** \par Assumptions, External Events, and Notes:
**          None
**
******************************************************************************/
void CFE_TIME_TaskMain(void);

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
int32 CFE_TIME_EarlyInit(void);

/*****************************************************************************/
/**
** \brief Removes TIME resources associated with specified Application
**
** \par Description
**        This function is called by cFE Executive Services to cleanup after
**        an Application has been terminated.  It frees resources
**        that have been allocated to the specified Application.
**
******************************************************************************/
int32 CFE_TIME_CleanUpApp(CFE_ES_AppId_t AppId);

/**@}*/

#endif /* CFE_TIME_CORE_INTERNAL_H */
