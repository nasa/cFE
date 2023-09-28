/************************************************************************
 * NASA Docket No. GSC-18,719-1, and identified as “core Flight System: Bootes”
 *
 * Copyright (c) 2020 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License. You may obtain
 * a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ************************************************************************/

/**
 * @file
 *  Implemention of the CRC algorithm(s) for CFE ES
 *
 *  Notes:
 *    Historically only CRC-16/ARC is defined, but this can be expanded.
 *
 */

#ifndef CFE_ES_CRC_H
#define CFE_ES_CRC_H

/*
** Include Files
*/
#include "common_types.h"
#include "cfe_es.h"

typedef uint32 (*const CFE_ES_ComputeCRC_Algo_t)(const void *DataPtr, size_t DataLength, uint32 InputCRC);

typedef const struct CFE_ES_ComputeCRC_Params
{
    uint32 InitialValue;
    uint32 FinalXOR;

    /**
     * Function that implments the processing for this algorithm
     */
    CFE_ES_ComputeCRC_Algo_t Algorithm;

} CFE_ES_ComputeCRC_Params_t;

CFE_ES_ComputeCRC_Params_t *CFE_ES_ComputeCRC_GetParams(CFE_ES_CrcType_Enum_t CrcType);

#endif /* CFE_ES_CRC_H */
