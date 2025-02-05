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
     */
    static Command GetCommand(std::string &command_str);
    static Flag GetFlag(std::string &flag_str);
    static Priority GetPriority(std::string &priority_str);
    static Status GetStatus(std::string &status_str);

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
     */
    bool FlagUsed(const Flag &flag) const;
    static bool IsFlag(const std::string &flag);
    static void PrintExitMessage();
    static DateValidationResult ValidateDateFormat(std::string &date);
    static void ToLower(std::string &str);
    static void SplitQuotedText(const std::string& input, std::vector<std::string>& output);

    /* Error Handling Methods:
     * ------------------------------------------------------------------------------
     * - PrintCommandNotFoundError      -> Prints an error if an invalid command is given.
     * - PrintInvalidDueDateFormatError -> Prints an error if the due date format is incorrect.
     * - PrintTaskNotFoundError         -> Prints an error if a task with the given ID doesn't exist.
     * - PrintInvalidFlagsError         -> Prints an error for invalid flags.
     * - PrintInvalidValuesError        -> Prints an error for invalid flag values.
     * - PrintArgumentError             -> Generic error for argument-related issues.
     */
    static void PrintCommandNotFoundError(const std::string &command);
    static void PrintInvalidDueDateFormatError(const std::string &date);
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
     * - Help     -> Displays available commands and usage information.
     */
    void Add(size_t argc, const std::vector<std::string> &argv);
    void List() const;
    void Edit(size_t argc, const std::vector<std::string> &argv);
    void Delete(size_t argc, const std::vector<std::string> &argv);
    void Complete(size_t argc, const std::vector<std::string> &argv);
    void Search(size_t argc, const std::vector<std::string> &argv);
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
    */
    unsigned short int m_prev_id{1};
    unsigned short int m_hidden_count{};
    std::vector<Task> m_tasks;
    std::unordered_map<Flag, std::vector<std::string>> m_flags;
};



#endif //MANAGER_H
