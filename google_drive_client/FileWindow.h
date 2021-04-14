#ifndef FILEWORKER_H
#define FILEWORKER_H

#include <QWidget>
#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QDebug>
#include <QStringListModel>

#include "Request.h"
//#include "ConnectionController.h"
#include "Task.h"
#include "FileWorker.h"

namespace Ui {
class FileWindow;
}

class FileWindow : public QWidget
{
    Q_OBJECT

public:

    enum class FileDownloadStatus
    {
        NOT_IN_USE = 0,
        IN_USE,
        JSON_RESPONCE_RESTORED,
        SIZE_RESTORED
    };

    enum class RequestType
    {
        UNKNOWN = 0,
        LIST_FILES,
        DOWNLOAD,
        UPLOAD,
        NEW_FOLDER,
        REMOVE
    };

    explicit FileWindow(QWidget *parent = nullptr);
    ~FileWindow();

private:

    void update_files_view();

//    void process_responce_list_file();
//    void process_responce_download_file();
//    void process_responce_new_folder();
//    void process_responce_upload_file();
//    void process_responce_remove_file();
//    void process_responce_default();

private slots:
//    void read_tcp_data();
//    void proccess_received_tcp_data();
    void slot_need_update(FileWorker::Status status, QJsonDocument doc);
    void slot_new_status(const QString status);
    // Task scope
//    void slot_task_status(const QString status);
//    void slot_task_finished();

    // UI scope
    void on_list_files_button_clicked();
    void on_new_folder_button_clicked();
    void on_upload_file_button_clicked();
    void on_remove_file_button_clicked();
    void on_download_file_button_clicked();

    void on_file_list_view_doubleClicked(const QModelIndex &index);

signals:
    void signal_add_request(Request& request);
//    void

private:
    Ui::FileWindow*      m_ui;

    FileWorker           m_worker;
    QThread              m_worker_thread;
    QStringListModel     m_list_model;
};

#endif // FILEWORKER_H
