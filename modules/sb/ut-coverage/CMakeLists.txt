##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################

foreach(SRC ${sb_SOURCES})
    list(APPEND UT_SOURCES "${CFE_SB_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_test(sb ALL
    "sb_UT.c"
    "${UT_SOURCES}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-sb-ALL-testrunner PRIVATE
    ${CFE_SB_SOURCE_DIR}/fsw/src
)

# The SB tests currently link with the _real_ SBR implementation (not a stub)
target_link_libraries(coverage-sb-ALL-testrunner ut_core_private_stubs sbr)

