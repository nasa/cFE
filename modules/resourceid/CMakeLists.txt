##################################################################
#
# cFE resource ID module CMake build recipe
#
##################################################################

project(CFE_RESOURCEID C)

# Module library
set(resourceid_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/cfe_resourceid_api.c)
add_library(resourceid STATIC ${resourceid_SOURCES})

target_include_directories(resourceid PUBLIC inc)

# Add unit test coverage subdirectory
if(ENABLE_UNIT_TESTS)
    add_subdirectory(unit-test-coverage)
endif(ENABLE_UNIT_TESTS)
