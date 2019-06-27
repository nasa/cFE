The cFE Table Service (TBL) unit tests were run in the following pc-linux environment:

Distributor ID: Red Hat Enterprise Linux Workstation release 6.9 (Santiago)
Release:        6.9

Linux gs582w-cfelnx 2.6.32-642.el6.i686 #1 SMP Wed Apr 13 00:50:26 EDT 2016 i686 i686 i386 GNU/Linux

Unit tests built with gcc (GCC) 4.4.7 20120313 (Red Hat 4.4.7-18)

The expected TBL unit test pass/fail test results are located in the ut_cfe_tbl_log.txt file

The TBL unit test results are expected to have the following
coverage results for each of the TBL source files:

ut_cfe_tbl PASSED 282 tests.
ut_cfe_tbl FAILED 0 tests.

gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/tbl/cfe_tbl_api.c' 99.22%  513
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/tbl/cfe_tbl_internal.c' 100.00%  458
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/tbl/cfe_tbl_task.c' 100.00%  97
gcov: '/home/mdeschu/cfe-660-ut/cfe/fsw/cfe-core/src/tbl/cfe_tbl_task_cmds.c' 99.77%  431

==========================================================================

cfe_tbl_api.c - 99.22% coverage (4 Lines of Code NOT executed)

In CFE_TBL_Register:

        -:  348:                        /* Register a CDS under the table name and determine if the table already exists there */
       22:  349:                        Status = CFE_ES_RegisterCDSEx(&RegRecPtr->CDSHandle, Size, TblName, TRUE);
        -:  350:
       22:  351:                        if (Status == CFE_ES_CDS_ALREADY_EXISTS)
        -:  352:                        {
        8:  353:                            Status = CFE_TBL_GetWorkingBuffer(&WorkingBufferPtr, RegRecPtr, TRUE);
        -:  354:
        8:  355:                            if (Status != CFE_SUCCESS)
        -:  356:                            {
        -:  357:                                /* Unable to get a working buffer - this error is not really */
        -:  358:                                /* possible at this point during table registration.  But we */
        -:  359:                                /* do need to handle the error case because if the function */
        -:  360:                                /* call did fail, WorkingBufferPtr would be a NULL pointer. */
    #####:  361:                                CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
    #####:  362:                                CFE_ES_WriteToSysLog("CFE_TBL:Register-Failed to get work buffer for '%s.%s' (ErrCode=0x%08X)\n",
        -:  363:                                                     AppName, Name, (unsigned int)Status);
        -:  364:                            }
        
REASON: See comment on lines 359-360        

In CFE_TBL_Update:

       10:  981:    if (Status != CFE_TBL_ERR_BAD_APP_ID)
        -:  982:    {
        -:  983:        /* Translate AppID of caller into App Name */
        8:  984:        CFE_ES_GetAppName(AppName, ThisAppId, OS_MAX_API_NAME);
        -:  985:    }
        -:  986:
        -:  987:    /* On Error conditions, notify ground of screw up */
       10:  988:    if (Status < 0)
        -:  989:    {
        4:  990:        if (RegRecPtr != NULL)
        -:  991:        {
    #####:  992:            CFE_EVS_SendEventWithAppID(CFE_TBL_UPDATE_ERR_EID,
        -:  993:                                       CFE_EVS_EventType_ERROR,
        -:  994:                                       CFE_TBL_TaskData.TableTaskAppId,
        -:  995:                                       "%s Failed to Update '%s', Status=0x%08X",
        -:  996:                                       AppName, RegRecPtr->Name, (unsigned int)Status);
        -:  997:        }

REASON: This event message is never issued in unit testing because in order for it to
occur, the function CFE_TBL_UpdateInternal would be required to return an error
code.  At the current time, the CFE_TBL_UpdateInternal function only returns
either CFE_SUCCESS or informational status. The event message is being
maintained in the event the CFE_TBL_UpdateInternal function is modified to
return an error.

In CFE_TBL_Manage:

      104: 1349:    while (!FinishedManaging)
        -: 1350:    {
        -: 1351:        /* Determine if the table has a validation or update that needs to be performed */
       48: 1352:        Status = CFE_TBL_GetStatus(TblHandle);
        -: 1353:
       48: 1354:        if (Status == CFE_TBL_INFO_VALIDATION_PENDING)
        -: 1355:        {
        -: 1356:            /* Validate the specified Table */
       20: 1357:            Status = CFE_TBL_Validate(TblHandle);
        -: 1358:
       20: 1359:            if (Status != CFE_SUCCESS)
        -: 1360:            {
        -: 1361:                /* If an error occurred during Validate, then do not perform any more managing */
    #####: 1362:                FinishedManaging = TRUE;
        -: 1363:            }
        -: 1364:        }
       28: 1365:        else if (Status == CFE_TBL_INFO_DUMP_PENDING)

REASON: The FinishedManaging variable can currently never be set because the only error
conditions that CFE_TBL_Validate can return are already captured by the
validation performed in the CFE_TBL_GetStatus function.

==========================================================================

cfe_tbl_internal.c - 100% coverage

==========================================================================

cfe_tbl_task_cmds.c - 99.77% coverage (1 Line of Code NOT executed)

In CFE_TBL_LoadCmd:

        4:  495:                        else if (Status == CFE_TBL_ERR_NO_BUFFER_AVAIL)
        -:  496:                        {
        4:  497:                            CFE_EVS_SendEvent(CFE_TBL_NO_WORK_BUFFERS_ERR_EID,
        -:  498:                                              CFE_EVS_EventType_ERROR,
        -:  499:                                              "No working buffers available for table '%s'",
        -:  500:                                              TblFileHeader.TableName);
        -:  501:                        }
        -:  502:                        else
        -:  503:                        {
    #####:  504:                            CFE_EVS_SendEvent(CFE_TBL_INTERNAL_ERROR_ERR_EID,
        -:  505:                                              CFE_EVS_EventType_ERROR,
        -:  506:                                              "Internal Error (Status=0x%08X)",
        -:  507:                                              (unsigned int)Status);
        -:  508:                        }

REASON: This event message is only issued when an unexpected error is detected in
underlying function calls.  In the current implementation, there isn't any
possibility of an unexpected error to occur.

==========================================================================

cfe_tbl_task.c - 100% coverage

==========================================================================
