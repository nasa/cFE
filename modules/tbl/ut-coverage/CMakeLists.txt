##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################


foreach(SRC ${tbl_SOURCES})
    list(APPEND UT_SOURCES "${CFE_TBL_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_test(tbl ALL
    "tbl_UT.c"
    "${UT_SOURCES}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-tbl-ALL-testrunner PRIVATE
    ${CFE_TBL_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-tbl-ALL-testrunner ut_core_private_stubs)


