##################################################################
#
# cFS version metadata collection script
#
# This small script runs at build time (as opposed to prep time)
# and is intended to extract information about the current
# build environment - this may change after initial makefile creation
#
##################################################################

# All 3 of these may be passed via environment variables to force a particular
# date, user, or hostname i.e. if hoping to reproduce an exact binary of a prior build
# They are free-form strings, will be built/linked into the final CONFIGDATA object.

# Get the current date and time
set(BUILDDATE $ENV{BUILDDATE})
if (NOT BUILDDATE)
    execute_process(
        COMMAND date "+%Y%m%d%H%M"
        OUTPUT_VARIABLE BUILDDATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif(NOT BUILDDATE)

# Get the build host
set(BUILDHOST $ENV{HOSTNAME})
if (NOT BUILDHOST)
    execute_process(
        COMMAND hostname
        OUTPUT_VARIABLE BUILDHOST
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (NOT BUILDHOST)

# Get the user ID
set(BUILDUSER $ENV{USER})
if (NOT BUILDUSER)
    execute_process(
        COMMAND whoami
        OUTPUT_VARIABLE BUILDUSER
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
endif (NOT BUILDUSER)

# Use configure_file() command to generate the final output file because this can detect
# and only update it if it changes.
set(CFE_KEYVALUE_TABLE_NAME "CFE_BUILD_ENV_TABLE")
configure_file(${BIN}/cfe_build_env.in ${BIN}/src/cfe_build_env_table.c @ONLY)
