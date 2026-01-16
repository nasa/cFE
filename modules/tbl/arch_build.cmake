###########################################################
#
# TBL Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TBL configuration
set(TBL_PLATFORM_CONFIG_FILE_LIST
  cfe_tbl_internal_cfg_values.h
  cfe_tbl_msgid_values.h
  cfe_tbl_msgids.h
  cfe_tbl_platform_cfg.h
)

generate_configfile_set(${TBL_PLATFORM_CONFIG_FILE_LIST})
