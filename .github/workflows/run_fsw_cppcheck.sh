#!/bin/bash

cppcheck_common_opts="--force --inline-suppr --std=c99 --language=c --enable=warning,performance,portability,style --suppress=variableScope --inconclusive"

# When checking time, only the "server" config option is enabled for now.
# Otherwise cppcheck attempts to check with both branches enabled and generates false errors
cppcheck_time_opts="-UCFE_PLATFORM_TIME_CFG_CLIENT -DCFE_PLATFORM_TIME_CFG_SERVER"

for mod in ${*}
do
    cppcheck ${cppcheck_common_opts} $(eval echo \$cppcheck_${mod}_opts) ./modules/${mod}/fsw
done

