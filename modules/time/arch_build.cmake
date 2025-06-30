###########################################################
#
# TIME Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TIME configuration
set(TIME_PLATFORM_CONFIG_FILE_LIST
  cfe_time_internal_cfg_values.h
  cfe_time_msgid_values.h
  cfe_time_msgids.h
  cfe_time_platform_cfg.h
)

generate_configfile_set(${TIME_PLATFORM_CONFIG_FILE_LIST})

