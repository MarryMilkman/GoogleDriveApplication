#include <Task.h>
#include <GoogleDriveController.h>
#include <ConnectionController.h>
#include <UserController.h>

Task::Task(CkSocket* socket)
    : m_socket(std::shared_ptr<CkSocket>(socket))
{
}

//Task::Task(Request request)
//    : m_request(request)
//{
//}

void
Task::set_work_status(WorkStatus status)
{
    if (status >= WorkStatus::COUNT)
    {
        return;
    }
    m_work_status = status;
}

Task::WorkStatus
Task::get_work_status() const
{
    return m_work_status;
}

void
Task::set_ck_task(CkTask* task)
{
    m_ck_task = std::shared_ptr<CkTask>(task);
}

std::shared_ptr<CkTask>
Task::get_ck_task() const
{
    return m_ck_task;
}

void
Task::work()
{
    switch (m_work_status)
    {
    case WorkStatus::NEED_PROCESS_REQUEST:
        // Processing of request (read socket)
        investigation_socket();
//        m_request.processing();
        if (m_work_status == WorkStatus::NEED_SEND_RESPONCE)
        {
            break;
        }
        if (m_request.get_type() == Request::RequestType::UNKNOWN)
        {
            // create responce with error
            // m_responce =
            m_work_status = WorkStatus::NEED_SEND_RESPONCE;
        }
        if (m_request.get_type() == Request::RequestType::UPLOAD)
        {
            unsigned long file_size = m_request.get_file_size();
            while (m_file_data.getSize() < file_size)
            {
                CkByteData data_from_socket;

                if (!m_socket->ReceiveBytes(data_from_socket))
                    break;
                std::cerr << "Received new part of data, size: " << data_from_socket.getSize() << "\n";
//                std::cerr << data_from_socket.to_s() << "\n";
                if (data_from_socket.getSize() <= 0)
                    break;
                m_file_data.append(data_from_socket);
            }
            if (m_file_data.getSize() != m_request.get_file_size())
            {
                std::cerr << "The current file size is not equal to the size in the request\n";
                m_work_status = WorkStatus::NEED_SEND_RESPONCE;
                break;
            }
        }
        m_work_status = WorkStatus::NEED_GOOGLE_DATA;
        break;
    case WorkStatus::NEED_GOOGLE_DATA:
        // sending request to google_drive_controller
        m_responce = Responce(request_data_from_google_drive_controller());
        m_responce.set_socket(m_socket);
        m_work_status = WorkStatus::NEED_PROCCESS_GOOGLE_DATA;
        break;
    case WorkStatus::NEED_PROCCESS_GOOGLE_DATA:
        // checking responce from google_drive_controller
        // if everesing alright - change status to NEED_SEND_RESPONCE
        m_work_status = WorkStatus::NEED_SEND_RESPONCE;
        break;
    case WorkStatus::NEED_SEND_RESPONCE:
        // sending responce using ConnectionController
        send_responce();
        m_work_status = WorkStatus::COMPLETE;
        break;
    default:
        break;
    }
}

void
Task::investigation_socket()
{
    CkByteData byte_data;
    bool success = m_socket->ReceiveBytes(byte_data);
    if (!success)
    {
        std::cerr << "Fail to receive data from client\n";
        return;
    }
    CkJsonObject *json_request = new CkJsonObject;

    json_request->Load(byte_data.to_s());
    m_request.init_by_json(json_request);
    UserController::get_instance().validate_request(m_request, m_responce);

    if (!m_responce.get_user_controller_error().empty() || !m_responce.get_token().empty())
    {
        m_work_status = WorkStatus::NEED_SEND_RESPONCE;
        return;
    }
//    std::string request_type = json_request->stringOf("type");
//    if (request_type == "SIGN_IN" || request_type == "SIGN_UP")
//    {

//    }
//    else
//    {
//    }
}

std::shared_ptr<CkJsonObject>
Task::request_data_from_google_drive_controller()
{
    Request::RequestType type = m_request.get_type();

    switch (type)
    {
    case Request::RequestType::LIST_FILES:
    {
        return GoogleDriveController::get_instance().get_list_files(
                    m_request.get_file_id());
    }
    case Request::RequestType::DOWNLOAD:
    {
        return GoogleDriveController::get_instance().download_file(
                    m_request.get_file_id(), m_file_data);
    }
    case Request::RequestType::UPLOAD:
    {
        return GoogleDriveController::get_instance().upload_file(
                    m_request.get_file_name(), m_request.get_file_type(), m_file_data, m_request.get_file_id());
    }
    case Request::RequestType::NEW_FOLDER:
    {
        return GoogleDriveController::get_instance().creane_new_folder(
                    m_request.get_file_name(), m_request.get_file_id());
    }
    case Request::RequestType::REMOVE:
    {
        return GoogleDriveController::get_instance().remove_file(
                    m_request.get_file_id());
    }
    default:
        break;
    }
    return nullptr;
}

void
Task::send_responce()
{
    ConnectionController &con_controller = ConnectionController::get_instance();
    if (m_request.get_type() == Request::RequestType::DOWNLOAD)
    {
        std::cerr << "size of file to send: " << m_file_data.getSize() << "\n";
        m_responce.set_file_data(m_file_data);
    }
    std::string str_general_responce = m_responce.form_general_responce();

    con_controller.send_data(str_general_responce, m_socket);
    if (m_request.get_type() == Request::RequestType::DOWNLOAD)
    {
        std::string str_file_data = m_responce.form_responce_file_data();
        if (!str_file_data.empty())
        {
            CkBinData bin_data;
            bin_data.AppendString(str_file_data.c_str(), "utf-8");

            CkTask* task = m_socket->SendBdAsync(bin_data, 0, 0);

            set_ck_task(task);
        }
    }
}
