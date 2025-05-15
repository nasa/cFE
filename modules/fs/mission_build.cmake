###########################################################
#
# FS Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the FS configuration
set(FS_MISSION_CONFIG_FILE_LIST
  cfe_fs_mission_cfg.h
  cfe_fs_interface_cfg_values.h
  cfe_fs_filedef.h
  cfe_fs_extern_typedefs.h
)

generate_configfile_set(${FS_MISSION_CONFIG_FILE_LIST})
