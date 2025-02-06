//
// Created by DarsenOP on 2/4/25.
//

#include "taskpch.h"
#include "Manager.h"

#include <ranges>

/* --------------------Consts-------------------- */

static constexpr short int HISTORY_LIMIT = 50;

static const std::string PRIORITY_EXPECTED_VALUES = "`high`, `medium`, `low`, or `none`";
static const std::string DUE_DATE_EXPECTED_FORMAT = "Format: `YYYY-MM-DD`, `YYYY.MM.DD`, `YYYY/MM/DD`";
static const std::string DUE_DATE_EXPECTED_DATE = "Real day in a calendar starting from (1900-01-01)";

static const std::string NO_CHANGE_SPECIFIED_ERROR = "❌ Error: No changes specified. Use other flags (e.g., --description, --priority, --due, --tags, --status) along with --id.\n";

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
        {"filter", Command::Filter},
        {"sort", Command::Sort},
        {"tag", Command::Tag},
        {"undo", Command::Undo},
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
        {"to", Flag::To},
        {"tags", Flag::Tags},
        {"id", Flag::ID},
        {"by", Flag::SortBy},
        {"order", Flag::SortOrder},
        {"add", Flag::Add},
        {"remove", Flag::Remove},
        {"list", Flag::List},
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
        {"low", Priority::Low},
        {"none", Priority::None}
    };

    if (const auto it = priority_map.find(priority_str); it != priority_map.end())
        return it->second;

    return Priority::Invalid;
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

inline Order Manager::GetOrder(std::string& order_str)
{
    ToLower(order_str);

    static const std::unordered_map<std::string, Order> order_map {
            {"asc", Order::Ascending},
            {"desc", Order::Descending}
    };

    if (const auto it = order_map.find(order_str); it != order_map.end())
        return it->second;

    return Order::None;
}

/* --------------------Converters-------------------- */

inline std::string Manager::GetPriorityStr(const Priority &priority)
{
    static const std::unordered_map<Priority, std::string> priority_str_map {
        {Priority::High, "high"},
        {Priority::Medium, "medium"},
        {Priority::Low, "low"},
        {Priority::None, "none"}
    };

    if (const auto it = priority_str_map.find(priority); it != priority_str_map.end())
        return it->second;

    return "invalid";
}

inline std::string Manager::GetFlagStr(const Flag &flag)
{
    static const std::unordered_map<Flag, std::string> flag_str_map {
        {Flag::Description, "description"},
        {Flag::Priority, "priority"},
        {Flag::Due, "due"},
        {Flag::To, "to"},
        {Flag::Tags, "tags"},
        {Flag::ID, "id"},
        {Flag::SortBy, "by"},
        {Flag::SortOrder, "order"},
        {Flag::Add, "add"},
        {Flag::Remove, "remove"},
        {Flag::List, "list"},
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

inline void Manager::PrintExitMessage()
{
    std::cout << "\n============================================\n"
              << "  ✅ Thank you for using Task Manager CLI! \n"
              << "     Have a productive day! 🚀  \n"
              << "============================================\n\n";
}

DateValidationResult Manager::ValidateDateFormat(std::string &date)
{
    const std::regex date_format(R"(^(\d{4})([-/\.])(\d{1,2})\2(\d{1,2})$)");

    std::smatch match;
    if (!std::regex_match(date, match, date_format))
        return DateValidationResult::InvalidFormat;

    const int year = std::stoi(match[1]);
    const int month = std::stoi(match[3]);
    const int day = std::stoi(match[4]);

    if (year < 1900 || month < 1 || month > 12)
        return DateValidationResult::InvalidValue;

    static constexpr std::array<int, 12> days_in_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    const bool is_leap_year = (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;

    const int max_days = days_in_month[month - 1] + ((month == 2 && is_leap_year) ? 1 : 0);
    if (day < 1 || day > max_days)
        return DateValidationResult::InvalidValue;

    std::ostringstream normalized_date;
    normalized_date << std::setfill('0') << std::setw(4) << year << "-"
                                            << std::setw(2) << month << "-"
                                            << std::setw(2) << day;

    date = normalized_date.str();
    return DateValidationResult::Success;
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
        } else {
            current_arg += c;
        }
    }

    // It is possible that the last word was not pushed into the vector, so we explicitly check for it
    if (!current_arg.empty()) {
        ToLower(current_arg);
        output.push_back(std::move(current_arg));
        current_arg.clear();
    }
}

void Manager::SortIndirectly(const size_t argc, const std::vector<std::string> &argv)
{
    if (m_prev_sort.first != Flag::None) {
        m_flags[Flag::SortBy] = {GetFlagStr(m_prev_sort.first)};
        m_flags[Flag::SortOrder] = {(m_prev_sort.second == Order::Ascending ? "asc" : "desc")};
        Sort(false);
    }
}

void Manager::ListIndirectly()
{
    m_hidden_count = std::ranges::count_if(m_tasks, [](const Task& task) { return task.hidden; });

    List(false);

    std::ranges::for_each(m_tasks, [](Task& task) { task.hidden = false; });
    m_hidden_count = 0;
}

void Manager::ListTags() const
{
if (m_tags.empty()) {
        std::cout << "\n📭 No tags available.\n";
        return;
    }

    // **Step 1: Determine Layout**
    size_t max_tag_length = 4;
    std::vector<std::string> tag_list;

    for (const auto& [tag, count] : m_tags) {
        tag_list.push_back(tag);
        max_tag_length = std::max(max_tag_length, tag.length());
    }

    max_tag_length += 2;

    const size_t tag_count = tag_list.size();
    const size_t num_columns = std::min(tag_count, static_cast<size_t>(5));
    const size_t num_rows = (tag_count + num_columns - 1) / num_columns;
    const size_t table_width = (max_tag_length + 3) * num_columns - (num_columns - 1);


    // **Step 2: Print Header**
    std::cout << std::setfill('-') << std::setw(static_cast<int>(table_width)) << "" << std::setfill(' ') << "\n";
    std::cout << "|" << std::setw(static_cast<int>(table_width / 2) + 1) << std::right << "Tags"
              << std::setw(static_cast<int>(table_width / 2) - 1 - (table_width % 2 == 0)) << std::right << "|" << "\n";
    std::cout << std::setfill('-') << std::setw(static_cast<int>(table_width)) << "" << std::setfill(' ') << "\n";

    // **Step 3: Print Tags in Columns**
    size_t index = 0;

    for (size_t row = 0; row < num_rows; ++row) {
        for (size_t col = 0; col < num_columns; ++col) {
            if (index < tag_count) {
                std::cout << "| " << std::setw(static_cast<int>(max_tag_length)) << std::left << tag_list[index++];
            } else {
                std::cout << "| " << std::setw(static_cast<int>(max_tag_length)) << " "; // Empty slot
            }
        }
        std::cout << "|\n";
    }

    // **Step 4: Print Bottom Border**
    std::cout << std::setfill('-') << std::setw(static_cast<int>(table_width)) << "" << std::setfill(' ') << "\n";
}

bool Manager::AddFlagUpdate(const Flag& flag, std::vector<std::string>& values, Task& task)
{
    switch (flag) {
        case Flag::Description:
            task.description = values[0];
        break;

        case Flag::Priority:
            task.priority = GetPriority(values[0]);
        if (task.priority == Priority::Invalid) {
            PrintInvalidValuesError("priority", values[0], PRIORITY_EXPECTED_VALUES);
            return false;
        }
        break;

        case Flag::Due:
            switch (ValidateDateFormat(values[0])) {
                case DateValidationResult::InvalidFormat:
                    PrintInvalidValuesError("due", values[0], DUE_DATE_EXPECTED_FORMAT);
                    return false;
                case DateValidationResult::InvalidValue:
                    PrintInvalidValuesError("due", values[0], DUE_DATE_EXPECTED_DATE);
                    return false;
                default:
                    task.due = values[0];
                break;
            }

        break;

        case Flag::Tags:
            for (auto &tag : values) {
                m_tags.try_emplace(tag, 0).first->second++;
            }
        task.tags = values;
        break;

        default:
            PrintInvalidFlagsError("add", {"description", "priority", "due", "tags"});
            return false;
    }

    return true;
}

bool Manager::EditFlagUpdate(const Flag& flag, std::vector<std::string>& values, const auto& it)
{
    if (flag == Flag::ID) return true;

    switch (flag) {
        case Flag::Description:
            it->description = values[0];
            break;

        case Flag::Priority:
            it->priority = GetPriority(values[0]);
            if (it->priority == Priority::Invalid) {
                PrintInvalidValuesError("priority", values[0], PRIORITY_EXPECTED_VALUES);
                return false;
            }
            break;

        case Flag::Due:
            switch (ValidateDateFormat(values[0])) {
                case DateValidationResult::InvalidFormat:
                    PrintInvalidValuesError("due", values[0], DUE_DATE_EXPECTED_FORMAT);
                    return false;
                case DateValidationResult::InvalidValue:
                    PrintInvalidValuesError("due", values[0], DUE_DATE_EXPECTED_DATE);
                    return false;
                default:
                    it->due = values[0];
                    break;
            }

        case Flag::Tags:
            for (auto &tag : it->tags) {
                if (--m_tags[tag] == 0) {
                    m_tags.erase(tag);
                }
            }
            for (auto &tag : values) {
                m_tags.try_emplace(tag, 0).first->second++;
            }
            it->tags = values;
            break;

        case Flag::Status:
            it->status = GetStatus(values[0]);
            if (it->status == Status::None) {
                PrintInvalidValuesError("status", values[0],
                    "`pending`, `completed`");
                return false;
            }
            break;

        default:
            PrintInvalidFlagsError("edit", {"description", "priority", "due", "tags", "status"});
            return false;
    }

    return true;
}

void Manager::AddToHistory()
{
    if (history.size() >= HISTORY_LIMIT) {
        history.pop_front();
    }

    history.push_back(m_tasks);
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
        if (!AddFlagUpdate(flag, values, task)) {
            return;
        }
    }

    task.id = m_prev_id++;

    m_tasks.push_back(task);

    SortIndirectly(argc, argv);

    std::cout << "📌 Task added successfully! (ID: " << task.id << ")\n";

    AddToHistory();
}

void Manager::List(const bool called_directly) const
{
    if (!m_flags.empty() && called_directly) {
        PrintArgumentError("no flags", "are allowed for this command.");
        return;
    }

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
                [](const size_t sum, const std::string& tag) { return sum + tag.length(); });

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
        std::cout << NO_CHANGE_SPECIFIED_ERROR;
        return;
    }


    try {
        if (!m_flags[Flag::ID][0].empty() && m_flags[Flag::ID][0][0] == '-') {
            PrintInvalidValuesError("id", m_flags[Flag::ID][0], "must be a positive integer.");
            return;
        }

        const unsigned short int id = std::stoi(m_flags[Flag::ID][0]);
        const auto it = std::ranges::find_if(m_tasks.begin(), m_tasks.end(),
            [id](const Task& task) { return task.id == id; });

        if (it == m_tasks.end()) {
            return PrintTaskNotFoundError(m_flags[Flag::ID][0]);
        }

        for (auto& [flag, values] : m_flags) {
            if (!EditFlagUpdate(flag, values, it)) {
                return;
            }
        }

        SortIndirectly(argc, argv);

        std::cout << "✏️  Task (ID: " << it->id << ") updated successfully!\n";

        AddToHistory();
    } catch (const std::exception&) {
        PrintInvalidValuesError("id", m_flags[Flag::ID][0], "integer");
    }
}

void Manager::Delete()
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
        if (!m_flags[Flag::ID][0].empty() && m_flags[Flag::ID][0][0] == '-') {
            PrintInvalidValuesError("id", m_flags[Flag::ID][0], "must be a positive integer.");
            return;
        }

        const unsigned short int id = std::stoi(m_flags[Flag::ID][0]);
        if (const auto it = std::ranges::find_if(m_tasks.begin(), m_tasks.end(),
            [id](const Task& task) { return task.id == id; }); it != m_tasks.end()) {
            for (auto &tag : it->tags) {
                if (--m_tags[tag] == 0) {
                    m_tags.erase(tag);
                }
            }
            m_tasks.erase(it);
            std::cout << "🗑️  Task (ID: " << id << ") deleted successfully!\n";

            AddToHistory();
        } else {
            PrintTaskNotFoundError(m_flags[Flag::ID][0]);
        }
    } catch (const std::exception&) {
        PrintInvalidValuesError("id", m_flags[Flag::ID][0], "integer");
    }
}

void Manager::Complete()
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
        if (!m_flags[Flag::ID][0].empty() && m_flags[Flag::ID][0][0] == '-') {
            PrintInvalidValuesError("id", m_flags[Flag::ID][0], "must be a positive integer.");
            return;
        }

        const unsigned short int id = std::stoi(m_flags[Flag::ID][0]);
        if (const auto it = std::ranges::find_if(m_tasks.begin(), m_tasks.end(),
            [id](const Task& task) { return task.id == id; }); it != m_tasks.end()) {
            it->status = Status::Completed;
            std::cout << "✅ Task (ID: " << id << ") marked as completed!\n";

            AddToHistory();
        } else {
            PrintTaskNotFoundError(m_flags[Flag::ID][0]);
        }
    } catch (const std::exception&) {
        PrintInvalidValuesError("id", m_flags[Flag::ID][0], "integer");
    }
}

void Manager::Search()
{
    const bool description_present = FlagUsed(Flag::Description);
    const bool tags_present = FlagUsed(Flag::Tags);

    if (!description_present && !tags_present) {
        return PrintArgumentError("--description / --tags", "should be present.");
    }

    if (m_flags.size() > (description_present + tags_present)) {
        return PrintInvalidFlagsError("search", {"description", "tags"});
    }

    if (description_present) {
        std::vector<std::string> keywords;
        SplitQuotedText(m_flags[Flag::Description][0], keywords);

        for (auto &task : m_tasks) {
            std::string description = task.description;
            ToLower(description);

            const bool has_any_occurrences = std::ranges::any_of(keywords,
                [&description](const std::string& keyword) {
                    return description.find(keyword) != std::string::npos;
                });

            task.hidden = !has_any_occurrences;
        }
    }

    if (tags_present) {
        std::vector<std::string> required_tags = m_flags[Flag::Tags];

        for (auto &task : m_tasks) {
            const bool has_any_tag = std::ranges::any_of(required_tags,
                [&task](const std::string& tag) {
                    return std::ranges::find_if(task.tags,
                        [&tag](const std::string& task_tag) {
                            return task_tag == tag;
                        }) != task.tags.end();
                });

            task.hidden &= !has_any_tag;
        }
    }

    ListIndirectly();
}

void Manager::Filter()
{
    const bool status_present = FlagUsed(Flag::Status);
    const bool priority_present = FlagUsed(Flag::Priority);
    const bool due_present = FlagUsed(Flag::Due);
    const bool to_present = FlagUsed(Flag::To);

    if (m_flags.size() > (status_present + priority_present + due_present + to_present)) {
        PrintInvalidFlagsError("filter", {"status", "priority", "due", "to"});
        return;
    }

    if ((due_present && ((to_present && m_flags.size() > 2) || (!to_present && m_flags.size() > 1)))
        || (!due_present && m_flags.size() > 1)) {
        PrintArgumentError("filter", "supports only one filter at a time.");
        return;
    }

    if (status_present) {
        const Status filter_status = GetStatus(m_flags[Flag::Status][0]);
        if (filter_status == Status::None) {
            PrintInvalidValuesError("status", m_flags[Flag::Status][0], "`pending` or `completed`");
            return;
        }

        std::ranges::for_each(m_tasks, [filter_status](Task& task) {
            task.hidden = (task.status != filter_status);
        });
    }
    else if (priority_present) {
        const Priority filter_priority = GetPriority(m_flags[Flag::Priority][0]);
        if (filter_priority == Priority::Invalid) {
            PrintInvalidValuesError("priority", m_flags[Flag::Priority][0], PRIORITY_EXPECTED_VALUES);
            return;
        }

        std::ranges::for_each(m_tasks, [filter_priority](Task& task) {
            task.hidden = (task.priority != filter_priority);
        });
    } else {
        std::string start_date;
        std::string end_date;
        if (due_present && !to_present) {
            start_date = m_flags[Flag::Due][0];
            end_date = "9999-12-31";
        } else if (!due_present && to_present) {
            start_date = "1900-01-01";
            end_date = m_flags[Flag::To][0];
        }

        const auto due_result = ValidateDateFormat(start_date);
        if (due_result != DateValidationResult::Success) {
            return PrintInvalidValuesError("due", start_date,
                due_result == DateValidationResult::InvalidFormat ? DUE_DATE_EXPECTED_FORMAT : DUE_DATE_EXPECTED_DATE);
        }

        const auto to_result = ValidateDateFormat(end_date);
        if (to_result != DateValidationResult::Success) {
            return PrintInvalidValuesError("to", end_date,
                to_result == DateValidationResult::InvalidFormat ? DUE_DATE_EXPECTED_FORMAT : DUE_DATE_EXPECTED_DATE);
        }

        if (start_date > end_date) {
            PrintArgumentError("--due", "should be before --to.");
            return;
        }

        std::ranges::for_each(m_tasks, [&start_date, &end_date](Task& task) {
            task.hidden = (task.due < start_date || task.due > end_date);
        });
    }

    ListIndirectly();
}

void Manager::Sort(const bool called_directly)
{
    const bool order_present = FlagUsed(Flag::SortOrder);

    if (called_directly) {
        if (!FlagUsed(Flag::SortBy)) {
            PrintArgumentError("--by", "is required for this command.");
            return;
        }

        if ((!order_present && m_flags.size() > 1) || (order_present && m_flags.size() > 2)) {
            PrintInvalidFlagsError("sort", {"by", "order"});
            return;
        }
    }


    const Flag sort_by = GetFlag(m_flags[Flag::SortBy][0]);

    if (sort_by == Flag::Priority || sort_by == Flag::Due ||
        sort_by == Flag::ID || sort_by == Flag::Status) {
        const Order order = order_present ? GetOrder(m_flags[Flag::SortOrder][0]) : Order::Ascending;

        if (order == Order::None) {
            PrintInvalidValuesError("order", m_flags[Flag::SortOrder][0], "`asc`, `desc`");
            return;
        }

        switch (sort_by) {
            case Flag::Priority:
                std::ranges::stable_sort(m_tasks, [](const Task& a, const Task& b) {
                    return static_cast<int>(a.priority) < static_cast<int>(b.priority);
                });
                break;
            case Flag::Due:
                std::ranges::stable_sort(m_tasks, [](const Task& a, const Task& b) {
                    return a.due < b.due;
                });
                break;
            case Flag::ID:
                std::ranges::stable_sort(m_tasks, [](const Task& a, const Task& b) {
                    return a.id < b.id;
                });
                break;
            default:
                std::ranges::stable_sort(m_tasks, [](const Task& a, const Task& b) {
                    return static_cast<int>(a.status) < static_cast<int>(b.status);
                });
                break;
        }

        if (order == Order::Descending) {
            std::ranges::reverse(m_tasks);
        }

        m_prev_sort = std::make_pair(sort_by, order);

        if (called_directly) {
            std::cout << "🔹 Tasks sorted by `" << m_flags[Flag::SortBy][0] << "` in `" << (order == Order::Ascending ? "ascending" : "descending") << "` order.\n";

            AddToHistory();
        }
    } else {
        PrintInvalidValuesError("sort", m_flags[Flag::SortBy][0], "`priority`, `due`, `id`, `status`");
    }
}

void Manager::Tag()
{
    const bool id_used = FlagUsed(Flag::ID);
    const bool list_used = FlagUsed(Flag::List);

    if (id_used && list_used) {
        PrintArgumentError("--id / --list", "cannot be used together.");
        return;
    }

    if (list_used) {
        if (m_flags.size() > 1) {
            PrintInvalidFlagsError("tag --list", {"list"});
            return;
        }

        ListTags();

        return;
    }

    if (!id_used) {
        PrintArgumentError("--id", "is required for tag manipulation.");
        return;
    }

    if (m_flags.size() != 2) {
        PrintArgumentError("tag", "must include --id and either --add or --remove.");
        return;
    }

    const bool add_used = FlagUsed(Flag::Add);
    const bool remove_used = FlagUsed(Flag::Remove);

    if (add_used == remove_used) {
        PrintArgumentError("tag", "must specify either --add or --remove, but not both or none.");
        return;
    }

    if (!m_flags[Flag::ID][0].empty() && m_flags[Flag::ID][0][0] == '-') {
        PrintInvalidValuesError("id", m_flags[Flag::ID][0], "must be a positive integer.");
        return;
    }

    const unsigned short int id = std::stoi(m_flags[Flag::ID][0]);

    if (const auto it = std::ranges::find_if(m_tasks,
                [id](const Task& task) { return task.id == id; }); it != m_tasks.end()) {
        const std::string tag = m_flags[add_used ? Flag::Add : Flag::Remove][0];

        if (add_used) {
            if (std::ranges::find(it->tags, tag) == it->tags.end()) {
                it->tags.push_back(tag);
                m_tags.try_emplace(tag, 0).first->second++;
                std::cout << "✅ Tag `" << tag << "` added to Task ID: " << id << "\n";

                AddToHistory();
            } else {
                std::cout << "⚠️ Task ID " << id << " already has tag `" << tag << "`.\n";
            }
        } else {
            const auto tag_it = std::ranges::find(it->tags, tag);
            if (tag_it != it->tags.end()) {
                if (--m_tags[tag] == 0) {
                    m_tags.erase(tag);
                }
                it->tags.erase(tag_it);
                it->tags.shrink_to_fit();
                std::cout << "🗑️  Tag `" << tag << "` removed from Task ID: " << id << "\n";

                AddToHistory();
            } else {
                std::cout << "⚠️ Task ID " << id << " does not have tag `" << tag << "`.\n";
            }
        }
    } else {
        PrintTaskNotFoundError(m_flags[Flag::ID][0]);
    }
}

void Manager::Undo()
{
    if (!m_flags.empty()) {
        PrintArgumentError("no flags", "are allowed for this command.");
        return;
    }

    if (history.size() == 1) {
        std::cout << "❌ Error: No actions to undo!\n";
        return;
    }

    std::cout << "🔄 Last action undone successfully!\n";
    history.pop_back();
    m_tasks = history.back();
}

void Manager::Help()
{
    std::cout << "\n📖 Task Manager CLI - Help Guide\n";
    std::cout << "===========================================\n";

    // 📌 Task Management Commands
    std::cout << "📌 Task Management Commands:\n";
    std::cout << "  ➕ `add`        - Add a new task (Requires: --description) [Optional: --priority, --due, --tags]\n";
    std::cout << "  🗑  `delete`     - Remove a task (Requires: --id)\n";
    std::cout << "  ✅ `complete`   - Mark a task as completed (Requires: --id)\n";
    std::cout << "  ✏️  `edit`       - Modify a task (Requires: --id) [Optional: --description, --priority, --due, --tags, --status]\n\n";

    // 📋 Viewing & Searching Commands
    std::cout << "📋 Viewing & Searching Commands:\n";
    std::cout << "  📋 `list`       - Show all tasks (No flags required)\n";
    std::cout << "  🔍 `search`     - Find tasks (Requires: --description or --tags)\n";
    std::cout << "  🔎 `filter`     - Filter tasks (Use one of: --status, --priority, --due & --to)\n";
    std::cout << "  🔀 `sort`       - Sort tasks (Requires: --by) [Optional: --order]\n\n";

    // 🏷️ Tag Management Commands
    std::cout << "🏷️ Tag Management:\n";
    std::cout << "  🏷️  `tag` --list               - Show all used tags (No other flags required)\n";
    std::cout << "  🏷️  `tag` --id N --add tag     - Add a tag to a task (Requires: --id & --add)\n";
    std::cout << "  🏷️  `tag` --id N --remove tag  - Remove a tag from a task (Requires: --id & --remove)\n\n";

    // ⚙️ General Commands
    std::cout << "⚙️ General Commands:\n";
    std::cout << "  🆘 `help`       - Show this guide\n";
    std::cout << "  ❌ `exit`       - Exit the Task Manager\n\n";

    // 🚩 Flags and Usage
    std::cout << "🚩 Flags and Usage:\n";
    std::cout << "     --description [TEXT]                 - Description of the task (Required for `add`)\n";
    std::cout << "     --priority [high|medium|low|none]    - Task priority (Optional for `add`, `edit`, `filter`)\n";
    std::cout << "     --due [YYYY-MM-DD]                   - Due date of the task (Optional for `add`, `edit`, `filter`)\n";
    std::cout << "     --to [YYYY-MM-DD]                    - End date for filtering (Used with `--due` in `filter`)\n";
    std::cout << "     --tags [tag1 tag2 ...]               - Tags for categorization (Optional for `add`, `edit`)\n";
    std::cout << "     --id [NUMBER]                        - Specify task ID (Required for `edit`, `delete`, `complete`, `tag`)\n";
    std::cout << "     --status [pending|completed]         - Change task status (For `edit`, `filter`)\n";
    std::cout << "     --by [priority|due|id]               - Sorting criteria (Required for `sort`)\n";
    std::cout << "     --order [asc|desc]                   - Sorting order (Default: asc)\n";
    std::cout << "     --add [TAG]                          - Add a tag to a task (Used with `tag --id`)\n";
    std::cout << "     --remove [TAG]                       - Remove a tag from a task (Used with `tag --id`)\n";
    std::cout << "     --list                               - List all available tags (Used with `tag` command)\n\n";

    // 💡 Examples
    std::cout << "💡 Examples:\n";
    std::cout << "  ➕ Add a new task:\n";
    std::cout << "     add --description \"Finish project\" --priority high --due 2025-02-10 --tags work urgent\n";
    std::cout << "  🗑  Delete a task:\n";
    std::cout << "     delete --id 3\n";
    std::cout << "  ✅ Mark a task as completed:\n";
    std::cout << "     complete --id 5\n";
    std::cout << "  🔍 Search tasks by tag:\n";
    std::cout << "     search --tags important\n";
    std::cout << "  🔀 Sort tasks by priority (descending):\n";
    std::cout << "     sort --by priority --order desc\n";
    std::cout << "  🏷️  Add a tag to a task:\n";
    std::cout << "     tag --id 3 --add important\n";
    std::cout << "  🏷️  Remove a tag from a task:\n";
    std::cout << "     tag --id 3 --remove urgent\n";
    std::cout << "  🏷️  List all tags:\n";
    std::cout << "     tag --list\n";

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
        case Command::Delete:   Delete();               break;
        case Command::Complete: Complete();             break;
        case Command::Search:   Search();               break;
        case Command::Filter:   Filter();               break;
        case Command::Sort:     Sort();                 break;
        case Command::Tag:      Tag();                  break;
        case Command::Undo:     Undo();                 break;
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
            if (flag_str == "list") {
                m_flags[flag] = {"list"};
                continue;
            }
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
