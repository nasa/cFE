##################################################################
#
# cFE unit test build recipe
#
# This CMake file contains the recipe for building the cFE unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Unit test object library sources, options, and includes
add_library(ut_resourceid_OBJS OBJECT ${resourceid_SOURCES})
target_compile_options(ut_resourceid_OBJS PRIVATE
    ${UT_COVERAGE_COMPILE_FLAGS}
)
target_include_directories(ut_resourceid_OBJS PRIVATE
     $<TARGET_PROPERTY:${DEP},INCLUDE_DIRECTORIES>
)

# Add executable
add_executable(resourceid_UT test_cfe_resourceid.c $<TARGET_OBJECTS:ut_resourceid_OBJS>)

# allow direct use of the "option" headers no matter which is actually selected by mission
target_include_directories(resourceid_UT PRIVATE ${CFE_RESOURCEID_SOURCE_DIR}/option)

# Also add the UT_COVERAGE_LINK_FLAGS to the link command
# This should enable coverage analysis on platforms that support this
target_link_libraries(resourceid_UT
    ${UT_COVERAGE_LINK_FLAGS}
    ut_core_private_stubs
    ut_assert)

add_test(resourceid_UT resourceid_UT)
foreach(TGT ${INSTALL_TARGET_LIST})
    install(TARGETS resourceid_UT DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
endforeach()
