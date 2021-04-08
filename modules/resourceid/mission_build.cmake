###########################################################
#
# Resource ID mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Check if strict/enforcing typedef should be used
if (MISSION_RESOURCEID_MODE STREQUAL "STRICT")
  set(RESOURCEID_HDR_FILE "cfe_resourceid_strict.h")
else ()
  set(RESOURCEID_HDR_FILE "cfe_resourceid_simple.h")
endif ()

# Generate the header definition files, use local default for this module)
generate_config_includefile(
    FILE_NAME           "cfe_resourceid_typedef.h"
    FALLBACK_FILE       "${CMAKE_CURRENT_LIST_DIR}/option_inc/${RESOURCEID_HDR_FILE}"
)

# Resource ID base value header
# Currently the "osal compatible" version is the only provided implementation,
# but missions can provide their own if desired to override this.
generate_config_includefile(
    FILE_NAME           "cfe_resourceid_basevalue.h"
    FALLBACK_FILE       "${CMAKE_CURRENT_LIST_DIR}/option_inc/cfe_resourceid_osal_compatible.h"
)

include_directories(${CMAKE_CURRENT_LIST_DIR}/inc)
