#include "UserController.h"
#include "Request.h"
#include "Responce.h"
#include "GoogleDriveController.h"

std::string UserController::m_general_root_folder_id = "1W5pBKnitkC46b-A_kI4pKEmrFKJNHbtT";

UserController::UserController()
{
    // init_data_base();
    init_user_list();
}

UserController&
UserController::get_instance()
{
    static UserController user_controller;

    return user_controller;
}

void
UserController::validate_request(Request& request, Responce& responce)
{
    using Type = Request::RequestType;
    auto type = request.get_type();

    if (type == Type::UNKNOWN)
    {
        responce.set_user_controller_error("Request error: unknown type request");
        return;
    }
    ///////////////////////
    if (request.get_file_id().empty())
        request.set_file_id(m_general_root_folder_id);

    return;
    ///////////////////////////
    if ( type == Type::SIGN_IN || type == Type::SIGN_UP)
    {
        std::shared_ptr<User> user_ptr = find_user_by_login(request.get_login());

        if (type == Type::SIGN_IN)
        {
            if (user_ptr == nullptr || user_ptr->password != request.get_password())
            {
                responce.set_user_controller_error("Authorization error: invalid login or password");
                return;
            }
            else
            {
                std::string &token = user_ptr->generate_token();
                responce.set_token(token);
            }
        }
        else
        {
            if (user_ptr != nullptr)
            {
                responce.set_user_controller_error("Authorization error: a user with this login already exists.");
            }
            std::shared_ptr<User>   new_user = std::make_shared<User>();

            new_user->login = request.get_login();
            new_user->password = request.get_password();
            std::shared_ptr<CkJsonObject> json_obj_ptr = GoogleDriveController::get_instance()
                    .creane_new_folder(user_ptr->login, m_general_root_folder_id);

            if (json_obj_ptr == nullptr || !json_obj_ptr->HasMember("id"))
            {
                responce.set_user_controller_error("Authorization error: unexpected error during creating an account. Try later.");
                return;
            }
            new_user->root_id = json_obj_ptr->stringOf("id");
            responce.set_token(user_ptr->root_id);
            m_list_user.push_back(new_user);
            // m_db_worker.add_new_user(new_user);
        }
        return;
    }
    else
    {
        if (request.get_token().empty())
        {
            responce.set_user_controller_error("Request error: no token");
            return;
        }
        auto user = find_user_by_token(request.get_token());
        if (user == nullptr)
        {
            responce.set_user_controller_error("Request error: invalid token");
            return;
        }
        auto file_id = request.get_file_id();

        if (file_id.empty() && type != Type::UPLOAD)
        {
            request.set_file_id(m_general_root_folder_id);
        }
        else
        {
            size_t count = user->map_files.count(request.get_file_id());

            if (count <= 0)
            {
                responce.set_user_controller_error("Request error: unknown file_id");
                return;
            }
        }

        if (type == Type::LIST_FILES && file_id.empty())
        {
            request.set_file_id(m_general_root_folder_id);
        }
//        else if (type == Type::NEW_FOLDER
//              || type == Type::REMOVE
//              || type == Type::LIST_FILES
//              || type == Type::LIST_FILES)
    }
}

void
UserController::init_user_list()
{

}

std::shared_ptr<User>
UserController::find_user_by_login(std::string login)
{
    return nullptr;
}


std::shared_ptr<User>
UserController::find_user_by_token(std::string login)
{
    return nullptr;
}
