###########################################################
#
# MSG mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Extended header inclusion selection
if (MISSION_INCLUDE_CCSDSEXT_HEADER)
  set(MSG_HDR_FILE "default_cfe_msg_hdr_priext.h")
else (MISSION_INCLUDE_CCSDSEXT_HEADER)
  set(MSG_HDR_FILE "default_cfe_msg_hdr_pri.h")
endif (MISSION_INCLUDE_CCSDSEXT_HEADER)

# Generate the header definition files, use local default for this module)
generate_config_includefile(
    FILE_NAME           "cfe_msg_hdr.h"
    FALLBACK_FILE       "${CMAKE_CURRENT_LIST_DIR}/option_inc/${MSG_HDR_FILE}"
)

generate_config_includefile(
    FILE_NAME           "cfe_msg_sechdr.h"
    FALLBACK_FILE       "${CMAKE_CURRENT_LIST_DIR}/option_inc/default_cfe_msg_sechdr.h"
)
