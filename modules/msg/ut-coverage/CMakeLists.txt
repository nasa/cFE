##################################################################
#
# cFE unit test build recipe
#
# This CMake file contains the recipe for building the cFE unit tests.
# It is invoked from the parent directory when unit tests are enabled.
#
##################################################################

set(UNIT_SRCS)
foreach(SRC ${${DEP}_SRC})
    get_filename_component(UNITNAME "${SRC}" NAME)
    list(APPEND UNIT_SRCS "../${SRC}")
endforeach()

set (ut_${DEP}_tests
    msg_UT.c
    test_msg_not.c
    test_msg_pri_not.c
    test_cfe_msg_init.c
    test_cfe_msg_integrity.c
    test_cfe_msg_ccsdspri.c
    test_cfe_msg_msgid_shared.c
    test_cfe_msg_checksum.c
    test_cfe_msg_fc.c
    test_cfe_msg_time.c)

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


add_cfe_coverage_test(${DEP} ALL
    "msg_UT.c;${ut_${DEP}_tests}"  # This list needs to be passed as a single argument
    "${UNIT_SRCS}"
)

# This permits UT test cases to directly access private headers in the fsw/src dir
target_include_directories(coverage-${DEP}-ALL-testrunner PRIVATE
    ../fsw/src
)

target_link_libraries(coverage-${DEP}-ALL-testrunner ut_core_private_stubs)
