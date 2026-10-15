##################################################################
#
# cFE TA unit test CMake build recipe
#
##################################################################

set(TESTCASE_SOURCES 
    "ta_ut_helpers.c"
    "ta_UT.c"         
)

# Switch the dispatch logic depending on the build type
if (CFE_EDS_ENABLED)
    list(APPEND TESTCASE_SOURCES ta_ut_eds_dispatch.c)
else(CFE_EDS_ENABLED)
    list(APPEND TESTCASE_SOURCES ta_ut_default_dispatch.c)
endif (CFE_EDS_ENABLED)

add_cfe_coverage_test(ta
    UNIT_NAME "ALL"
    TESTCASE_SOURCES ${TESTCASE_SOURCES}
    SUBJECT_SOURCES  ${ta_SOURCES}
)

# This permits UT test cases to directly access private headers
target_include_directories(coverage-ta-ALL-testrunner PRIVATE
    ${CFE_SOURCE_DIR}/modules/ta/fsw/src
)

target_link_libraries(coverage-ta-ALL-testrunner ut_core_private_stubs)
