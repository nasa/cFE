###########################################################
#
# CFE arch/platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Generate the "cfe_platform_cfg.h" and "cfe_msgids.h" header files
# these usually come from user config, but CFE provides defaults

set(CORE_API_PLATFORM_CONFIG_FILE_LIST
    cfe_core_api_base_msgids.h
    cfe_msgids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(CORE_API_CFGFILE ${CORE_API_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${CORE_API_CFGFILE}" NAME_WE)
  if (DEFINED CORE_API_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${CORE_API_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${CORE_API_CFGFILE}")
  endif()

  generate_config_includefile(
    FILE_NAME           "${CORE_API_CFGFILE}"
    PREFIXES            ${BUILD_CONFIG} cfe
    ${DEFAULT_SOURCE}
  )
endforeach()
