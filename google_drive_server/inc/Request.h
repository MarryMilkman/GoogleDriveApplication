#pragma once

#include <lib.h>
#include <CkSocket.h>
#include <CkTask.h>

class Request
{
public:

    enum class RequestType
    {
        UNKNOWN = 0,
        LIST_FILES,
        DOWNLOAD,
        UPLOAD,
        NEW_FOLDER
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

    Request(CkSocket* socket);
    ~Request() {};


    void                        processing();

    RequestType                 get_type();

    Status                      get_status();
    void                        set_status(Status status);

    std::shared_ptr<CkSocket>   get_socket();
    std::string                 get_request_body();

private:
    std::shared_ptr<CkSocket>   m_socket;
    std::string                 m_request_str;
    std::string                 m_request_body;
    Status                      m_status{Status::WAIT_FOR_START};
    RequestType                 m_type{RequestType::UNKNOWN};

};
