#!/bin/sh

export BASE_DIR=$PWD
export CFLAGS="-DLINUX -Wno-implicit-function-declaration -Wno-format"
export ARFLAGS="rcs"
export LDFLAGS="-lrt"
export CC=gcc

echo "--- generando libreria arqtraza ---"
cd $BASE_DIR/libs/arqtraza
make clean all

echo "--- generando libreria tcputil ---"
cd $BASE_DIR/libs/tcputil
make clean all

echo "--- generando supervisor ---"
cd $BASE_DIR/supervisor
make clean all
