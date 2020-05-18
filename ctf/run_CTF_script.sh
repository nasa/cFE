#!/bin/bash

source activate\_ctf\_env.sh

python3 installPlugins.py

../../../cfs_gw/tools/ctf/ctf --config ReqValidation_config.ini ./scripts/$1
