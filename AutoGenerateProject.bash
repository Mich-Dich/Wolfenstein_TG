#!/bin/bash

echo "=================================== Building Project ==================================================="
echo "starting to build project in [./build] folder"
echo " "

# ANSI color codes
RED='\x1B[38;2;255;30;30m'
GREEN='\x1B[38;2;20;255;20m'
BLUE='\x1B[38;2;120;120;255m'
NC='\033[0m' # No Color

# Default configuration
configuration="Release"

# Process command-line arguments
while getopts ":c:" opt; do
  case $opt in
    c)
      configuration=$OPTARG
      echo -e "[${BLUE}bash${NC}] TARGET: $configuration" >&2
      ;;
    \?)
      echo -e "[${BLUE}bash${NC}] Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo -e "[${BLUE}bash${NC}] Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

directory="build"

# Check if the directory exists
if [ ! -d "$directory" ]; then
    mkdir -p "$directory" || exit 1
    echo -e "[${BLUE}bash${NC}] ./$directory not fount => creating directory"
else
    echo -e "[${BLUE}bash${NC}] ./$directory fount"
fi

echo -e "[${BLUE}bash${NC}] executing premake"
echo " "
# Generate Makefiles using premake4 for the specified configuration
premake4 gmake --config="$configuration"

echo " "
echo "- - - - - - - - - - - - Finished [premake4] command  ==>  starting [make] command - - - - - - - - - - -"
echo " "

cd build
echo -e "[${BLUE}bash${NC}] executing make"
make

# Check if the make command was successful
if [ $? -eq 0 ]; then
    # Run the compiled binary for the specified configuration
    ./bin/Wolfenstein_TG  # Adjust the path based on your project structure
else
    echo -e "${RED}Build failed${NC} => Execution aborted."
fi

echo "===================================  Finished Build  ==================================================="
