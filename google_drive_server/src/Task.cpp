#include <Task.h>
#include <GoogleDriveController.h>
#include <ConnectionController.h>

Task::Task(Request request)
    : m_request(request)
{
}

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
Task::work()
{
    switch (m_work_status)
    {
    case WorkStatus::NEED_PROCESS_REQUEST:
        // Processing of request (read socket)
        m_request.processing();
        if (m_request.get_type() == Request::RequestType::UNKNOWN)
        {
            // create responce with error
            // m_responce =
            m_work_status = WorkStatus::NEED_SEND_RESPONCE;
        }
        m_work_status = WorkStatus::NEED_GOOGLE_DATA;
        break;
    case WorkStatus::NEED_GOOGLE_DATA:
        // sending request to google_drive_controller
        m_responce = Responce(request_data_from_google_drive_controller());
        m_responce.set_socket(m_request.get_socket());
        m_work_status = WorkStatus::NEED_PROCCESS_GOOGLE_DATA;
        break;
    case WorkStatus::NEED_PROCCESS_GOOGLE_DATA:
        // checking responce from google_drive_controller
        // if everesing alright - change status to NEED_SEND_RESPONCE
        m_work_status = WorkStatus::NEED_SEND_RESPONCE;
        break;
    case WorkStatus::NEED_SEND_RESPONCE:
        // sending responce using ConnectionController
        ConnectionController::get_instance().send_responce(m_responce);
        m_work_status = WorkStatus::COMPLETE;
        break;
    default:
        break;
    }
}

std::shared_ptr<CkJsonObject>
Task::request_data_from_google_drive_controller()
{
    Request::RequestType type = m_request.get_type();

    switch (type)
    {
    case Request::RequestType::LIST_FILES:
        return GoogleDriveController::get_instance().get_list_files(
                    m_request.get_request_body());
        break;
    case Request::RequestType::DOWNLOAD:
        return GoogleDriveController::get_instance().download_file(
                    m_request.get_request_body());
        break;
    case Request::RequestType::UPLOAD:
        return GoogleDriveController::get_instance().upload_file(
                    m_request.get_request_body());
        break;
    case Request::RequestType::NEW_FOLDER:
        return GoogleDriveController::get_instance().creane_new_folder(
                    m_request.get_request_body());
        break;
    default:
        break;
    }
    return nullptr;
}
