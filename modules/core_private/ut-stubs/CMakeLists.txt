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

#
# Create the generic stubs library
#
add_library(ut_${DEP}_stubs STATIC
    src/ut_osprintf_stubs.c
    src/ut_support.c
)

target_include_directories(ut_${DEP}_stubs PUBLIC inc)


# linking with the CFE stubs implies also linking
# with the OSAL and PSP stubs.  This is in line with
# how the real application is linked, in that cfe-core
# executable also provides OSAL and PSP functions.
target_link_libraries(ut_${DEP}_stubs
    core_private
    ut_core_api_stubs
    ut_psp-${CFE_SYSTEM_PSPNAME}_stubs
    ut_osapi_stubs
    ut_assert
)
