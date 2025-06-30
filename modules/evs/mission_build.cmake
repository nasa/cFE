###########################################################
#
# EVS Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the EVS configuration
set(EVS_MISSION_CONFIG_FILE_LIST
  cfe_evs_mission_cfg.h
  cfe_evs_interface_cfg_values.h
  cfe_evs_extern_typedefs.h
  cfe_evs_fcncode_values.h
  cfe_evs_msgdefs.h
  cfe_evs_msg.h
  cfe_evs_msgstruct.h
  cfe_evs_topicid_values.h
)

generate_configfile_set(${EVS_MISSION_CONFIG_FILE_LIST})

