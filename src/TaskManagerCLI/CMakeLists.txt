# Specify the project
project(TaskManagerCLI)

# Define C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable for ProjectA
 set(SOURCES main.cpp Manager.cpp Manager.h Tasks.h
         taskpch.h)
 add_executable(TaskManagerCLI ${SOURCES})

# Link any required external libraries (if applicable)
# target_link_libraries(ProjectA some_library)
find_package(nlohmann_json REQUIRED)
target_link_libraries(TaskManagerCLI PRIVATE nlohmann_json::nlohmann_json)