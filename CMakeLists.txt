# Set the minimum version of CMake required
cmake_minimum_required(VERSION 3.10)

# Define the project name
project(project_portfolio)

# Set the C++ standard to C++17 (or change this as needed)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add subdirectories for each project
add_subdirectory(src/TaskManagerCLI)
# Add more projects as needed