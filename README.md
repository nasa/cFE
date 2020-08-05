Travis-CI: [![Build Status](https://travis-ci.com/nasa/cFE.svg)](https://travis-ci.com/nasa/cFE)

# Core Flight System : Framework : Core Flight Executive

This repository contains NASA's Core Flight Executive (cFE), which is a framework component of the Core Flight System.

This is a collection of services and associated framework to be located in the `cfe` subdirectory of a cFS Mission Tree. The Core Flight System is bundled at <https://github.com/nasa/cFS>, which includes build and execution instructions.

The detailed cFE user's guide can be viewed at <https://github.com/nasa/cFS/blob/gh-pages/cFE_Users_Guide.pdf>.

## Version History

### Development Build: 6.7.0+dev292

- Add missing include path to the target/h and wrn/coreip directory.
Set and clarify difference between WIND_HOME and WIND_BASE variables.
Remove unrelated comment about CEXP (remnant from RTEMS). No more errors about missing headers.
- Version reporting is does not span multiple lines.
- See <https://github.com/nasa/cFE/pull/792>

### Development Build: 6.7.0+dev289

- Update `CFE_SB_TimeStampMsg` to save the message pointer argument `UT_Stub_CopyFromLocal` so that unit tests can check it
- Only affects build system. Fully backward compatible. The defaults are applied if a user has nothing specifically configured in their `targets.cmake`. The defaults will select osal, cfe-core, and psp as before. The user now has the option to explicitly configure and control the inclusion of these modules and also provide mission-specific search paths to override them as desired.
- Note this only affects UT stubs. Change the internal names of some stub arguments to match prototype. Ensure that:
  - All functions in the CFE public API have a stub function implemented
  - All parameters to the stub function are registered in the context object, so the values will be available to hook functions.
  - The names of all parameters match the prototype/documentation, so hook functions that use name-based argument value retrieval will work.
-  Adds to table search path in `arch_build.cmake`
- Calls to OS_open() now use the OSAL-defined symbol, not the POSIX symbol. 
-  Defines new macros to report the build number and build baseline and new strings containing the version number of cFE and a combined string with the version number for OSAL, PSP, and CFE.
- Allow explicitly setting of the processor ID in `targets.cmake`. The `TGTx_PROCESSOR_ID` setting will be passed to the final build/link of CFE core as the CPU ID. If unspecified, then the CMake index value is used instead (backward compatible).
- `cmake` now detects conditions where no files were present to fulfill an config include file requirement and reports an error during `make prep` lists the files it checked for rather than generating an empty file.
- See <https://github.com/nasa/cFE/pull/765>

### Development Build: 6.7.21

- If a string is exactly the size of the field when using the `CFE_TBL_FILEDEF()` macro it will produce a compiler error
- Added cFE User's Guide Reference to README.md
- Removes old license
- See <https://github.com/nasa/cFE/pull/743>
 
### Development Build: 6.7.20

- SB Unit use of the UT assert framework is closer to original design intent
- See <https://github.com/nasa/cFE/pull/743>

### Development Build: 6.7.19

- API Change: cFE ut_sb_stubs now has CFE_SB_DeletePipe available.
Behavior Change: App unit tests requiring this will not fail to build due to undefined reference to CFE_SB_DeletePipe
- Hook functions may now use the va_list form and obtain the full set of variable arguments passed to CFE_EVS_SendEvent and variants.
- Replace all direct references to data types defined in ccsds.h with the abstract type defined in cfe_sb.h.
- See <https://github.com/nasa/cFE/pull/729> for details.

### Development Build: 6.7.18

- Using ut stubs CFE_EVS_SendEvent and CFE_ES_WriteToSysLog, the register buffer will have the correct size. access to register buffer element will exist
- Both the main task and the child task(s) are successfully deleted and restarted after the exception occurs.
- Fixes doxygen warnings for the tbl subsystem.
- No compiler warnings or errors on cross build.
- Changes Message Key from uint16 to uint32 to avoid rollover and system hang
- See <https://github.com/nasa/cFE/pull/712> for more details

### Development Build: 6.7.17

- No longer automatically decompresses apps/libraries as part of load
- Deletes now unused CFE_ES_CountObjectCallback and CFE_ES_ListResourcesDebug. Flags were unused
- Removes all conditional preprocessing blocks related to CFE_ARINC653.
- Ensure clean build, no warnings on string operations using GCC 9.3.0.
- When OMIT_DEPRECATED = true attempt to send output to shell command will result in command error counter increment (unrecognized function code)
- SBN will need to init command with new MID
- Documentation links and references will now work properly
- API CFE_ES_ProcessCoreException is removed, replaced with async event.
- Removed duplicate prototype in cfe_time_utils.h
- Removes unused defines and adds documentation to TBL event defines.
- Deprecates CFE_TIME_CFE2FSSeconds and CFE_TIME_FS2CFESeconds.
- Unit tests now build and run when MESSAGE_FORMAT_IS_CCSDS_VER_2 is configured.
- Build now works with both extended headers and OMIT_DEPRECATED options set.
- No more alignment warnings
- Adds new unit test macros
- See <https://github.com/nasa/cFE/pull/692> for more details

### Development Build: 6.7.16

- Users must now select OSAL options via the CMake file in their defs directory, rather than the osconfig.h file.
- See <https://github.com/nasa/cFE/pull/672> for more details

### Development Build: 6.7.15

- Upon power on reset, default system log mode set to overwrite. Upon processor reset, default system log mode set to discard.
- No longer locks while locked (no issue observed on linux/posix, but user reported issue on FreeRTOS 10)
- Internal `CFE_TBL_LoadFromFile()` API changed slightly to add AppName as a parameter. Return value from `LoadFromFile()` no longer relevant for event generation.
- Updates `CFE_TBL_CleanUpApp` such that it now checks the 'used flag' prior to calling `CFE_TBL_RemoveAccessLink` for a given TblHandle. Also sets the AppId to `CFE_TBL_NOT_OWNED` after removing the access descriptor link from linked list.
- Removed `OS_FS_SUCCESS, OS_FS_ERROR , OS_FS_ERR_INVALID_POINTER, OS_FS_ERR_NO_FREE_FDS , OS_FS_ERR_INVALID_FD, and OS_FS_UNIMPLEMENTED` from `osapi-os-filesys.h`
- See <https://github.com/nasa/cFE/pull/649> for more details

### Development Build: 6.7.14

- Exposes the `CFE_SB_IsValidMsgId()` for application usage.
- `CFE_SB_GetLastSenderID` will now detect if it is being called prior to a message being sent on a given pipe.
- Mismatches between PSP/BSP/OS are now detected and warned about during make prep. Only the `CFE_SYSTEM_PSPNAME` is actually required to be specified for a CFE build now. Others can be omitted.
- See <https://github.com/nasa/cFE/pull/635> for more details

### Development Build: 6.7.13

- RTEMS builds without error.
- Use the INTERFACE_COMPILE_DEFINITIONS and INTERFACE_INCLUDE_DIRECTORIES properties from the osal target and apply them to the entire CFE build as a directory-scope property. No impact until these are set in OSAL.
- Minor other updates (see <https://github.com/nasa/cFE/pull/615>)

### Development Build: 6.7.12

- Cmd code (and checksum) are always in the same place (matches GSFC spec for command secondary header)
- No impact to behavior. Previously the perf log dump file frequently contained errors due to out of order or otherwise corrupted entries, which is now fixed.
- Minor other updates (see <https://github.com/nasa/cFE/pull/586>)

### Development Build: 6.7.11

- Improve documentation
- Update makefile to report branch coverage
- Minor other updates (see <https://github.com/nasa/cFE/pull/566>)

### Development Build: 6.7.10

- Fix potential unit test problems with name collisions
- Improve documentation
- Minor other updates (see <https://github.com/nasa/cFE/pull/563>)

### Development Build: 6.7.9

- No longer requires sed "hack" to change the setting in default_config.h
- Minor other updates (see <https://github.com/nasa/cFE/pull/544>)

### Development Build: 6.7.8

- Updates and cleaned up documentation and requirements
- Fixes incorrect debug messages
- Decrease child task count when one is deleted
- Minor other updates (see <https://github.com/nasa/cFE/pull/530>)

### Development Build: 6.7.7

- Adds a new function, CFE_SB_GetPipeIdByName, which retrieves the pipe ID given a name of a pipe.
- Improvement in error reporting when using a pipe name that is already in use, or when the queue limit has been reached.
- Added userguide and osalguide to the local target list to avoid makefile warning
- Minor other updates (see <https://github.com/nasa/cFE/pull/511>)

### Development Build: 6.7.6

- Adds OMIT_DEPRECATED prep flag
- Adds and enforces strict warnings
- Software Bus now increments sequence counter even if there are no subscribers
- Warning, comment, and code coverage cleanup (see <https://github.com/nasa/cFE/pull/490>)

### Development Build: 6.7.5

- Added CI static analysis check
- Resolved static analysis warnings
- Minor other updates (see <https://github.com/nasa/cFE/pull/479>)

### Development Build: 6.7.4

- Minor updates (see <https://github.com/nasa/cFE/pull/448>)

### Development Build: 6.7.3

- Minor updates (see <https://github.com/nasa/cFE/pull/413>)

### Development Build: 6.7.2

- Minor bugs and enhancements (see <https://github.com/nasa/cFE/pull/388>)

### Development Build: 6.7.1

- Fix strlen in CFE_ES_TaskInit <https://github.com/nasa/cFE/pull/23>
- Minor bug fixes (see <https://github.com/nasa/cFE/pull/378>)

### **_OFFICIAL RELEASE: 6.7.0 - Aquila_**

- This is a point release from an internal repository
- Changes are detailed in [cFS repo](https://github.com/nasa/cFS) release documentation
- Apache 2.0

### **_OFFICIAL RELEASE: 6.6.0a_**

- This is a point release from an internal repository
- Apache 2.0
- Additional release notes are found in [release notes](https://github.com/nasa/cFE/blob/v6.6.0a/docs/cFE_release_notes.md)
- See the [version description document](https://github.com/nasa/cFE/blob/v6.6.0a/docs/cFE_6_6_0_version_description.pdf) for the full document
- Test results can be found in [test results](https://github.com/nasa/cFE/tree/v6.6.0a/test-and-ground/test-review-packages/Results)

## Known issues

See all open issues and closed to milestones later than this version.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at <https://github.com/nasa/cFS>.

Official cFS page: <http://cfs.gsfc.nasa.gov>
