//
// Created by arsen on 2/9/25.
//

#ifndef EXCEPTION_H
#define EXCEPTION_H

#include "../../Enumerators/LogEnums.h"
#include <exception>

class FileManagerException : public std::exception
{
public:
    FileManagerException() = default;

    [[nodiscard]] virtual LogLevel GetLogLevel() const = 0;
};

#endif //EXCEPTION_H
