# This example toolchain file describes the cross compiler to use for
# the target architecture indicated in the configuration file.

SET(CFE_SYSTEM_PSPNAME   "generic-vxworks-rtp")

# The VxWorks toolchain relies on several environment variables,
# which should be set already by an environment setup script.

MESSAGE("CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

# WIND_HOME refers to the top-level installation directory
set(WIND_HOME_DFL "$ENV{WIND_SDK_HOME}")
if(NOT WIND_HOME_DFL)
  MESSAGE(FATAL_ERROR "WIND_SDK_HOME environment variable is not set. Run SDK setup script.")
endif(NOT WIND_HOME_DFL)
set(WIND_HOME "${WIND_HOME_DFL}" CACHE PATH "Wind River top level SDK directory")

set(VXWORKS_TOOLS_PREFIX "${WIND_HOME}/vxsdk/host/x86_64-linux")

# Add architecture defs
add_definitions("-DVX7_RTP")
add_definitions("-DVXWORKS_RTP")

set(VX_TARGET_TYPE RTP)

# specify the cross compiler - adjust accord to compiler installation
# This uses the compiler-wrapper toolchain that buildroot produces
SET(SDKHOSTBINDIR          "${VXWORKS_TOOLS_PREFIX}/bin")
set(TARGETSUFFIX           "${CMAKE_SYSTEM_PROCESSOR}")
set(VXWORKS_BSP_C_FLAGS     "-rtp")

# set C/C++ compiler
set(CMAKE_C_COMPILER wr-cc${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_CXX_COMPILER wr-c++${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_AR wr-ar${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_NM wr-nm${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_RANLIB wr-ranlib${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_OBJCOPY wr-objcopy${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_LINKER wr-ld${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_OBJDUMP wr-objdump${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_READELF wr-readelf${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_STRIP wr-strip${CMAKE_HOST_EXECUTABLE_SUFFIX})
set(CMAKE_SIZE wr-size${CMAKE_HOST_EXECUTABLE_SUFFIX})

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM   NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   ONLY)

SET(CMAKE_PREFIX_PATH                   /)

#
# This section accounts for a difference between the public SDKs and
# the Licensed SDK (older SR0650 release)
# The Licensed SDK defines WIND_CC_SYSROOT while the public SDKs do not
# It can be obtained by running the compiler if it is not defined
#
if(EXISTS $ENV{WIND_CC_SYSROOT})
    set(CMAKE_SYSROOT $ENV{WIND_CC_SYSROOT})
else()
    execute_process(COMMAND wr-cc${CMAKE_HOST_EXECUTABLE_SUFFIX} -print-sysroot
    OUTPUT_VARIABLE __WIND_CC_SYSROOT__
    OUTPUT_STRIP_TRAILING_WHITESPACE)

    # set cmake sysroot to VSB directory
    if(NOT __WIND_CC_SYSROOT__ STREQUAL "")
        set(CMAKE_SYSROOT ${__WIND_CC_SYSROOT__})
    else()
        message(FATAL_ERROR "VxWorks compiler sysroot not found. Please set WIND_CC_SYSROOT")
    endif()
endif()

# set RTP build flags to compiler
set(CMAKE_C_FLAGS_INIT "-rtp")

include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h/published/UTILS_UNIX)
include_directories(${WIND_HOME}/vxsdk/sysroot/share/h)
include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h)
include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h/system)
include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h/public)
