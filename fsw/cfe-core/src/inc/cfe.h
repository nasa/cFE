/*
**  GSC-18128-1, "Core Flight Executive Version 6.7"
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

/*
** File: cfe.h
**
** Purpose:  cFE header file
**
** Author:   David Kobe, the Hammers Company, Inc.
**
** Notes:    This header file centralizes the includes for all cFE
**           Applications.  It includes all header files necessary
**           to completely define the cFE interface.
**
*/

/*************************************************************************/

/*
** Ensure that header is included only once...
*/
#ifndef _cfe_
#define _cfe_

#include "common_types.h"     /* Define basic data types */

#include "osapi.h"            /* Define OS API function prototypes */

#include "cfe_mission_cfg.h"  /* Define mission configuration parameters */

#include "cfe_error.h"        /* Define common cFE error codes */

#include "cfe_es.h"           /* Define Executive Service API */
#include "cfe_evs.h"          /* Define Event Service API */
#include "cfe_fs.h"           /* Define File Service API */
#include "cfe_sb.h"           /* Define Software Bus API */
#include "cfe_time.h"         /* Define Time Service API */
#include "cfe_tbl.h"          /* Define Table Service API */

#include "cfe_msg_api.h"      /* Define Message API */

#include "cfe_psp.h"          /* Define Platform Support Package API */

#endif  /* _cfe_ */
