##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################

foreach(SRC ${es_SOURCES})
    list(APPEND UT_SOURCES "${CFE_ES_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_test(es ALL
    "es_UT.c"
    "${UT_SOURCES}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-es-ALL-testrunner PRIVATE
    ${CFE_ES_SOURCE_DIR}/fsw/src
)

target_link_libraries(coverage-es-ALL-testrunner ut_core_private_stubs)

