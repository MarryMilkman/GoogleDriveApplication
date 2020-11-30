#include <ConnectionController.h>

ConnectionController::ConnectionController()
{
    init();
}

ConnectionController&
ConnectionController::getInstance()
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
}

void
ConnectionController::operator()()
{
    while(true)
    {
        listen_new_conection();
        processing_requests();
        clean_complited_requests();
        clean_complited_tasks();
    }
}

void
ConnectionController::listen_new_conection()
{ // check new connection
    CkSocket *connectedSocket = 0;
    connectedSocket = m_listen_socket.AcceptNextConnection(2000);
    if (m_listen_socket.get_LastMethodSuccess() == false) {
        std::cout << m_listen_socket.lastErrorText() << "\r\n";
        return;
    }
    m_list_requests.push_back(connectedSocket);
}

void
ConnectionController::processing_requests()
{
    for (auto &request : m_list_requests)
    {
        auto status = request.get_status();

        switch (status)
        {
        case Request::Status::WAIT_FOR_START:
        {
            request.processing();
            break;
        }
        case Request::Status::COMPLITE:
        {
            CkTask* task = request.get_socket()->SendStringAsync(request.get_responce().c_str());
            task->Run();
            m_list_tasks.push_back(std::shared_ptr<CkTask>(task));
            break;
        }
        default:
            break;
        };
    }
}

void
ConnectionController::clean_complited_requests()
{
    auto last_request = std::remove_if(
        m_list_requests.begin(),
        m_list_requests.end(),
        [](Request request){
            if (request.get_status() == Request::Status::TO_REMOVE)
                return true;
            return false;
        }
    );
    m_list_requests.erase(last_request, m_list_requests.end());
}

void
ConnectionController::clean_complited_tasks()
{
    auto last_task = std::remove_if(
        m_list_tasks.begin(),
        m_list_tasks.end(),
        [](std::shared_ptr<CkTask> task){
            if (task->get_Finished())
                return true;
            return false;
        }
    );
    m_list_tasks.erase(last_task, m_list_tasks.end());
}
