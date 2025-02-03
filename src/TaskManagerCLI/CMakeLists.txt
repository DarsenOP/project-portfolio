# Specify the project
project(TaskManagerCLI)

# Define C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the executable for ProjectA
add_executable(TaskManagerCLI main.cpp)

# If ProjectA uses other source files, list them here
# set(SOURCES main.cpp additional_file.cpp)
# add_executable(ProjectA ${SOURCES})

# Link any required external libraries (if applicable)
# target_link_libraries(ProjectA some_library)
