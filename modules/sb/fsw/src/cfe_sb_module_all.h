/**
 * @file
 *
 * Encapsulates all SB module internal header files, as well
 * as the public API from all other CFE core modules, OSAL, and PSP.
 *
 * This simplifies the set of include files that need to be put at the
 * start of every source file.
 */

#ifndef CFE_SB_MODULE_ALL_H
#define CFE_SB_MODULE_ALL_H

/*
** Includes
*/
#include "cfe.h"
#include "cfe_platform_cfg.h"
#include "cfe_msgids.h"
#include "cfe_perfids.h"

#include "cfe_sb_core_internal.h"

#include "cfe_sb_priv.h"
#include "cfe_sb_events.h"
#include "cfe_sb_destination_typedef.h"
#include "cfe_sb_msg.h"
#include "cfe_sbr.h"
#include "cfe_core_resourceid_basevalues.h"

#endif /* CFE_SB_MODULE_ALL_H */
