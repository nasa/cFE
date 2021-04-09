##################################################################
#
# cFE Inter-module interface
#
##################################################################


add_library(core_private INTERFACE)

# code compiled as CFE core internal will have _CFE_CORE_ macro set
target_compile_definitions(core_private INTERFACE _CFE_CORE_)

target_include_directories(core_private INTERFACE
    fsw/inc     # includes shared typedefs among CFE core apps, not called/used by apps
    ${CFE_SOURCE_DIR}/cmake/target/inc # allow CFE core apps to get "target_config.h" file
)

# also use all headers/definitions from public intf
target_link_libraries(core_private INTERFACE core_api)

# Add unit test coverage subdirectory
if (ENABLE_UNIT_TESTS)
    add_subdirectory(ut-stubs)
endif (ENABLE_UNIT_TESTS)

cfs_app_check_intf(core_private
    cfe_es_erlog_typedef.h
    cfe_evs_log_typedef.h
    cfe_es_resetdata_typedef.h
    cfe_sbr.h
    cfe_sb_destination_typedef.h
    cfe_es_perfdata_typedef.h
    cfe_core_resourceid_basevalues.h
)
