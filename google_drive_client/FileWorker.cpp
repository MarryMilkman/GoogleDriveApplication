#include "FileWorker.h"
#include "QDebug"
#include <QJsonArray>

//void
//FileWorker::run()
//{
//    while (m_in_work)
//    {
////        if (m_some_task_completed)
//        {
//            std::lock_guard<std::mutex> lock(m_task_mutex);

//            for (size_t i = 0; i < m_list_tasks.size(); )
//            {
//                std::shared_ptr<Task> task = m_list_tasks[i];
//                if (task->get_status() == Task::Status::PENDING_EXECUTION)
//                {
//                    task->execude();
//                }
//                else if (task->get_status() == Task::Status::EXECUTED)
//                {
//                    process_task_responce(task);
//                    m_list_tasks.erase(m_list_tasks.begin() + i);
//                    continue;
//                }
//                i++;
//            }
////            m_some_task_completed = false;
//        }
//        msleep(500);
//    }
//}

FileWorker::FileWorker()
{
    m_parent = std::make_shared<File>();
}

void
FileWorker::stop()
{
    m_in_work = false;
}

const std::map<QString, std::shared_ptr<File>>&
FileWorker::get_current_list_files() const
{
    return m_parent->get_childs();
}

std::shared_ptr<File>
FileWorker::get_worker_parent() const
{
    return m_parent;
}

void
FileWorker::set_perent(std::shared_ptr<File> parent_ptr)
{
    if (parent_ptr == nullptr)
    {
        return;
    }
    m_parent = parent_ptr;
}

void
FileWorker::slot_task_status(const QString status)
{
    emit signal_new_status(status);
}

void
FileWorker::slot_task_finished()
{
    std::lock_guard<std::mutex> lock(m_task_mutex);

    for (size_t i = 0; i < m_list_tasks.size(); )
    {
        std::shared_ptr<Task> task = m_list_tasks[i];

        if (task->get_status() == Task::Status::EXECUTED)
        {
            process_task_responce(task);
            m_list_tasks.erase(m_list_tasks.begin() + i);
            continue;
        }
        i++;
    }
}

void
FileWorker::process_task_responce(std::shared_ptr<Task>& task)
{
    Status status = Status::NEED_UPDATE_SCREEN;

    if (task->get_error_code() != Task::ErrorCode::ERROR_NONE)
    {
        status = Status::ERROR;
    }
    else
    {
        if (task->get_request().m_type == Request::RequestType::DOWNLOAD)
        {
            status = Status::DOWNLOADING_COMPLETE;
        }
        else if (task->get_request().m_type == Request::RequestType::UPLOAD)
        {
            status = Status::UPLOUDING_COMPLETE;
        }
    }
    update_files(task);
    QJsonDocument doc = QJsonDocument::fromJson(task->get_json_data());
    qRegisterMetaType<Status>();
    emit signal_need_update_winow(status, doc);
}

void
FileWorker::update_files(std::shared_ptr<Task>& task)
{
    assert(m_parent != nullptr);
    switch (task->get_request().m_type)
    {
    case Request::RequestType::LIST_FILES:
    {
        QJsonDocument doc = QJsonDocument::fromJson(task->get_json_data());
        QJsonArray json_files_arr = doc.object()["general_response"].toObject()["files"].toArray();
        std::map<QString, std::shared_ptr<File>>& list_childs = m_parent->get_childs();

        if (static_cast<long long>(list_childs.size()) != json_files_arr.size())
        {
            for(auto& file_map : m_parent->get_childs())
            {
                auto& file_ptr = file_map.second;
                file_ptr->remove_relationships();
            }
            list_childs.clear();
        }
        int i = 0;

        while (i < json_files_arr.size())
        {
            QJsonValue json_file = json_files_arr[i];
            QString file_id = json_file.toObject()["id"].toString();
            if (list_childs.count(file_id) > 0)
            {
                QString modifiedTime = json_file.toObject()["modifiedTime"].toString();
                if (list_childs[file_id]->get_modifiedTime() != modifiedTime)
                {
                    list_childs[file_id]->init_file(json_file);
                }
            }
            else
            {
                auto file = std::make_shared<File>(json_file);

                file->set_parent(m_parent);
                m_parent->add_child(file);
            }
            i++;
        }
        break;
    }
    }
}

void
FileWorker::slot_add_request(Request request)
{
//    preprocess_request(request);
    qRegisterMetaType<Status>();
    emit signal_need_update_winow(Status::NEED_UPDATE_SCREEN, QJsonDocument());
    std::shared_ptr<Task> task = std::make_shared<Task>(request);

    QObject::connect( task.get(),
                      SIGNAL(signal_status(const QString)),
                      this,
                      SLOT(slot_task_status(const QString)),
                      Qt::ConnectionType::QueuedConnection);
    QObject::connect( task.get(),
                      SIGNAL(signal_finished()),
                      this,
                      SLOT(slot_task_finished()),
                      Qt::ConnectionType::QueuedConnection);
    qDebug() << "New request\n";
    {
        std::lock_guard<std::mutex> lock(m_task_mutex);
        m_list_tasks.push_back(task);
    }
    task->execude();
}

//void
//FileWorker::preprocess_request(Request& request)
//{
//    if (request.m_id == "parent" && !m_list_files.empty())
//    {
//        std::lock_guard<std::mutex> lock(m_file_mutex);
//        std::shared_ptr<File> file_ptr = m_list_files.begin()->second;
//        if (file_ptr != nullptr && file_ptr->get_parent() != nullptr)
//        {
//            m_list_files = file_ptr->get_upper_list();
//            request.m_id = (file_ptr->get_parent()->get_parent() != nullptr)
//                    ? file_ptr->get_parent()->get_parent()->get_id()
//                    : "";
//        }
//    }
//    if (request.m_id == "parent")
//    {
//        request.m_id = "";
//    }
//}
