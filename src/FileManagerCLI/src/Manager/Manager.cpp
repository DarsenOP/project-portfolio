//
// Created by DarsenOP on 2/8/25.
//

#include "Manager.h"
#include "../Core/Core.h"
#include "../CommandProcessor/CommandProcessor.h"

Manager& Manager::GetInstance()
{
    static Manager _instance;
    return _instance;
}

void Manager::Start()
{
    size_t empty_input_count{};

    while (true) {
        std::string input;

        std::cout << "#" << account_manager->GetCurrentUserName() << ":~> ";
        std::getline(std::cin, input, '\n');

        CommandProcessor processor;
        processor.ParseCommand(input);

        if (processor.IsValid()) {
            empty_input_count = 0;
            std::vector<std::string> args = processor.GetResult();
            const size_t arg_count = processor.GetParsedResultWordCount();

            HandleCommand(arg_count, args);
        } else {
            const auto exception = processor.GetException();

            if (std::dynamic_pointer_cast<EmptyInputException>(exception) && ++empty_input_count < 3) {
                continue;
            }
            Log::LOG(exception->GetLogLevel(), exception->what());
            empty_input_count = 0;
        }
    }
}

void Manager::HandleCommand(const size_t argc, std::vector<std::string>& argv)
{
    ToLower(argv[0]);
    const Command first_command = GetCommand(argv[0]);

    if (first_command == Command::sudo) {
        if (argc == 1) {
            SudoHelp();
            return;
        }

        Sudo(argc, argv);
    } else {
        ExecuteCommand(first_command, 0, argc, argv);
    }
}

void Manager::ExecuteCommand(const Command &command, const size_t command_index, const size_t argc, std::vector<std::string>& argv)
{

}


/* Getters */

Command Manager::GetCommand(const std::string &command_str)
{
    if (command_str == "sudo")      return Command::sudo;
    if (command_str == "useradd")   return Command::useradd;
    if (command_str == "passwd")    return Command::passwd;

    return Command::none;
}

/* Helpers */

void Manager::ToLower(std::string &str)
{
    std::ranges::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

/* Sudo Handling */

void Manager::Sudo(const size_t argc, std::vector<std::string> &argv)
{
    for (size_t i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            ToLower(argv[i]);
            const Command command = GetCommand(argv[i]);

            if (command == Command::none) {
                Log::LOG(LogLevel::ERROR, "Command not found");
                return;
            }

            HandleSudoVFlag();

            ExecuteCommand(command, i, argc, argv);
        } else {
            ToLower(argv[i]);
            if (argv[i] == "-v") {
                if (argc != 2) {
                    std::cout << "Too many arguments.\n";
                    return;
                }

                HandleSudoVFlag();

                return;
            }
            if (argv[i] == "-k") {
                if (argc != 2) {
                    std::cout << "Too many arguments.\n";
                    return;
                }

                account_manager->SetCurrentUserPrivileges(false);

                return;
            }
            if (argv[i] == "-h") {
                if (argc != 2) {
                    std::cout << "Too many arguments.\n";
                    return;
                }

                SudoHelp();

                return;
            }
            if (argv[i] == "-u") {
                if (i >= argc - 2) {
                    std::cout << "Invalid number of arguments.\n";
                    return;
                }

                const std::string& username = argv[i + 1];
                HandleSudoUFlag(username, 3, argc, argv);

                return;
            } else {
                std::cout << "invalid flag!\n";
            }
        }
    }
}



bool Manager::GiveCurrentUserRootPrivileges() const
{
    std::string password;
    std::cout << "admin password: ";
    getline(std::cin, password, '\n');
    if (password == "admin") {
        account_manager->SetCurrentUserPrivileges(true);
        return true;
    }

    return false;
}

void Manager::HandleSudoVFlag() const
{
    if (!account_manager->CurrenUserHasRootPrivileges()) {
        if (GiveCurrentUserRootPrivileges()) {
            std::cout << "Admin privileges.\n";
        } else {
            std::cout << "Cannot get admin privileges.\n";
        }
    }
}

void Manager::HandleSudoUFlag(const std::string &username, const size_t command_index, size_t argc, std::vector<std::string>& argv)
{
    // If user doesnt exist

    // If exists give the user privilege

    const Command command = GetCommand(argv[command_index]);
    if (command == Command::none) {
        std::cout << "Invalid command.\n";
        return;
    }

    ExecuteCommand(command, command_index, argc, argv);

    // Take the privileges bacl
}


void Manager::SudoHelp() const
{
    std::cout << "Help\n";
}
