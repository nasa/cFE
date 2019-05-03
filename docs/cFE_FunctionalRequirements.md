---
title: Core Flight Software Requirements
tags: cFE
abstract: Software Requirements for the Core Flight System core applications
---

# Executive Services Requirements (cES) {#cES}

## Executive Services Subgroup: Operational Interface {#cES10}

### cES1000

Upon receipt of a Command, the cFE shall generate a Software Bus message that 
includes the following items:

- Number of Registered Applications
- Number of Registered Child Tasks
- Number of Registered Shared Libraries
- Reset Type
- Reset Subtype
- Number of entries in System Log
- Size of the System Log
- Number of bytes used in the System Log
- Current Exception and Reset Log Index
- Number of Processor Resets
- Maximum Number of Processor Resets before a Power
- On Reset
- Boot Source
- ES Valid Command Counter
- ES Invalid Command Counter

*Rationale: It is common for sub-systems to report housekeeping status upon receipt 
of a housekeeping request command.*

### cES1001

Upon receipt of a Command, the cFE shall generate a NO-OP event message.

*Rationale: This command is useful as a general sub-system aliveness test.*

### cES1002

Upon receipt of a valid Command, the cFE shall increment a valid Command 
counter.

*Rationale: The ground needs the capability to verify that the command was received 
and accepted by the cFE.  Details of valid commands are documented in the cFE 
Application Developer's Guide and the cFE User's Guide.*

### cES1003

Upon receipt of an invalid Command, the cFE shall increment the invalid Command 
counter and generate an event message.

*Rationale: The ground needs an indicator if a command is rejected by the cFE.  
Details of what makes a command invalid are documented in the cFE Application 
Developer's Guide and the cFE User's Guide.*

### cES1004

Upon receipt of a Command, the cFE shall set to zero the valid Command counter 
and invalid Command counter.

*Rationale: This command is a common feature in heritage sub-system software design.  
In general, command counter (valid and invalid) are reset.*

### cES1005

Upon receipt of a Command, the cFE shall create the Command specified 
Application by defining the Application in the System Resources Definition 
using information from the Command specified file, and beginning execution of 
the Application.

*Rationale: A basic feature of the cFE is to be able to dynamically (while the cFE 
is running) start applications. This requirement allows for an application to be 
created and started from one of the cFE file systems.*

#### cES1005.1

The Command shall include the following parameters:

- Application Path/Filename 
- Application Entry Point 
- Application Name 
- Application Priority 
- Application Stack Size 
- Application Load Address 
- Exception Action (restart application or perform processor reset)

#### cES1005.2

The Command specified cFE Application file shall be in any valid cFE file 
system including the volatile file system and the non-volatile file system.

*Rationale: The command itself does not care about where the cFE Application 
comes from, it is specified In the path.*

#### cES1005.3

If the Command specified Application is undefined then the cFE shall reject 
the Command, increment the invalid command counter and generate an event 
message.

*Rationale: Can't start an undefined application.*

#### cES1005.4

If the Command specified Application is already defined and executing, then the 
cFE shall reject the Command, increment the invalid Command counter and 
generate an event message.

*Rationale: Can't start an application that is already running.*

### cES1006

Upon receipt of a Command, the cFE shall delete the Command specified 
Application including all child tasks.

*Rationale: Need to be able to stop the execution of an Application 
and remove its System Resources.  The delete will clean-up the application's 
main task and all of its child tasks.  Note: other cFE components are 
required to have cleanup routines that ES calls.*

#### cES1006.1

If the specified Application is undefined then the cFE shall reject the 
Command, increment the invalid command counter and generate an event message.

*Rationale: Can't delete an undefined application*

### cES1007

Upon receipt of a Command, the cFE shall Restart the Command specified 
Application

*Rationale: Need to be able to restart an Application. A restart involves 
deleting it (cleaning up) and then starting it again. This is similar to 
starting the cFE Application from a file system. When an Application is restarted, 
the only command parameter required is the application name. All other parameters 
including the filename are the same as the original cFE Application Create command. 
The restart is intended for error recovery such as an exception, and should not be 
used to start a new version of an Application. If a Critical Data Store Area is 
allocated for the Application, it is preserved, and the Application may re-connect 
to the Critical Data Store Area when it is running again.*

#### cES1007.1

If the Command specified Application is undefined then the cFE shall reject the 
Command, increment the invalid Command counter and generate an event message.

*Rationale: Can't restart an undefined application.*

#### cES1007.2

If the original cFE Application file is not found then the cFE shall reject the 
Command, increment the invalid Command counter, and generate an event message.

*Rationale: Can't restart the Application if the original file has been removed. In 
this case, the Application will continue without a restart.*

#### cES1007.3

If the cFE Application Restart fails due to a non-parameter error, then the cFE 
shall delete the Application, increment the invalid Command counter, and 
generate an event message.

*Rationale: If the Application is already stopped and there is an error restarting it, 
then the cFE application will be deleted.*

### cES1008

Upon receipt of a Command the cFE shall Reload the Command specified cFE 
Application from the Command specified cFE Application file.

*Rationale: This command enables the ground to replace an Application with only one 
command. This is required for applications such as a Command Uplink Application, which 
must be replaced with one command. The specified cFE Application file may be from any 
valid cFE.*

#### cES1008.1

If the specified Application is undefined then the cFE shall reject the 
Command, increment the invalid Command counter and generate an event message.

*Rationale: Can't reload an undefined Application.*

#### cES1008.2

If the specified cFE Application file does not exist then the cFE shall reject 
the command, increment the invalid Command counter, and generate an event 
message.

*Rationale: Can't reload the Application if the new file does not exist.*

#### cES1008.3

If the cFE Application Reload fails due to a non-parameter error, then the cFE 
shall delete the Application, increment the invalid Command counter, and 
generate an event message.

*Rationale: If the Application is already stopped and there is an error restarting 
it, then the cFE application will be deleted. Should the old Application be 
restarted?  Need to be able to reset the cFE in the event that there is a critical 
problem. As a result of the Power On Reset, all code and data are re-initialized 
and the cFE is returned to a default power-on state. This reset is initiated through 
a common interface.*

### cES1009

Upon receipt of a Command, the cFE shall perform a Power On Reset of the Core 
Flight Executive.

*Rationale: On a flight processor or other embedded processor this command results in 
rebooting the processor board through the Boot Software. On a desktop system this command 
will result in the restarting of the cFE, but not the operating system. Note: If the cFE 
implementation includes more than one cFE core images, it is the responsibility of the 
Boot Software to select which cFE image is booted.*

### cES1010

Upon receipt of a Command, the cFE shall perform a Processor Reset of the Core 
Flight Executive.

*Rationale: Need to be able to restart the cFE in the event that there is a problem with the 
Real Time OS or cFE Core software. Note that restarting the cFE will result in a restart of 
all of the cFE applications. In addition, a restart of the cFE will initialize the*

### cES1011

Upon receipt of a Command, the cFE shall generate a message that contains a 
summary of the Command specified Application's properties and state as defined 
in the Systems Resources Definition including: 

- cFE Application Name 
- cFE Application Filename 
- cFE Application Entry Point 
- cFE Application Priority 
- cFE Application Stack Size 
- cFE Application Memory Start Address 
- cFE Application Memory Size 
- cFE Application Child Task Count

*Rationale: In order to support remote Application management then the Application's properties 
and current state need to be externally observable.  Because the ground interface is with 
Applications rather than Tasks, Task information should be included as well.*

### cES1012

Upon receipt of a Command, the cFE shall generate a Command specified file that 
contains all properties and states of all cFE Applications that are defined in 
the Systems Resources Definition.

*Rationale: May want information about all applications and tasks defined in the Systems 
Resources Definition in order to diagnose a problem.*

#### cES1012.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to specify a default if the user does not want to specify a new filename.*

### cES1013

Upon receipt of a Command, the cFE shall submit to the operating system shell, 
the string supplied as a parameter.

*Rationale: Having access to the operating system shell has proved invaluable during FSW 
development on missions such as JWST and Triana.  Although it is available on-orbit, it 
is not intended to be used during normal operations.*

#### cES1013.1

Upon execution of the operating system Command, the cFE shall generate one or 
more messages containing the ascii output generated by the operating system 
command.

*Rationale: This message will provide the "output" from the shell command to allow the 
implementation of a shell like interface to the cFE.*

### cES1014

The cFE shall maintain an Executive Services System Log which contains a series 
of ASCII text strings describing significant events or errors.

*Rationale: Examples of ES System Log information includes: "Created new cFE 
Application:  StoredCommand.app" or "Could Not Create OS Queue" or "File not found 
error: /eebank1/StoredCommand.app" This requirement states that the cFE needs to 
maintain this information.  There is a separate requirement for the creation of a file 
to transfer the information to the ground. Note that the information can also be 
obtained with a raw memory read.*

#### cES1014.1

Each entry in the Executive Services System Log shall be time tagged with the 
time that the event happened.

*Rationale: Need to be able to determine when the event occurred.*

#### cES1014.2

The cFE shall calculate the number of bytes used and number of entries in 
Executive Services System Log.

*Rationale: Ground operations need indication of how full the System Log is so that 
they can clear it, if necessary, in order to make room for new entries (or at least 
write it to a file to preserve it).*

##### cES1014.2.1

If the Executive Services System Log is full and the System Log Mode is set to 
OVERWRITE then the cFE shall write all new entries from the top of the log.

*Rationale: Want to provide the capability to continuously record all new System Messages*

##### cES1014.2.2

If the Executive Services Syste Log is full and the System Log Mode is set to 
DISCARD then the cFE shall discard all new entries.

*Rationale: Want to provide capability to stop writing to the System Log in order to preserve 
to Log which may contain important anomolous messages*

### cES1015

Upon receipt of a Command, the cFE shall clear the Executive Services System 
Log.

*Rationale: Want to be able to clear the Executive Services System Log Buffer so that only the 
new information is saved*

### cES1016

Upon receipt of a Command, the cFE shall copy the information contained in the 
Executive Services System Log into a Command specified file.

*Rationale: We did not want to count on a file system for storing the ES System errors in the 
event that there was a problem with the file system or the file system was not mounted yet.  
We did want to provide an easy ground interface for getting the data to the ground*

#### cES1016.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event a user does not want to have to specify one.*

### cES1017

The cFE shall maintain an Executive Services Exception and Reset Log which will 
log critical system data for exceptions and resets including: 

- A time stamp 
- Processor Context information 
- Critical system variables 
- ASCII string stating the reason for the reset

*Rationale: Want to be able to save state information prior to a restart 
(processor, power-on, application, task etc) to help with diagnosing problems.  
There is a separate requirement for the creation of a file to transfer the 
information to the ground.*

### cES1018

Upon receipt of a Command, the cFE shall clear the Executive Services Exception 
and Reset Log.

*Rationale: Want to be able to clear the Executive Services Exception and Reset 
Log so that only the new information is saved.*

### cES1019

Upon receipt of a Command, the cFE shall copy the information contained in the 
Executive Services Exception and Reset Log Buffer into a Command specified file.

*Rationale: The cFE Exception and Reset Log contains a large amount of data, and 
is variable in size. It is easier to dump the contents to a file.*

#### cES1019.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event a user does not want to have 
to specify one.*

### cES1021

The cFE shall maintain an Executive Services Logic Analyzer Capture Log for 
capturing application specified timestamps and events for off-line performance 
analysis.

*Rationale: The Logic Analyzer Capture Log is used along with a performance log 
API to allow the cFE and cFE Applications to save performance data that can be 
downloaded.*

### cES1022

Upon receipt of a Request, the cFE shall record the specified Logic Analyzer 
Capture Tag in the Logic Analyzer Capture Log.

*Rationale: The cFE Core and cFE Applications make specific calls to create 
log entries in the Logic Analyzer Capture Log.*

#### cES1022.1

The cFE shall store a timestamp along with the specified Logic Analyzer Capture 
Tag.

*Rationale: Each entry is time-stamped.*

#### cES1022.2

If the Logic Analyzer Capture Log is full, then the cFE shall write all new 
entries from the top of the log.

### cES1023

Upon receipt of a Command, the cFE shall copy the information contained in the 
Logic Analyzer Capture Log into a Command Specified file.

*Rationale: Want to be able to catpure the log to a file for post processing.*

#### cES1023.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to have a default filename.*

### cES1024

Upon receipt of a Command, the cFE shall set the Processor Resets counter to 
zero.

*Rationale: Ground may want to clear this counter so that infrequent Processor 
Resets don't result in a Power-on Reset*

### cES1025

Upon receipt of a Command, the cFE shall set the Maximum Processor Resets 
counter to the Command Specified value.

*Rationale: Based on ST-5 experience, want to be able to increase the 
maximum number of Processor Resets in order to prevent a Power-on Reset.  
ST-5 used these features when they were having the multiple bit errors in 
their recorder memory. They set both numbers to 4*

### cES1026

Upon receipt of a Command, the cFE shall copy the following Critical Data Store 
information into the Command Specified file:

1. Critical Data Store Name
2. Size
3. Data Integrity Value

*Rationale: This provides a registry of the Critical Data Store.*

### cES1027

Upon receipt of a Command, the cFE shall delete the Command Specified Critical 
Data Store.

*Rationale: As part of an Application clean-up, want to clean-up the allocated 
resources.*

### cES1028

Upon receipt of Command, the cFE shall set the System Log Mode to the 
Command-specified mode, either overwrite or discard.

*Rationale: While in Overwrite Mode the oldest logged System  message 
will be overwritten by the new System message when the System Log Full 
Flag is set to true.  While in Discard Mode the new message will be 
discarded, preserving the contents of the full log.*

## Executive Services Subgroup: Application Interface {#cES13}

### cES1300

Upon receipt of a Request, the cFE shall register the calling cFE Application 
with the system.

*Rationale: cFE Applications must register with the cFE in order to allow the 
cFE to track the Application's resources. This function also allows the system 
to synchronize the application startup. The cFE Application will wait in this 
function until the cFE starts up*

### cES1301

Upon receipt of a Request, the cFE shall provide the type of last reset 
performed by the processor.

*Rationale: cFE Applications may perform processing that is specific to each 
reset type.*

#### cES1301.1

The reset types include: Power On Reset Processor Reset.

### cES1302

Upon receipt of a Request, the cFE shall provide the Processor ID on which the 
Request was made.

*Rationale: Need to determine the Processor ID. This feature is useful in 
missions with multiple processors, or to help distinguish prototype vs. 
flight processor features.*

### cES1303

Upon receipt of a Request, the cFE shall provide the Spacecraft ID on which the 
Request was made.

*Rationale: Need to determine the Spacecraft ID. This feature is useful in 
missions with multiple Spacecraft, or Spacecraft with multiple processors.*

### cES1304

Upon receipt of a Request, the cFE shall provide the cFE Application ID of the 
calling cFE Application.

*Rationale: A cFE Application needs to determine its own Application ID.*

### cES1305

Upon receipt of a Request, the cFE shall provide the cFE Task Name and cFE 
Application Name which corresponds to the specified cFE Task ID.

*Rationale: The command will provide a way to find the cFE Task Name and 
the parent cFE Application name from any cFE Application or Child task.*

### cES1306

Upon receipt of a Request, the cFE shall provide the cFE Application ID which 
corresponds to the specified cFE Application Name.

*Rationale: The Executive Services will assign an Application ID. The Application 
Name is specified when the cFE Application is created.  This Request will provide 
a way to determine the cFE Application ID when the pre-determined Application Name 
is passed in.*

### cES1307

Upon receipt of a Request, the cFE shall provide the cFE Application Name which 
corresponds to the specified cFE Application ID.

*Rationale: The Executive Services will assign an Application ID. This Request 
will provide a way to determine an Application's name from its Application ID.*

### cES1309

Upon receipt of a Request, the cFE shall delete the specified Application 
including all child tasks.

*Rationale: Need to be able to stop the execution of an Application and remove 
its System Resources.  The delete will clean-up the application's main task and 
all of its child tasks.  Note: other cFE components are required to have cleanup 
routines that ES calls.*

#### cES1309.1

If the specified Application is undefined then the cFE shall record the error 
in the System Log, and return an error code.

*Rationale: Can't delete an undefined application.*

### cES1310

Upon receipt of a Request the cFE shall Restart the specified Application.

*Rationale: Need to be able to restart an Application. A restart involves deleting it 
(cleaning up) and then starting it again. This is similar to starting the cFE 
Application from a file system. When an Application is restarted, the only parameter 
required is the application name. All other parameters including the filename are the 
same as the original cFE Application Create Request. The restart is intended for error 
recovery such as an exception, and should not be used to start a new version of an 
Application. If a Critical Data Store Area is allocated for the Application, it is 
preserved, and the Application may re-connect to the Critical Data Store Area when it 
is running again.*

#### cES1310.1

If the specified Application is undefined then the cFE shall record the error 
in the System Log, and return an error code.

*Rationale: Can't restart an undefined application.*

#### cES1310.2

If the original cFE Application file is not found then the cFE shall record the 
error in the System Log, and return an error code.

*Rationale: Can't restart the Application if the original file has been removed. 
In this case, the Application will continue without a restart.*

#### cES1310.3

If the cFE Application Restart fails due to a non-parameter error, then the cFE 
shall record the error in the System Log, and return an error code.

*Rationale: If the Application is already stopped and there is an error restarting 
it, then the cFE application will be deleted.*

### cES1311

Upon receipt of a Request, the cFE shall create the specified cFE Child Task 
within the cFE Application that owns the task and begin execution of the task.

*Rationale: Each cFE Application's main task has the capability to create and 
start one or more child tasks.*

#### cES1311.1

In the event that the cFE Child Task cannot be created, the cFE shall record 
the error in the System Log, and return an error code.

*Rationale: Need to keep track of the Child Task Create Failures.*

#### cES1311.2

In the event a child task attempts to create another child task, the cFE shall 
record the error in the System Log, and return an error code.

*Rationale: Only the cFE Application's main task can create a child task. This 
prevents confusion with parent/child task relationships and the allocation/deallocation 
of resources.*

### cES1312

Upon receipt of a Request, the cFE shall delete the specified cFE Child Task 
within the cFE Application that owns the task.

*Rationale: As part of a cFE Application cleanup, the Application needs to be 
able to delete each child task.*

#### cES1312.1

If the specified task is the cFE Application Main Task, the request shall 
record the error in the System Log, and return an error code.

*Rationale: Cannot use Child Task Exit or Delete on the cFE Application Main Task.*

### cES1313

Upon receipt of a Request, the cFE shall register the calling cFE Child Task 
with the system.

*Rationale: Just like cFE Applications, Child Tasks must be registered to work 
properly in the cFE. This function call is intended to be called by the newly 
created cFE Child Task.*

### cES1314

Upon receipt of a Request, the cFE shall end execution of the calling cFE Child 
Task.

*Rationale: A cFE Child Task needs to be able to exit and end execution.*

#### cES1314.1

If the calling task is the cFE Application Main Task, the cFE shall record the 
error in the System Log, and return an error code.

*Rationale: Cannot use Child Task Exit or Delete on the cFE Application Main Task.*

### cES1315

Upon receipt of a Request, the cFE shall reserve the Request specified amount 
of memory in the Critical Data Store for the cFE Application using the Request 
specified name.

*Rationale: The Critical Data Store will be used by Applications to store critical 
parameters that will be preserved after an application or processor restart. ES 
will allocate the memory for the application.  A pointer or memory address will be 
provided to the alloc.*

#### cES1315.1

If a Critical Data Store exists for the Request specified name but has a 
different size than what is specified in the Request, the cFE shall remove the 
existing Critical Data Store and create a new one using the Request specified 
name and size.

*Rationale: Assumes that if the sizes don't match then something has changed and 
the CDS can't be trusted.*

#### cES1315.2

If a Critical Data Store exists for the Request specified name but the Data 
Integrity value is invalid, the cFE shall remove the existing Critical Data 
Store and create a new one using the Request specified name and size.

*Rationale: Assumes that if the CRC is invalid then the CDS can't be trusted.*

### cES1316

Upon receipt of a Request, the cFE shall copy the contents from the Request 
specified Critical Data Store to the Request specified address.

*Rationale: Provides the capability to restore the local data with the contents 
of the critical data store.*

#### cES1316.1

If the Data Integrity Value is invalid then the data shall not be copied from 
the Critical Data Store.

*Rationale: Critical Data Store should be verified before restoring.  Assumes 
that if the calculated CRC does not match the stored CRC than the CDS cannot 
be trusted.*

#### cES1316.2

If the Request Specified Critical Data Store does not exist then the data shall 
not be copied.

*Rationale: Can't copy data if the reference is wrong.*

### cES1317

Upon receipt of a Request, the cFE shall perform a Power On Reset of the Core 
Flight Executive.

*Rationale: Applications need to periodically copy the local data into the CDS 
so that it can be preserved.  Note that the CDS is not required to exist on-card 
(local address space).  This provides the capability for a mission to use off-card 
bulk storage.*

### cES1318

Upon receipt of a Request, the cFE shall perform a Processor Reset of the Core 
Flight Executive.

*Rationale: Everytime data is written to the CDS, a CRC must be recalculated in 
order to have a reference for any CDS validation.  Note that Applications are 
responsible for determining whether the contents of a CDS Block are still 
logically valid.*

### cES1319

Upon receipt of a Request, the cFE shall exit the calling cFE Application and 
delete the Applications' resources.

*Rationale: Can't copy data if the reference is wrong.*

### cES1320

Upon receipt of a Request, the cFE shall prepare a Memory Pool for runtime 
memory allocation/de-allocation.

*Rationale: The Memory Allocation interface allows the cFE Application to 
supply a Pool of memory, which can be used for efficient memory allocation 
and de-allocation.*

#### cES1320.1

If the specified size is less than the <PLATFORM_DEFINED> minimum block size,
the cFE shall record the error in the System Log, and return an error code.

*Rationale: If the size of the Pool is not valid, then an error must be returned.*

### cES1321

Upon receipt of a Request the cFE shall allocate a block of memory of the 
specified size from the specified Memory Pool.

*Rationale: The Memory Allocation interface allows the cFE Application to 
allocate a block of memory from a previously created memory pool.*

#### cES1321.2

If the specified Memory Pool identifier is invalid then the cFE shall record 
the error in the System Log, and return an error code.

*Rationale: If the handle or Memory Pool ID is not valid, then an error must 
be returned.*

#### cES1321.3

If the specified size is too large for the specified Memory Pool, the cFE shall 
record the error in the System Log, and return an error code.

*Rationale: Cannot allocate a memory block bigger than the pool.*

### cES1322

Upon receipt of a Request the cFE shall de-allocate the specified block of 
memory from the specified Memory Pool.

*Rationale: The Memory Allocation interface allows the cFE Application to 
de-allocate a block of memory from a previously created memory pool.*

#### cES1322.1

If the specified Memory Pool identifier is invalid, then the cFE shall record 
the error in the System Log, and return an error code.

*Rationale: Need to have a valid Memory Pool identifier in order to de-allocate 
a block of memory.*

### cES1323

Upon receipt of a Request, the cFE shall calculate a Data Integrity value over 
the given range of memory using the specified algorithm. The algorithm shall be 
one of the following:

- XOR: Exclusive OR
- CRC16: 16 Bit Cyclic Redundancy Check
- CRC32: 32 Bit Cyclic Redundancy Check

*Rationale: We need to provide a checksum/CRC utility.*

### cES1324

Upon receipt of a Request, the cFE shall load and initialize a hardware device 
driver and connect it with the specified hardware handshaking and device 
processing code.

*Rationale: Need a standard device driver interface.*

### cES1325

Upon receipt of a Request, the cFE shall unload a specified hardware device 
driver and de-allocate all previously allocated resources used by the driver.

*Rationale: Need a way of unloading, stopping a hardware device driver.*

### cES1326

Upon receipt of a Request, the cFE shall disable a specified hardware device 
driver.

*Rationale: Need a way of temporarily disabling the device driver and it's 
associated interrupt.*

#### cES1326.1

If the specified hardware device driver is not loaded, then the cFE shall 
record the error in the System Log, and return an error code.

*Rationale: Cannot disable a device driver that is not loaded.*

### cES1327

Upon receipt of a Request, the cFE shall re-enable a specified hardware device 
driver.

*Rationale: Need a way to re-enable the device driver and it's associated interrupt.*

#### cES1327.1

If the specified hardware device driver is not loaded, then the cFE shall 
record the error in the System Log, and return an error code.

*Rationale: Cannot disable a device driver that is not loaded.*

### cES1328

Upon receipt of a Request, the cFE shall copy the data starting at the Request 
specified address to the Request specified Critical Data Store.

#### cES1328.1

The cFE shall calculate a Data Integrity Value for the Request specified 
Critical Data Store and store it.

#### cES1328.2

If the Request Specified Critical Data Store does not exist then the data shall 
not be copied.

## Executive Services Subgroup: Hardware Interfaces {#cES15}

### cES1500

Upon a Power-on Reset, the cFE shall identify the `<PLATFORM_DEFINED>` Power On 
reset sub-type.

*Rationale: Each mission may want to further distinguish between Processor reset 
types in order to tailor their system's behavior.  For example a mission may want 
to take different behavior for a watchdog time out and the execution of the 
processor's reset instructions.*

### cES1501

Upon a Power-On Reset, the cFE shall clear the Executive Services System Log.

*Rationale: Want to be able to determine what errors are logged by the cFE. 
This log is not preserved on a Power-On reset.*

### cES1502

Upon a Power-On Reset, the cFE shall clear the Executive Services Exception and 
Reset Log.

*Rationale: Want to be able to get a snapshot of some critical parameters prior 
to a reset as well as log the resets that have occurred.*

### cES1503

Upon a Power-On Reset, the cFE shall clear the Volatile File system.

*Rationale: The Volatile File system is initialized during a Power-on Reset.*

### cES1504

Upon a Power-On Reset, the cFE shall clear the contents of the Critical Data 
Store.

*Rationale: The purpose of the critical data store is to save data that an 
Application wants to preserve across a processor restart. This area will be 
cleared during a Power-On Reset.*

### cES1505

Upon a Power-on Reset, the cFE shall create all operating system objects 
required by the cFE.

*Rationale: This is a table driven startup that includes: Core cFE Applications, 
semaphores, queues, and shared memory segments. It can also contain calls to 
initialize device drivers and interrupts.*

### cES1506

Upon a Power-on Reset, the cFE shall mount the non-volatile file system.

*Rationale: Non-volatile file system contains the files for each of the 
cFE Applications. On a Power-On reset, the external cFE Applications are 
loaded from the Volatile file system.*

### cES1507

Upon a Power-on Reset, the cFE shall create, format and mount the volatile file 
system.

*Rationale: The volatile file system is a key part of the cFE. It is used for logs, 
data files and new cFE Applications.*

### cES1508

Upon a Power-on Reset, the cFE shall process all entries in the cFE Startup 
File located in the non-volatile file system.

*Rationale: There is a file that contains all of the Applications, Shared 
Libraries, and Device Drivers that are to be created and started.*

#### cES1508.1

The cFE shall create and start cFE Applications according to the entry in the 
cFE Startup File.

*Rationale: The cFE supports loading and creating new cFE Applications during 
system startup.*

#### cES1508.2

The cFE shall create and initialize cFE Shared Libraries according to the entry 
in the cFE Startup File.

*Rationale: The cFE supports creating and initializing cFE Shared Libraries 
during system startup.*

#### cES1508.3

The cFE shall create and initialize cFE Device Drivers according to the entry 
in the cFE Startup File.

*Rationale: The cFE supports loading and initializing cFE Device Drivers during 
system startup.*

### cES1509

Upon a Power On Reset, the cFE shall make an entry in the Executive Services 
Exception and Reset Log, recording the Power On Reset.

*Rationale: One purpose of the Executive Services Exception and Reset Log is 
to log all resets.*

### cES1510

Upon a Processor Reset, the cFE shall identify the `<PLATFORM_DEFINED>` 
Processor reset sub-type.

*Rationale: Each mission may want to further distinguish between Processor reset 
types in order to tailor their system's behavior.  For example a mission may want 
to take different behavior for a watchdog time out and the execution of the 
processor's reset instructions.*

### cES1511

Upon a Processor Reset, the cFE shall preserve the Executive Services System 
Log.

*Rationale: ES needs to retain this information to support diagnosing cause of 
processor reset as well as preserving information that is required to  count 
the number of resets.*

### cES1512

Upon a Processor Reset, the cFE shall preserve the Executive Services Exception 
and Reset Log.

*Rationale: Want to be able to determine what errors are logged by the cFE. This 
log is not preserved on a Power-On reset.*

### cES1513

Upon a Processor Reset, the cFE shall preserve the Volatile File system.

*Rationale: Want to be able to get a snapshot of some critical parameters 
prior to a reset as well as log the resets that have occurred.*

### cES1514

Upon a Processor Reset, the cFE shall preserve the contents of the Critical 
Data Store.

*Rationale: During a processor reset, the Volatile File System and it's contents 
will be preserved.*

### cES1515

Upon a Processor Reset, the cFE shall create all operating system objects 
required by the cFE.

*Rationale: The purpose of the critical data store is to save data that an 
Application wants to preserve across a processor restart.*

#### cES1515.1

If the creation of the operating system object fails, the cFE shall perform a 
power on reset.

*Rationale: Items such as tasks, semaphores, queues, and shared memory segments 
would be initialized.*

### cES1516

Upon a Processor Reset, the cFE shall mount the non-volatile file system.

*Rationale: If the processor reset fails, then the software must default 
to a safer Power-on Reset.*

### cES1517

Upon a Processor Reset, the cFE shall check and mount the volatile file system.

*Rationale: Non-volatile file system contains the files for each of the cFE 
Applications. On a Power-On reset, the external cFE Applications can be 
loaded from the Volatile file system.*

#### cES1517.1

If the volatile file system check fails, the cFE shall format the volatile file 
system and create a system log entry.

*Rationale: Want to preserve the Volatile file system across a processor reset.*

### cES1518

Upon a Processor Reset, the cFE shall process all entries in the cFE Startup 
File located in the volatile file system.

*Rationale: If the volatile file system is corrupt, it must be reformatted 
to allow the cFE to function.*

#### cES1518.1

The cFE shall create and start cFE Applications according to the entry in the 
cFE Startup File.

*Rationale: There is a file that contains all of the cFE Applications and 
Shared Libraries that are to be loaded and started. The cFE will check to see 
if this file is in the volatile file system. This file facilitates restarting 
a processor using patches that were made.*

#### cES1518.2

The cFE shall create and initialize Shared Libraries according to the entry in 
the cFE Startup File.

*Rationale: The cFE supports loading and creating new cFE Applications during 
system startup.*

### cES1519

If the system startup file is not present in the volatile file system then the 
cFE shall process all entries in the cFE Startup File located in the 
non-volatile file system.

*Rationale: The cFE supports loading and initializing cFE Shared Libraries during 
system startup.*

#### cES1519.1

The cFE shall create and start cFE Applications according to the entry in the 
cFE Startup File.

*Rationale: Need to have a default system startup file which is used if there 
is no system startup file in volatile memory.*

#### cES1519.2

The cFE shall create and initialize Shared Libraries according to the entry in 
the cFE Startup File.

*Rationale: The cFE supports loading and creating new cFE Applications during 
system startup.*

### cES1520

Upon a Processor Reset, the cFE shall make an entry in the Executive Services 
Exception and Reset Log recording the Processor Reset.

*Rationale: The cFE supports loading and initializing cFE Shared Libraries 
during system startup.*

### cES1521

Upon a Processor Reset, the cFE shall preserve the following: 

- Boot Source 
- Reset Type 
- Reset Subtype 
- Reset Reason 
- Number of Processor Resets 
- Maximum Processor Resets 
- Number of entries in System Log 
- Size of System Log 
- Number of bytes used in the System Log

*Rationale: The purpose of the Executive Services Exception and Reset Log is to 
log all resets and all exceptions that occur.*

### cES1522

Upon a Processor Reset, the cFE shall set the System Log Mode to discard.

*Rationale: Want to preserve the System Events that may have captured the cause 
of the processor reset.*

## Executive Services Subgroup: Platform Adaptation {#cES17}

### cES1700

The cFE shall support a maximum `<PLATFORM_DEFINED>` cFE Applications.

*Rationale: TBD has never been exceeded on past missions.  Need to 
bound the number of Applications in order to size the Systems Resources 
information.*

### cES1702

The cFE shall detect all unmasked CPU exceptions.

*Rationale: Need to be able to detect processor exceptions so that the 
appropriate action can be taken.*

#### cES1702.1

Upon detection of a CPU exception, the cFE shall add an entry in the Executive 
Services Exception And Reset Log.

*Rationale: Need to log processor exceptions so that the ground can have 
visibility into the exception.*

#### cES1702.2

If the CPU exception was caused by a cFE Application and the Exception Action 
indicates that the Application can be started individually, the cFE shall 
restart the cFE Application that caused the exception.

*Rationale: In most cases, restarting the cFE application will clear up the 
problem. In some cases, however, applications are tightly coupled with other 
applications in which case, starting an individual application may have 
undesirable consequences.  The cFE should provide the ability to define if the 
application should can be restarted or if a processor reset should occur.*

#### cES1702.3

If the CPU exception was caused by the Operating System or cFE Core then the 
cFE shall initiate a `<PLATFORM_DEFINED>` response.

*Rationale: An exception in the cFE core or OS will restart the cFE, which 
results in all cFE Applications being restarted.*

### cES1703

The cFE shall detect all unmasked processor Floating Point Exceptions.

*Rationale: The low level BSP routines allow the mission to determine 
what Floating Point exceptions are masked, and what Floating Point 
exceptions can interrupt the software.*

#### cES1703.1

Upon detection of an unmasked Floating Point exception, the cFE shall add an 
entry in the Executive Services Exception and Reset Log.

*Rationale: Need to log unmasked Floating Point exceptions so that the ground 
can have visibility into the exception.*

#### cES1703.2

If the Floating Point exception was caused by a cFE Application and the 
Exception Action indicates that the Application can be started individually, 
the cFE shall restart the cFE Application that caused the exception.

*Rationale: Want the capability to restart an individual application on a processor 
reset.  Not all Applications should be started individually as they may have some 
dependencies that result in undesirable behavior.  When the Application is started, 
one of the parameters is whether to restart the app of perform a cFE processor reset.*

#### cES1703.3

If the Floating Point exception was caused by the OS or cFE Core then the cFE 
shall initiate a `<PLATFORM_DEFINED>` response.

*Rationale: An exception in the cFE core or OS will restart the cFE, which results 
in all cFE Applications being restarted.*

### cES1704

The cFE shall support a `<PLATFORM_DEFINED>` byte volatile file system.

*Rationale: TBD seems like a reasonable size based on heritage missions.*

### cES1705

The cFE shall support a `<PLATFORM_DEFINED>` byte non-volatile file system.

*Rationale: TBD seems like a reasonable size based on heritage missions.*

### cES1706

The cFE shall support a `<PLATFORM_DEFINED>` byte Executive Services System Log.

*Rationale: TBD seems like a reasonable size based on heritage missions.*

### cES1707

The cFE shall support a `<PLATFORM_DEFINED>` byte Executive Services Exception 
And Reset Log.

*Rationale: TBD seems like a reasonable size based on heritage missions.*

### cES1708

The cFE shall support a `<PLATFORM_DEFINED>` byte Critical Data Store.

*Rationale: TBD seems like a reasonable size based on heritage missions.*

### cES1709

If the cFE Core goes through `<PLATFORM_DEFINED>` Maximum Processor Resets, the 
cFE shall initiate a Power-On Reset of the cFE.

*Rationale: After a <PLATFORM DEFINED> number of Processor Resets, the cFE will 
attempt to recover by doing a Power-on Reset.*

# Event Services Requirements (cEVS) {#cEVS}

## Event Services Subgroup 30xx {#cEVS30}

### cEVS3000

Upon receipt of Command the cFE shall enable/disable, as specified in the 
Command, the future generation of Event Messages for the Command-specified 
Event Type.

*Rationale: Filtering by type allows the system to behave differently for 
different users.*

### cEVS3001

Upon receipt of Command, the cFE shall set the SB Event Format Mode to the 
command specified value, either Long or Short.

*Rationale: Providing a short format accommodates missions with limited telemetry 
bandwidth. A long format is desirable because it contains the most information.*

### cEVS3002

Upon receipt of Command the cFE shall generate a NO-OP event message.

*Rationale: This command is useful as a general sub-system aliveness test.*

### cEVS3003

Upon receipt of Command the cFE shall set the following counters to zero in 
Event Services telemetry:

- Valid Command Counter 
- Invalid Command Counter 
- Event Message Sent Counter 
- Event Message Truncation Counter 
- Unregistered Application Send Counter

*Rationale: This command is a common feature in heritage sub-system software design.*

### cEVS3004

Upon receipt of Command, the cFE shall write the following information to the 
Command specified cFE EVS Application Data file for each registered 
Application: 

- Application Event Message Sent Counter 
- Application Event Service Enable Status 
- Application Event Type Enable Statuses (one for each Event Type) 
- Application Event IDs (for events to be filtered) 
- Application Binary Filter Masks (one per registered Event ID) 
- Application Binary Filter Counters (one per registered Event ID)

*Rationale: Visibility of Application information is essential for FSW diagnostics, 
testing and maintenance.*

#### cEVS3004.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event that a user does not want to 
specify.*

### cEVS3005

Upon receipt of valid command, the cFE shall increment the valid command 
counter.

*Rationale: The ground needs the capability to verify that the command was 
received and accepted by the cFE.  Details of valid commands are documented 
in the cFE User' Guide.*

### cEVS3006

Upon receipt of an invalid command, the cFE shall in increment the invalid 
command counter.

*Rationale: The ground needs an indicator if a command is rejected by the cFE.  
Details of what makes a command invalid are documented in the cFE Developer's 
Guide and the cFE User's Guide.*

### cEVS3007

Upon receipt of Command the cFE shall enable/disable, as specified in the 
Command, the future generation of Event Messages for the Command-specified 
Application and Event Type.

*Rationale: When diagnosing an Application it's useful to enable its informational 
and diagnostic Event Messages.*

### cEVS3008

Upon receipt of Command the cFE shall enable/disable, as specified in the 
Command, the future generation of Event Messages for the Command-specified 
Application.

*Rationale: During development and integration it can be useful to turn off 
an Application's Event Message's without regard of the Event Type.*

### cEVS3009

Upon receipt of Command, the cFE shall set the Command-specified Application's 
Event Message Sent Counter to zero.

*Rationale: Allow operators the ability to reset the count of application events.*

### cEVS3010

Upon receipt of Command, the cFE shall set an Application's Binary Filter 
Counter to zero for the Command-specified Event ID.

*Rationale: Clearing an Application's Filtered Event Message Counter is a convenient 
method for resetting the filter on the event.*

### cEVS3011

Upon receipt of Command, the cFE shall set all of an Application's Binary 
Filter Counters to zero.

*Rationale: Having the ability to reset all Application Filtered Event Message 
Counters is a quick method for resetting all the application's event filters. 
Note: This command gives operators the ability to reset all exhausted event filters 
(i.e. send 16 and stop) so that filtered events may be generated once again.*

### cEVS3012

Upon receipt of Command the cFE shall set an Application's Binary Filter Mask 
to the Command-specified Event Filter for the given Application Event ID.

*Rationale: Allow an operator to tune the system for a particular operational 
environment.*

### cEVS3013

_(OPTIONAL)_ Upon receipt of Command, the cFE shall clear the Local Event Log.

*Rationale: Need to be able to clear the log if the event logging is operating 
in discard mode.*

### cEVS3014

_(OPTIONAL)_ Upon receipt of Command, the cFE shall set the Event Logging Mode 
to the Command-specified mode, either overwrite or discard.

*Rationale: While in Overwrite Mode the oldest logged event will be overwritten 
by the new event when the Event Log Full Flag is set to true.  While in Discard 
Mode the new event will be discarded, preserving the contents of the full log. 
Need the ability to switch between Event Message logging modes.*

### cEVS3015

_(OPTIONAL)_ Upon receipt of Command, the cFE shall write the contents of the 
Local Event Log to the Command specified file.

*Rationale: Allows for ground view of the log.  Local Event Log is not intended 
for operation playback.  The main purpose of the Local Event Log is for ground 
testing.  Ground operators may view the data file for playing back stored events.*

#### cEVS3015.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event that a user does not want to 
specify one.*

### cEVS3016

_(OPTIONAL)_ The cFE shall write each Event Message from the earliest logged 
message to the most recently logged message.

*Rationale: Ground operators like to see the progression of events in the order that 
they occurred.   In a scenario when a time correction has been made, the earliest 
logged may not necessarily mean the oldest time stamp.  This type of playback may not 
be desirable for Event Logs of considerable size.  It is worthwhile for cFE users to 
keep Event Logs relatively small.*

### cEVS3017

Upon receipt of Command the cFE shall enable/disable, as specified in the 
Command, the routing of all future Event Messages to the Command
- specified Event Message Port.

*Rationale: Event Message Output Ports may not be available or needed; therefore 
the ability to configure sending to an Event Message Port is useful.*

### cEVS3018

The cFE shall provide the following Event Service data items in telemetry (SB 
Messages): 

- Valid Command CounterInvalid Command Counter 
- SB Event Format ModeEvent Message Sent Counter 
- Event Message Truncation Counter 
- Unregistered Application Send Counter 
- Event Message Output Port Enable Statuses 
- _(OPTIONAL)_ Local Event Log Full Flag 
- _(OPTIONAL)_ Local Event Log Overflow Counter 
- _(OPTIONAL)_ Logging ModeFor each registered 
- Application:Application Event Message Sent Counter 
- Application Event Service Enable Status


### cEVS3019

Upon receipt of Command the cFE shall add the specified Event Filter for the 
specified cFE Application and Event ID.

*Rationale: Want to provide the ability for filters to be added at runtime.  
Original implentation did not provide this capability which resulted in 
modifications to Applications for those events that were not registered 
(i.e. forgotten).*

#### cEVS3019.1

If the Event ID is already registered for filtering, the cFE shall reject the 
command and generate an event message.

*Rationale: Filter is done on a per Event ID basis.*

#### cEVS3019.2

If the maximum number of Event IDs have been registered for the specified 
Application then the cFE shall reject the command and generate an event message.

*Rationale: In order to scale the cFE, allocations are made for each 
cFE Application (see cEVS3302)*

### cEVS3020

Upon receipt of Command the cFE shall delete the specified Event Filter for the 
specified cFE Appliction and Event ID.

*Rationale: Want to be able to remove event filtering since the cFE limits the 
number of filters that can be registered for a given cFE Application (see cEVS3302)*

#### cEVS3020.1

If the Event ID is not registered for filtering then the cFE shall reject the 
command and generate an event message.

*Rationale: Notify caller that the Filter was not registered.*

## Event Services Subgroup: Application Interface {#cEVS31}

### cEVS3100

Upon receipt of Request, the cFE shall register an Application for event 
service, enabling the Application Event Service Enable Status and storing the 
following request specified Application data: Application Event IDs (for events 
to be filtered) Application Binary Filter Masks (one per registered Event ID).

*Rationale: Supports the cFE's dynamic Application model.*

#### cEVS3100.1

Upon receipt of Request to register an Application for event service, the cFE 
shall create one Application Binary Filter Counter per registered Event ID.

*Rationale: Supports the cFE's Binary Filtering Algorithm.*

#### cEVS3100.2

Upon receipt of Request to register an Application for event service, the cFE 
shall create an Application Event Message Sent Counter.

*Rationale: Supports the cFE's dynamic Application model.*

#### cEVS3100.3

Upon receipt of Request to register an Application for event service, the cFE 
shall use the `<PLATFORM_DEFINED>` Application Event Type Enable Statuses for 
each event.

*Rationale: By default each platform should have a standard configuration for 
each event message type (eg. DEBUGs disable.  INFO, ERROR and CRITICAL events 
DISABLED) that should be applied to all events.  Commands are available to 
modify the settings.*

### cEVS3101

Upon receipt of Request, the cFE shall un-register an Application from using 
event services, deleting the following Application data: 

- Application Event Message Sent Counter 
- Application Event Service Enable Status 
- Application Event Type Enable Statuses (one for each Event Type) 
- Application Filtered Event IDs 
- Application Binary Filter Masks (one per registered Event ID) 
- Application Binary Filter Counters (one per registered Event ID)

*Rationale: Supports the cFE's dynamic Application model allowing applications to 
clean up after themselves.*

### cEVS3102

The cFE shall support the following Event Message Types: Debug Informational 
Error Critical.

*Rationale: Supports cFE event services.*

### cEVS3103

Upon receipt of a Request to send an Event Message, the cFE shall create a 
Short or Long Event Message, as specified by the SB Event Format Mode, ONLY if 
the following cFE conditions are met: 

1. The requesting Applications Event Service Enable Status is Enabled. 
2. The requesting Applications registered message filtering algorithm indicates 
the message should be sent. 
3. The requesting Applications Event Type Enable Status is Enabled for the 
Event Type of the request-specified Event Message.

*Rationale: Supports cFE event services*

#### cEVS3103.1

If the requesting Application has been determined to be unregistered the cFE 
shall increment the Unregistered Application Send Counter and send an event 
message.

*Rationale: Provides ground operators the ability to see when unregistered 
applications are using event services.*

#### cEVS3103.2

The cFE shall support the following data types within an applications Request 
specified Event Data: 

- Signed Character 
- Unsigned Character 
- Short Integer 
- Unsigned Short Integer 
- Long Integer 
- Unsigned Long Integer

*Rationale: Not all processors supported under the cFE will allow floating point or 
real types.*


#### cEVS3103.3

If the request specified Event ID has been registered for binary event 
filtering, the cFE shall increment the request specified Application's Binary 
Filter Counter for the request-specified Event ID.

*Rationale: Support binary event filtering algorithm.*

##### cEVS3103.3.1

If the Binary Filter Counter has reached its maximum value of 2^16^-1, the cFE 
shall retain the maximum value (i.e. do not rollover to zero).

*Rationale: Preventing a counter rollover to zero eliminates the case when a 
user may think no events have occurred when in fact many events have occurred.*

#### cEVS3103.4

If the SB Format Mode is set to Long, the cFE shall generate an SB Event 
Message formatted as specified in the cFE User's Guide containing the 
Spacecraft Time, Processor ID, Application ID, Event ID, Event Type, and the 
Request-specified Event Data.

*Rationale: To assist with system test and diagnostics it is important to 
distinguish otherwise identical messages by their sources.  This is especially 
true in a distributed system with common subsystems. It is the intent of this 
requirement to allow identical subsystems to be resident in multiple processors 
within the flight system.  In cases like this, the subsystem identifier, and 
message identifier may be the same; only the processor identifier would make the 
messages unique.  A long format is desirable because it contains the most 
information.  Design Note: It is expected that in the implemented event service, 
the API only requires the Event ID and Type, and that the underlying utility code 
fills in the processor ID, and Application ID before the message is sent.*

##### cEVS3103.4.1

If the Event Data is greater than the `<MISSION_DEFINED>` maximum Event Data, 
the cFE shall truncate the Event Message with a string-termination character 
and increment the Message Truncation Counter.

*Rationale: It's important to know if data is lost, but it doesn't need to be 
maintained on a per-Event ID basis, therefore a single counter is used.  A 
platform-defined length was considered but this prevents application reuse unless 
a minimum length is specified.*

#### cEVS3103.5

If the SB Format Mode is set to Short, the cFE shall generate an SB Event 
Message formatted as specified in the cFE User's Guide containing the 
spacecraft time, Processor ID, Application ID, Event ID, and Event Type.

*Rationale: Providing a short format accommodates missions with limited 
telemetry bandwidth.*

#### cEVS3103.6

The requester shall be able to specify the Application ID to be used in the 
Event Message.

*Rationale: Need to provide APIs a way to specify an Application ID to ensure 
the context of the Event is correct.*

#### cEVS3103.7

The requester shall be able to specify the time to be used in the Event Message.

*Rationale: This routine should be used in situations where an error condition is 
detected at one time, but the event message is reported at a later time*

### cEVS3104

**ID:** 1126

For each created Event Message, the cFE shall increment the Application Event 
Message Sent Counter for the request
- specified Application.

*Rationale: A counter provides a means for keeping track of how many events an 
application has sent through the event service. Note: The Application Event 
Message Sent Counter is incremented once regardless of how many enabled ports 
the message has been sent through.*

#### cEVS3104.1

If the Application Event Message Sent Counter has reached its maximum value of 
2^16^-1, the cFE shall retain the maximum value (i.e. do not rollover to zero).

*Rationale: Preventing a counter rollover to zero eliminates the case when a 
user may think no events have occurred when in fact many events have occurred.*

### cEVS3105

For each created Event Message, the cFE shall increment the Event Message Sent 
Counter.

*Rationale: A counter provides a means for keeping track of how many events have 
been sent through the event service. Note: The Event Message Sent Counter is a 
count of ALL messages sent through the Event Service regardless of the Application 
sending the event. The Event Message Sent Counter is incremented once regardless 
of how many enabled ports the message has been sent through.*

#### cEVS3105.1

If the Event Message Sent Counter has reached its maximum value of 2^16^-1 the 
cFE shall retain the maximum value (i.e. do not rollover to zero).

*Rationale: Preventing a counter rollover to zero eliminates the case when a 
user may think no events have occurred when in fact many events have occurred.*

### cEVS3106

Upon receipt of a request, the cFE shall set an Application's Binary Filter 
Counter to zero for the Application request-specified Event ID.

*Rationale: Clearing an Application's Binary Filter Counter is a convenient method 
for resetting the filter on the event.*

### cEVS3107

Upon receipt of a request, the cFE shall set all of an Application's Binary 
Filter Counters to zero for the request-specified Application.

*Rationale: Having the ability to reset all Application Filtered Event Message 
Counters is a quick method for resetting all the application's event filters.*

### cEVS3108

_(OPTIONAL)_ For each created Event Message, the cFE shall store the Event 
Message in the Local Event Log in the Long Event Message Format.

*Rationale: It's useful to save Event Messages when external communications 
is unavailable. This may occur during system initialization (especially events 
generated from other cFE components) and during a communications failure.*

#### cEVS3108.1

_(OPTIONAL)_ If the Local Event Log becomes full, the cFE shall set the Local 
Event Log Full Flag to true.

*Rationale: Ground operations needs to know the state of the Local Even Log.*

#### cEVS3108.2

_(OPTIONAL)_ If the Local Event Log is full, the cFE shall increment the Local 
Event Log Overflow counter.

*Rationale: Ground operations needs to know how many Event Messages were discarded 
or overwritten.*

#### cEVS3108.3

_(OPTIONAL)_ If the Local Event Log is full, the cFE shall either (1) overwrite 
the oldest Event Message if the Event Logging Mode is overwrite, or (2) discard 
the Event Message if the Event Logging Mode is discard.

*Rationale: Overwriting the oldest message is useful for nominal operations because 
a user doesn't need to periodically clear the Log. If an error occurs when there's 
no communication then the Local Event Log size must be large enough to retain the Event 
Messages since communications was lost. Discarding the newest Event Message is useful for 
trouble shooting a problem. For example there may be a problem during processor initialization 
that occurs when there's no communications with the processor interfacing with the User. In 
this case the original Event Messages are critical to solving the problem so they should be 
preserved. Note that the Event Logging Mode can be changed via stored commanding.*

### cEVS3109

For each created Event Message, the cFE shall route the Event Message, 
formatted as an ASCII text string, to each enabled Event Message Output Port.

*Rationale: Debug ports are very useful for FSW development and maintenance.*

### cEVS3110

Upon receipt of Request the cFE shall free resources allocation for the 
specified Application.

*Rationale: Need to be able to remove all resources associated with an 
application when Deleting, Restarting or Reloading an Application.  In 
addtion, this is required as part of a Processor Reset*

## Event Services Subgroup: Hardware Interfaces {#cEVS32}

### cEVS3200

Upon a Power-on Reset, the cFE shall set the `<PLATFORM_DEFINED>` Event Message 
Output Ports to their `<PLATFORM_DEFINED>` default state: 

- Port Number 1: `<PLATFORM_DEFINED, Enabled >` 
- Port Number 2: `<PLATFORM_DEFINED, Disabled >` 
- Port Number 3: `<PLATFORM_DEFINED, Disabled >` 
- Port Number 4: `<PLATFORM_DEFINED, Disabled >`

*Rationale: Event Message Ports are customized for each platform.  A rationale for 
all of the Power-on Reset requirements is that the cFE must initialize to a known 
state.*

### cEVS3201

Upon a Power-on Reset, the cFE shall set SB Event Format Mode to Long.

*Rationale: Each mission must determine what format they need.*

### cEVS3202

_(OPTIONAL)_ Upon a Power-on Reset, the cFE shall set the Local Event Log Full 
Flag to false.

*Rationale: Set cFE to default status across Power-on Resets.*

### cEVS3203

_(OPTIONAL)_ Upon a Power-on Reset, the cFE shall set the Event Logging Mode to 
`<PLATFORM_DEFINED, overwrite>`.

*Rationale: Set cFE to default status across Power-on Resets.*

### cEVS3207

_(OPTIONAL)_ Upon a Processor Reset, the cFE shall preserve or overwrite the 
contents of the Local Event Log based on the setting of the Event Logging Mode 
configuration parameter.
            
Note: The contents of the Local Event Log will be preserved if the Event 
Logging Mode is configured to Discard (1). The contents of the Local Event Log 
may be overwritten (depending on the size and contents of the log prior to the 
reset) if the Event Logging Mode is configured to Overwrite (0).

*Rationale: Try to retain the contents even across a processor reset because 
there may be useful diagnostic information.*

### cEVS3208

OPTIONAL> Upon a Processor Reset, the cFE shall preserve the Local Event Log 
Full state.

*Rationale: Retain the cFE state across Processor Resets.*

### cEVS3209

_(OPTIONAL)_ Upon a Processor Reset, the cFE shall set the Event Logging Mode 
to the `<PLATFORM_DEFINED>` value.

*Rationale: Retain the cFE state across Processor Resets.*

### cEVS3210

_(OPTIONAL)_ Upon a Processor Reset, the cFE shall preserve the Local Event Log 
Overflow Counter.

*Rationale: Retain the cFE state across Processor Resets*

## Event Services Subgroup: Platform Adaptation {#cEVS33}

### cEVS3300

The cFE shall support `<PLATFORM_DEFINED, 4>` Event Message Ports.

*Rationale: Since the number of Event Message Ports is hardware dependent a 
mission must be able to specify how many ports it will support.*

### cEVS3301

_(OPTIONAL)_ The cFE shall define a Local Event Log with a capacity of 
`<PLATFORM_DEFINED, 20>` Event Messages.

*Rationale: Each mission's processor card memory capacities will dictate 
what is reasonable.*

### cEVS3302

The cFE shall support `<PLATFORM_DEFINED, 8>` Event Message Filters per cFE 
Application.

*Rationale: Each cFE Application that register with EVS is allocated memory to 
support this limit.*

# Software Bus Requirements (cSB) {#cSB}

## Software Bus Subgroup: Operational Interface {#cSB40}

### cSB4000

Upon receipt of a NOOP command, the cFE shall increment the command counter.

*Rationale: Useful for verifying communication between the ground and SB task.*

### cSB4001

Upon receipt of Command the cFE shall set to zero the following counters in 
housekeeping telemetry: 

- Valid command counter 
- Invalid command counter 
- No subscriptions counter 
- Message send error counter 
- Message receive error counter 
- Create Pipe error counter 
- Subscribe error counter 
- Pipe Overflow error counter 
- MsdID-to-pipe limit error counter

*Rationale: Providing a clear counter command eliminates the need for operators
or onboard scripts to keep track of the last value of the counter.*

### cSB4002

Upon receipt of a Command, the cFE shall send an SB Message containing the SB 
statistics.

*Rationale: Visibility of the SB statistics is essential for FSW diagnostics,
testing, and maintenance.*

### cSB4003

Upon receipt of a Command, the cFE shall save the SB routing information to the 
Command specified file.

*Rationale: Visibility of the SB routing information is essential for FSW
diagnostics, testing, and maintenance.*

#### cSB4003.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event that a user does not want to
specify one.*

### cSB4004

Upon receipt of a Command, the cFE shall enable or disable routing the command 
specified message to the command specified pipe.

*Rationale: Supports testing and maintenance. In testing, it may be desirable
to enable/disable routing a message to a diagnostic application.*

### cSB4005

The cFE shall send an SB Message containing the following housekeeping 
telemetry items: 

- Valid command counter 
- Invalid command counter 
- No subscriptions counter 
- Message send error counter 
- Message receive error counter 
- Pipe overflow error counter 
- MsgId-to-pipe limit error counter

*Rationale: SB Telemetry is essential for operations, FSW diagnostics,
testing, and maintenance.*

### cSB4007

Upon receipt of a Command, the cFE shall save the Pipe information to the 
Command specified a file.

*Rationale: This gives detailed information about each pipe such as owner
of the pipe, queue depth and send errors which can be used for diagnostics purposes.*

#### cSB4007.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event that a user does not want to
specify one.*

### cSB4008

Upon receipt of a Command, the cFE shall save the message map information to 
the Command specified a file.

*Rationale: This information provides information pertaining to the internals
of the SB (Msg ID, and sB Routing table index) which can be used for
diagnostic purposes.*

#### cSB4008.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event that a user does not want to
specify one.*

## Software Bus Subgroup: Application Interface {#cSB43}

### cSB4300

The cFE shall provide a zero-copy message transfer mode for intra-processor 
communication.

*Rationale: The heritage software bus offered a zero-copy mode, it was used
for transferring large packets from one application to another on the same
processor. The cost of the zero-copy mode is a more complex application
interface.*

### cSB4301

Upon receipt of a Request to create a Pipe, the cFE shall create a Pipe with 
the Request-specified Pipe Depth and the Request-specified name.

*Rationale: Runtime creation of Pipes supports the cFE goals of easy
Application integration and system reconfiguration.*

### cSB4302

Upon receipt of a Request to delete a Pipe, the cFE shall Unsubscribe all 
messages to the Request-specified Pipe, then remove the Pipe from the Routing 
Information.

*Rationale: An Application must be capable of freeing its own resources*

### cSB4303

Upon receipt of a Request to Subscribe to an SB message, the cFE shall 
establish a route using the Request-specified Message ID, the Request-specified 
Pipe ID, the Request-specified MsgId-to-Pipe limit and the Request-specified 
QoS.

*Rationale: Runtime subscription supports the cFE goals of easy system\
integration and runtime reconfigurations.*

#### cSB4303.1

If the Subscription is a duplicate of a previous subscription then the cFE 
shall issue and event message.

*Rationale: Duplicate subscriptions are still a success, however, want to
notify the ground.*

### cSB4304

Upon receipt of a Request to Unsubscribe to an SB Message, the cFE shall remove 
the route corresponding to the Request-specified Message Id and the 
Request-specified Pipe Id from the Routing Information.

*Rationale: Allow an Application to dynamically change what Messages it receives.
Comments:  Since the ground can manipulate the state of the routing, it is not
considered an error if the Message is not currently being subscribed to when an
unsubscribe request is made.*

### cSB4305

Upon receipt of a Request to send an SB Message, the cFE shall route the SB 
Message to the Pipes of all Applications that have Subscribed to the SB Message.

*Rationale: This form of message passing, supports one-to-one, one-to-many, and
many-to-one (all desired by branch members) in a flexible, low overhead manner.
Comment:  It has been common for telemetry packets to be sent to many
destinations, such as a real-time telemetry.*

#### cSB4305.1

The cFE shall limit the number of messages of a particular Message ID that can 
be sent to an Application's Pipe.

*Rationale: Prevent an errant application from monopolizing a Software Bus Pipe.
This feature was part of the heritage software bus and was found to be a useful
diagnostic feature. Comment:    When subscribing to a packet, an Application
can specify a limit on the n*

#### cSB4305.3

If routing a Message to an Application's Pipe results in a Pipe Overflow, the 
cFE shall abort the send to that pipe, issue an event, and continue sending to 
the remaining pipes.

*Rationale: The ground needs to know if a message cannot be routed to its
destination. Comment:  A Pipe Overflow occurs when the SB attempts to write
to a Pipe that is completely full of SB messages. The number of messages a
Pipe is capable of storing is defined by a platform configuration parameter.*

#### cSB4305.4

If routing a Message to an Application's Pipe would exceed the MsgId-to-Pipe 
Limit, the cFE shall abort the send to that pipe, issue an event, and continue 
sending to the remaining pipes.

*Rationale: The ground needs to know if a message cannot be routed to its
destination. Comment:  The MsgId-to-Pipe Limit is exceeded when the SB attempts
to write to a Pipe that contains the maximum number of messages of a particular
Message Id.*

#### cSB4305.5

Upon receipt of a Request to send a Message, if no Application's have 
Subscribed to the Message, the cFE shall increment the No Subscriber's Counter 
and send an event that displays the Message ID.

*Rationale: A single counter is enough to notify operators that there are no
receivers for a packet that is being sent. The 'no subscriber's' event is
completely filtered by default. The ground can unfilter the 'no subscribers'
event to get more information about the*

#### cSB4305.6

If the SB Message is greater than the `<MISSION_DEFINED,32767>` bytes then the 
cFE shall not send the message, issue and event message and increment the 
message send error counter.

### cSB4306

Upon receipt of a Request to receive a SB Message from a Pipe without a 
timeout, the cFE shall remove the next SB Message from the Pipe and provide the 
message to the Application.

*Rationale: Applications need a mechanism to retrieve the SB Message they have
subscribed to receive. A receive Request without a suspension timeout provides
a non-blocking method of retrieving SB messages. Receives are performed on a
Pipe basis not on a SB Message b*

### cSB4307

Upon receipt of a Request to receive a SB Message from a Pipe with a pending 
timeout, the cFE shall suspend execution of the Application until a SB Message 
is present on the Pipe or the timeout has expired.

*Rationale: A receive Request with a suspension timeout provides a blocking
method of retrieving SB messages. This is useful for data driven Applications
and has been used on all heritage missions with a SB. The timeout is useful for
fault recovery for Applications t*

### cSB4308

Upon receipt of a Request to receive a SB Message from a Pipe with an infinite 
timeout, the cFE shall suspend execution of the Application until a SB Message 
is present on the Pipe.

*Rationale: This mode of receiving has been the most commonly used mode on the
heritage SB.*

### cSB4309

Upon receipt of a Request, the cFE shall provide sender information for the 
last message received on an Application's Pipe.

*Rationale: Heritage SB did a 'valid senders check' before delivering a packet
to a pipe. Since the cFE supports a dynamic environment and the sender of a
packet is somewhat unknown, the cFE must provide a means for the application to
do the 'valid sender check'*

### cSB4310

Upon receipt of Request the cFE shall free resources allocation for the 
specified Application.

*Rationale: Need to be able to remove all resources associatedwith an
application when Deleting, Restarting or Reloading an Application.
In addtion, this is required as part of a Processor Reset.*

## Software Bus Subgroup: Hardware Interfaces {#cSB45}

### cSB4500

Upon a Power-on Reset the cFE shall initialize the Routing Information and 
clear all error counters.

*Rationale: The cFE must initialize to a known state.*

### cSB4501

Upon a Processor Reset the cFE shall initialize the Routing Information and 
clear all error counters.

*Rationale: The cFE must initialize to a known state.*

## Software Bus Subgroup: Platform Adaptation {#cSB47}

### cSB4700

The cFE shall support a maximum of `<PLATFORM_DEFINED, 1024>` Message ID's.

*Rationale: Allow a mission to scale the number of messages it can process to
reduce SB memory usage.  A maximum is specified to yield an efficient design.
1024 was chosen as the default based on the message count of previous missions.*

### cSB4701

The cFE shall support a `<MISSION_DEFINED, 32768>` bytes maximum system packet 
size.

*Rationale: The cFE must have some protection in the event that the packet
length field of a sender's packet becomes corrupted or is invalid.*

### cSB4704

The cFE shall support a maximum of `<PLATFORM_DEFINED, 64>` Destinations for a 
Message ID.

*Rationale: Allow a mission to scale the number of destinations per message ID
to reduce memory requirements. A maximum is specified to yield an efficient
design. The default of 16 is used in the heritage SB.*

### cSB4705

The cFE shall support a maximum of `<PLATFORM_DEFINED, 255>` Pipes per 
processor.

*Rationale: Allow a mission to scale the number of Pipes to reduce memory
requirements.  The default of 255 allows a single byte to be used for the
pipe id.*

### cSB4706

The cFE shall support a maximum Pipe depth of `<PLATFORM_DEFINED, 65535>` SB 
Messages.

*Rationale: Allow a mission to scale the Pipe depth to reduce memory
requirements.  The default of 65535 is a power of 2 which may simplify
implementation and based on recent missions 64 SB Messages is more than
adequate.*

# Table Services Requirements (cTBL) {#cTBL}

## Table Services Subgroup: Operational Interface {#cTBL60}

### cTBL6000

Upon receipt of Command the cFE shall load an Inactive Table Image with the 
contents of the Command specified File.

*Rationale: Loading from a file allows for multiple versions of a table to
be stored on board and loaded to the active table when appropriate. Design
Note: The file header will identify the Table that the file contents are for.*

#### cTBL6000.1

If the Command specified file's header indicates that the file contains only a 
portion of the Table, the cFE shall first load an Inactive Table Image with the 
contents of the Active Table Image and then load the contents of the Command 
specified File.

*Rationale: A Partial Table load capability is useful when dealing with large
Tables.  It helps to ensure that additional parameters are not unintentionally
modified, reduces command time required to perform a Table update and is a
feature that has been used on previous missions.*

#### cTBL6000.2

If the number of data bytes contained in the file is greater than the maximum 
size of the table then the load shall be aborted and an event message shall be 
generated.

*Rationale: This is a sanity check to make sure that the ground generated table
load does not include more data than a table can handle.*

#### cTBL6000.3

If the number of bytes specified in the file's header is not equal to the 
number of data bytes contained in the file then the load shall be aborted and 
an event message be generated.

*Rationale: This is another sanity check to make sure that thenumber of bytes
specified in thefile header is equal to the number of data bytes in the file.*

#### cTBL6000.4

The Inactive Table Image shall only be loaded with the contents of the Active 
Table if the Inactive Table Image has not been initialized.

*Rationale: Want to be able to perform successive  partial table loads.  The
first partial load of a table requires that the inactive table image be
initalized with the active table image (see cTBL6000.2).  Any subsequent loads
should be made with the exisiting contents of the Inactive Buffer.*

#### cTBL6000.5

If the specified table is defined as Dump Only then the command shall be 
rejected and an event message be generated.

*Rationale: Dump only tables cant be loaded.*

### cTBL6001

Upon receipt of Command the cFE shall dump the Command specified Active or 
Inactive Table contents to a Command specified File.

*Rationale: Dumping the contents of a table to a file simplifies the
telemetering of Table contents.  The file transfer protocol between the ground
and the spacecraft is capable of handling variable sized files.  The
telemetering of Table contents directly to ground.*


### cTBL6002

Upon receipt of Command the cFE shall determine the validity of the contents of 
either the Active or Inactive Table Image of the Command specified Table.

*Rationale: Operations needs to verify the contents of an Inactive Table before
Committing it. Similarly, operations personnel may need to re-validate or
identify the contents of an Active Table.*

#### cTBL6002.1

The cFE shall compute a Data Integrity Check Value on the contents of either 
the Active or Inactive Table Image of the Command specified Table and report 
the result in telemetry.

*Rationale: A Data Integrity Check Value can provide a quick method of
validating the proper contents of a Table without performing a Table Dump
and Comparison.*

#### cTBL6002.2

The cFE shall Request an Application to validate the contents of either the 
Active or Inactive Table Image of the Command specified Table and report the 
result in telemetry.

*Rationale: A Table Validation function provided by the Application can
verify that the data content of a Table are reasonable.*

### cTBL6003

Upon receipt of Command the cFE shall make an Inactive Table Image of the 
Command specified Table the Active Table Image.

*Rationale: A Commit Command is useful by allowing modifications and the
verification of those modifications to be accomplished in the background
while an Application continues to use its previous Table data.  Then, upon
Command, the new Table data is made Active.*

#### cTBL6003.1

If a Table Validation Function exists for the specified Table, the Inactive 
Table Image shall be validated.

*Rationale: Validation functions are there for a reason so want to prevent
loading invalid tables.  Inactive buffer retains the data so that the ground
can dump the contents to see why the validation failed.*

##### cTBL6003.1.1

If the Inactive Table Image fails validation then the Inactive Table Image 
shall not be loaded into the Active Table Image.

*Rationale: Prevent loading of an invalid table. Note that the Inactive Table
still contains the table image so that the ground can dump it.*

##### cTBL6003.1.2

If a Table Validation Function does not exist for the specified Table, the 
Table shall be considered valid.

*Rationale: If there is no validation function then assume the table has
good data in it (no way to check otherwise).*

### cTBL6005

Upon receipt of Command the cFE shall write the contents of the Table Registry 
to a file.

*Rationale: The Table Registry will provide a list of the Tables currently
registered, their attributes (double or single-buffered, dump-only, etc), a
history of what files were last loaded into the Active Table Images, what
time the load occurred and whether the ima*

#### cTBL6005.1

If a file is not specified, the cFE shall use the `<PLATFORM_DEFINED>` filename.

*Rationale: Want to provide a default in the event that a user does not want to
specify one.*

### cTBL6006

Upon receipt of Command the cFE shall telemeter the contents of the Table 
Registry associated with a Command specified Table.

*Rationale: Operations personnel may wish to see the attributes (double or
single-buffered, dump-only, etc), a history of what file(s) was/were last\
loaded into the Active Table Image, what time the load(s) occurred, etc.*

### cTBL6007

Upon receipt of Command the cFE shall generate a NO-OP event message.

*Rationale: This command is useful as a general sub-system aliveness test.*

### cTBL6008

Upon receipt of Command the cFE shall set the following counters reported in 
telemetry to zero: 

- Valid Command Counter 
- Invalid Command Counter 
- Number of Validation Requests 
- Number of successful Table Validations 
- Number of Unsuccessful Validations 
- Number of Completed Validations

*Rationale: This command is a common feature in heritage sub-system software
design.*

### cTBL6009

Upon receipt of valid command, the cFE shall increment the valid command 
counter.

*Rationale: The ground needs the capability to verify that the command was
received a accepted by the cFE.  Details of  valid commands are documented
in the cFE User's Guide.*

### cTBL6010

Upon receipt of an invalid command, the cFE shall in increment the invalid 
command counter.

*Rationale: The ground needs an indicator if a command is rejected by the cFE.
Details of what makes a command invalid are documented in the cFE User's Guide.*

### cTBL6011

The cFE shall provide the following Table Service data items in telemetry (SB 
Messages):

- Valid Command Counter
- Invalid Command Counter
- Number of Tables Currently Registered
- Table Identifier of Last Table Modified
- Time of Last Table Modification
- Source Filename of Last Table Load
- Destination Filename of Last Table Dump
- Table Identifier of Commanded Data Integrity Check
- Commanded Data Integrity Check Value
- Commanded Table Verification Function Result
- Number of unused Shared Buffers
- Table Name of Last Table Load

### cTBL6012

Upon receipt of Command the cFE shall abort the loading of the specified Table.

*Rationale: The ground needs a way to clear the inactive buffer of all loads in
the event, for example, that erroneous values were loaded to the table by the
ground. Note that once a table is activated, it cannot be aborted (significant
for double-buffer tables).*

#### cTBL6012.1

If the Table buffering characteristics for the specified Table indicate that it 
is a Single-buffered Table, then the allocated shared buffer shall be released.

*Rationale: The shared buffer being used for this table load should be placed
back into the pool of available shared buffers.*

#### cTBL6012.2

If the Table buffering characteristics for the specified Table indicate that it 
is a Double-buffered table, then the inactive buffer shall be marked as 
uninitalized.

*Rationale: Need to make sure that the table image gets re-initalized with the
contents of the active buffer prior to a subsequent load after an abort command
is sent (see cTBL6000.4)*

#### cTBL6012.3

The Table Registry shall indicate that there are no loads pending for the 
specified Table.

*Rationale: Status of the table needs to updated to reflect that*

## Table Services Subgroup: Application Interface {#cTBL63}

### cTBL6300

Upon receipt of Request, the cFE shall create a zero filled Table Image with 
the Request specified name, size, buffering characteristics (single or 
double-buffer), dump-only characteristics, and criticality and Table Validation 
Function address.

*Rationale: Supports the cFE's dynamic Application model.  Note: Tables will be
able to be either single or double-buffered.  Double-buffering will allow
instantaneous updates from the Application's point of view and will be useful for
interrupt service routines, etc.  However, double-buffering will consume more
memory resources.  The Table Validation Function is optionally provided by the
Application and is called when a Command to Validate the table is made. Note that
a zero filled image will not be created if the Application specifies an address for
the dump-only table (see cTBL6300.1).*

#### cTBL6300.1

The cFE shall allow an Application to specify an address as the one and only 
buffer for a dump-only Table.

*Rationale: Heritage code allowed for dump only tables to come from an address
instead of a true table.  In this case, table services does not allocate any
memory for the dump only table.  This is referred to an an
Application-Defined-Address-Table.*

### cTBL6301

Upon receipt of Request, the cFE shall free the resources associated the 
Request specified Application.

*Rationale: When an Application exits prematurely, the cFE requires a
mechanism to free resources allocated to that Application.*

### cTBL6302

Upon receipt of Request, the cFE shall initialize the contents of the Request 
specified Table Image with the contents of the Request specified File.

*Rationale: Supports the cFE's dynamic Application model.*

#### cTBL6302.1

If the Request specified File contains more data than the size of the Request 
specified Table, the Table Image will not be initialized and an Event Message 
shall be generated.

*Rationale: If a File contains more data than memory allows, there is a high
likelihood that the Application is expecting the data to be in a different
format.  Additionally, the cFE would not know whether to throw out the last
part or the first part of the data.*

#### cTBL6302.2

If the Request specified File contains less data than the size of the Request 
specified Table, the first portion of the Table Image will be initialized with 
the contents of the File and an Event Message shall be generated.

*Rationale: Some tables may allocate more space than is necessary at all times.
The Event Message will serve as a warning but not prevent the initialization of
the Table.*

### cTBL6303

Upon receipt of Request, the cFE shall provide the calling Application with a 
unique identifier of an existing Table Image.

*Rationale: Some Applications will not want to create a Table but obtain a
handle to one that is created by another Application.*

### cTBL6304

Upon receipt of Request, the cFE shall free resources allocated for the Request 
specified Table.

*Rationale: Supports the cFE's dynamic Application model.*

### cTBL6305

Upon receipt of Request, the cFE shall provide the calling Application with the 
address of the Request specified Table data.

*Rationale: The Table Service 'owns' all Tables and Applications must ask for
the current address of the data because the Table Service may move the Table as
updates are made.*

#### cTBL6305.1

Upon providing a calling Application with the address of a Table's data, the 
cFE shall lock the contents of the Table to prevent modification.

*Rationale: The Table Service must wait for an Application to be done with the
contents of the Table before considering modifying its contents.  This ensures
data integrity while the Application accesses the Table contents. Note: More
than one Application can access*

#### cTBL6305.2

If a Table has been modified since the last Table address request, the cFE 
shall notify the calling Application that the Table has been modified.

*Rationale: An Application may need to know when the contents of a Table have
been updated.*

### cTBL6306

Upon receipt of Request, the cFE shall unlock the contents of the Request 
specified Table.

*Rationale: The Table Service must wait for an Application to be done with
the contents of the Table before considering modifying its contents.  This
ensures data integrity while the Application accesses the Table contents.
Note:  More than one Application can access*

### cTBL6308

Upon receipt of Request, the cFE shall update the request specified Table if a 
load is pending and it is unlocked.

*Rationale: This gives control to the Application as to when the table update
occurs.*

#### cTBL6308.1

If a Table is locked when an update Request is made, an appropriate error code 
shall be returned to the calling Application and the update shall not occur.

### cTBL6309

Upon receipt of Request, the cFE shall provide the following information to the 
calling Application for the specified Table: 

- Size of the table 
- Number of Users the access to the table (sharing) 
- Filename of the last file used to modify the table 
- File creation time of last file used to modify the table 
- Time of last modificationFlag indicating if the Table has been initialized 
- Flag indicating if the Table is dump only 
- Flag indicating if the Table has a dedicated buffer 
- Flag indicating if the table is maintained in the Critical Data Store

### cTBL6310

Upon receipt of Request the cFE shall indicate if the specified table has a 
validation, update, or dump pending.

*Rationale: Applications must periodically check to see if they need to perform
updates, validations or dumps.*

### cTBL6311

Upon receipt of Request, the cFE shall provide the calling Application with the 
addresses of the data for the tables requested if more than one table is needed.

*Rationale: Applications may need to work with more than one table at a time.*

#### cTBL6311.1

Upon providing a calling Application with the addresses of a Tables' data, the 
cFE shall lock the contents of the Tables to prevent modification.

*Rationale: The Table Service must wait for an Application to be done with the
contents of the Table before considering modifying its contents.  This ensures
data integrity while the Application accesses the Table contents.*

#### cTBL6311.2

If at least one Table has been modified since the last Table addresses request, 
the cFE shall notify the calling Application that a Table has been modified.

*Rationale: An Application may need to know when the contents of a Tables has
been updated; the Application just won't know which one.*

### cTBL6312

Upon receipt of Request, the cFE shall unlock the contents of the Request 
specified Tables.

*Rationale: Applications may been to work with more than one table at a time.*

## Table Services Subgroup: Hardware Interfaces {#cTBL65}

### cTBL6500

Upon a Power-on Reset all Table resources will be freed.

*Rationale: The Table Service cannot assume the contents
of any Table is legitimate after a reset.*

### cTBL6501

Upon a Processor Reset all Table resources will be freed.

*Rationale: The Table Service cannot assume the contents
of any Table is legitimate after a reset.*

#### cTBL6501.1

If the Table is a critical table then the contents of the Table shall be 
preserved.

*Rationale: Goal of a processor reset is preserve certain data to facillitate
recovery.*

## Table Services Subgroup: Platform Adaptation {#cTBL67}

### cTBL6700

The cFE shall support `<PLATFORM_DEFINED, 128>` Tables.

*Rationale: Each mission's processor card memory capacities will dictate what
is reasonable.*

### cTBL6701

The cFE shall support `<PLATFORM_DEFINED, 4>` Single

- Buffer Table Loads and Application
- Defined
- Address
- Table dumps simultaneously

*Rationale: It is desirable to allow for Tables to be modified by ground command
while other Tables are  be being modified autonomously by on board software
(i.e. stored command processor, etc). This limit represents the total supported
for the combined number of Single-Buffer and Application-Defined-Address-Table
Tables being loaded.*

# Time Services Requirements (cTIME) {#cTIME}

## Time Services Subgroup: Operational Interface {#cTIME20}

### cTIME2000

Upon receipt of Command the cFE shall generate a Software Bus message that 
includes the following items: Time StatusMETSTCFLeap SecondsSTCF continuous 
adjustment value.

*Rationale: It is common for sub-systems to report housekeeping status upon 
receipt of a housekeeping request command.*

### cTIME2001

Upon receipt of Command the cFE shall generate a NO-OP event message. (Time 
Server and Time Client)

*Rationale: This command is useful as a general sub-system aliveness test.*

### cTIME2002

Upon receipt of Command the cFE shall set to zero all counters reported in Time 
Services telemetry. (Time Server and Time Client)

*Rationale: This command is a common feature in heritage sub-system software design.*

### cTIME2003

Upon receipt of valid command, the cFE shall increment the valid command 
counter.

*Rationale: The ground needs the capability to verify that the command was received 
and accepted by the cFE.  Details of  valid commands are documented in the cFE 
User's Guide.*

### cTIME2004

Upon receipt of an invalid command, the cFE shall in increment the invalid 
command counter.

*Rationale: The ground needs an indicator if a command is rejected by the cFE.  
Details of what makes a command invalid are documented in the cFE Uer's Guide.*

### cTIME2005

Upon receipt of Command the cFE shall set the number of Leap Seconds to the 
Command-specified value.

*Rationale: The decision to introduce a leap second in UTC is the responsibility 
of the International Earth Rotation Service (IERS).  The count of Leap Seconds has 
been incremented about every 500 days since 1972.  It is therefore likely that a 
mission will need to update.*

### cTIME2006

Upon receipt of Command the cFE shall set the STCF to the Command-specified 
value.

*Rationale: The cFE must be provided with the appropriate correlation factor, 
that when combined with the current MET and Leap Seconds values, will result 
in current time. Historically this command has been referred to as 
'jam loading' time.*

### cTIME2007

Upon receipt of Command the cFE shall compute a new value for STCF using the 
Command-specified value as current time.

*Rationale: This command provides a useful alternative to setting the STCF 
explicitly, as the command does not require knowledge of the current MET value. 
Note: If the default time format is TAI then the new value for STCF is the 
Command-specified time value less MET.*

### cTIME2008

Upon receipt of Command the cFE shall make a one time delta adjustment to the 
STCF by the Command-specified value.

*Rationale: It is common to set the clock as a delta adjustment from the current 
time value. Comment: The commanded value is signed so a positive or negative 
adjustment may be made. Note, there's no need for a command to apply a 'large' 
adjustment over a command-spec.*

### cTIME2009

Upon receipt of Command the cFE shall make a continuous 1Hz delta adjustment to 
the STCF by the Command-specified value.

*Rationale: Upon receipt of a Command to make a 'continuous' adjustment to the STCF, 
the cFE shall adjust the STCF each second by the Command-specified value. Comment: The 
commanded value is signed so a positive or negative adjustment may be made.*

### cTIME2010

`<MISSION_DEFINED>` Upon receipt of Command the cFE shall switch to the 
Command-specified hardware clock source.

*Rationale: This command is extremely hardware configuration dependent.  
The number and nature of alternate clocks will vary by system.  Many system designs 
provide primary and redundant clock sources (tone signals) and require autonomous 
clock switching based on failure.*

### cTIME2011

Upon receipt of Command the cFE shall generate a Software Bus message that 
includes time diagnostic information.

*Rationale: When testing or debugging the time application, more details about 
time services are required.*

### cTIME2012

Upon receipt of Command the cFE Time Services shall enter the Flywheel state.

*Rationale: *

#### cTIME2012.1

The cFE shall ignore Time Updates while in Flywheel state.

*Rationale: *

### cTIME2013

Upon receipt of Command the cFE shall adjust the spacecraft time by adding the 
Command specified value (seconds and subseconds) to spacecraft time.

*Rationale: *

### cTIME2014

SR1306 Upon receipt of Command the cFE shall adjust the spacecraft time by 
subtracting the Command specified value (seconds and subseconds) from 
spacecraft time.

*Rationale: *

## Time Services Subgroup: Application Interface {#cTIME23}

### cTIME2300

Upon receipt of a Request for the current time computed as TAI, the cFE shall 
provide the TAI to the requester using the format specified in the cFE 
Application Developer's Guide.

*Rationale: Need to make the TAI available to applications.*

### cTIME2301

Upon receipt of a Request for the current time computed as UTC, the cFE shall 
provide the UTC to the requester using the format specified in the cFE 
Application Developer's Guide.

*Rationale: Need to make the UTC available to applications.*

### cTIME2302

Upon receipt of a Request for the current time computed in the default format, 
the cFE shall provide the current time computed in the default selection of UTC 
or TAI to the requester using the format specified in the cFE Application 
Developer's Guide.

*Rationale: One of the mission defined selections is the choice of whether TAI or 
UTC will be the default time format.  And most callers do not care which time format 
is the current default.  This function avoids having to modify each individual caller 
when the default is requested.*

### cTIME2303

Upon receipt of a Request for the current MET, the cFE shall provide the MET to 
the requester using the format specified in the cFE Application Developer's 
Guide.

*Rationale: Need to make the MET available to applications.*

### cTIME2304

Upon receipt of a Request for the current MET seconds, the cFE shall provide 
the MET seconds to the requester using the format specified in the cFE 
Application Developer's Guide.

*Rationale: Need to make the MET seconds available to applications.  Comment: 
There was some discussion about whether this function was actually needed since 
the same data could be extracted from the MET structure by the caller. 
Per Mike Blau: the function function simplifies application coding and the development 
effort is trivial.*

### cTIME2305

Upon receipt of a Request for the current MET sub-seconds, the cFE shall 
provide the MET sub-seconds to the requester using the format specified in the 
cFE Application Developer's Guide.

*Rationale: Need to make the MET sub-seconds available to applications. Comment: There 
was some discussion about whether this function was actually needed since the same data 
could be extracted from the MET structure by the caller. Per Mike Blau: the function 
simplifies application coding and the development effort is trivial.*

### cTIME2306

Upon receipt of a Request for the current STCF, the cFE shall provide the STCF 
to the requester using the format specified in the cFE Application Developer's 
Guide.

*Rationale: Need to make the STCF available to applications.*

### cTIME2307

Upon receipt of a Request for the current Leap Seconds, the cFE shall provide 
the Leap Seconds to the requester using the format specified in the cFE 
Application Developer's Guide.

*Rationale: Need to make the Leap Seconds available to applications.*

### cTIME2308

Upon receipt of a Request for the current Clock State, the cFE shall provide 
the Clock State to the requester using the format specified in the cFE 
Application Developer's Guide.

*Rationale: Need to make the Clock State available to applications.  Note: Due 
to lack of a demonstrable use case, the requirement to provide clock state data 
as part of 'get time' functions has been deleted.*

### cTIME2309

Upon receipt of a Request to add two time values, the cFE shall provide the 
result to the requester using the format specified in the cFE Application 
Developer's Guide.

*Rationale: Need to provide a common time addition function available to 
applications.*

### cTIME2310

Upon receipt of a Request to subtract two time values, the cFE shall provide 
the result to the requester using the format specified in the cFE Application 
Developer's Guide.

*Rationale: Need to provide a common time subtraction function available to 
applications.*

### cTIME2311

Upon receipt of a Request to compare two time values, the cFE shall provide the 
result to the requester using the format specified in the cFE Application 
Developer's Guide.

*Rationale: Need to provide a common time comparison function available to 
applications.*

### cTIME2312

Upon receipt of a Request to convert a cFE sub-seconds value to micro-seconds, 
the cFE shall provide the result to the requester.

*Rationale: Converting elapsed time from sub-seconds to micro-seconds is a 
commonly used conversion utility (per Dave McComas).*

### cTIME2313

Upon receipt of a Request to convert a number of micro-seconds to sub-seconds, 
the cFE shall provide the result to the requester.

*Rationale: Need to provide the inverse of the previous function.*

### cTIME2314

Upon receipt of a Request the cFE shall return the provided system time in the 
following format: `yyyy-ddd-hh:mm:ss.xxxxx\0`

*Rationale: Provides time in a readable string  format.*

## Time Services Subgroup: Hardware Interfaces {#cTIME25}

### cTIME2500

Upon a Power-on Reset the cFE shall set the following time elements to their 
`<MISSION_DEFINED>` default values: 

- Time Status Data
- STCF
- Leap Seconds

*Rationale: At power on, the presumption is that the MET h/w register has been 
reset.  It is common to set the STCF and Leap Seconds at startup such that time 
is computed to a known and recognizable value.*

### cTIME2501

Upon a Processor Reset the cFE shall acquire the following time elements from 
the `<MISSION_DEFINED>` Critical Data Store: 

- Time Status Data
- STCF
- Leap Seconds

*Rationale: Time is a critical system resource and every effort should be made 
to preserve it.  Following a processor reset, the presumption is that the MET 
h/w register is unaffected and that previously 'good' values for STCF and Leap 
Seconds can still be used to correct.*

### cTIME2502

Upon a Processor Reset the cFE shall verify the Critical Data Store used to 
store time values.

*Rationale: Verification of potentially damaged stored data is a standard 
practice.  Note that because time can be any value, verification potentiall 
involves using a pattern.*

#### cTIME2502.1

If the critical data store is not valid, all of the time elements shall be 
initialized in the same fashion as following a power-on reset.

*Rationale: If the CDS can't be trusted then need to initialize to default values.*

## Time Services Subgroup: Platform Adaptation {#cTIME27}

### cTIME2700

During normal operation, the cFE shall preserve the following time elements in 
the `<MISSION_DEFINED>` Critical Data Store: 

- Time Status Data
- STCF
- Leap Seconds
- MET

*Rationale: Time is a critical system resource and every effort should be made 
to preserve it.  Preserving time allows applications to 'fly through' resets.*

### cTIME2701

The cFE Time Services Server shall send a time at the tone Software Bus message 
within a `<MISSION_DEFINED>` period of time preceding or following the tone.

*Rationale: The primary purpose of a time server is to distribute time updates to 
time clients. Note:  Although the time tone is generally a 1PPS signal, there is 
no cFE restriction on the frequency of the tone.  Also, there is no cFE restriction 
on whether the "time at the tone" message precedes or follows the tone.*

### cTIME2702

The cFE Time Services Server shall update its MET using the timer hardware 
interface defined in the cFE Application Developer's Guide.

*Rationale: Every mission needs an MET and using a cFE-defined software interface 
to the timer hardware driver makes the Time Services  portable.*

### cTIME2703

The cFE shall define a MET with a `<MISSION_DEFINED>` resolution.

*Rationale: The resolution (clock ticks per second) is hardware dependent. Since no 
other cFE component places any constraints on the resolution this requirement is not 
bounded with a minimum.  However, both the maximum number of MET seconds and the resolution 
of sub*

