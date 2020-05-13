Travis-CI: [![Build Status](https://travis-ci.com/nasa/cFE.svg)](https://travis-ci.com/nasa/cFE)

# Core Flight System : Framework : Core Flight Executive

This repository contains NASA's Core Flight Executive (cFE), which is a framework component of the Core Flight System.

This is a collection of services and associated framework to be located in the `cfe` subdirectory of a cFS Mission Tree. The Core Flight System is bundled at <https://github.com/nasa/cFS>, which includes build and execution instructions.

## Version History

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

### **_OFFICIAL RELEASE: 6.7.0_**

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
