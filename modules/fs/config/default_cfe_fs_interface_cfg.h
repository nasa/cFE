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
 *
 * Declarations and prototypes for cfe_fs_extern_typedefs module
 */

#ifndef CFE_FS_INTERFACE_CFG_H
#define CFE_FS_INTERFACE_CFG_H

/******************* Macro Definitions ***********************/

/*
 * NOTE: the value of CFE_FS_HDR_DESC_MAX_LEN, if modified, should
 * be constrained to multiples of 4, as it is used within a structure that
 * also contains uint32 types.  This ensures that the entire structure
 * remains 32-bit aligned without the need for implicit padding bytes.
 */

#define CFE_FS_HDR_DESC_MAX_LEN 32 /**< \brief Max length of description field in a standard cFE File Header */

#define CFE_FS_FILE_CONTENT_ID 0x63464531 /**< \brief Magic Number for cFE compliant files (= 'cFE1') */

#endif /* CFE_FS_INTERFACE_CFG_H */
