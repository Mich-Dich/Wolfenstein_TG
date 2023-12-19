#!/bin/bash

directory="build"

# Check if the directory exists
if [ -d "$directory" ]; then
    mkdir -p "$directory" || exit 1
fi

# Rest of your script...
premake4 gmake --zo=./build

cd build
make

exit
