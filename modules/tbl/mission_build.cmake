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
  cfe_tbl_interface_cfg.h
  cfe_tbl_extern_typedefs.h
  cfe_tbl_fcncodes.h
  cfe_tbl_msgdefs.h
  cfe_tbl_msg.h
  cfe_tbl_msgstruct.h
  cfe_tbl_topicids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(TBL_CFGFILE ${TBL_MISSION_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${TBL_CFGFILE}" NAME_WE)
  if (DEFINED TBL_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${TBL_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${TBL_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${TBL_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
