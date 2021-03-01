#include <lib.h>
#include <UserController.h>
#include <GoogleDriveController.h>
#include <ConnectionController.h>
#include <TaskController.h>


#include <CkAuthGoogle.h>
#include <CkRest.h>
#include <CkJsonObject.h>

void ChilkatSample(void)
{
    bool success = true;

    //  It requires the Chilkat API to have been previously unlocked.
    //  See Global Unlock Sample for sample code.

    //  This example uses a previously obtained access token having permission for the
    //  Google Drive scope.

    CkAuthGoogle gAuth;
    gAuth.put_AccessToken(GoogleDriveController::get_instance().get_access_token().c_str());

    CkRest rest;

    //  Connect using TLS.
    bool bAutoReconnect = true;
    success = rest.Connect("www.googleapis.com",443,true,bAutoReconnect);

    //  Provide the authentication credentials (i.e. the access token)
    rest.SetAuthGoogle(gAuth);

    rest.AddHeader("Content-Type","application/json; charset=UTF-8");

    CkJsonObject json;
    json.AppendString("role","reader");
    json.AppendString("type","user");
    json.AppendString("value","gdmail140@gmail.com");

    //  A note from a Chilkat customer:
    //
    //    My coding attempt was frustrated because I tried a role of "owner".
    //    Even when providing the extra parameter of ...?transferOwnership=true
    //    that approach fails (400, Bad Request).
    //

    //  This example is inserting a permission for the file whose fileId is "0By-hrxCCDzSQUGxJY0hzNUFpNmc".
    const char *jsonResponse = rest.fullRequestString("POST","/drive/v2/files/1W5pBKnitkC46b-A_kI4pKEmrFKJNHbtT/permissions",json.emit());
    if (rest.get_LastMethodSuccess() != true) {
        std::cout << rest.lastErrorText() << "\r\n";
        return;
    }

    //  A successful response will have a status code equal to 200.
    if (rest.get_ResponseStatusCode() != 200) {
        std::cout << "response status code = " << rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "response status text = " << rest.responseStatusText() << "\r\n";
        std::cout << "response header: " << rest.responseHeader() << "\r\n";
        std::cout << "response JSON: " << jsonResponse << "\r\n";
        return;
    }

    //  Show the JSON response.
    json.Load(jsonResponse);

    //  Show the full JSON response.
    json.put_EmitCompact(false);
    std::cout << json.emit() << "\r\n";

    std::cout << "Success." << "\r\n";
}


int main()
{
//    ChilkatSample();
    GoogleDriveController& gdc = GoogleDriveController::get_instance();
    static_cast<void>(gdc);
    UserController &usr_controller = UserController::get_instance();
    static_cast<void>(usr_controller);

    TaskController &task_controller = TaskController::get_instance();
    ConnectionController &connection_controller = ConnectionController::get_instance();

    std::thread connection_controller_thread(std::ref(connection_controller));
    std::thread task_controller_thread(std::ref(task_controller));

    if (task_controller_thread.joinable())
        task_controller_thread.join();
    if (connection_controller_thread.joinable())
        connection_controller_thread.join();
    return 0;
}

