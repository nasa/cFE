##################################################################
#
# cFE Table Services (TBL) module CMake build recipe
#
##################################################################

project(CFE_TBL C)

# Table Services source files
set(tbl_SOURCES
    fsw/src/cfe_tbl_api.c
    fsw/src/cfe_tbl_accdesc.c
    fsw/src/cfe_tbl_load.c
    fsw/src/cfe_tbl_loadbuff.c
    fsw/src/cfe_tbl_registry.c
    fsw/src/cfe_tbl_regrec.c
    fsw/src/cfe_tbl_dump.c
    fsw/src/cfe_tbl_dumpctl.c
    fsw/src/cfe_tbl_handlelink.c
    fsw/src/cfe_tbl_valresult.c
    fsw/src/cfe_tbl_internal.c
    fsw/src/cfe_tbl_transaction.c
    fsw/src/cfe_tbl_task.c
    fsw/src/cfe_tbl_task_cmds.c
)

if (CFE_EDS_ENABLED)
  # Use EDS-based message dispatch and codec
  list(APPEND tbl_SOURCES fsw/src/cfe_tbl_eds_codec.c fsw/src/cfe_tbl_eds_dispatch.c)
else()
  # Use hardcoded message dispatch and passthru codec
  list(APPEND tbl_SOURCES fsw/src/cfe_tbl_passthru_codec.c fsw/src/cfe_tbl_dispatch.c)
endif()

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
