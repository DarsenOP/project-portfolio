//
// Created by DarsenOP on 2/8/25.
//

#ifndef MANAGER_H
#define MANAGER_H

#include <memory>
#include <vector>
#include <string>
#include "Enumerators/CommandsEnum.h"
#include "System/SystemManagers/AccountManager.h"
#include "System/SystemManagers/FilesystemManager.h"

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

    /* Main Loop */
    void Start();

    /* Command Handling */
    bool HandleCommand(size_t argc, std::vector<std::string>& argv);
    bool ExecuteCommand(const Command& command, size_t command_index, size_t argc, std::vector<std::string>& argv) const;

    /* Sudo */
    bool Sudo(size_t argc, std::vector<std::string>& argv);
    [[nodiscard]] bool PromptForAdminPassword(bool give_sudo=true) const;
    void HandleSudoVFlag() const;
    void HandleSudoKFlag() const;
    bool HandleSudoUFlag(const std::string& username, size_t command_index, size_t argc, std::vector<std::string>& argv);
    void HandleSudoIFlag(const std::string& username) const;
    static void SudoHelp();
};

#endif //MANAGER_H
