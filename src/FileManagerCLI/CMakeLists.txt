# Specify the project
project(FileManagerCLI)

# Define C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
set(SOURCES src/main.cpp
        src/Manager/Manager.h
        src/Manager/Manager.cpp
        src/Core/filespch.h
        src/Core/filespch.cpp
        src/Logging/Log.cpp
        src/Logging/Log.h
        src/Core/Core.h
        src/CommandProcessor/CommandProcessor.h
        src/Exceptions/InputParsing/FileManagerException.h
        src/Exceptions/InputParsing/EmptyInputException.h
        src/CommandProcessor/CommandProcessor.cpp
        src/Exceptions/InputParsing/EmptyQuotedArgumentException.h
        src/Exceptions/InputParsing/NonIsolatedQuotedArgumentException.h
        src/Exceptions/InputParsing/UnpairedQuoteException.h
        src/Enumerators/LogEnums.h
        src/Enumerators/SystemEnums.h
        src/System/SystemManagers/AccountManager.cpp
        src/System/SystemManagers/AccountManager.h
        src/System/SystemManagers/FilesystemManager.cpp
        src/System/SystemManagers/FilesystemManager.h
        src/System/Account/AccountEntity.cpp
        src/System/Account/AccountEntity.h
        src/System/Filesystem/FilesystemEntity.cpp
        src/System/Filesystem/FilesystemEntity.h
        src/System/Filesystem/Directory.cpp
        src/System/Filesystem/Directory.h
        src/System/Account/User.cpp
        src/System/Account/User.h
        src/System/Account/Group.cpp
        src/System/Account/Group.h
        src/System/Filesystem/File.cpp
        src/System/Filesystem/File.h
        src/Enumerators/CommandsEnum.h
)
add_executable(FileManagerCLI ${SOURCES})

# Enable precompiled headers
target_precompile_headers(FileManagerCLI PRIVATE src/Core/filespch.h)

# Link spdlog library
add_subdirectory(external/spdlog)
target_link_libraries(FileManagerCLI PRIVATE spdlog::spdlog)
