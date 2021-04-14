#include "File.h"
#include <sstream>
#include <QJsonObject>

File::File(QJsonValue data)
{
    init_file(data);
}

File::ErrorCode
File::init_file(QJsonValue data)
{
    m_full_name = data.toObject()["name"].toString();
    m_extension = data.toObject()["mimeType"].toString();
    m_size = data.toObject()["size"].toInt();
    m_id = data.toObject()["id"].toString();
    m_modifiedTime = data.toObject()["modifiedTime"].toString();

    if (m_extension == "application/vnd.google-apps.folder")
    {
        m_type = Type::FOLDER;
    }
    else if (!m_extension.isEmpty())
    {
        m_type = Type::FILE;
    }

    return is_valid();
}

void
File::remove_relationships()
{
    m_parent = nullptr;
    for(auto& file_map : m_list_childs)
    {
        auto& file_ptr = file_map.second;
        file_ptr->remove_relationships();
    }
    m_list_childs.clear();
}


File::ErrorCode
File::is_valid() const
{
    if (m_id.isEmpty() || m_extension.isEmpty()
        || m_full_name.isEmpty() || m_type != Type::UNKNOWN || m_modifiedTime.isEmpty())
    {
        return ErrorCode::ERROR_INIT;
    }
    return ErrorCode::ERROR_NONE;
}

std::shared_ptr<File>&
File::get_parent()
{
    return m_parent;
}

File::ErrorCode
File::set_parent(std::shared_ptr<File> parent)
{
    if (parent == nullptr)
    {
        return ErrorCode::ERROR_NULLPTR;
    }
    ErrorCode file_check = parent->is_valid();

    if (file_check != ErrorCode::ERROR_NONE)
    {
        return file_check;
    }
    if (parent->get_type() != Type::FOLDER)
    {
        return ErrorCode::ERROR_NOT_FOLDER;
    }
    m_parent = parent;

    return ErrorCode::ERROR_NONE;
}

std::map<QString, std::shared_ptr<File>>&
File::get_childs()
{
    return m_list_childs;
}

File::ErrorCode
File::add_child(std::shared_ptr<File>& child)
{
    if (child == nullptr)
    {
        return ErrorCode::ERROR_NULLPTR;
    }
    ErrorCode file_check = child->is_valid();

    if (file_check != ErrorCode::ERROR_NONE)
    {
        return file_check;
    }
    m_list_childs[child->get_id()] = child;

    return ErrorCode::ERROR_NONE;
}

std::map<QString, std::shared_ptr<File>>&
File::get_upper_list()
{
    return m_parent->get_parent()->get_childs();
}

File::Type
File::get_type() const
{
    return m_type;
}

QString
File::get_full_name() const
{
    return m_full_name;
}

QString
File::get_extension() const
{
    return m_extension;
}

int
File::get_size() const
{
    return m_size;
}

QString
File::get_id() const
{
    return m_id;
}

QString
File::get_modifiedTime() const
{
    return m_modifiedTime;
}
