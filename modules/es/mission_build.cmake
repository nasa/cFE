###########################################################
#
# ES Core Module mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the ES configuration
set(ES_MISSION_CONFIG_FILE_LIST
  cfe_es_mission_cfg.h
  cfe_es_interface_cfg.h
  cfe_es_extern_typedefs.h
  cfe_es_fcncodes.h
  cfe_es_msgdefs.h
  cfe_es_msg.h
  cfe_es_msgstruct.h
  cfe_es_topicids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(ES_CFGFILE ${ES_MISSION_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${ES_CFGFILE}" NAME_WE)
  if (DEFINED ES_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${ES_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${ES_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${ES_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
