#include "Task.h"
#include <memory>
#include <QDir>

Task::Task(const Request& request)
    : m_socket(std::make_unique<QTcpSocket>())
    , m_request(request)
{
//    m_socket = std::make_unique<QTcpSocket>();
//    m_request = request;
    QTcpSocket* pointer_socet = m_socket.get();
    connect( pointer_socet, &QTcpSocket::readyRead, this, &Task::read_tcp_data );
    connect( pointer_socet, &QTcpSocket::readChannelFinished, this, &Task::tcp_data_received );
}

void
Task::execude()
{
    if (m_request.m_type == Request::RequestType::UNKNOWN)
    {
        m_status = Status::EXECUTED;
        m_error_code = ErrorCode::ERROR_REQUEST_TYPE_UNKNOWN;
        emit signal_status("Fail: RequestType::UNKNOWN");
        return;
    }
    if (m_request.m_type == Request::RequestType::DOWNLOAD)
    {
        QDir dir;
        if (!dir.exists(m_folder_for_receiving))
        {
            m_status = Status::EXECUTED;
            m_error_code = ErrorCode::ERROR_NO_RECEIVING_PATH;
            emit signal_status("Fail: ERROR_NO_RECEIVING_PATH");
            return;
        }
    }
    QJsonDocument Doc(m_request.get_json_request());
    QByteArray byte_array = Doc.toJson();

    m_socket->connectToHost("localhost", 5555);
    if( m_socket->waitForConnected() ) {
        m_status = Status::SENDING_DATA;
        m_socket->write( byte_array );
        m_socket->waitForBytesWritten(1000);
    } else {
        emit signal_status("Error: Failed to connect");
        m_status = Status::EXECUTED;
        m_error_code = ErrorCode::ERROR_CONNECT;
        return;
    }
    emit signal_status("Waiting for a response from the server ...");
    if (m_request.m_type == Request::RequestType::UPLOAD)
    {
        upload_file();
    }
    if (m_request.m_type == Request::RequestType::DOWNLOAD)
    {
        QString file_name = m_folder_for_receiving + m_request.m_name;
        m_target_file.setFileName(m_folder_for_receiving + m_request.m_name);
        m_target_file.open(QIODevice::WriteOnly);
        if (!m_target_file.isOpen())
        {
//            assert(0);
            return;
        }
    }
    return;
}


void
Task::upload_file()
{
    QJsonDocument doc(m_request.get_json_request());
    QByteArray byte_array = doc.toJson();

    qint64 sended_bytes = 0;
    qint64 file_size = doc.object()["file_info"].toObject()["size"].toInt();
    QFile file(m_request.m_file_path);
    bool is_file_open = file.open(QIODevice::ReadOnly);
    if (!is_file_open)
    {
        emit signal_status("Error: Can't open file");
        m_error_code = ErrorCode::ERROR_NO_SUCH_FILE;
        return;
    }

    emit signal_status("Waiting for a response from the server ...");

    while (sended_bytes < file_size)
    {
        qDebug() << "upload_file";
        size_t bytes_to_read = 65536;
        QByteArray file_byte_data = file.read(bytes_to_read);

        sended_bytes += m_socket->write( file_byte_data );
        bool success = m_socket->waitForBytesWritten(5000);
        if (!success)
        {
            emit signal_status("Error: Failed to send file");
            m_error_code = ErrorCode::ERROR_SENDING_DATA;
            m_status = Status::EXECUTED;
            return;
        }
        std::string info = std::to_string(sended_bytes) + "bytes sended from " + std::to_string(file_size);
        emit signal_status(info.c_str());
    }
    m_status = Status::RECEIVING_DATA;
}

void
Task::read_tcp_data()
{
    qDebug()<< "read_tcp_data";
    QByteArray full_data;
    QByteArray new_data = m_socket->read(65536);

    if (m_request.m_type == Request::RequestType::DOWNLOAD)
    {
        if (m_downloading_status == DownloadingStatus::NEED_BASE_INFO)
        {
            m_json_data = new_data;
            QJsonDocument doc = QJsonDocument::fromJson(m_json_data);
            auto file_size_obj = doc.object()["file_size"];

            if (file_size_obj.isNull() || file_size_obj.toInt() <= 0)
            {
                m_error_code = ErrorCode::ERROR_INVALID_RESPONCE;
                m_target_file.close();
                m_socket->close();
                emit signal_finished();
            }
            m_file_size = file_size_obj.toInt();
            m_downloading_status = DownloadingStatus::RECEIVING_DATA;
        }
        else if (m_downloading_status == DownloadingStatus::RECEIVING_DATA)
        {
            m_target_file.write(QByteArray::fromBase64(new_data));

            if (m_target_file.size() > m_file_size)
            {
                m_error_code = ErrorCode::ERROR_INVALID_RESPONCE;
                m_target_file.remove();
                m_target_file.close();
                m_socket->close();
                emit signal_finished();
            }
        }

        return;
    }
    else
    {
       m_json_data += new_data;
    }
    std::string info = "Received " + std::to_string(new_data.size()) + "bytes.";
    emit signal_status(QString(info.c_str()));
}

void
Task::tcp_data_received()
{
    qDebug()<< "tcp_data_received";
    m_status = Status::EXECUTED;
    if (m_request.m_type == Request::RequestType::DOWNLOAD)
    {
        m_target_file.close();
    }
    m_socket->close();
    QJsonDocument doc = QJsonDocument::fromJson(m_json_data);
    emit signal_finished();
}


Task::Status
Task::get_status() const
{
    return m_status;
}

Task::ErrorCode
Task::get_error_code() const
{
    return m_error_code;
}

const Request&
Task::get_request() const
{
    return m_request;
}

const QByteArray&
Task::get_json_data() const
{
    return m_json_data;
}
