###########################################################
#
# SB Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the SB configuration
set(SB_PLATFORM_CONFIG_FILE_LIST
  cfe_sb_internal_cfg_values.h
  cfe_sb_msgid_values.h
  cfe_sb_msgids.h
  cfe_sb_platform_cfg.h
)

generate_configfile_set(${SB_PLATFORM_CONFIG_FILE_LIST})
