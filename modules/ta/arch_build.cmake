###########################################################
#
# TA Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TA configuration
set(TA_PLATFORM_CONFIG_FILE_LIST
  cfe_ta_internal_cfg_values.h
  cfe_ta_msgid_values.h
  cfe_ta_msgids.h
  cfe_ta_platform_cfg.h
)

generate_configfile_set(${TA_PLATFORM_CONFIG_FILE_LIST})
