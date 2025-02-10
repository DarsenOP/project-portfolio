//
// Created by DarsenOP on 2/9/25.
//

#include "../Core/filespch.h"
#include "CommandProcessor.h"

/* Helpers */

void CommandProcessor::Trim(std::string &input)
{
    const size_t start = input.find_first_not_of(" \t");
    const size_t end = input.find_last_not_of(" \t");

    if (start != std::string::npos && end != std::string::npos) {
        input = input.substr(start, end - start + 1);
    }
}

void CommandProcessor::NormalizeSpaces(std::string &input)
{
    input += ' ';
    Trim(input);

    const auto new_end = std::ranges::unique(input.begin(), input.end(),
                [](const char left, const char right) {
                    return std::isspace(left) && std::isspace(right);
                });

    input.erase(new_end.begin(), input.end());
}

/* Parser */

bool CommandProcessor::Parse(const std::string& input)
{
    if (input.empty()) {
        exception = std::make_shared<EmptyInputException>();
        return false;
    }

    std::istringstream iss(input);
    std::string current_arg{};

    const size_t input_size{std::size(input)};
    size_t last_quote_pos{};
    bool inside_quotes{false};

    /* Helper Lambda Function */
    auto CheckIsolatedQuote = [&](size_t i) {
        if (inside_quotes && i != input_size - 1 && input[i + 1] != ' ') {
            exception = std::make_shared<NonIsolatedQuotedArgumentException>(i, input);
            return false;
        }
        if (!inside_quotes && i != 0 && input[i - 1] != ' ') {
            exception = std::make_shared<NonIsolatedQuotedArgumentException>(i, input);
            return false;
        }

        return true;
    };

    for (size_t i = 0; i < input_size; i++) {
        if (input[i] == '"') {
            if (inside_quotes) {
                if (!CheckIsolatedQuote(i)) { return false; }

                NormalizeSpaces(current_arg);
                if (current_arg == " ") {
                    exception = std::make_shared<EmptyQuotedArgumentException>(last_quote_pos, i, input);
                    return false;
                }

                parsed_result.push_back(std::move(current_arg));
                current_arg.clear();
                inside_quotes = false;
            } else {
                if (!CheckIsolatedQuote(i)) { return false; }
                inside_quotes = true;
                last_quote_pos = i;
            }
        } else if (input[i] == ' ') {
            if (inside_quotes) {
                current_arg += input[i];
            } else {
                if (!current_arg.empty()) {
                    parsed_result.push_back(std::move(current_arg));
                    current_arg.clear();
                }
            }
        } else {
            current_arg += input[i];
        }
    }

    if (inside_quotes) {
        exception = std::make_shared<UnpairedQuoteException>(last_quote_pos, input);
        return false;
    }

    if (!current_arg.empty()) {
        parsed_result.push_back(std::move(current_arg));
    }

    parsed_result_word_count = std::size(parsed_result);

    if (parsed_result_word_count == 0) {
        exception = std::make_shared<EmptyInputException>();
        return false;
    }

    return true;
}

/* Command Handler */

void CommandProcessor::ParseCommand(const std::string& input)
{
    is_valid = Parse(input);
}