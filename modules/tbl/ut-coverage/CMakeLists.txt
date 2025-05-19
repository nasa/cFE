##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################


set(TESTCASE_SOURCES 
    "tbl_ut_helpers.c" # Useful helper functions for everything 
    "tbl_UT.c"         # The traditional "tbl_UT.c" has most of the test cases
    "tbl_ut_load.c"    # These focus on table loading - CFE_TBL_Load()
    "tbl_ut_support_passthru.c" # Support functions for passthru I/O (only option in TBL here)
)

# This relies on the updated add_cfe_coverage_test() for a couple features:
#  - Use of the keyword syntax allows multiple TESTCASE_SOURCES files, not just 1.
#  - We can pass in the tbl_SOURCES list from parent project as SUBJECT_SOURCES without scrubbing
add_cfe_coverage_test(tbl
    UNIT_NAME "ALL"
    TESTCASE_SOURCES ${TESTCASE_SOURCES}
    SUBJECT_SOURCES  ${tbl_SOURCES}
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-tbl-ALL-testrunner PRIVATE
    ${CFE_TBL_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-tbl-ALL-testrunner ut_core_private_stubs)


