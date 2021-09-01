# Mini-HOWTO guide for CFS application static linkage

For some CFE deployments there are good reasons to _not_ use a dynamic loader 
for applications, and instead link CFE plus all applications into a single 
executable. This would typically be a "minimalist" target using an RTOS that 
does not include dynamic loading, or a safety-critical subsystem where dynamic 
loading is avoided due to the operational variables it introduces.


## Background and Prerequisites

Although a similar mode of operation had been possible in previous versions of 
CFE, it required some modification to the application and some special compiler 
macros in order to work.  With the recent updates to OSAL it is possible to do 
this in a much cleaner fashion, and no changes to the application should be 
required.  Furthermore, with the new and improved approach, applications are 
initialized in the correct (startup script) order as well as allowing for 
mixing dynamically- and statically- linked applications if desired.

The issue with static applications occurs at loading time, where the ES startup 
calls `OS_ModuleLoad()` to dynamically load an application file followed by 
`OS_SymbolLookup()` to locate that application's entry point.  The previous 
methods basically bypassed this path, and created special loading logic that 
directly called the application entry point.

This takes a different approach and rather than bypassing the OS module loading 
and symbol lookup process, it utilizes some recent OSAL changes to effectively 
allow the same general procedure to work for both dynamic and statically linked 
applications.  The latest version of OSAL (with the structured implementation) 
is required for this functionality to work.  

This OSAL update adds a configuration for a local static symbol list, such that 
calls to `OS_ModuleLoad()` and `OS_SymbolLookup()` can succeed and return a 
correct address.  This works even if there is no operating system / C library 
support for dynamic loading and symbol lookup, provided that the application 
developer has provided a list of module names and symbol names which are 
"statically linked" into the binary.

With these changes, the same CFE application loading logic can 
transparently instantiate both dynamic and statically linked applications.


## OSAL configuration changes

The only necessary `osconfig.h` directive is:

    #define OS_STATIC_LOADER

With this directive enabled, OSAL will reference an additional application- 
specific symbol table in its logic as part of the `OS_ModuleLoad()` and 
`OS_SymbolLookup()` calls.  The CFE build system will generate this table to 
include those symbols specified in the configuration (see below).

Optionally, if all applications will be statically linked and no dynamic 
loading is used at all, these two directives can also be configured:

    #undef OS_INCLUDE_MODULE_LOADER
    #define OS_MAX_MODULES 0

This will "compile-out" the dynamic loading logic entirely resulting in a 
smaller OSAL size.

**NOTE**: In such a configuration that uses no dynamic loading at all, it may 
be possible to additionally use the `-fvisibility=hidden` GCC compiler switch 
such that the resulting binaries become even smaller.  This saves about 50kb 
from a basic CFE build at the time of this writing (about 10% reduction in size 
compared to a build without this switch).


## CFE Mission Configuration changes

There are two mission configuration changes that can be specified in the 
`targets.cmake` file that allow for static linkage:

1. Set `TGT<x>_STATIC_APPLIST` instead of `TGT<x>_APPLIST`

This is the variable that lists the applications included in the build for each 
CPU.  The new STATIC variant works the same way except that it will generate a 
static library instead of a shared library, and it also includes the resulting 
application libraries with the CFE during the final link stage.

2. Set `TGT<x>_STATIC_SYMLIST`

This is a new directive.  This is a list of symbols that is directly translated 
to the generated `OS_STATIC_SYMBOL_TABLE` that is in turn used by the static 
loader feature of OSAL.  Symbols specified in this list will be known to 
`OS_SymbolLookup()` without any involvement from the underlying C library.  
This allows symbol lookups to succeed even on systems that do not have any 
dynamic lookup capabilities.

Each entry should be a comma-separated pair indicating a symbol name and the 
module name that would define it in a dynamically-linked environment.

**NOTE**: To be more portable this operates on the abstract module name as 
opposed to the file name.  This corresponds to the 4th parameter on each line 
of the ES startup script file, _not_ the 2nd parameter.

### Example CFE mission configuration

To link the "sample_app" application statically, the following two lines are 
used in `targets.cmake`:

    set(TGT1_STATIC_APPLIST sample_app)
    set(TGT1_STATIC_SYMLIST SAMPLE_AppMain,SAMPLE_APP)

The `TGT1_STATIC_APPLIST` simply causes the application code to be built as a 
static library instead of a shared library.  The `TGT1_STATIC_SYMLIST` directive 
will generate a runtime lookup table for OSAL containing the address of the 
`SAMPLE_AppMain` symbol that is defined in a "virtual" module called 
`SAMPLE_APP`.  The effect of this second line is two fold:

- A call to `OS_ModuleLoad()` with the name `SAMPLE_APP` always returns 
successfully without doing any loading, because it is assumed the module is 
statically linked, and therefore already effectively loaded into memory.
- A call to `OS_SymbolLookup()` with the name `SAMPLE_AppMain` will return the 
correct address for this symbol.  This lookup is done directly in OSAL without 
calling the C library lookup function.
