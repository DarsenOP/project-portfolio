//
// Created by arsen on 2/12/25.
//

#include "Util.h"

void Util::ToLower(std::string &str)
{
    std::ranges::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

Command Util::GetCommand(const std::string &command_str)
{
    if (command_str == "sudo")      return Command::sudo;
    if (command_str == "exit")      return Command::exit;
    if (command_str == "useradd")   return Command::useradd;
    if (command_str == "passwd")    return Command::passwd;

    return Command::none;
}

Flag Util::GetSudoFlag(const std::string &sudo_flag_str)
{
    if (sudo_flag_str == "-v")      return Flag::sudo_v;
    if (sudo_flag_str == "-k")      return Flag::sudo_k;
    if (sudo_flag_str == "-u")      return Flag::sudo_u;
    if (sudo_flag_str == "-h")      return Flag::sudo_h;
    if (sudo_flag_str == "-i")      return Flag::sudo_i;

    return Flag::none;
}

Flag Util::GetUseraddFlag(const std::string &useradd_flag_str)
{
    if (useradd_flag_str == "-m")      return Flag::useradd_m;
    if (useradd_flag_str == "-s")      return Flag::useradd_s;
    if (useradd_flag_str == "-g")      return Flag::useradd_g;

    return Flag::none;
}

Flag Util::GetPasswdFlag(const std::string &passwd_flag_str)
{
    if (passwd_flag_str == "-d")        return Flag::passwd_d;

    return Flag::none;
}