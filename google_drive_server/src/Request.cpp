#include <Request.h>

Request::Request(CkSocket* socket)
{
    m_socket = std::shared_ptr<CkSocket>(socket);
}

void
Request::processing()
{
    m_request_str = m_socket->receiveString();
    std::stringstream ss(m_request_str);
    std::string request_type;
    std::string request_body;

    std::getline(ss, request_type);
    std::getline(ss, request_body);
    if (request_type == "LIST_FILES")
    {
        m_type = RequestType::LIST_FILES;
    }
    else if (request_type == "DOWNLOAD")
    {
        m_type = RequestType::DOWNLOAD;
    }
    else if (request_type == "UPLOAD")
    {
        m_type = RequestType::UPLOAD;
    }
    else if (request_type == "NEW_FOLDER")
    {
        m_type = RequestType::NEW_FOLDER;
    }
    m_request_body = request_body;
}

Request::RequestType
Request::get_type()
{
    return m_type;
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
Request::get_request_body()
{
    return m_request_body;
}
