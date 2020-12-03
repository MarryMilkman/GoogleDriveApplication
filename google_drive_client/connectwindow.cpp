#include "connectwindow.h"
#include "ui_connectwindow.h"

ConnectWindow::ConnectWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ConnectWindow),
    tcpSocket(new QTcpSocket)
{
    ui->setupUi(this);
    connect( tcpSocket, SIGNAL(readyRead()), SLOT(readTcpData()) );

}

ConnectWindow::~ConnectWindow()
{
    delete ui;
}

void ConnectWindow::on_sendRequestButton_clicked()
{
    std::string data = "LIST_FILES"; // <-- fill with data

    tcpSocket->connectToHost("localhost", 5555);
    if( tcpSocket->waitForConnected() ) {
        tcpSocket->write( data.c_str() );
    }
}

void ConnectWindow::readTcpData()
{
    QByteArray data = tcpSocket->readAll();
    std::string data_str = data.toStdString();
    ui->responceBar->setText(QString(data_str.c_str()));
    tcpSocket->close();
}

void ConnectWindow::on_newFolderButton_clicked()
{
    std::string folder_name = ui->folderNameFild->toPlainText().toStdString();
    if (folder_name.empty())
    {
        ui->responceBar->setText(QString("Error: Please input folder name"));
        return;
    }
    std::string data = "NEW_FOLDER\n";

    data += folder_name;

    tcpSocket->connectToHost("localhost", 5555);
    if( tcpSocket->waitForConnected() ) {
        tcpSocket->write( data.c_str() );
    }
}
