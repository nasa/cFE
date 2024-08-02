##################################################################
#
# cFE Configuration Service (CONFIG) module CMake build recipe
#
##################################################################

project(CFE_CONFIG C)

# Executive services source files
set(config_SOURCES
    fsw/src/cfe_config_init.c
    fsw/src/cfe_config_lookup.c
    fsw/src/cfe_config_get.c
    fsw/src/cfe_config_set.c
)
add_library(config STATIC
  ${config_SOURCES}
  ${MISSION_BINARY_DIR}/src/cfe_configid_nametable.c
  ${CMAKE_BINARY_DIR}/src/cfe_platformcfg_tables.c
)

add_custom_command(
  OUTPUT ${CMAKE_BINARY_DIR}/src/cfe_platformcfg_tables.c
  COMMAND ${CMAKE_COMMAND}
    -D LIST_FILE=${MISSION_BINARY_DIR}/src/cfeconfig_platformdata_${TARGETSYSTEM}.list
    -D CONTENT_FILE=${MISSION_BINARY_DIR}/src/cfeconfig_platformdata_${TARGETSYSTEM}.c
    -D OUTPUT_FILE=${CMAKE_BINARY_DIR}/src/cfe_platformcfg_tables.c
    -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/create_cfe_platform_cfg.cmake
  WORKING_DIRECTORY
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  DEPENDS
    ${MISSION_BINARY_DIR}/src/cfeconfig_platformdata_${TARGETSYSTEM}.c
    ${MISSION_BINARY_DIR}/src/cfeconfig_platformdata_${TARGETSYSTEM}.list
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/create_cfe_platform_cfg.cmake
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/cfe_platform_cfg.c.in
)

target_include_directories(config PUBLIC fsw/inc)
target_link_libraries(config PRIVATE core_private)

# Add unit test coverage subdirectory
if (ENABLE_UNIT_TESTS)
  add_subdirectory(ut-coverage)
endif (ENABLE_UNIT_TESTS)
