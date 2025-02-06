//
// Created by DarsenOP on 2/4/25.
//

#ifndef TASKS_H
#define TASKS_H

#include <string>
#include <vector>

// Possible commands
enum class Command
{
    Add,
    List,
    Edit,
    Delete,
    Complete,
    Search,
    Filter,
    Sort,
    Tag,
    Undo,
    Help,
    Exit,
    None
};

// Possible flags to modify commands in intended way
enum class Flag
{
    Description,
    Priority,
    Due,
    Tags,
    ID,
    Status,
    To,
    SortBy,
    SortOrder,
    Add,
    Remove,
    List,
    None
};

// Possible priorities for the tasks
enum class Priority
{
    None,
    Low,
    Medium,
    High,
    Invalid
};

// Possible statuses for the tasks
enum class Status
{
    Pending,
    Completed,
    None
};

enum class Order
{
    Ascending,
    Descending,
    None
};

// Possible outputs when validating dates
enum class DateValidationResult
{
    Success,
    InvalidFormat,
    InvalidValue
};

// Possible outcomes of the output (that is exit the program or not)
enum class RunStatus
{
    Run,
    Exit,
    None
};

// Task structure with all its parameters
struct Task
{
    Task()
        : id(0), priority(Priority::None), status(Status::Pending), due(), tags{}, hidden(false) {}

    unsigned short int id;
    std::string description;
    Priority priority;
    Status status;
    std::string due;
    std::vector<std::string> tags;
    bool hidden;
};

#endif //TASKS_H
