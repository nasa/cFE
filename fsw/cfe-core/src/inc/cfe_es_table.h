#ifndef cfe_es_table_h
#define cfe_es_table_h

#include "cfe_platform_cfg.h"
#include "osconfig.h"

typedef struct
{
    char Name[OS_MAX_API_NAME]; /**< \cfetlmmnemonic \ES_APPNAME \brief The registered name of the library */
    char FileName[OS_MAX_PATH_LEN]; /**< \cfetlmmnemonic \ES_APPFILENAME \brief The filename of the file containing the library */
    char EntryPoint[OS_MAX_API_NAME]; /**< \cfetlmmnemonic \ES_APPENTRYPT \brief The entry point label for the library */
}
CFE_ES_LibTblEntry_t;

typedef struct
{
    char Name[OS_MAX_API_NAME]; /**< \cfetlmmnemonic \ES_APPNAME \brief The registered name of the application */
    char FileName[OS_MAX_PATH_LEN]; /**< \cfetlmmnemonic \ES_APPFILENAME \brief The filename of the file containing the application */
    char EntryPoint[OS_MAX_API_NAME]; /**< \cfetlmmnemonic \ES_APPENTRYPT \brief The entry point label for the application */
    uint32 StackSize; /**< \cfetlmmnemonic \ES_STACKSIZE */
    uint16 Priority; /**< \cfetlmmnemonic \ES_PRIORITY \brief The priority of the application */
    uint8 ExceptionAction; /**< \cfetlmmnemonic \ES_EXCEPTNACTN \brief What should occur if application has an exception (restart application or restart processor) */
}
CFE_ES_AppTblEntry_t;

typedef struct
{
    /** indicate the end of the libraries with a zero-length Name */
    CFE_ES_LibTblEntry_t Libs[CFE_PLATFORM_ES_MAX_LIBRARIES];

    /** indicate the end of the applications with zero-length Name */
    CFE_ES_AppTblEntry_t Apps[CFE_PLATFORM_ES_MAX_APPLICATIONS];
}
CFE_ES_AppLibTbl_t;

#endif /* cfe_es_table_h */
