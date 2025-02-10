//
// Created by arsen on 2/9/25.
//

#ifndef EMPTYQUOTEDTEXTEXCEPTION_H
#define EMPTYQUOTEDTEXTEXCEPTION_H

#include "string"
#include "FileManagerException.h"

class EmptyQuotedArgumentException final : public FileManagerException  {
    std::string _input;
    size_t _start_index;
    size_t _end_index;
public:
    EmptyQuotedArgumentException(const size_t start_index, const size_t end_index, std::string input)
        : FileManagerException(),
          _start_index(start_index + 1), _end_index(end_index + 1), _input(std::move(input)) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        static std::string message;
        message = "Empty Quoted Argument Error: Quoted argument is empty. Start index: "
                  + std::to_string(_start_index) + ", End index: " + std::to_string(_end_index)
                  + " in input string: `" + _input + "`";

        return message.c_str();
    }

    [[nodiscard]] inline LogLevel GetLogLevel() const override { return LogLevel::ERROR; }
};

#endif //EMPTYQUOTEDTEXTEXCEPTION_H
