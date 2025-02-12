//
// Created by DarsenOP on 2/8/25.
//

#include "Manager.h"
#include "Util.h"
#include "Core/Core.h"
#include "CommandProcessor/CommandProcessor.h"

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

            if (!HandleCommand(arg_count, args)) {
                Log::LOG(LogLevel::INFO, "Thank you for using the File Manager. Goodbye!");
                return;
            } else {
                account_manager->ListUsers();
            }
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

/* Command Handling */

bool Manager::HandleCommand(const size_t argc, std::vector<std::string>& argv)
{
    Util::ToLower(argv[0]);
    const Command first_command = Util::GetCommand(argv[0]);

    if (first_command == Command::none) {
        Log::LOG(LogLevel::ERROR, "Command '"+argv[0]+"' not found. Please check the spelling or refer to the help section.");
        return true;
    }

    if (first_command == Command::sudo) {
        if (argc == 1) {
            SudoHelp();
            return true;
        }


        return Sudo(argc, argv);
    } else {
        return ExecuteCommand(first_command, 0, argc, argv);
    }
}

bool Manager::ExecuteCommand(const Command &command, const size_t command_index, const size_t argc,
                             std::vector<std::string>& argv) const
{
    switch (command) {
        case Command::exit:
            return false;
        case Command::useradd:
            if (account_manager->CurrenUserHasRootPrivileges())
                account_manager->HandleUseradd(command_index, argc, argv);
            else
                Log::LOG(LogLevel::ERROR, "Insufficient privileges to create a new user. Please ensure you have admin access.");
            return true;
        case Command::passwd:
            account_manager->HandlePasswd(command_index, argc, argv);
        default:
            return true;
    }

}

/* Sudo Handling */

bool Manager::Sudo(const size_t argc, std::vector<std::string> &argv)
{
    for (size_t i = 1; i < argc; i++) {
        if (argv[i][0] != '-') {
            Util::ToLower(argv[i]);
            const Command command = Util::GetCommand(argv[i]);

            if (command == Command::none) {
                Log::LOG(LogLevel::ERROR, "Command '"+argv[i]+"' not found. Please check the spelling or refer to the help section.");
                return true;
            }

            std::string current_username = account_manager->GetCurrentUserName();
            if (account_manager->SwitchUser("admin")) {
                if (ExecuteCommand(command, i, argc, argv)) {
                    account_manager->SwitchUser(current_username, true);
                    return true;
                }
            }

        } else {
            Util::ToLower(argv[i]);
            const Flag sudo_flag = Util::GetSudoFlag(argv[i]);

            switch (sudo_flag) {
                case Flag::sudo_v:
                    if (argc != 2) {
                        Log::LOG(LogLevel::ERROR, "Invalid usage of '-v' flag. Correct usage: sudo -v");
                        return true;
                    }
                    HandleSudoVFlag();
                    return true;

                case Flag::sudo_k:
                    if (argc != 2) {
                        Log::LOG(LogLevel::ERROR, "Invalid usage of '-k' flag. Correct usage: sudo -k");
                        return true;
                    }
                    HandleSudoKFlag();
                    return true;

                case Flag::sudo_u:
                    if (i == argc - 1) {
                        Log::LOG(LogLevel::ERROR, "Missing username after -u flag. Correct usage: sudo -u [username] [command]");
                        return true;
                    }
                    if (i == argc - 2) {
                        Log::LOG(LogLevel::ERROR, "Incorrect number of arguments for -u flag. Correct usage: sudo -u [username] [command] / sudo -u [username] -i");
                        return true;
                    }
                    return HandleSudoUFlag(argv[i + 1], 3, argc, argv);

                case Flag::sudo_h:
                    if (argc != 2) {
                        Log::LOG(LogLevel::ERROR, "Invalid usage of '-h' flag. Correct usage: sudo -h");
                        return true;
                    }
                    SudoHelp();
                    return true;

                case Flag::sudo_i:
                    if (i != argc - 1) {
                        Log::LOG(LogLevel::ERROR, "Invalid argument after '-i' flag. The 'sudo -i' command should not have additional arguments.");
                        return false;
                    }
                    HandleSudoIFlag("admin");
                    return true;

                default:
                    Log::LOG(LogLevel::ERROR, "Flag '"+argv[i]+"' not found. Please check the spelling or refer to the help section.");
                    return true;
            }
        }
    }

    return true;
}

bool Manager::PromptForAdminPassword(const bool give_sudo) const
{
    std::string password;
    std::cout << "[sudo] password for admin: ";
    getline(std::cin, password, '\n');

    if (password == account_manager->GetAdminPassword()) {
        account_manager->SetCurrentUserPrivileges(give_sudo);
        return true;
    }

    return false;
}

void Manager::HandleSudoVFlag() const
{
    if (!account_manager->CurrenUserHasRootPrivileges()) {
        if (PromptForAdminPassword()) {
            Log::LOG(LogLevel::INFO, "Credentials validated. Admin access is now enabled.");
        } else {
            Log::LOG(LogLevel::ERROR, "Invalid credentials. Unable to verify admin privileges.");
        }
    } else {
        Log::LOG(LogLevel::TRACE, "You already have admin privileges.");
    }
}

void Manager::HandleSudoKFlag() const
{
    if (!account_manager->IsCurrentUserAnAdmin() && account_manager->CurrenUserHasRootPrivileges()) {
        if (PromptForAdminPassword(false)) {
            Log::LOG(LogLevel::INFO,"Cached credentials cleared. You will be prompted for authentication again the next time you use sudo.");
        } else {
            Log::LOG(LogLevel::ERROR, "Incorrect password. Admin privileges could not be revoked.");
        }
    } else if (!account_manager->IsCurrentUserAnAdmin()) {
        Log::LOG(LogLevel::TRACE,"You do not have admin privileges to revoke.");
    } else {
        Log::LOG(LogLevel::TRACE,"You are the admin. You cannot revoke the privileges from yourself.");
    }
}

bool Manager::HandleSudoUFlag(const std::string &username, const size_t command_index, size_t argc, std::vector<std::string>& argv)
{
    if (!account_manager->UserExists(username)) {
        Log::LOG(LogLevel::ERROR, "Specified username does not exist. Please provide a valid username.");
        return false;
    }

    Util::ToLower(argv[command_index]);
    const Command command = Util::GetCommand(argv[command_index]);
    if (command == Command::none) {

        if (argv[command_index] == "-i") {
            if (command_index != argc - 1) {
                Log::LOG(LogLevel::ERROR, "Invalid argument after 'sudo -u <username> -i'. The 'sudo' command with '-u' and '-i' should not have additional arguments.");
                return false;
            }

            HandleSudoIFlag(username);
            return true;
        }

        Log::LOG(LogLevel::ERROR, "Command '"+argv[command_index]+"' not found. Please check the spelling or refer to the help section.");
        return true;
    }

    std::string current_username = account_manager->GetCurrentUserName();
    if (account_manager->SwitchUser(username)) {
        if (ExecuteCommand(command, command_index, argc, argv)) {
            account_manager->SwitchUser(current_username, true);
            return true;
        }
    }

    return false;
}

void Manager::HandleSudoIFlag(const std::string& username) const
{
    account_manager->SwitchUser(username);
}

void Manager::SudoHelp()
{
    std::cout << "Usage: sudo [OPTION] COMMAND [ARGUMENTS...]\n\n";
    std::cout << "Run a command as another user, typically with elevated privileges (root).\n\n";
    std::cout << "Options:\n";
    std::cout << std::left;
    std::cout << std::setw(7) << "  -h" << std::setw(30) << "Display this help message and exit.\n";
    std::cout << std::setw(7) << "  -v" << std::setw(30) << "Display the current user's cached credentials,\n";
    std::cout << std::setw(7) << "    " << std::setw(30) << "and verify if they are still valid.\n";
    std::cout << std::setw(7) << "  -k" << std::setw(30) << "Invalidates the user's cached credentials, forcing\n";
    std::cout << std::setw(7) << "    " << std::setw(30) << "the user to authenticate again for the next sudo command.\n";
    std::cout << std::setw(7) << "  -u" << std::setw(30) << "Run the specified COMMAND as the specified user\n";
    std::cout << std::setw(7) << "    " << std::setw(30) << "(instead of root). You must be allowed to run the command\n";
    std::cout << std::setw(7) << "    " << std::setw(30) << "as that user in the sudoers file.\n";
}
