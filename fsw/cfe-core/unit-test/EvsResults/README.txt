The cFE Event Service (EVS) unit tests were run in the following pc-linux environment:

Distributor ID: Red Hat Enterprise Linux Workstation release 6.9 (Santiago)
Release:        6.9

Linux gs582w-cfelnx 2.6.32-642.el6.i686 #1 SMP Wed Apr 13 00:50:26 EDT 2016 i686 i686 i386 GNU/Linux

Unit tests built with gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-18)

The expected EVS unit test pass/fail test results are located in the ut_cfe_evs_log.txt file

The EVS unit test results are expected to have the following
coverage results for each of the EVS source files:

ut_cfe_evs PASSED 198 tests.
ut_cfe_evs FAILED 0 tests.

gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/evs/cfe_evs.c' 100.00%  98
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/evs/cfe_evs_log.c' 100.00%  75
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/evs/cfe_evs_task.c' 99.60%  499
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/evs/cfe_evs_utils.c' 100.00%  129

==========================================================================

cfe_evs.c - 100.00 % coverage

==========================================================================

cfe_evs_log.c - 100.00% coverage

==========================================================================
cfe_evs_task.c - 99.61% % coverage (2 Lines of Code NOT executed)

The lines not covered are in CFE_EVS_TaskInit:

        -:  385:   /* Register EVS task for event services */
        5:  386:   Status = CFE_EVS_Register(NULL, 0, CFE_EVS_BINARY_FILTER);
        5:  387:   if (Status != CFE_SUCCESS)
        -:  388:   {
    #####:  389:      CFE_ES_WriteToSysLog("EVS:Call to CFE_EVS_Register Failed:RC=0x%08X\n",(unsigned int)Status);
    #####:  390:      return Status;
        -:  391:   }

REASON: None - A DCR was entered to get full coverage of this file.

==========================================================================
cfe_evs_utils.c - 100.00% coverage

==========================================================================
