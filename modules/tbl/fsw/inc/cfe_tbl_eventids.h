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
 *  cFE Table Services Event IDs
 */

#ifndef CFE_TBL_EVENTS_H
#define CFE_TBL_EVENTS_H

/**
 * \name TBL event IDs
 */
/**\{*/

/**
 * \brief TB Initialization Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Table Services Task initialization complete.
 */
#define CFE_TBL_INIT_INF_EID 1

/**
 * \brief TBL No-op Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_NOOP_CC NO-OP TBL No-op Command \endlink success.
 */
#define CFE_TBL_NOOP_INF_EID 10

/**
 * \brief TBL Reset Counters Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_RESET_COUNTERS_CC TBL Reset Counters Command \endlink success.
 */
#define CFE_TBL_RESET_INF_EID 11

/**
 * \brief TBL Load Table Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_LOAD_CC TBL Load Table Command \endlink successfully loaded
 *  the new table data to the working buffer.
 */
#define CFE_TBL_FILE_LOADED_INF_EID 12

/**
 * \brief TBL Write Table To Existing File Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  TBL write table to an existing file success.
 */
#define CFE_TBL_OVERWRITE_DUMP_INF_EID 13

/**
 * \brief TBL Write Table To New File Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  TBL write table to a new file success.
 */
#define CFE_TBL_WRITE_DUMP_INF_EID 14

/**
 * \brief TBL Write Table Registry To Existing File Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  TBL Write Table Registry to an existing file completed successfully.
 */
#define CFE_TBL_OVERWRITE_REG_DUMP_INF_EID 15

/**
 * \brief TBL Validate Table Request Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_VALIDATE_CC TBL Validate Table Command \endlink success.
 *  Note this event signifies the request to validate the table has been successfully
 *  submitted. Completion will generate a #CFE_TBL_VALIDATION_INF_EID or
 *  #CFE_TBL_VALIDATION_ERR_EID event messages.
 */
#define CFE_TBL_VAL_REQ_MADE_INF_EID 16

/**
 * \brief TBL Load Table Pending Notification Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  TBL load table pending notification successfully sent.
 */
#define CFE_TBL_LOAD_PEND_REQ_INF_EID 17

/**
 * \brief TBL Telemeter Table Registry Entry Command Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_SEND_REGISTRY_CC TBL Telemeter Table Registry Entry command \endlink successfully
 *  set the table registry index to telemeter in the next housekeeping packet.
 */
#define CFE_TBL_TLM_REG_CMD_INF_EID 18

/**
 * \brief TBL Abort Table Load Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_ABORT_LOAD_CC TBL Abort Table Load Command \endlink success.
 */
#define CFE_TBL_LOAD_ABORT_INF_EID 21

/**
 * \brief TBL Write Table Registry To New File Success Event ID
 *
 *  \par Type: DEBUG
 *
 *  \par Cause:
 *
 *  TBL Write Table Registry to a new file completed successfully.
 */
#define CFE_TBL_WRITE_REG_DUMP_INF_EID 22

/**
 * \brief TBL Validate Table Valid Due To No Validation Function Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_VALIDATE_CC TBL Validate Table Command \endlink marking table
 *  as valid due to no validation function being registered.
 */
#define CFE_TBL_ASSUMED_VALID_INF_EID 23

/**
 * \brief TBL Load Table API Success Event ID
 *
 *  \par Type: DEBUG (the first time) and INFORMATION (normally)
 *
 *  \par Cause:
 *
 *  #CFE_TBL_Load API success for dump only or normal table. OVERLOADED
 */
#define CFE_TBL_LOAD_SUCCESS_INF_EID 35

/**
 * \brief TBL Validate Table Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Table active or inactive image successfully validated by the registered validation function. OVERLOADED
 */
#define CFE_TBL_VALIDATION_INF_EID 36

/**
 * \brief TBL Update Table Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  Table update successfully completed.
 */
#define CFE_TBL_UPDATE_SUCCESS_INF_EID 37

/**
 * \brief TBL Delete Table CDS Command Success Event ID
 *
 *  \par Type: INFORMATION
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DELETE_CDS_CC TBL Delete Table CDS Command \endlink success.
 */
#define CFE_TBL_CDS_DELETED_INFO_EID 38

/**
 * \brief TBL Invalid Message ID Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid message ID received on the TBL message pipe.
 */
#define CFE_TBL_MID_ERR_EID 50

/**
 * \brief TBL Invalid Command Code Received Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid command code for message ID #CFE_TBL_CMD_MID received on the TBL message pipe.
 */
#define CFE_TBL_CC1_ERR_EID 51

/**
 * \brief TBL Invalid Command Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid length for the message ID and command code received on the TBL message pipe.
 */
#define CFE_TBL_LEN_ERR_EID 52

/**
 * \brief TBL Load Table File Open Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Load Table failure opening the file. OVERLOADED
 */
#define CFE_TBL_FILE_ACCESS_ERR_EID 53

/**
 * \brief TBL Load Table File Read Standard Header Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Load Table failure reading the file standard header.
 */
#define CFE_TBL_FILE_STD_HDR_ERR_EID 54

/**
 * \brief TBL Load Table File Read Table Header Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Load Table failure reading the file table header.
 */
#define CFE_TBL_FILE_TBL_HDR_ERR_EID 55

/**
 * \brief TBL Send Housekeeping Command Transmit Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_SEND_HK_MID TBL Send Housekeeping Command \endlink failure
 *  transmitting the housekeeping message.
 */
#define CFE_TBL_FAIL_HK_SEND_ERR_EID 56

/**
 * \brief TBL Table Name Not Found Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL command handler unable to find table name. OVERLOADED
 */
#define CFE_TBL_NO_SUCH_TABLE_ERR_EID 57

/**
 * \brief TBL Load Table Invalid File Content ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table failure due to invalid file content ID.
 */
#define CFE_TBL_FILE_TYPE_ERR_EID 58

/**
 * \brief TBL Load Table Invalid File Subtype Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table Failure due to invalid file subtype.
 */
#define CFE_TBL_FILE_SUBTYPE_ERR_EID 59

/**
 * \brief TBL Load Or Dump Table No Working Buffers Available Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load or Dump failure due to no working buffers available or internal error.  OVERLOADED
 */
#define CFE_TBL_NO_WORK_BUFFERS_ERR_EID 60

/**
 * \brief TBL Write File Creation Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Write Table or Table Registry File failed to create file. OVERLOADED
 */
#define CFE_TBL_CREATING_DUMP_FILE_ERR_EID 62

/**
 * \brief TBL Write Standard File Header Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Write Table or Table Registry File failure writing the standard file header. OVERLOADED
 */
#define CFE_TBL_WRITE_CFE_HDR_ERR_EID 63

/**
 * \brief TBL Write Table File Header Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Write Table failure writing the table image file header.
 */
#define CFE_TBL_WRITE_TBL_HDR_ERR_EID 64

/**
 * \brief TBL Write Table File Data Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Write Table failure writing the table data.
 */
#define CFE_TBL_WRITE_TBL_IMG_ERR_EID 65

/**
 * \brief TBL Validate Or Write Table Command No Inactive Buffer Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_VALIDATE_CC TBL Validate Table Command \endlink or
 *  \link #CFE_TBL_DUMP_CC TBL Write Table Command \endlink failure due
 *  to requesting non-existent inactive buffer. OVERLOADED
 */
#define CFE_TBL_NO_INACTIVE_BUFFER_ERR_EID 66

/**
 * \brief TBL Validate Table Command Result Storage Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_VALIDATE_CC TBL Validate Table Command \endlink failure
 *  due to exceeding result storage.
 */
#define CFE_TBL_TOO_MANY_VALIDATIONS_ERR_EID 67

/**
 * \brief TBL Write Table Registry File Data Failure Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TB Write Table Registry failure writing file data.
 */
#define CFE_TBL_WRITE_TBL_REG_ERR_EID 68

/**
 * \brief TBL Abort Table Load No Load Started Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_ABORT_LOAD_CC TBL Abort Table Load Command \endlink failure
 *  due to no load in progress.
 */
#define CFE_TBL_LOAD_ABORT_ERR_EID 69

/**
 * \brief TBL Activate Table Command No Inactive Buffer Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_ACTIVATE_CC TBL Activate Table Command \endlink failure
 *  due to no associated inactive buffer.
 */
#define CFE_TBL_ACTIVATE_ERR_EID 70

/**
 * \brief TBL Load Table Incomplete Load Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table failure due to inability to read the size of data specified in the
 *  table header from file. OVERLOADED
 */
#define CFE_TBL_FILE_INCOMPLETE_ERR_EID 71

/**
 * \brief TBL Load Table File Exceeds Table Size Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table failure due to the file header specified offset and/or size of data exceeding the
 *  table size. OVERLOADED
 */
#define CFE_TBL_LOAD_EXCEEDS_SIZE_ERR_EID 72

/**
 * \brief TBL Load Table File Zero Length Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table failure due to the file header specified size of data being zero.
 */
#define CFE_TBL_ZERO_LENGTH_LOAD_ERR_EID 73

/**
 * \brief TBL Load Table Uninitialized Partial Load Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table failure due to attempting a partial load to an uninitialized table. OVERLOADED
 */
#define CFE_TBL_PARTIAL_LOAD_ERR_EID 74

/**
 * \brief TBL Load Table File Excess Data Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table failure due to the file header specified size of data being smaller than
 *  the actual data contained in the file. OVERLOADED
 */
#define CFE_TBL_FILE_TOO_BIG_ERR_EID 75

/**
 * \brief TBL Write Table Command Dump Only Control Blocks Exceeded Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DUMP_CC TBL Write Table Command \endlink failure due to exceeding the
 *  allocated number of control blocks available to write a dump only table.
 */
#define CFE_TBL_TOO_MANY_DUMPS_ERR_EID 76

/**
 * \brief TBL Write Table Command Already In Progress Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DUMP_CC TBL Write Table Command \endlink failure due to a dump already
 *  in progress for the same table.
 */
#define CFE_TBL_DUMP_PENDING_ERR_EID 77

/**
 * \brief TBL Activate Table Command For Dump Only Table Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_ACTIVATE_CC TBL Activate Table Command \endlink failure due
 *  to table being dump only.
 */
#define CFE_TBL_ACTIVATE_DUMP_ONLY_ERR_EID 78

/**
 * \brief TBL Load Table For Dump Only Table Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL Load Table failure due to table being dump only. OVERLOADED
 */
#define CFE_TBL_LOADING_A_DUMP_ONLY_ERR_EID 79

/**
 * \brief TBL Validate Or Write Table Command Invalid Buffer Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_VALIDATE_CC TBL Validate Table Command \endlink or
 *  \link #CFE_TBL_DUMP_CC TBL Write Table Command \endlink failure due
 *  to an invalid buffer selection. OVERLOADED
 */
#define CFE_TBL_ILLEGAL_BUFF_PARAM_ERR_EID 80

/**
 * \brief TBL Activate Table Command Inactive Image Not Validated Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_ACTIVATE_CC TBL Activate Table Command \endlink failure due
 *  to the inactive image not being validated.
 */
#define CFE_TBL_UNVALIDATED_ERR_EID 81

/**
 * \brief TBL Delete Table CDS Command For Registered Table Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DELETE_CDS_CC TBL Delete Table CDS Command \endlink failure
 *  due to the table being currently registered.
 */
#define CFE_TBL_IN_REGISTRY_ERR_EID 82

/**
 * \brief TBL Delete Table CDS Command Invalid CDS Type Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DELETE_CDS_CC TBL Delete Table CDS Command \endlink failure
 *  due to CDS being in the table registry but not registered as a table within ES.
 */
#define CFE_TBL_NOT_CRITICAL_TBL_ERR_EID 83

/**
 * \brief TBL Delete Table CDS Command Not In Critical Table Registry Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DELETE_CDS_CC TBL Delete Table CDS Command \endlink failure
 *  due to the table not being in the critical table registry.
 */
#define CFE_TBL_NOT_IN_CRIT_REG_ERR_EID 84

/**
 * \brief TBL Delete Table CDS Command Not In CDS Registry Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DELETE_CDS_CC TBL Delete Table CDS Command \endlink failure
 *  due to the table name not found in the CDS registry.
 */
#define CFE_TBL_CDS_NOT_FOUND_ERR_EID 85

/**
 * \brief TBL Delete Table CDS Command Internal Error Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DELETE_CDS_CC TBL Delete Table CDS Command \endlink failure
 *  due to an internal error.  See the system log for more information.
 */
#define CFE_TBL_CDS_DELETE_ERR_EID 86

/**
 * \brief TBL Delete Table CDS Command App Active Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_DELETE_CDS_CC TBL Delete Table CDS Command \endlink failure
 *  due to the owning application being active.
 */
#define CFE_TBL_CDS_OWNER_ACTIVE_ERR_EID 87

/**
 * \brief TBL Load Table Command Load Pending Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  \link #CFE_TBL_LOAD_CC TBL Load Table Command \endlink failed due to a load already pending.
 */
#define CFE_TBL_LOADING_PENDING_ERR_EID 88

/**
 * \brief TBL Send Notification Transmit Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL send notification transmit message failure.
 */
#define CFE_TBL_FAIL_NOTIFY_SEND_ERR_EID 89

/**
 * \brief TBL Register Table Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL table registration failure.  See system log for more information.
 */
#define CFE_TBL_REGISTER_ERR_EID 90

/**
 * \brief TBL Share Table Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL share table failure.  See system log for more information.
 */
#define CFE_TBL_SHARE_ERR_EID 91

/**
 * \brief TBL Unregister Table Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL unregister table failure. See system log for more information.
 */
#define CFE_TBL_UNREGISTER_ERR_EID 92

/**
 * \brief TBL Validation Function Invalid Return Code Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid table validation function return code.
 */
#define CFE_TBL_LOAD_VAL_ERR_EID 93

/**
 * \brief TBL Load Table API Invalid Source Type Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_TBL_Load API valid due to invalid source type.
 */
#define CFE_TBL_LOAD_TYPE_ERR_EID 94

/**
 * \brief TBL Update Table Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL update table failure due to an internal error. OVERLOADED
 */
#define CFE_TBL_UPDATE_ERR_EID 95

/**
 * \brief TBL Validate Table Validation Failed Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  TBL validate table function indicates validation failed. OVERLOADED
 */
#define CFE_TBL_VALIDATION_ERR_EID 96

/**
 * \brief TBL Read Header Invalid Spacecraft ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid spacecraft ID in table file header.
 */
#define CFE_TBL_SPACECRAFT_ID_ERR_EID 97

/**
 * \brief TBL Read Header Invalid Processor ID Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Invalid processor ID in table file header.
 */
#define CFE_TBL_PROCESSOR_ID_ERR_EID 98

/**
 * \brief TBL Load Table API Load Already In Progress Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_TBL_Load API failure due to load already in progress.
 */
#define CFE_TBL_LOAD_IN_PROGRESS_ERR_EID 100

/**
 * \brief TBL Load Table Filename Too Long Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Load table filename too long.
 */
#define CFE_TBL_LOAD_FILENAME_LONG_ERR_EID 101

/**
 * \brief TBL Load Table Name Mismatch Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  Load table name in the table file header does not match the specified table name.
 */
#define CFE_TBL_LOAD_TBLNAME_MISMATCH_ERR_EID 102

/**
 * \brief TBL Load Table API Access Violation Event ID
 *
 *  \par Type: ERROR
 *
 *  \par Cause:
 *
 *  #CFE_TBL_Load API failure due to the application not owning the table.
 */
#define CFE_TBL_HANDLE_ACCESS_ERR_EID 103
/**\}*/

#endif /* CFE_TBL_EVENTS_H */
