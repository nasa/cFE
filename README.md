Travis-CI: [![Build Status](https://travis-ci.com/nasa/cFE.svg)](https://travis-ci.com/nasa/cFE)

# Core Flight System : Framework : Core Flight Executive

This repository contains NASA's Core Flight Executive (cFE), which is a framework component of the Core Flight System.

This is a collection of services and associated framework to be located in the `cfe` subdirectory of a cFS Mission Tree.  The Core Flight System is bundled at https://github.com/nasa/cFS, which includes build and execution instructions.

## Version Notes
- 6.7.13: DEVELOPMENT
  - RTEMS builds without error.
  - Use the INTERFACE_COMPILE_DEFINITIONS and INTERFACE_INCLUDE_DIRECTORIES properties from the osal target and apply them to the entire CFE build as a directory-scope property. No impact until these are set in OSAL.
  - Minor other updates (see https://github.com/nasa/cFE/pull/615)
- 6.7.12: DEVELOPMENT
  - Cmd code (and checksum) are always in the same place (matches GSFC spec for command secondary header)
  - No impact to behavior. Previously the perf log dump file frequently contained errors due to out of order or otherwise corrupted entries, which is now fixed.
  - Minor other updates (see https://github.com/nasa/cFE/pull/586)
- 6.7.11: DEVELOPMENT
  - Improve documentation
  - Update makefile to report branch coverage
  - Minor other updates (see https://github.com/nasa/cFE/pull/566)
- 6.7.10: DEVELOPMENT
  - Fix potential unit test problems with name collisions
  - Improve documentation
  - Minor other updates (see https://github.com/nasa/cFE/pull/563)
- 6.7.9: DEVELOPMENT
  - No longer requires sed "hack" to change the setting in default_config.h  
  - Minor other updates (see https://github.com/nasa/cFE/pull/544)
- 6.7.8: DEVELOPMENT
  - Updates and cleaned up documentation and requirements
  - Fixes incorrect debug messages
  - Decrease child task count when one is deleted
  - Minor other updates (see https://github.com/nasa/cFE/pull/530)
- 6.7.7: DEVELOPMENT
  - Adds a new function, CFE_SB_GetPipeIdByName, which retrieves the pipe ID given a name of a pipe.
  - Improvement in error reporting when using a pipe name that is already in use, or when the queue limit has been reached.
  - Added userguide and osalguide to the local target list to avoid makefile warning
  - Minor other updates (see https://github.com/nasa/cFE/pull/511)
- 6.7.6: DEVELOPMENT
  - Adds OMIT_DEPRECATED prep flag
  - Adds and enforces strict warnings
  - Software Bus now increments sequence counter even if there are no subscribers
  - Warning, comment, and code coverage cleanup (see https://github.com/nasa/cFE/pull/490)
- 6.7.5: DEVELOPMENT
  - Added CI static analysis check
  - Resolved static analysis warnings
  - Minor other updates (see https://github.com/nasa/cFE/pull/479)
- 6.7.4: DEVELOPMENT
  - Minor updates (see https://github.com/nasa/cFE/pull/448)
- 6.7.3: DEVELOPMENT
  - Minor updates (see https://github.com/nasa/cFE/pull/413)
- 6.7.2: DEVELOPMENT
  - Minor bugs and enhancements (see https://github.com/nasa/cFE/pull/388)
- 6.7.1: DEVELOPMENT
  - Minor bug fixes (see https://github.com/nasa/cFE/pull/378)
  - Fix strlen in CFE_ES_TaskInit https://github.com/nasa/cFE/pull/23
- **6.7.0 OFFICIAL RELEASE**:
  - This is a point release from an internal repository
  - Changes are detailed in [cFS repo](https://github.com/nasa/cFS) release documentation
  - Apache 2.0
- **6.6.0a OFFICIAL RELEASE**:
  - This is a point release from an internal repository
  - Apache 2.0
  - Additional release notes are found in [release notes](https://github.com/nasa/cFE/blob/v6.6.0a/docs/cFE_release_notes.md)
  - See the [version description document](https://github.com/nasa/cFE/blob/v6.6.0a/docs/cFE_6_6_0_version_description.pdf) for the full document
  - Test results can be found in [test results](https://github.com/nasa/cFE/tree/v6.6.0a/test-and-ground/test-review-packages/Results)

## Known issues

See all open issues and closed to milestones later than this version.

## Getting Help

For best results, submit issues:questions or issues:help wanted requests at https://github.com/nasa/cFS.

Official cFS page: http://cfs.gsfc.nasa.gov
