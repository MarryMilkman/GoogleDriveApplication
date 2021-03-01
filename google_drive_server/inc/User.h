#ifndef USER_H
#define USER_H

#include <lib.h>

struct User
{
    std::string                                             login{};
    std::string                                             password{};

    std::string                                             token{};
    std::string                                             root_id{};
    std::map<std::string /*file_id*/, bool /*is_folder*/>   map_files;

    std::string&     generate_token();
};

#endif // USER_H
