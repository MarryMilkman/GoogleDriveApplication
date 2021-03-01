#include "TaskController.h"
#include "Task.h"
#include "Request.h"

TaskController::TaskController()
{
}

TaskController&
TaskController::get_instance()
{
    static TaskController task_controller;

    return task_controller;
}

void
TaskController::operator()()
{
    while ( !m_stop_flag )
    {
        if (m_list_task.empty())
        {
            continue;
        }
        std::lock_guard<std::mutex> lock(m_task_mutex);
        size_t i = 0;
        while (i < m_list_task.size())
        {
            Task& task = m_list_task[i];
            process_task(task);
            if (task.get_work_status() == Task::WorkStatus::COMPLETE &&
                (task.get_ck_task() == nullptr || task.get_ck_task()->get_Finished() == true))
            {
                m_list_task.erase(m_list_task.begin() + i);
                continue;
            }
            i++;
        }
        i = 0;
//        while (i < m_list_ck_tasks.size())
//        {
//            CkTask* ptr_ck_task = m_list_ck_tasks[i];

//            if (ptr_ck_task != nullptr && ptr_ck_task->get_Finished() == false)
//            {
//                i++;
//                continue;
//            }
//            m_list_ck_tasks.erase(m_list_ck_tasks.begin() + i);
//        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void
TaskController::stop()
{
    m_stop_flag = true;
}

Task&
TaskController::create_task(CkSocket* socket)
{
    std::lock_guard<std::mutex> lock(m_task_mutex);
    Task task(socket);

    m_list_task.push_back(task);
    return m_list_task.back();
}

//void
//TaskController::add_ck_task(CkTask* ck_task)
//{
//    std::lock_guard<std::mutex> lock(m_ck_task_mutex);

//    if (ck_task != nullptr)
//    {
//        m_list_ck_tasks.push_back(ck_task);
//    }
//}


void
TaskController::process_task(Task& task)
{
    task.work();
}
