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
    fsw/src/cfe_tbl_task.c
    fsw/src/cfe_tbl_task_cmds.c
    fsw/src/cfe_tbl_api.c
    fsw/src/cfe_tbl_internal.c
    fsw/src/cfe_tbl_task.c
    fsw/src/cfe_tbl_task_cmds.c
)
add_library(tbl STATIC ${tbl_SOURCES})

target_include_directories(tbl PUBLIC fsw/inc)
target_link_libraries(tbl PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    cfe_tbl_msg.h
    cfe_tbl_events.h
)
