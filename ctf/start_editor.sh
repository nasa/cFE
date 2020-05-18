#!/bin/bash

source activate\_ctf\_env.sh

python3 installPlugins.py

cd ../../../cfs_gw/tools/ctf/tools/ctf_ui
npm start &

