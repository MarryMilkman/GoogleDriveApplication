#ifndef TASK_H
#define TASK_H

#include <lib.h>

#include <Request.h>
#include <Responce.h>

class GoogleDriveController;

class Task
{
public:

    enum class WorkStatus
    {
        NEED_PROCESS_REQUEST = 0,
        NEED_GOOGLE_DATA,
        NEED_PROCCESS_GOOGLE_DATA,
        NEED_SEND_RESPONCE,
        COMPLETE,
        COUNT

    };

    Task(CkSocket* socket);
//    Task(Request request);
    Task() = delete;
    ~Task() = default;

    void        set_work_status(WorkStatus status);
    WorkStatus  get_work_status() const;

    void        set_ck_task(CkTask* task);
    std::shared_ptr<CkTask> get_ck_task() const;

    void        work();

private:
    void                            investigation_socket();
    std::shared_ptr<CkJsonObject>   request_data_from_google_drive_controller();
    void                            send_responce();


private:
    WorkStatus                      m_work_status{WorkStatus::NEED_PROCESS_REQUEST};
    std::shared_ptr<CkSocket>       m_socket;
    Request                         m_request;
    Responce                        m_responce;

    CkByteData                      m_file_data;
    std::shared_ptr<CkTask>         m_ck_task;
//    Responce   m_responce;

};

#endif // TASK_H
