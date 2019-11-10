#!/bin/bash

# change to the build directory
mkdir -p ./build
cd ./build

export PKG_CONFIG_PATH=/usr/local/Cellar/libarchive/3.4.0/lib/pkgconfig:/usr/local/Cellar/json-c/0.13.1/lib/pkgconfig:

# generate the build files
cmake -G "Unix Makefiles" .. -DwxWidgets_CONFIG_EXECUTABLE=/usr/local/Cellar/wxmac/3.0.4_2/bin/wx-config

# go back to parent directory
cd ..
