##################################################################
#
# cFE unit test CMake build recipe
#
##################################################################

set(UT_SOURCES)
foreach(SRC ${config_SOURCES})
    list(APPEND UT_SOURCES "${CFE_CONFIG_SOURCE_DIR}/${SRC}")
endforeach()

add_cfe_coverage_stubs(cfe_config_internal
    ut-stubs/cfe_config_init_stubs.c
    ut-stubs/cfe_config_external_stubs.c
    ut-stubs/cfe_config_lookup_stubs.c
    ut-stubs/cfe_config_set_stubs.c
)

target_include_directories(coverage-cfe_config_internal-stubs PUBLIC ../fsw/inc ../fsw/src)
target_link_libraries(coverage-cfe_config_internal-stubs ut_core_api_stubs ut_core_private_stubs)

add_cfe_coverage_test(config ALL
    "test_cfe_config.c"
    "${UT_SOURCES}"
)

add_cfe_coverage_dependency(config ALL cfe_config_internal)
