##################################################################
#
# cFE stub function build recipe
#
# This CMake file contains the recipe for building the stub function
# libraries that correlate with the CFE public API.  This library supports
# unit testing of OTHER modules, where the test cases for those modules
# are linked with the stubs supplied here, rather than the normal CFE.
#
##################################################################

# Reference the UT assert include directory
include_directories(${osal_MISSION_DIR}/ut_assert/inc)

#
# Create the generic stubs library
#
add_library(ut_cfe-core_stubs STATIC 
    ut_es_stubs.c
    ut_evs_stubs.c
    ut_msg_stubs.c
    ut_sb_stubs.c
    ut_tbl_stubs.c
    ut_time_stubs.c
    ut_fs_stubs.c)

# linking with the CFE stubs implies also linking
# with the OSAL and PSP stubs.  This is in line with
# how the real application is linked, in that cfe-core
# executable also provides OSAL and PSP functions.
target_link_libraries(ut_cfe-core_stubs
    ut_psp-${CFE_SYSTEM_PSPNAME}_stubs
    ut_osapi_stubs
)
