##################################################################
#
# cFE Task Affinity (TA) module CMake build recipe
#
##################################################################

project(CFE_TA C)

include_directories(fsw/src)

# Task Affinity source files
# Notice: No CPU_AFFINITY file is listed here
# The table is now compiled by the Target Executable via the config module.
set(ta_SOURCES
    fsw/src/cfe_ta_task.c
    fsw/src/cfe_ta_objtab.c
)

if (CFE_EDS_ENABLED)
  # Use EDS-based message dispatch
  list(APPEND ta_SOURCES
    fsw/src/cfe_ta_eds_dispatch.c
  )
else()
  # Use hardcoded message dispatch
  list(APPEND ta_SOURCES
    fsw/src/cfe_ta_dispatch.c
  )
endif()

add_library(ta STATIC ${ta_SOURCES})

target_include_directories(ta PUBLIC fsw/inc)
target_link_libraries(ta PRIVATE core_private)

# Add unit test coverage subdirectory ONLY if TA is in the core modules list
if(ENABLE_UNIT_TESTS AND "ta" IN_LIST MISSION_CORE_MODULES)
    add_subdirectory(ut-coverage)
endif()

cfs_app_check_intf(${DEP}
    cfe_ta_msg.h
    cfe_ta_fcncodes.h
    cfe_ta_interface_cfg.h
    cfe_ta_internal_cfg.h
    cfe_ta_mission_cfg.h
    cfe_ta_msgdefs.h
    cfe_ta_msgids.h
    cfe_ta_msgstruct.h
    cfe_ta_platform_cfg.h
    cfe_ta_eventids.h
    cfe_ta_topicids.h
)
