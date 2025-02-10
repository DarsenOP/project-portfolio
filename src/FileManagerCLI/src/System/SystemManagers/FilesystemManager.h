//
// Created by arsen on 2/10/25.
//

#ifndef FILESYSTEMMANAGER_H
#define FILESYSTEMMANAGER_H

#include <memory>
#include "../Filesystem/Directory.h"

class FilesystemManager {
    std::unique_ptr<Directory> root_directory;
    std::unique_ptr<Directory> current_directory;
public:
    FilesystemManager();

    [[nodiscard]] inline bool IsRootDirectory() const
    {
        return current_directory->GetID() == 0;
    }

    [[nodiscard]] inline size_t GetCurrentDirectoryID() const
    {
        return current_directory->GetID();
    }

    [[nodiscard]] inline std::string GetCurrentDirectoryName() const
    {
        return current_directory->GetName();
    }

    [[nodiscard]] inline std::string GetCurrentDirectoryPath() const
    {
        return current_directory->GetPath();
    }
};


#endif //FILESYSTEMMANAGER_H
