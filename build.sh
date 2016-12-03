#!/bin/bash

cd "`dirname "$0"`"

rekln()
{
    if [ -d "$1" ]; then
	test -d /"$1" || sudo mkdir -p /"$1"
	for file in "$1"/*; do
	    rekln "$file"
	done
    else
	test -e /"$1" || sudo ln -s "$PWD"/"$1" /"$1"
    fi
}

test -d build || mkdir -p build
cd build
cmake -DCMAKE_BUILD_TYPE=Debug .. || exit 1
make -j7 all || exit 1
test -d root || mkdir -p root
fakeroot make install DESTDIR=root || exit 1
cd root
rekln .
cd ..
make -j7 test || exit 1
