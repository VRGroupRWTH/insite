#!/bin/bash
source /nest-install/bin/nest_vars.sh
export LD_LIBRARY_PATH=$NEST_MODULE_PATH:$LD_LIBRARY_PATH
python3 /insite/examples/sim2.py