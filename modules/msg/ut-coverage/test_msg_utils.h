/**
 * @file
 *
 * Message header test utilities
 */

#ifndef TEST_MSG_UTILS_H
#define TEST_MSG_UTILS_H

/*
 * Includes
 */
#include "cfe_msg_api_typedefs.h"

/*
 * Defines
 */

/* Subtest macro */
#define MSG_UT_ADD_SUBTEST(Func) UtTest_AddSubTest(Func, NULL, NULL, __func__, #Func)

#endif /* TEST_MSG_UTILS_H */
