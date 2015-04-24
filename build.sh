#!/bin/sh

#set -x

MYPWD=`pwd`

rm -rf build
mkdir -p build
cd build

cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_VERBOSE_MAKEFILE=0 ../
#cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_VERBOSE_MAKEFILE=1 ../
make

cd $MYPWD

cd build/test
ctest --output-on-failure .

cd $MYPWD
cd build/src

lcov --zerocounters --directory . > lcov.log
lcov --capture --initial --directory . --output-file test_bmp >> lcov.log
../../output_test/test_bmp >/dev/null
lcov --no-checksum --directory . --capture --output-file test_bmp.info >> lcov.log
rm -r $MYPWD/lcov_result
genhtml -O $MYPWD/lcov_result test_bmp.info

cd $MYPWD
pwd
