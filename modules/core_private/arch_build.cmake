###########################################################
#
# CFE arch/platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Generate the "cfe_platform_cfg.h" and "cfe_msgids.h" header files
# these must come from mission config

generate_config_includefile(
    FILE_NAME           "cfe_platform_cfg.h"
    FALLBACK_FILE        "${CMAKE_CURRENT_LIST_DIR}/config/default_cfe_platform_cfg.h"
    MATCH_SUFFIX        "platform_cfg.h"
    PREFIXES            ${BUILD_CONFIG} cfe
)

generate_config_includefile(
    FILE_NAME           "cfe_core_private_internal_cfg.h"
    FALLBACK_FILE        "${CMAKE_CURRENT_LIST_DIR}/config/default_cfe_core_private_internal_cfg.h"
    PREFIXES            ${BUILD_CONFIG}
)
