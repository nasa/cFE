#!/bin/sh
#
# Enforces a standard style recursively from current location
#

find . -name "*.[ch]" -exec \
  indent --blank-lines-after-declarations --blank-lines-after-procedures \
        --braces-after-func-def-line --brace-indent 0 \
        --braces-after-struct-decl-line --case-indentation 4 \
        --honour-newlines --indent-level 4 --declaration-indentation 16 \
        --line-length 120 \
        --preprocessor-indentation 3 --no-space-after-casts \
        --no-space-after-function-call-names --dont-break-procedure-type \
        --no-tabs --comment-line-length 80 --swallow-optional-blank-lines \
        {} +

