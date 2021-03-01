#include <Request.h>

Request::Request()
{
//    CkJsonObject* json_obj = new CkJsonObject;
//    m_json_request = std::shared_ptr<CkJsonObject>(json_obj);
}

bool
Request::init_by_json(CkJsonObject* json)
{
//    CkByteData byte_data;
//    bool success = m_socket->ReceiveBytes(byte_data);
//    if (!success)
//    {
//        std::cerr << "Fail to receive data from client\n";
//        return;
//    }
    m_json_request = std::shared_ptr<CkJsonObject>(json);
    {
        m_json_request->put_EmitCompact(false);
        std::cerr << "Request from client:\n";
        std::cerr << m_json_request->emit() << "\n";
    }

    if (!m_json_request->HasMember("type"))
    {
        return false;
    }
    if (m_json_request->HasMember("token"))
    {
        std::string token = m_json_request->stringOf("token");
    }
    std::string request_type = m_json_request->stringOf("type");
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
        CkJsonObject *js_file_info = nullptr;
        if (m_json_request->HasMember("file_info"))
        {
            js_file_info = m_json_request->ObjectOf("file_info");
        }
        if (js_file_info)
        {
            if (m_json_request->HasMember("size") && m_json_request->HasMember("type"))
            {
                m_file_size = js_file_info->IntOf("size");
                const char* str = js_file_info->stringOf("type");
                if (str)
                {
                    m_file_type = str;
                }
            }
        }
        m_type = RequestType::UPLOAD;
    }
    else if (request_type == "NEW_FOLDER")
    {
        m_type = RequestType::NEW_FOLDER;
    }
    else if (request_type == "REMOVE")
    {
        m_type = RequestType::REMOVE;
    }
    else if (request_type == "SIGN_IN")
    {
        m_type = RequestType::SIGN_IN;
    }
    else if (request_type == "SIGN_UP")
    {
        m_type = RequestType::SIGN_UP;
    }
    else if (request_type == "ROOT_FOLDER")
    {
        m_type = RequestType::ROOT_FOLDER;
    }
    else
    {
        return false;
    }

    if (m_json_request->HasMember("id"))
    {
        m_file_id = m_json_request->stringOf("id");
    }
    if (m_json_request->HasMember("name"))
    {
        m_file_name = m_json_request->stringOf("name");
    }

    if (m_type == RequestType::SIGN_UP || m_type == RequestType::SIGN_IN)
    {
        if (m_json_request->HasMember("login") && m_json_request->HasMember("password"))
        {
            m_login = m_json_request->stringOf("login");
            m_password = m_json_request->stringOf("password");
        }
    }
    return true;
}

Request::RequestType
Request::get_type()
{
    return m_type;
}


Request::Status
Request::get_status() const
{
    return m_status;
};

void
Request::set_status(Request::Status status)
{
    m_status = status;
};

std::string
Request::get_token() const
{
    return m_token;
}



size_t
Request::get_file_size() const
{
    return m_file_size;
}

void
Request::set_file_id(std::string file_id)
{
    m_file_id = file_id;
}

std::string
Request::get_file_id() const
{
    return m_file_id;
}

std::string
Request::get_file_name() const
{
    return m_file_name;
}


std::string
Request::get_file_type() const
{
    return m_file_type;
}

std::string
Request::get_login() const
{
    return m_login;
}


std::string
Request::get_password() const
{
    return m_password;
}
