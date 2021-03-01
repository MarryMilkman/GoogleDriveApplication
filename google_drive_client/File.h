#ifndef FILE_H
#define FILE_H

#include <QString>
#include <QJsonDocument>

#include <memory>
#include <vector>
#include <map>

class File
{
public:
    enum class ErrorCode
    {
        ERROR_NONE = 0,
        ERROR_NOT_FOLDER,
        ERROR_INIT,
        ERROR_NULLPTR
    };

    enum class Type
    {
        UNKNOWN = 0,
        FOLDER,
        FILE
    };

public:
    File() = default;
    ~File() = default;
    File(QJsonValue data);

    ErrorCode   init_file(QJsonValue data);
    void        remove_relationships();

    ErrorCode   is_valid() const;

    std::shared_ptr<File>&                      get_parent();
    ErrorCode                                   set_parent(std::shared_ptr<File> parent);

    std::map<QString, std::shared_ptr<File>>&   get_childs();
    ErrorCode                                   add_child(std::shared_ptr<File> child);

    std::map<QString, std::shared_ptr<File>>&   get_upper_list();

    Type get_type() const;
    QString get_full_name() const;
    QString get_extension() const;
    int get_size() const;
    QString get_id() const;
    QString get_modifiedTime() const;

private:

private:
    QString                                     m_full_name;
    QString                                     m_extension;
    size_t                                      m_size = 0;
    QString                                     m_id{""};
    QString                                     m_modifiedTime;

    Type                                        m_type;

    std::shared_ptr<File>                       m_parent = nullptr;
    std::map<QString, std::shared_ptr<File>>    m_list_childs;
};

#endif // FILE_H
