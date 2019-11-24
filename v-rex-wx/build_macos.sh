#!/bin/bash

# change to the build directory
mkdir -p ./build
cd ./build

# generate the build files
cmake .. -DCMAKE_TOOLCHAIN_FILE=/Users/abhishekmishra/vcpkg/scripts/buildsystems/vcpkg.cmake -DENABLE_TESTS=On -DENABLE_LUA=On -DwxWidgets_CONFIG_EXECUTABLE=/usr/local/bin/wx-config

# build
make clean all

# go back to parent directory
cd ..
