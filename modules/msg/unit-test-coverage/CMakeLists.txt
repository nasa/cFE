##################################################################
#
# cFE unit test build recipe
#
# This CMake file contains the recipe for building the cFE unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

# Unit test object library sources, options, and includes
add_library(ut_${DEP}_objs OBJECT ${${DEP}_SRC})
target_compile_options(ut_${DEP}_objs PRIVATE ${UT_COVERAGE_COMPILE_FLAGS})
target_include_directories(ut_${DEP}_objs PRIVATE
     $<TARGET_PROPERTY:${DEP},INCLUDE_DIRECTORIES>)

set (ut_${DEP}_tests
    msg_UT.c
    test_msg_utils.c
    test_msg_not.c
    test_msg_pri_not.c
    test_cfe_msg_init.c
    test_cfe_msg_ccsdspri.c
    test_cfe_msg_msgid_shared.c
    test_cfe_msg_checksum.c
    test_cfe_msg_fc.c
    test_cfe_msg_time.c
    $<TARGET_OBJECTS:ut_${DEP}_objs>)

# Add extended header tests if appropriate
if (MISSION_INCLUDE_CCSDSEXT_HEADER)
    list(APPEND ut_${DEP}_tests
        test_msg_ext_not.c
        test_cfe_msg_ccsdsext.c)
else (MISSION_INCLUDE_CCSDSEXT_HEADER)
    list(APPEND ut_${DEP}_tests
        test_msg_prionly.c)
endif (MISSION_INCLUDE_CCSDSEXT_HEADER)

# Add the correct message id test
if (MISSION_MSGID_V2)
    list(APPEND ut_${DEP}_tests
        test_cfe_msg_msgid_v2.c)
else (MISSION_MSGID_V2)
    list(APPEND ut_${DEP}_tests
        test_cfe_msg_msgid_v1.c)
endif (MISSION_MSGID_V2)

# Add executable
add_executable(${DEP}_UT ${ut_${DEP}_tests})

# Add include to get private defaults
target_include_directories(${DEP}_UT PRIVATE ../private_inc)

# Also add the UT_COVERAGE_LINK_FLAGS to the link command    
# This should enable coverage analysis on platforms that support this
target_link_libraries(${DEP}_UT
    ${UT_COVERAGE_LINK_FLAGS}
    ut_cfe-core_support
    ut_cfe-core_stubs
    ut_assert)

add_test(${DEP}_UT ${DEP}_UT)
foreach(TGT ${INSTALL_TARGET_LIST})
    install(TARGETS ${DEP}_UT DESTINATION ${TGT}/${UT_INSTALL_SUBDIR})
endforeach()
