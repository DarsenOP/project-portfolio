#!/bin/bash

# Navigate to the root of the project (relative path from tasks.sh to project root)
cd ~/Documents/dev/project-portfolio/  # Go up to the project root

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Run CMake to configure the project (silent output)
cmake .. > /dev/null

# Build the project using make (silent output)
make > /dev/null

# Run the 'tasks' executable (the symlink you created)
~/Documents/dev/project-portfolio/build/src/FileManagerCLI/FileManagerCLI "$@"  # Pass all arguments to the tasks executable
