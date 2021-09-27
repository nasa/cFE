##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################

set(UT_SOURCES)
foreach(SRC ${config_SOURCES})
    list(APPEND UT_SOURCES "${CFE_CONFIG_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_test(config ALL
    "test_cfe_config.c"
    "${UT_SOURCES}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-config-ALL-testrunner PRIVATE
    ${CFE_CONFIG_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-config-ALL-testrunner ut_core_private_stubs)
