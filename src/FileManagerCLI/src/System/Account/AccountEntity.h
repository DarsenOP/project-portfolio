//
// Created by arsen on 2/10/25.
//

#ifndef ACCOUNTENTITY_H
#define ACCOUNTENTITY_H

#include <string>

class AccountEntity {
protected:
    size_t id;
    std::string name;
public:
    AccountEntity(const size_t id_p, std::string name_p)
        : id(id_p), name(std::move(name_p)) {}

    virtual ~AccountEntity() = default;
    [[nodiscard]] virtual size_t GetID() const = 0;
    [[nodiscard]] virtual std::string GetName() const = 0;
};



#endif //ACCOUNTENTITY_H
