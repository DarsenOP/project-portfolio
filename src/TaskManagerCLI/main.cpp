//
// Created by DarsenOP on 2/3/25.
//

#include "taskpch.h"
#include "Manager.h"

/*
 * GetInput(std::string& input, std::vector<std::string>& output)
 * ------------------------------------------------------------------------------
 * Parses user input, splitting it into separate words.
 * - Words inside **double quotes ("...")** are treated as a **single argument**
 *   regardless of spaces within them.
 * - Extra spaces between words are ignored.
 */
void GetInput(const std::string &input, std::vector<std::string> &output, bool& empty_quote);

int main(const int argc, const char *argv[])
{
    /* Entry Point - Initializes Task Manager and Processes User Commands */

    if (argc == 1 || (argc > 1 && static_cast<std::string>(argv[0]) == "/home/arsen/Documents/dev/project-portfolio/src/TaskManagerCLI/tasks")) {
        Manager manager;

        while (true) {
            std::string input_str;
            std::cout << "tasks> ";
            std::getline(std::cin, input_str);

            std::vector<std::string> args;
            bool empty_quotes;
            GetInput(input_str, args, empty_quotes);

            if (args.empty()) continue;

            if (empty_quotes) {
                std::cout << "❌ Error: Quoted text cannot be empty or contain only spaces.\n"
                          << "🔹 Example of correct usage: search --description \"important task\"\n"
                          << "🔹 Incorrect: search --description \"   \" (only spaces inside quotes)\n";
                continue;
            }

            if (manager.HandleCommand(args.size(), args) == RunStatus::Exit) {
                return 0;
            }
        }
    } else {
        std::cerr << "❌ Invalid command: `" << argv[0] << "`\n"
                  << "To enter Task Manager CLI, type: `tasks`\n";
        return 1;
    }
}

void GetInput(const std::string& input, std::vector<std::string>& output, bool& empty_quote)
{
    std::string current_arg;
    bool inside_quotes{false};
    empty_quote = false;

    for (const char c : input) {
        if (c == ' ' && !inside_quotes) {
            if (!current_arg.empty()) {
                output.push_back(std::move(current_arg));
                current_arg.clear();
            }
        } else if (c == '"') {
            inside_quotes = !inside_quotes;

            if (!inside_quotes && !current_arg.empty()) {
                if (std::ranges::all_of(current_arg.begin(), current_arg.end(),
                    [](const char chr){ return std::isspace(chr); })) {
                    empty_quote = true;
                    return;
                }
                output.push_back(std::move(current_arg));
                current_arg.clear();
            } else if (!inside_quotes && current_arg.empty()) {
                empty_quote = true;
                return;
            }
        } else {
            current_arg += c;
        }
    }

    // It is possible that the last word was not pushed into the vector, so we explicitly check for it
    if (!current_arg.empty()) {
        output.push_back(std::move(current_arg));
    }
}
