###########################################################
#
# CFE mission build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# Generate the "cfe_mission_cfg.h" and "cfe_perfids.h" header files
# these usually come from user config, but CFE provides defaults

set(CORE_API_PLATFORM_CONFIG_FILE_LIST
    cfe_core_api_interface_cfg.h
    cfe_mission_cfg.h
    cfe_perfids.h
)

if (CFE_EDS_ENABLED_BUILD)

  # In an EDS-based build, these files come generated from the EDS tool
  set(CORE_API_CFGFILE_SRC_cfe_core_api_interface_cfg "cfe_mission_eds_designparameters.h")

endif(CFE_EDS_ENABLED_BUILD)


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

  # For backward compatibilty, strip off the leading "cfe_" and do a suffix match
  # Other CFS apps don't do this, but this is a remnant of when the build system was first implemented
  string(REPLACE "cfe_" "" SUFFIX ${CORE_API_CFGFILE})

  generate_config_includefile(
    FILE_NAME           "${CORE_API_CFGFILE}"
    MATCH_SUFFIX        ${SUFFIX}
    PREFIXES            ${MISSIONCONFIG} cfe
    ${DEFAULT_SOURCE}
  )
endforeach()
