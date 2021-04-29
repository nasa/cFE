##################################################################
#
# cFE resource ID module CMake build recipe
#
##################################################################

project(CFE_RESOURCEID C)

# Module library
set(resourceid_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/fsw/src/cfe_resourceid_api.c
)
add_library(resourceid STATIC ${resourceid_SOURCES})

target_link_libraries(resourceid PRIVATE core_private)
target_include_directories(resourceid PUBLIC fsw/inc)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(ut-coverage)
endif(ENABLE_UNIT_TESTS)
