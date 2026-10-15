#ifndef TA_UT_HELPERS_H
#define TA_UT_HELPERS_H

#include "ta_UT.h"

typedef struct
{
    uint16 EventID;
    uint16 Count;
} UT_TA_EventCapture_t;

extern UT_TA_EventCapture_t UT_TA_EventBuf;

/* Stubs for dispatch IDs */
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_NOOP_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_RESET_COUNTERS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_QUERY_ALL_TASKS_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_SET_TASK_AFFINITY_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_GET_TASK_AFFINITY_CC;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_SEND_HK;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_INVALID_MID;
extern const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_INVALID_CC;

void UT_InitData_TA(void);
void UT_TA_DoDispatchCheckEvents(void                   *MsgPtr,
                                 size_t                  MsgSize,
                                 UT_TaskPipeDispatchId_t DispatchId,
                                 UT_TA_EventCapture_t   *EventCapture);

#endif /* TA_UT_HELPERS_H */
