#pragma once
#include <lib.h>
#include <Request.h>
#include <CkSocket.h>

class Responce;
class TaskController;

class ConnectionController
{
private:
    const uint16_t MAX_CONNECTION = 10;

    ConnectionController();
public:
    static ConnectionController& get_instance();
    ConnectionController(ConnectionController&) = delete;
    ConnectionController(ConnectionController&&) = delete;

    void operator()();

    void stop();
    void send_responce(Responce& responce);

private:
    void init();

    void listen_new_conection();
//    void processing_requests();
//    void clean_complited_requests();
    void clean_complited_tasks();


private:
    TaskController&                         m_task_cotroller;

    bool                                    m_stop_flag;
    CkSocket                                m_listen_socket;
    std::vector<std::shared_ptr<CkTask>>    m_list_chilkat_tasks;
    std::mutex                              m_task_mutex;
};
