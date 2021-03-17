##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################


foreach(SRC ${evs_SOURCES})
    list(APPEND UT_SOURCES "${CFE_EVS_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_test(evs ALL
    "evs_UT.c"
    "${UT_SOURCES}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-evs-ALL-testrunner PRIVATE
    ${CFE_EVS_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-evs-ALL-testrunner ut_core_private_stubs)

