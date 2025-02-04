//
// Created by DarsenOP on 2/3/25.
//

#include "taskpch.h"
#include "Tasks.h"
#include "Manager.h"

void GetInput(std::string& input, std::vector<std::string>& output);

int main(int argc, char *argv[])
{
    /* Entry Point of The Program */

    auto manager = std::make_unique<Manager>();

    // TODO: Change the "tasks" to the appropriate name
    if (argc == 1 || (argc > 1 && std::string(argv[0]) == "tasks")) {
        while (true) {
            std::string input_str;
            std::cout << "tasks> ";
            std::getline(std::cin, input_str);

            std::vector<std::string> args;
            GetInput(input_str, args);

            size_t arg_count {args.size()};

            manager->HandleCommand(arg_count, args);
        }
    } else {
        std::cout << "Invalid command: Please use `tasks` to enter the task manager.\n";
        return 1;
    }

    return 0;
}

void GetInput(std::string& input, std::vector<std::string>& output)
{
    /* GetInput(std::string& input, std::vector<std::string>& output)
     * ------------------------------------------------------------------------------
     * For a given input by the user the function separates all the input into words,
     * considering "..." as one word, regardless number of space in the "..."
     */

    std::string current_arg;
    bool inside_quotes{false};

    for (size_t i{}; i < input.length(); i++) {
        char c = input[i];

        if (c == ' ' && !inside_quotes) {
            if (!current_arg.empty()) {
                output.push_back(current_arg);
                current_arg.clear();
            }
        } else if (c == '"') {
            inside_quotes = !inside_quotes;

            if (!inside_quotes && !current_arg.empty()) {
                output.push_back(current_arg);
                current_arg.clear();
            }
        } else {
            current_arg += c;
        }
    }

    // It is possible that the last word was not pushed into the vector, so we explicitly check for it
    if (!current_arg.empty()) {
        output.push_back(current_arg);
    }
}
