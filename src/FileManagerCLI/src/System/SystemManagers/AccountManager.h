//
// Created by arsen on 2/10/25.
//

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <memory>
#include <vector>
#include "../Account/User.h"
#include "../Account/Group.h"
#include "../Filesystem/File.h"

class AccountManager {
    size_t regular_user_count;
    std::vector<User> users;
    std::vector<Group> groups;
    std::unique_ptr<User> current_user;
public:
    AccountManager();

    [[nodiscard]] bool IfAdminUser() const { return current_user->GetID() == 0; }

    /* Getters */
    [[nodiscard]] size_t GetCurrentUserID() const { return current_user->GetID(); }
    [[nodiscard]] std::string GetCurrentUserName() const { return current_user->GetName(); }
    [[nodiscard]] bool CurrenUserHasRootPrivileges() const { return current_user->HasRootPrivileges(); }

    /* Setters */
    void SetCurrentUserPrivileges(const bool value) { current_user->SetPrivileges(value); }

    bool ParseAdduser(size_t starting_index, size_t argc, std::vector<std::string>& argv,
                      std::string& user_name, bool& create_home_directory, File& shell);
    void AddUser(const std::string& user_name, bool create_home_directory, const File& shell);
};



#endif //ACCOUNTMANAGER_H
