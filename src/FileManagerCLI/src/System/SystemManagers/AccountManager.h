//
// Created by arsen on 2/10/25.
//

#ifndef ACCOUNTMANAGER_H
#define ACCOUNTMANAGER_H

#include <memory>
#include <unordered_set>
#include <vector>
#include "Logging/Log.h"
#include "System/Account/User.h"
#include "System/Account/Group.h"
#include "System/Filesystem/File.h"

class AccountManager {
    std::unordered_map<std::string, std::unordered_set<std::string>> groups_of_user;
    std::unordered_map<std::string, std::unordered_set<std::string>> users_of_group;

    size_t current_admin_user_id;
    size_t current_regular_user_id;
    std::vector<std::shared_ptr<User>> users;
    std::vector<std::shared_ptr<Group>> groups;
    std::shared_ptr<User> current_user;
public:
    AccountManager();

    [[nodiscard]] bool IfAdminUser() const { return current_user->GetID() == 0; }

    /* Checkers */
    [[nodiscard]] bool CurrenUserHasRootPrivileges() const { return current_user->HasRootPrivileges(); }
    [[nodiscard]] bool IsCurrentUserAnAdmin() const { return current_user->IsAdminUser(); }
    [[nodiscard]] bool UserExists(const std::string& user_name) const
    {
        return std::ranges::any_of(users.begin(), users.end(), [&](std::shared_ptr<User> user) {
           return user->GetName() == user_name;
        });
    }

    /* Getters */
    [[nodiscard]] size_t GetCurrentUserID() const { return current_user->GetID(); }
    [[nodiscard]] std::string GetCurrentUserName() const { return current_user->GetName(); }

    [[nodiscard]] std::string GetAdminPassword() const { return users[0]->GetPassword(); }

    /* Setters */
    void SetCurrentUserPrivileges(const bool value) const { current_user->SetPrivileges(value); }

    /* Add User */
    void HandleUseradd(size_t starting_index, size_t argc, std::vector<std::string>& argv);
    void ParseUseradd(size_t starting_index, size_t argc, std::vector<std::string>& argv);
    void Useradd(const std::string& user_name, bool create_home_directory, const File& shell, const std::vector<std::string>& groups_to_be_added);
    std::vector<std::string> UseraddGetGroups(const std::string& groups_str) const;

    /* Passwd */
    void HandlePasswd(size_t starting_index, size_t argc, std::vector<std::string>& argv);
    void ParsePasswd(size_t starting_index, size_t argc, std::vector<std::string>& argv);
    void Passwd(std::shared_ptr<User>& user, const std::string& password) const;
    std::string AskForNewPassword() const;

    /* Switch user with sudo -i */
    bool SwitchUser(const std::string& user_name, bool switch_back=false);
    std::string AskForUserPassword(const std::string& username) const;

    /* Temporary */
    void ListUsers();
};



#endif //ACCOUNTMANAGER_H
