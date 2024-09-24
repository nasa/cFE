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
  cfe_fs_interface_cfg.h
  cfe_fs_filedef.h
  cfe_fs_extern_typedefs.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(FS_CFGFILE ${FS_MISSION_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${FS_CFGFILE}" NAME_WE)
  if (DEFINED FS_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${FS_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${FS_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${FS_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
