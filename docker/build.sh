#!/bin/bash

cd /qsl
test -d build && rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make doc install
cd ../examples/simple
cmake .
make
