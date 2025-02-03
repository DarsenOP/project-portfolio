#!/bin/bash

# Navigate to the root of the project (relative path from tasks.sh to project root)
cd "$(dirname "$0")"  # Navigate to where tasks.sh is located (TaskManagerCLI folder)
cd ../..  # Go up to the project root

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Run CMake to configure the project (silent output)
cmake .. > /dev/null

# Build the project using make (silent output)
make > /dev/null

# Run the 'tasks' executable (the symlink you created)
../tasks "$@"  # Pass all arguments to the tasks executable
