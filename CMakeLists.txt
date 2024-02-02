##########################################################################
#
# Core Flight Software Mission top-level CMake build script
# This will build cFS for all target machine(s) defined by the mission
#
# Note that the target CPUs may use different architectures, therefore each
# architecture must be done as a separate sub-build since none of the binaries
# can be shared.
#
# This is actually two build scripts in one:
#   - A "top-level" script which divides the overall build by architecture
#       (This is run when TARGETSYSTEM is unset)
#   - An architecture-specific build that generates the binaries
#       (This is run when TARGETSYSTEM is set)
#
# This file implements the common operation sequence between the mission build
# and the architecture-specific sub build.  It relies on several functions
# that are implemented in a separate include files:
#
# initialize_globals:
#   This function sets up the basic global variables such as MISSION_SOURCE_DIR,
#   MISSIONCONFIG, ENABLE_UNIT_TESTS, SIMULATION and others.  These are the
#   basic variables that must exist _before_ the mission configuration is read.
#
# read_targetconfig:
#   Parse the information from targets.cmake and create the build lists.  Note
#   this function is common to both mission and arch-specific builds.
#
# prepare:
#   Use the information in the target config to set up additional variables
#   and satisfy any prerequisites for targets.  Most importantly this stage
#   is responsible for finding the actual location of all source files for apps
#   listed in the mission configuration, along with collecting any supplemental
#   sources, such as EDS files or additional compiler flags.
#
# process_arch:
#   This is called multiple times, once for each CPU architecture specified in
#   the main targets.cmake file.  At the mission level, this creates a sub
#   project target using the correct toolchain for cross compile.  In the arch
#   specific level (inside the sub-project) it generates the actual library and
#   executable targets.
#
#
##########################################################################

# Squelch a warning when building on Win32/Cygwin
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

# Add a path for any locally-supplied CMake modules
# These would typically be a part of any custom PSPs in use.
# (this is not required, and the directory can be empty/nonexistent)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../psp/cmake/Modules" ${CMAKE_MODULE_PATH})

# The minimum CMake version is chosen because v3.5.1 is what is
# available by default with Ubuntu 16.04 LTS at the time of development
# RHEL/CentOS users should install the "cmake3" package from EPEL repo
cmake_minimum_required(VERSION 3.5)

# This top-level file does not define ANY targets directly but we know
# that the subdirectories will at least use the "C" language, so
# indicate that now.  Doing this early initializes the CFLAGS
# so they won't change later.
# Note: this line defines the CFE_SOURCE_DIR variable.
project(CFE C)

# Allow unit tests to be added by any recipe
enable_testing()

# This switch determines whether to use EDS framework
# By default it is set OFF/false as this is a new/experimental feature.
option(CFE_EDS_ENABLED_BUILD "Use EDS framework" OFF)

# Always create directories to hold generated files/wrappers
# EDS makes signficant use of generated files.  In non-EDS builds 
# some headers and wrapper files are also generated.  Directories 
# may simply remain empty if not used/needed in the current config.
file(MAKE_DIRECTORY 
    "${CMAKE_BINARY_DIR}/eds"
    "${CMAKE_BINARY_DIR}/obj"
    "${CMAKE_BINARY_DIR}/inc"
    "${CMAKE_BINARY_DIR}/src"
)

# Include the global routines
include("cmake/global_functions.cmake")

# Load a sub-script that defines the other functions,
# depending on whether TARGETSYSTEM is defined or not
if (TARGETSYSTEM)
    # Arch-specific/CPU build mode -- use the "arch_build" implementation
    set(IS_CFS_ARCH_BUILD TRUE)
    include("cmake/arch_build.cmake")
else (TARGETSYSTEM)
    # Host System/Top Level build mode -- use the "mission_build" implementation
    set(IS_CFS_MISSION_BUILD TRUE)
    include("cmake/mission_build.cmake")
endif (TARGETSYSTEM)

# Call the initialization function defined by the sub-script
# This is implemented differently depending on whether this is a
# top-level or arch-specific build
initialize_globals()

# Load the target configuration information (used by all builds)
# This is at the top level so all vars set in here will become globals.
# The "defaults" file is included first, which the user-supplied targets
# file may override as necessary.
include("cmake/mission_defaults.cmake")
include(${MISSION_DEFS}/targets.cmake)

# Scan the list of targets and organize by target system type.
read_targetconfig()

# Include global-scope build customization
# Note if this feature is used it should only set basic options
# that have wide support (e.g. add_definitions).  It should not
# set anything target or machine specific.
include("${MISSION_DEFS}/global_build_options.cmake" OPTIONAL)

# Additionally the target mission might require additional
# custom build steps or override some routines.  In particular
# some architectures might need some special installation steps
# The custom script may override functions such as the
# cfe_exec_do_install() and cfe_app_do_install() functions for this
if (IS_CFS_ARCH_BUILD)
    include("${MISSION_DEFS}/arch_build_custom.cmake" OPTIONAL)
    include("${MISSION_DEFS}/arch_build_custom_${TARGETSYSTEM}.cmake" OPTIONAL)
elseif (IS_CFS_MISSION_BUILD)
    include("${MISSION_DEFS}/mission_build_custom.cmake" OPTIONAL)
endif (IS_CFS_ARCH_BUILD)

# Call the prepare function defined by the sub-script
# This is implemented differently depending on whether this is a
# top-level or arch-specific build
prepare()

# Call the process_arch macro for each architecture
foreach(SYSVAR ${TGTSYS_LIST})
  process_arch(${SYSVAR})
endforeach(SYSVAR ${TGTSYS_LIST})
