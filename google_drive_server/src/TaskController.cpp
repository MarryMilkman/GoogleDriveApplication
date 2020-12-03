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
            if (task.get_work_status() == Task::WorkStatus::COMPLETE)
            {
                m_list_task.erase(m_list_task.begin() + i);
                continue;
            }
            i++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void
TaskController::stop()
{
    m_stop_flag = true;
}

Task&
TaskController::create_task(Request request)
{
    std::lock_guard<std::mutex> lock(m_task_mutex);
    Task task(request);
    m_list_task.push_back(task);
}

void
TaskController::process_task(Task& task)
{
    task.work();
}
