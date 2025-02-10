//
// Created by arsen on 2/10/25.
//

#include "AccountManager.h"

AccountManager::AccountManager() : regular_user_count(0)
{
    current_user = std::make_unique<User>(0, "admin", "admin");
    groups.push_back(Group(0, "admins"));
    groups.push_back(Group(1000, "regular-users"));
    users.push_back(*current_user);
}

bool AccountManager::ParseAdduser(const size_t starting_index, const size_t argc, std::vector<std::string>& argv,
                                  std::string &user_name, bool &create_home_directory, File &shell)
{
    if (starting_index == argc) {
        // Error: trying to add a user without username
        return false;
    }

    for (size_t i = starting_index; i < argc; i++) {
        if (argv[i][0] == '-') {

        } else {

        }
    }
}


void AccountManager::AddUser(const std::string &user_name, bool create_home_directory, const File &shell)
{

}
