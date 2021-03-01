#ifndef USERCONTROLLER_H
#define USERCONTROLLER_H

#include "lib.h"
#include "User.h"

class Request;
class Responce;

class UserController
{
    UserController();
public:
    static UserController& get_instance();
    UserController(UserController&) = delete;
    UserController(UserController&&) = delete;

    void validate_request(Request& m_request, Responce& m_responce);

    std::shared_ptr<User>   find_user_by_login(std::string login);
    std::shared_ptr<User>   find_user_by_token(std::string token);

private:
    void    init_data_base();
    void    init_user_list();

    void    authorization_actions();

private:
    std::vector<std::shared_ptr<User>>   m_list_user;

    static std::string  m_general_root_folder_id;
};

#endif // USERCONTROLLER_H
