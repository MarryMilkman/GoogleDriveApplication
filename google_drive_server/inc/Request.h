#pragma once

#include <lib.h>
#include <CkSocket.h>
#include <CkTask.h>

class Request
{
public:

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

    Status                      get_status();
    void                        set_status(Status status);

    std::shared_ptr<CkSocket>   get_socket();
    std::string                 get_responce();

private:
    std::shared_ptr<CkSocket>   m_socket;
    std::string                 m_reqest;
    Status                      m_status;

    std::string                 m_responce;
};
