##################################################################
#
# cFE Time Services (TIME) module CMake build recipe
#
##################################################################

project(CFE_TIME C)

# Time Services source files
set(time_SOURCES
    fsw/src/cfe_time_api.c
    fsw/src/cfe_time_task.c
    fsw/src/cfe_time_tone.c
    fsw/src/cfe_time_utils.c
    fsw/src/cfe_time_api.c
    fsw/src/cfe_time_task.c
    fsw/src/cfe_time_tone.c
    fsw/src/cfe_time_utils.c
)
add_library(time STATIC ${time_SOURCES})

target_include_directories(time PUBLIC fsw/inc)
target_link_libraries(time PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

cfs_app_check_intf(${DEP}
    cfe_time_events.h
    cfe_time_msg.h
)
