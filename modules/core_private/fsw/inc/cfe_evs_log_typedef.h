/**
 * @file
 *
 * Definition of the CFE_EVS_Log structure type.
 * This was moved into its own header file since it is referenced by multiple CFE core apps.
 */

#ifndef CFE_EVS_LOG_TYPEDEF_H
#define CFE_EVS_LOG_TYPEDEF_H

#include "common_types.h"
#include "cfe_platform_cfg.h"

#include "cfe_evs_msg.h" /* Required for CFE_EVS_LongEventTlm_t definition */

/*
** \brief  EVS Log type definition. This is declared here so ES can include it
**  in the reset area structure
*/
typedef struct
{
    uint16                 Next;               /**< \brief Index of the next entry in the local event log */
    uint16                 LogCount;           /**< \brief Local Event Log counter */
    uint8                  LogFullFlag;        /**< \brief Local Event Log full flag */
    uint8                  LogMode;            /**< \brief Local Event Logging mode (overwrite/discard) */
    uint16                 LogOverflowCounter; /**< \brief Local Event Log overflow counter */
    CFE_EVS_LongEventTlm_t LogEntry[CFE_PLATFORM_EVS_LOG_MAX]; /**< \brief The actual Local Event Log entry */

} CFE_EVS_Log_t;

#endif /* CFE_EVS_LOG_TYPEDEF_H */
