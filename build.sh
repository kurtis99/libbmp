#!/bin/sh

set -x

PWD=`pwd`

rm -rf build
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=0 ../
#cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=1 ../
make

cd $PWD

cd build/test
ctest --output-on-failure .

cd $PWD
cd build/src

lcov --zerocounters --directory .
lcov --capture --initial --directory . --output-file test_bmp
../../output_test/test_bmp
lcov --no-checksum --directory . --capture --output-file test_bmp.info
rm -r $PWD/lcov_result
genhtml -O $PWD/lcov_result test_bmp.info

cd $PWD
pwd
