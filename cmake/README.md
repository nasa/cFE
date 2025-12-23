# Using CMake Build System

This is intended to be a general **how to** guide for using the cFS CMake-based build scripts and adapting to your project.

Also see [OpenSatKit](https://github.com/OpenSatKit/OpenSatKit) and [CFS-101](https://github.com/nasa/CFS-101) for more in-depth guides. 


## Quick Start+

The cFS Bundle documents quick set-up, build, and run:

- [Setup](https://github.com/nasa/cFS#setup)

- [Build and Run](https://github.com/nasa/cFS#build-and-run)

- [Send commands and Receive Telemetry](https://github.com/nasa/cFS#send-commands-receive-telemetry)

After install, the following directories will be populated:

- ``build/exe/host`` contains programs to be deployed to the ground
     station (or ground-station like machine).

- ``build/exe/cpu1`` contains a file system tree suitable for
     deployment to the embedded target.

## Prerequisites

The build uses 3rd party applications and libraries to provide some base functionality. For libraries the "-dev" (Debian/Ubuntu) or "-devel" (RedHat/CentOS) versions of these packages should be installed in order to compile source code using these libraries.

- gcc, make, libc-dev: Standard prerequisites for any build environment. Ubuntu provides a meta package called "build-essential" that depends on all the basics.

- For Ubuntu 64-bit Host machine you may need to install the multilib tools for 32-bit compatibility: `sudo apt-get install gcc-multilib g++-multilib`
- cmake: The build system uses this. Any recent version should work.
- libexpat (XML parser) - Although this is not required by the current build, a future CFE build using electronic data sheets will require this. Any recent version should work.
- git: Required if you want to access the official CFS distribution directly from the CFS community source repositories.

## Mission Configuration

Before any builds can be done, the mission configuration must be created. This is centralized in a directory below the top level source directory. The directory is identified by a _defs suffix in the directory name.

The example build provides a sample\_defs directory that can be modified to suit the project needs. This directory contains:

- `targets.cmake`: describes the target architecture and apps to use on each CPU (important)
Global configuration file to serve as cfe_mission_cfg.h
- `osconfig.h`: OS-specific files for OSAL as required by the mission
- `cfe_platform_cfg.h`: Platform-specific files as required by the mission
- `cfe_es_startup.scr`: Startup scripts for each CPU as required by the mission

Other project-specific files, such as table source files or generic data files can also be placed in the sample\_defs directory.

### Multiple "defs" directories 

The build scripts support using more than one "defs" directory within a single project; this is intended to support different combinations of processor-in-the-loop and hardware-in-the-loop debugging environments. The appropriate "defs" directory must be specified by the user at "preparation" time. This advanced use case will be detailed later.

To get started using the provided sample mission configuration as a baseline, the directory must be copied out to the top-level source directory. It cannot be used directly from the repository distribution location. This is intentional, since these files must be modified to suit a particular mission, any such modifications should be done to a mission-owned copy and not to the original files distributed with the source.

For example:

```bash
cd $HOME/my-mission-dir
cp -r cfe/cmake/sample_defs .
At this point, it is also a good idea to rename the directory from sample_defs to something more mission-specific. Note that when the top-level directory is renamed, the sample_mission_cfg.h file within the directory must also be renamed as well.
```

### Examples
An extremely simple targets.cmake file to build the core flight executive and the sample_app for a single CPU:

```cmake
SET(MISSION_NAME "SampleMission")
SET(SPACECRAFT_ID 42)

SET(TGT1_NAME cpu1)
SET(TGT1_APPLIST sample_app)
SET(TGT1_FILELIST cfe_es_startup.scr)
```

This file sets up CMake variables that have special meaning to the build scripts:

- `MISSION_NAME` and `SPACECRAFT_ID` are compiled into the executable and potentially used at run time by any application. In particular, the value specified for `SPACECRAFT_ID` will be returned by `CFE_PSP_GetSpacecraftId()`.
- `TGT1_NAME` provides a name for the first CPU. The build scripts will also look for a cross-compiler toolchain file by the same name, in this case toolchain-cpu1.cmake, to use for building the binaries used on this CPU. Important: The toolchain can also be specified by the `TGT1_PLATFORM` variable, which will override this if present. This allows the name and toolchain to specified independently.
- `TGT1_APPLIST` provides a space-separated list of applications to build for the target CPU. The application source code must be in a directory of the same name in one of the search path locations in the build scripts. These paths will be searched to find the source code for this application.
- `TGT1_FILELIST` is a space-separated list of files to copy to the target during the make install process. Note: Name clashes are common here, since all targets typically use a cfe_es_startup.scr to start the applications. This is handled by allowing a prefix to the filename to indicate which cpu the file belongs to. For instance, cpu1_cfe_es_startup.scr will be copied to cpu1 as cfe_es_startup.scr during the installation step.

## Preparing to Build CFE/CFS

The build procedure always uses "out-of-tree" builds; all generated files are stored in a separate directory from the source code. This is important for version control purposes, as the build will not modify or add any files within the source tree, and multiple builds can be done from a single source tree.

A special prep target is implemented in the example top-level makefile to facilitate setting up a new build directory. This in turn calls "cmake" with the right options to set up the build.

Special variables used by the top-level makefile wrapper:

- `O`: Specifies the output directory, may be absolute or relative. This allows all the builds to coexist simultaneously from the same source tree. If `O` is not specified then "build" is assumed.

- `SIMULATION`: Overrides the architecture(s) specified in the targets file. Setting "SIMULATION=native" will indicate to use the host machine's native compiler (e.g. /usr/bin/gcc) for all compilation tasks. For any other keyword/string, the scripts will search for a file called "toolchain-KEYWORD.cmake" that specifies the details of the toolchain. The sample_defs directory includes "i686-rtems4.11", "powerpc-440-linux-gnu" and "arm-cortexa8_neon-linux-gnueabi" as examples of how this works.

### Examples

To prepare a normal build for the target hardware:

```bash
make prep
```

To prepare a workstation simulation build (this overrides cross-compilation such that all binaries are built to run directly on the native host platform):

```bash
make SIMULATION=native O=build-sim prep
```

To prepare a "processor-in-the-loop" simulation build, assuming that the target processor toolchain is defined by "arm-cortexa8_neon-linux-gnueabi":

```bash
make SIMULATION=arm-cortexa8_neon-linux-gnueabi O=build-pil prep
```

For all builds, once the prep step has been completed, the variables used (such as "SIMULATION") are cached and therefore DO NOT need to be passed with any future invocation of the "make" commands. This allows the makefiles to be easily called from an IDE without needing to set up specific environment variables first.

After this initial preparation step, CMake is not used again unless the build scripts are modified. However, dependencies are built into the generated makefiles such that CMake generally runs automatically when necessary. The only major exception to this is if a source file is added to a build script that was using "wildcards" or shell glob commands to find source files. 

In the specific case of source file additions without modifications to existing build scripts, the prep step needs to be run again to pick up the new added files.

## Building and Installing

The CFS mission binary files may be generated by simply running:

```make```

The CMake build scripts will generate the binary files in a deep build tree based on the original location of the source file from which it was built. This is generally not suitable for copying to a target machine, so an "install" target is also implemented to make this process easier:

```make install```

Will copy all the generated executable files (typically the "core" executable plus any loadable object files and/or configuration files specified in the mission configuration) to a common directory. The directory structure is organized by CPU name so that it is easy to copy the entire tree to a target, or the directory could be NFS mounted in the case of Linux targets.

The default "all" makefile target (which generates the binaries) is also a prerequisite of the "install" target, so running "make install" is typically sufficient to build the entire project.

### Generating Documentation

The CMake build system can also generate documentation in latex, html, and pdf formats.
 
To build all documentation:

```make docs```

To build the cFS User's Guide:

```make usersguide```
 
To build the OSAL User's Guide:
```make osalguide```

## Writing CMakeLists files for applications

Every application or module is expected to provide a top-level CMakeLists.txt file in its respective source tree that contains the recipe for building that module.

The build system provides two useful functions that can be used to simplify this task for most modules:

```cmake
add_cfe_app(app_name source1.c source2.c ...)
add_cfe_tables(app_name table1 table2 ...)
```

`add_cfe_app` generates rules to compile and link the given source files into a module named app_name, using the configured cross compiler and all relevant switches/settings defined by the build system. The resulting module file will be installed to all targets that have app_name listed in their respective configuration in targets.cmake.

`add_cfe_tables` generates rules to compile a source file for each table name specified, then execute the appropriate conversion tool (such as elf2cfetbl) on the binary file to produce a cFE table file with a .tbl extension. The final .tbl file will be installed to the target directory.

`add_cfe_tables` will first search the mission-specific areas of the source tree for matching filenames. These mission-specific source files will be given preference over source files contained within the application module source tree, which are assumed to be "example" files.
Since table files are generally customized for mission-specific operation (as opposed to using an example table file "as-is" from the source distribution), the build system discourages modifying original source files for mission-specific purposes. 

For each table definition, `add_cfe_tables` will search the following paths:

- ${​​​​MISSION_DEFS}​​​​/tables/${​​​​CPUNAME}​​​​_${​​​​TBL}​​​​.c
- ${​​​​MISSION_SOURCE_DIR}​​​​/tables/${​​​​CPUNAME}​​​​_${​​​​TBL}​​​​.c
- ${​​​​MISSION_DEFS}​​​​/tables/${​​​​TBL}​​​​.c
- ${​​​​MISSION_SOURCE_DIR}​​​​/tables/${​​​​TBL}​​​​.c

`MISSION_DEFS` represents the "defs" directory which contains targets.cmake and others; `CPUNAME` represents the name of the CPU; `TBL` represents the name of the table as indicated in the add_cfe_tables command; and `MISSION_SOURCE_DIR` is the mission top-level source directory; 

For a concrete example using the included "sample_defs" configuration, if an application recipe contains `add_cfe_tables(myapp myapp_table1)`, the following will be searched relative to ${​​​​MISSION_SOURCE_DIR}​​​​:

- sample_defs/tables/cpu1_table1.c
- tables/cpu1_table1.c
- sample_defs/tables/table1.c
- tables/table1.c

Note that first searching for filenames prefixed with the CPU name i.e. cpu1_table1.c allows different table definitions to be used on different CPUs. For example, `cpu2_table1.c` can be defined if the application is to be also used on a platform named "cpu2".

## Handling Multiple Configurations

The recommended way to build for multiple configurations depends on the purpose of the different configurations.
 
*Purpose 1:* Software Runs on Multiple CPUS
If the flight software is split across two or more different CPUS, which run in parallel and work together for form a complete system, the preferred approach is to define multiple targets in targets.cmake, defining the apps that run on each processor. 
 
*Purpose 2:* Running the same configuration on a different processor
If the same flight software configuration needs to be built for a different process, the preferred approach is to generate a different build tree using the "SIMULATION" flag.  This flag overrides the "<tgt>_SYSTEM" setting and allows you to specify an alternate tool chain such as "native" for the native system.  
 
Thus you can have one "build-native" tree (for debugging on a local Linux workstation) in parallel with the normal "build" tree for the FSW/cross target.  You can then run "make" in whichever tree you want to build.
 
### Advantages of this approach:

Keeps configuration files cleaner, no clutter with debug environment stuff
All Message IDs stay the same between debug and FSW (so test configuration is more similar to the flight configuration)
No need to redefine tables unless you want a different configuration
Custom Installation
The CMake build system allows users to provide custom installation instructions through an install_custom.cmake file.  If used, this file should be placed in defs/<tgt>.  For example, sample_defs/cpu1/install_custom.cmake.
 
The install_custom.cmake file is imported in the "process_arch" function in the "arch_build.cmake" file.
 
The following example snippet copies a configuration file specified in targets.cmake into the installation directory under the name "CFG" (this is a simple modification to the standard FILELIST behavior).
 
In targets.cmake:

```cmake
SET(cpu1_CFG_FILE MY_CFG_FILE)
```

In sample_defs/cpu1/install_custom.cmake:

```cmake
set(CFGFILE ${​​​​​​​${​​​​​​​TGTNAME}​​​​​​​_CFG_FILE}​​​​​​​)
if(EXISTS ${​​​​​​​MISSION_DEFS}​​​​​​​/${​​​​​​​TGTNAME}​​​​​​​/${​​​​​​​CFGFILE}​​​​​​​)
  set(FILESRC ${​​​​​​​MISSION_DEFS}​​​​​​​/${​​​​​​​TGTNAME}​​​​​​​/${​​​​​​​CFGFILE}​​​​​​​)
elseif(EXISTS ${​​​​​​​MISSION_DEFS}​​​​​​​/${​​​​​​​TGTNAME}​​​​​​​_${​​​​​​​CFGFILE}​​​​​​​)
  set(FILESRC ${​​​​​​​MISSION_DEFS}​​​​​​​/${​​​​​​​TGTNAME}​​​​​​​_${​​​​​​​CFGFILE}​​​​​​​)
elseif(EXISTS ${​​​​​​​MISSION_DEFS}​​​​​​​/${​​​​​​​CFGFILE}​​​​​​​)
  set(FILESRC ${​​​​​​​MISSION_DEFS}​​​​​​​/${​​​​​​​CFGFILE}​​​​​​​)
else()
  set(FILESRC)
endif()
if (FILESRC)
  # In case the file is a symlink, follow it to get to the actual file
  get_filename_component(FILESRC "${​​​​​​​FILESRC}​​​​​​​" REALPATH)
  message("NOTE: Selected ${​​​​​​​FILESRC}​​​​​​​ as source for ${​​​​​​​CFGFILE}​​​​​​​ on ${​​​​​​​TGTNAME}​​​​​​​")
  install(FILES ${​​​​​​​FILESRC}​​​​​​​ DESTINATION ${​​​​​​​TGTNAME}​​​​​​​/${​​​​​​​INSTALL_SUBDIR}​​​​​​​ RENAME CFG)
else(FILESRC)
  message("WARNING: Install file ${​​​​​​​CFGFILE}​​​​​​​ for ${​​​​​​​TGTNAME}​​​​​​​ not found")
endif (FILESRC)
```

## CMake Unit Tests

The new cFE distributed unit tests can now be build and run using CMake.  Note that not all GSFC apps have been converted to the new "distributed" UT Assert.  In order to build the new distributed UT Assert:
 
```bash 
make distclean
make SIMULATION=native ENABLE_UNIT_TESTS=TRUE prep
make
make test
```

Tests can also be run individually from the build/native directory.

## Other CMake Tips

Running with GDB
To run cFS with GDB, add `set(CMAKE_BUILD_TYPE Debug)` to your CMake toolchain file
To build cFS as 32-bit, run: 

```bash
make CFLAGS="-m32" prep
```

### Installing App-Specific Files

To install files when building an app:

```cmake
foreach(TGT ${​​​​​​APP_INSTALL_LIST}​​​​​​)
    install(FILES <filename> DESTINATION ${​​​​​​TGT}​​​​​​/${​​​​​​INSTALL_SUBDIR}​​​​​​)
endforeach()
```