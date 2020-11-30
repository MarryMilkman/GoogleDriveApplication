#include <Request.h>

Request::Request(CkSocket* socket)
{
    m_socket = std::shared_ptr<CkSocket>(socket);
}

void
Request::processing()
{
    m_responce = "Hello! It is test response fore you =)";
}

Request::Status
Request::get_status()
{
    return m_status;
};

void
Request::set_status(Request::Status status)
{
    m_status = status;
};

std::shared_ptr<CkSocket>
Request::get_socket()
{
    return m_socket;
}

std::string
Request::get_responce()
{
    return m_responce;
}

