#!/bin/sh
source /Users/simon/Code/nest-simulator/build/macos-gcc9.2/install/bin/nest_vars.sh
export DYLD_LIBRARY_PATH=$NEST_MODULE_PATH:$DYLD_LIBRARY_PATH
# export NEST_MODULE_PATH=/Users/simon/Code/nest-simulator/build/macos-gcc9.2/install/lib/nest:$NEST_MODULE_PATH
# export SLI_PATH=/Users/simon/Code/nest-simulator/build/macos-gcc9.2/install/share/nest/sli:$SLI_PATH
python3 examples/sim2.py