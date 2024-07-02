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
 * Contains CFE internal prototypes and definitions related to resource
 * management and related CFE resource IDs.
 *
 * A CFE ES Resource ID is a common way to identify CFE-managed resources such
 * as apps, tasks, counters, memory pools, CDS blocks, and other entities.
 */

#ifndef CFE_CORE_RESOURCEID_BASEVALUES_H
#define CFE_CORE_RESOURCEID_BASEVALUES_H

/*
** Include Files
*/
#include "cfe_resourceid_basevalue.h"

/** @defgroup CFEESResourceIDBase cFE Resource ID base values
 * @{
 */

/*
 * Assign unique offsets per resource types used in CFE core apps.
 *
 * Applications should not use these values directly, but rather
 * in conjunction with the CFE_RESOURCEID_MAKE_BASE macro provided
 * by the Resource ID module.  (see below)
 */
enum
{
    /*
     * Note for Task ID base value --
     * This currently shares the same offset as OSAL tasks, such that
     * when "simple" (non-enforcing/backward-compatible) IDs are selected,
     * the CFE task IDs and the OSAL task IDs end up as the same value.
     *
     * The "CFE_RESOURCEID_MARK" bit still differentiates the value when
     * in strict mode, so there is no overlap in that case.
     */
    CFE_RESOURCEID_ES_TASKID_BASE_OFFSET = OS_OBJECT_TYPE_OS_TASK,

    /* Other ES managed resources */
    CFE_RESOURCEID_ES_APPID_BASE_OFFSET      = OS_OBJECT_TYPE_USER + 1,
    CFE_RESOURCEID_ES_LIBID_BASE_OFFSET      = OS_OBJECT_TYPE_USER + 2,
    CFE_RESOURCEID_ES_COUNTID_BASE_OFFSET    = OS_OBJECT_TYPE_USER + 3,
    CFE_RESOURCEID_ES_POOLID_BASE_OFFSET     = OS_OBJECT_TYPE_USER + 4,
    CFE_RESOURCEID_ES_CDSBLOCKID_BASE_OFFSET = OS_OBJECT_TYPE_USER + 5,

    /* SB managed resources */
    CFE_RESOURCEID_SB_PIPEID_RESOURCE_BASE_OFFSET = OS_OBJECT_TYPE_USER + 6,

    /* configuration registry */
    CFE_RESOURCEID_CONFIGID_BASE_OFFSET = OS_OBJECT_TYPE_USER + 7,

    /* TBL managed resources */
    CFE_RESOURCEID_TBL_VALRESULTID_BASE_OFFSET = OS_OBJECT_TYPE_USER + 8,
    CFE_RESOURCEID_TBL_DUMPCTRLID_BASE_OFFSET  = OS_OBJECT_TYPE_USER + 9,

};

/*
 * Assign actual base values from the offsets above
 *
 * Using "enum" ensures these are resolved as integers now, as opposed to at the point of use like macros.
 */
enum
{
    /* ES managed resources */
    CFE_ES_TASKID_BASE     = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_ES_TASKID_BASE_OFFSET),
    CFE_ES_APPID_BASE      = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_ES_APPID_BASE_OFFSET),
    CFE_ES_LIBID_BASE      = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_ES_LIBID_BASE_OFFSET),
    CFE_ES_COUNTID_BASE    = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_ES_COUNTID_BASE_OFFSET),
    CFE_ES_POOLID_BASE     = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_ES_POOLID_BASE_OFFSET),
    CFE_ES_CDSBLOCKID_BASE = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_ES_CDSBLOCKID_BASE_OFFSET),

    /* SB managed resources */
    CFE_SB_PIPEID_BASE = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_SB_PIPEID_RESOURCE_BASE_OFFSET),

    /* configuration registry */
    CFE_CONFIGID_BASE = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_CONFIGID_BASE_OFFSET),

    /* TBL managed resources */
    CFE_TBL_VALRESULTID_BASE = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_TBL_VALRESULTID_BASE_OFFSET),
    CFE_TBL_DUMPCTRLID_BASE  = CFE_RESOURCEID_MAKE_BASE(CFE_RESOURCEID_TBL_DUMPCTRLID_BASE_OFFSET),

};

/** @} */

#endif /* CFE_CORE_RESOURCEID_BASEVALUES_H */
