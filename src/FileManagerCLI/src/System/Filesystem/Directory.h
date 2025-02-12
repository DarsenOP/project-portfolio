//
// Created by arsen on 2/10/25.
//

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include "FilesystemEntity.h"


class Directory : public FilesystemEntity {
public:
    Directory(const size_t id_p, std::string name_p, std::string path_p)
        : FilesystemEntity(id_p, std::move(name_p), std::move(path_p))
    {

    }

    ~Directory() override = default;
    [[nodiscard]] inline size_t GetID() const override { return id; }
    [[nodiscard]] inline std::string GetName() const override { return name; }
    [[nodiscard]] inline std::string GetPath() const override { return path; }

};



#endif //DIRECTORY_H
