#ifndef FILE_WORKER_H
#define FILE_WORKER_H

#include <QThread>
#include <QMetaType>
#include <memory>
#include <mutex>

#include "Task.h"
#include "File.h"

class FileWorker: public QObject/*: public QThread*/
{
    Q_OBJECT
public:

    enum class TypeRequest
    {
        SERVER_REQUEST = 0,
        LIST_FILES,
        PARENT_LIST
    };

    enum class Status
    {
        NEED_UPDATE_SCREEN = 0,
        DOWNLOADING_COMPLETE,
        UPLOUDING_COMPLETE,
        ERROR
    };

public:
    FileWorker();

//    void run() override;

//    void    add_request(Request& request);
    void stop();

    const std::map<QString, std::shared_ptr<File>>& get_current_list_files() const;
    std::shared_ptr<File>                           get_worker_parent() const;
    void                                            set_perent(std::shared_ptr<File> parent_ptr);

public slots:
    void    slot_task_status(const QString status);
    void    slot_task_finished();

    void    slot_add_request(Request request);

private:
    void process_task_responce(std::shared_ptr<Task>& task);
    void update_files(std::shared_ptr<Task>& task);
    void preprocess_request(Request& request);

signals:
    void    signal_new_status(QString status_str);
    void    signal_need_update_winow(FileWorker::Status,
                                     QJsonDocument);

private:
    bool                                        m_in_work{true};
    std::atomic<bool>                           m_some_task_completed;
    std::vector<std::shared_ptr<Task>>          m_list_tasks;
    std::map<QString, std::shared_ptr<File>>    m_list_files;
    std::shared_ptr<File>                       m_parent;

    std::mutex                                  m_task_mutex;
    std::mutex                                  m_file_mutex;
};

Q_DECLARE_METATYPE(FileWorker::Status)

#endif // FILEWORKER_H
