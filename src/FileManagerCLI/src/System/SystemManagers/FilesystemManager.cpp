//
// Created by arsen on 2/10/25.
//

#include "FilesystemManager.h"


FilesystemManager::FilesystemManager()
{
    root_directory = std::make_unique<Directory>(0, "root", "/");
    current_directory = std::make_unique<Directory>(1, "admin", "/admin");
}
