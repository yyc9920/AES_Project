#!/bin/sh
# script to launch application for vv_launcher
#

echo run cover_flow
SDK_ROOT=

cd ../../..
SDK_ROOT=`pwd`
export LD_LIBRARY_PATH=$SDK_ROOT/drivers

cd $SDK_ROOT/samples/vdk
./cover_flow
cd $SDK_ROOT/samples/es20/vv_launcher

