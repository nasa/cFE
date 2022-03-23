/**
 * @file
 *
 * Encapsulates all ES module internal header files, as well
 * as the public API from all other CFE core modules, OSAL, and PSP.
 *
 * This simplifies the set of include files that need to be put at the
 * start of every source file.
 */

#ifndef CFE_ES_MODULE_ALL_H
#define CFE_ES_MODULE_ALL_H

/*
** Includes
*/
#include "cfe.h"
#include "cfe_platform_cfg.h"

#include "cfe_msgids.h"
#include "cfe_perfids.h"

#include "cfe_es_core_internal.h"
#include "cfe_es_apps.h"
#include "cfe_es_cds.h"
#include "cfe_es_perf.h"
#include "cfe_es_generic_pool.h"
#include "cfe_es_mempool.h"
#include "cfe_es_global.h"
#include "cfe_es_cds_mempool.h"
#include "cfe_es_events.h"
#include "cfe_es_start.h"
#include "cfe_es_task.h"
#include "cfe_es_resource.h"
#include "cfe_es_log.h"

#endif /* CFE_ES_MODULE_ALL_H */
