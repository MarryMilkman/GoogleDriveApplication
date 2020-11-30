#include <lib.h>
#include <GoogleDriveController.h>
#include <ConnectionController.h>


int main(int argc, char *argv[])
{
    ConnectionController &connection_controller = ConnectionController::getInstance();

    std::thread connection_controller_thread(std::ref(connection_controller));
    if (connection_controller_thread.joinable())
        connection_controller_thread.join();
    return 0;
}

