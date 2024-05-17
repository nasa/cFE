#include "cfeconfig_platformdata_tool.h"

#include "cfe_config_nametable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <assert.h>
#include <stdbool.h>

#define MAX_INSTANCE_NAME_LEN 64

typedef void (*CFE_ConfigTool_RenderFunc_t)(const char *, const CFE_ConfigTool_DetailBuffer_t *);

/*----------------------------------------------------------------
 *
 * Prints command line usage information
 *
 *-----------------------------------------------------------------*/
void CFE_ConfigTool_PrintUsage(const char *cmd)
{
    fprintf(stderr, "Usage: %s [(entrylist|initcode) platform_name]\n\n", cmd);
    fprintf(stderr, "If executed without any arguments, generate a list of platform names\n");
    fprintf(stderr, "If a platform name is given on command line,\n");
    fprintf(stderr, " initcode:  generates a \"C\" source chunk for that platform\n");
    fprintf(stderr, " entrylist: generate \"C\" macros for valid entries on that platform\n\n");
}

/*----------------------------------------------------------------
 *
 * Renders content structure for a List/Array style platform config entry
 *
 *-----------------------------------------------------------------*/
void CFE_ConfigTool_RenderListContent(const char *prefix, const CFE_ConfigTool_ListDetail_t *detail_ptr)
{
    char entry_string[MAX_INSTANCE_NAME_LEN];
    int  i;
    int  idv;
    int  width;

    bool is_reverse       = detail_ptr->options & CFE_CONFIGTOOL_LIST_DESCENDING;
    bool use_leading_zero = detail_ptr->options & CFE_CONFIGTOOL_USE_LEADING_ZERO;

    width = 1;
    if (use_leading_zero)
    {
        i = detail_ptr->num_entries;
        while (i >= 10)
        {
            i /= 10;
            ++width;
        }
    }

    snprintf(entry_string, sizeof(entry_string), "CFE_CONFIG_%s_CONTENT", prefix);

    printf("static const %s %s[%zu] = {\n", detail_ptr->type_name, entry_string, detail_ptr->num_entries);

    i   = detail_ptr->num_entries;
    idv = 0;
    while (i > 0)
    {
        if (is_reverse)
        {
            idv = i;
        }

        --i;

        if (!is_reverse)
        {
            idv = detail_ptr->num_entries - i;
        }

        if (idv == detail_ptr->num_entries && detail_ptr->custom_max != NULL)
        {
            /* it is the "max" */
            snprintf(entry_string, sizeof(entry_string), "%s", detail_ptr->custom_max);
        }
        else
        {
            snprintf(entry_string, sizeof(entry_string), "CFE_%s_%0*d", prefix, width, idv);
        }

        printf("  %s%c\n", entry_string, (i == 0) ? ' ' : ',');
    }

    printf("};\n\n");
}

/*----------------------------------------------------------------
 *
 * Gets the rendering function for a given detail entry
 * If output should not be rendered at all, returns NULL.
 *
 *-----------------------------------------------------------------*/
CFE_ConfigTool_RenderFunc_t CFE_ConfigTool_GetRenderFunc(const CFE_ConfigTool_DetailEntry_t *cfgid_p)
{
    CFE_ConfigTool_RenderFunc_t result;

    result = NULL;
    if (cfgid_p->render_style == CFE_ConfigTool_RenderStyle_ARRAY)
    {
        /* An array with size of 0 is allowed config-wise but should not be rendered,
         * as it will be a compile error.  Just skip it entirely. */
        if (cfgid_p->u.list.num_entries > 0)
        {
            result = (CFE_ConfigTool_RenderFunc_t)CFE_ConfigTool_RenderListContent;
        }
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Gets the printable name from a config ID offset
 * If the offset is not valid, returns NULL.
 *
 *-----------------------------------------------------------------*/
const char *CFE_ConfigTool_GetNameFromOffset(CFE_ConfigIdOffset_Enum_t offset)
{
    const char *result;

    if (offset > 0 && offset < CFE_ConfigIdOffset_MAX)
    {
        result = CFE_CONFIGID_NAMETABLE[offset].Name;
    }
    else
    {
        result = NULL;
    }

    return result;
}

/*----------------------------------------------------------------
 *
 * Finds the detail list for the given platform name.
 * If the plaform name is not valid, returns NULL.
 *
 *-----------------------------------------------------------------*/
const CFE_ConfigTool_DetailEntry_t *CFE_ConfigTool_FindDetailByName(const char *name)
{
    const CFE_ConfigTool_PlatformMapEntry_t *platform_ptr;
    const CFE_ConfigTool_DetailEntry_t *     result_ptr;

    platform_ptr = CFECONFIG_PLATFORMDATA_TABLE;
    result_ptr   = NULL;
    while (platform_ptr->plat_name != NULL)
    {
        if (strcasecmp(platform_ptr->plat_name, name) == 0)
        {
            result_ptr = platform_ptr->detail_ptr;
            break;
        }

        ++platform_ptr;
    }

    return result_ptr;
}

/*----------------------------------------------------------------
 *
 * Renders the full content structure for a given platform name
 * Returns true if successful, or false if it is not able to be rendered
 *
 *-----------------------------------------------------------------*/
bool CFE_ConfigTool_WritePlatformCfg(const char *plat_name)
{
    const CFE_ConfigTool_DetailEntry_t *detail_ptr;
    CFE_ConfigTool_RenderFunc_t         render_func;

    detail_ptr = CFE_ConfigTool_FindDetailByName(plat_name);
    if (detail_ptr == NULL)
    {
        fprintf(stderr, "%s: platform undefined\n", plat_name);
        return false;
    }

    printf("/* THIS IS GENERATED CONTENT, DO NOT EDIT */\n\n");

    while (detail_ptr->render_style != CFE_ConfigTool_RenderStyle_NONE)
    {
        render_func = CFE_ConfigTool_GetRenderFunc(detail_ptr);

        if (render_func != NULL)
        {
            render_func(CFE_ConfigTool_GetNameFromOffset(detail_ptr->ido), &detail_ptr->u);
        }
        ++detail_ptr;
    }

    return true;
}

/*----------------------------------------------------------------
 *
 * Prints a list of config ID entries that are valid for the given platform
 *
 *-----------------------------------------------------------------*/
bool CFE_ConfigTool_WriteValidEntries(const char *plat_name)
{
    const CFE_ConfigTool_DetailEntry_t *detail_ptr;
    const char *                        cfgname;

    detail_ptr = CFE_ConfigTool_FindDetailByName(plat_name);
    if (detail_ptr == NULL)
    {
        fprintf(stderr, "%s: platform undefined\n", plat_name);
        return false;
    }

    printf("/* THIS IS GENERATED CONTENT, DO NOT EDIT */\n\n");

    /* For the purpose of writing names, just use the first platform entry -
     * they will all be the same in this context, as this only uses names, not values */
    while (detail_ptr->render_style != CFE_ConfigTool_RenderStyle_NONE)
    {
        /*
         * Checking the render function - in that if the content wasn't rendered, the
         * reference to it should not be rendered, either (for example an array with 0 entries).
         */
        if (CFE_ConfigTool_GetRenderFunc(detail_ptr) != NULL)
        {
            cfgname = CFE_ConfigTool_GetNameFromOffset(detail_ptr->ido);
            if (cfgname != NULL)
            {
                printf("CFE_PLATFORMCFG_ENTRY(%s)\n", cfgname);
            }
        }
        ++detail_ptr;
    }

    return true;
}

/*----------------------------------------------------------------
 *
 * Writes the names of all platforms known to the tool
 *
 *-----------------------------------------------------------------*/
void CFE_ConfigTool_WritePlatformNames(void)
{
    const CFE_ConfigTool_PlatformMapEntry_t *platform_ptr;

    platform_ptr = CFECONFIG_PLATFORMDATA_TABLE;
    while (platform_ptr->plat_name != NULL)
    {
        printf("CFE_PLATFORM_NAME(%s)\n", platform_ptr->plat_name);
        ++platform_ptr;
    }
}

/*----------------------------------------------------------------
 *
 * Main routine
 *
 *-----------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    bool success;

    success = false;

    if (argc == 1)
    {
        CFE_ConfigTool_WritePlatformNames();
        success = true;
    }
    else if (argc == 3)
    {
        if (strcasecmp(argv[1], "entrylist") == 0)
        {
            success = CFE_ConfigTool_WriteValidEntries(argv[2]);
        }
        else if (strcasecmp(argv[1], "initcode") == 0)
        {
            success = CFE_ConfigTool_WritePlatformCfg(argv[2]);
        }
    }

    if (!success)
    {
        CFE_ConfigTool_PrintUsage(argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}