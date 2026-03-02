###########################################################
#
# EVS Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the EVS configuration
set(EVS_PLATFORM_CONFIG_FILE_LIST
  cfe_evs_internal_cfg_values.h
  cfe_evs_msgid_values.h
  cfe_evs_msgids.h
  cfe_evs_platform_cfg.h
)

generate_configfile_set(${EVS_PLATFORM_CONFIG_FILE_LIST})
