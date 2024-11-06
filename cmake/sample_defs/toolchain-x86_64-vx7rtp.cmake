# This example toolchain file describes the cross compiler to use for
# the target architecture indicated in the configuration file.

# Basic cross system configuration
set(CMAKE_SYSTEM_NAME VxWorks-RTP-CFE)
set(CMAKE_SYSTEM_PROCESSOR x86_64)
set(CMAKE_SYSTEM_VERSION 7)

SET(OSAL_SYSTEM_OSTYPE   "posix")
SET(OSAL_SYSTEM_BSPTYPE  "generic-vxworks-rtp")
SET(CFE_SYSTEM_PSPNAME   "generic-vxworks-rtp")

# The VxWorks toolchain relies on several environment variables,
# which should be set already by an environment setup script.

MESSAGE("Toolchain Selected: x86_64-vx7rtp")
MESSAGE("CMAKE_SYSTEM_NAME = ${CMAKE_SYSTEM_NAME}")

# WIND_HOME refers to the top-level installation directory
set(WIND_HOME_DFL "$ENV{WIND_SDK_HOME}")
if(NOT WIND_HOME_DFL)
  set(WIND_HOME_DFL "$ENV{HOME}/opt/wrsdk-vxworks7-qemu")
endif(NOT WIND_HOME_DFL)
set(WIND_HOME "${WIND_HOME_DFL}" CACHE PATH "Wind River top level SDK directory")

MESSAGE("WIND_HOME_DFL = ${WIND_HOME_DFL}")
MESSAGE("WIND_HOME = ${WIND_HOME}")

set(VXWORKS_TOOLS_PREFIX "${WIND_HOME}/vxsdk/host/x86_64-linux")

# This port is for kernel mode
# ADD_DEFINITIONS(-D_WRS_KERNEL)

# Add architecture defs
add_definitions("-D__x86_64__")
add_definitions("-DX86QEMU")
add_definitions("-DVX7_RTP")
add_definitions("-DVXWORKS_RTP")

# set(VX_TARGET_TYPE DKM)
set(VX_TARGET_TYPE RTP)

# The following is from the VxWorks Cmakefiles
# disable compiler check when bulid dkm
set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

# specify the cross compiler - adjust accord to compiler installation
# This uses the compiler-wrapper toolchain that buildroot produces
SET(SDKHOSTBINDIR          "${VXWORKS_TOOLS_PREFIX}/bin")
set(TARGETSUFFIX           "${CMAKE_SYSTEM_PROCESSOR}")
# set(VXWORKS_BSP_C_FLAGS   "-dkm")
set(VXWORKS_BSP_C_FLAGS     "-rtp")

# set C/C++ compiler
set(CMAKE_C_COMPILER wr-cc${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE STRING "" FORCE)
set(CMAKE_CXX_COMPILER wr-c++${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE STRING "" FORCE)

set(CMAKE_AR wr-ar${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_NM wr-nm${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_RANLIB wr-ranlib${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_OBJCOPY wr-objcopy${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_LINKER wr-ld${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_OBJDUMP wr-objdump${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_READELF wr-readelf${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_STRIP wr-strip${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)
set(CMAKE_SIZE wr-size${CMAKE_HOST_EXECUTABLE_SUFFIX} CACHE FILEPATH "" FORCE)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM   NEVER)

# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY   ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE   ONLY)

SET(CMAKE_PREFIX_PATH                   /)


execute_process(COMMAND wr-cc${CMAKE_HOST_EXECUTABLE_SUFFIX} -print-sysroot
    OUTPUT_VARIABLE __WIND_CC_SYSROOT__
    OUTPUT_STRIP_TRAILING_WHITESPACE)

# set cmake sysroot to VSB directory
if(NOT __WIND_CC_SYSROOT__ STREQUAL "")
    set(CMAKE_SYSROOT ${__WIND_CC_SYSROOT__})
elseif(EXISTS $ENV{WIND_CC_SYSROOT})
    set(CMAKE_SYSROOT $ENV{WIND_CC_SYSROOT})
else()
    message(FATAL_ERROR "VxWorks compiler sysroot not found. Please set WIND_CC_SYSROOT")
endif()

# set DKM build flags to compiler
set(CMAKE_C_FLAGS_INIT "-rtp")
# set(CMAKE_ASM_FLAGS_INIT "-dkm")
# set(CMAKE_CXX_FLAGS_INIT "-dkm")

# Add install and staging prefix to find root path to ensure
# find_xxx() could search from them

if (NOT CMAKE_FIND_NO_INSTALL_PREFIX)
    list(APPEND __VXWORKS_PREFIX_PATH__
        # Project install destination.
        "${CMAKE_INSTALL_PREFIX}"
    )
    if(CMAKE_STAGING_PREFIX)
        list(APPEND __VXWORKS_PREFIX_PATH__
        # User-supplied staging prefix.
        "${CMAKE_STAGING_PREFIX}")
    endif()
endif()

if (VX_TARGET_TYPE STREQUAL "DKM")
   include_directories(${WIND_HOME}/vxsdk/sysroot/krnl/h/public)
   include_directories(${WIND_HOME}/vxsdk/sysroot/krnl/h/published/UTILS_UNIX)
   include_directories(${WIND_HOME}/vxsdk/sysroot/share/h)
   include_directories(${WIND_HOME}/compilers/llvm-17.0.6.1/LINUX64/lib/clang/17/include)
endif()

if (VX_TARGET_TYPE STREQUAL "RTP")
   include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h/published/UTILS_UNIX)
   include_directories(${WIND_HOME}/vxsdk/sysroot/share/h)
   include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h)
   include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h/system)
   include_directories(${WIND_HOME}/vxsdk/sysroot/usr/h/public)
endif()


