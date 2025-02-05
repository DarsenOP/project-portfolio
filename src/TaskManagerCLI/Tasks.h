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
    None
};

// Possible priorities for the tasks
enum class Priority
{
    High,
    Medium,
    Low,
    None
};

// Possible statuses for the tasks
enum class Status
{
    Pending,
    Completed,
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
