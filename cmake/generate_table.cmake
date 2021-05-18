##################################################################
#
# Sub-script to generate a table file via elf2cfetbl
#
# This small script runs at build time (as opposed to prep time)
# which converts a static library (.a) with a single object into a
# table (.tbl) file
#
##################################################################

#
# Required passed in values:
#  CMAKE_AR => path to "ar" utility for working with static lib files
#  TBLTOOL => path to "elf2cfetbl" utility
#  LIB => name of library file to convert
#
# This assumes/requires that the static library has a single object in it.
# Note, In newer versions of CMake an object library can be used. This workaround
# is intended to also be compatible with older CMake versions.
#

# First run "ar t" to get the object file name, there should be only 1 file.
execute_process(COMMAND ${CMAKE_AR} t "${LIB}"
    OUTPUT_VARIABLE OBJNAME
    RESULT_VARIABLE RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Failure running ${CMAKE_AR} t ${LIB}")
endif()

# Next run "ar x" to extract that file.
execute_process(COMMAND ${CMAKE_AR} x "${LIB}" "${OBJNAME}"
    RESULT_VARIABLE RESULT
)
if (NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Failure running ${CMAKE_AR} x ${LIB} ${OBJNAME}")
endif()

# Finally invoke the table tool (elf2cfetbl) on the object
message("Executing Process: ${TBLTOOL} ${OBJNAME}")
execute_process(COMMAND ${TBLTOOL} "${OBJNAME}"
    RESULT_VARIABLE RESULT
)
if (NOT RESULT EQUAL 0)
    message(FATAL_ERROR "Failure running ${TBLTOOL}")
endif()

message("Successfully converted ${LIB} to a CFE table")
