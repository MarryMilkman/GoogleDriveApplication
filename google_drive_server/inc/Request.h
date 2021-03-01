#pragma once

#include <lib.h>

class Request
{
public:

    enum class RequestType
    {
        UNKNOWN = 0,
        SIGN_IN,
        SIGN_UP,
        ROOT_FOLDER,
        LIST_FILES,
        DOWNLOAD,
        UPLOAD,
        NEW_FOLDER,
        REMOVE
    };

    enum class Status
    {
        WAIT_FOR_START = 0,
        IN_PROGRES,
        COMPLITE,
        TO_REMOVE,
        ERROR,
        COUNT
    };

    Request();
    ~Request() {};


    bool                        init_by_json(CkJsonObject* json);

    RequestType                 get_type();

    Status                      get_status() const;
    void                        set_status(Status status);

    std::string                 get_token() const;

    size_t                      get_file_size() const;

    // Used by UserController for some RequestType (ROOT_FOLDER)
    void                        set_file_id(std::string file_id);
    std::string                 get_file_id() const;

    std::string                 get_file_name() const;
    std::string                 get_file_type() const;

    std::string                 get_login() const;
    std::string                 get_password() const;

private:
//    std::shared_ptr<CkSocket>       m_socket;


    RequestType                     m_type{RequestType::UNKNOWN};
    Status                          m_status{Status::WAIT_FOR_START};

    std::string                     m_token;

    size_t                          m_file_size{0};
    std::string                     m_file_id;
    std::string                     m_file_name;
    std::string                     m_file_type;

    std::string                     m_login;
    std::string                     m_password;

    std::shared_ptr<CkJsonObject>   m_json_request;
};
