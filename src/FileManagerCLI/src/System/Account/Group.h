//
// Created by arsen on 2/10/25.
//

#ifndef GROUP_H
#define GROUP_H
#include "AccountEntity.h"


class Group : public AccountEntity {
public:
    Group(const size_t id_p, std::string name_p)
        : AccountEntity(id_p, std::move(name_p)) {}

    ~Group() override = default;
    [[nodiscard]] size_t GetID() const override { return id; }
    [[nodiscard]] std::string GetName() const override { return name; }
};



#endif //GROUP_H
