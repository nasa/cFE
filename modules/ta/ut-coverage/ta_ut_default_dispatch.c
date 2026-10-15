#include "ta_UT.h"
#include "ta_ut_helpers.h"

#define TA_UT_MID_DISPATCH(intf) \
    .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, .MsgId = CFE_SB_MSGID_WRAP_VALUE(CFE_TA_##intf##_MID)

#define TA_UT_MSG_DISPATCH(intf, cmd)       TA_UT_MID_DISPATCH(intf), UT_TPD_SETSIZE(CFE_TA_##cmd)
#define TA_UT_CC_DISPATCH(intf, cc, cmd)    TA_UT_MSG_DISPATCH(intf, cmd), UT_TPD_SETCC(cc)
#define TA_UT_ERROR_DISPATCH(intf, cc, err) TA_UT_MID_DISPATCH(intf), UT_TPD_SETCC(cc), UT_TPD_SETERR(err)

/* clang-format off */
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_NOOP_CC = { TA_UT_CC_DISPATCH(CMD, CFE_TA_NOOP_CC, NoopCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_RESET_COUNTERS_CC = { TA_UT_CC_DISPATCH(CMD, CFE_TA_RESET_COUNTERS_CC, ResetCountersCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_QUERY_ALL_TASKS_CC = { TA_UT_CC_DISPATCH(CMD, CFE_TA_QUERY_ALL_TASKS_CC, QueryAllTasksCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_SET_TASK_AFFINITY_CC = { TA_UT_CC_DISPATCH(CMD, CFE_TA_SET_TASK_AFFINITY_CC, SetTaskAffinityCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_GET_TASK_AFFINITY_CC = { TA_UT_CC_DISPATCH(CMD, CFE_TA_GET_TASK_AFFINITY_CC, GetTaskAffinityCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_SEND_HK = { TA_UT_MSG_DISPATCH(SEND_HK, SendHkCmd) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_INVALID_MID = { .Method = UT_TaskPipeDispatchMethod_MSG_ID_CC, UT_TPD_SETERR(CFE_STATUS_UNKNOWN_MSG_ID) };
const UT_TaskPipeDispatchId_t UT_TPID_CFE_TA_CMD_INVALID_CC = { TA_UT_ERROR_DISPATCH(CMD, -1, CFE_STATUS_BAD_COMMAND_CODE) };
/* clang-format on */
