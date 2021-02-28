#ifndef CONNECTIONCONTROLLER_H
#define CONNECTIONCONTROLLER_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>

#include <Request.h>
#include <queue>
#include <mutex>

class ConnectionController : public QWidget
{
    ConnectionController();
public:
    static ConnectionController& get_instance();
    ~ConnectionController();
    ConnectionController(ConnectionController&) = delete;
    ConnectionController& operator=(ConnectionController&) = delete;

    void operator()();

    void add_request_to_queue(Request new_request);

private:
    void proccess_request(Request &request);
    void upload_file(Request& request);

private:
    QTcpSocket*             m_tcpSocket = nullptr;
    std::queue<Request>     m_queue_request;
    QByteArray              m_full_data;
    std::mutex              m_queue_request_mutex;

    Request                 m_processed_request;

private slots:

    void read_tcp_data();
    void proccess_received_tcp_data();

};

#endif // CONNECTIONCONTROLLER_H
