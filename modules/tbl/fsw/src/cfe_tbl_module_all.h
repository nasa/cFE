/**
 * @file
 *
 * Encapsulates all TBL module internal header files, as well
 * as the public API from all other CFE core modules, OSAL, and PSP.
 *
 * This simplifies the set of include files that need to be put at the
 * start of every source file.
 */

#ifndef CFE_TBL_MODULE_ALL_H
#define CFE_TBL_MODULE_ALL_H

/*
** Includes
*/
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "cfe_msgids.h"
#include "cfe_perfids.h"

#include "cfe_tbl_core_internal.h"

#include "cfe_tbl_events.h"
#include "cfe_tbl_msg.h"
#include "cfe_tbl_internal.h"
#include "cfe_tbl_task.h"
#include "cfe_tbl_task_cmds.h"

/*
 * Additionally TBL needs to use special/extra CDS APIs that are not in the normal API
 */
#include "cfe_es_core_internal.h"

#endif /* CFE_TBL_MODULE_ALL_H */
