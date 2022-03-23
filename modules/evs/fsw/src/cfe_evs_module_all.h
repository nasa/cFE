/**
 * @file
 *
 * Encapsulates all EVS module internal header files, as well
 * as the public API from all other CFE core modules, OSAL, and PSP.
 *
 * This simplifies the set of include files that need to be put at the
 * start of every source file.
 */

#ifndef CFE_EVS_MODULE_ALL_H
#define CFE_EVS_MODULE_ALL_H

/********************* Include Files  ************************/

#include "cfe.h" /* All CFE+OSAL public API definitions */
#include "cfe_platform_cfg.h"

#include "cfe_msgids.h"
#include "cfe_perfids.h"

#include "cfe_evs_core_internal.h"

#include "cfe_evs_events.h" /* EVS event IDs */
#include "cfe_evs_task.h"   /* EVS internal definitions */
#include "cfe_evs_log.h"    /* EVS log file definitions */
#include "cfe_evs_utils.h"  /* EVS utility function definitions */

#endif /* CFE_EVS_MODULE_ALL_H */
