#!/bin/bash
source /mnt/d/Code/nest-simulator/build/linux-gcc7/install/bin/nest_vars.sh
export LD_LIBRARY_PATH=$NEST_MODULE_PATH:$LD_LIBRARY_PATH
python3 examples/sim2.py