//
// Created by arsen on 2/10/25.
//

#ifndef USER_H
#define USER_H

#include "AccountEntity.h"
#include "../Filesystem/Directory.h"

class User : public AccountEntity {
    bool has_root_privileges = false;
    std::string password;
public:
    User(const size_t user_id, std::string user_name, std::string user_password)
        : AccountEntity(user_id, std::move(user_name)), password(std::move(user_password))
    {
    }

    ~User() override = default;

    /* Getters */
    [[nodiscard]] inline size_t GetID() const override { return id; }
    [[nodiscard]] inline std::string GetName() const override { return name; }
    [[nodiscard]] inline bool HasRootPrivileges() const { return has_root_privileges; }

    /* Setters */
    inline void SetPrivileges(const bool has_privileges) { has_root_privileges = has_privileges; }
};



#endif //USER_H
