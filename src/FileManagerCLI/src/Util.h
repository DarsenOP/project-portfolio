//
// Created by arsen on 2/12/25.
//

#ifndef UTIL_H
#define UTIL_H

#include "Enumerators/CommandsEnum.h"

class Util {
public:
    static void ToLower(std::string& str);
    static Command GetCommand(const std::string &command_str);
    /* Flags */
    static Flag GetSudoFlag(const std::string &sudo_flag_str);
    static Flag GetUseraddFlag(const std::string &useradd_flag_str);
    static Flag GetPasswdFlag(const std::string &passwd_flag_str);
};



#endif //UTIL_H
