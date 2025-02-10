//
// Created by DarsenOP on 2/8/25.
//

#ifndef MANAGER_H
#define MANAGER_H

#include <memory>
#include <vector>
#include <string>

#include "../Enumerators/CommandsEnum.h"
#include "../System/SystemManagers/AccountManager.h"
#include "../System/SystemManagers/FilesystemManager.h"

class Manager
{
    std::unique_ptr<AccountManager> account_manager;
    std::unique_ptr<FilesystemManager> filesystem_manager;

    Manager()
        : account_manager(std::make_unique<AccountManager>()),
          filesystem_manager(std::make_unique<FilesystemManager>()) {}

    ~Manager() = default;
public:
    static Manager& GetInstance();

    Manager(const Manager&) = delete;
    Manager& operator=(const Manager&) = delete;

    /* Getters */
    static Command GetCommand(const std::string& command_str);

    /* Helpers */
    static void ToLower(std::string& str);

    void Start();
    void HandleCommand(size_t argc, std::vector<std::string>& argv);
    void ExecuteCommand(const Command& command, size_t command_index, size_t argc, std::vector<std::string>& argv);

    /* Sudo */
    void Sudo(size_t argc, std::vector<std::string>& argv);

    /* Helpers For Sudo */
    [[nodiscard]] bool GiveCurrentUserRootPrivileges() const;
    void HandleSudoVFlag() const;
    void HandleSudoUFlag(const std::string& username, size_t command_index, size_t argc, std::vector<std::string>& argv);
    void SudoHelp() const;
};

#endif //MANAGER_H
