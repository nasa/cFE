##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################

set(TESTCASE_SOURCES 
    "sb_ut_helpers.c" # Useful helper functions for everything 
    "sb_UT.c"         # The traditional "sb_UT.c" has most of the test cases
)

# The final bit of support/helper code varies depending on whether EDS is enabled
if (CFE_EDS_ENABLED)
    list(APPEND TESTCASE_SOURCES sb_ut_eds_dispatch.c)
else(CFE_EDS_ENABLED)
    list(APPEND TESTCASE_SOURCES sb_ut_default_dispatch.c)
endif (CFE_EDS_ENABLED)

# This relies on the updated add_cfe_coverage_test() for a couple features:
#  - Use of the keyword syntax allows multiple TESTCASE_SOURCES files, not just 1.
#  - We can pass in the sb_SOURCES list from parent project as SUBJECT_SOURCES without scrubbing
add_cfe_coverage_test(sb
    UNIT_NAME "ALL"
    TESTCASE_SOURCES ${TESTCASE_SOURCES}
    SUBJECT_SOURCES  ${sb_SOURCES}
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-sb-ALL-testrunner PRIVATE
    ${CFE_SB_SOURCE_DIR}/fsw/src
)

# The SB tests currently link with the _real_ SBR implementation (not a stub)
target_link_libraries(coverage-sb-ALL-testrunner ut_core_private_stubs sbr)

# If this is EDS build, also link with those stubs
if (CFE_EDS_ENABLED)
    target_link_libraries(coverage-sb-ALL-testrunner ut_edslib_stubs ut_missionlib_stubs)
endif()

