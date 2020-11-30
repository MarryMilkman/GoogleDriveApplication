#pragma once
#include <lib.h>
#include <Request.h>
#include <CkSocket.h>

class ConnectionController
{
private:
    const uint16_t MAX_CONNECTION = 2;

    ConnectionController();
public:
    static ConnectionController& getInstance();
    ConnectionController(ConnectionController&) = delete;
    ConnectionController(ConnectionController&&) = delete;

    void operator()();

private:
    void init();

    void listen_new_conection();
    void processing_requests();
    void clean_complited_requests();
    void clean_complited_tasks();

private:
    CkSocket                                m_listen_socket;

    std::vector<Request>                    m_list_requests;
    std::vector<std::shared_ptr<CkTask>>    m_list_tasks;

};
