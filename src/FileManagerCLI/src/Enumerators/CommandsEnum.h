//
// Created by arsen on 2/10/25.
//

#ifndef COMMANDSENUM_H
#define COMMANDSENUM_H

enum class Command
{
    sudo,
    exit,
    useradd,
    passwd,
    none
};

enum class Flag
{
    /* sudo */
    sudo_v,
    sudo_k,
    sudo_u,
    sudo_h,
    sudo_i,

    /* useradd */
    useradd_m,
    useradd_s,
    useradd_g,

    /* passwrd */
    passwd_d,

    none
};

#endif //COMMANDSENUM_H
