#!/bin/bash

# Default configuration
configuration="Release"

# Process command-line arguments
while getopts ":c:" opt; do
  case $opt in
    c)
      configuration=$OPTARG
      echo "       TARGET: $configuration" >&2
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
    :)
      echo "Option -$OPTARG requires an argument." >&2
      exit 1
      ;;
  esac
done

directory="build"

# Check if the directory exists
if [ ! -d "$directory" ]; then
    mkdir -p "$directory" || exit 1
fi

# Generate Makefiles using premake4 for the specified configuration
premake4 gmake --config="$configuration"
cd build
make

# Check if the make command was successful
if [ $? -eq 0 ]; then
    # Run the compiled binary for the specified configuration
    ./bin_linux/Wolfenstein_TG  # Adjust the path based on your project structure
else
    echo "Build failed. Execution aborted."
    exit 1
fi

