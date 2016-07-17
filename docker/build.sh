#!/bin/bash

cd /qsl
test -d build && rm -rf build
mkdir build
cd build
cmake -DCMAKE_INSTALL_PREFIX=/usr ..
make doc install

for dir in ../examples/*
do
    if [ -d "$dir" ]
    then
	pushd $dir
	cmake .
	make
	popd
    fi
done
