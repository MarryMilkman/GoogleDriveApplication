#include "Request.h"

Request::Request(const Request& request)
    : m_type(request.m_type)
    , m_id(request.m_id)
    , m_name(request.m_name)
    , m_file_info(request.m_file_info)
    , m_file_path(request.m_file_path)
{

}

Request::Request(RequestType type,
                 QString id,
                 QString name,
                 QJsonObject file_info)
    : m_type(type)
    , m_id(id)
    , m_name(name)
    , m_file_info(file_info)
{
}

QJsonObject
Request::get_json_request() const
{
    std::string reuest_type_str;
    switch (m_type)
    {
    case RequestType::LIST_FILES:
        reuest_type_str = "LIST_FILES";
        break;
    case RequestType::DOWNLOAD:
        reuest_type_str = "DOWNLOAD";
        break;
    case RequestType::UPLOAD:
        reuest_type_str = "UPLOAD";
        break;
    case RequestType::NEW_FOLDER:
        reuest_type_str = "NEW_FOLDER";
        break;
    case RequestType::REMOVE:
        reuest_type_str = "REMOVE";
        break;
    default:
        break;
    }

    QJsonObject object
    {
        {"type", reuest_type_str.c_str()},
        {"id", m_id},
        {"name", m_name},
        {"file_info", m_file_info}
    };

    return object;
}
