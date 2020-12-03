#ifndef TASK_H
#define TASK_H

#include <lib.h>

#include <Request.h>
#include <Responce.h>
#include <CkJsonObject.h>

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

    Task(Request request);
    Task() = delete;
    ~Task() = default;

    void        set_work_status(WorkStatus status);
    WorkStatus  get_work_status() const;

    void        work();

private:
    std::shared_ptr<CkJsonObject>   request_data_from_google_drive_controller();


private:
    WorkStatus                      m_work_status{WorkStatus::NEED_PROCESS_REQUEST};
    Request                         m_request;

    Responce                        m_responce;
//    Responce   m_responce;

};

#endif // TASK_H
