#include "User.h"
#include <chrono>
#include <ctime>

//User::User()
//{

//}

std::string&
User::generate_token()
{
    std::time_t t = std::time(0);
    std::string time = std::ctime(&t);

    token = login + time + password;
    return token;
}
