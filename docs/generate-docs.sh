#!/bin/bash

pandoc "$1" \
    -f markdown \
    -V linkcolor:blue \
    -V geometry:margin=1in \
    --highlight-style breezedark \
    --syntax-definition=my-c.xml \
    --standalone \
    -o "$2" \
    --lua-filter=fix-code-blocks.lua
