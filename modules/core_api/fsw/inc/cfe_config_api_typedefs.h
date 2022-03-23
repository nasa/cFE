/**
 * @file
 *
 *  Title:    cFE Status Code Definition Header File
 *
 *  Purpose:
 *            Common source of cFE API return status codes.
 *
 *  Design Notes:
 *
 *  References:
 *     Flight Software Branch C Coding Standard Version 1.0a
 *
 */

#ifndef CFE_CONFIG_API_TYPEDEFS_H
#define CFE_CONFIG_API_TYPEDEFS_H

/* Include Files */
#include "common_types.h"
#include "cfe_resourceid_api_typedefs.h"

/**
 * @brief A type for Configuration IDs
 *
 * This is the type that is used for any API accepting or returning a configuration key ID
 */
typedef CFE_RESOURCEID_BASE_TYPE CFE_ConfigId_t;

#define CFE_CONFIGID_C(val)    ((CFE_ConfigId_t)CFE_RESOURCEID_WRAP(val))
#define CFE_CONFIGID_UNDEFINED CFE_CONFIGID_C(CFE_RESOURCEID_UNDEFINED)

typedef void (*CFE_Config_Callback_t)(void *Arg, CFE_ConfigId_t Id, const char *Name);

#endif /* CFE_CONFIG_API_TYPEDEFS_H */
