###########################################################
#
# SB Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the SB configuration
set(SB_MISSION_CONFIG_FILE_LIST
  cfe_sb_mission_cfg.h
  cfe_sb_interface_cfg_values.h
  cfe_sb_extern_typedefs.h
  cfe_sb_fcncode_values.h
  cfe_sb_msgdefs.h
  cfe_sb_msg.h
  cfe_sb_msgstruct.h
  cfe_sb_topicid_values.h
)

generate_configfile_set(${SB_MISSION_CONFIG_FILE_LIST})
