#ifndef TASK_H
#define TASK_H

#include <QWidget>
#include <QFile>
#include <QJsonDocument>
#include <QTcpSocket>
#include <Request.h>
#include <QFile>
#include <memory>

class Task: public QWidget
{
    Q_OBJECT
public:
    enum class ErrorCode
    {
        ERROR_NONE = 0,
        ERROR_NO_RECEIVING_PATH,
        ERROR_REQUEST_TYPE_UNKNOWN,
        ERROR_NO_SUCH_FILE,
        ERROR_CONNECT,
        ERROR_SENDING_DATA,
        ERROR_INVALID_RESPONCE
    };

    enum class Status
    {
        PENDING_EXECUTION = 0,
        SENDING_DATA,
        RECEIVING_DATA,
        EXECUTED
    };

    enum class DownloadingStatus
    {
        NEED_BASE_INFO = 0,
        RECEIVING_DATA
    };

public:
    Task() = default;
    ~Task() = default;
    Task(const Request& request);



    void execude();

    Status              get_status() const;
    ErrorCode           get_error_code() const;
    const Request&      get_request() const;
    const QByteArray&   get_json_data() const;

private:
    void upload_file();

private slots:
    void read_tcp_data();
    void tcp_data_received();

signals:
    void signal_status(const QString status);
    void signal_finished();

private:
    std::unique_ptr<QTcpSocket> m_socket;
    const Request&              m_request;

    Status                      m_status{Status::PENDING_EXECUTION};
    ErrorCode                   m_error_code{ErrorCode::ERROR_NONE};
    QByteArray                  m_json_data;


    // Downloading
    DownloadingStatus           m_downloading_status{DownloadingStatus::NEED_BASE_INFO};
    qint64                      m_file_size{0};
    QString                     m_folder_for_receiving{"D:/all_with_prog/GoogleAPI/Received_folder/"};
    QFile                       m_target_file;
};

#endif // TASK_H
