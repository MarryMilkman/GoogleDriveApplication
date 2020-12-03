#include <lib.h>
#include <GoogleDriveController.h>
#include <ConnectionController.h>
#include <TaskController.h>

#include <CkJsonObject.h>

int main()
{
    GoogleDriveController::get_instance();
    TaskController          &task_controller = TaskController::get_instance();
    ConnectionController    &connection_controller = ConnectionController::get_instance();

    std::thread connection_controller_thread(std::ref(connection_controller));
    std::thread task_controller_thread(std::ref(task_controller));

    if (task_controller_thread.joinable())
        task_controller_thread.join();
    if (connection_controller_thread.joinable())
        connection_controller_thread.join();
    return 0;
}

