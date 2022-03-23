/**
 * @file
 *
 * Encapsulates all TIME module internal header files, as well
 * as the public API from all other CFE core modules, OSAL, and PSP.
 *
 * This simplifies the set of include files that need to be put at the
 * start of every source file.
 */

#ifndef CFE_TIME_MODULE_ALL_H
#define CFE_TIME_MODULE_ALL_H

/********************* Include Files  ************************/

#include "cfe.h" /* All CFE+OSAL public API definitions */
#include "cfe_platform_cfg.h"

#include "cfe_msgids.h"
#include "cfe_perfids.h"

#include "cfe_time_core_internal.h"

#include "cfe_time_msg.h"
#include "cfe_time_events.h"
#include "cfe_time_utils.h"

#endif /* CFE_TIME_MODULE_ALL_H */
