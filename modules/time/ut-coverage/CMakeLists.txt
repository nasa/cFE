##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################


foreach(SRC ${time_SOURCES})
    list(APPEND UT_SOURCES "${CFE_TIME_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_test(time ALL
    "time_UT.c"
    "${UT_SOURCES}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-time-ALL-testrunner PRIVATE
    ${CFE_TIME_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-time-ALL-testrunner ut_core_private_stubs)


