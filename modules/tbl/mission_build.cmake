###########################################################
#
# TBL Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TBL configuration
set(TBL_MISSION_CONFIG_FILE_LIST
  cfe_tbl_mission_cfg.h
  cfe_tbl_interface_cfg_values.h
  cfe_tbl_extern_typedefs.h
  cfe_tbl_fcncode_values.h
  cfe_tbl_msgdefs.h
  cfe_tbl_msg.h
  cfe_tbl_msgstruct.h
  cfe_tbl_topicid_values.h
)

generate_configfile_set(${TBL_MISSION_CONFIG_FILE_LIST})
