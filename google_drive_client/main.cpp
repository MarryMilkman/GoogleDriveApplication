#include "mainwindow.h"
#include <QApplication>
#include <thread>
//#include <ConnectionController.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    ConnectionController &connection_controller = ConnectionController::get_instance();

//    std::thread connection_controller_thread(std::ref(connection_controller));
    MainWindow w;
    w.show();

    int ret_value = a.exec();
//    if (connection_controller_thread.joinable())
//        connection_controller_thread.join();
    return ret_value;
}
