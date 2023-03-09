# Introduction

## Purpose

To provide a simple naming convention guide for cFE/cFS for commands and
telemetry that simplifies the EDS to ground telemetry and commands
database conversion. The naming convention will apply to all shared
command and telemetry mnemonics among cFE and related apps as well as
commonly used words and abbreviations. This proposal sets to achieve
four goals:

1.  Simplify conversion of flight code into ground database. Strive for
    equivalent variable names whenever possible.

2.  Maintain binary compatibility and do not impact code functionality

3.  Enhance readability of code

4.  Provide end user with practical ground database mnemonics,
    minimizing the need for an added conversion layer or mapping.

The methodology sections below include the process in which the
convention was selected along with some examples and proposed names.

## Background

The following are inputs from Database Managers and users from TIRS, TIRS2,
MMS, ATLAS, DISCOVER that serve as the basis for the naming convention proposal.

### Commands

-   In a ground system, consistent naming and order is essential and is useful for
    procedure development and commanding during operations.  For instance,
    (Processor + Application + Action)

-   Consideration for the character limit on commands (limit use of
    underscores to classifiers, abbreviations when necessary; avoid
    unnecessary text like NoopCmd when Noop would suffice.)

-   Consistent use of underscores to make mnemonics readable.

-   Biggest problems during I&T often occur with parameters. Consistent
    command parameter names, order, lengths) simplify commanding.

### Telemetry

-   Avoid confusion by limiting underscores, periods, punctuation. Used
    to designate processors, applications, subsystems, etc. In other
    words, things that a program might want to parse on.

-   Misspellings should be corrected wherever possible.

-   Use consistent abbreviations but avoid unclear ones like DIS, PRT,
    CNT (connect vs count), SC (sequence count vs spacecraft) unless it
    is very clear in its context.


# Recommended Naming Methodology

Some general rules/recommendations to follow for cFS identifiers:

1.  Use command and parameter names that convey the intent of the corresponding
    command or parameter

2.  Use the simplest format possible for command names and parameters.

    * Avoid abbreviation unless it is common usage/knowledge and/or
    required to avoid character limit.

3.  Remove words that are inherent in that type of the mnemonic.

    * `Noop` instead of `NoopCmd`
    * `HK` instead of `HK_Pkt_Tlm` if it is always a telemetry packet
    * `Name` instead of `FileName` or `Path` instead of `FilePath` if the
    command only deals with files

4.  Individual identifier names should consist of only alphanumeric characters
    and underscores, and must _not_ begin with a digit as some languages disallow
    identifiers that begin with digits (i.e. `ONEHZ` instead of `1HZ`).

    CamelCase is preferred for terms where possible, but in cases where existing terms
    are in common use the existing case should be preserved.

5.  A full command/mnemonic name identifier recommended to be a concatenation of
    the following terms in order of significance, from left to right, where each
    term reflects a superset of the next term:

    * Spacecraft Name
    * Processor Name
    * Application Name  (aka Namespace, e.g. `CFE_ES`, `SCH`, `TO_LAB`, etc.)
    * Interface Name (aka Topic, e.g. `CMD`, `HK`, etc.)
    * Subcommand Name (if applicable, e.g. `Noop`, `Restart`, etc.)

    Example: `SPACECRAFT/cpu1/CFE_ES/CMD/Noop`

6.  A consistent character should be used as a separator between terms, but the
    specific separator character depends on what is most appropriate for the system,
    tool, or language in use.  Tools may use a forward slash (`/`), an
    underscore (`_`), a period (`.`), or a double-colon (`::`) as necessary.

    Note that underscores are allowed characters within terms, so while it is acceptable
    to concatenate using underscores, it may not be possible to perform the reverse
    operation and determine the original terms in this case.

7.  Terms at the far left or far right of a multi-part identifier may be omitted where
    they are not known or not necessary based on the context.

    * Modular software components should omit the Spacecraft and Processor names from
      the left hand side of identifiers used in flight software.

    * When referring to an entire application, such as in a test script, one may omit the
      Interface and Subcommand terms from the right hand side of identifiers.

    __NOTE__: a sufficient number of terms must always be used in order to ensure uniqueness
    and avoid ambiguity.


## Applicability to Actions / Commands

This section specifically concerns the last term within a fully qualified command name, which
indicates the specific command to perform.  As mentioned previously, command names should clearly
indicate the action being taken as well as the subject of the action.

The recommended convention for command names is a CamelCase identifier consisting of the
action first, followed by the subject.

Example action terms:

* Reset
* Send
* Enable / Disable

Example subjects:

* Counters
* Diagnostics
* SubscriptionReporting


  Example Command                     Command Name using ActionSubject Convention
  ----------------------------------- -------------------------------------------
  No operation command                Noop
  Reset Counters Command              ResetCounters
  Send Diagnostic Telemetry           SendDiagnosticTlm
  Enable Subscription Reporting       EnableSubscriptionReporting


__NOTE__: Clarity of the command name should supersede this recommendation where relevant.  In cases
where strictly following this convention would lead to command names that are less clear or do not
adequately convey intent, it is acceptable to not follow this convention.  This recommendation is
mainly to avoid command identifiers which are a mixture of `CounterReset` and `ResetCounter`, etc.


## Applicability to Flight Software

This section documents some additional rules/recommendations to follow specifically for flight
software code implemented in C or C++.

In general, to avoid name conflicts, all global identifiers in software code must employ some level
of namespace protection.

In C, any global software identifiers or terms should begin with the application name followed by
an underscore, e.g. `CFE_FS_FILE_CONTENT_ID`.

In C++, where applicable, the `namespace` keyword with the application name may be used here instead
of an underscore prefix as described below.

### Typedefs

All typedefs in source code should begin with the namespace/application prefix and end in a `_t` suffix. For
instance, the "SysTime" structure within the "CFE_TIME" application would be declared as:

`CFE_TIME_SysTime_t`

In some cases it may be beneficial to specify the nature of the type within the typedef name itself,
to differentiate between scalars, structures, enumerations, strings, arrays.  Note this is not required,
but when it is done the convention should be to put the designator just before the `_t` suffix:

  Data type classification                          Typedef suffix
  ------------------------------------------------- ------------------------
  Atomic entities (such as integers and floats)     `_Atom_t`
  Enumerated types                                  `_Enum_t`
  Strings                                           `_String_t`
  Arrays                                            `_Array_t`

Example: `CFE_ES_LogMode_Enum_t`, `CFE_EVS_EventMessage_String_t`


### Enumerators

Whenever labels are used to designate certain numeric values, the `enum` keyword should be preferred
rather than using a `#define` preprocessor macro.

Rationale:

* enum labels can only ever correspond to integer numbers, but preprocessor macros are a free-form replacement
* enum labels can be scoped into namespaces or classes in C++ code, preprocessor macros are always global

When creating enumeration labels that exist in the global namespace (which is _always_ the case for C code),
the label and type names must be constructed using a consistent convention.  The labels should be prefixed
using the enumerated type name without the suffix, concatenated using an underscore.

For example, in the CFE_EVS application, the "LogMode" parameter has two possible options, "OVERWRITE"
and "DISCARD".  Using this convention, the specific identifiers would be as follows:

Basic type name in external tool/ground system: `CFE_EVS/LogMode`

Typedef name in C code: `CFE_EVS_LogMode_Enum_t`

Label names in C code:

* `CFE_EVS_LogMode_OVERWRITE`
* `CFE_EVS_LogMode_DISCARD`

Note that this recommendation is specific for C and C++ where the label names are in global scope.  In other languages
or systems where enumeration label is not automatically a global identifier, this prefix may not be required.

## Applicability to Events and Event Identifiers

Most CFS applications also use event services (EVS) to send status messages, outside of the normal Telemetry stream.  In
addition to the message text, events also contain a numeric event identifier (referred to as an "EventID" or "EID").

As these identifiers serve to associate a human-readable name with the numeric ID for event labeling purposes, an
enumeration is a natural fit for this purpose as well.  The enumeration labels should follow all the same naming
conventions as any other enumeration, described above.

For consistency, an enumeration used for this purpose should be named `EventID`, and the C typedef name for the
enumeration should follow the same pattern of `<APP>_EventID_Enum_t`.

The labels should convey the event name as well as the type of the event, separated by an underscore, in all capital
letters. This should follow the general pattern of `<EVENTNAME>_<EVT>` where "EVT" is a 3-letter abbreviation.

For example:

| Event ID Label             | Application       | Event Description | Event Type    |
| :------------------------: | :---------------: | :---------------: | :-----------: |
| `CFE_EVS_EventID_NOOP_INF` | CFE Event Service | NOOP Command      | Informational |
| `HS_EventID_CR_PIPE_ERR`   | Health & Safety   | Create Pipe Error | Error         |

Common events should be named consistently across applications for ease of recognition.

Common command/event descriptions include:

| Event Name     | Event Description                        | Event Type    |
| :------------: | :--------------------------------------: | :-----------: |
| `MID_ERR`      | Invalid Message ID Received              | Error         |
| `LEN_ERR`      | Incorrect Message Length                 | Error         |
| `CC_ERR`       | Invalid Command Code Received            | Error         |
| `NOOP_INF`     | No-op Command Success                    | Informational |
| `INIT_INF`     | Applicaiton Initialization Success       | Informational |
| `RESET_INF`    | Reset Command Counters Command Success   | Informational |

# Appendix: Abbreviation Guide

In general it is preferable to use entire words unless this would create an identifier which is
unacceptably long.  However, in cases where words do need to be shortened, it is beneficial to
be consistent in the abbreviated term.

__NOTE__: If the original term is 5 characters or less the term should generally __not__
be abbreviated (e.g. use Error, not Err).


  Common Term               Proposed Shortened Term
  ------------------------- ------------------------
  Command                   Cmd
  Telemetry                 Tlm
  Packet                    Pkt
  Diagnostic                Diag
  Address                   Addr
  Message                   Msg
  Count/Counter             Cnt
  Parameter                 Param
  Enable                    Ena
  Disable                   Dis
  Subscription              Subscr
