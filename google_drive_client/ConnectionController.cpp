#include "ConnectionController.h"
#include <thread>


ConnectionController::ConnectionController()
    : m_tcpSocket(new QTcpSocket)
{
    QObject::connect( m_tcpSocket, SIGNAL(readyRead()), SLOT(read_tcp_data()) );
    QObject::connect( m_tcpSocket, SIGNAL(readChannelFinished()), SLOT(proccess_received_tcp_data()) );
}

ConnectionController::~ConnectionController()
{
}

ConnectionController&
ConnectionController::get_instance()
{
    static ConnectionController connection_controller;

    return connection_controller;
}

void
ConnectionController::operator()()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        if (!m_queue_request.empty()) {
            {
                std::lock_guard<std::mutex> lock(m_queue_request_mutex);

                m_processed_request = m_queue_request.front();
                m_queue_request.pop();
            }
            proccess_request(m_processed_request);
        }
    }
}

void
ConnectionController::add_request_to_queue(Request new_request)
{
    std::lock_guard<std::mutex> lock(m_queue_request_mutex);

    m_queue_request.push(new_request);
}

void
ConnectionController::proccess_request(Request& request)
{
    QJsonDocument Doc(request.get_json_request());
    QByteArray byte_array = Doc.toJson();

    m_tcpSocket->connectToHost("localhost", 5555);
    if( m_tcpSocket->waitForConnected() ) {
        m_tcpSocket->write( byte_array );
    } else {
//        m_processed_request.m_callback_status("Error: Failed to connect");
    }
//    m_processed_request.m_callback_status("Waiting for a response from the server ...");
    if (request.m_type == Request::RequestType::UPLOAD)
    {
        upload_file(request);
    }
}

void
ConnectionController::upload_file(Request& request)
{
    QJsonDocument doc(request.get_json_request());
    QByteArray byte_array = doc.toJson();

    qint64 sended_bytes = 0;
    qint64 file_size = doc["file_size"].toInt();
    QFile file(request.m_file_path);
    bool is_file_open = file.open(QIODevice::ReadOnly);
    if (!is_file_open)
    {
//        request.m_callback_status("Error: Can't open file");
        return;
    }

    while (sended_bytes < file_size)
    {
        size_t bytes_to_read = 65536;
        QByteArray file_byte_data = file.read(bytes_to_read);

        sended_bytes += m_tcpSocket->write( file_byte_data );
        bool success = m_tcpSocket->waitForBytesWritten(10000);
        if (!success)
        {
//            request.m_callback_status("Error: Failed to send file");
            return;
        }
        std::string info = std::to_string(sended_bytes) + "bytes sended from " + std::to_string(file_size);
//        request.m_callback_status(info.c_str());
    }
//    request.m_callback_status("Waiting for a response from the server ...");
}

void
ConnectionController::read_tcp_data()
{
    QByteArray full_data;
    QByteArray new_data;

    new_data = m_tcpSocket->read(65536);
    m_full_data += new_data;
    std::string info = "Received " + std::to_string(new_data.size()) + "bytes. Full data size: " + std::to_string(m_full_data.size());
//    m_processed_request.m_callback_status(info);
}

void
ConnectionController::proccess_received_tcp_data()
{
    QJsonDocument doc = QJsonDocument::fromJson(m_full_data);
//    m_processed_request.m_callback_finished(m_processed_request.m_type, doc);
//    QJsonValue data_for_response_bar = doc["general_response"];

//    m_ui->responce_bar->setText(data_for_response_bar.toString());

//    size_t responce_size = m_full_data.size();
//    int file_size = doc["file_size"].toInt();
//    if (file_size)
//    {
//        QByteArray base64_file_data = doc["file_data"].toString().toStdString().c_str();

//        if (base64_file_data.size() != file_size)
//        {
//            std::string size_fail_info = "Size mast be " + std::to_string(doc["file_size"].toInt()) +
//                                         ", but actual recaived size is " + std::to_string(base64_file_data.size());
//            m_ui->responce_bar->setText(QString(size_fail_info.c_str()));
//            m_full_data.clear();
//            m_tcpSocket->close();
//            return;
//        }

//        QByteArray file_data;

//        file_data = QByteArray::fromBase64(base64_file_data);

//        QDir dir;
//        QString received_folder_path("D:/all_with_prog/GoogleAPI/Received_folder/");

//        if(!dir.exists(received_folder_path))
//        {
//            m_ui->responce_bar->setText(QString("Folder with downloaded files not exsit. Check please!"));
//            m_full_data.clear();
//            m_tcpSocket->close();
//            return;
//        }

//        QFile file(received_folder_path + m_ui->file_to_download_name->toPlainText());

//        file.open(QIODevice::WriteOnly);
//        file.write(file_data);
//        file.close();
//        m_ui->responce_bar->setText(QString("File downloaded successfully!"));
//    }

//    m_full_data.clear();
//    m_tcpSocket->close();
}
