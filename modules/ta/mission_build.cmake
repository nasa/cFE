###########################################################
#
# TA Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TA configuration
set(TA_MISSION_CONFIG_FILE_LIST
  cfe_ta_mission_cfg.h
  cfe_ta_interface_cfg_values.h
  cfe_ta_fcncode_values.h
  cfe_ta_msgdefs.h
  cfe_ta_msg.h
  cfe_ta_msgstruct.h
  cfe_ta_topicid_values.h
)

generate_configfile_set(${TA_MISSION_CONFIG_FILE_LIST})
