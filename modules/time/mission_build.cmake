###########################################################
#
# TIME Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TIME configuration
set(TIME_MISSION_CONFIG_FILE_LIST
  cfe_time_mission_cfg.h
  cfe_time_interface_cfg_values.h
  cfe_time_extern_typedefs.h
  cfe_time_fcncode_values.h
  cfe_time_msgdefs.h
  cfe_time_msg.h
  cfe_time_msgstruct.h
  cfe_time_topicid_values.h
)

generate_configfile_set(${TIME_MISSION_CONFIG_FILE_LIST})

