##################################################################
#
# cFE Table Services (TBL) module CMake build recipe
#
##################################################################

project(CFE_TBL C)

# Table Services source files
set(tbl_SOURCES
    fsw/src/cfe_tbl_api.c
    fsw/src/cfe_tbl_internal.c
    fsw/src/cfe_tbl_resource.c
    fsw/src/cfe_tbl_transaction.c
    fsw/src/cfe_tbl_task.c
    fsw/src/cfe_tbl_task_cmds.c
    fsw/src/cfe_tbl_dispatch.c
)
add_library(tbl STATIC ${tbl_SOURCES})

target_include_directories(tbl PUBLIC fsw/inc)
target_link_libraries(tbl PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    cfe_tbl_eventids.h
    cfe_tbl_extern_typedefs.h
    cfe_tbl_fcncodes.h
    cfe_tbl_interface_cfg.h
    cfe_tbl_internal_cfg.h
    cfe_tbl_mission_cfg.h
    cfe_tbl_msgdefs.h
    cfe_tbl_msg.h
    cfe_tbl_msgids.h
    cfe_tbl_msgstruct.h
    cfe_tbl_platform_cfg.h
    cfe_tbl_topicids.h
)
