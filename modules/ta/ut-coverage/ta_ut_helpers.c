#include "ta_ut_helpers.h"

UT_TA_EventCapture_t UT_TA_EventBuf;

void UT_InitData_TA(void)
{
    UT_InitData();
    memset(&UT_TA_EventBuf, 0, sizeof(UT_TA_EventBuf));
}

/* Custom hook to intercept CFE_EVS_SendEvent and record the exact EventID that was sent */
static int32 UT_TA_EventHook(void *UserObj, int32 StubRetcode, uint32 CallCount, const UT_StubContext_t *Context)
{
    UT_TA_EventCapture_t *EventCapture = UserObj;

    /* Grab the 'EventID' argument passed to CFE_EVS_SendEvent */
    EventCapture->EventID = UT_Hook_GetArgValueByName(Context, "EventID", uint16);
    EventCapture->Count++;

    return StubRetcode;
}

void UT_TA_DoDispatchCheckEvents(void                   *MsgPtr,
                                 size_t                  MsgSize,
                                 UT_TaskPipeDispatchId_t DispatchId,
                                 UT_TA_EventCapture_t   *EventCapture)
{
    EventCapture->EventID = 0xFFFF; /* Reset to unknown */
    EventCapture->Count   = 0;

    /* Tell the UT Framework to route CFE_EVS_SendEvent calls to our hook */
    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), UT_TA_EventHook, EventCapture);

    /* EDS hack: if we are in EDS mode and simulating a bad length (size=0), force the error */
    if (MsgSize == 0 && EdsDispatch_EdsComponent_CFE_TA_Application_Telecommand != NULL)
    {
        UT_SetDeferredRetcode(UT_KEY(EdsDispatch_EdsComponent_CFE_TA_Application_Telecommand),
                              1,
                              CFE_STATUS_WRONG_MSG_LENGTH);
    }

    /* Safely dispatch the command (handles both Native size mocking and EDS routing) */
    UT_CallTaskPipe(CFE_TA_TaskPipe, (CFE_MSG_Message_t *)MsgPtr, MsgSize, DispatchId);

    UT_SetHookFunction(UT_KEY(CFE_EVS_SendEvent), NULL, NULL);
}
