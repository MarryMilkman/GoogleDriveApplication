#ifndef CONNECTWINDOW_H
#define CONNECTWINDOW_H

#include <QWidget>
#include <QTcpSocket>

namespace Ui {
class ConnectWindow;
}

class ConnectWindow : public QWidget
{
    Q_OBJECT

public:
    explicit ConnectWindow(QWidget *parent = nullptr);
    ~ConnectWindow();

private slots:
    void on_sendRequestButton_clicked();
    void readTcpData();

    void on_newFolderButton_clicked();

private:
    Ui::ConnectWindow* ui;
    QTcpSocket* tcpSocket = nullptr;
};

#endif // CONNECTWINDOW_H
