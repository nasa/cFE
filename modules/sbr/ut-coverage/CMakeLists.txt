##################################################################
#
# cFE unit test build recipe
#
# This CMake file contains the recipe for building the cFE unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Set tests once so name changes are in one location
set(SBR_TEST_MAP_DIRECT "sbr_map_direct")
set(SBR_TEST_MAP_HASH "sbr_map_hash")
set(SBR_TEST_ROUTE_UNSORTED "sbr_route_unsorted")

# All coverage tests always built
set(SBR_TEST_SET ${SBR_TEST_MAP_DIRECT} ${SBR_TEST_MAP_HASH} ${SBR_TEST_ROUTE_UNSORTED})

# Add configured map implementation to routing test source
if (MISSION_MSGMAP_IMPLEMENTATION STREQUAL "DIRECT")
    set(${SBR_TEST_ROUTE_UNSORTED}_SRC ${CFE_SBR_SOURCE_DIR}/fsw/src/cfe_sbr_map_direct.c)
elseif (MISSION_MSGMAP_IMPLEMENTATION STREQUAL "HASH")
    set(${SBR_TEST_ROUTE_UNSORTED}_SRC ${CFE_SBR_SOURCE_DIR}/fsw/src/cfe_sbr_map_hash.c)
endif()

# Add route implementation to map hash
set(${SBR_TEST_MAP_HASH}_SRC ${CFE_SBR_SOURCE_DIR}/fsw/src/cfe_sbr_route_unsorted.c)

foreach(SBR_TEST ${SBR_TEST_SET})

    # Unit test object library sources, options, and includes
    add_library(ut_${SBR_TEST}_objs OBJECT ${${SBR_TEST}_SRC} ${CFE_SBR_SOURCE_DIR}/fsw/src/cfe_${SBR_TEST}.c)
    target_compile_options(ut_${SBR_TEST}_objs PRIVATE ${UT_COVERAGE_COMPILE_FLAGS})
    target_include_directories(ut_${SBR_TEST}_objs PRIVATE
         $<TARGET_PROPERTY:${DEP},INCLUDE_DIRECTORIES>)

    set (ut_${SBR_TEST}_tests
        test_cfe_${SBR_TEST}.c
        $<TARGET_OBJECTS:ut_${SBR_TEST}_objs>)

    # Add executable
    add_executable(${SBR_TEST}_UT ${ut_${SBR_TEST}_tests})

    # Add include to get private defaults
    target_include_directories(${SBR_TEST}_UT PRIVATE ../fsw/src)

    # Also add the UT_COVERAGE_LINK_FLAGS to the link command
    # This should enable coverage analysis on platforms that support this
    target_link_libraries(${SBR_TEST}_UT
        ${UT_COVERAGE_LINK_FLAGS}
        ut_core_private_stubs
        ut_core_api_stubs
        ut_assert)

    add_test(${SBR_TEST}_UT ${SBR_TEST}_UT)
    foreach(TGT ${INSTALL_TARGET_LIST})
        install(TARGETS ${SBR_TEST}_UT DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
    endforeach()

endforeach(SBR_TEST ${SBR_TEST_SET})
