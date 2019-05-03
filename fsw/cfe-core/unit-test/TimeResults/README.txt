The cFE Time Service (TIME) unit tests were run in the following pc-linux environment:

Distributor ID: Red Hat Enterprise Linux Workstation release 6.9 (Santiago)
Release:        6.9

Linux gs582w-cfelnx 2.6.32-642.el6.i686 #1 SMP Wed Apr 13 00:50:26 EDT 2016 i686 i686 i386 GNU/Linux

Unit tests built with gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-18)

The expected TIME unit test pass/fail test results are located in the ut_cfe_time_log.txt file

The TIME unit test results are expected to have the following
coverage results for each of the TIME source files:

ut_cfe_time PASSED 188 tests.
ut_cfe_time FAILED 0 tests.

gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/time/cfe_time_api.c' 97.94%  194
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/time/cfe_time_task.c' 99.41%  337
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/time/cfe_time_tone.c' 100.00%  157
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/time/cfe_time_utils.c' 100.00%  281
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/time/cfe_time_utils.h' 100.00%  7

==========================================================================

cfe_time_api.c - 97.99% coverage (4 lines not executed)

In CFE_TIME_CFE2FSSeconds:

        3:  651:uint32 CFE_TIME_CFE2FSSeconds(uint32 SecondsCFE)
        -:  652:{
        -:  653:    /*
        -:  654:    ** Using a signed integer allows the factor to be negative...
        -:  655:    */
        3:  656:    int32 ConvertFactor = CFE_MISSION_TIME_FS_FACTOR;
        -:  657:
        -:  658:    /*
        -:  659:    ** File system time = cFE time + conversion factor...
        -:  660:    */
        3:  661:    uint32 SecondsFS = SecondsCFE + (uint32) ConvertFactor;
        -:  662:
        -:  663:    /*
        -:  664:    ** Prevent file system time from going below zero...
        -:  665:    */
        3:  666:    if (ConvertFactor < 0)
        -:  667:    {
    #####:  668:        if (-ConvertFactor > SecondsCFE)
        -:  669:        {
    #####:  670:            SecondsFS = 0;
        -:  671:        }
        -:  672:    }
        -:  673:
        3:  674:    return(SecondsFS);
        -:  675:
        -:  676:} /* End of CFE_TIME_CFE2FSSeconds() */

REASON: These lines are contingent on a platform configuration setting and it is only set one way for the unit test.

In CFE_TIME_Print:

        -:  740:    /*
        -:  741:    ** Convert the cFE time (offset from epoch) into calendar time...
        -:  742:    */
       10:  743:    NumberOfMinutes = (TimeToPrint.Seconds / 60) + CFE_MISSION_TIME_EPOCH_MINUTE;
       10:  744:    NumberOfSeconds = (TimeToPrint.Seconds % 60) + CFE_MISSION_TIME_EPOCH_SECOND;
        -:  745:
        -:  746:    /*
        -:  747:    ** Adding the epoch "seconds" after computing the minutes avoids
        -:  748:    **    overflow problems when the input time value (seconds) is
        -:  749:    **    at, or near, 0xFFFFFFFF...
        -:  750:    */
       20:  751:    while (NumberOfSeconds >= 60)
        -:  752:    {
    #####:  753:        NumberOfMinutes++;
    #####:  754:        NumberOfSeconds -= 60;
        -:  755:    }

REASON: These lines are contingent on a platform configuration setting and it is only set one way for the unit test.

==========================================================================

cfe_time_task.c - 99.41 % coverage

        -:  332:    /*
        -:  333:    ** Subscribe to time at the tone "request data" commands...
        -:  334:    */
        -:  335:    #if (CFE_PLATFORM_TIME_CFG_SERVER == TRUE)
        7:  336:    Status = CFE_SB_Subscribe(CFE_TIME_SEND_CMD_MID,
        7:  337:                                  CFE_TIME_TaskData.CmdPipe);
        7:  338:    if(Status != CFE_SUCCESS)
        -:  339:    {
    #####:  340:      CFE_ES_WriteToSysLog("TIME:Error subscribing to time at the tone request data cmds:RC=0x%08X\n",(unsigned int)Status);
    #####:  341:      return Status;
        -:  342:    }/* end if */
        -:  343:    #endif

REASON: These lines are contingent on a platform configuration setting and it is only set one way for the unit test.

==========================================================================

cfe_time_tone.c - 100.00% coverage

==========================================================================

cfe_time_utils.c - 100.00% coverage

==========================================================================
