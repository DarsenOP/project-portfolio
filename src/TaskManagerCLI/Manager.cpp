//
// Created by DarsenOP on 2/4/25.
//

#include "taskpch.h"
#include "Manager.h"

/* --------------------Getters-------------------- */

inline Command Manager::GetCommand(std::string& command_str)
{
    ToLower(command_str);

    static const std::unordered_map<std::string, Command> command_map {
        {"add", Command::Add},
        {"list", Command::List},
        {"edit", Command::Edit},
        {"delete", Command::Delete},
        {"complete", Command::Complete},
        {"search", Command::Search},
        {"help", Command::Help},
        {"exit", Command::Exit}
    };

    if (const auto it = command_map.find(command_str); it != command_map.end())
        return it->second;

    return Command::None;
}

inline Flag Manager::GetFlag(std::string& flag_str)
{
    ToLower(flag_str);

    static const std::unordered_map<std::string, Flag> flag_map {
        {"description", Flag::Description},
        {"priority", Flag::Priority},
        {"due", Flag::Due},
        {"tags", Flag::Tags},
        {"id", Flag::ID},
        {"status", Flag::Status}
    };

    if (const auto it = flag_map.find(flag_str); it != flag_map.end())
        return it->second;

    return Flag::None;
}

inline Priority Manager::GetPriority(std::string& priority_str)
{
    ToLower(priority_str);

    static const std::unordered_map<std::string, Priority> priority_map {
        {"high", Priority::High},
        {"medium", Priority::Medium},
        {"low", Priority::Low}
    };

    if (const auto it = priority_map.find(priority_str); it != priority_map.end())
        return it->second;

    return Priority::None;
}

inline Status Manager::GetStatus(std::string& status_str)
{
    ToLower(status_str);

    static const std::unordered_map<std::string, Status> status_map {
        {"completed", Status::Completed},
        {"pending", Status::Pending}
    };

    if (const auto it = status_map.find(status_str); it != status_map.end())
        return it->second;

    return Status::None;
}

/* --------------------Converters-------------------- */

inline std::string Manager::GetPriorityStr(const Priority &priority)
{
    static const std::unordered_map<Priority, std::string> priority_str_map {
        {Priority::High, "high"},
        {Priority::Medium, "medium"},
        {Priority::Low, "low"},
    };

    if (const auto it = priority_str_map.find(priority); it != priority_str_map.end())
        return it->second;

    return "none";
}

inline std::string Manager::GetFlagStr(const Flag &flag)
{
    static const std::unordered_map<Flag, std::string> flag_str_map {
        {Flag::Description, "description"},
        {Flag::Priority, "priority"},
        {Flag::Due, "due"},
        {Flag::Tags, "tags"},
        {Flag::ID, "id"},
        {Flag::Status, "status"}
    };

    if (const auto it = flag_str_map.find(flag); it != flag_str_map.end())
        return it->second;

    return "none";
}

/* --------------------Helpers-------------------- */

inline void Manager::ToLower(std::string& str)
{
    std::ranges::transform(str, str.begin(), ::tolower);
}

inline bool Manager::FlagUsed(const Flag &flag) const
{
    const auto it = m_flags.find(flag);
    return it != m_flags.end() && !it->second.empty();
}

inline bool Manager::IsFlag(const std::string& flag)
{
    return flag.length() >= 3 && flag.starts_with("--");
}

DateValidationResult Manager::ValidateDateFormat(std::string &date)
{
    const std::regex format(R"(^(\d{4})([-/\.])(\d{1,2})\2(\d{1,2})$)");

    std::smatch match;
    if (!std::regex_match(date, match, format))
        return DateValidationResult::InvalidFormat;

    const int year = std::stoi(match[1]);
    const int month = std::stoi(match[3]);
    const int day = std::stoi(match[4]);

    constexpr int MIN_YEAR = 1900;
    if (year < MIN_YEAR || month < 1 || month > 12)
        return DateValidationResult::InvalidValue;

    static constexpr std::array<int, 12> days_in_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    const int max_days = days_in_month[month - 1] +
        ((month == 2 && ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)) ? 1 : 0);
    if (day < 1 || day > max_days)
        return DateValidationResult::InvalidValue;

    std::ostringstream normalized_date;
    normalized_date << std::setfill('0') << std::setw(4) << year << "-"
                                            << std::setw(2) << month << "-"
                                            << std::setw(2) << day;

    date = normalized_date.str();
    return DateValidationResult::Success;
}

inline void Manager::PrintExitMessage()
{
    std::cout << "\n============================================\n"
              << "  ✅ Thank you for using Task Manager CLI! \n"
              << "     Have a productive day! 🚀  \n"
              << "============================================\n\n";
}

void Manager::SplitQuotedText(const std::string& input, std::vector<std::string>& output)
{
    std::string current_arg;

    for (const char c : input) {
        if (c == ' ') {
            if (!current_arg.empty()) {
                ToLower(current_arg);
                output.push_back(std::move(current_arg));
                current_arg.clear();
            }
        }

        current_arg += c;
    }

    // It is possible that the last word was not pushed into the vector, so we explicitly check for it
    if (!current_arg.empty()) {
        ToLower(current_arg);
        output.push_back(std::move(current_arg));
    }
}

/* --------------------Main Functionality-------------------- */

void Manager::Add(const size_t argc, const std::vector<std::string>& argv)
{
    if (!FlagUsed(Flag::Description)) {
        PrintArgumentError("--description", "is required for this command.");
        return;
    }

    Task task;

    for (auto &[flag, values] : m_flags) {
        if (flag == Flag::Status || flag == Flag::ID) {
            PrintInvalidFlagsError("add", {"description", "priority", "due", "tags"});
            continue;
        }

        switch (flag) {
            case Flag::Description:
                task.description = values[0];
                break;

            case Flag::Priority:
                task.priority = GetPriority(values[0]);
                if (task.priority == Priority::None) {
                    PrintInvalidValuesError("priority", values[0],
                        "`high`, `medium`, or `low`");
                    return;
                }
                break;

            case Flag::Due:
                switch (ValidateDateFormat(values[0])) {
                    case DateValidationResult::InvalidFormat:
                        PrintInvalidDueDateFormatError(values[0]);
                        return;
                    case DateValidationResult::InvalidValue:
                        PrintInvalidValuesError("due", values[0], "Format: `YYYY-MM-DD`, `YYYY.MM.DD`, `YYYY/MM/DD`");
                        return;
                    default:
                        task.due = values[0];
                        break;
                }

                break;

            case Flag::Tags:
                task.tags = values;
                break;

            default:
                break;
        }
    }

    task.id = m_prev_id++;

    m_tasks.push_back(task);

    std::cout << "📌 Task added successfully! (ID: " << task.id << ")\n";
}

void Manager::List() const
{
    if (m_tasks.empty() || m_tasks.size() == m_hidden_count) {
        std::cout << "\n📭 No tasks available.\n";
        return;
    }

    // **Step 1: Compute Column Widths Dynamically**
    size_t id_width = 4;
    size_t desc_width = 11;
    size_t due_width = 9;
    size_t priority_width = 8;
    size_t status_width = 7;
    size_t tags_width = 4;

    for (const auto& task : m_tasks) {
        if (task.hidden) continue;

        id_width = std::max(id_width, std::to_string(task.id).length());
        desc_width = std::max(desc_width, task.description.length());
        due_width = std::max(due_width, task.due.length());
        priority_width = std::max(priority_width, GetPriorityStr(task.priority).length());
        status_width = std::max(status_width, static_cast<size_t>(task.status == Status::Pending ? 8 : 9));

        size_t tag_length = 0;

        if (!task.tags.empty()) {
            const size_t total_tag_chars = std::accumulate(
                task.tags.begin(), task.tags.end(), 0,
                [](size_t sum, const std::string& tag) { return sum + tag.length(); });

            const size_t num_commas = 2 * (task.tags.size() - 1); // Space for ", " between tags

            tag_length = total_tag_chars + num_commas;
        }

        tags_width = std::max(tags_width, tag_length);
    }

    // Add some padding for readability
    id_width += 2;
    desc_width += 2;
    due_width += 2;
    priority_width += 2;
    status_width += 2;
    tags_width += 2;

    // **Step 2: Print Header**
    const size_t total_width = id_width + desc_width + due_width + priority_width + status_width + tags_width + 14;

    std::cout << std::setfill('-') << std::setw(static_cast<int>(total_width)) << "" << std::setfill(' ') << "\n";

    std::cout << std::left
              << "| " << std::setw(static_cast<int>(id_width)) << "ID"
              << "| " << std::setw(static_cast<int>(desc_width)) << "Description"
              << "| " << std::setw(static_cast<int>(due_width)) << "Due Date"
              << "| " << std::setw(static_cast<int>(priority_width)) << "Priority"
              << "| " << std::setw(static_cast<int>(status_width)) << "Status"
              << "| " << std::setw(static_cast<int>(tags_width)) << "Tags"
              << " |" << "\n";

    std::cout << std::setfill('-') << std::setw(static_cast<int>(total_width)) << "" << std::setfill(' ') << "\n";

    // **Step 3: Print Each Task**
    for (const auto& task : m_tasks) {
        if (task.hidden) continue;

        std::ostringstream tags_stream;
        for (size_t i = 0; i < task.tags.size(); ++i) {
            if (i > 0) tags_stream << ", ";  // Add separator before each new tag (except first)
            tags_stream << task.tags[i];
        }

        std::cout << std::left
                  << "| " << std::setw(static_cast<int>(id_width)) << task.id
                  << "| " << std::setw(static_cast<int>(desc_width)) << task.description
                  << "| " << std::setw(static_cast<int>(due_width)) << task.due
                  << "| " << std::setw(static_cast<int>(priority_width)) << GetPriorityStr(task.priority)
                  << "| " << std::setw(static_cast<int>(status_width)) << (task.status == Status::Pending ? "Pending" : "Completed")
                  << "| " << std::setw(static_cast<int>(tags_width)) << tags_stream.str()
                  << " |" << "\n";
    }

    // **Step 4: Print Bottom Line**
    std::cout << std::setfill('-') << std::setw(static_cast<int>(total_width)) << "" << std::setfill(' ') << "\n";
}

void Manager::Edit(const size_t argc, const std::vector<std::string>& argv)
{
    if (!FlagUsed(Flag::ID)) {
        PrintArgumentError("--id", "is required for this command.");
        return;
    }

    if (m_flags.size() == 1) {
        std::cout << "❌ Error: No changes specified. Use other flags (e.g., --description, --priority, --due, --tags, --status) along with --id.\n";
        return;
    }

    try {
        unsigned short int ID = std::stoi(m_flags[Flag::ID][0]);
        const auto it = std::ranges::find_if(m_tasks.begin(), m_tasks.end(), [ID](const Task& task) { return task.id == ID; });

        if (it == m_tasks.end()) {
            return PrintTaskNotFoundError(m_flags[Flag::ID][0]);
        }

        for (auto& [flag, values] : m_flags) {
            if (flag == Flag::ID) continue;

            DateValidationResult date_status;

            switch (flag) {
                case Flag::Description:
                    it->description = values[0];
                    break;

                case Flag::Priority:
                    it->priority = GetPriority(values[0]);
                    if (it->priority == Priority::None) {
                        return PrintInvalidValuesError("priority", values[0],
                            "`high`, `medium`, or `low`");
                    }
                    break;

                case Flag::Due:
                    date_status = ValidateDateFormat(values[0]);
                    if (date_status == DateValidationResult::InvalidFormat) {
                        return PrintInvalidDueDateFormatError(values[0]);
                    } else if (date_status == DateValidationResult::InvalidValue) {
                        return PrintInvalidValuesError("due", values[0],
                            "Format: `YYYY-MM-DD`, `YYYY.MM.DD`, `YYYY/MM/DD`");
                    }
                    it->due = values[0];
                    break;

                case Flag::Tags:
                    it->tags = values;
                    break;

                case Flag::Status:
                    it->status = GetStatus(values[0]);
                    if (it->status == Status::None) {
                        return PrintInvalidValuesError("status", values[0],
                            "`pending`, `completed`");
                    }
                    break;

                default:
                    break;
            }
        }

        std::cout << "✏️  Task (ID: " << it->id << ") updated successfully!\n";
    } catch (const std::exception&) {
        PrintInvalidValuesError("id", m_flags[Flag::ID][0], "integer");
    }
}

void Manager::Delete(const size_t argc, const std::vector<std::string>& argv)
{
    if (!FlagUsed(Flag::ID)) {
        PrintArgumentError("--id", "is required for this command.");
        return;
    }

    if (m_flags.size() > 1) {
        PrintInvalidFlagsError("delete", {"id"});
        return;
    }

    try {
        const unsigned short int ID = std::stoi(m_flags[Flag::ID][0]);

        if (const auto it = std::ranges::find_if(m_tasks.begin(), m_tasks.end(),
            [ID](const Task& task) { return task.id == ID; }); it != m_tasks.end()) {
            m_tasks.erase(it);
            std::cout << "🗑️  Task (ID: " << ID << ") deleted successfully!\n";
        } else {
            PrintTaskNotFoundError(m_flags[Flag::ID][0]);
        }
    } catch (const std::exception&) {
        PrintInvalidValuesError("id", m_flags[Flag::ID][0], "integer");
    }
}

void Manager::Complete(const size_t argc, const std::vector<std::string>& argv)
{
    if (!FlagUsed(Flag::ID)) {
        PrintArgumentError("--id", "is required for this command.");
        return;
    }

    if (m_flags.size() > 1) {
        PrintInvalidFlagsError("complete", {"id"});
        return;
    }

    try {
        const unsigned short int ID = std::stoi(m_flags[Flag::ID][0]);

        if (const auto it = std::ranges::find_if(m_tasks.begin(), m_tasks.end(),
            [ID](const Task& task) { return task.id == ID; }); it != m_tasks.end()) {
            it->status = Status::Completed;
            std::cout << "✅ Task (ID: " << ID << ") marked as completed!\n";
        } else {
            PrintTaskNotFoundError(m_flags[Flag::ID][0]);
        }
    } catch (const std::exception&) {
        PrintInvalidValuesError("id", m_flags[Flag::ID][0], "integer");
    }
}

void Manager::Search(const size_t argc, const std::vector<std::string> &argv)
{
    const bool descriptionPresent = FlagUsed(Flag::Description);
    const bool tagsPresent = FlagUsed(Flag::Tags);

    if (!descriptionPresent && !tagsPresent) {
        PrintArgumentError("--description / --tags", "should be present.");
        return;
    }

    if (m_flags.size() > (descriptionPresent + tagsPresent)) {
        PrintInvalidFlagsError("search", {"description", "tags"});
        return;
    }

    if (descriptionPresent) {
        std::vector<std::string> keywords;
        SplitQuotedText(m_flags[Flag::Description][0], keywords);

        for (auto &task : m_tasks) {
            std::string description = task.description;
            ToLower(description);

            const bool hasAnyOccurrences = std::ranges::any_of(keywords.begin(), keywords.end(),
                [&description](const std::string& keyword) {
                    return description.find(keyword) != std::string::npos;
                });

            task.hidden = !hasAnyOccurrences;
        }
    }

    if (tagsPresent) {
        std::vector<std::string> requiredTags = m_flags[Flag::Tags];
        std::ranges::for_each(requiredTags, [](std::string& word) { ToLower(word); });

        for (auto &task : m_tasks) {
            const bool hasAnyTag = std::ranges::any_of(requiredTags,
                [&task](const std::string& tag) {
                    return std::ranges::find_if(task.tags,
                        [&tag](const std::string& taskTag) {
                            std::string taskTagLower = taskTag;
                            ToLower(taskTagLower);
                            return taskTagLower == tag;
                        }) != task.tags.end();
                });

            task.hidden |= !hasAnyTag;
        }
    }

    m_hidden_count = std::ranges::count_if(m_tasks, [](const Task& task) { return task.hidden; });

    List();

    if (m_hidden_count > 0) {
        std::ranges::for_each(m_tasks, [](Task& task) { task.hidden = false; });
        m_hidden_count = 0;
    }
}

void Manager::Help()
{
    std::cout << "\n📖 Task Manager CLI - Help Guide\n";
    std::cout << "===========================================\n";
    std::cout << "📌 Task Management Commands:\n";
    std::cout << "  ➕ `add`        - Add a new task\n";
    std::cout << "  🗑  `delete`     - Remove a task by ID\n";
    std::cout << "  ✅ `complete`   - Mark a task as completed\n";
    std::cout << "  ✏️  `edit`       - Modify a task\n\n";

    std::cout << "📋 Viewing & Searching Commands:\n";
    std::cout << "  📋 `list`       - Show all tasks\n";
    std::cout << "  🔍 `search`     - Find tasks by description or tag\n\n";

    std::cout << "⚙️  General Commands:\n";
    std::cout << "  🆘 `help`       - Show this guide\n";
    std::cout << "  ❌ `exit`       - Exit the Task Manager\n\n";

    std::cout << "🚩 Flags and Usage:\n";
    std::cout << "  --description [TEXT]            - Description of the task (Required for add)\n";
    std::cout << "  --priority [high|medium|low]    - Task priority\n";
    std::cout << "  --due [YYYY-MM-DD]              - Due date of the task\n";
    std::cout << "  --tags [tag1 tag2 ...]          - Tags for categorization\n";
    std::cout << "  --id [NUMBER]                   - Specify task ID (Required for delete, edit, complete)\n";
    std::cout << "  --status [pending|completed]    - Change task status (For edit)\n\n";

    std::cout << "💡 Examples:\n";
    std::cout << "  ➕ Add a new task:\n";
    std::cout << "     task add --description \"Finish project\" --priority high --due 2025-02-10 --tags work,urgent\n";
    std::cout << "  🗑  Delete a task:\n";
    std::cout << "     task delete --id 3\n";
    std::cout << "  ✅ Mark a task as completed:\n";
    std::cout << "     task complete --id 5\n";
    std::cout << "  🔍 Search tasks by tag:\n";
    std::cout << "     task search --tags important\n\n";

    std::cout << "⚠️  Notes:\n";
    std::cout << "  - `add` requires `--description`.\n";
    std::cout << "  - `delete`, `edit`, and `complete` require `--id`.\n";
    std::cout << "  - `list` has no flags and simply displays tasks.\n";
    std::cout << "  - `search` can be used with `--description` or `--tags`.\n";

    std::cout << "\n✨ Enjoy using Task Manager CLI! 🚀\n";
}

/* --------------------Command Handling-------------------- */

RunStatus Manager::HandleCommand(const size_t argc, const std::vector<std::string> &argv)
{
    std::string command_str = argv[0];
    const Command command = GetCommand(command_str);
    if (command == Command::None) {
        PrintCommandNotFoundError(command_str);
        return RunStatus::Run;
    }

    if (!InitFlagMap(argc, argv)) {
        ClearFlagMap();
        return RunStatus::Run;
    }

    if (command == Command::Exit) {
        PrintExitMessage();
        return RunStatus::Exit;
    }

    ExecuteCommand(command, argc, argv);

    ClearFlagMap();
    return RunStatus::Run;
}

void Manager::ExecuteCommand(const Command &command, const size_t argc, const std::vector<std::string> &argv)
{
    switch (command) {
        case Command::Add:      Add(argc, argv);        break;
        case Command::List:     List();                 break;
        case Command::Edit:     Edit(argc, argv);       break;
        case Command::Delete:   Delete(argc, argv);     break;
        case Command::Complete: Complete(argc, argv);   break;
        case Command::Search:   Search(argc, argv);     break;
        case Command::Help:     Help();                 break;
        default:                                        break;
    }
}

/* --------------------Flag-Value Map Handling-------------------- */

bool Manager::InitFlagMap(const size_t argc, const std::vector<std::string> &argv)
{
    for (size_t i{1}; i < argc; i++) {
        std::string flag_str;

        if (IsFlag(argv[i])) {
            flag_str = argv[i].substr(2);
        } else {
            PrintArgumentError(argv[i], "is not a valid flag or was used incorrectly.");
            return false;
        }

        Flag flag = GetFlag(flag_str);
        if (flag == Flag::None) {
            PrintArgumentError("--" + flag_str, "is not recognized.");
            return false;
        }

        if (i + 1 >= argc || IsFlag(argv[i + 1])) {
            PrintArgumentError("--" + flag_str, "is missing a required value.");
            return false;
        }

        i++;

        std::vector<std::string>& values = m_flags[flag];
        while (i < argc && !IsFlag(argv[i])) {
            values.push_back(argv[i]);
            i++;
        }

        if (flag != Flag::Tags && values.size() > 1) {
            PrintArgumentError("--" + flag_str, "only accepts **one** value. Use `--tags tag1 tag2 ...` for multiple values.");
            return false;
        }

        --i;
    }

    return true;
}

inline void Manager::ClearFlagMap()
{
    m_flags.clear();
}

/* --------------------Error Handling-------------------- */

static constexpr auto HELP_HINT = "🔹 Type 'help' for more details.\n";

void Manager::PrintCommandNotFoundError(const std::string &command)
{
    std::cout << "\n❌ Error: Unknown command → `" << command << "`\n"
              << HELP_HINT << "\n";
}

void Manager::PrintInvalidDueDateFormatError(const std::string &date)
{
    std::cout << "\n❌ Error: Invalid date format → `" << date << "`\n"
              << "🔹 Accepted formats: `YYYY-MM-DD`, `YYYY.MM.DD`, `YYYY/MM/DD`\n"
              << HELP_HINT << "\n";
}

void Manager::PrintTaskNotFoundError(const std::string& id) {
    std::cout << "❌ Error: Task with ID `" << id << "` not found.\n";
}

void Manager::PrintInvalidFlagsError(const std::string& command, const std::vector<std::string>& valid_flags)
{
    std::ostringstream oss;
    oss << "\n❌ Error: Invalid flags used with the `" << command << "` command.\n"
        << "🔹 Allowed flags: ";

    for (size_t i = 0; i < valid_flags.size(); i++) {
        oss << "`--" << valid_flags[i] << "`" << (i + 1 < valid_flags.size() ? ", " : "");
    }

    oss << "\n" << HELP_HINT << "\n";
    std::cout << oss.str();
}

void Manager::PrintInvalidValuesError(const std::string& flag, const std::string& value, const std::string& expected)
{
    std::cout << "\n❌ Error: Invalid value `" << value << "` for flag `--" << flag << "`.\n"
              << "🔹 Expected: " << expected << "\n"
              << HELP_HINT << "\n";
}

void Manager::PrintArgumentError(const std::string& arg, const std::string& message)
{
    std::cout << "\n❌ Error: `" << arg << "` " << message << "\n" << HELP_HINT << "\n";
}
