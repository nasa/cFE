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
  ${MISSION_BINARY_DIR}/src/cfe_config_map.c
)

# need to include the "src" dir explicitly here, in order to compile
# the generated tables under ${MISSION_BINARY_DIR}
target_include_directories(config PRIVATE fsw/src)
target_link_libraries(config PRIVATE core_private)

# Add unit test coverage subdirectory
if (ENABLE_UNIT_TESTS)
  add_subdirectory(ut-coverage)
endif (ENABLE_UNIT_TESTS)
