#ifndef TA_UT_H
#define TA_UT_H

#include "utassert.h"
#include "uttest.h"
#include "utstubs.h"
#include "ut_support.h"

#include "cfe_ta_task.h"
#include "cfe_ta_module_all.h"
#include "cfe_ta_eventids.h"
#include "cfe_config.h"

/* Global data structure from cfe_ta_task.c */
extern CFE_TA_Global_t CFE_TA_Global;

/* Weak declaration allows this to safely compile in both Native and EDS modes */
extern int32 EdsDispatch_EdsComponent_CFE_TA_Application_Telecommand(const CFE_SB_Buffer_t *, const void *)
    __attribute__((weak));

/* Helper macro to add tests with automatic reset */
#define TA_UT_ADD_SUBTEST(Func) UtTest_AddSubTest(Func, TA_ResetUnitTest, NULL, __func__, #Func)

/* External task loop functions */
extern void  CFE_TA_TaskMain(void);
extern int32 CFE_TA_SetStartupAffinity(const char *taskname);
extern void  CFE_TA_TaskPipe(const CFE_SB_Buffer_t *SBBufPtr);

/* Prototypes for test groups */
void Test_Init(void);
void Test_Main(void);
void Test_Cmds(void);
void Test_QueryAll(void);
void Test_StartupAffinity(void);
void Test_Events(void);

/* Prototypes for individual tests */
void Test_TA_EarlyInit_NullTable(void);
void Test_TA_EarlyInit_WithTable(void);
void Test_TA_EarlyInit_MaxTasks(void);
void Test_TA_EarlyInit_Error(void);
void Test_TA_TaskInit(void);
void Test_TA_TaskInit_Errors(void);
void Test_TA_CleanUpApp(void);
void Test_TA_EarlyInit_HandlerError(void);

void Test_TA_Main_InitFail(void);
void Test_TA_Main_RcvError(void);
void Test_TA_Main_Nominal(void);

void Test_TA_TaskPipe_InvalidMsgId(void);
void Test_TA_TaskPipe_HK(void);
void Test_TA_ProcessCmd_BadCC(void);

void Test_TA_ProcessCmd_BadLength_Noop(void);
void Test_TA_ProcessCmd_BadLength_Reset(void);
void Test_TA_ProcessCmd_BadLength_QueryAll(void);
void Test_TA_ProcessCmd_BadLength_SetAffinity(void);
void Test_TA_ProcessCmd_BadLength_GetAffinity(void);

void Test_TA_ProcessCmd_QueryAll(void);
void Test_TA_ProcessCmd_SetAffinity(void);
void Test_TA_ProcessCmd_GetAffinity(void);
void Test_TA_ProcessCmd_ResetCounters(void);
void Test_TA_NoopCmd(void);
void Test_TA_ResetCountersCmd(void);
void Test_TA_Dispatch_CommandError(void);

void Test_TA_SetTaskAffinityCmd_Nominal(void);
void Test_TA_SetTaskAffinityCmd_NotFound(void);
void Test_TA_SetTaskAffinityCmd_SetError(void);
void Test_TA_SetTaskAffinityCmd_GetError(void);
void Test_TA_SetTaskAffinityCmd_TableFull(void);
void Test_TA_SetTaskAffinityCmd_IdNotFound(void);

void Test_TA_GetTaskAffinityCmd_NominalSame(void);
void Test_TA_GetTaskAffinityCmd_NominalDiff(void);
void Test_TA_GetTaskAffinityCmd_NotFound(void);
void Test_TA_GetTaskAffinityCmd_GetError(void);
void Test_TA_GetTaskAffinityCmd_TableFull(void);
void Test_TA_GetTaskAffinityCmd_IdNotFound(void);

void Test_TA_CpuGet_MaxCores(void);
void Test_TA_CpuSet_LoopLimit_MaxBits(void);
void Test_TA_CpuSet_LoopLimit_MaxCpus(void);
void Test_TA_CpuSet_LoopLimit_AvailCores(void);

void Test_TA_QueryAll_Nominal(void);
void Test_TA_QueryAll_ParseError(void);
void Test_TA_QueryAll_OpenError(void);
void Test_TA_QueryAll_WriteHdrError(void);
void Test_TA_QueryAll_WriteDataError(void);

void Test_TA_SetStartupAffinity_AddSuccess(void);
void Test_TA_SetStartupAffinity_AddGetError(void);
void Test_TA_SetStartupAffinity_OsDefaultSuccess(void);
void Test_TA_SetStartupAffinity_OsDefaultGetError(void);
void Test_TA_SetStartupAffinity_MaskSuccess(void);
void Test_TA_SetStartupAffinity_MaskSetError(void);
void Test_TA_SetStartupAffinity_MaskGetError(void);
void Test_TA_SetStartupAffinity_InvalidType(void);
void Test_TA_SetStartupAffinity_TableFull(void);
void Test_TA_SetStartupAffinity_IdNotFound(void);

void Test_TA_OS_EventHandler_Nominal(void);
void Test_TA_OS_EventHandler_OtherEvent(void);
void Test_TA_OS_EventHandler_AllOtherEvents(void);
void Test_TA_OS_EventHandler_GetNameError(void);

#endif /* TA_UT_H */
