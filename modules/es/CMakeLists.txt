##################################################################
#
# cFE Executive Services (ES) module CMake build recipe
#
##################################################################

project(CFE_ES C)

# Executive services source files
set(es_SOURCES
    fsw/src/cfe_es_api.c
    fsw/src/cfe_es_apps.c
    fsw/src/cfe_es_backgroundtask.c
    fsw/src/cfe_es_cds.c
    fsw/src/cfe_es_cds_mempool.c
    fsw/src/cfe_es_crc.c
    fsw/src/cfe_es_dispatch.c
    fsw/src/cfe_es_erlog.c
    fsw/src/cfe_es_generic_pool.c
    fsw/src/cfe_es_mempool.c
    fsw/src/cfe_es_objtab.c
    fsw/src/cfe_es_perf.c
    fsw/src/cfe_es_resource.c
    fsw/src/cfe_es_start.c
    fsw/src/cfe_es_syslog.c
    fsw/src/cfe_es_task.c
)
add_library(es STATIC ${es_SOURCES})

target_include_directories(es PUBLIC fsw/inc)
target_link_libraries(es PRIVATE core_private)

# Add unit test coverage subdirectory
if (ENABLE_UNIT_TESTS)
  add_subdirectory(ut-coverage)
endif (ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    cfe_es_eventids.h
    cfe_es_extern_typedefs.h
    cfe_es_fcncodes.h
    cfe_es_interface_cfg.h
    cfe_es_internal_cfg.h
    cfe_es_mission_cfg.h
    cfe_es_msgdefs.h
    cfe_es_msg.h
    cfe_es_msgids.h
    cfe_es_msgstruct.h
    cfe_es_platform_cfg.h
    cfe_es_topicids.h
  )
