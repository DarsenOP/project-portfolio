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

    void HandleCommand(size_t& argc, std::vector<std::string>& argv);
private:
    /* Getter Methods:
     *      GetCommand  -> takes the command as a string and returns corresponding Command struct member
     *      GetFlag     -> takes the flag as a string and returns corresponding Flag struct member
     *      GetPriority -> takes the priority as a string and returns corresponding Priority struct member
     *      GetStatus   -> takes the status as a string and returns corresponding Priority struct member
     */
    Command GetCommand(std::string& command);
    Flag GetFlag(std::string& flag_str);
    Priority GetPriority(std::string& priority_str);
    Status GetStatus(std::string& status_str);

    /* Converter Methods:
     *      These methods do the opposite of the getter functions, that is
     *      - take a Struct member and return the string version of it.
     */
    std::string GetStrPriority(const Priority& priority) const;

    /* Main Functionality Methods:
     *      Add -> Adds a task to the task manager
     *      List -> Lists all the tasks available in the task manager
     *      Edit -> Edits a task with a certain ID
     *      Delete -> Deletes a task with a certain ID
     *      Complete -> Marks a task with a certain ID as completed
     *      Search -> TODO: Add comment when done implementing it
     *      Help -> Gives an output to the user for help, including all the options, flags, priorities, etc.
     */
    void Add(size_t& argc, std::vector<std::string>& argv);
    void List() const;
    void Edit(size_t& argc, std::vector<std::string>& argv);
    void Delete(size_t& argc, std::vector<std::string>& argv);
    void Complete(size_t& argc, std::vector<std::string>& argv);
    void Search(size_t& argc, std::vector<std::string>& argv) const;
    void Help() const;

    /* Flag-Value Related:
     *      InitFlagMap  -> Initializes flag-value map, that is for every flag (for example --id)
     *                      contains the corresponding value (for example 7 for the flag --id)
     *      ClearFlagMap -> Because this map is per-command after running the command it should be cleared.
     *                      The function implements exactly that: the cleanup of the map
     */
    void InitFlagMap(size_t& argc, std::vector<std::string>& argv);
    void ClearFlagMap();
private:
    /* Member Variables:
     *      m_tasks   -> A container of all tasks
     *      m_prev_id -> An integer that provides the ID of the previously added task (for giving ID to the current one)
     *      m_flags   -> Flag-Value map that links the flag with its corresponding value
     */
    std::vector<Task> m_tasks;
    unsigned int m_prev_id{1};
    std::unordered_map<Flag, std::string> m_flags;
};



#endif //MANAGER_H
