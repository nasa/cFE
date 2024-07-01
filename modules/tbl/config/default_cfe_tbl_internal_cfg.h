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
 *   CFE Table Services (CFE_TBL) Application Private Config Definitions
 *
 * This provides default values for configurable items that are internal
 * to this module and do NOT affect the interface(s) of this module.  Changes
 * to items in this file only affect the local module and will be transparent
 * to external entities that are using the public interface(s).
 *
 * @note This file may be overridden/superceded by mission-provided definitions
 * either by overriding this header or by generating definitions from a command/data
 * dictionary tool.
 */
#ifndef CFE_TBL_INTERNAL_CFG_H
#define CFE_TBL_INTERNAL_CFG_H

/**
**  \cfeescfg Define TBL Task Priority
**
**  \par Description:
**       Defines the cFE_TBL Task priority.
**
**  \par Limits
**       Not Applicable
*/
#define CFE_PLATFORM_TBL_START_TASK_PRIORITY 70

/**
**  \cfeescfg Define TBL Task Stack Size
**
**  \par Description:
**       Defines the cFE_TBL Task Stack Size
**
**  \par Limits
**       There is a lower limit of 2048 on this configuration parameter.  There
**       are no restrictions on the upper limit however, the maximum stack size
**       is system dependent and should be verified.  Most operating systems provide
**       tools for measuring the amount of stack used by a task during operation. It
**       is always a good idea to verify that no more than 1/2 of the stack is used.
*/
#define CFE_PLATFORM_TBL_START_TASK_STACK_SIZE CFE_PLATFORM_ES_DEFAULT_STACK_SIZE

/* Platform Configuration Parameters for Table Service (TBL) */

/**
**  \cfetblcfg Size of Table Services Table Memory Pool
**
**  \par Description:
**       Defines the TOTAL size of the memory pool that cFE Table Services allocates
**       from the system.  The size must be large enough to provide memory for each
**       registered table, the inactive buffers for double buffered tables and for
**       the shared inactive buffers for single buffered tables.
**
**  \par Limits
**       The cFE does not place a limit on the size of this parameter.
*/
#define CFE_PLATFORM_TBL_BUF_MEMORY_BYTES 524288

/**
**  \cfetblcfg Maximum Size Allowed for a Double Buffered Table
**
**  \par Description:
**       Defines the maximum allowed size (in bytes) of a double buffered table.
**
**  \par Limits
**       The cFE does not place a limit on the size of this parameter but it must be
**       less than half of #CFE_PLATFORM_TBL_BUF_MEMORY_BYTES.
*/
#define CFE_PLATFORM_TBL_MAX_DBL_TABLE_SIZE 16384

/**
**  \cfetblcfg Maximum Size Allowed for a Single Buffered Table
**
**  \par Description:
**       Defines the maximum allowed size (in bytes) of a single buffered table.
**       \b NOTE: This size determines the size of all shared table buffers.
**       Therefore, this size will be multiplied by #CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS
**       below when allocating memory for shared tables.
**
**  \par Limits
**       The cFE does not place a limit on the size of this parameter but it must be
**       small enough to allow for #CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS number of tables
**       to fit into #CFE_PLATFORM_TBL_BUF_MEMORY_BYTES.
*/
#define CFE_PLATFORM_TBL_MAX_SNGL_TABLE_SIZE 16384

/**
**  \cfetblcfg Maximum Number of Tables Allowed to be Registered
**
**  \par Description:
**       Defines the maximum number of tables supported by this processor's Table Services.
**
**  \par Limits
**       This number must be less than 32767.  It should be recognized that this parameter
**       determines the size of the Table Registry.  An excessively high number will waste
**       memory.
*/
#define CFE_PLATFORM_TBL_MAX_NUM_TABLES 128

/**
**  \cfetblcfg Maximum Number of Critical Tables that can be Registered
**
**  \par Description:
**       Defines the maximum number of critical tables supported by this processor's Table Services.
**
**  \par Limits
**       This number must be less than 32767.  It should be recognized that this parameter
**       determines the size of the Critical Table Registry which is maintained in the Critical
**       Data Store.  An excessively high number will waste Critical Data Store memory.  Therefore,
**       this number must not exceed the value defined in CFE_PLATFORM_ES_CDS_MAX_NUM_ENTRIES.
*/
#define CFE_PLATFORM_TBL_MAX_CRITICAL_TABLES 32

/**
**  \cfetblcfg Maximum Number of Table Handles
**
**  \par Description:
**       Defines the maximum number of Table Handles.
**
**  \par Limits
**       This number must be less than 32767.  This number must be at least as big as
**       the number of tables (#CFE_PLATFORM_TBL_MAX_NUM_TABLES) and should be set higher if tables
**       are shared between applications.
*/
#define CFE_PLATFORM_TBL_MAX_NUM_HANDLES 256

/**
**  \cfetblcfg Maximum Number of Simultaneous Loads to Support
**
**  \par Description:
**       Defines the maximum number of single buffered tables that can be
**       loaded simultaneously.  This number is used to determine the number
**       of shared buffers to allocate.
**
**  \par Limits
**       This number must be less than 32767.  An excessively high number will
**       degrade system performance and waste memory.  A number less than 5 is
**       suggested but not required.
*/
#define CFE_PLATFORM_TBL_MAX_SIMULTANEOUS_LOADS 4

/**
**  \cfetblcfg Maximum Number of Simultaneous Table Validations
**
**  \par Description:
**       Defines the maximum number of pending validations that
**       the Table Services can handle at any one time.  When a
**       table has a validation function, a validation request is
**       made of the application to perform that validation.  This
**       number determines how many of those requests can be
**       outstanding at any one time.
**
**  \par Limits
**       This number must be less than 32767.  An excessively high number will
**       degrade system performance and waste memory.  A number less than 20 is
**       suggested but not required.
*/
#define CFE_PLATFORM_TBL_MAX_NUM_VALIDATIONS 10

/**
**  \cfetblcfg Default Filename for a Table Registry Dump
**
**  \par Description:
**       Defines the file name used to store the table registry when
**       no filename is specified in the dump registry command.
**
**  \par Limits
**       The length of each string, including the NULL terminator cannot exceed the
**       #OS_MAX_PATH_LEN value.
*/
#define CFE_PLATFORM_TBL_DEFAULT_REG_DUMP_FILE "/ram/cfe_tbl_reg.log"

/**
**  \cfetblcfg Number of Spacecraft ID's specified for validation
**
**  \par Description:
**       Defines the number of specified spacecraft ID values that
**       are verified during table loads.  If the number is zero
**       then no validation of the spacecraft ID field in the table
**       file header is performed when tables are loaded.  Non-zero
**       values indicate how many values from the list of spacecraft
**       ID's defined below are compared to the spacecraft ID field
**       in the table file header.  The ELF2CFETBL tool may be used
**       to create table files with specified spacecraft ID values.
**
**  \par Limits
**       This number must be greater than or equal to zero and
**       less than or equal to 2.
*/
#define CFE_PLATFORM_TBL_VALID_SCID_COUNT 0

/* macro to construct 32 bit value from 4 chars */
#define CFE_PLATFORM_TBL_U32FROM4CHARS(_C1, _C2, _C3, _C4) \
    ((uint32)(_C1) << 24 | (uint32)(_C2) << 16 | (uint32)(_C3) << 8 | (uint32)(_C4))

/**
**  \cfetblcfg Spacecraft ID values used for table load validation
**
**  \par Description:
**       Defines the spacecraft ID values used for validating the
**       spacecraft ID field in the table file header.  To be valid,
**       the spacecraft ID specified in the table file header must
**       match one of the values defined here.
**
**  \par Limits
**       This value can be any 32 bit unsigned integer.
*/
#define CFE_PLATFORM_TBL_VALID_SCID_1 (0x42)
#define CFE_PLATFORM_TBL_VALID_SCID_2 (CFE_PLATFORM_TBL_U32FROM4CHARS('a', 'b', 'c', 'd'))

/**
**  \cfetblcfg Number of Processor ID's specified for validation
**
**  \par Description:
**       Defines the number of specified processor ID values that
**       are verified during table loads.  If the number is zero
**       then no validation of the processor ID field in the table
**       file header is performed when tables are loaded.  Non-zero
**       values indicate how many values from the list of processor
**       ID's defined below are compared to the processor ID field
**       in the table file header.  The ELF2CFETBL tool may be used
**       to create table files with specified processor ID values.
**
**  \par Limits
**       This number must be greater than or equal to zero and
**       less than or equal to 4.
*/
#define CFE_PLATFORM_TBL_VALID_PRID_COUNT 0

/**
**  \cfetblcfg Processor ID values used for table load validation
**
**  \par Description:
**       Defines the processor ID values used for validating the
**       processor ID field in the table file header.  To be valid,
**       the spacecraft ID specified in the table file header must
**       match one of the values defined here.
**
**  \par Limits
**       This value can be any 32 bit unsigned integer.
*/
#define CFE_PLATFORM_TBL_VALID_PRID_1 (1)
#define CFE_PLATFORM_TBL_VALID_PRID_2 (CFE_PLATFORM_TBL_U32FROM4CHARS('a', 'b', 'c', 'd'))
#define CFE_PLATFORM_TBL_VALID_PRID_3 0
#define CFE_PLATFORM_TBL_VALID_PRID_4 0

#endif
