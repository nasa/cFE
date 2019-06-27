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
#   and satisfy any preequisites for targets.  Most importantly this stage
#   is reposible for finding the actual location of all source files for apps
#   listed in the mission configuration, along with collecting any supplemental
#   sources, such as EDS files or additional compiler flags. 
#   
# process_arch:
#   This is called multiple times, once for each CPU architecture specfied in
#   the main targets.cmake file.  At the mission level, this creates a sub
#   project target using the correct toolchain for cross compile.  In the arch
#   specific level (inside the sub-project) it generates the actual library and
#   executable targets.
#
# 
##########################################################################

# Squelch a warning when building on Win32/Cygwin
set(CMAKE_LEGACY_CYGWIN_WIN32 0)

# The minimum CMake version is chosen because 2.6.4 is what is
# included by default with RHEL/Centos 5.x
cmake_minimum_required(VERSION 2.6.4)

# This top-level file does not define ANY targets directly but we know
# that the subdirectories will at least use the "C" language, so
# indicate that now.  Doing this early initializes the CFLAGS
# so they won't change later.
project(CFETOP C)

# Allow unit tests to be added by any recipe
enable_testing()

# Include the global routines
include("cmake/global_functions.cmake")

# Load a sub-script that defines the other functions, 
# depending on whether TARGETSYSTEM is defined or not
if (TARGETSYSTEM)
    # Arch-specific/CPU build mode -- use the "arch_build" implementation
    include("cmake/arch_build.cmake")
else (TARGETSYSTEM)
    # Host System/Top Level build mode -- use the "mission_build" implementation
    include("cmake/mission_build.cmake")
endif (TARGETSYSTEM)

# Call the initialization function defined by the sub-script
# This is implemented differently depending on whether this is a 
# top-level or arch-specific build
initialize_globals()

# Load the target configuration information (used by all builds)
# This is at the top level so all vars set in here will become globals.
include(${MISSION_DEFS}/targets.cmake)
    
# Scan the list of targets and organize by target system type.
read_targetconfig()

# Define preprocessor directive(s) which can be used to 
# gate conditionally compiled code when using these CMake scripts.  
# Macro defined here at the topmost level will be defined in all targets 
# on all processors. These macros can preserve compatibility with the old makefiles 
# such that the code can still be built using the old method without the new features.
add_definitions("-D_ENHANCED_BUILD_")

# Call the prepare function defined by the sub-script
# This is implemented differently depending on whether this is a 
# top-level or arch-specific build
prepare()

# Call the process_arch macro for each architecture
foreach(SYSVAR ${TGTSYS_LIST})
  process_arch(${SYSVAR})
endforeach(SYSVAR IN LISTS TGTSYS_LIST)

