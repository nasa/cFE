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
 *   Specification for the CFE Event Services (CFE_EVS) command function codes
 *
 * @note
 *   This file should be strictly limited to the command/function code (CC)
 *   macro definitions.  Other definitions such as enums, typedefs, or other
 *   macros should be placed in the msgdefs.h or msg.h files.
 */
#ifndef CFE_EVS_FCNCODES_H
#define CFE_EVS_FCNCODES_H

/** \name Event Services Command Codes */
/** \{ */

/** \cfeevscmd Event Services No-Op
**
**  \par Description
**       This command performs no other function than to increment the
**       command execution counter. The command may be used to verify
**       general aliveness of the Event Services task.
**
**  \cfecmdmnemonic \EVS_NOOP
**
**  \par Command Structure
**       #CFE_EVS_NoopCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with the
**       following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**         increment
**       - The #CFE_EVS_NOOP_EID informational event message will
**         be generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Event
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS itself) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       None
**
**  \sa
*/
#define CFE_EVS_NOOP_CC 0

/** \cfeevscmd Event Services Reset Counters
**
**  \par Description
**       This command resets the following counters within the Event
**       Services housekeeping telemetry:
**       - Command Execution Counter (\EVS_CMDPC)
**       - Command Error Counter (\EVS_CMDEC)
**
**  \cfecmdmnemonic \EVS_RESETCTRS
**
**  \par Command Structure
**       #CFE_EVS_ResetCountersCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter
**         will be reset to 0
**       - \b \c \EVS_CMDEC - command error counter
**         will be reset to 0
**       - The #CFE_EVS_RSTCNT_EID debug event message will be
**         generated
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Event
**       Services receives the command, the event is sent (although it
**       may be filtered by EVS) and the counter is incremented
**       unconditionally.
**
**  \par Criticality
**       This command is not inherently dangerous.  However, it is
**       possible for ground systems and on-board safing procedures
**       to be designed such that they react to changes in the counter
**       values that are reset by this command.
**
**  \sa  #CFE_EVS_RESET_APP_COUNTER_CC
*/
#define CFE_EVS_RESET_COUNTERS_CC 1

/** \cfeevscmd Enable Event Type
**
**  \par Description
**       This command enables the command specified Event Type allowing event
**       messages of this type to be sent through Event Service.  An Event Type
**       is defined to be a classification of an Event Message such as debug,
**       informational, error and critical. This command is a global enable of a
**       particular event type, it applies to all applications.
**
**  \cfecmdmnemonic \EVS_ENAEVENTTYPE
**
**  \par Command Structure
**       #CFE_EVS_EnableEventTypeCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be enabled (or unfiltered).
**        A zero in a bit position means the filtering state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**
**        - \b \c \EVS_CMDPC - command execution counter will
**         increment
**        - The generation of  #CFE_EVS_ENAEVTTYPE_EID debug message
**
**  \par Error Conditions
**        This command may fail for the following reason(s):
**    	 - BitMask field invalid - mask cannot be zero, and only bits 0-3 may be set
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Enabling an event type is not particularly hazardous, as the result may
**      be turning on necessary event messages and communication to the ground
**      system.  However, inappropriately enabling an event type could result
**      in flooding of the system.
**
**  \sa #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC,
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_ENABLE_EVENT_TYPE_CC 2

/** \cfeevscmd Disable Event Type
**
**  \par Description
**      This command disables the command specified Event Type preventing event
**      messages of this type to be sent through Event Service.   An Event Type
**      is defined to be a classification of an Event Message such as debug,
**      informational, error and critical. This command is a global disable of a
**      particular event type, it applies to all applications.
**
**  \cfecmdmnemonic \EVS_DISEVENTTYPE
**
**  \par Command Structure
**       #CFE_EVS_DisableEventTypeCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be disabled (or filtered).
**        A zero in a bit position means the filtering state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
         - The generation of #CFE_EVS_DISEVTTYPE_EID debug message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**    	 - BitMask field invalid - mask cannot be zero, and only bits 0-3 may be set
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Disabling an event type is not particularly hazardous, as the result
**       may be shutting off unnecessary event messages and possible event
**       flooding of the system.  However, inappropriately disabling an event
**       type could result in a loss of critical information and missed
**       behavior for the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC,
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_DISABLE_EVENT_TYPE_CC 3

/** \cfeevscmd Set Event Format Mode
**
**  \par Description
**  This command sets the event format mode to the command specified value.
**  The event format mode may be either short or long.  A short event format
**  detaches the Event Data from the event message and only includes the
**  following information in the event packet: Processor ID, Application ID,
**  Event ID, and Event Type.  Refer to section 5.3.3.4 for a description of
**  the Event Service event packet contents.  Event Data is defined to be data
**  describing an Event that is supplied to the cFE Event Service.  ASCII text
**  strings are used as the primary format for Event Data because heritage
**  ground systems use string compares as the basis for their automated alert
**  systems.  Two systems, ANSR and SERS were looked at for interface
**  definitions.  The short event format is used to accommodate experiences
**  with limited telemetry bandwidth.  The long event format includes all event
**  information included within the short format along with the Event Data.
**
**  \cfecmdmnemonic \EVS_SETEVTFMT
**
**  \par Command Structure
**       #CFE_EVS_SetEventFormatModeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_SETEVTFMTMOD_EID debug message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**  	 - Invalid MsgFormat mode selection
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Setting the event format mode is not particularly hazardous, as the
**      result may be saving necessary bandwidth.  However, inappropriately
**      setting the event format mode could result in a loss of information
**      and missed behavior for the ground system
**  \sa
*/
#define CFE_EVS_SET_EVENT_FORMAT_MODE_CC 4

/** \cfeevscmd Enable Application Event Type
**
**  \par Description
**      This command enables the command specified event type for the command
**      specified application, allowing the application to send event messages
**      of the command specified event type through Event Service.  An Event
**      Type is defined to be a classification of an Event Message such as
**      debug, informational, critical, and error.
**      Note: In order for this command to take effect, applications must be
**      registered for Event Service.
**
**  \cfecmdmnemonic \EVS_ENAAPPEVTTYPE
**
**  \par Command Structure
**       #CFE_EVS_EnableAppEventTypeCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be enabled (or
**          unfiltered) for the specified application.
**        A zero in a bit position means the filtering state is unchanged for
**          the specified application.
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_ENAAPPEVTTYPE_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**    	 - BitMask field invalid - mask cannot be zero, and only bits 0-3 may be set
**       - Application name is not valid or not registered with event services
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Enabling an application event type is not particularly hazardous, as
**      the result may be turning on necessary event messages and
**      communication to the ground system.  However, inappropriately enabling
**      an application's event type could result in flooding of the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC,
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_ENABLE_APP_EVENT_TYPE_CC 5

/** \cfeevscmd Disable Application Event Type
**
**  \par Description
**      This command disables the command specified event type for the command
**      specified application, preventing the application from sending event
**      messages of the command specified event type through Event Service.
**      An Event Type is defined to be a classification of an Event Message such
**      as debug, informational, critical, and error.  Note: In order for this
**      command to take effect, applications must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_DISAPPEVTTYPE
**
**  \par Command Structure
**       #CFE_EVS_DisableAppEventTypeCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Debug
**            Bit 1 - Informational
**            Bit 2 - Error
**            Bit 3 - Critical
**        A one in a bit position means the event type will be disabled (or
**          filtered) for the specified application.
**        A zero in a bit position means the filtering state is unchanged for
**          the specified application.
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_DISAPPENTTYPE_EID debug event message
**       - The clearing of the Event Type Active Flag in The Event Type Active Flag in EVS App Data File
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**    	 - BitMask field invalid - mask cannot be zero, and only bits 0-3 may be set
**       - Application name is not valid or not registered with event services
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Disabling an application's event type is not particularly hazardous,
**      as the result may be shutting off unnecessary event messages and
**      possible event flooding of the system.  However, inappropriately
**      disabling an application's event type could result in a loss of critical
**      information and missed behavior for the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC,
**       #CFE_EVS_ENABLE_APP_EVENTS_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_DISABLE_APP_EVENT_TYPE_CC 6

/** \cfeevscmd Enable Event Services for an Application
**
**  \par Description
**      This command enables the command specified application to send events
**      through the Event Service.  Note: In order for this command to take
**      effect, applications must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_ENAAPPEVGEN
**
**  \par Command Structure
**       #CFE_EVS_EnableAppEventsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_ENAAPPEVT_EID debug event message
**       - The setting of the Active Flag in The Active Flag in EVS App Data File
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Enabling an application events is not particularly hazardous,
**      as the result may be turning on necessary event messages and
**      communication to the ground system.  However, inappropriately enabling
**      an application's events could result in flooding of the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC,
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_DISABLE_APP_EVENTS_CC
*/
#define CFE_EVS_ENABLE_APP_EVENTS_CC 7

/** \cfeevscmd Disable Event Services for an Application
**
**  \par Description
**      This command disables the command specified application from sending
**      events through Event Service.  Note: In order for this command to take
**      effect, applications must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_DISAPPEVGEN
**
**  \par Command Structure
**       #CFE_EVS_DisableAppEventsCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_DISAPPEVT_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Disabling an application's events is not particularly hazardous, as the
**      result may be shutting off unnecessary event messages and possible event
**      flooding of the system.  However, inappropriately disabling an
**      application's events could result in a loss of critical information and
**      missed behavior for the ground system.
**
**  \sa #CFE_EVS_ENABLE_EVENT_TYPE_CC, #CFE_EVS_DISABLE_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENT_TYPE_CC,
**      #CFE_EVS_DISABLE_APP_EVENT_TYPE_CC, #CFE_EVS_ENABLE_APP_EVENTS_CC
*/
#define CFE_EVS_DISABLE_APP_EVENTS_CC 8

/** \cfeevscmd Reset Application Event Counters
**
**  \par Description
**      This command sets the command specified application's event counter to zero.
**      Note: In order for this command to take effect, applications must be registered
**      for Event Service.
**
**  \cfecmdmnemonic \EVS_RSTAPPCTRS
**
**  \par Command Structure
**       #CFE_EVS_ResetAppCounterCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_RSTEVTCNT_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      This command is not inherently dangerous.  However, it is possible for
**      ground systems and on-board safing procedures to be designed such that
**      they react to changes in the counter value that is reset by this command.
**
**  \sa #CFE_EVS_RESET_COUNTERS_CC
*/
#define CFE_EVS_RESET_APP_COUNTER_CC 9

/** \cfeevscmd Set Application Event Filter
**
**  \par Description
**      This command sets the command specified application's event filter mask
**      to the command specified value for the command specified event.  Note:
**      In order for this command to take effect, applications must be
**      registered for Event Service.
**
**  \cfecmdmnemonic \EVS_SETBINFLTRMASK
**
**  \par Command Structure
**       #CFE_EVS_SetFilterCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_SETFILTERMSK_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**       - Specified event ID is not found in the application event filter
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**      Setting an application event filter mask is not particularly hazardous,
**      as the result may be shutting off unnecessary event messages and possible
**      event flooding of the system.  However, inappropriately setting an
**      application's event filter mask could result in a loss of critical
**      information and missed behavior for the ground system or flooding of the
**      ground system.
**
**  \sa #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC,
*#CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_SET_FILTER_CC 10

/** \cfeevscmd Enable Event Services Output Ports
**
**  \par Description
**      This command enables the command specified port to output event messages
**
**  \cfecmdmnemonic \EVS_ENAPORT
**
**  \par Command Structure
**       #CFE_EVS_EnablePortsCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Port 1
**            Bit 1 - Port 2
**            Bit 2 - Port 3
**            Bit 3 - Port 4
**        A one in a bit position means the port will be enabled.
**        A zero in a bit position means the port state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_ENAPORT_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**    	 - BitMask field invalid - mask cannot be zero, and only bits 0-3 may be set
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**          None.
**
**  \sa #CFE_EVS_DISABLE_PORTS_CC
*/
#define CFE_EVS_ENABLE_PORTS_CC 11

/** \cfeevscmd Disable Event Services Output Ports
**
**  \par Description
**      This command disables the specified port from outputting event messages.
**
**  \cfecmdmnemonic \EVS_DISPORT
**
**  \par Command Structure
**       #CFE_EVS_DisablePortsCmd_t
**        The following bit positions apply to structure member named 'BitMask'.
**            Bit 0 - Port 1
**            Bit 1 - Port 2
**            Bit 2 - Port 3
**            Bit 3 - Port 4
**        A one in a bit position means the port will be disabled.
**        A zero in a bit position means the port state is unchanged.
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_DISPORT_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**    	 - BitMask field invalid - mask cannot be zero, and only bits 0-3 may be set
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_ENABLE_PORTS_CC
*/
#define CFE_EVS_DISABLE_PORTS_CC 12

/** \cfeevscmd Reset an Event Filter for an Application
**
**  \par Description
**      This command resets the command specified application's event filter for
**      the command specified event ID.  Note: In order for this command to take
**      effect, applications must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_RSTBINFLTRCTR
**
**  \par Command Structure
**       #CFE_EVS_ResetFilterCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_RSTFILTER_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**       - Specified event ID is not found in the application event filter
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC,
*#CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_RESET_FILTER_CC 13

/** \cfeevscmd Reset All Event Filters for an Application
**
**  \par Description
**      This command resets all of the command specified applications event
**      filters.  Note: In order for this command to take effect, applications
**      must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_RSTALLFLTRS
**
**  \par Command Structure
**       #CFE_EVS_ResetAllFiltersCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_RSTALLFILTER_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_ADD_EVENT_FILTER_CC, #CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_RESET_ALL_FILTERS_CC 14

/** \cfeevscmd Add Application Event Filter
**
**  \par Description
**      This command adds the given filter for the given application identifier and event identifier.
**      Note: In order for this command to take effect, applications
**      must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_ADDEVTFLTR
**
**  \par Command Structure
**       #CFE_EVS_AddEventFilterCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_ADDFILTER_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**       - Specified event ID is already added to the application event filter
**       - Maximum number of event IDs already added to filter
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_DELETE_EVENT_FILTER_CC
*/
#define CFE_EVS_ADD_EVENT_FILTER_CC 15

/** \cfeevscmd Delete Application Event Filter
**
**  \par Description
**      This command removes the given filter for the given application identifier and event identifier.
**      Note: In order for this command to take effect, applications
**      must be registered for Event Service.
**
**  \cfecmdmnemonic \EVS_DELEVTFLTR
**
**  \par Command Structure
**       #CFE_EVS_DeleteEventFilterCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_DELFILTER_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Application name is not valid or not registered with event services
**       - Specified event ID is not found in the application event filter
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       None.
**
**  \sa #CFE_EVS_SET_FILTER_CC, #CFE_EVS_RESET_FILTER_CC, #CFE_EVS_RESET_ALL_FILTERS_CC, #CFE_EVS_ADD_EVENT_FILTER_CC
*/
#define CFE_EVS_DELETE_EVENT_FILTER_CC 16

/** \cfeevscmd Write Event Services Application Information to File
**
**  \par Description
**       This command writes all application data to a file for all applications that
**           have registered with the EVS.  The application data includes the Application ID,
**           Active Flag, Event Count, Event Types Active Flag, and Filter Data.
**
**  \cfecmdmnemonic \EVS_WRITEAPPDATA2FILE
**
**  \par Command Structure
**       #CFE_EVS_WriteAppDataFileCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_WRDAT_EID debug event message
**       - The file specified in the command (or the default specified
**         by the #CFE_PLATFORM_EVS_DEFAULT_APP_DATA_FILE configuration parameter) will be
**         updated with the latest information.
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified FileName cannot be parsed
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Writing a file is not particularly hazardous, but if proper file management is not
**       taken, then the file system can fill up if this command is used repeatedly.
**
**  \sa #CFE_EVS_WRITE_LOG_DATA_FILE_CC, #CFE_EVS_SET_LOG_MODE_CC
*/
#define CFE_EVS_WRITE_APP_DATA_FILE_CC 17

/** \cfeevscmd Write Event Log to File
**
**  \par Description
**      This command requests the Event Service to generate a file containing
**      the contents of the local event log.
**
**  \cfecmdmnemonic \EVS_WRITELOG2FILE
**
**  \par Command Structure
**       #CFE_EVS_WriteLogDataFileCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_WRLOG_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - The specified FileName cannot be parsed
**       - An Error occurs while trying to write to the file
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Writing a file is not particularly hazardous, but if proper file management is not
**       taken, then the file system can fill up if this command is used repeatedly.
**
**  \sa #CFE_EVS_WRITE_APP_DATA_FILE_CC, #CFE_EVS_SET_LOG_MODE_CC, #CFE_EVS_CLEAR_LOG_CC
*/
#define CFE_EVS_WRITE_LOG_DATA_FILE_CC 18

/** \cfeevscmd Set Logging Mode
**
**  \par Description
**       This command sets the logging mode to the command specified value.
**
**  \cfecmdmnemonic \EVS_SETLOGMODE
**
**  \par Command Structure
**       #CFE_EVS_SetLogModeCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - The generation of #CFE_EVS_LOGMODE_EID debug event message
**
**  \par Error Conditions
**       This command may fail for the following reason(s):
**       - Invalid LogMode selected - must be either #CFE_EVS_LogMode_OVERWRITE
**         or #CFE_EVS_LogMode_DISCARD
**
**       Evidence of failure may be found in the following telemetry:
**       - \b \c \EVS_CMDEC - command error counter will increment
**       - An Error specific event message
**
**  \par Criticality
**       Setting the event logging mode is not particularly hazardous, as the
**       result may be saving valuable event data.  However, inappropriately
**       setting the log mode could result in a loss of critical information.
**      Note: the event log is a back-up log to the on-board recorder.
**
**  \sa #CFE_EVS_WRITE_LOG_DATA_FILE_CC, #CFE_EVS_CLEAR_LOG_CC
*/
#define CFE_EVS_SET_LOG_MODE_CC 19

/** \cfeevscmd Clear Event Log
**
**  \par Description
**       This command clears the contents of the local event log.
**
**  \cfecmdmnemonic \EVS_CLRLOG
**
**  \par Command Structure
**       #CFE_EVS_ClearLogCmd_t
**
**  \par Command Verification
**       Successful execution of this command may be verified with
**       the following telemetry:
**       - \b \c \EVS_CMDPC - command execution counter will
**       increment
**       - \b \c \EVS_LOGFULL - The LogFullFlag in the Housekeeping
**       telemetry will be cleared
**       - \b \c \EVS_LOGOVERFLOWC - The LogOverflowCounter in the
**       Housekeeping telemetry will be reset to 0
**
**  \par Error Conditions
**       There are no error conditions for this command. If the Event
**       Services receives the command, the log is cleared.
**
**  \par Criticality
**       Clearing the local event log is not particularly hazardous, as the
**       result may be making available space to record valuable event data.
**       However, inappropriately clearing the local event log could result
**       in a loss of critical information.  Note: the event log is a back-up
**       log to the on-board recorder.
**
**  \sa #CFE_EVS_WRITE_LOG_DATA_FILE_CC, #CFE_EVS_SET_LOG_MODE_CC
*/
#define CFE_EVS_CLEAR_LOG_CC 20
/** \} */

#endif
