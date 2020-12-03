#ifndef TASKCONTROLLER_H
#define TASKCONTROLLER_H

#include <lib.h>

class Task;
class Request;

class TaskController
{
    TaskController();

public:
    static TaskController& get_instance();
    TaskController(TaskController &) = delete;
    TaskController(TaskController &&) = delete;

    void operator()();

    void stop();
    Task& create_task(Request);

private:
    void process_task(Task& task);

private:
    bool                m_stop_flag{false};
    std::vector<Task>   m_list_task;

    std::mutex          m_task_mutex;

};

#endif // TASKCONTROLLER_H
