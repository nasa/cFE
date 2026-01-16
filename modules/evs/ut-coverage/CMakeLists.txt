##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################

set(TESTCASE_SOURCES 
    "evs_ut_helpers.c" # Useful helper functions for everything 
    "evs_UT.c"         # The traditional "es_UT.c" has most of the test cases
)

# The final bit of support/helper code varies depending on whether EDS is enabled
if (CFE_EDS_ENABLED)
    list(APPEND TESTCASE_SOURCES evs_ut_eds_dispatch.c)
else(CFE_EDS_ENABLED)
    list(APPEND TESTCASE_SOURCES evs_ut_default_dispatch.c)
endif (CFE_EDS_ENABLED)

# This relies on the updated add_cfe_coverage_test() for a couple features:
#  - Use of the keyword syntax allows multiple TESTCASE_SOURCES files, not just 1.
#  - We can pass in the evs_SOURCES list from parent project as SUBJECT_SOURCES without scrubbing
add_cfe_coverage_test(evs
    UNIT_NAME "ALL"
    TESTCASE_SOURCES ${TESTCASE_SOURCES}
    SUBJECT_SOURCES  ${evs_SOURCES}
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-evs-ALL-testrunner PRIVATE
    ${CFE_EVS_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-evs-ALL-testrunner ut_core_private_stubs)

