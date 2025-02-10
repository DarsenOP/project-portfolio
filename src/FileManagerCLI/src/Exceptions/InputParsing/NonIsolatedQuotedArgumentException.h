//
// Created by arsen on 2/9/25.
//

#ifndef NONISOLATEDQUOTEDARGUMENT_H
#define NONISOLATEDQUOTEDARGUMENT_H

#include "FileManagerException.h"

class NonIsolatedQuotedArgumentException : public FileManagerException {
    std::string _input;
    size_t _index;
public:
    NonIsolatedQuotedArgumentException(const size_t index, std::string input)
    : FileManagerException(), _index(index + 1), _input(std::move(input)) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        static std::string message;
        message = "Non-Isolated Quoted Argument Error: Invalid quote placement at index "
                  + std::to_string(_index) + " in input string: `" + _input + "`";

        return message.c_str();
    }

    [[nodiscard]] inline LogLevel GetLogLevel() const override { return LogLevel::ERROR; }
};



#endif //NONISOLATEDQUOTEDARGUMENT_H
