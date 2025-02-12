//
// Created by arsen on 2/10/25.
//

#include "Util.h"
#include "Core/filespch.h"
#include "AccountManager.h"

/* -------------------------------------------------- Init -------------------------------------------------- */

AccountManager::AccountManager() : current_regular_user_id(0), current_admin_user_id(1)
{
    // Two main groups created - admins and regular users
    const auto admin_group = std::make_shared<Group>(0, "admins");
    const auto regular_group = std::make_shared<Group>(1000, "regular-users");
    groups.emplace_back(admin_group);
    groups.emplace_back(regular_group);

    // Here the admin user is created
    current_user = std::make_shared<User>(0, "admin", "admin");
    current_user->SetPrivileges(true);
    current_user->SetAdmin();
    users.emplace_back(current_user);

    // Mapping of user-group and group-user happens here
    users_of_group["admins"].insert(current_user->GetName());
    groups_of_user[current_user->GetName()].insert("admins");

    users_of_group["regular-users"].insert(current_user->GetName());
    groups_of_user[current_user->GetName()].insert("regular-users");
}

/* -------------------------------------------------- Useradd -------------------------------------------------- */

void AccountManager::HandleUseradd(const size_t starting_index, const size_t argc, std::vector<std::string> &argv)
{
    // Here we check if useradd was passed without any other arguments
    if (starting_index == argc - 1) {
        Log::LOG(LogLevel::ERROR, "User name not specified! Please provide a valid username to create a new user.");
        return;
    }

    // Here we parse the input for the useradd command
    ParseUseradd(starting_index, argc, argv);
}

void AccountManager::ParseUseradd(const size_t starting_index, const size_t argc, std::vector<std::string>& argv)
{
    // The parameters of the user to be added
    bool create_home_directory{false};
    File shell;
    std::vector<std::string> groups_to_be_added;

    for (size_t i = starting_index + 1; i < argc; i++) {
        // If not a flag then it is the username, because flag values are being skipped
        if (argv[i][0] != '-') {
            // If there are other arguments after the username then throw an error
            if (i != argc - 1) {
                Log::LOG(LogLevel::ERROR, "Username must be the last argument. Please provide a valid username.");
                return;
            }

            // If there are no groups then add it to the default-regular-users group
            if (groups_to_be_added.empty()) {
                groups_to_be_added.emplace_back("regular-users");
            }

            // Add the user
            Useradd(argv[i], create_home_directory, shell, groups_to_be_added);
            return;
        } else {
            // Here we are getting the flag
            Util::ToLower(argv[i]);
            const Flag useradd_flag = Util::GetUseraddFlag(argv[i]);

            switch (useradd_flag) {
                case Flag::useradd_m:
                    create_home_directory = true;
                    break;

                case Flag::useradd_s:
                    if (i == argc - 1) {
                        // error shell should be specified
                        std::cout << "no shell scpecified!\n";
                        return;
                    }
                    Util::ToLower(argv[i + 1]);
                    if (!argv[i + 1].starts_with("bin") && !argv[i + 1].starts_with("/bin")) {
                        // the shells are in bin
                        std::cout << "invalid shell scpecified!\n";
                        return;
                    }
                    // Check if shell exists
                    // Assign the shell to the variable shell
                    i++;
                    break;

                case Flag::useradd_g:
                    // If the flag is the last argument then the value is missed
                    if (i == argc - 1) {
                        Log::LOG(LogLevel::ERROR, "No groups specified. Please provide a valid group name.");
                        return;
                    }

                    // Here we call a function to get all the groups we should get in
                    for (auto& group : UseraddGetGroups(argv[i + 1])) {
                        groups_to_be_added.emplace_back(group);
                    }

                    // Skip over the value of the flag
                    i++;
                    break;

                default:
                    // Unknown flag
                    Log::LOG(LogLevel::ERROR, "Flag '"+argv[i]+"' not found. Please check the spelling or refer to the help section.");
                    return;
            }
        }
    }

    // If we got here then no username was specified
    Log::LOG(LogLevel::ERROR, "User name not specified! Please provide a valid username to create a new user.");
}

void AccountManager::Useradd(const std::string &user_name, bool create_home_directory, const File &shell, const std::vector<std::string> &groups_to_be_added)
{
    // If the user already exists then throw an error
    const bool user_exists = std::ranges::any_of(users.begin(), users.end(), [&user_name](const auto& user) {
        return user->GetName() == user_name;
    });

    if (user_exists) {
        Log::LOG(LogLevel::ERROR, "User already exists! Please choose a different username.");
        return;
    }

    // If the user is an admin user then the ID will start from 0, otherwise from 1000
    const bool is_admin_user = std::ranges::any_of(groups_to_be_added.begin(), groups_to_be_added.end(), [](const std::string& group) {
        return group == "admins";
    });

    const size_t id = (is_admin_user ? current_admin_user_id++ : 1000+current_regular_user_id++);

    auto new_user = std::make_shared<User>(id, user_name, "");

    new_user->SetAdmin(is_admin_user);
    new_user->SetPrivileges(true);

    // Add user-group, group-user mapping for the added user
    for (const auto& group : groups_to_be_added) {
        std::string message = user_name;
        message += " added to group: ";
        message += group;
        message += ".";
        Log::LOG(LogLevel::INFO, message);
        groups_of_user[new_user->GetName()].insert(group);
        users_of_group[group].insert(new_user->GetName());
    }

    users.emplace_back(new_user);
}

// Useradd Helpers //

std::vector<std::string> AccountManager::UseraddGetGroups(const std::string& groups_str) const
{
    std::vector<std::string> result;
    std::stringstream ss(groups_str);
    std::string group;

    // Here we check if groups exist then we add it to the user's groups, if not just inform and skip
    while (std::getline(ss, group, ',')) {
        if (users_of_group.contains(group)) {
            result.emplace_back(group);
        } else {
            Log::LOG(LogLevel::ERROR, "Unable to add the user to group '" + group + "' because the group name is invalid.");
        }
    }

    return result;
}

/* -------------------------------------------------- Passwd -------------------------------------------------- */

void AccountManager::HandlePasswd(const size_t starting_index, const size_t argc, std::vector<std::string> &argv)
{
    if (starting_index == argc - 1) {
        Passwd(current_user, AskForNewPassword());
    } else {
        ParsePasswd(starting_index, argc, argv);
    }
}

void AccountManager::ParsePasswd(const size_t starting_index, const size_t argc, std::vector<std::string> &argv)
{
    for (size_t i = starting_index + 1; i < argc; i++) {
        // There cannot be any value without flag in passwd
        if (argv[i][0] != '-') {
            Log::LOG(LogLevel::ERROR, "Invalid argument. The 'passwd' command expects flags and their associated values.");
            return;
        } else {
            Util::ToLower(argv[i]);

            switch (Util::GetPasswdFlag(argv[i])) {
                case Flag::passwd_d:
                    // If just flag is given without value
                    if (i == argc - 1) {
                        Log::LOG(LogLevel::ERROR, "The '-d' flag requires a username. Please specify a valid username after '-d'.");
                        return;
                    }

                    // If there are more things after -d uname
                    if (i + 1 != argc - 1) {
                        Log::LOG(LogLevel::ERROR, "Only one argument is allowed after the '-d' flag. No additional arguments should follow the username.");
                        return;
                    }

                    // Check if the user exists, and if it admin then we should have admin privileges
                    if (const auto it = std::ranges::find_if(users.begin(), users.end(), [&argv, &i](const auto& user) {
                        return argv[i + 1] == user->GetName();
                    }); it != users.end()) {
                        if (argv[i + 1] == "admin" && CurrenUserHasRootPrivileges())
                            Passwd(users[it - users.begin()], "");
                        else
                            Log::LOG(LogLevel::ERROR, "You do not have the required privileges to change the password for the 'admin' user.");
                    } else {
                        Log::LOG(LogLevel::ERROR, "Specified username does not exist. Please provide a valid username.");
                    }
                    return;
                default:
                    Log::LOG(LogLevel::ERROR, "Flag '"+argv[i]+"' not found. Please check the spelling or refer to the help section.");
                    return;
            }
        }
    }

    Passwd(current_user, AskForNewPassword());
}

void AccountManager::Passwd(std::shared_ptr<User>& user, const std::string& password) const
{
    // Change the password, and if we are changing the password of the admin and we are not the admin then revoke the privileges
    user->SetPassword(password);
    if (user->IsAdminUser() && !IsCurrentUserAnAdmin())
        SetCurrentUserPrivileges(false);
}

// Passwd Helpers //

std::string AccountManager::AskForNewPassword() const
{
    // Here we ask for a password
    std::string password;
    std::cout << "[passwd] new password for " << current_user->GetName() << ": ";
    getline(std::cin, password, '\n');
    // TODO: Better Password Checker
    if (std::ranges::any_of(password.begin(), password.end(), [](const char& c) {
        return std::isspace(c);
    })) {
        std::cout << "Please enter a valid password!\n";
        password = current_user->GetPassword();
    }

    return password;
}

/* Switch user with sudo -i */

bool AccountManager::SwitchUser(const std::string &user_name, const bool switch_back)
{
    const auto switch_user = std::ranges::find_if(users.begin(), users.end(), [&user_name](const auto& user) {
        return user->GetName() == user_name;
    });

    if (!switch_back) {
        if (AskForUserPassword((*switch_user)->GetName()) == (*switch_user)->GetPassword()) {
            current_user = *switch_user;
            Log::LOG(LogLevel::INFO, "User successfully switched to '" + user_name + "'.");
            return true;
        }

        Log::LOG(LogLevel::ERROR, "Failed to switch user. Incorrect password for user '" + user_name + "'.");
        return false;
    }

    current_user = *switch_user;
    Log::LOG(LogLevel::INFO, "User successfully switched back to '" + user_name + "'.");
    return true;

}

// Switch User Helpers //

std::string AccountManager::AskForUserPassword(const std::string& username) const
{
    std::string password;
    std::cout << "[sudo] password for " << username << ": ";
    getline(std::cin, password, '\n');
    // TODO: Better Password Checker
    return password;
}


/* Temporary */

void AccountManager::ListUsers()
{
    std::cout << "USERS:\n";
    for (auto& user : users) {
        std::string output = "\t" + user->GetName() + "(" + std::to_string(user->GetID()) + "): ";
        for (auto& group : groups_of_user[user->GetName()]) {
            output += group+",";
        }
        Log::LOG(LogLevel::TRACE, output);
    }

    std::cout << "GROUPS:\n";
    for (auto& group : groups) {
        std::string output = "\t" + group->GetName() + ":";
        for (auto& user : users_of_group[group->GetName()]) {
            output += user+",";
        }
        Log::LOG(LogLevel::TRACE, output);
    }
}

