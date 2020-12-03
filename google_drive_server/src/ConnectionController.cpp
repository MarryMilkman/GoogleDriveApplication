#include <ConnectionController.h>
#include <TaskController.h>
#include "Responce.h"

ConnectionController::ConnectionController()
    : m_task_cotroller(TaskController::get_instance())
{
    init();
}

ConnectionController&
ConnectionController::get_instance()
{
    static ConnectionController connection_controller;

    return connection_controller;
}

void
ConnectionController::init()
{
    bool success = m_listen_socket.BindAndListen(5555, MAX_CONNECTION);
    if (success != true) {
        std::cout << m_listen_socket.lastErrorText() << "\r\n";
        return;
    }
    else
    {
        std::cerr << "ConnectionController: success init\n";
    }
}

void
ConnectionController::operator()()
{
    while( !m_stop_flag )
    {
        listen_new_conection( );
//        processing_requests();
//        clean_complited_requests();
        clean_complited_tasks( );
    }
}

void
ConnectionController::stop()
{
    m_stop_flag = true;
}

void
ConnectionController::send_responce(Responce& responce)
{
    auto socket = responce.get_socket();

    socket->SendString(responce.form_responce().c_str());
    socket->Close(100);
//    CkTask* task = responce.get_socket()->SendStringAsync(
//                       responce.form_responce().c_str());
//    task->Run();
//    {
//        std::lock_guard<std::mutex> lock(m_task_mutex);

//        m_list_chilkat_tasks.push_back(std::shared_ptr<CkTask>(task));
//    }
}


void
ConnectionController::listen_new_conection()
{ // check new connection
    CkSocket *connectedSocket = 0;
    connectedSocket = m_listen_socket.AcceptNextConnection(10000);
    if (m_listen_socket.get_LastMethodSuccess() == false) {
        std::cout << m_listen_socket.lastErrorText() << "\r\n";
        return;
    }
    std::cerr << "NEW CONNECTIO!\n";
    m_task_cotroller.create_task(Request(connectedSocket));
}

//void
//ConnectionController::processing_requests()
//{
//    for (auto &request : m_list_requests)
//    {
//        auto status = request.get_status();

//        switch (status)
//        {
//        case Request::Status::WAIT_FOR_START:
//        {
//            request.processing();
//            break;
//        }
//        case Request::Status::COMPLITE:
//        {
//            CkTask* task = request.get_socket()->SendStringAsync(request.get_responce().c_str());
//            task->Run();
//            m_list_chilkat_tasks.push_back(std::shared_ptr<CkTask>(task));
//            break;
//        }
//        default:
//            break;
//        };
//    }
//}

//void
//ConnectionController::clean_complited_requests()
//{
//    auto last_request = std::remove_if(
//        m_list_requests.begin(),
//        m_list_requests.end(),
//        [](Request request){
//            if (request.get_status() == Request::Status::TO_REMOVE)
//                return true;
//            return false;
//        }
//    );
//    m_list_requests.erase(last_request, m_list_requests.end());
//}

void
ConnectionController::clean_complited_tasks()
{
    if (m_list_chilkat_tasks.empty())
    {
        return;
    }
    std::lock_guard<std::mutex> lock(m_task_mutex);
    auto last_task = std::remove_if(
        m_list_chilkat_tasks.begin(),
        m_list_chilkat_tasks.end(),
        [](std::shared_ptr<CkTask> task){
            if (task->get_Finished())
                return true;
            return false;
        }
    );
    m_list_chilkat_tasks.erase(last_task, m_list_chilkat_tasks.end());
}
