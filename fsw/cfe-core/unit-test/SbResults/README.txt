The cFE Software Bus Service (SB) unit tests were run in the following pc-linux environment:

Distributor ID: Red Hat Enterprise Linux Workstation release 6.9 (Santiago)
Release:        6.9

Linux gs582w-cfelnx 2.6.32-642.el6.i686 #1 SMP Wed Apr 13 00:50:26 EDT 2016 i686 i686 i386 GNU/Linux

Unit tests built with gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-18)

The expected SB unit test pass/fail test results are located in the ut_cfe_sb_log.txt file

The SB unit test results are expected to have the following
coverage results for each of the SB source files:

ut_cfe_sb PASSED 178 tests.
ut_cfe_sb FAILED 0 tests.

gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/ccsds.c' 100.00%  13
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_api.c' 99.67%  611
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_buf.c' 100.00%  51
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_init.c' 100.00%  47
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_msg_id_util.c' 100.00%  9
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_priv.h' 100.00%  12
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_priv.c' 100.00%  158
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_task.c' 99.49%  394
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/sb/cfe_sb_util.c' 100.00%  119
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/inc/cfe_es.h' 100.00%  3

==========================================================================

ccsds.c - 100.00% coverage

==========================================================================

cfe_sb_api.c - 99.67% coverage - (1 Line of Code NOT executed)

In CFE_SB_SetPipeOpts

        5:  410:    Status = CFE_ES_GetAppID(&AppID);
        5:  411:    if(Status != CFE_SUCCESS)
        -:  412:    {
        -:  413:        /* shouldn't happen... */
    #####:  414:        return Status;
        -:  415:    }

REASON: See comment provided on line 413

In CFE_SB_SendMsgFull:

        -: 1364:        /* The DestPtr should never be NULL in this loop, this is just extra
        -: 1365:           protection in case of the unforseen */
       23: 1366:        if(DestPtr == NULL)
        -: 1367:        {
    #####: 1368:          break;
        -: 1369:        }      
        
REASON: See comment provided on line 1364

==========================================================================

cfe_sb_buf.c - 100% coverage

==========================================================================

cfe_sb_init.c - 100% coverage

==========================================================================

cfe_sb_msg_id_util.c - 100% coverage

==========================================================================

cfe_sb_priv.c - 100% coverage

==========================================================================

cfe_sb_task.c - 99.49% coverage 

The first line of code that is not executed in Unit Testing is in the
CFE_SB_TaskMain function.  The code snippet is as follows:

        -:   85:        /* Pend on receipt of packet */
        1:   86:        Status = CFE_SB_RcvMsg(&CFE_SB.CmdPipePktPtr,
        1:   87:                                CFE_SB.CmdPipe,
        -:   88:                                CFE_SB_PEND_FOREVER);
        -:   89:
        1:   90:        CFE_ES_PerfLogEntry(CFE_MISSION_SB_MAIN_PERF_ID);
        -:   91:
        1:   92:        if(Status == CFE_SUCCESS)
        -:   93:        {
        -:   94:            /* Process cmd pipe msg */
    #####:   95:            CFE_SB_ProcessCmdPipePkt();
        -:   96:        }else{
        1:   97:            CFE_ES_WriteToSysLog("SB:Error reading cmd pipe,RC=0x%08X\n",(unsigned int)Status);
        -:   98:        }/* end if */

REASON: Have not found a way to execute this line while allowing the unit test
        terminate normally. 

In CFE_SB_AppInit:

        -:  189:    /* Be sure the number of events to register for filtering
        -:  190:    ** does not exceed CFE_PLATFORM_EVS_MAX_EVENT_FILTERS */
       11:  191:    if(CFE_PLATFORM_EVS_MAX_EVENT_FILTERS < CfgFileEventsToFilter){
    #####:  192:      CfgFileEventsToFilter = CFE_PLATFORM_EVS_MAX_EVENT_FILTERS;
        -:  193:    }/* end if */
        
REASON: Cannot be executed without altering EVS or SB config parameters.

==========================================================================

cfe_sb_util.c - 100% coverage

==========================================================================
