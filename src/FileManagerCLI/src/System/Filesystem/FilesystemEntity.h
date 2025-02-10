//
// Created by arsen on 2/10/25.
//

#ifndef FILESYSTEMENTITY_H
#define FILESYSTEMENTITY_H

#include <string>

class FilesystemEntity {
protected:
    size_t id;
    std::string name;
    std::string path;
public:
    FilesystemEntity(const size_t id_p, std::string name_p, std::string path_p)
        : id(id_p), name(std::move(name_p)), path(std::move(path_p))
    {

    }

    virtual ~FilesystemEntity() = default;
    [[nodiscard]] virtual size_t GetID() const = 0;
    [[nodiscard]] virtual std::string GetName() const = 0;
    [[nodiscard]] virtual std::string GetPath() const = 0;
};



#endif //FILESYSTEMENTITY_H
