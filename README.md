![Static Analysis](https://github.com/nasa/cfe/workflows/Static%20Analysis/badge.svg)

# Core Flight System : Framework : Core Flight Executive

This repository contains NASA's Core Flight Executive (cFE), which is a framework component of the Core Flight System.

This is a collection of services and associated framework to be located in the `cfe` subdirectory of a cFS Mission Tree. The Core Flight System is bundled at <https://github.com/nasa/cFS>, which includes build and execution instructions.

The detailed cFE user's guide can be viewed at <https://github.com/nasa/cFS/blob/gh-pages/cFE_Users_Guide.pdf>.

## Version History

### Development Build: v6.8.0-rc1+dev498

- Reports test failures as CFE events. Test status messages are now sent as Events rather than Syslog. This allows for more processing capability, and allows failures to be received externally (e.g. ground system).
- See <https://github.com/nasa/cFE/pull/1295> and <https://github.com/nasa/cFS/pull/242>

### Development Build: v6.8.0-rc1+dev494

- Adds new tests for the ES Info APIs
- See <https://github.com/nasa/cFE/pull/1284> and <https://github.com/nasa/cFS/pull/238>

### Development Build: v6.8.0-rc1+dev490

- Removes `PspConfig` member from the `GLOBAL_CONFIGDATA` object. Updates the only remaining reference to this object inside the CFE_PSP_VERSION and uses the new Version API instead. Updates the OSAL version print to use the version API call and CFE uses the macro directly.
- Replaces duplicate mention of the removed `CFE_SB_ZeroCopyGetPtr` with the correct removal target of `CFE_SB_ZeroCopyReleasePtr`
- See <https://github.com/nasa/cFE/pull/1279> and <https://github.com/nasa/cFS/pull/233>

### Development Build: v6.8.0-rc1+dev484

- Removes cases in `cfe_es_apps.c` and `cfe_tbl_internal.c` that could never hit the alternate condition since the condition was already checked
- Removes all APIs deprecated in #777 and #998
- Resolves CodeQL warnings on uninitialized variables.
- Refactors a small portion of `CFE_TIME_UnregisterSynchCallback` and initializes variables to resolve "uninitialized variable" false alarms.
- Fixes a typo in initialization in `CFE_TBL_Validate( CFE_TBL_Handle_t TblHandle )`
- Initializes `TotalMsgSize` as 0 to avoid static analysis warning of "use before initialized"
- Increments the `CreatePipeErrorCounter` for all create pipe errors to eliminate a trivial static analysis warning
- Removes redundant or unreachable assignments and checks
- Updates header guards to standard format. Converts some file-scope block comments to a doxygen format to include a summary of the file.
- Enables the internal helper functions that determine table slot availability to handle NULL pointers.
- Resolves static analysis warnings by removing redundant check for `CFE_SUCCESS` in `CFE_EVS_EarlyInit`
- Moves the invocation of `CFE_PSP_AttachExceptions()` from the registration function to the pre-entry function and removes all references to task registration in code, docs, and tests. **This API change affects cFS apps.**
- Renames `CFE_TestRunner_AppMain` as `CFE_TR_AppMain` so it is less than 20 characters long. Updates App file names in documentation for `cfe_es_startup.scr`.
- Replace the call to `CFE_SB_MessageStringGet()` with the new filename-aware function `CFE_FS_ParseInputFileName()` for commands that contain file names like `CFE_ES_StopPerfDataCmd`. The default pathname/extension logic is now applied here too and only a "basename" is strictly necessary, although if a full/absolute path is given, it will be used as is.
- Removes the now-unnecessary `CFE_SB_ZeroCopyHandle_t` type and all APIs that refer or require it .Replaces `CFE_SB_ZeroCopyGetPtr()` and `CFE_SB_ZeroCopyReleasePtr()` with two new simplified functions `CFE_SB_AllocateMessageBuffer()` and `CFE_SB_ReleaseMessageBuffer()` , respectively.  These new functions do not use a separate handle. Updates the `CFE_SB_TransmitBuffer()` API to also remove the handle. **This breaks public APIs**.
- Internal cleanup localized to ES implementation. Consolidate all ES global variables under the `CFE_ES_Global` struct. Removes the separate `CFE_ES_TaskData` and some random pointers that were stored at global scope. Adjusts all references to the deprecated items accordingly (search and replace).
- Adds PSP version info to ES Housekeeping TLM messages. Changes both PSP and OSAL version info assignments on HK TLM to use the new version info API.
- Fixes check for "NumBuckets" member to use `<=` instead of `<`. `CFE_ES_GenPoolValidateState()` now returns `true` if using the max number of buckets (17 by default) and the pool structure using max value will correctly validate
- Replaces remaining `CFE_ES_ERR_BUFFER` with `CFE_ES_BAD_ARGUMENT` for when functions receive an invalid null-pointer argument. Adds null pointer checks in `cfe_es_api.c`.
- Adds branch coverage to html report when running `make lcov`
- See <https://github.com/nasa/cFE/pull/1258>

### Development Build: v6.8.0-rc1+dev436

- Adds a local definition of SOFTWARE_BIG/LITTLE_BIT_ORDER directly inside cfe_endian.h to provide a compatible symbol for apps that still require this. This allows CFE to build and run successfully when OSAL stops providing this in `common_types.h`.
- Removes incorrect statements from Application Developers Guide
- Fixes truncation handling on vsnprintf error by adding a cast to avoid implicit conversion
- Clarify the documentation on SB MsgId regarding requirements for command and telemetry messages
- Avoids undefined behavior and resolves static analysis warnings by casting isspace input to unsigned char.
- Updates message module and msgid v1, CFE_MSG_SetMsgId, to use mask instead of cast to alter value. Resolves static analysis warning.
- Updates CFE_ES_FileWriteByteCntErr to report status, not a size_t actual since OS_write returns int32. Use int16 for local type from CFE_TBL_FindTableInRegistry since it's an index, not a status.
- Replaces <> with " in local #includes
- Adds CONTRIBUING.md that links to the main cFS contributing guide.
- See <https://github.com/nasa/cFE/pull/1243>

### Development Build: v6.8.0-rc1+dev412

- Apply standard code style and format
- Add new continuous integration workflow to enforce this format
- See <https://github.com/nasa/cFE/pull/1219>

### Development Build: v6.8.0-rc1+dev402

- HOTFIX 20210312, updates to work with older CMake
- Fix #972, reorganize directory structure
- HOTFIX IC 2021-03-05: Correct static app build issue
- See <https://github.com/nasa/cFE/pull/1222>

### Development Build: v6.8.0-rc1+dev392

- Fix #665, update pipe name documentation.
- Fix #1165, remove configs about shells
- Fix #1094, Update CRC documentation
- Fix #979, add stack size and priority to task info …
- Fix #1170, refactor target config objects …
- Fix #1207, Add wrapper targets to simplify app builds …
- Fix #1211, Add Testing Tools to the Security Policy
- See <https://github.com/nasa/cFE/pull/1213>

### Development Build: v6.8.0-rc1+dev382

- Refactors the SB buffer descriptor object `CFE_SB_BufferD_t` and simplify the zero-copy buffer paradigm. Combines the zero-copy and the normal CFE buffer descriptor into a single unified `CFE_SB_BufferD_t` object. Results in a simpler zero-copy design that is similarto the the standard, non-zero-copy message path. All message descriptor objects are now tracked in a list by SB. All changes are internal to SB. This does not affect API or behavior of any existing APIs (but see note). Corrects a minor issue where the `MsgSendErrorCounter` would get incremented if there were no subscribers, but only in the zero copy API.  
- Replaces `int_32` with `CFE_Status_t` for all error message codes  
- Removes references to `cfeesugshellsrv` from user guide
- Adds null pointer checks and size checks to validate method parameters. Returning the input instead of an error code
- Removes use of `LogEnabled` element in HK telemetry for EVS logic since log is always enabled now. On failures, reset area or semaphore will panic.
-  Fixes various build warnings when `BUILDTYPE=release`.
- See <https://github.com/nasa/cFE/pull/1196>

### Development Build: v6.8.0-rc1+dev365

- Implements a generic FS facility to perform file writes as a background job. Applications wanting to use this need to instantiate a state object (metadata) in global memory and two callback APIs, one to get a data record, another to send events. The following file requests are changed to use this facility:
  - ES ER Log dump
  - SB Pipe Info
  - SB Message Map
  - SB Route Info
  - TBL Registry Dump
- Changes the internal SB member names for consistency and thus fixes propagation of `Depth` and `CurrentDepth` into files:
  - `MaxQueueDepth` for maximum depth at queue creation time (previously was QueueDepth or Depth depending on context)
  - `CurrentQueueDepth` for the running count (previously was InUse or CurrentDepth depending on context)
  - `PeakQueueDepth` for the highest "watermark" (previously was PeakInUse or PeakDepth depending on context)
- Encapsulates all parameters for apps and tasks into a structure object. Cleans up internal APIs to pass this new object rather than individual parameters. Adds details to the relevant record (i.e. a task record has all relevant task details) which eliminates the need to traverse the app record to find some data.
- Enables items in `FILELIST` to be in a target name directory as well as symlinks. `arch_build.cmake` now checks a name-based subdirectory under `${MISSION_DEFS}` for files listed in the `FILELIST` for that target. If file is a symlink, the link should be followed so the correct content is installed, not a symlink.
- Adds documentation on  inclusion presence of null terminators for length parameters.
- Shortened `CFE_PLATFORM_ES_DEFAULT_TASK_LOG_FILE` name so it is within the `OSAL_MAX_FILE_NAME` size limit. Will now output task info to default filename if no filename is provided in command.
- Replaces `UT_Stub_SetForceFail` with `UT_Stub_SetDefaultReturnValue`. No behavior change.
- See <https://github.com/nasa/cFE/pull/1171>

### Development Build: v6.8.0-rc1+dev348

- Corrects reference to PSP header file location. Build now succesfully completes the build succeeds again when using `add_psp_module()` in custom CMakeLists file.
- Replace "send" with "write" in names for commands that write files. For example, `CFE_SB_**SEND**_ROUTING_INFO_CC` is now `CFE_SB_**WRITE**_ROUTING_INFO_CC`. Updates function names, command code names and comments.
- Removes incorrectly implemented deferred return code of `-1` for `CFE_SB_ReceiveBuffer` from software bus setup in `UT_InitData`.
- Implements more informative **assert messages** by making `SETUP, TEARDOWN, ASSERT` print `0x%lx` while `ASSERT_EQ` now prints both `%lf` and `0x%lx` format for the inputs
- Updates continuous-integration badges in `ReadMe.md`. The badges now reflect the success status of different runs.
- Remove `Test_SB_Cmds_SubRptUnexpCmdCode` which was a duplicate of `Test_SB_Cmds_CmdUnexpCmdCode` and did not implement any new tests.
- Initializes status in `CFE_ES_WaitForSystemState` and adds missing success test case so the function doesn't return an uninitialized `Status`.
- Removes the `HkPacket` and `TblRegPacket` message initializations from `CFE_TBL_EarlyInit` since they are initialized in `CFE_TBL_InitData`. Moves the `NotifyMsg` message initialization to `CFE_TBL_InitData` and sets the message ID each time it's sent from `CFE_TBL_SendNotificationMsg`. Possibly results in small performance improvement since the message isn't initialized every call.
- Removes unimplemented `CFE_ES_AppGetList` and `CFE_ES_AppDumpAllInfo` prototypes.
- Adds a 15-minute timeout to continuous integration workflows to prevent excess resource utilization.
- Makes debug subscription events only print the Pipe ID, not a name, in the debug events.
- Updates the documentation and verification for `CFE_PLATFORM_SB_HIGHEST_VALID_MSGID` to allows the full range of values.
- Clarifies the difference between "restart" and "reload" in API/cmd and user's guide documentation for `CFE_ES_RESTART_APP_CC`.
- Switches throttle indexes to use `CFE_SB_RouteId_Atom_t` and combines helper function given that msgid was removed due to being a resource hog. Resolves static analysis warning.
- `CFE_ES_RestartApp` now checks for file existence as part of command processing and does not remove the app if the file doesn't exist (just avoids one error case). it also rejects the command and increments command error counter if file is missing.
- Removes `CFE_PLATFORM_SB_MAX_PIPE_DEPTH` in favor of `OS_QUEUE_MAX_DEPTH`. This depth parameter in command is now checked prior to attempting OSAL call.
- Filters pointer now `const` in API and reports truncation when registering filters with `CFE_EVS_Register`.
- Removes the ability to disable the log by not defining `CFE_PLATFORM_EVS_LOG_ON` so users are no longer able to disable log completely. For minimum memory use define `CFE_PLATFORM_EVS_LOG_MAX = 1`. Note: This could remove control based on LogEnabled, panic on reset area fail and limp along if "sem create" fails.
- Removes the remnants of the table service exclusion logic and documentation: `EXCLUDE_CFE_TBL` no longer available, even if defined, table services will still start.
- Set ES and EVS pipe message limit to defaults as opposed to the custom, unjustified, `CFE_SB_SubscribeEx`. This change might queue additional HK messages, but SCH loads after ES anyways.
- Replaces `CFE_SB_Default_Qos` with `CFE_SB_DEFAULT_QOS` macro that avoids global variable exposure. Removes SB-internal defines that are not implemented nor used.
- Explicity `memset` the task data to zero at the start of EarlyInit. Standardize the global typdef/variable names.
- Moves all functions, macros, types, and other definitions related to resource IDs and generic resource management into a separate module, like `CFE MSG`, `SBR`, etc. This allows a mission to elect "strict" implementations of these objects, where every ID type is unique, and assigning between them or `uint32` results in a compiler error. **API now has separate types for each resource type (Apps, Tasks, Libs, Counters, etc).** The user can elect at the mission level whether this is a simple typedef (all uint32, all interchangeable) or a wrapper type (separate/strict type, cannot be interchanged). The former is backward compatible but the latter is not - must use proper types.
- Adds Code QL analysis to continuous integration workflow.
- See <https://github.com/nasa/cFE/pull/1150>

### Development Build: v6.8.0-rc1+dev290

- Documentation: Add Security.md with instructions to report vulnerability
- Documentation: Update cpuname/MISSION_CPUNAMES documentation
- Fixes `UT_CheckEventHistoryFromFunc()` helper routine to read the correct number of IDs so it checks the correct number of events. Also correct bad event checks in TBL UT.
- Adds `OS_printf` to `CFE_ES_SYSLOG_APPEND` so it matches `CFE_ES_WriteToSysLog`
- Removes unused `SenderReporting` and `CFE_PLATFORM_SB_DEFAULT_REPORT_SENDER`
- Tests pass when debug events are enabled via `CFE_PLATFORM_EVS_DEFAULT_TYPE_FLAG` in platform config.
- Removes references to `UT_CheckForOpenSockets` which is no longer applicable since the UT framework resets the state for each unit test.
- Rename `UT_ClearForceFail` as `UT_ClearDefaultValue` given change in https://github.com/nasa/osal/issues/724
- Adds checks that ensure `CFE_SB_GetUserData` works with all payload data types.
- Adds header padding to 64-bit so that `CFE_SB_GetUserData` will work for message structures with elements up to 64 bit
- For primary-only header config: telemetry header required to 64 bit boundary (affects all receivers)
- For primary and extended header config: command header required padding to 64 bit boundary (affects all senders)
- Refactor `CFE_TIME_RegisterSynchCallback` to only have one return point and eliminates "possible uninitialized variable" static analysis warning
- None of these changes are expected to cause problematic.
- Addresses message delivery issues due to inconsistent locking by reworking cFE-SB API implementation. Ensures all events are generated and counters are incremented consistently by avoiding early returns in functions and using the `PendingEventID` register to record what event ID should be sent per the current operation.
- Employs the `CFE_ES_ResourceID_t` type and related patterns for managing the SB Pipe IDs.
- Will break code which directly accessed these items without going through the lookup function.
- **`CFE_SB_PipeId_t` type is no longer usable as a direct array index**, increased in size from 8 to 32 bits, and is now consistent with all other ID types in both behavior and size.
- **The "pipe stats" structure in the Pipe TLM is also changed**. This structure contained a `CFE_SB_PipeId_t` value, hence why it had to be updated because the type is now bigger. The spare bytes are also moved to the end of the struct.
- Removes `OS_printf` checks of stub calls in unit tests and checks for specific format string in history instead to confirm the right path was taken.
- Removes `CFE_MISSION_REV` from platform config.
- Removes the rest of the references and uses of `CFE_PLATFORM_ES_PERF_MAX_IDS` in favor of `CFE_MISSION_ES_PERF_MAX_IDS`
- Remove uses of strncpy and other minor hardcoded references
- Cleanup unit tests to reflect size changes in `CFE_MISSION_MAX_API_LEN` and `CFE_MISSION_MAX_PATH_LEN`.
- Moved ES pipe name and lengths to defines
- Removed PipeName and PipeDepth variables from app global
- Removed unnecessary (char *) casts
- Simplified `&stingname[0]` to `stringname` where observed
- Enables projects that have OSs with different limits to maintain a standard cmd/tlm and have unit tests pass.
- Make `CFE_ES_WriteToSysLog` stub unit test more informative by adding `UtDebug` output
- See <https://github.com/nasa/cFE/pull/1109>

### Development Build: v6.8.0-rc1+dev248

- Replace `OS_FileSysStatVolume()` with`OS_fsBlocksFree()` which will be deprecated. This call reports the number of total blocks, not just the free blocks, making the check more accurate and removing the need for a workaround for desktop machines.
- Instead of accessing `OS_time_t` values directly, use the OSAL-provided conversion and access methods. This provides independence and abstraction from the specific `OS_time_t` definition and allows OSAL to transition to a 64 bit value.
- Removes the spurious `CFE_SB_TimeOut_t` typedef from `cfe_sb.h`. May affect any apps that inappropriately rely on the private typedef.
- Removes unused `network_includes.h`. Not used by the framework anywhere,  apps should use OSAL Socket APIs instead.   
- Fixes deprecation directive typos
- See <https://github.com/nasa/cFE/pull/1088>

### Development Build: v6.8.0-rc1+dev236

- Resolved doxygen warnings for osalguide and updated header file references
- Corrects the documentation for the CFE_SB_GetPipeName() unit test stub function.
- Adds a new github actions workflow file to run cppcheck
- See <https://github.com/nasa/cFE/pull/1066>

### Development Build: v6.8.0-rc1+dev228

- Remove use of `osapi-os-loader.h` from ES UT.
- Use volatile `sig_atomic_t` for system state to avoid race issue if uint32 isn't atomic on a system
- Set the flags parameter on the OS_ModuleLoad() properly to allow an app to be properly unloaded, which in turn allows the reload command to work as expected. Fixes problem where unload comand resulted in continuous restarting of the same app code.
- Replaced `Test_MSG_PrintMsg` with `UT_DisplayPkt`. Also removed unused `Test_MSG_Sum`.
- See <https://github.com/nasa/cFE/pull/1047>

### Development Build: v6.8.0-rc1+dev218

- Adds `CFE_SB_TransmitMsg`, `CFE_SB_TransmitBuffer`, `CFE_SB_ReceiveBuffer`
  - Main change is to utilize `CFE_SB_Buffer_t` and `CFE_MSG_Message_t` in a consistent manner to facilitate alignment
  - Deprecates multiple `CFE_SB_*` items
  - Redefines `CFE_MSG_Size_t` as `size_t` to minimize duplicated work and facilitate transition to `size_t`
- Use a generic void* as the interface type for the pool buffer pointers. This reduces the need for local type casting in the apps and makes it generally easier to use.
- Remove  reference to CEXP in RTEMS 4.11 i686 toolchain. Add an RTEMS 5.1 i686 toolchain file.
- See <https://github.com/nasa/cFE/pull/1045>


### Development Build: v6.8.0-rc1+dev204

- Backward compatible API change. Replace many uses of generic uint16 and uint32 with a more purpose-specific type. Replace all sizes with size_t across the API.
- Rename `UT_SetForceFail` to `UT_SetDefaultReturnValue` since some functions that retain more than 1 value are not necessarily failing
- Deprecates many SB Elements and replaces them with the new MSG module. See https://github.com/nasa/cFE/issues/777 for list.
-  App and Lib info telemetry structures no longer contain the `ModuleId` value from OSAL.
- Add an extra write of a null char after `strncpy` which squelches a warning and appease compiler warning logic.
- Uses `CFE_PLATFORM_ES_DEFAULT_STACK_SIZE` as a default instead of a minimum. Affects the Start App command; if stack size is specified as zero, then the default stack size value from platform config is used. Otherwise the value in the command will be passed through and used as-is.
- Changes the type of the AppID parameter on "Cleanup" routines from `uint32` to `CFE_ES_ResourceID_t`.
- See <https://github.com/nasa/cFE/pull/1027>

### Development Build: v6.8.0-rc1+dev179

- Adds macros for more compact calls to `CFE_EVS_SendEvent`, making the type be part of the function name.
- The sample configs leap seconds default value is now up to date. (As of Oct 2020)
- Removed the clear=false logic (and clear parameter) `CFE_MSG_Init()` now always zeroes entire message and sets defaults.
- Adds flags parameter to calls to `OS_ModuleLoad()`. Initially just pass 0 (GLOBAL) to maintain old behavior.
- Updates `CFE_ES_RegisterCDSEx` stub to match current signature
- Includes `cfe_private.h` for stubs that implement related elements.
- See <https://github.com/nasa/cFE/pull/1008>

### Development Build: v6.8.0-rc1+dev164

- Keeps task names under 16 chars to make more debugger friendly, regardless
of the OSAL limit. Task name shows up as `ES_BG_TASK`
- Move ES typedefs shared across API and telemetry messages into the `cfe_es_extern_typedefs.h`.
- Move all ES typedefs that define the telemetry interface and structures that define the output of commands that write data files into this group (query all apps, query all tasks, query all CDS).
- Remove some localized definitions and replace with MISSION scope definitions where appropriate/necessary.
- Adjust `strncpy()` call to avoid compiler warning
- Cast fixed width types to the type used in the `printf` call. Removes `printf` type warnings on the 32-bit RTEMS build.
- See <https://github.com/nasa/cFE/pull/991>

### Development Build: v6.8.0-rc1+dev150

- Provide new Library API similar to App API
- Allows the existing CFE_ES_AppInfo_t structure to be extended to libraries as well as applications by introducing a new value (3) for the Type field.
- Allows Libraries to be queried via API calls similar to App API.
- Extends the Query All/Query One commands to operate on Libraries or Applications.
- Breaks up the monolithic AppCreate and LoadLibrary functions and have these call subroutines that operate on the common components.
- Fix race conditions in app request processing state machine.
- Adds SBR module which includes message map and routing table. The access APIs are on the SB side which still owns the destination logic
- Removes passing of route index or pointers being. Everything is now based on route and message id
- Oversized the hash message map (4x) to minimize collisions
- Hash designed for 32 bit, a change in CFE_SB_MsgId_Atom_t size may require implementation updates
- Adds debug event for collisions during add
- Dropped routing push/pop, dropped "key" in direct implementation
- Deletes unused code CFE_SB_FindGlobalMsgIdCnt
- Fixes variable declaration violations of coding standard
- Individual events for deleting destinations when deleting a pipe removed to avoid a race condition around a 10-20% performance hit to hash via rough comparison on a linux box, no memory impact
- See <https://github.com/nasa/cFE/pull/975>

### Development Build: v6.8.0-rc1+dev139

- For all resource types which have names, IDs are not re-issued after deletion, helping ensure safety as previously deleted IDs will not validate. Provides a consistent Name-ID translation API for all resource types. Enforces consistent argument/name validation on all resource types, and also enforces name uniqueness where relevant.
- Enhancement to use the full 16 bits of resource ID space, which avoids repeating ID values that have already been used. This significantly decreases the likelihood that a previously deleted ID will alias a newly allocated/valid ID.
- See <https://github.com/nasa/cFE/pull/959>

### Development Build: v6.8.0-rc1+dev129

- Rather than having a second pool implementation only for CDS, use the generic pool implementation. This also uses the abstract resource identifiers to identify CDS blocks, rather than a direct reference.
- Add the system-specific module suffix (.o, .so, .obj, etc) and the default CFE core executable name to the configdata structure.
- See <https://github.com/nasa/cFE/pull/944>

### Development Build: v6.8.0-rc1+dev122

- Adds the field `UnregAppID` to track whether an "unregistered" event was generated, un-overloading the EventCount field to serve its primary purpose of counting actual events generated from a valid/registered AppID.
- Move the AppID lookup execution to be early in the `CFE_SB_SendMsgFull` implementation. This avoids double locking between SB+ES and avoids a block-scope local variable.
- Instead of identifying a memory pool by its memory address, use a resource ID. IDs are a constant size, regardless of whether the host machine is 32 or 64 bits.
  - IDs can be put into commands/telemetry and maintain a more consistent format with consistent alignment requirements.
  - IDs can be independently verified without dereferencing memory. Previously the only way to validate a memory pool was to read the address pointed to, which results in a segfault if the address was bad.
- Change from `OS_MAX*` defines to appropriately-scoped CFE defines for array sizing
- This creates the new `CFE_Status_t` typedef for function's return status codes. Also adds a note to `CFE_TBL_GetStatus` since its behavior will likely change in the future in the hopes of not having a non-zero "info" status.
- See <https://github.com/nasa/cFE/pull/936>

### Development Build: v6.8.0-rc1+dev109

- Add a new typedef `CFE_ES_ResourceID_t` that can replace `uint32` for all ID storage and manipulation. Initially this is just an alias to `uint32` for backward compatibility.
- See <https://github.com/nasa/cFE/pull/916>

### Development Build: v6.8.0-rc1+dev105

- Removes dependency on CCSDS version define.
- Removes old name and id defines.
- CFE_ES_CalculateCRC default stub behavior.
- Replaces calls to OS_open and OS_creat
- Replaces UT_Text with UtPrintf
- Updates variable checks in read_targetconfig

- See <https://github.com/nasa/cFE/pull/912>

### Development Build: v6.8.0-rc1+dev91

- Sets Revision to 99 for development build.
- Installs unit test to target directory.
- Returns processor ID to default to unbreak toolchain
- Applies the appid/taskid/counterid pattern to Library resources.
- See <https://github.com/nasa/cFE/pull/891>

### Development Build: v6.8.0-rc1+dev81

- Deconflict CFE_ES_LIB_ALREADY_LOADED and CFE_ES_ERR_SYS_LOG_TRUNCATED EIDs
- Scrub all CFE references/uses of OSAL IDs to use the proper osal_id_t type. Any place that an OSAL ID is stored in memory or passed in an API call are changed to the osal_id_t type, rather than uint32. Conversions between this and other types (e.g. bare integer) is done using the OSAL-supplied conversion helpers.
- After the changes implemented in #101, there may be routing table entries with no subscribers (RoutePtr->ListHeadPtr would be NULL.) This could cause a seg-fault. Also, even if there are entries in the routing table, there will be no event generated if the unsubscribe does not find a matching route entry.
- Adds debug message.
- Applies the appid/taskid pattern to Generic Counter resources.
- Adds test for SB subscribe/unusubscribe/unsubscribe.
- See <https://github.com/nasa/cFE/pull/876>


### Development Build: v6.8.0-rc1+dev65

- In the next major CFE release, this code will be no longer supported at all. It should be removed early in the cycle to avoid needing to maintain this compatibility code.
- The CFE_ES_FindCDSInRegistry function had an unusual loop control structure with mixed types of signed and unsigned. This has the possibility of being infinite if the MaxNumRegEntries is zero due to the way the end condition is structured. Simplify to be like other loops and use unsigned int control variable.
- Fixes the cast-align error (use the aligned Msg since it's available already).
- HOTFIX-20200902 - Fix sb unit test setup issue.
- HOTFIX 20200902 - Update documentation links for deprecated symbols.
- HOTFIX 20200902 - Fix SB Test_CleanupApp_API AppID.
- See <https://github.com/nasa/cFE/pull/861>

### Development Build: v6.8.0-rc1+dev42

- Removes reference from documentation.
- CFE_SB_SendMsg stub now behaves the same as CFE_SB_TimeStampMsg (copies message pointer from local). No longer need to emulate CFE_SB_InitMsg from test code, set the API/stub data buffers directly.
- Removed iterator modification from within the loop... replaced with break.
- Resolves loop iterator size too small for comparison.
- Replaced CFE_MISSION_SPACECRAFT_ID use with CFE_PSP_GetSpacecraftId() and updated unit test
- See <https://github.com/nasa/cFE/pull/849>

### Development Build: v6.8.0-rc1+dev28

- Add msg stubs, update SB_UT to use them, and remove msg module include from unit tests
- Collapses time options down to just 32 bit second, 16 bit subsecond, always big endian. Removes old defines, and triggers an error if the configuration is set to a format that was removed.
- Enables source selection and out-of-tree mission-defined overrides in the msg directory
- Unit tests added from within unit tests will not execute, replaced this pattern with direct calls to the main subtest setup routine.
- See <https://github.com/nasa/cFE/pull/833>

### Development Build: v6.8.0-rc1+dev13

- Deprecates `CFE_SB_GetLastSenderId()` API by introducing new `CFE_OMIT_DEPRECATED_6_8` tag
- Documentation update remove deleted requiremements
- Add a new *cfe_assert* module for functional testing by making it possible to load the UT assert object code as a CFE library. These are compiled as separate, independent modules and only loaded on demand. Also includes a draft example for cFE testing, which calls some basic ES AppId functions.
- See <https://github.com/nasa/cFE/pull/816>

### Development Build: v6.7.0+dev292

- Add missing include path to the target/h and wrn/coreip directory.
Set and clarify difference between WIND_HOME and WIND_BASE variables.
Remove unrelated comment about CEXP (remnant from RTEMS). No more errors about missing headers.
- Version reporting is does not span multiple lines.
- See <https://github.com/nasa/cFE/pull/792>

### Development Build: v6.7.0+dev289

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

### **_OFFICIAL RELEASE: v6.7.0 - Aquila_**

- This is a point release from an internal repository
- Changes are detailed in [cFS repo](https://github.com/nasa/cFS) release documentation
- Apache 2.0

### **_OFFICIAL RELEASE: v6.6.0a_**

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
