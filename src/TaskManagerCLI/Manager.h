//
// Created by DarsenOP on 2/4/25.
//

#ifndef MANAGER_H
#define MANAGER_H

#include <unordered_map>

#include "Tasks.h"

class Manager final {
public:
    Manager() = default;
    ~Manager() = default;

    /* Command Handling
     * ------------------------------------------------------------------------------
     * If an invalid command is given, the function informs the user.
     * If a valid command is given, it calls ExecuteCommand to execute the command.
     */
    RunStatus HandleCommand(size_t argc, const std::vector<std::string> &argv);
private:
    /* Command Execution
     * ------------------------------------------------------------------------------
     * Calls the corresponding function for the given command.
     */
    void ExecuteCommand(const Command &command, size_t argc, const std::vector<std::string> &argv);

    /* Getter Methods:
     * ------------------------------------------------------------------------------
     * - GetCommand  -> Converts a string command into a `Command` enum.
     * - GetFlag     -> Converts a string flag into a `Flag` enum.
     * - GetPriority -> Converts a string priority into a `Priority` enum.
     * - GetStatus   -> Converts a string status into a `Status` enum.
     * - GetOrder    -> Converts a string order into an `Order` enum.
     */
    static Command GetCommand(std::string &command_str);
    static Flag GetFlag(std::string &flag_str);
    static Priority GetPriority(std::string &priority_str);
    static Status GetStatus(std::string &status_str);
    static Order GetOrder(std::string &order_str);

    /* Converter Methods:
     * ------------------------------------------------------------------------------
     * - GetPriorityStr -> Converts a `Priority` enum into a string.
     * - GetFlagStr     -> Converts a `Flag` enum into a string.
     */
    static std::string GetPriorityStr(const Priority &priority);
    static std::string GetFlagStr(const Flag &flag);

    /* Helper Methods:
     * ------------------------------------------------------------------------------
     * - FlagUsed           -> Checks if the given flag was used in the command.
     * - IsFlag             -> Checks if the given string is a flag (starts with `--`).
     * - PrintExitMessage   -> Prints an exit message.
     * - ValidateDateFormat -> Validates the date format and correctness.
     * - ToLower            -> Converts a string to lowercase.
     * - SplitQuotedText    -> Converts a quoted text with spaces into a vector of all the words in the expression
     * - SortIndirectly     -> After adding, editing resorts the tasks
     * - ListIndirectly     -> After search, filtering automatically shows all the tasks
     * - ListTags           -> Lists all the tags
     * - AddFlagUpdate      -> Updates the task when adding the task
     * - EditFlagUpdate     -> Updates the task when editing the task
     */
    bool FlagUsed(const Flag &flag) const;
    static bool IsFlag(const std::string &flag);
    static void PrintExitMessage();
    static DateValidationResult ValidateDateFormat(std::string &date);
    static void ToLower(std::string &str);
    static void SplitQuotedText(const std::string& input, std::vector<std::string>& output);
    void SortIndirectly(size_t argc, const std::vector<std::string> &argv);
    void ListIndirectly();
    void ListTags() const;
    bool AddFlagUpdate(const Flag& flag, std::vector<std::string>& values, Task& task);
    bool EditFlagUpdate(const Flag& flag, std::vector<std::string>& values, const auto& it);
    void AddToHistory();

    /* Error Handling Methods:
     * ------------------------------------------------------------------------------
     * - PrintCommandNotFoundError      -> Prints an error if an invalid command is given.
     * - PrintTaskNotFoundError         -> Prints an error if a task with the given ID doesn't exist.
     * - PrintInvalidFlagsError         -> Prints an error for invalid flags.
     * - PrintInvalidValuesError        -> Prints an error for invalid flag values.
     * - PrintArgumentError             -> Generic error for argument-related issues.
     */
    static void PrintCommandNotFoundError(const std::string &command);
    static void PrintTaskNotFoundError(const std::string &id);
    static void PrintInvalidFlagsError(const std::string& command, const std::vector<std::string>& valid_flags);
    static void PrintInvalidValuesError(const std::string& flag, const std::string& value, const std::string& expected);
    static void PrintArgumentError(const std::string& arg, const std::string& message);

    /* Main Functionality Methods:
     * ------------------------------------------------------------------------------
     * - Add      -> Adds a task to the task manager.
     * - List     -> Lists all available tasks.
     * - Edit     -> Edits a task by ID.
     * - Delete   -> Deletes a task by ID.
     * - Complete -> Marks a task as completed.
     * - Search   -> Searches for tasks with specific description and tags
     * - Filter   -> Filters the tasks based on status, priority, or date
     * - Sort     -> Sort all the tasks based on id, date, priority, or status
     * - Tag      -> Can add, remove tags to certain tasks, or show all the available tags
     * - Help     -> Displays available commands and usage information.
     */
    void Add(size_t argc, const std::vector<std::string> &argv);
    void List(bool called_directly=true) const;
    void Edit(size_t argc, const std::vector<std::string> &argv);
    void Delete();
    void Complete();
    void Search();
    void Filter();
    void Sort(bool called_directly=true);
    void Tag();
    void Undo();
    static void Help();

    /* Flag-Value Processing:
     * ------------------------------------------------------------------------------
     * - InitFlagMap  -> Initializes a flag-value map.
     *                   Each flag (e.g., `--id`) maps to its corresponding value(s).
     * - ClearFlagMap -> Clears the flag map after processing a command.
     */
    bool InitFlagMap(size_t argc, const std::vector<std::string>& argv);
    void ClearFlagMap();
private:
    /* Member Variables:
     * ------------------------------------------------------------------------------
     * - `m_tasks`          -> Stores all tasks.
     * - `m_prev_id`        -> Tracks the last assigned task ID.
     * - `m_flags`          -> Maps flags to their corresponding values.
     * - `m_hidden_count`   -> Number of hidden tasks
     * - `m_prev_sort`      -> The previous sorting setting to make sure when new task added follow the same sorting
     * - `m_tags`           -> Stores all the tags used in the tasks
     * - `m_prev_states`    -> All previous states of the program for preforming `undo`
    */
    unsigned short int m_prev_id{1};
    unsigned short int m_hidden_count{};
    std::unordered_map<Flag, std::vector<std::string>> m_flags {};
    std::unordered_map<std::string, int> m_tags {};
    std::vector<Task> m_tasks {};
    std::pair<Flag, Order> m_prev_sort {std::make_pair(Flag::None, Order::None)};
    std::deque<std::vector<Task>> history{m_tasks};
};



#endif //MANAGER_H
