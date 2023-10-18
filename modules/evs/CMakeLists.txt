##################################################################
#
# cFE Event Services (EVS) module CMake build recipe
#
##################################################################

project(CFE_EVS C)

# Event services source files
set(evs_SOURCES
    fsw/src/cfe_evs.c
    fsw/src/cfe_evs_log.c
    fsw/src/cfe_evs_task.c
    fsw/src/cfe_evs_utils.c
    fsw/src/cfe_evs_dispatch.c
)
add_library(evs STATIC ${evs_SOURCES})

target_include_directories(evs PUBLIC fsw/inc)
target_link_libraries(evs PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    cfe_evs_eventids.h
    cfe_evs_extern_typedefs.h
    cfe_evs_fcncodes.h
    cfe_evs_interface_cfg.h
    cfe_evs_internal_cfg.h
    cfe_evs_mission_cfg.h
    cfe_evs_msgdefs.h
    cfe_evs_msg.h
    cfe_evs_msgids.h
    cfe_evs_msgstruct.h
    cfe_evs_platform_cfg.h
    cfe_evs_topicids.h
)
