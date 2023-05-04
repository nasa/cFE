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
  cfe_sb_interface_cfg.h
  cfe_sb_extern_typedefs.h
  cfe_sb_fcncodes.h
  cfe_sb_msgdefs.h
  cfe_sb_msg.h
  cfe_sb_msgstruct.h
  cfe_sb_topicids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(SB_CFGFILE ${SB_MISSION_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${SB_CFGFILE}" NAME_WE)
  if (DEFINED SB_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${SB_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${SB_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${SB_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
