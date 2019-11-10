#!/bin/bash

# change to the build directory
mkdir -p ./build
cd ./build

# generate the build files
# cmake .. -DCMAKE_TOOLCHAIN_FILE=/home/abhishek/code/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake ..

# go back to parent directory
cd ..
