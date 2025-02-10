//
// Created by arsen on 2/9/25.
//

#ifndef UNPAIREDQUOTEEXCEPTION_H
#define UNPAIREDQUOTEEXCEPTION_H

#include "FileManagerException.h"

class UnpairedQuoteException final : public FileManagerException {
    std::string _input;
    size_t _index;
public:
    UnpairedQuoteException(const size_t index, std::string input)
        : FileManagerException(), _index(index + 1), _input(std::move(input)) {}

    [[nodiscard]] const char* what() const noexcept override
    {
        static std::string message;
        message = "Unpaired Quote Error: Missing closing quote at index "
                  + std::to_string(_index) + " in input string: `" + _input + "`";

        return message.c_str();
    }

    [[nodiscard]] inline LogLevel GetLogLevel() const override { return LogLevel::ERROR; }
};



#endif //UNPAIREDQUOTEEXCEPTION_H
