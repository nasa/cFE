# Changelog

## Development Build: equuleus-rc1+dev239
- address sanitizer issue in coverage-evs-ALL
- See <https://github.com/nasa/cFE/pull/2643>

## Development Build: equuleus-rc1+dev235
- add initialization of table indices
- See <https://github.com/nasa/cFE/pull/2633>

## Development Build: equuleus-rc1+dev231
- Update Upload Artifact Version
- See <https://github.com/nasa/cFE/pull/2631>

## Development Build: equuleus-rc1+dev227
- Add CFE_TBL_BufferSelect_Enum_t Type Name
- See <https://github.com/nasa/cFE/pull/2629>

## Development Build: equuleus-rc1+dev223
- Correct type of ActiveTableFlag variable
- See <https://github.com/nasa/cFE/pull/2459>

## Development Build: equuleus-rc1+dev219
- Consistent use of CFE_EVS_EventType_Enum_t for EventType
- Combine redundant switch blocks to simplify CFE_ES_CleanupObjectCallback
- Unloading module after failed app start
- Simplify and clarify EVS_AddLog logic
- See <https://github.com/nasa/cFE/pull/2308>, <https://github.com/nasa/cFE/pull/2612>, <https://github.com/nasa/cFE/pull/2616>, and <https://github.com/nasa/cFE/pull/2309>

## Development Build: equuleus-rc1+dev209
- Remove redundant status check in CFE_ES_RegisterCDSEx()
- See <https://github.com/nasa/cFE/pull/2329>

## Development Build: equuleus-rc1+dev205
- fix sb coverage test failure
- Pass the CMAKE_PREFIX_PATH to targets
- See <https://github.com/nasa/cFE/pull/2602> and <https://github.com/nasa/cFE/pull/2606>

## Development Build: equuleus-rc1+dev199
- Bump actions/download-artifact from 3 to 4.1.7 in /.github/workflows
- See <https://github.com/nasa/cFE/pull/2599>

## Development Build: equuleus-rc1+dev195
- Use string append and add newline
- Yield cpu to other tasks in SB Perf Test
- See <https://github.com/nasa/cFE/pull/2596> and <https://github.com/nasa/cFE/pull/2593>

## Development Build: equuleus-rc1+dev187
- Use proper printf format for size_t
- See <https://github.com/nasa/cFE/pull/2591>

## Development Build: equuleus-rc1+dev183
- Runtime Error in coverage-es-ALL, TestApps
- add config tool for platform-specific settings
- Adding bytes read check
- Cast to unsigned int
- Update comment re. limit of CFE_PLATFORM_SB_HIGHEST_VALID_MSGID
- Add element to write error event in CFE_EVS_WriteAppDataFileCmd()
- See <https://github.com/nasa/cFE/pull/2579>, <https://github.com/nasa/cFE/pull/2565>, <https://github.com/nasa/cFE/pull/2584>, <https://github.com/nasa/cFE/pull/2585>, <https://github.com/nasa/cFE/pull/2586>, and <https://github.com/nasa/cFE/pull/2326>

## Development Build: equuleus-rc1+dev167
- CFE_TIME unit test failure when CFE_MISSION_TIME_AT_TONE_WILL_BE set to true
- See <https://github.com/nasa/cFE/pull/2573>

## Development Build: equuleus-rc1+dev163
- Improve CFE_SB_AllocateMessageBuffer error report
- Update CFE_PLATFORM_ES_DEFAULT_SYSLOG_MODE macro in es_verify.h
- Update comment re. limits of CFE_PLATFORM_SB_HIGHEST_VALID_MSGID
- Use size_t for variables/parameters representing size
- Remove superfluous status assignment in CFE_SB_CreatePipe
- null value in vsnprintf
- Adds JSC 2.1 Static Analysis comments
- Reduces CFE_EVS_MAX_PORT_MSG_LENGTH to prevent new line character truncation
- See <https://github.com/nasa/cFE/pull/2560>, <https://github.com/nasa/cFE/pull/2344>, <https://github.com/nasa/cFE/pull/2359>, <https://github.com/nasa/cFE/pull/2376>, <https://github.com/nasa/cFE/pull/2496>, <https://github.com/nasa/cFE/pull/2554>, <https://github.com/nasa/cFE/pull/2568>, and <https://github.com/nasa/cFE/pull/2566>

## Development Build: equuleus-rc1+dev141
- use resourceids for internal table validation and dump control blocks
- refactor SB to support additional use cases
- msg api test buffer overrun
- See <2551>, <2381>, and <2537>

## Development Build: equuleus-rc1+dev137
- msg api test buffer overrun
- send CMake message to stdout instead of stderr
- See <https://github.com/nasa/cFE/pull/2537> and <https://github.com/nasa/cFE/pull/2525>

## Development Build: equuleus-rc1+dev131
- add handle list operation routines
- See <https://github.com/nasa/cFE/pull/2548>

## Development Build: equuleus-rc1+dev127
- improve app dev guide
- consistent TIME values for TBL structures
- See <https://github.com/nasa/cFE/pull/2541> and <https://github.com/nasa/cFE/pull/2544>

## Development Build: equuleus-rc1+dev120
- Switch pc rtems toolchains to use osal generic-rtems bsp
- table transaction initial implementation
- See <https://github.com/nasa/cFE/pull/2535> and <https://github.com/nasa/cFE/pull/2539>

## Development Build: equuleus-rc1+dev114
- Split up and simplify control flow in CFE_TBL_Register()
- naming convention for SendHkCmd
- See <https://github.com/nasa/cFE/pull/2387> and <https://github.com/nasa/cFE/pull/2531>

## Development Build: equuleus-rc1+dev107
- propagate stack pointer for child tasks
- add missing memset() for stack variables
- See <https://github.com/nasa/cFE/pull/2517> and <https://github.com/nasa/cFE/pull/2527>

## Development Build: equuleus-rc1+dev100
- Add runtime TopicId conversion routines to SB
- See <https://github.com/nasa/cFE/pull/2520>

## Development Build: equuleus-rc1+dev96
- change CFE_MSG_Message from union to struct
- Remove unreachable/dead branch in CFE_ES_RunPerfLogDump()
- See <https://github.com/nasa/cFE/pull/2515> and <https://github.com/nasa/cFE/pull/2330>

## Development Build: equuleus-rc1+dev90
- EDS XML file updates
- add EDS cmake hooks
- See <https://github.com/nasa/cFE/pull/2510> and <https://github.com/nasa/cFE/pull/2511>

## Development Build: equuleus-rc1+dev84
- CFE updates needed for generated header compatibility (EDS)
- See <https://github.com/nasa/cFE/pull/2505>

## Development Build: equuleus-rc1+dev79
- Relative prefix exclusion for cfe implementation file search
- See <https://github.com/nasa/cFE/pull/2494>

## Development Build: equuleus-rc1+dev71
- updating cFE to use new versioning system
- clean up and move table build scripts
- implement header files for testcase
- separate bad argument test
- See <https://github.com/nasa/cFE/pull/2463>, <https://github.com/nasa/cFE/pull/2486>, <https://github.com/nasa/cFE/pull/2485>, and <https://github.com/nasa/cFE/pull/2489>

## Development Build: v7.0.0-rc4+dev434
- Update docs and UT to use CFE_MSG_PTR
- document ES Perf enums
- document and use topicid numbers for cfe
- See <https://github.com/nasa/cFE/pull/2472>, <https://github.com/nasa/cFE/pull/2411>, and <https://github.com/nasa/cFE/pull/2474>

## Development Build: v7.0.0-rc4+dev424
- Add perfid limit info to ES docstring
- TBL UT update for OSAL/CFE path length mismatch
- add multi threaded SB perf tests
- message integrity API
- change workflow to use output on failure option
- See <https://github.com/nasa/cFE/pull/2409>, <https://github.com/nasa/cFE/pull/2373>, <https://github.com/nasa/cFE/pull/2466>, <https://github.com/nasa/cFE/pull/2468>, and <https://github.com/nasa/cFE/pull/2470>

## Development Build: v7.0.0-rc4+dev411
- rename reference table
- Remove nonexist reference for perf tool
- See <https://github.com/nasa/cFE/pull/2462> and <https://github.com/nasa/cFE/pull/2408>

## Development Build: v7.0.0-rc4+dev405
- add code coverage for null check
- See <https://github.com/nasa/cFE/pull/2377>

## Development Build: v7.0.0-rc4+dev401
- minor reorg of message headers
- See <https://github.com/nasa/cFE/pull/2453>

## Development Build: v7.0.0-rc4+dev395
- Adds an empty string or null pointer check for pipe creation
- crc calculation refactor
- See <https://github.com/nasa/cFE/pull/2440> and <https://github.com/nasa/cFE/pull/2450>

## Development Build: v7.0.0-rc4+dev389
- Adds a cast to the negation of unsigned expression
- See <https://github.com/nasa/cFE/pull/2437>

## Development Build: v7.0.0-rc4+dev384
- Uninitialized PktTime in EVS_SendViaPorts
- Uninitialized ExceptionTaskID in CFE_ES_RunExceptionScan
- implement SB bulk transfer test
- TriggerMode unsigned compare with 0
- Fixes errors in IC Bundle workflow file
- See: <https://github.com/nasa/cFE/pull/2401>, <https://github.com/nasa/cFE/pull/2403>, <https://github.com/nasa/cFE/pull/2380>, <https://github.com/nasa/cFE/pull/2400>, and <https://github.com/nasa/cFE/pull/2420/>

## Development Build: v7.0.0-rc4+dev370
- 'Fix #2430, reinstate example header files.'
- See: <https://github.com/nasa/cFE/pull/2431>


## Development Build: v7.0.0-rc4+dev366
- 'Fix #2427, Race condition in TestCreateChild'
- See: <https://github.com/nasa/cFE/pull/2428>


## Development Build: v7.0.0-rc4+dev362
- 'Fix #2421, adjust dependencies on table lib'
- 'Fix #2423, avoid use of abspath make function'
- See: <https://github.com/nasa/cFE/pull/2422> <https://github.com/nasa/cFE/pull/2424>


## Development Build: v7.0.0-rc4+dev356
- 'Fix #2415, use correct object extension in table build'
- See: <https://github.com/nasa/cFE/pull/2416>


## Development Build: v7.0.0-rc4+dev342
- 'Fix #2368, cFE Functional Test 23'
- 'Fix #1407, Add error report on EVS failure during log header write'
- 'Fix #1515, Remove unnecessary memset from CFE_TBL_LoadFromFile'
- See: <https://github.com/nasa/cFE/pull/2369> <https://github.com/nasa/cFE/pull/2272> <https://github.com/nasa/cFE/pull/2276>


## Development Build: v7.0.0-rc4+dev334
- Format update for additional counters in summary
- make shared tbl configurable
- Update README_Functionaltest.md
- Increase CFE_PLATFORM_EVS_APP_EVENTS_PER_SEC to 15
- Increase slp time in funct test workflow
- CFE_TIME_Print() calls strftime()
- Adds utassert message descriptions to README_Functionaltest.md
- See <https://github.com/nasa/cFE/pull/2350>, <https://github.com/nasa/cFE/pull/2354>, <https://github.com/nasa/cFE/pull/2355>, <https://github.com/nasa/cFE/pull/2366>, <https://github.com/nasa/cFE/pull/2364>, <https://github.com/nasa/cFE/pull/2356>, and <https://github.com/nasa/cFE/pull/2361>

## Development Build: v7.0.0-rc4+dev318
- generalize update header parameter
- add msg verify capability
- See <https://github.com/nasa/cFE/pull/2336> and <https://github.com/nasa/cFE/pull/2338>

## Development Build: v7.0.0-rc4+dev312
- adjust UT pool buffer size for platform config
- Remove unused CFE_PLATFORM_ES_RESET_AREA_SIZE
- See <https://github.com/nasa/cFE/pull/2328> and <https://github.com/nasa/cFE/pull/2331>

## Development Build: v7.0.0-rc4+dev304
- reorganize headers
- Update XML files per interoperability testing
- uniqueness of tbl target names
- See <https://github.com/nasa/cFE/pull/2321>, <https://github.com/nasa/cFE/pull/2314>, and <https://github.com/nasa/cFE/pull/2319>

## Development Build: v7.0.0-rc4+dev287
- document CFS component file naming
- Add EVS port timestamp and simplify port selection
- remove else statement that was unreachable by unit tests
- correct fallback file case
- See <https://github.com/nasa/cFE/pull/2306>, <https://github.com/nasa/cFE/pull/2295>, <https://github.com/nasa/cFE/pull/2303>, and <https://github.com/nasa/cFE/pull/2312>

## Development Build: v7.0.0-rc4+dev276
- improve add_cfe_tables function
- add option to link to generated files
- Create Workflow for IC Bundle Generation
- See <https://github.com/nasa/cFE/pull/2299>, <https://github.com/nasa/cFE/pull/2300>, and <https://github.com/nasa/cFE/pull/2298>

## Development Build: v7.0.0-rc4+dev268
- update naming convention document
- remove return value doxygen markup
- implement common search routine for config files
- See <https://github.com/nasa/cFE/pull/2254>, <https://github.com/nasa/cFE/pull/2288>, and <https://github.com/nasa/cFE/pull/2290>

## Development Build: v7.0.0-rc4+dev260
- add more generic status codes
- separate dispatcher for messages
- See <https://github.com/nasa/cFE/pull/2259> and <https://github.com/nasa/cFE/pull/2263>

## Development Build: v7.0.0-rc4+dev254
- improve 64-bit memory address handling in CMD/TLM
- See <https://github.com/nasa/cFE/pull/2256>

## Development Build: v7.0.0-rc4+dev250
- CFE_TBL_FILEDEF does not need static
- See <https://github.com/nasa/cFE/pull/2244>

## Development Build: v7.0.0-rc4+dev246
- Adds workflow_dispatch to all workflows
- See <https://github.com/nasa/cFE/pull/2236>

## Development Build: v7.0.0-rc4+dev242
- EDS Updates to match the current mainline
- Update UTs to use correct cmd types
- Move CRC types and convert to enum
- See <https://github.com/nasa/cFE/pull/2231>, <https://github.com/nasa/cFE/pull/2229>, and <https://github.com/nasa/cFE/pull/2192>

## Development Build: v7.0.0-rc4+dev233
- Replace CFE_MSG_CommandHeader_t w/ CFE_TBL_NoArgsCmd_t
- See <https://github.com/nasa/cFE/pull/2226>

## Development Build: v7.0.0-rc4+dev229
- add abstract documentation dependency target
- add -fno-common to arch build flags
- update github action versions
- Combine MESSAGE and CMD ENTRY Macros
- See <https://github.com/nasa/cFE/pull/2215>, <https://github.com/nasa/cFE/pull/2219>, <https://github.com/nasa/cFE/pull/2217>, and <https://github.com/nasa/cFE/pull/2222>

## Development Build: v7.0.0-rc4+dev218
- Update CI to use Ubuntu 20.04
- Enforce strict cast alignment on arch
- Support clang for whole archive flags
- Explicit toolchain in sample target.cmake
- See <https://github.com/nasa/cFE/pull/2209>, <https://github.com/nasa/cFE/pull/2206>, <https://github.com/nasa/cFE/pull/2204>, and <https://github.com/nasa/cFE/pull/2203>

## Development Build: v7.0.0-rc4+dev205
- resolve static analysis errors in tbl_UT.c
- See <https://github.com/nasa/cFE/pull/2197>

## Development Build: v7.0.0-rc4+dev201
- use osal_public_api header targets in doc
- See <https://github.com/nasa/cFE/pull/2184>

## Development Build: v7.0.0-rc4+dev197
- Remove Changelog in Doxygen Config
- See <https://github.com/nasa/cFE/pull/2165>

## Development Build: v7.0.0-rc4+dev193
- Remove redundant/inconsistent comments (/* end of function */, /* end if */ etc.) and clean up empty lines.
- Update TO_LAB entry point in cfe_es_startup.scr
- Create CHANGELOG.md
- See <https://github.com/nasa/cFE/pull/2163>, <https://github.com/nasa/cFE/pull/2158>, and <https://github.com/nasa/cFE/pull/2159>

## Development Build: v7.0.0-rc4+dev183
- Fixes issue #2144- Propagate CMAKE_EXPORT_COMPILE_COMMANDS variable
- Remove CodeQL Paths Ignore
- Duplicated Logic in CFE_SB_BroadcastBufferToRoute
- Remove 'return;' from last line of void functions.
- Remove unnecessary parentheses around return values.
- See <https://github.com/nasa/cFE/pull/2145>, <https://github.com/nasa/cFE/pull/2148>, <https://github.com/nasa/cFE/pull/2151>, <https://github.com/nasa/cFE/pull/2156>, and <https://github.com/nasa/cFE/pull/2154>

## Development Build: v7.0.0-rc4+dev171
- Resolve doxygen 1.8.17 latex make errors
- See <https://github.com/nasa/cFE/pull/2143>

## Development Build: v7.0.0-rc4+dev167
- Squash uninit var static analysis warnings
- See <https://github.com/nasa/cFE/pull/2138>

## Development Build: v7.0.0-rc4+dev163
- Increase event burst credit to not squelch at startup in CI
- Grammatical/format updates to EVS flooding requirements
- See <https://github.com/nasa/cFE/pull/2132> and <https://github.com/nasa/cFE/pull/2134>

## Development Build: v7.0.0-rc4+dev157
- use correct loop var in add_cfe_app_dependency
- Automatic suppression of flooding events
- Remove stray TODO in UT
- See <https://github.com/nasa/cFE/pull/2129>, <https://github.com/nasa/cFE/pull/2117>, and <https://github.com/nasa/cFE/pull/2126>

## Development Build: v7.0.0-rc4+dev145
- remove commented out code
- See <https://github.com/nasa/cFE/pull/2122>

## Development Build: v7.0.0-rc4+dev143
- Endian macro mask before shift to avoid shift overflow warning
- Add macro for initializing command header
- Add CFE_ES_StatusToString and CFE_STATUS_C
- See <https://github.com/nasa/cFS/pull/505>

## Development Build: v7.0.0-rc4+dev136
- Resolve UT uninit var static analysis warnings
- Resolve fsw uninit var static analysis warnings
- Resolve UT non-distinct identifier warnings
- Resolve UT buffer underflow warning
- Fix cFE_Users_guide url 404 error'
- See <https://github.com/nasa/cFE/pull/2111> and <https://github.com/nasa/cFS/pull/492>

## Development Build: v7.0.0-rc4+dev127
- Update RTEMS toolchain files
- Add support for fractional seconds in epoch
- Add truncation warning suppression flags
- Deploy docs from local workflow on main branch push
- Remove redundant word in App Developers Guide    
- UT updates for alternate time configuration
- See <https://github.com/nasa/cFE/pull/2098> and <https://github.com/nasa/cFS/pull/471>

## Development Build: v7.0.0-rc4+dev110
- Only increment SB InternalErrorCounter on errors
- Improve ES unit test coverage
- Add config module to coverage enforcemnt    
- Consistent doxygen flow/naming for generation and use reusable documentation generation workflow
- Remove contributor license agreements
- See <https://github.com/nasa/cfe/pull/2091> and <https://github.com/nasa/cFS/461>

## Development Build: v7.0.0-rc4+dev94
- Resolve build failure for time cfg 
- Update Copyright Headers
- Standardize version.h 
- See <https://github.com/nasa/cfe/pull/2074> and <https://github.com/nasa/cFS/445>

## Development Build: v7.0.0-rc4+dev87
- Revert development version identifier to 99 for revision number
- Use osal-common.doxygen to resolve OSAL Doxygen refs
- Refactor doxygen mainpage into frontpage
- See <https://github.com/nasa/cFE/pull/2066> and <https://github.com/nasa/cFS/pull/443>

## Development Build: v7.0.0-rc4+dev80
- Missing SB include for v2 msgid
- Resolve doxygen doc warnings and enforce in CI
- Remove explicit filename doxygen comments
- Add CFE_ES_CreateChildTask default handler
- See <https://github.com/nasa/cFE/pull/2061> and <https://github.com/nasa/cFS/pull/432>

## Development Build: v7.0.0-rc4+dev70
- Improve CFE_SB_IsValidMsgId handler
- Update CodeQL workflow
- Replace CFE_SB_ValueToMsgId(0) with CFE_SB_INVALID_MSG_ID
- See <https://github.com/nasa/cFE/pull/2043> and <https://github.com/nasa/cFS/pull/414>

## Development Build: v7.0.0-rc4+dev60
- Add CFE_ES_AppInfo_t element documentation
- Add FS Functional test
- Fix mismatched foreach in cmake function
- Improved code highlighting in cFE Application Developers Guide
- CFE_FS_ParseInputFileNameEx avoid uninit var
- Fix array length calculation for perf structs
- Limit SBR UT loops
- Initialize BlockData in ES UT
- Fix `osal_id_t` type conversion in `es_UT.c`
- See <https://github.com/nasa/cFE/pull/2019> and <https://github.com/nasa/cFS/pull/410>

## Development Build: v7.0.0-rc4+dev39
- Use cfe time clock state enum t in cmd/tlm and handling
- EVS functional test add for different event types
- Patch for recursive event loop
- CFE_ES_MemAddOff structure to associated CFE_ES_MemAddress_t and CFE_ES_MemOffset_t
- Avoid aliasing warnings
- Message ID type improvements
- CFE_SB_ValueToMsgId/MsgIdToValue wrappers
- Use CFE_TIME_ClockState_Enum_t in cmd/tlm and handling
- Consolidate repeated MSG stub setup in sb_UT
- Return type conversions in CFE_ES_GetTaskName
- Better message type pointer conversion
- Replacing hardcoded message limit in TIME services
- Use macro in CFE_ResourceId_IsDefined
- Checking against IsNewOffset only
- Set new build baseline for cFS-Caelum-rc4: v7.0.0-rc4
- See <https://github.com/nasa/cFE/pull/2001> and <https://github.com/nasa/cFS/pull/390>

## Development Build: v6.8.0-rc1+dev1030
- Grammatical cleanup of requirements
- Fix extended header definitions and update tests
- See <https://github.com/nasa/cFE/pull/1989> and <https://github.com/nasa/cFS/pull/371>

## Development Build: v6.8.0-rc1+dev1024
- CFE_Assert macro names
- Update msgid testcase to match implementation 
- Single time domain in functional time tests
- Add missing inclusions in CFE API headers 
- Use existing /ram for FS header test
- Add static local to function test so data section is nonzero
- Make invalid buffer length consistent in es task test
- Only check base filename in library info functional
- Confirm sb/time reset requirements in coverage test
- Avoid alias warning on some compilers
- Fix broken link in Application Developers Guide
- See <https://github.com/nasa/cFE/pull/1967> and <https://github.com/nasa/cFS/pull/359>

## Development Build: v6.8.0-rc1+dev994
- Update directory diagrams in cFE Application Developers Guide 
- Improve SB coverage
- Improve ES code coverage
- Update cFE Application Developers Guide for message module
- Update documentation for message map hash implementation 
- Update SB/MSG tests to verify "Not Implemented" functions
- See <https://github.com/nasa/cFE/pull/1939> and <https://github.com/nasa/cFS/pull/351>

## Development Build: v6.8.0-rc1+dev980
- Add SB API test cases
- Add ES API test cases
- Add TBL API test cases
- Add FS API test cases
- Add Time Clock Test
- Add EVS coverage test for MessageSendCounter not incrementing when sending a filtered event.
- SB test improvements: 
  - unsubscribe of single pipe ID, confirms other subscriptions not changed
  - Confirms that MsgLimit and PipeDepth are both honored and that delivery to other (open) pipes is not affected when some pipes have reached delivery limits
  - Adds use of CFE_SB_PEND_FOREVER in some cases
  - Adds a sequence number validation to the Zero copy test.
- Verify that CFE_TBL_Modified calculates and sets the table CRC and the Updated flag
- Add call to CFE_ES_ExitChildTask in es_task_test
- Incorrect OSAL Format in Users Guide Reference
- Improve EVS code coverage
- Improve FS coverage
- Improve MSG branch coverage
- Improve resource ID branch coverage 
- Improve SBR branch coverage
- Improve TIME branch coverage
- Improve TBL code coverage
- Support custom PSP directory in build system
- Update time tests to use bitmask check macros
- See <https://github.com/nasa/cFE/pull/1885> and <https://github.com/nasa/cFS/pull/348>

## Development Build: v6.8.0-rc1+dev933
- Add software bus tests 
- Separate variable for OSAL status 
- Increase SB pool max size bucket 
- Add Null check for CFE_ResourceId_FindNext 
- Improve tbl coverage tests 
- Add missing cases for msg id func tests 
- Consolidate msg get/set doxygen group 
- Adding coverage tests to cFE TIME 
- Enable strict resource id w/OMIT_DEPRECATED
- Hotfix for strict resource IDs. 
- HOTFIX IC-20210819, type correction TBL coverage test
- Consolidate msg get/set doxygen group
- Add test for ES BackgroundWakeup
- Success Test for CFE_ES_RestartApp
- Success Test for CFE_ES_GetMemPoolStats
- Subscribe to Message Limit Greater Than CFE_PLATFORM_SB_DEFAULT_MSG_LIMIT
- Group MSG APIs documentation by header type
- Add retroactive CFE status asserts
- Correct syslog message in pool create
- Check error ctr to TransmitMsg test
- End Child Task requirement remove error code
- Add functional tests for resource misc
- Remove unused CFE_TBL_ERR_BAD_APP_ID
- Remove unused CFE_ES_POOL_BOUNDS_ERROR
- Remove unused CFE_TBL_ERR_FILE_NOT_FOUND
- Remove comment referencing CFE_TBL_ERR_FILE_NOT_FOUND
- See <https://github.com/nasa/cFE/pull/1839> and <https://github.com/nasa/cFS/pull/340> 

## Development Build: v6.8.0-rc1+dev873
- Add CFE assert macros to functional test
- Adds invalid id syslog to for CFE_ES_DeleteApp and CFE_ES_ReloadApp and verifies required reporting 
- Stop memory leak & add cds size test. 
- Mark read only inputs as const 
- Check resource ID idx is less than max 
- Update CFE_ES_RunLoop documentation 
- Add Message Api Functional Test
- Partial #1724, update in/out status and nonnull/nonzero tags
- Add External Time Source Functional Tests
- Add Perf API functional tests 
- See <https://github.com/nasa/cFE/pull/1808> and <https://github.com/nasa/cFS/pull/337> 

## Development Build: v6.8.0-rc1+dev844
- Move global count into test global struct
- Add ES application control API functional tests 
- Add null pointer check to CFE_ES_ReloadApp functional test 
- Add Generic Counter API test 
- Update coverage test to use UtAssert macros 
- Add resource ID API functional tests 
- Fix last char truncated in coverage log output 
- Fix typos in some copyright headers 
- Add misc time api functional test 
- Add Functional Test for EVS Reset Filters API
- Replace cFE_FTAssert_VOIDCALL with new UtAssert_VOIDCALL
- Remove multiple instantiations of CFE_FT_Global
- Add functional tests for cFE Table APIs
- See <https://github.com/nasa/cFE/pull/1759> and <https://github.com/nasa/cFS/pull/328>

## Development Build: v6.8.0-rc1+dev810
- Add null pointer check to table GetAddresses and ReleaseAddresses
- Explain FS Header offset & add offset functional tests.
- Handle fail status in GetTypeFromMsgId
- Add Functional Test for EVS Send Event API
- Remove SB get last message sender info requirement
- Update UTs to use UtAssert_MIR
- Add Functional Tests cFE Message ID
- Add Time Conversion Functional Test
- See <https://github.com/nasa/cFE/pull/1759> and <https://github.com/nasa/cFS/pull/328>

## Development Build: v6.8.0-rc1+dev789
- Correct return code check
- Documentation for FS APIs that return OSAL codes
- Adding coverage tests for cfe_es_apps.c
- Add File Utility Functional Tests.
- Add custom epoch support to TIME UT
- Requirements update for Caelum
- Add null check to CFE_ES_TaskID_ToIndex.
- Scrub command documentation
- Add time arithmetic functional tests
- Update docs for CFE_FS_BackgroundFileDumpRequest
- Correct return code mismatches
- See <https://github.com/nasa/cFE/pull/1701> and <https://github.com/nasa/cFS/pull/313>

## Development Build: v6.8.0-rc1+dev762
- Change CI to use Test Log.
- remove default .dat extension
- Update API doxygen list
- update documentation for CFE_ES_GetPoolBufInfo
- CFE_SB_MsgHdrSize returns size_t
- improve SB coverage test
- correct function name in UT_BSP_Unlock
- See <https://github.com/nasa/cfe/pull/1665> and <https://github.com/nasa/cfs/pull/311>

## Development Build: v6.8.0-rc1+dev746
- Size unit test table load buffer based on config
- Add SB Pipe Management Functional Tests
- See <https://github.com/nasa/cFE/pull/1661> and <https://github.com/nasa/cFS/pull/297>

## Development Build: v6.8.0-rc1+dev739
- Change index type to resolve infinite loop warning
- Adding coverage for cfe_es_task.c
- Expand FS Header Functional tests.
- See <https://github.com/nasa/cfe/pull/1654> and <https://github.com/nasa/cfs/pull/287>

## Development Build: v6.8.0-rc1+dev726
- correct path to users guide warning log
- add additional test cases for Child Tasks
- Add cfe functional tests to CI
- rename/clean CFE coverage assert macros
- Added UT tests for cFE ES Api
- Expand CDS Functional Tests.
- Event ID updates
- add test log file
- scrub all UT_Report calls
- See <https://github.com/nasa/cFE/pull/1632> and <https://github.com/nasa/cfs/pull/270>

## Development Build: v6.8.0-rc1+dev693
- Add CI workflow to run cFE coverage tests
- Updated FS Read/WriteHeader API return documentation
- Document CFE_ES_RunLoop increment task counter behavior
- standardize on "docs" subdirectory
- Update version description per current design
- Document CFE_TBL_Unregister use-case
- provide CFE assert lock/unlock
- Simplify CFE_FS_SetTimestamp and fix syslog typo
- Removed redundant check/set of CFE_CPU_ID_VALUE
- add time get reference error bit
- Add workflow to build cFE documentation
- Requirements updates
- ES/ResourceID documentation cleanup
- allow multiple sources in add_cfe_coverage_test
- add ES Misc Functional test
- add Mempool functional tests
- Clean/simplify version header and reporting
- doxygen cleanup
- Prepend system log messages with function name
- See <https://github.com/nasa/cFE/pull/1619> and <https://github.com/nasa/cFS/pull/265>

## Development Build: v6.8.0-rc1+dev642
- Update ES verify errors to match
- Ensures error messages in `cfe_es_verify.h` match test logic.
- Resolve mismatched `endforeach` of `CMakeList.txt`
- Improve event filter documentation
- Adds documentation for the behavior of CFE_EVS_ResetFilter and explains how Event Filter Mask Values work.
- EVS/FS documentation cleanup
- Remove shell file subtype and clarify scope
- Removes CFE_FS_SubType_ES_SHELL and clarifies the scope of CFE_FS_SubType_Enum_t so that it only includes the cFE-defined file types
- Simplify CFE_ES_QueryAllCmd file open logic
- Removes the obsolete OS_OpenCreate and file-remove-on-success logic from `CFE_ES_QueryAllCmd`
- Remove deprecated elements
- Replace Header Content Type magic number
- Remove unused EVS LogMode defines
- Clean up SB, MSG, and SBR documentation and remove unused `CFE_SB_NO_SUBSCRIBERS`
- Add `CFE_SB_DestinationD` tag and use for pointers
- Use `CFE_MSG_SequenceCount_t` for seqcnt type
- Document `CFE_ES_PoolCreateEx` NumBlockSizes error handling
- Documentation cleanup in TBL/TIME
- Document `TblName` as app specific
- Use OSAL script to generate API guide
- Include verify headers to validate config
- Add doxygen aliases for OSAL parameter/retvals
- See <https://github.com/nasa/cFE/pull/1584> and <https://github.com/nasa/cFS/pull/263/>

## Development Build: v6.8.0-rc1+dev593
- Uses `int` with %d conversions inUtAssert_True and UtPrintf for platform portability.
- Specifying the special string NULL as the entry point in a startup script results in no entry point being called for the library. Equivalent to leaving the field empty.
- [build system] Adds a hook for an `install_custom.cmake` script that can be added to a CPU-specific subdirectory under the "defs" directory. This hook can perform extra installation steps for custom implementations. Tweaks the `add_cfe_tables` function so it can be called from the `install_custom.cmake` script to generate additional table binaries for that CPU.
- [build system] `add_cfe_tables` now uses the "APP_NAME" parameter to associate the table files with the app library, so the same set of include files can be used. Still allows any unique string to be used as "APP_NAME" for backward compatibility. The script will now generate a "Note" message to the user if it does not match an application name.
- If the multiple table feature is used, it actually needs to match the application name, or else the include paths may be incomplete.
- Removes discrepancies (return type, parameter names, etc) between function prototypes and implementation. Also fixes some but not all use of CFE_Status_t in the implementations. Updates ut-stubs accordingly.
- See <>https://github.com/nasa/cFE/pull/1568> and <https://github.com/nasa/cFS/pull/260>

## Development Build: v6.8.0-rc1+dev580
- Adds `CFE_MSG_GetNextSequenceCount` so the auto-increment of the local sequence counter works when sending tlm (and increment is enabled). Updates unit tests and adds the old-style stub. The unit tests check for the correct rollover behavior.Sequence count will roll over based on the mask. Before the fix the sequence counter would "stick" in telemetry until the passed in value rolled over.
- [docs] Correctly format code block section terminator so Markdown renders correctly
- [docs] Fix typos in developer guide
- See <https://github.com/nasa/cFE/pull/1508> and <https://github.com/nasa/cFS/pull/259>

## Development Build: v6.8.0-rc1+dev575
- Removes broken travis-ci script
- Updates CFE core stub libraries to use stubs generated from new UT Assert script. Converts existing stub logic to
a default handler routine.
- Moves all of the internal API prototypes from "core_api" to the "core_private" interface lib.
- Moves `CFE_FS_RunBackgroundFileDump` to internal API
- Renames hook files to handler
- [docs] Adds documentation and comments about how the configuration facility works and where to find information on the complete set of options available
- Updates the configuration files in "sample_defs" to reflect only those parameters that are set to a non-default value along with information about why the configurable item is changed.
- Removes --quiet option so files checked go to stdout
- See <https://github.com/nasa/cFE/pull/1492> and <https://github.com/nasa/cFS/pull/256>

## Development Build: v6.8.0-rc1+dev559
- Adds tests for nominal use cases of the ES Critical Data Store API.
- Adds functional tests for nominal uses of FS Header API.
- Adds functional tests for Time Current API.
- [docs] Makes comment blocks in source and header files more consistent for all internal, CFE core APIs. Moves information about the function behavior to its prototype in the header in doxygen format. Comment blocks on the function implementation refer back to the prototype, it does not duplicate the info. Local helper functions that are not separately prototyped, are exceptions to this pattern. Adds intended scope to all functions: global, application-internal, or file/local.
- See <https://github.com/nasa/cFE/pull/1481> and <https://github.com/nasa/cFS/pull/252>

## Development Build: v6.8.0-rc1+dev540
- Changes the type of pointer for `MemPtr` in `CFE_ES_PoolCreateNoSem` API from uint8* to void* to be more consistent and easier to use. Should be backward compatible.
Updates the doxygen documentation for this parameter, as it was incorrectly specifying a 32-bit alignment requirement.
- Adds new functional tests for ES Child Task API. Does not check edge cases. Fixed spelling mistake in `UtAssert_ResourceID_Undifeined` name
- Removes BUILDDIR reference and an old comment. No behavior changes
- Moves and renames `cfe_resourceid_basevalue.h` to `cfe_resourceid_basevalue.h`. Since it is assumed/required that resource IDs follow the "osal compatible" pattern. Perhaps in a future version this could change, but
- Ensures that the `CFE_SUCCESS` constant is of the `CFE_Status_t` type. Since an explicit cast is required on all error codes that are expected to be negative values.
- Removes unused error codes: `CFE_ES_ERR_SHELL_CMD` and `CFE_SB_NO_MSG_RECV`. No impact to behavior.
- When a startup file has a line with too many tokens the build script will generate a concise warning including an indicator of which line is causing the problem.
- Confirm that the call to `CFE_ES_CDS_CachePreload` returns `CFE_SUCCESS` before continuing. No behavior changes. Now shows up as untested lines in the coverage report since error condition cannot be exercised through coverage.
- [docs] Clarify that `CFE_ES_DeleteCDS` does not wipe or erase the block, it only returns resources to the pool for re-use.
- [docs] Adds comments in `CFE_ES_RunExceptionScan` describing the logic when an exception cannot be traced back to a specific app, in that it should fall back to the PSP reset.
- `CFE_ES_GenPoolInitialize` now returns ` CFE_ES_BAD_ARGUMENT` error if the `AlignSize` passed-in value is not actually a power of two instead of "fixing" the alignment mask,
- Replace internal `CFE_ES_SYSLOG_APPEND` macro with the `CFE_ES_WriteToSysLog()` API since coding standards discourage use of multi-line macros.
- [docs] Improve Resource IDs documentation. Specifically on use of the various helper functions and common patterns Documents that the "IsMatch()" functions accept NULL pointers so they can be used with initial validation (gatekeeper). All other helper functions assume a non-NULL pointer.
- Compiler will catch if the `CFE_RESOURCEID_MAX` value changes in such a way that makes it not usable as a bit mask as intended. Add a compile time assert to ensure that `CFE_RESOURCEID_MAX` value is one less than a power of two  (i.e. an LSB-justified bit mask). Notes in the comments that it serves as both a numeric limit and a mask.
- See <https://github.com/nasa/cFE/pull/1431> and <https://github.com/nasa/cFS/pull/250>

## Development Build: v6.8.0-rc1+dev509
- Separates the list of CFE core interface modules (e.g. core_api) from the list of CFE core implementation modules (e.g. msg). This allows the content of core_api to be expanded to locally include any additional modules the user has added to cFE core via the `MISSION_CORE_MODULES` list.
- Adds documentation for `CFE_ES_RegisterCDS()` regarding clearing.
- Removes the separate CFE "testrunner" module and moves the logic associated with running a test into cfe_assert library. Converts the "testcase" module from a library into an app, by calling into the runner logic that is now inside cfe_assert. Each functional test is a separate app, not a library, so it can be started and stopped via ES command like any other app.
- Removes check on `ENABLE_UNIT_TESTS=true` when building "cfe_assert", it should be built all the time.
- See <https://github.com/nasa/cfe/pull/1406> and <https://github.com/nasa/cfs/pull/248>

## Development Build: v6.8.0-rc1+dev498
- Reports test failures as CFE events. Test status messages are now sent as Events rather than Syslog. This allows for more processing capability, and allows failures to be received externally (e.g. ground system).
- See <https://github.com/nasa/cFE/pull/1295> and <https://github.com/nasa/cFS/pull/242>

## Development Build: v6.8.0-rc1+dev494
- Adds new tests for the ES Info APIs
- See <https://github.com/nasa/cFE/pull/1284> and <https://github.com/nasa/cFS/pull/238>

## Development Build: v6.8.0-rc1+dev490
- Removes `PspConfig` member from the `GLOBAL_CONFIGDATA` object. Updates the only remaining reference to this object inside the CFE_PSP_VERSION and uses the new Version API instead. Updates the OSAL version print to use the version API call and CFE uses the macro directly.
- Replaces duplicate mention of the removed `CFE_SB_ZeroCopyGetPtr` with the correct removal target of `CFE_SB_ZeroCopyReleasePtr`
- See <https://github.com/nasa/cFE/pull/1279> and <https://github.com/nasa/cFS/pull/233>

## Development Build: v6.8.0-rc1+dev484
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

## Development Build: v6.8.0-rc1+dev436
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

## Development Build: v6.8.0-rc1+dev412
- Apply standard code style and format
- Add new continuous integration workflow to enforce this format
- See <https://github.com/nasa/cFE/pull/1219>

## Development Build: v6.8.0-rc1+dev402
- HOTFIX 20210312, updates to work with older CMake
- reorganize directory structure
- HOTFIX IC 2021-03-05: Correct static app build issue
- See <https://github.com/nasa/cFE/pull/1222>

## Development Build: v6.8.0-rc1+dev392
- update pipe name documentation.
- remove configs about shells
- Update CRC documentation
- add stack size and priority to task info …
- refactor target config objects …
- Add wrapper targets to simplify app builds …
- Add Testing Tools to the Security Policy
- See <https://github.com/nasa/cFE/pull/1213>

## Development Build: v6.8.0-rc1+dev382
- Refactors the SB buffer descriptor object `CFE_SB_BufferD_t` and simplify the zero-copy buffer paradigm. Combines the zero-copy and the normal CFE buffer descriptor into a single unified `CFE_SB_BufferD_t` object. Results in a simpler zero-copy design that is similar to the standard, non-zero-copy message path. All message descriptor objects are now tracked in a list by SB. All changes are internal to SB. This does not affect API or behavior of any existing APIs (but see note). Corrects a minor issue where the `MsgSendErrorCounter` would get incremented if there were no subscribers, but only in the zero copy API.  
- Replaces `int_32` with `CFE_Status_t` for all error message codes  
- Removes references to `cfeesugshellsrv` from user guide
- Adds null pointer checks and size checks to validate method parameters. Returning the input instead of an error code
- Removes use of `LogEnabled` element in HK telemetry for EVS logic since log is always enabled now. On failures, reset area or semaphore will panic.
-  Fixes various build warnings when `BUILDTYPE=release`.
- See <https://github.com/nasa/cFE/pull/1196>

## Development Build: v6.8.0-rc1+dev365
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

## Development Build: v6.8.0-rc1+dev348
- Corrects reference to PSP header file location. Build now successfully completes the build succeeds again when using `add_psp_module()` in custom CMakeLists file.
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
- Explicitly `memset` the task data to zero at the start of EarlyInit. Standardize the global typedef/variable names.
- Moves all functions, macros, types, and other definitions related to resource IDs and generic resource management into a separate module, like `CFE MSG`, `SBR`, etc. This allows a mission to elect "strict" implementations of these objects, where every ID type is unique, and assigning between them or `uint32` results in a compiler error. **API now has separate types for each resource type (Apps, Tasks, Libs, Counters, etc).** The user can elect at the mission level whether this is a simple typedef (all uint32, all interchangeable) or a wrapper type (separate/strict type, cannot be interchanged). The former is backward compatible but the latter is not - must use proper types.
- Adds Code QL analysis to continuous integration workflow.
- See <https://github.com/nasa/cFE/pull/1150>

## Development Build: v6.8.0-rc1+dev290
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
- None of these changes are expected to be problematic.
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

## Development Build: v6.8.0-rc1+dev248
- Replace `OS_FileSysStatVolume()` with`OS_fsBlocksFree()` which will be deprecated. This call reports the number of total blocks, not just the free blocks, making the check more accurate and removing the need for a workaround for desktop machines.
- Instead of accessing `OS_time_t` values directly, use the OSAL-provided conversion and access methods. This provides independence and abstraction from the specific `OS_time_t` definition and allows OSAL to transition to a 64 bit value.
- Removes the spurious `CFE_SB_TimeOut_t` typedef from `cfe_sb.h`. May affect any apps that inappropriately rely on the private typedef.
- Removes unused `network_includes.h`. Not used by the framework anywhere,  apps should use OSAL Socket APIs instead.   
- Fixes deprecation directive typos
- See <https://github.com/nasa/cFE/pull/1088>

## Development Build: v6.8.0-rc1+dev236
- Resolved doxygen warnings for osalguide and updated header file references
- Corrects the documentation for the CFE_SB_GetPipeName() unit test stub function.
- Adds a new github actions workflow file to run cppcheck
- See <https://github.com/nasa/cFE/pull/1066>

## Development Build: v6.8.0-rc1+dev228
- Remove use of `osapi-os-loader.h` from ES UT.
- Use volatile `sig_atomic_t` for system state to avoid race issue if uint32 isn't atomic on a system
- Set the flags parameter on the OS_ModuleLoad() properly to allow an app to be properly unloaded, which in turn allows the reload command to work as expected. Fixes problem where unload command resulted in continuous restarting of the same app code.
- Replaced `Test_MSG_PrintMsg` with `UT_DisplayPkt`. Also removed unused `Test_MSG_Sum`.
- See <https://github.com/nasa/cFE/pull/1047>

## Development Build: v6.8.0-rc1+dev218
- Adds `CFE_SB_TransmitMsg`, `CFE_SB_TransmitBuffer`, `CFE_SB_ReceiveBuffer`
  - Main change is to utilize `CFE_SB_Buffer_t` and `CFE_MSG_Message_t` in a consistent manner to facilitate alignment
  - Deprecates multiple `CFE_SB_*` items
  - Redefines `CFE_MSG_Size_t` as `size_t` to minimize duplicated work and facilitate transition to `size_t`
- Use a generic void* as the interface type for the pool buffer pointers. This reduces the need for local type casting in the apps and makes it generally easier to use.
- Remove  reference to CEXP in RTEMS 4.11 i686 toolchain. Add an RTEMS 5.1 i686 toolchain file.
- See <https://github.com/nasa/cFE/pull/1045>

## Development Build: v6.8.0-rc1+dev204
- Backward compatible API change. Replace many uses of generic uint16 and uint32 with a more purpose-specific type. Replace all sizes with size_t across the API.
- Rename `UT_SetForceFail` to `UT_SetDefaultReturnValue` since some functions that retain more than 1 value are not necessarily failing
- Deprecates many SB Elements and replaces them with the new MSG module. See https://github.com/nasa/cFE/issues/777 for list.
-  App and Lib info telemetry structures no longer contain the `ModuleId` value from OSAL.
- Add an extra write of a null char after `strncpy` which squelches a warning and appease compiler warning logic.
- Uses `CFE_PLATFORM_ES_DEFAULT_STACK_SIZE` as a default instead of a minimum. Affects the Start App command; if stack size is specified as zero, then the default stack size value from platform config is used. Otherwise the value in the command will be passed through and used as-is.
- Changes the type of the AppID parameter on "Cleanup" routines from `uint32` to `CFE_ES_ResourceID_t`.
- See <https://github.com/nasa/cFE/pull/1027>

## Development Build: v6.8.0-rc1+dev179
- Adds macros for more compact calls to `CFE_EVS_SendEvent`, making the type be part of the function name.
- The sample configs leap seconds default value is now up to date. (As of Oct 2020)
- Removed the clear=false logic (and clear parameter) `CFE_MSG_Init()` now always zeroes entire message and sets defaults.
- Adds flags parameter to calls to `OS_ModuleLoad()`. Initially just pass 0 (GLOBAL) to maintain old behavior.
- Updates `CFE_ES_RegisterCDSEx` stub to match current signature
- Includes `cfe_private.h` for stubs that implement related elements.
- See <https://github.com/nasa/cFE/pull/1008>

## Development Build: v6.8.0-rc1+dev164
- Keeps task names under 16 chars to make more debugger friendly, regardless
of the OSAL limit. Task name shows up as `ES_BG_TASK`
- Move ES typedefs shared across API and telemetry messages into the `cfe_es_extern_typedefs.h`.
- Move all ES typedefs that define the telemetry interface and structures that define the output of commands that write data files into this group (query all apps, query all tasks, query all CDS).
- Remove some localized definitions and replace with MISSION scope definitions where appropriate/necessary.
- Adjust `strncpy()` call to avoid compiler warning
- Cast fixed width types to the type used in the `printf` call. Removes `printf` type warnings on the 32-bit RTEMS build.
- See <https://github.com/nasa/cFE/pull/991>

## Development Build: v6.8.0-rc1+dev150
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

## Development Build: v6.8.0-rc1+dev139
- For all resource types which have names, IDs are not re-issued after deletion, helping ensure safety as previously deleted IDs will not validate. Provides a consistent Name-ID translation API for all resource types. Enforces consistent argument/name validation on all resource types, and also enforces name uniqueness where relevant.
- Enhancement to use the full 16 bits of resource ID space, which avoids repeating ID values that have already been used. This significantly decreases the likelihood that a previously deleted ID will alias a newly allocated/valid ID.
- See <https://github.com/nasa/cFE/pull/959>

## Development Build: v6.8.0-rc1+dev129
- Rather than having a second pool implementation only for CDS, use the generic pool implementation. This also uses the abstract resource identifiers to identify CDS blocks, rather than a direct reference.
- Add the system-specific module suffix (.o, .so, .obj, etc) and the default CFE core executable name to the configdata structure.
- See <https://github.com/nasa/cFE/pull/944>

## Development Build: v6.8.0-rc1+dev122
- Adds the field `UnregAppID` to track whether an "unregistered" event was generated, un-overloading the EventCount field to serve its primary purpose of counting actual events generated from a valid/registered AppID.
- Move the AppID lookup execution to be early in the `CFE_SB_SendMsgFull` implementation. This avoids double locking between SB+ES and avoids a block-scope local variable.
- Instead of identifying a memory pool by its memory address, use a resource ID. IDs are a constant size, regardless of whether the host machine is 32 or 64 bits.
  - IDs can be put into commands/telemetry and maintain a more consistent format with consistent alignment requirements.
  - IDs can be independently verified without dereferencing memory. Previously the only way to validate a memory pool was to read the address pointed to, which results in a segfault if the address was bad.
- Change from `OS_MAX*` defines to appropriately-scoped CFE defines for array sizing
- This creates the new `CFE_Status_t` typedef for function's return status codes. Also adds a note to `CFE_TBL_GetStatus` since its behavior will likely change in the future in the hopes of not having a non-zero "info" status.
- See <https://github.com/nasa/cFE/pull/936>

## Development Build: v6.8.0-rc1+dev109
- Add a new typedef `CFE_ES_ResourceID_t` that can replace `uint32` for all ID storage and manipulation. Initially this is just an alias to `uint32` for backward compatibility.
- See <https://github.com/nasa/cFE/pull/916>

## Development Build: v6.8.0-rc1+dev105
- Removes dependency on CCSDS version define.
- Removes old name and id defines.
- CFE_ES_CalculateCRC default stub behavior.
- Replaces calls to OS_open and OS_creat
- Replaces UT_Text with UtPrintf
- Updates variable checks in read_targetconfig

- See <https://github.com/nasa/cFE/pull/912>

## Development Build: v6.8.0-rc1+dev91
- Sets Revision to 99 for development build.
- Installs unit test to target directory.
- Returns processor ID to default to unbreak toolchain
- Applies the appid/taskid/counterid pattern to Library resources.
- See <https://github.com/nasa/cFE/pull/891>

## Development Build: v6.8.0-rc1+dev81
- Deconflict CFE_ES_LIB_ALREADY_LOADED and CFE_ES_ERR_SYS_LOG_TRUNCATED EIDs
- Scrub all CFE references/uses of OSAL IDs to use the proper osal_id_t type. Any place that an OSAL ID is stored in memory or passed in an API call are changed to the osal_id_t type, rather than uint32. Conversions between this and other types (e.g. bare integer) is done using the OSAL-supplied conversion helpers.
- After the changes implemented in #101, there may be routing table entries with no subscribers (RoutePtr->ListHeadPtr would be NULL.) This could cause a seg-fault. Also, even if there are entries in the routing table, there will be no event generated if the unsubscribe does not find a matching route entry.
- Adds debug message.
- Applies the appid/taskid pattern to Generic Counter resources.
- Adds test for SB subscribe/unusubscribe/unsubscribe.
- See <https://github.com/nasa/cFE/pull/876>

## Development Build: v6.8.0-rc1+dev65
- In the next major CFE release, this code will be no longer supported at all. It should be removed early in the cycle to avoid needing to maintain this compatibility code.
- The CFE_ES_FindCDSInRegistry function had an unusual loop control structure with mixed types of signed and unsigned. This has the possibility of being infinite if the MaxNumRegEntries is zero due to the way the end condition is structured. Simplify to be like other loops and use unsigned int control variable.
- Fixes the cast-align error (use the aligned Msg since it's available already).
- HOTFIX-20200902 - Fix sb unit test setup issue.
- HOTFIX 20200902 - Update documentation links for deprecated symbols.
- HOTFIX 20200902 - Fix SB Test_CleanupApp_API AppID.
- See <https://github.com/nasa/cFE/pull/861>

## Development Build: v6.8.0-rc1+dev42
- Removes reference from documentation.
- CFE_SB_SendMsg stub now behaves the same as CFE_SB_TimeStampMsg (copies message pointer from local). No longer need to emulate CFE_SB_InitMsg from test code, set the API/stub data buffers directly.
- Removed iterator modification from within the loop... replaced with break.
- Resolves loop iterator size too small for comparison.
- Replaced CFE_MISSION_SPACECRAFT_ID use with CFE_PSP_GetSpacecraftId() and updated unit test
- See <https://github.com/nasa/cFE/pull/849>

## Development Build: v6.8.0-rc1+dev28
- Add msg stubs, update SB_UT to use them, and remove msg module include from unit tests
- Collapses time options down to just 32 bit second, 16 bit subsecond, always big endian. Removes old defines, and triggers an error if the configuration is set to a format that was removed.
- Enables source selection and out-of-tree mission-defined overrides in the msg directory
- Unit tests added from within unit tests will not execute, replaced this pattern with direct calls to the main subtest setup routine.
- See <https://github.com/nasa/cFE/pull/833>

## Development Build: v6.8.0-rc1+dev13
- Deprecates `CFE_SB_GetLastSenderId()` API by introducing new `CFE_OMIT_DEPRECATED_6_8` tag
- Documentation update remove deleted requirements
- Add a new *cfe_assert* module for functional testing by making it possible to load the UT assert object code as a CFE library. These are compiled as separate, independent modules and only loaded on demand. Also includes a draft example for cFE testing, which calls some basic ES AppId functions.
- See <https://github.com/nasa/cFE/pull/816>

## Development Build: v6.7.0+dev292
- Add missing include path to the target/h and wrn/coreip directory.
Set and clarify difference between WIND_HOME and WIND_BASE variables.
Remove unrelated comment about CEXP (remnant from RTEMS). No more errors about missing headers.
- Version reporting is does not span multiple lines.
- See <https://github.com/nasa/cFE/pull/792>

## Development Build: v6.7.0+dev289
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

## Development Build: 6.7.21
- If a string is exactly the size of the field when using the `CFE_TBL_FILEDEF()` macro it will produce a compiler error
- Added cFE User's Guide Reference to README.md
- Removes old license
- See <https://github.com/nasa/cFE/pull/743>

## Development Build: 6.7.20
- SB Unit use of the UT assert framework is closer to original design intent
- See <https://github.com/nasa/cFE/pull/743>

## Development Build: 6.7.19
- API Change: cFE ut_sb_stubs now has CFE_SB_DeletePipe available.
Behavior Change: App unit tests requiring this will not fail to build due to undefined reference to CFE_SB_DeletePipe
- Hook functions may now use the va_list form and obtain the full set of variable arguments passed to CFE_EVS_SendEvent and variants.
- Replace all direct references to data types defined in ccsds.h with the abstract type defined in cfe_sb.h.
- See <https://github.com/nasa/cFE/pull/729> for details.

## Development Build: 6.7.18
- Using ut stubs CFE_EVS_SendEvent and CFE_ES_WriteToSysLog, the register buffer will have the correct size. access to register buffer element will exist
- Both the main task and the child task(s) are successfully deleted and restarted after the exception occurs.
- Fixes doxygen warnings for the tbl subsystem.
- No compiler warnings or errors on cross build.
- Changes Message Key from uint16 to uint32 to avoid rollover and system hang
- See <https://github.com/nasa/cFE/pull/712> for more details

## Development Build: 6.7.17
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

## Development Build: 6.7.16
- Users must now select OSAL options via the CMake file in their defs directory, rather than the osconfig.h file.
- See <https://github.com/nasa/cFE/pull/672> for more details

## Development Build: 6.7.15
- Upon power on reset, default system log mode set to overwrite. Upon processor reset, default system log mode set to discard.
- No longer locks while locked (no issue observed on linux/posix, but user reported issue on FreeRTOS 10)
- Internal `CFE_TBL_LoadFromFile()` API changed slightly to add AppName as a parameter. Return value from `LoadFromFile()` no longer relevant for event generation.
- Updates `CFE_TBL_CleanUpApp` such that it now checks the 'used flag' prior to calling `CFE_TBL_RemoveAccessLink` for a given TblHandle. Also sets the AppId to `CFE_TBL_NOT_OWNED` after removing the access descriptor link from linked list.
- Removed `OS_FS_SUCCESS, OS_FS_ERROR , OS_FS_ERR_INVALID_POINTER, OS_FS_ERR_NO_FREE_FDS , OS_FS_ERR_INVALID_FD, and OS_FS_UNIMPLEMENTED` from `osapi-os-filesys.h`
- See <https://github.com/nasa/cFE/pull/649> for more details

## Development Build: 6.7.14
- Exposes the `CFE_SB_IsValidMsgId()` for application usage.
- `CFE_SB_GetLastSenderID` will now detect if it is being called prior to a message being sent on a given pipe.
- Mismatches between PSP/BSP/OS are now detected and warned about during make prep. Only the `CFE_SYSTEM_PSPNAME` is actually required to be specified for a CFE build now. Others can be omitted.
- See <https://github.com/nasa/cFE/pull/635> for more details

## Development Build: 6.7.13
- RTEMS builds without error.
- Use the INTERFACE_COMPILE_DEFINITIONS and INTERFACE_INCLUDE_DIRECTORIES properties from the osal target and apply them to the entire CFE build as a directory-scope property. No impact until these are set in OSAL.
- Minor other updates (see <https://github.com/nasa/cFE/pull/615>)

## Development Build: 6.7.12
- Cmd code (and checksum) are always in the same place (matches GSFC spec for command secondary header)
- No impact to behavior. Previously the perf log dump file frequently contained errors due to out of order or otherwise corrupted entries, which is now fixed.
- Minor other updates (see <https://github.com/nasa/cFE/pull/586>)

## Development Build: 6.7.11
- Improve documentation
- Update makefile to report branch coverage
- Minor other updates (see <https://github.com/nasa/cFE/pull/566>)

## Development Build: 6.7.10
- Fix potential unit test problems with name collisions
- Improve documentation
- Minor other updates (see <https://github.com/nasa/cFE/pull/563>)

## Development Build: 6.7.9
- No longer requires sed "hack" to change the setting in default_config.h
- Minor other updates (see <https://github.com/nasa/cFE/pull/544>)

## Development Build: 6.7.8
- Updates and cleaned up documentation and requirements
- Fixes incorrect debug messages
- Decrease child task count when one is deleted
- Minor other updates (see <https://github.com/nasa/cFE/pull/530>)

## Development Build: 6.7.7
- Adds a new function, CFE_SB_GetPipeIdByName, which retrieves the pipe ID given a name of a pipe.
- Improvement in error reporting when using a pipe name that is already in use, or when the queue limit has been reached.
- Added userguide and osalguide to the local target list to avoid makefile warning
- Minor other updates (see <https://github.com/nasa/cFE/pull/511>)

## Development Build: 6.7.6
- Adds OMIT_DEPRECATED prep flag
- Adds and enforces strict warnings
- Software Bus now increments sequence counter even if there are no subscribers
- Warning, comment, and code coverage cleanup (see <https://github.com/nasa/cFE/pull/490>)

## Development Build: 6.7.5
- Added CI static analysis check
- Resolved static analysis warnings
- Minor other updates (see <https://github.com/nasa/cFE/pull/479>)

## Development Build: 6.7.4
- Minor updates (see <https://github.com/nasa/cFE/pull/448>)

## Development Build: 6.7.3
- Minor updates (see <https://github.com/nasa/cFE/pull/413>)

## Development Build: 6.7.2
- Minor bugs and enhancements (see <https://github.com/nasa/cFE/pull/388>)

## Development Build: 6.7.1
- Fix strlen in CFE_ES_TaskInit <https://github.com/nasa/cFE/pull/23>
- Minor bug fixes (see <https://github.com/nasa/cFE/pull/378>)

## **_OFFICIAL RELEASE: v6.7.0 - Aquila_**
- This is a point release from an internal repository
- Changes are detailed in [cFS repo](https://github.com/nasa/cFS) release documentation
- Apache 2.0

## **_OFFICIAL RELEASE: v6.6.0a_**
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
