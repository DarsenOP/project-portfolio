//
// Created by DarsenOP on 2/4/25.
//

#include "taskpch.h"
#include "Manager.h"

/* --------------------Getters-------------------- */

Command Manager::GetCommand(std::string& command)
{
    std::ranges::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if      (command == "add") return Command::Add;
    else if (command == "list") return Command::List;
    else if (command == "edit") return Command::Edit;
    else if (command == "delete") return Command::Delete;
    else if (command == "complete") return Command::Complete;
    else if (command == "search") return Command::Search;
    else if (command == "help") return Command::Help;
    else if (command == "exit") return Command::Exit;
    else return Command::None;
}

Flag Manager::GetFlag(std::string& flag_str)
{
    std::ranges::transform(flag_str.begin(), flag_str.end(), flag_str.begin(), ::tolower);

    if (flag_str == "description") return Flag::Description;
    else if (flag_str == "priority") return Flag::Priority;
    else if (flag_str == "due") return Flag::Due;
    else if (flag_str == "tags") return Flag::Tags;
    else if (flag_str == "id") return Flag::ID;
    else if (flag_str == "status") return Flag::Status;
    else return Flag::None;
}

Priority Manager::GetPriority(std::string& priority_str)
{
    std::ranges::transform(priority_str.begin(), priority_str.end(), priority_str.begin(), ::tolower);

    if (priority_str == "high") return Priority::High;
    else if (priority_str == "medium") return Priority::Medium;
    else if (priority_str == "low") return Priority::Low;
    else return Priority::None;
}

Status Manager::GetStatus(std::string& status_str)
{
    std::ranges::transform(status_str.begin(), status_str.end(), status_str.begin(), ::tolower);

    if (status_str == "completed") return Status::Completed;
    else if (status_str == "pending") return Status::Pending;
    else return Status::None;
}

/* --------------------Converters-------------------- */

std::string Manager::GetStrPriority(const Priority &priority) const
{
    switch (priority) {
        case Priority::High: return "high";
        case Priority::Medium: return "medium";
        case Priority::Low: return "low";
        default: return "none";
    }
}

/* --------------------Main Functionality-------------------- */

void Manager::Add(size_t& argc, std::vector<std::string>& argv)
{
    // Description is required
    if (m_flags[Flag::Description].empty()) {
        std::cout << "You must provide a description for the task!" << std::endl;
        return;
    }

    Task task;

    // Here we go through all the flags and add the task
    for (auto& flag : m_flags) {
        // Flags that are not possible to set when adding a task
        if (flag.first == Flag::Status || flag.first == Flag::ID) continue;

        switch (flag.first) {
            case Flag::Description:
            {
                task.description = flag.second;
                break;
            }

            case Flag::Priority:
            {
                Priority priority = GetPriority(flag.second);
                if (priority == Priority::None) {
                    std::cout << "Unknown priority `" << flag.second << "`" << std::endl;
                    std::cout << "Type `help` for help!" << std::endl;
                    return;
                }
                task.priority = priority;
                break;
            }

            case Flag::Due:
            {
                task.due = flag.second;
                break;
            }

            case Flag::Tags:
            {
                // TODO: Handle the tags
                break;
            }

            default: break;
        }
    }

    // Things that the user cannot give
    task.id = m_prev_id++;
    task.status = Status::Pending;

    m_tasks.push_back(task);
} // add --description DESCRIPTION --priority PRIORITY --due DUE DATE --tags tag1 tag2 ... tagN

void Manager::List() const
{
    // Top line
    std::cout << std::setfill('-') << std::setw(126) << "" << std::setfill(' ') << std::endl;

    // Titles
    std::cout << std::left;
    std::cout << std::setw(7) << "| ID";
    std::cout << std::setw(50) << "| Description";
    std::cout << std::setw(13) << "| Due Date";
    std::cout << std::setw(13) << "| Priority";
    std::cout << std::setw(13) << "| Status";
    std::cout << std::setw(29) << "| Tags";
    std::cout << std::right << "|" << std::left << std::endl;

    for (auto& task : m_tasks) {
        // Task splitters
        std::cout << std::setfill('-') << std::left;
        std::cout << std::setw(7) << "|";
        std::cout << std::setw(50) << "|";
        std::cout << std::setw(13) << "|";
        std::cout << std::setw(13) << "|";
        std::cout << std::setw(13) << "|";
        std::cout << std::setw(30) << "|";
        std::cout << std::setfill(' ') << std::endl;

        // Task
        std::cout << std::left;
        std::cout << "| " << std::setw(5) << task.id;
        std::cout << "| " << std::setw(48) << task.description;
        std::cout << "| " << std::left << std::setw(11) << task.due;
        std::cout << "| " << std::setw(11) << GetStrPriority(task.priority);
        std::string status = ((task.status == Status::Pending) ? "Pending" : "Completed");
        std::cout << "| " << std::setw(11) << status;
        // TODO: Display the tags
        std::cout << "| " << std::setw(27) << "tags";
        std::cout << std::right << "|" << std::left << std::endl;
    }
    std::cout << std::setfill('-') << std::setw(126) << "";
    std::cout << std::setfill(' ') << std::endl;
} // list

void Manager::Edit(size_t& argc, std::vector<std::string>& argv)
{
    // ID is required
    if (m_flags[Flag::ID].empty()) {
        std::cout << "You must provide a valid ID!" << std::endl;
        return;
    }

    unsigned int ID{};

    // Possible that the user entered non-numeric value
    try {
        bool validID{false};

        ID = std::stoi(m_flags[Flag::ID]);

        // Check if there is a task with a given ID
        for (size_t i{}; i < m_tasks.size(); i++) {
            if (m_tasks[i].id == ID) {
                validID = true;
            }
        }

        // No task with a given ID was found
        if (!validID) {
            std::cout << "Invalid ID: Task with the ID " << argv[2] << " doesn't exist!" << std::endl;
            return;
        }
    } catch (const std::exception& e) {
        std::cout << "The ID of the task should be an integer!" << std::endl;
        return;
    }

    // Here we go through all the flags and add the task
    for (auto& flag : m_flags) {
        switch (flag.first) {
            case Flag::Description:
            {
                m_tasks[ID].description = flag.second;
                break;
            }

            case Flag::Priority:
            {
                Priority priority = GetPriority(flag.second);
                if (priority == Priority::None) {
                    std::cout << "Unknown priority: " << flag.second << std::endl;
                    std::cout << "Type `help` for help!" << std::endl;
                    return;
                }
                m_tasks[ID].priority = priority;
                break;
            }

            case Flag::Due:
            {
                m_tasks[ID].due = flag.second;
                break;
            }

            case Flag::Tags:
            {
                // TODO: Handle the tags
                break;
            }

            case Flag::Status:
            {
                Status status = GetStatus(flag.second);
                if (status == Status::None) {
                    std::cout << "Unknown status: " << flag.second << std::endl;
                    std::cout << "Type `help` for help!" << std::endl;
                    return;
                }
                m_tasks[ID].status = status;
                break;
            }

            default: break;
        }
    }
} // edit --description DESCRIPTION --priority PRIORITY --due DUE DATE --status STATUS

void Manager::Delete(size_t& argc, std::vector<std::string>& argv)
{
    // ID required
    if (m_flags[Flag::ID].empty()) {
        std::cout << "You must provide a valid ID!" << std::endl;
        return;
    }

    // Possible that the ID is non-numeric
    try {
        // Check if there is a task with a given ID
        unsigned short int ID = std::stoi(m_flags[Flag::ID]);
        for (size_t i{}; i < m_tasks.size(); i++) {
            if (m_tasks[i].id == ID) {
                m_tasks.erase(m_tasks.begin() + i);
                return;
            }
        }
        std::cout << "Invalid ID: Task with the ID " << argv[2] << " doesn't exist!" << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cout << "The ID of the task should be an integer!" << std::endl;
        return;
    }
} // delete --id ID

void Manager::Complete(size_t& argc, std::vector<std::string>& argv)
{
    // ID required
    if (m_flags[Flag::ID].empty()) {
        std::cout << "You must provide a valid ID!" << std::endl;
        return;
    }

    try {
        // Check if there is a task with a given ID
        unsigned short int ID = std::stoi(m_flags[Flag::ID]);
        for (size_t i{}; i < m_tasks.size(); i++) {
            if (m_tasks[i].id == ID) {
                m_tasks[i].status = Status::Completed;
                return;
            }
        }
        std::cout << "Invalid ID: Task with the ID " << argv[2] << " doesn't exist!" << std::endl;
        return;
    } catch (const std::exception& e) {
        std::cout << "The ID of the task should be an integer!" << std::endl;
        return;
    }
} // complete --id ID

void Manager::Help() const
{
    /* GetHelp : Gives the user all the available options */

    std::cout << "\nUsage: [options] [flags] values" << std::endl;
    std::cout << "\n[options]:\n";
    std::cout << std::left;
    std::cout << std::setw(20) << "\tadd" << std::setw(50) << "Add tasks to the manager." << std::endl;
    std::cout << std::setw(20) << "\tlist" << std::setw(50) << "List all the tasks in the manager." << std::endl;
    std::cout << std::setw(20) << "\tedit" << std::setw(50) << "Edit the tasks in the manager." << std::endl;
    std::cout << std::setw(20) << "\tdelete" << std::setw(50) << "Delete the tasks in the manager." << std::endl;
    std::cout << std::setw(20) << "\tcomplete" << std::setw(50) << "Complete the tasks in the manager." << std::endl;
    std::cout << std::setw(20) << "\tsearch" << std::setw(50) << "Search the tasks in the manager." << std::endl;
    std::cout << std::setw(20) << "\thelp" << std::setw(50) << "Display this help." << std::endl;
    std::cout << std::setw(20) << "\texit" << std::setw(50) << "Exit the program." << std::endl;
    std::cout << "[flags]:\n";
    std::cout << std::left;
    std::cout << std::setw(20) << "\t--description" << std::setw(50) << "Description of the task" << std::endl;
    std::cout << std::setw(20) << "\t--priority" << std::setw(50) << "Priority of the task" << std::endl;
    std::cout << std::setw(20) << "\t--due" << std::setw(50) << "Due date of the task" << std::endl;
    std::cout << std::setw(20) << "\t--tags" << std::setw(50) << "Tags of the task" << std::endl;
    std::cout << std::setw(20) << "\t--id" << std::setw(50) << "ID of the task" << std::endl;
    std::cout << std::setw(20) << "\t--status" << std::setw(50) << "Status of the task" << std::endl;
} // help

/* --------------------Command Handling-------------------- */

void Manager::HandleCommand(size_t& argc, std::vector<std::string>& argv)
{
    /* This function executes the command given by the user, and in case of an error handles it */

    Command command = GetCommand(argv[0]);

    if (command == Command::None) {
        std::cout << "Command not found: " << argv[0] << "\n";
        std::cout << "For a list of available commands, type 'help'.\n";
        return;
    }

    InitFlagMap(argc, argv);

    switch (command) {
        case Command::Add:
        {
            Add(argc, argv);
            break;
        }

        case Command::List:
        {
            List();
            break;
        }

        case Command::Edit:
        {
            Edit(argc, argv);
            break;
        }

        case Command::Delete:
        {
            Delete(argc, argv);
            break;
        }

        case Command::Complete:
        {
            Complete(argc, argv);
            break;
        }

        case Command::Search:
        {
            std::cout << "Searching tasks..." << std::endl;
            break;
        }

        case Command::Help:
        {
            Help();
            break;
        }

        case Command::Exit:
        {
            std::cout << "\n\nThank you for using this program.\nExiting the program..." << std::endl;
            exit(0);
        }

        default:
        {
            std::cerr << "Unknown error!" << std::endl;
            std::cerr << "Possible configuration file problems, please try to reinstall the program." << std::endl;
            break;
        }
    }

    ClearFlagMap();
}

/* --------------------Flag-Value Map Related-------------------- */

void Manager::InitFlagMap(size_t& argc, std::vector<std::string>& argv)
{
    for (size_t i{1}; i < argc; i++) {
        std::string flag_str{};

        // TODO: Handle when the flag is --tags, because if it is then there might be multiple values
        if (argv[i][0] == '-' && argv[i][1] == '-') {
            flag_str = argv[i].substr(2);
        } else {
            std::cout << "Value without a flag detected!\nPlease follow the syntax : [--flag value]" << std::endl;
            return;
        }

        Flag flag = GetFlag(flag_str);

        if (flag == Flag::None) {
            std::cout << "Flag not found: --" << flag_str << std::endl;
            std::cout << "For a list of available commands, type 'help'.\n";
            return;
        }

        if (i + 1 >= argc) {
            std::cout << "Too few arguments. After --" << flag_str << " you should provide a value!" << std::endl;
            return;
        } else if (argv[i + 1][0] == '-' && argv[i + 1][1] == '-') {
            std::cout << "Value not detected for the flag `" << argv[i] << "`" << std::endl;
            std::cout << "Please follow the syntax : [--flag value]" << std::endl;
            return;
        }

        m_flags[flag] = argv[i + 1];
        i++;
    }
}

void Manager::ClearFlagMap()
{
    m_flags.clear();
}
