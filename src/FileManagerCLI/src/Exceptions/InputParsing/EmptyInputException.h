//
// Created by arsen on 2/9/25.
//

#ifndef EMPTYINPUTEXCEPTION_H
#define EMPTYINPUTEXCEPTION_H

#include "string"
#include "FileManagerException.h"

class EmptyInputException : public FileManagerException
{
public:
    EmptyInputException() : FileManagerException() {};

    [[nodiscard]] const char* what() const noexcept override
    {
        static std::string message;
        message = "Empty Input Error: The input provided is empty. Please enter a valid command.";

        return message.c_str();
    }

    [[nodiscard]] inline LogLevel GetLogLevel() const override { return LogLevel::WARN; }
};



#endif //EMPTYINPUTEXCEPTION_H
