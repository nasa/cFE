# Core Flight Software Contributor Style Guide

The maintainers of the Core Flight Software (cFS) codebase recommend coders follow these guidelines in how to format their code,
as well as the naming standards to follow for functions, variables, macros, and filenames.

Do *not* change the style of existing code unless you are altering the functionality of that code.
While GIT is fairly capable of ignoring whitespace changes, it is not perfect and other tools are less
capable.

## TODO: merge in/replace the IdentifierNamingConvention

## TODO: Decide...

1. PipeId vs PipeID

2. "Out" parameters on the left? -- GetPipeName(char *buf, PipeID_t PipeID)

3. Modify/clear "out" parameters on error? -- call to GetPipeName(char *buf, PipeID_t PipeID) clears buf even if it errs out.

4. "Out" array buffers, always include size of buffer? (Should it be GetPipeName(char *buf, size_t buf_sz, PipeId_t PipeId)?)
4.1. Should the buffer size immediately follow the out buffer or be at the end? - GetPipeName(char *buf, size_t buf_sz, PipeId_t PipeId) or GetPipeName(char *buf, PipeId_t PipeId, size_t buf_sz)

5. Type on left of name? (PipeGetName() vs GetPipeName()?)

## Code Formatting Guidance

The vast bulk of cFS is written in C, and the following guidelines should be followed. Currently we recommend you use
the [GNU Indent](https://www.gnu.org/software/indent/manual/) command with the following options:

    % indent --blank-lines-after-declarations --blank-lines-after-procedures --braces-after-func-def-line --brace-indent 0 --braces-after-struct-decl-line --format-first-column-comments --format-all-comments --indent-level 4 --line-length 120 --preprocessor-indentation 3 --no-space-after-casts --no-space-after-function-call-names --dont-break-procedure-type --no-tabs --comment-line-length 80 --swallow-optional-blank-lines <file.c>

### Details of Indent Options

    --blank-lines-after-declarations

A blank line is added after each block of declarations.

    --blank-lines-after-procedures
    
A blank line is added after each function.

    --braces-after-func-def-line

The opening curly brace is on the line after the function definition.

    --brace-indent 0

Opening curly brace is aligned with the line above.

    --braces-after-struct-decl-line

Opening curly brace for struct on the following line (as with curly braces for function.

    --format-first-column-comments
    --format-all-comments

Apply formatting to all comments.

    --indent-level 4

Indent nested blocks of code/structs 4 spaces per level.

--line-length 120

Lines should not be more than 120 characters long.

    --preprocessor-indentation 3

Indent nested preprocessor blocks 3 spaces for each level.

    --no-space-after-casts

Type casts should not have a space between the type and the variable.

    --no-space-after-function-call-names

No space between the function name and the opening parenthesis.

    --dont-break-procedure-type

The return type of a function should be on the same line as the function name.

    --no-tabs

Don't use tabs.

    --comment-line-length 80

Limit comment lines to 80 characters width.

    --swallow-optional-blank-lines

Remove excessive blank lines.

#### Example Code Following the Above Formatting Guidelines

    #include <stdio.h>
    
    /* 
     * This is a simple example file.
     */
    char *message = "Hello world!";
    
    #if A
    #   if B
    #      define A_AND_B
    #   else
    #      define A_NOT_B
    #   endif
    #endif
    
    int dummy_arr[10];
    
    struct
    {
        int a;
        void *b;
    } dum_struct;
    
    int fn(int p)
    {                               /* simple fn */
        printf("%s\n", message);
        switch (p)
        {
        case 0:
            printf("Zero\n");
            break;
        case 1:
            {
                printf("One\n");
            }
            break;
        default:
            print("Many\n");
        }
    }
    
    int main(int c, char **v)
    {
        if (c > 1)
        {
            printf("%d args\n", c);
        }
        else
        {
            printf("no args\n");
        }
    
        fn(c);
    }
