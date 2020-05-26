
# Create the static module
add_library(exthdr_msgformat STATIC 
    src/exthdr_msgformat.c
)

target_include_directories(exthdr_msgformat PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
  
# If UT is enabled, then add the tests from the subdirectory
if (ENABLE_UNIT_TESTS)
#  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)

