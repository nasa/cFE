
#include "cfeconfig_platformdata_tool.h"

#include <cfe_platform_cfg.h>

/*
 * To prevent name collisions, this table should be named using a platform-specific name.
 * This can be done by setting a custom macro definition for this symbol.  If this fails,
 * it points to a bug in the cmake script which is supposed to set that up.
 */
#ifndef MAP_ENTRY_LIST
#error MAP_ENTRY_LIST should be defined to a platform-specific symbol in this file
#endif

/* Historical backward compatibility items, do not use if OMIT_DEPRECATED is set */
#ifndef CFE_OMIT_DEPRECATED

/* Originally SB did not have its own number of buckets, it simply used the ES value */
#ifndef CFE_PLATFORM_SB_POOL_MAX_BUCKETS
#define CFE_PLATFORM_SB_POOL_MAX_BUCKETS CFE_PLATFORM_ES_POOL_MAX_BUCKETS
#endif

/*
 * These table items were allowed to be undefined,
 * which had the same effect as defining it as 0
 */
#if !defined(CFE_PLATFORM_TBL_VALID_SCID_COUNT)
#define CFE_PLATFORM_TBL_VALID_SCID_COUNT 0
#endif

#if !defined(CFE_PLATFORM_TBL_VALID_PRID_COUNT)
#define CFE_PLATFORM_TBL_VALID_PRID_COUNT 0
#endif

#endif

/*
 * NOTE: The CONFIG_ID type/constants cannot be used here, as that is a CFE-defined type (resourceid)
 * and thus would necessitate the full set of CFE core API headers.  But as this is the global/mission
 * scope and not platform scope, the latter is not available.
 */

CFE_ConfigTool_DetailEntry_t MAP_ENTRY_LIST[] = {
    {
        .render_style = CFE_ConfigTool_RenderStyle_ARRAY,
        .ido          = CFE_ConfigIdOffset_PLATFORM_TBL_VALID_SCID,
        .u.list =
            {
                .prefix      = "CFE_PLATFORM_TBL_VALID_SCID",
                .type_name   = "uint32_t",
                .num_entries = CFE_PLATFORM_TBL_VALID_SCID_COUNT,
            },
    },
    {
        .render_style = CFE_ConfigTool_RenderStyle_ARRAY,
        .ido          = CFE_ConfigIdOffset_PLATFORM_TBL_VALID_PRID,
        .u.list =
            {
                .prefix      = "CFE_PLATFORM_TBL_VALID_PRID",
                .type_name   = "uint32_t",
                .num_entries = CFE_PLATFORM_TBL_VALID_PRID_COUNT,
            },
    },
    {
        .render_style = CFE_ConfigTool_RenderStyle_ARRAY,
        .ido          = CFE_ConfigIdOffset_PLATFORM_ES_MEM_BLOCK_SIZE,
        .u.list       = {.prefix      = "CFE_PLATFORM_ES_MEM_BLOCK_SIZE",
                   .type_name   = "size_t",
                   .num_entries = CFE_PLATFORM_ES_POOL_MAX_BUCKETS,
                   .custom_max  = "CFE_PLATFORM_ES_MAX_BLOCK_SIZE",
                   .options     = CFE_CONFIGTOOL_LIST_DESCENDING | CFE_CONFIGTOOL_USE_LEADING_ZERO},
    },
    {
        .render_style = CFE_ConfigTool_RenderStyle_ARRAY,
        .ido          = CFE_ConfigIdOffset_PLATFORM_ES_CDS_MEM_BLOCK_SIZE,
        .u.list       = {.prefix      = "CFE_PLATFORM_ES_CDS_MEM_BLOCK_SIZE",
                   .type_name   = "size_t",
                   .num_entries = CFE_PLATFORM_ES_POOL_MAX_BUCKETS,
                   .custom_max  = "CFE_PLATFORM_ES_CDS_MAX_BLOCK_SIZE",
                   .options     = CFE_CONFIGTOOL_LIST_DESCENDING | CFE_CONFIGTOOL_USE_LEADING_ZERO},
    },
    {
        .render_style = CFE_ConfigTool_RenderStyle_ARRAY,
        .ido          = CFE_ConfigIdOffset_PLATFORM_SB_MEM_BLOCK_SIZE,
        .u.list       = {.prefix      = "CFE_PLATFORM_SB_MEM_BLOCK_SIZE",
                   .type_name   = "size_t",
                   .num_entries = CFE_PLATFORM_SB_POOL_MAX_BUCKETS,
                   .custom_max  = "CFE_PLATFORM_SB_MAX_BLOCK_SIZE",
                   .options     = CFE_CONFIGTOOL_LIST_DESCENDING | CFE_CONFIGTOOL_USE_LEADING_ZERO},
    },

    {0} /* Keep this last */
};
