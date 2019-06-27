The cFE Executive Service (ES) unit tests were run in the following pc-linux environment:

Distributor ID: Red Hat Enterprise Linux Workstation release 6.9 (Santiago)
Release:        6.9

Linux gs582w-cfelnx 2.6.32-642.el6.i686 #1 SMP Wed Apr 13 00:50:26 EDT 2016 i686 i686 i386 GNU/Linux

Unit tests built with gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-18)

The expected ES unit test pass/fail test results are located in the ut_cfe_es_log.txt file

The ES unit test results are expected to have the following
results for each of the ES services source files:

ut_cfe_es PASSED 491 tests.
ut_cfe_es FAILED 0 tests.

gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_api.c' 97.76%  491
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_apps.c' 99.57%  470
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_cds.c' 100.00%  204
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_cds_mempool.c' 100.00%  229
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_esmempool.c' 100.00%  210
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_erlog.c' 100.00%  29
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_perf.c' 100.00%  146
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_shell.c' 100.00%  131
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_start.c' 100.00%  231
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_syslog.c' 99.28%  138
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/es/cfe_es_task.c' 99.65%  567
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/inc/cfe_es.h' 100.00%  3

==========================================================================

cfe_es_api.c - 98.99% coverage (5 Lines of Code NOT executed)

In CFE_ES_ReloadApp:
        -:  289:       /*
        -:  290:       ** Check to see if the file exists
        -:  291:       */
        1:  292:       if (OS_stat(AppFileName, &FileStatus) == OS_SUCCESS)
        -:  293:       {
        1:  294:           CFE_ES_SysLogWrite_Unsync("CFE_ES_ReloadApp: Reload Application %s Initiated. New filename = %s\n",
        -:  295:                                CFE_ES_Global.AppTable[AppID].StartParams.Name, AppFileName);
        1:  296:           strncpy((char *)CFE_ES_Global.AppTable[AppID].StartParams.FileName, AppFileName, OS_MAX_PATH_LEN);
        1:  297:           CFE_ES_Global.AppTable[AppID].StateRecord.AppControlRequest = CFE_ES_RunStatus_SYS_RELOAD;
        1:  298:           CFE_ES_SetAppState(AppID, CFE_ES_AppState_WAITING);
        1:  299:           CFE_ES_Global.AppTable[AppID].StateRecord.AppTimer = CFE_PLATFORM_ES_APP_KILL_TIMEOUT;
        -:  300:       }
        -:  301:       else
        -:  302:       {
    #####:  303:           CFE_ES_SysLogWrite_Unsync ("CFE_ES_ReloadApp: Cannot Reload Application %s, File %s does not exist.\n",
        -:  304:                                  CFE_ES_Global.AppTable[AppID].StartParams.Name,
        -:  305:                                  AppFileName);
    #####:  306:           ReturnCode = CFE_ES_FILE_IO_ERR;
        -:  307:       }

REASON: No existing set return code capability for OS_stat in ut assert library


In CFE_ES_ExitApp:

        -:  436:       else /* It is an external App */
        -:  437:       {
        -:  438:
    #####:  439:          CFE_ES_SysLogWrite_Unsync ("Application %s called CFE_ES_ExitApp\n",
        -:  440:                                 CFE_ES_Global.AppTable[AppID].StartParams.Name);
    #####:  441:          CFE_ES_SetAppState(AppID, CFE_ES_AppState_STOPPED);
        -:  442:
        -:  443:          /*
        -:  444:          ** Unlock the ES Shared data before suspending the app
        -:  445:          */
    #####:  446:          CFE_ES_UnlockSharedData(__func__,__LINE__);
        -:  447:
        -:  448:          /*
        -:  449:          ** Suspend the Application until ES kills it.
        -:  450:          ** It might be better to have a way of suspending the app in the OS
        -:  451:          */
        -:  452:          while(1)
        -:  453:          {
    #####:  454:             OS_TaskDelay(500);
    #####:  455:          }
        -:  456:
        -:  457:       } /* end if */

REASON: Unit testing this branch is not possible because of the need for the
        infinite loop.

In CFE_ES_DeleteChildTask:

        -: 1153:    /*
        -: 1154:    ** Make sure the task ID is within range
        -: 1155:    */
        5: 1156:    if (OS_ConvertToArrayIndex(OSTaskId, &TaskId) == OS_SUCCESS)
        -: 1157:    {
                    ...
        -: 1233:    }
        -: 1234:    else
        -: 1235:    {
        -: 1236:       /*
        -: 1237:       ** Task ID is invalid ( too large )
        -: 1238:       */
    #####: 1239:       CFE_ES_WriteToSysLog("CFE_ES_DeleteChildTask Error: Invalid Task ID: %u\n",(unsigned int)OSTaskId );
    #####: 1240:       ReturnCode = CFE_ES_ERR_TASKID;
        -: 1241:
        -: 1242:    }

REASON: OS_ConvertToArrayIndex is an inline function defined in cfe_es.h and always returns
        success.

In CFE_ES_GetAppIDInternal:

       53: 1698:int32 CFE_ES_GetAppIDInternal(uint32 *AppIdPtr)
        -: 1699:{
       53: 1700:   int32  Result = CFE_ES_ERR_APPID;
        -: 1701:   uint32 TaskId;
        -: 1702:
        -: 1703:   /*
        -: 1704:   ** Step 1: Get the OS task ID
        -: 1705:   */
       53: 1706:   if (OS_ConvertToArrayIndex(OS_TaskGetId(), &TaskId) == OS_SUCCESS)
        -: 1707:   {
        -: 1708:      /*
        -: 1709:      ** Step 2: get the Application ID for the current task
        -: 1710:      */
       53: 1711:      if ( CFE_ES_Global.TaskTable[TaskId].RecordUsed == TRUE )
        -: 1712:      {
       37: 1713:         *AppIdPtr = CFE_ES_Global.TaskTable[TaskId].AppId;
       37: 1714:         Result = CFE_SUCCESS;
        -: 1715:      }
        -: 1716:      else
        -: 1717:      {
       16: 1718:         *AppIdPtr = 0;
        -: 1719:      } /* end if */
        -: 1720:   }
        -: 1721:   else
        -: 1722:   {
    #####: 1723:      *AppIdPtr = 0;
        -: 1724:   } /* end if */
        -: 1725:
       53: 1726:   return(Result);
        -: 1727:
        -: 1728:} /* End of CFE_ES_GetAppIDInternal() */

REASON: OS_ConvertToArrayIndex is an inline function defined in cfe_es.h and always returns
        success.

==========================================================================
cfe_es_apps.c - 99.57% coverage

In CFE_ES_AppCreate:

        -:  675:         /*
        -:  676:         ** Remove the temporary RAM disk file
        -:  677:         */
       20:  678:         if ( IsRamDiskFile == TRUE )
        -:  679:         {
        1:  680:            ReturnCode = OS_remove(RamDiskPath);
        -:  681:
        1:  682:            if (ReturnCode != OS_SUCCESS)
        -:  683:            {
    #####:  684:                CFE_ES_WriteToSysLog("ES Startup: Error removing temp RAM disk file, EC = 0x%08X\n",
        -:  685:                                     (unsigned int) ReturnCode);
        -:  686:            }
        -:  687:         }

REASON: No existing set return code capability for OS_remove in ut assert library

In CFE_ES_LoadLibrary:

        -:  954:       /*
        -:  955:        * If the above code had used a temp file, then remove it
        -:  956:        */
        5:  957:       if ( IsRamDiskFile == TRUE )
        -:  958:       {
        1:  959:           Status = OS_remove(RamDiskPath);
        -:  960:
        1:  961:           if (Status != OS_SUCCESS)
        -:  962:           {
    #####:  963:                CFE_ES_WriteToSysLog("ES Startup: Error removing temp lib RAM disk file, EC = 0x%08X\n",
        -:  964:                                     (unsigned int) Status);
        -:  965:           }
        -:  966:       }

REASON: No existing set return code capability for OS_remove in ut assert library

==========================================================================
cfe_es_cds.c - 100% coverage

==========================================================================
cfe_es_cds_mempool.c - 100% coverage

==========================================================================
cfe_es_erlog.c - 100% coverage

==========================================================================
cfe_esmempool.c - 100% coverage

==========================================================================
cfe_es_perf.c - 100% coverage

==========================================================================
cfe_es_shell.c - 100% coverage

==========================================================================
cfe_es_start.c - 100% coverage

==========================================================================
cfe_es_syslog.c - 99.28% coverage

In CFE_ES_SysLogReadData:

        6:  317:        if (BlockSize == 0)
        -:  318:        {
        -:  319:            /* should be impossible for this to happen,
        -:  320:             * just in case, do not spin endlessly */
    #####:  321:            break;
        -:  322:        }

REASON: Block size should never be zero inside this function

==========================================================================

cfe_es_task.c - 99.65% coverage

In CFE_ES_HousekeepingCmd:

        -:  647:    /*
        -:  648:     * Fill out the perf trigger/filter mask objects
        -:  649:     * The entire array in the HK payload object (external size) must be filled,
        -:  650:     * to avoid sending garbage data.
        -:  651:     *
        -:  652:     * If it is larger than what the platform supports (internal size), it will
        -:  653:     * be padded with 0's
        -:  654:     *
        -:  655:     * If it is smaller than what the platform supports, then truncate.
        -:  656:     */
       10:  657:    for (PerfIdx = 0; PerfIdx < CFE_ES_PERF_TRIGGERMASK_EXT_SIZE; ++PerfIdx)
        -:  658:    {
        8:  659:        if (PerfIdx < CFE_ES_PERF_TRIGGERMASK_INT_SIZE)
        -:  660:        {
       16:  661:            CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] =
        8:  662:                    CFE_ES_ResetDataPtr->Perf.MetaData.TriggerMask[PerfIdx];
        -:  663:        }
        -:  664:        else
        -:  665:        {
    #####:  666:            CFE_ES_TaskData.HkPacket.Payload.PerfTriggerMask[PerfIdx] = 0;
        -:  667:        }
        -:  668:    }
        -:  669:
       10:  670:    for (PerfIdx = 0; PerfIdx < CFE_ES_PERF_FILTERMASK_EXT_SIZE; ++PerfIdx)
        -:  671:    {
        8:  672:        if (PerfIdx < CFE_ES_PERF_FILTERMASK_INT_SIZE)
        -:  673:        {
       16:  674:            CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] =
        8:  675:                    CFE_ES_ResetDataPtr->Perf.MetaData.FilterMask[PerfIdx];
        -:  676:        }
        -:  677:        else
        -:  678:        {
    #####:  679:            CFE_ES_TaskData.HkPacket.Payload.PerfFilterMask[PerfIdx] = 0;
        -:  680:        }
        -:  681:    }

REASON: Cannot be executed without altering ES config parameters.

==========================================================================
