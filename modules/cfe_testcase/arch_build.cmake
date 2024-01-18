###########################################################
#
# TEST Core Module platform build setup
#
# This file is evaluated as part of the "prepare" stage
# and can be used to set up prerequisites for the build,
# such as generating header files
#
###########################################################

# The list of header files that control the TEST configuration
set(TEST_PLATFORM_CONFIG_FILE_LIST
  cfe_test_msgids.h
)

# Create wrappers around the all the config header files
# This makes them individually overridable by the missions, without modifying
# the distribution default copies
foreach(TEST_CFGFILE ${TEST_PLATFORM_CONFIG_FILE_LIST})
  get_filename_component(CFGKEY "${TEST_CFGFILE}" NAME_WE)
  if (DEFINED TESTCASE_CFGFILE_SRC_${CFGKEY})
    set(DEFAULT_SOURCE GENERATED_FILE "${TESTCASE_CFGFILE_SRC_${CFGKEY}}")
  else()
    set(DEFAULT_SOURCE FALLBACK_FILE "${CMAKE_CURRENT_LIST_DIR}/config/default_${TEST_CFGFILE}")
  endif()
  generate_config_includefile(
    FILE_NAME           "${TEST_CFGFILE}"
    ${DEFAULT_SOURCE}
  )
endforeach()
