#!/bin/sh

set -x

rm -rf build
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=1 ../
make

cd ..
