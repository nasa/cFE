/**
 * @file
 *
 * cFE Internal configuration APIs
 *
 * The internal APIs prototyped within this file are only intended to be invoked from
 * other CFE core apps.  They still need to be prototyped in the shared header such that
 * they can be called from other core modules, but applications should not call these.
 */

#ifndef CFE_CONFIG_CORE_INTERNAL_H
#define CFE_CONFIG_CORE_INTERNAL_H

#include "cfe_config_api_typedefs.h"

#include "common_types.h"

/** @defgroup CFEAPIConfigCoreInternal cFE Internal configuration APIs, internal to CFE core
 * @{
 */

int32 CFE_Config_Init(void);

/*
 * The "Set" API is only used during init phase
 */
void CFE_Config_SetValue(CFE_ConfigId_t ConfigId, uint32 Value);
void CFE_Config_SetString(CFE_ConfigId_t ConfigId, const char *Ptr);
void CFE_Config_SetObjPointer(CFE_ConfigId_t ConfigId, const void *Ptr);

/**@}*/

#endif /* CFE_CONFIG_CORE_INTERNAL_H */
