##################################################################
#
# cFE Software Bus (SB) module CMake build recipe
#
##################################################################

project(CFE_SB C)

# Software Bus source files
set(sb_SOURCES
    fsw/src/cfe_sb_api.c
    fsw/src/cfe_sb_buf.c
    fsw/src/cfe_sb_init.c
    fsw/src/cfe_sb_msg_id_util.c
    fsw/src/cfe_sb_priv.c
    fsw/src/cfe_sb_dispatch.c
    fsw/src/cfe_sb_task.c
    fsw/src/cfe_sb_util.c
)
add_library(sb STATIC ${sb_SOURCES})

target_include_directories(sb PUBLIC fsw/inc)
target_link_libraries(sb PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    cfe_sb_eventids.h
    cfe_sb_extern_typedefs.h
    cfe_sb_fcncodes.h
    cfe_sb_interface_cfg.h
    cfe_sb_internal_cfg.h
    cfe_sb_mission_cfg.h
    cfe_sb_msgdefs.h
    cfe_sb_msg.h
    cfe_sb_msgids.h
    cfe_sb_msgstruct.h
    cfe_sb_platform_cfg.h
    cfe_sb_topicids.h
)
