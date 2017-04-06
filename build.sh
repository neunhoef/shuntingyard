#!/bin/bash
if [[ ! -d ./build ]] ; then
    mkdir build
    cd build
    ccmake ..
    cd ..
fi
cd build
make $@
