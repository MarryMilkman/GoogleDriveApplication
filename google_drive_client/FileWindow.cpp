#include "FileWindow.h"
#include "ui_FileWindow.h"


FileWindow::FileWindow(QWidget *parent) :
    QWidget(parent),
    m_ui(new Ui::FileWindow)
//    m_tcpSocket(new QTcpSocket)
{
    m_ui->setupUi(this);
    QObject::connect(&m_worker,
                     &FileWorker::signal_new_status,
                     this,
                     &FileWindow::slot_new_status,
                     Qt::ConnectionType::QueuedConnection);
    QObject::connect(&m_worker,
                     &FileWorker::signal_need_update_winow,
                     this,
                     &FileWindow::slot_need_update,
                     Qt::ConnectionType::QueuedConnection);

    QObject::connect(this,
                     &FileWindow::signal_add_request,
                     &m_worker,
                     &FileWorker::slot_add_request,
                     Qt::ConnectionType::QueuedConnection);
    m_worker.moveToThread(&m_worker_thread);
    m_worker_thread.start();
//    m_worker.start();
//    connect( m_tcpSocket, SIGNAL(readyRead()), SLOT(read_tcp_data()) );
//    connect( m_tcpSocket, SIGNAL(readChannelFinished()), SLOT(proccess_received_tcp_data()) );

    update_files_view();
}

FileWindow::~FileWindow()
{
    delete m_ui;
    m_worker.stop();
//    m_worker.quit();
//    m_worker.wait();
}

//void
//FileWindow::execude_task(Request &request)
//{
//    m_task_ptr = std::make_unique<Task>(request);
//    Task* task_ptr = m_task_ptr.get();
//    QObject::connect( task_ptr,
//                      SIGNAL(signal_status(const QString)),
//                      this,
//                      SLOT(slot_task_status(const QString)));
//    QObject::connect( task_ptr,
//                      SIGNAL(signal_finished()),
//                      this,
//                      SLOT(slot_task_finished()));

//    task_ptr->execude();
//}

QString
form_file_info_str(const File& file)
{
    QString file_info;

    file_info += file.get_full_name() + " | " + file.get_extension() + " | " + file.get_modifiedTime();
    return file_info;
}

void
FileWindow::update_files_view()
{
    QStringList list_file_info_str;
    const std::map<QString, std::shared_ptr<File>>& list_files
            = m_worker.get_current_list_files();

    list_file_info_str.append("<--Parent-->");
    for (auto file_iterator : list_files)
    {
        if (file_iterator.second)
        {
            const File& file = *file_iterator.second;

            list_file_info_str.append(form_file_info_str(file));
        }

    }
    m_list_model.setStringList(list_file_info_str);
    m_ui->file_list_view->setModel(&m_list_model);
//    list.append("First");
}

void FileWindow::on_list_files_button_clicked()
{
//    on_request_status("Hello!");
    Request request(Request::RequestType::LIST_FILES,
                    m_ui->list_files_parent_id->toPlainText(),
                    "");
    qRegisterMetaType<Request>();
    emit signal_add_request(request);

//    request.m_callback_status = [this](std::string status)
//    {
//        this->on_request_status(status);
//    };
//    request.m_callback_finished = [this](Request::RequestType type, QJsonDocument doc)
//    {
//        on_request_finished(type, doc);
//    };

//    ConnectionController::get_instance().add_request_to_queue(request);
}

void
FileWindow::on_new_folder_button_clicked()
{
    QString folder_name = m_ui->folder_name_fild->toPlainText();
    if (folder_name.isEmpty())
    {
        m_ui->responce_bar->setText(QString("Error: Please input folder name"));
        return;
    }
    Request request(Request::RequestType::NEW_FOLDER,
                    m_ui->new_folder_parent_id->toPlainText(),
                    folder_name);
    qRegisterMetaType<Request>();
    emit signal_add_request(request);
//    request.m_callback_status = [this](std::string status)
//    {
//        on_request_status(status);
//    };
//    request.m_callback_finished = [this](Request::RequestType type, QJsonDocument doc)
//    {
//        on_request_finished(type, doc);
//    };
//    ConnectionController::get_instance().add_request_to_queue(request);
}

void FileWindow::on_upload_file_button_clicked()
{
    QString path_to_file = m_ui->file_path_fild->toPlainText();
    if (path_to_file.isEmpty())
    {
        m_ui->responce_bar->setText(QString("Error: Please input path to file"));
        return;
    }
    QFile file(path_to_file);
    bool is_file_open = file.open(QIODevice::ReadOnly);
    if (!is_file_open)
    {
        QMessageBox::information(0, "info", file.errorString());
        m_ui->responce_bar->setText(QString("Error: Can't open file"));
        return;
    }

    QFileInfo fileInfo(file);
    QJsonObject json_file_info
    {
        {"size", 0},
        {"type", "image/jpeg"}
    };

    json_file_info["size"] = fileInfo.size();
    Request request(Request::RequestType::UPLOAD,
                    m_ui->upload_parent_id->toPlainText(),
                    fileInfo.fileName(),
                    json_file_info);

    request.m_file_path = path_to_file;
    qRegisterMetaType<Request>();
    emit signal_add_request(request);
//    request.m_callback_status = [this](std::string status)
//    {
//        on_request_status(status);
//    };
//    request.m_callback_finished = [this](Request::RequestType type, QJsonDocument doc)
//    {
//        on_request_finished(type, doc);
//    };
//    ConnectionController::get_instance().add_request_to_queue(request);
}

//void
//FileWindow::read_tcp_data()
//{
//    QByteArray full_data;
//    QByteArray new_data;

//    new_data = m_tcpSocket->read(65536);
//    m_full_data += new_data;
//    std::string info = "Received " + std::to_string(new_data.size()) + "bytes. Full data size: " + std::to_string(m_full_data.size());

//    m_ui->status_bar->setText(QString(info.c_str()));
//}

//void
//FileWindow::proccess_received_tcp_data()
//{
//    QJsonDocument doc = QJsonDocument::fromJson(m_full_data);
//    QJsonValue data_for_response_bar = doc["general_response"];

//    m_ui->responce_bar->setText(data_for_response_bar.toString());

//    size_t responce_size = m_full_data.size();
//    int file_size = doc["file_size"].toInt();
//    if (file_size)
//    {
//        QByteArray base64_file_data = doc["file_data"].toString().toStdString().c_str();

//        if (base64_file_data.size() != file_size)
//        {
//            std::string size_fail_info = "Size mast be " + std::to_string(doc["file_size"].toInt()) +
//                                         ", but actual recaived size is " + std::to_string(base64_file_data.size());
//            m_ui->responce_bar->setText(QString(size_fail_info.c_str()));
//            m_full_data.clear();
//            m_tcpSocket->close();
//            return;
//        }

//        QByteArray file_data;

//        file_data = QByteArray::fromBase64(base64_file_data);

//        QDir dir;
//        QString received_folder_path("D:/all_with_prog/GoogleAPI/Received_folder/");

//        if(!dir.exists(received_folder_path))
//        {
//            m_ui->responce_bar->setText(QString("Folder with downloaded files not exsit. Check please!"));
//            m_full_data.clear();
//            m_tcpSocket->close();
//            return;
//        }

//        QFile file(received_folder_path + m_ui->file_to_download_name->toPlainText());

//        file.open(QIODevice::WriteOnly);
//        file.write(file_data);
//        file.close();
//        m_ui->responce_bar->setText(QString("File downloaded successfully!"));
//    }

//    m_full_data.clear();
//    m_tcpSocket->close();
//}

void FileWindow::on_remove_file_button_clicked()
{
    QString file_id = m_ui->remove_file_id->toPlainText();
    if (file_id.isEmpty())
    {
        m_ui->responce_bar->setText(QString("Error: Please input file id"));
        return;
    }
    Request request(Request::RequestType::REMOVE,
                    file_id,
                    "");
    qRegisterMetaType<Request>();
    emit signal_add_request(request);
//    request.m_callback_status = [this](std::string status)
//    {
//        on_request_status(status);
//    };
//    request.m_callback_finished = [this](Request::RequestType type, QJsonDocument doc)
//    {
//        on_request_finished(type, doc);
//    };
//    ConnectionController::get_instance().add_request_to_queue(request);
}

void FileWindow::on_download_file_button_clicked()
{
    QString file_id = m_ui->file_to_download_id->toPlainText();
    QString file_name = m_ui->file_to_download_name->toPlainText();
    if (file_id.isEmpty())
    {
        m_ui->responce_bar->setText(QString("Error: Please input file id"));
        return;
    }
    Request request(Request::RequestType::DOWNLOAD,
                    file_id,
                    file_name);
    qRegisterMetaType<Request>();
    emit signal_add_request(request);
//    request.m_callback_status = [this](std::string status)
//    {
//        m_ui->status_bar->setText(QString(status.c_str()));
//    };
//    request.m_callback_finished = [this](Request::RequestType type, QJsonDocument doc)
//    {
//        on_request_finished(type, doc);
//    };
//    ConnectionController::get_instance().add_request_to_queue(request);
}

void
FileWindow::slot_need_update(FileWorker::Status status, QJsonDocument doc)
{
    static_cast<void>(status);
    QJsonValue data_for_response_bar = doc.object()["general_response"];

    m_ui->responce_bar->setText(data_for_response_bar.toString());
    update_files_view();
}

void
FileWindow::slot_new_status(const QString status)
{
    if (!status.isEmpty())
    {
        m_ui->status_bar->setText(status);
    }
}

//void
//FileWindow::slot_task_finished()
//{
//    QJsonValue data_for_response_bar = doc["general_response"];

//    m_ui->responce_bar->setText(data_for_response_bar.toString());

//    int file_size = doc["file_size"].toInt();
//    if (file_size)
//    {
//        QByteArray base64_file_data = doc["file_data"].toString().toStdString().c_str();

//        if (base64_file_data.size() != file_size)
//        {
//            std::string size_fail_info = "Size mast be " + std::to_string(doc["file_size"].toInt()) +
//                                         ", but actual recaived size is " + std::to_string(base64_file_data.size());
//            m_ui->responce_bar->setText(QString(size_fail_info.c_str()));
//            return;
//        }

//        QByteArray file_data;

//        file_data = QByteArray::fromBase64(base64_file_data);

//        QDir dir;
//        QString received_folder_path("D:/all_with_prog/GoogleAPI/Received_folder/");

//        if(!dir.exists(received_folder_path))
//        {
//            m_ui->responce_bar->setText(QString("Folder with downloaded files not exsit. Check please!"));
//            return;
//        }

//        QFile file(received_folder_path + m_ui->file_to_download_name->toPlainText());

//        file.open(QIODevice::WriteOnly);
//        file.write(file_data);
//        file.close();
//        m_ui->responce_bar->setText(QString("File downloaded successfully!"));
//    }
//    switch (type) {
//    case Request::RequestType::LIST_FILES :
//        process_responce_list_file();
//        break;
//    case Request::RequestType::DOWNLOAD :
//        process_responce_download_file();
//        break;
//    case Request::RequestType::NEW_FOLDER :
//        process_responce_new_folder();
//        break;
//    case Request::RequestType::UPLOAD :
//        process_responce_upload_file();
//        break;
//    case Request::RequestType::REMOVE :
//        process_responce_remove_file();
//        break;
//    default:
//        process_responce_default();
//        break;
//    }
//}

void FileWindow::on_file_list_view_doubleClicked(const QModelIndex &index)
{
    int file_index = index.column() - 1;
    QString request_file_id("parent");

    if (file_index >= 0)
    {
        for (auto& map_iter : m_worker.get_current_list_files())
        {
            if (file_index-- == 0)
            {
                std::shared_ptr<File> file = map_iter.second;
                request_file_id = file->get_id();
                break;
            }
        }
    }
    Request request(Request::RequestType::LIST_FILES,
                    request_file_id,
                    "");
    qRegisterMetaType<Request>();
    emit signal_add_request(request);
}
