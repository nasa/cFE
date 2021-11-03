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
add_library(ut_core_api_stubs STATIC
    src/cfe_config_handlers.c
    src/cfe_config_stubs.c
    src/cfe_es_handlers.c
    src/cfe_es_stubs.c
    src/cfe_evs_handlers.c
    src/cfe_evs_stubs.c
    src/cfe_fs_handlers.c
    src/cfe_fs_stubs.c
    src/cfe_msg_handlers.c
    src/cfe_msg_stubs.c
    src/cfe_resourceid_handlers.c
    src/cfe_resourceid_stubs.c
    src/cfe_sb_handlers.c
    src/cfe_sb_stubs.c
    src/cfe_tbl_handlers.c
    src/cfe_tbl_stubs.c
    src/cfe_time_handlers.c
    src/cfe_time_stubs.c
)

# Define _CFE_CORE_ within stubs to also reveal internal APIs in header
target_compile_definitions(ut_core_api_stubs PRIVATE _CFE_CORE_)

# linking with the CFE stubs implies also linking
# with the OSAL and PSP stubs.  This is in line with
# how the real application is linked, in that cfe-core
# executable also provides OSAL and PSP functions.
target_link_libraries(ut_core_api_stubs
    core_api
    ut_psp-${CFE_SYSTEM_PSPNAME}_stubs
    ut_osapi_stubs
    ut_assert
)
