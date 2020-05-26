
# Create the static module
add_library(ccsds_msgformat STATIC 
    src/ccsds_msgformat.c
)

target_include_directories(ccsds_msgformat PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
)
  
# If UT is enabled, then add the tests from the subdirectory
if (ENABLE_UNIT_TESTS)
#  add_subdirectory(unit-test)
endif (ENABLE_UNIT_TESTS)

