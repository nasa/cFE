##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################


foreach(SRC ${fs_SOURCES})
    list(APPEND UT_SOURCES "${CFE_FS_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_test(fs ALL
    "fs_UT.c"
    "${UT_SOURCES}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-fs-ALL-testrunner PRIVATE
    ${CFE_FS_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-fs-ALL-testrunner ut_core_private_stubs)

