##################################################################
#
# cFE File Services (FS) module CMake build recipe
#
##################################################################

project(CFE_FS C)

# File services source files
set(fs_SOURCES
    fsw/src/cfe_fs_api.c
    fsw/src/cfe_fs_priv.c
    fsw/src/cfe_fs_api.c
    fsw/src/cfe_fs_priv.c
)
add_library(fs STATIC ${fs_SOURCES})

target_include_directories(fs PUBLIC fsw/inc)
target_link_libraries(fs PRIVATE core_private)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)

