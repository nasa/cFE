---
title: Core Flight Software Version Description Document
tags: cFE
abstract: |    
    ![cFS Logo](./media/image1.png){width="2.75625in" height="2.0243in"}
    
    \begin{center}
    \LARGE
    core Flight Executive (cFE)
    
    Version 6.6.0
    \normalsize
    \end{center}
    \newpage                            
---

\newpage

# Signatures {-}

\par\vspace{0.25in}

## Submitted By {-}

\par\vspace{0.25in}

| ___________________________________
| Susanne Strege/GSFC-5820
| cFS Flight Software Development Lead


## Approved By {-}

\par\vspace{0.25in}

| ___________________________________
| Joseph Hickey/GRC-LSS0
| cFS Configuration Control Board

\par\vspace{0.25in}

| ___________________________________
| Christopher Knight/ARC-TI
| cFS Configuration Control Board

\par\vspace{0.25in}

| ___________________________________
| Steve Duran/JSC-ER611
| cFS Configuration Control Board

\par\vspace{0.25in}

| ___________________________________
| Chris Thames/LARC-D207
| cFS Configuration Control Board

\par\vspace{0.25in}

| ___________________________________
| Scott Tashakkor/MSFC-ES52
| cFS Configuration Control Board

\par\vspace{0.25in}

| ___________________________________
| Chris Monaco/JHU-APL
| cFS Configuration Control Board

\pagebreak

#  FSW Version Description

## Purpose and Summary

The purpose of this build is to continue to refine the cFE FSW product. This build provides various bug
fixes, as well as, new features and enhancements including:

+   Symmetric Multi-Processor (SMP) updates to ensure multi-thread saftey when writing to the system log
    and performing internal time updates and time services

+   New platform configuration to expand the Consultative Committee for Spacecraft Data Systems
    (CCSDS) APID name space beyond 2^11^. Using the CCSDS Version 2 Spacecraft Onboard Interface
    Services Area (SOIS) standard, this new configuration uses the extensions to the CCSDS Space
    Packet secondary header containing APID qualifier and subsystem ID information.  The CCSDS
    Primary Header APID continues to be used as a unique system-wide message identifier.
    Configurable macros are then utilized to generate a unique mission wide message identifier
    from the APID qualifier, command/telemetry bit, subsystem ID, and APID information with the
    CCSDS headers.  This new configuration addresses several issues including:

    - Difficulties allocating and managing the limited number of APIDs during development
    - Insufficient number of APIDs for large systems
    - APID allocations for formation flying, distributed systems, multi-core, and partitioned systems
    - Loss of spacecraft and subsystem identifying information as packets traverse the network stacks

+   ES Memory Pool updates to guarantee buffers allocated from a pool meet the target CPU architecture
    requirements for the largest C99 data types (`long long int`, `long double`, and `void*`). Additionally,
    this introduces a new platform configuration option to increase the alignment to greater value, as
    this can yield performance improvements on some architectures.

+   CCSDS Electronic Data Sheets (EDS) support.  An EDS is a formal machine readable (XML) specification
    of interfaces for both hardware and software components.  The primary use cases are to warrent:
    Exchange of unambiguous interface definitions in a standard format between organizations (As CCSDS
    is an international standards body, organizations includes other international agencies, and
    industry partners);
    Automatic generation of interface code, system models, system tests, and mission operational databases;
    and Run-time support for Plug-and-Play systems.
    EDS updates in this release include:

    - SOIS EDS XML files for each of the core applications (ES, EVS, SB, FS, TBL, and TIME)
    - Adoption and implementation of a consistent naming convention for all identifiers.  This serves as a
      prerequisite to using any sort of generated header files or source code from the EDS XML files.
    - Ensuring library loads are not duplicates.  The lack of a runtime ID becomes important/relevant if a
      loadable library contains an EDS that needs to be registered in the local EDS table for other apps to use.
    - Message dispatch consistency across the core applications.   Every interface message defined in the
      external interface (be it EDS or otherwise) now has a separate dedicated handler function. This
      includes no-op and send housekeeping commands.  The message handler functions all have a similar
      prototype:

      `int32 MessageHandler(const MessageType_t *data);`

+   Actual short format event message

+   Removal of compile time upper bound checks on platform configuration parameters

+   Adoption and implementation of a consistent naming convention for all mission and platform configuration
    parameters

+   New platform configuration to define the length of time (in milliseconds) ES will delay when sending shell
    command packets over the software bus

+   Moving Performance ID definitions from private to public header files

+   Adding missing command length verification for SB and TIME applications

+   Adding new Software Bus API to control whether the app publishing a message receives the message in its
    own pipe. 

There were no changes to existing APIs made in this build however, there were two changes that result in
expected compiler warnings building with the latest release of the OSAL, Version 4.2.1:

1. `cfe/fsw/cfe-core/src/es/cfe_es_shell.c: In function 'CFE_ES_ShellOutputCommand': line 98: warning: passing
argument 1 of 'OS_ShellOutputToFile' discards qualifiers from pointer target type.
osal/src/os/inc/osapi-os-filesys.h:439: note: expected 'char *' but argument is of type 'const char *'`

2. `cfe/fsw/cfe-core/src/tbl/cfe_tbl_task_cmds.c: In function 'CFE_TBL_DumpToFile': line 784: warning: passing
argument 2 of 'OS_write' discards qualifiers from pointer target type.
osal/src/os/inc/osapi-os-filesys.h:280: note: expected 'void *' but argument is of type 'const void *'`

These warnings will be resolved in a future release of the OSAL.

In addition, backward compatibility is not supported with versions of the OSAL library older than 4.2.0 and
versions of the PSP library older than 1.3.0.  Additional release notes may be found at:

<https://babelfish.arc.nasa.gov/trac/cfs_cfe/wiki/ReleaseNotes>

These release notes are also include in the `/docs` directory where this VDD is located.

Unit testing has been completed and baselined on cFE build 6.6.0 using
the updated cFE test suite. Results are included in the release package
in the "Results" directories included under:

`/fsw/cfe-core/unit-test/`

Additional unit testing has been performed on each of the platforms listed in Table 1.4-1.  
The test results have been archived on babelfish.arc.nasa.gov in the cfs_cfe Trac
Ticketing system under Trac #207.   

Regression testing was performed on cFE 6.6.0 on a PPC/mcp750 running
VxWorks 6.9. Results are included in the release package under:

`/test-and-ground/test-review-packages/Results`

Functional testing was performed on a variety of platforms. See section
[Tested Platforms] for a listing of the platforms on which functional 
testing was performed.

There are some outstanding issues being investigated. Resolutions to
these issues may require a new release. The project CCB and community
inputs will determine which Trac Tickets to include in the next release.

## New/Changed functionality in this version

Table 1.2-1 identifies new FSW functionality that has been implemented
and is integrated into this FSW version and the Trac tickets associated
with these changes.

### New Functionality in this Version

--------------------------------------------------------------------------------------------------------------
 Trac Component     Summary                                      Commits                                        
----- ------------- -------------------------------------------- -----------------------------------------------
 #167 es            Additional CFE start up state for            `465a958`: Add additional sync state for apps  
                    application sync                             `b227d57`: Remove stale comment in             
                                                                 CFE_ES_ExitApp                                 
                   
 #175 common        CCSDS APID Name Space Expansion              `b68417f`: Changes to implement extended APID  
                                                                 name space.  Also other assorted minor cleanup 
                                                                 `a6e3d70`: Minor cleanup action items          
                                                                 `d3d39e2`: Fixes for MsgKey/MsgId/RouteIdx     
                                                                 `3521508`: Cleanup APID qualifier header       
                   
 #176 common        CCSDS Electronic Data Sheet (EDS)            `8ae2f8d`: Naming convention updates           
                    integration                                  `fc652ab`: Add EDS files for CFE core apps     
                                                                 `d124688`: Make CFE_TIME_Copy macro public     
                                                                 `b54a2b7`: prefer sizeof(instance) rather than 
                                                                 type                                           
                                                                 `69edaa0`: Implement Generic status codes       
                                                                 `4737d6a`: Update ES message dispatch pattern  
                                                                 `3244d92`: Update EVS message dispatch pattern 
                                                                 `b91baff`: Update TBL message dispatch pattern 
                                                                 `478ffdb`: Update TIME message dispatch        
                                                                 pattern                                        
                                                                 `0f46da2`: Update SB message dispatch pattern  
                                                                 `27cc676`: Correlate TLM struct names between  
                                                                 code and EDS                                   
                                                                 `d547d5c`: Fix issue with EVS mode/format      
                                                                 command                                        
                   
 #190 sb            option to not receive messages I send?       `a47335c`: Added CFE\_SB\_{Set|Get}PipeOpts()
                                                                 API, with one field currently defined to       
                                                                 control whether the app publishing a message   
                                                                 receives the message in its own pipe.          
                                                                 Addresses .                                    
                   
 #203 build         CMake script cleanup                         `8ba8be4`: CMake script file cleanup           
                                                                 `6ea63f8`: Build PSP earlier in the build      
                                                                 process                                        
                   
 #204 other         CFE SB and TIME components missing length    `bd40a2d`: Add command length checks for SB    
                    verification on incoming messages            and TIME                                       
--------------------------------------------------------------------------------------------------------------

### Changes to Previously Delivered Functionality, Bug Fixes, and Documentation Updates

--------------------------------------------------------------------------------------------------------------
 Trac Component     Summary                                      Commits                                        
----- ------------- -------------------------------------------- -----------------------------------------------
  #12 other         External CFE message definitions should not  `81c64e0`: Separate PLATFORM, OS, and API      
                    depend on values from cfe_platform_cfg.h or  limits                                         
                    osconfig.h                                   `5099bc5`: Cleanup msgid inclusion in public   
                                                                 header                                         

  #30 other         Review use of CFE_PSP_MemCpy/CFE_PSP_MemSet  `57485cf`: Replace CFE_PSP_MemCpy/MemSet       
                   
  #39 other         Enforce Strict ASCII                         `8474136`: Scrub all non-ASCII characters in   
                                                                 source files                                   
                   
  #43 time          SMP: CFE TIME uses OSAL IntLock/IntUnlock    `ebbb081`: Fix TIME use of OS_IntLock for SMP  
                    for mutual exclusion                         `2a42f5f`: 2017-10-20 CCB - Volatile shared    
                                                                 memory values                                  
                   
  #44 es            SMP: CFE_ES_WriteToSyslog() is not           `05fdcb0`: Make CFE_ES_WriteToSysLog() thread  
                    multi-thread safe                            safe                                           
                                                                 `baa53e5`: Fixup from 2017-10-10 review        
                   
  #64 other         Suspicious implementation of SHORT_FORMAT    `d49d4ef`: Alternate MsgID Solution            
                    mode in EVS_SendPacket()                                                                    
                   

  #83 other         Default Configuration Setting for            `cee20c9`: Changed default from 30 to 1
                    CFE_ES_STARTUP_SCRIPT_TIMEOUT_MSEC is Too
                    Big

  #86 sb            Correction of an infinite loop in            `59640e0`: Replaced continue with if/else statement
                    cfe_sb_task.c

 #100 common        Update CFE_ES_SYSTEM_LOG_SIZE Verify to      `03ea234`: Removed unnecessary ES verify.h checks
                    Allow Larger Sys Log Files                   Updated the comments in the platform configuration
                                                                 file accordingly.
                                                                 `5a8f876`: Updated upper bound verification on RAM
                                                                 disk parameters, CDS size, user reserved memory size,
                                                                 and reset area size to use UINT_MAX macro. Updated
                                                                 platform configuration comments in accordance.
                                                                 `aab911a`: Corrected/removed upper bound checks on
                                                                 RAM_DISK_SECTOR_SIZE and RAM_DISK_NUM_SECTORS 
                                                                 configuration parameters. Updated the comments in
                                                                 the platform configuration file accordingly. Updated 
                                                                 sample_defs cpu1 platform_cfg.h to match comment 
                                                                 changes made to the /fsw/platform_inc/cpu1 copy.
                                                                 `e24fdfd`: Removed unneeded upper bound verifications 
                                                                 from EVS, SB, TBL, and TIME verify.h files.  Updated 
                                                                 configuration file documentation accordingly. Added 
                                                                 missing STACK_SIZE verifications. Added "disclaimer" 
                                                                 to top of configuration files.
                                                                 `fd32572`: Updated 
                                                                 CFE_PLATFORM_ES_START_TASK_STACK_SIZE default value to
                                                                 CFE_PLATFORM_ES_DEFAULT_STACK_SIZE in the 
                                                                 platform_cfg.h files. Updated ES and SB verify.h files 
                                                                 to use UINT32_MAX macro vs. the UINT_MAX macro.

 #107 sb            SB - Duplicate Pipe Creation Causes Failure  `2ef96fd`: In the initializer for
                    to Delete Pipe                               OriginalPipeIdParfamValue, the value assigned when the 
                                                                 PipeIdPtr is NULL needs to be of the same type as the 
                                                                 variable being initialized. We all missed the fact 
                                                                 that NULL (in C) explicitly includes a cast that 
                                                                 forces it to be a pointer, but the data type needed is 
                                                                 an integer.  This fix modifies the code: 
                                                                 if PipeIdPtr is NULL, then set the original PipeID to 
                                                                 zero, not NULL.  This value is not subsequently used: 
                                                                 all subsequent uses of this variable are beyond an 
                                                                 early exit taken if PipeIdPtr is NULL.

 #111 other         Naming convention for macros in              `35a2d07`: Proposed macro naming conventions
                    cfe_mission_cfg and cfe_platform_cfg



 #115 other         Standardize Version Numbering (in CFE)       `987cd80`: Added cFS version standards info from wiki 
                                                                 to both the main.dox and cfe_version.h files

 #117 es            CFE_ES_GetAppName() undefined output when    `42025d1`: CFE_ES_GetAppName() empty string on 
                    failure occurs                               error                                          
                                                                 `d0d0554`: Doxygen update associated with      
                                                                 proposed fix                                   
                   
 #133 es            CFE_ES_AppCreate does not unload an object 
                    file if the entry point is not found

 #135 sb            SB: "cfe_sb.h" should not depend on          `b6d65a3`: Remove SB implicit plafform_cfg     
                    cfe_platform_cfg.h                           include                                        
                   
 #137 es            Possible buffer overrun in format strings    `28244a6`: Avoid using scanf in startup script 
                    used for scanf                               `7e450fc`: CCB 2017-10-20: Warn about          
                                                                 permissive parsing                             
                   
 #140 es            EDS: The ES "LoadLibrary()" call - avoid     `b65114f`: Refactor CFE_ES_LoadLibrary
                    duplicates and pass ID

 #143 es            ES does not check target file existence      `624aae7`: Added check (OS_stat call) to ensure file
                    before attempting to reload an application   file exists before updating the variables to notify
                                                                 ES to restart application.
                                                                 `3f6d729`: Added doxygen documentation for the
                                                                 CFE_ES_ReloadApp function prototype comments
                                                                 describing recovery sequence in the event an 
                                                                 application cannot be restarted due to
                                                                 existing/corrupt file.

 #144 time          SMP: Thread safety issues in CFE_TIME        `ae8eedd`: Thread safety of timer sync         
                    around the Sync Callbacks                    callbacks                                      
                   
 #156 time          Incorrect leap seconds in docs               `51e721f`: Replaced hardcoded "current" leap seconds 
                                                                 with a link to the official source for determining 
                                                                 leap seconds.

 #164 es            cFE cES1702.3 and cES1703.3 Requirement      `2089d9b`: Updated the doxygen documentation for the
                    Failures                                     CFE_PLATFORM_ES_EXCEPTION_FUNTION parameter to provide 
                                                                 user guidance on where this callback function is 
                                                                 invoked.
                                                                 `887e11f`: Updated doxygen comments for 
                                                                 CFE_PLATFORM_ES_EXCEPTION_FUNCTION platform config 
                                                                 parameter with 10/10/17 CCB approved example.

 #165 sb            Misleading cFE Doxygen: CFE_SB_DeletePipe    `1190340`: CFE_SB_DeletePipe documentation was claiming 
                                                                 that applications were responsible for deleting pipes 
                                                                 in their normal shutdown process; in reality, this is 
                                                                 done by CFE_SB_CleanUpApp, which is called by 
                                                                 CFE_ES_CleanUpApp, as part of the framework support
                                                                 for application shutdown.

 #169 other         cFE Cmake Does Not Build Out-of-the-Box      `86ce044`: In order to enable the out-of-box experience 
                                                                 for CMake users of CFE 6.5 this imports the correct 
                                                                 sample_defs which coorespond to the remainder of the 
                                                                 release.

 #170 common        Doxygen generator code had gotten stale      `0075b72`: Fix cmake doxygen documentation builds.  The
                                                                 existing "mission-doc" target will build the detailed 
                                                                 documentation, which includes all functions. This 
                                                                 target had been broken since it referenced a 
                                                                 nonexistent Doxyfile and this fixes it. This also adds 
                                                                 another target called "cfe-usersguide" which generates 
                                                                 the CFE users guide package, which should match the 
                                                                 output of the classic build.  This is only CFE focused 
                                                                 and does not include the apps.  NOTE: this commit takes 
                                                                 a slightly different approach than the previous 
                                                                 attempts at this. This does not change any existing 
                                                                 files that were used by the classic build EXCEPT for 
                                                                 correcting some non-UTF8 characters. Therefore this 
                                                                 will not break the classic build.

 #177 common        Remove all MKS $log comments in file header  `b748523`: Removed MKS $log information from comment
                    prologs                                      blocks

 #180 other         'printf': Mismatch between the type          `5567bb4`: Added missing type casts to all
                    expected by the conversion specifier %x and  EVS_SendEvent calls  
                    the type of the argument.

 #183 es            ES Shell Command Telemetry Timing is         `ae5473b`: Added new CFE_PLATFORM_ES_SHELL_OS_DELAY
                    Hardcoded                                    platform configuation parameter.   Replaced hardcoded 
                                                                 task delay in CFE_ES_ShellOutputCommand with new
                                                                 configuration parameter.
                                                                 `c00fde0`: Updated name of new platform configuration 
                                                                 parameter from CFE_PLATFORM_ES_SHELL_OS_DELAY to 
                                                                 CFE_PLATFORM_ES_SHELL_OS_DELAY_MILLISEC.  Minor fix to
                                                                 comment for this configuation parameter.

 #184 other         cFE Performance IDs are Private Definitions  `142a9ef`: Moved performance id definitions to
                                                                 platform configuration, changing the name of the file
                                                                 to be consistent with cFS.
                                                                 `21e5683`: Moved performance IDs to mission include.
                                                                 Renamed the performance ID macros to follow mission
                                                                 configuration naming standard. Updated macros
                                                                 accordingly in source code files.

 #193 sb            CFE_SB_CreatePipe should avoid nesting       `54a1ad`: Removed nested locks
                    locks

 #194 other         Add "Maximum EID" Comment to the Top of      `37342e3`: Added max EID comments to ES, EVS, TBL, and
                    ALL cFE events.h Files                       TIME events.h files as was done for SB under ticket
                                                                 #190.

 #197 es            ES - Incorrect Use of                        `06a9a6a`: Removed all calls to
                    CFE_SB_MessageStringGet Function in          CFE_SB_MessageStringGet from the
                    CFE_ES_ShellOutputCommand                    CFE_ES_ShellOutputCommand function. Removed unneeded
                                                                 local buffers.  Corrected usage of strncmp to strcmp.
                                                                 `99e3d2a`: Corrected undefined behavior when checking 
                                                                 Filname for NULL and assigning default.
                                                                 `1a9d3a1`: removed type cast in call to 
                                                                 OS_ShellOutputToFile in CFE_ES_ShellOutputCommand 
                                                                 function.

 #198 sb            Build failure when using std=c99             `7b27925`: Fix build failure with strict flags 
                   
 #199 es            CFE_ES mempool returns buffers that are not  `fd4d9da`: Patch for mempool alignment issues  
                    aligned                                      `5c53b99`: Updates per CCB discussion          
                   
 #202 other         Clean up build warnings for CFE 6.6          `5344db9`: Fix warnings in the current build   
                                                                 `18c3d0a`: in CFE_ES_SetupResetVariables       
                                                                 function, removed "status =" from all calls to 
                                                                 CFE_ES_WriteToERLog                            
                                                                 `c099fc6`: in CFE_ES_GetAppID function,        
                                                                 removed "Result" variable initialization       
                                                                 `769e198`: in CFE_ES_AppCreate function, added 
                                                                 return code check after call to                
                                                                 OS_remove(RamDiskPath), if error then          
                                                                 WriteToSysLog                                  
                                                                 `8157f47`: cfe_es_apps.c:890 - in              
                                                                 CFE_ES_LoadLibrary function, added return code 
                                                                 check after call to OS_remove(RamDiskPath), if 
                                                                 error then WriteToSysLog                       
                                                                 `08add53`: In CFE_ES_ExitApp function, add     
                                                                 explicit ReturnCode check and return after     
                                                                 call to CFE_ES_ResetCFE                        
                                                                 `252ed2a`: CFE_ES_ProcessCoreException         
                                                                 function, removed "Status =" from call to  
                                                                 CFE_ES_RestartApp. In CFE_ES_RestartApp        
                                                                 function, else case where "App ID is not     
                                                                 valid" added WriteToSysLog                   
                   
 #213 other         Update cFE Version Number for 6.6.0 Release  `657b40c`: Updated version number to 6.6.0 for 6.6.0 
                                                                 testing and release

 #215 tbl           Table services sometimes copies buffers to   `8a7faa5`: Avoid memcpy overlap in TBL services
                    itself                                                                                      
                   
 #216 other         Table Services Task Pipe Function            `25d7de3`: Fix TBL service message truncation  
                    Incorrectly Handling Commands                                                               
                   
 #217 other         Fix EDS discrepancies after #175 merge       `f10a142`: XML and corresponding header changes                                 
                                                                 `6e3ecbd`: Move "CFE_SB_RoutingTblIdx_t"       
                                                                 `cc5c8fc`: Fix bug route file output           
                                                                 `c6008ca`: Improve forward compatibility

 #219 es            ES - Memory Pool Size No Longer Requires     `bfa1e46`: Updated requirements to remove memory pool
                    32-bit Alignment                             32-bit alighment.  Requirements updated to be in sync
                                                                 with the changes associated with Trac #199
--------------------------------------------------------------------------------------------------------------


## Missing features and known problems

Information on currently open Trac tickets is available at
<https://babelfish.arc.nasa.gov/trac/cfs_cfe/report/1>. Note that this
is a restricted website that requires a server account.  

The regression test report located in the 
/test-and-ground/test-review-packages/Results directory includes a listing
of the functions and known discrepancies that were absent from cFE Build 
6.6.0 at the time of testing. Additional Trac Tickets may have been submitted 
after preparation of this test report.

A Trac Ticket report containing a listing of open Trac Tickets is available on 
request for customers who do not have access to the above servers. Please contact 
David McComas, <david.c.mccomas@nasa.gov>. 


## Development Tool Versions Associated with this FSW Version

Table 1.4-1 identifies the versions of development tools known to
work with this FSW version:

**Table 1.4-1 - Development Tool Versions Associated with this FSW Version**

Tool Type       Tool Name                            Version Used
--------------- ------------------------------------ ----------------
RTOS            VxWorks                              6.9
Compiler        VxWorks ccppc                        3.3.2
RTOS            RTEMS                                4.11.2
Compiler        i386-rtems4.11-gcc                   4.9.3
RTOS            Yocto "Poky" Linux Distribution      2.4                                   
Kernel          RTLinux                              4.12.12-yocto-rt
Compiler        arm-poky-linux-gcc                   7.2.0
Dev Host        Ubuntu                               16.04.3 LTS   
Ground System   ASIST                                20.2
--------------- ------------------------------------ ----------------


## Tested Platforms

Since the cFE uses the Operating System Abstraction Layer (OSAL),
multiple operating systems are supported. Build Testing of cFE Build
6.6.0 has been done on a PPC/mcp750 using the RTOS and compiler
specified in Table 1.4-1 above. In addition, cFE Build 6.6.0 has been
functionally tested with the hardware/software platforms listed in
Table 1.5-1 below (build test platform is included for completeness).

**Table 1.5-1 - Functional Test Platforms Associated with this FSW Version**

-------------------------------------------------------------------------------------------------------------
Machine         CPU         Operating System    PSP          Notes  
--------------- ----------- ------------------- ------------ ------------------------------------------------
PC              x86-64      Ubuntu 16.04 LTS,   pc-linux     Verify operation of native 64-bit build on
                            Kernel 4.13.0-16                 8-core SMP, little endian machine.
                            

Virtual PC      i686        RTEMS 4.11.2        pc-rtems     Verify operation on 32-bit RTEMS.
(QEMU 2.5.0)                with "pc686" BSP                 Updated PSP needed for compatibility with
                                                             RTEMS 4.11.2 release (no change to CFE itself).

Beaglebone      ARMv7       Yocto Project Poky  pc-linux     Verify operation on 32-bit little endian ARM 
Black                       Distribution 2.4,                processor on Linux with Realtime (RT) kernel,
                            RTLinux 4.12.12                  running on actual hardware (TI AM3359 CPU).
                            
Virtual G4      PowerPC     Yocto Project Poky  pc-linux     Verify operation on 32-bit big endian processor
(QEMU 2.10.0)   7400        Distribution 2.4,                
                            Linux 4.12.12
  
Virtual Malta   MIPS 64     Yocto Project Poky  pc-linux     Verify operation on 64-bit big endian processor
(QEMU 2.10.0)   (64-bit)    Distribution 2.4,                (native 64 bit build)
                            Linux 4.12.12

Maxwell SCS750  PowerPC     VxWorks 6.9         sp0-vxworks  Verify operation on 32-bit VxWorks platform     
                750FX                                         
-------------------------------------------------------------------------------------------------------------


# Delivered products

Table 2.0-1 identifies the locations of FSW products relevant to this
FSW Build. The version or date of the Build and where the product can be
located are provided. Changes from a previous VDD are identified.

**Table 2.0-1 - Delivered Products and their Locations**

  -------------------------------------------------------------------------------------------- ---------------------------- --------------------- ----------------------------------------------------------------------------------------------------------------------------
  Software Element                                                                             Changed with this Version?   New Version or Date   Location

  Executable for this build                                                                    Yes                          6.6.0                 N/A. Executables are not delivered for the cFE

  Installation Procedures & Special Instructions                                               No                           N/A                   See Deployment Guide
                                                                                                                                                  
                                                                                                                                                  babelfish.arc.nasa.gov (in git system master branchs - cFE and TOOLS) and <http://sourceforge.net/projects/coreflightexec>

  Source Code of this FSW Build                                                                Yes                          6.6.0                 babelfish.arc.nasa.gov (in git system master branch) and <http://sourceforge.net/projects/coreflightexec>

  FSW Build Plan                                                                               No                           N/A                   None

  Annotated S/W Detailed Design Docs                                                           No                           N/A                   cFE Application Developer's Guide
                                                                                                                                                  
                                                                                                                                                  babelfish.arc.nasa.gov (in git system master branch) and <http://sourceforge.net/projects/coreflightexec>

  Ground System T&C Database                                                                   Yes                          6.6.0                 babelfish.arc.nasa.gov (in git system master branch) and <http://sourceforge.net/projects/coreflightexec>

  Ground System Scripts developed by FSB                                                       No                           N/A                   babelfish.arc.nasa.gov (in git system master branch) and <http://sourceforge.net/projects/coreflightexec>

  Simulator and Test Data Generator Software                                                   N/A                          N/A                   None

  Executable - Ground Tools associated with FSW (tools to build stored command loads, etc.)    No                           N/A                   babelfish.arc.nasa.gov (in git system TOOLS master branch) and <http://sourceforge.net/projects/coreflightexec>

  Source Code - Ground Tools associated with FSW (tools to build stored command loads, etc.)   Yes                          N/A                   \$WORK
                                                                                                                                                  
                                                                                                                                                  Perl scripts to generate ground database and build verification procedures from templates

  Unit Test Procedures                                                                         Yes                          6.6.0                 babelfish.arc.nasa.gov (in git system master branch) and <http://sourceforge.net/projects/coreflightexec>

  Unit Test Data                                                                               Yes                          6.6.0                 babelfish.arc.nasa.gov (in git system master branch) and <http://sourceforge.net/projects/coreflightexec>

  Unit Test Results                                                                            Yes                          6.6.0                 babelfish.arc.nasa.gov (in git system master branch) and <http://sourceforge.net/projects/coreflightexec>

  FSW Make Files                                                                               Yes                          6.6.0                 babelfish.arc.nasa.gov (in git system master TOOLS branch) and <http://sourceforge.net/projects/coreflightexec>

  Linker & Compiler Configuration Files                                                        Yes                          6.6.0                 babelfish.arc.nasa.gov (in git system master TOOLS branch) and <http://sourceforge.net/projects/coreflightexec>
  -------------------------------------------------------------------------------------------- ---------------------------- --------------------- ----------------------------------------------------------------------------------------------------------------------------

# Installation Procedures

Table 3.0-1 identifies the nominal FSW Installation Procedure(s) for
this FSW Build onto the intended target system (including the commercial
applications used and the configuration settings). The procedure version
identifier, the date of the procedure and where it can be located are
also provided.

**Table 3.0-1 FSW Installation Procedure(s)**

  ----------------------------------- ---------------------- ------------------ -----------------------------------------------------------------------------------------------------------------
  Destination                         Filename               Version and Date   Location
                                                                                
  (Target System)                                                               

  Procedure is generic for each CPU   cFS Deployment Guide   3.0                babelfish.arc.nasa.gov (in git system master TOOLS branch) and <http://sourceforge.net/projects/coreflightexec>
  ----------------------------------- ---------------------- ------------------ -----------------------------------------------------------------------------------------------------------------

# Configuration summary and version identification

cFE Build 6.6.0 can be found on babelfish.arc.nasa.gov (in git system
master branch) and is provided as open source on sourceforge.net:

<http://sourceforge.net/projects/coreflightexec/>

Telemetry is available to indicate cFE Build 6.6.0 and is documented in
the following source file: /fsw/cfe/core/src/inc/cfe\_version.h.

# Appendix

## Acronyms

API Application Program Interface

cFE Core Flight Executive

C&DH Command and Data Handling

cFS Core Flight Software System

CM Configuration Management

CPM CFS Performance Monitor

COTS Commercial Off-The-Shelf

DCR Discrepancy/Change Request

EDS Electronic Data Sheet

ES Executive Services

ETU Engineering Test Unit

EVS Event Services

FSB Flight Software Branch

FSW Flight Software

I&T Integration & Test

MISRA Integration & Test

OSAL Operating System Abstraction Layer

PPC Power Personal Computer

RTOS Real-Time Operating System

SB Software Bus Services

TBL Table Services

TIME Time Services

T&C Telemetry and Command

URL Universal Resource Locator

UTF Unit Test Framework

VDD Version Description Document
