#ifndef REQUEST_H
#define REQUEST_H

#include <QJsonObject>
#include <QObject>

struct Request: public QObject
{
private:
    Q_OBJECT

public:
    enum class RequestType
    {
        UNKNOWN = 0,
        LIST_FILES,
        DOWNLOAD,
        UPLOAD,
        NEW_FOLDER,
        REMOVE,
        GET_ROOT
    };

    ~Request() = default;
    Request() = default;
    Request(const Request&);

    Request(RequestType type,
            QString id,
            QString name,
            QJsonObject file_info = QJsonObject());

    QJsonObject get_json_request() const;

    RequestType m_type = RequestType::UNKNOWN;
    QString     m_id;
    QString     m_name;
    QJsonObject m_file_info;

    QString     m_file_path;

//    // Callbacks
//    std::function<void(std::string)>                m_callback_status;
//    std::function<void(RequestType, QJsonDocument)> m_callback_finished;
};

Q_DECLARE_METATYPE(Request)

#endif // REQUEST_H
