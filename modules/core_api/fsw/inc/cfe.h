/**
 * @file
 *
 * Purpose:  cFE header file
 *
 * Author:   David Kobe, the Hammers Company, Inc.
 *
 * Notes:    This header file centralizes the includes for all cFE
 *           Applications.  It includes all header files necessary
 *           to completely define the cFE interface.
 *
 */

#ifndef CFE_H
#define CFE_H

#include "common_types.h" /* Define basic data types */

#include "osapi.h" /* Define OS API function prototypes */

#include "cfe_mission_cfg.h" /* Define mission configuration parameters */

#include "cfe_error.h" /* Define common cFE error codes */

#include "cfe_es.h"   /* Define Executive Service API */
#include "cfe_evs.h"  /* Define Event Service API */
#include "cfe_fs.h"   /* Define File Service API */
#include "cfe_sb.h"   /* Define Software Bus API */
#include "cfe_time.h" /* Define Time Service API */
#include "cfe_tbl.h"  /* Define Table Service API */

#include "cfe_msg.h"        /* Define Message API */
#include "cfe_resourceid.h" /* Define ResourceID API */

#include "cfe_psp.h" /* Define Platform Support Package API */

#endif /* CFE_H */
