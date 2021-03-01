#include <lib.h>
#include <UserController.h>
#include <GoogleDriveController.h>
#include <ConnectionController.h>
#include <TaskController.h>


#include <CkAuthGoogle.h>
#include <CkRest.h>
#include <CkJsonObject.h>
#include <cstdio>

#include <boost/program_options.hpp>
#include <exception>

//void ChilkatSample(void)
//{
//    bool success = true;

//    //  It requires the Chilkat API to have been previously unlocked.
//    //  See Global Unlock Sample for sample code.

//    //  This example uses a previously obtained access token having permission for the
//    //  Google Drive scope.

//    CkAuthGoogle gAuth;
//    gAuth.put_AccessToken(GoogleDriveController::get_instance().get_access_token().c_str());

//    CkRest rest;

//    //  Connect using TLS.
//    bool bAutoReconnect = true;
//    success = rest.Connect("www.googleapis.com",443,true,bAutoReconnect);

//    //  Provide the authentication credentials (i.e. the access token)
//    rest.SetAuthGoogle(gAuth);

//    rest.AddHeader("Content-Type","application/json; charset=UTF-8");

//    CkJsonObject json;
//    json.AppendString("role","reader");
//    json.AppendString("type","user");
//    json.AppendString("value","gdmail140@gmail.com");

//    //  A note from a Chilkat customer:
//    //
//    //    My coding attempt was frustrated because I tried a role of "owner".
//    //    Even when providing the extra parameter of ...?transferOwnership=true
//    //    that approach fails (400, Bad Request).
//    //

//    //  This example is inserting a permission for the file whose fileId is "0By-hrxCCDzSQUGxJY0hzNUFpNmc".
//    const char *jsonResponse = rest.fullRequestString("POST","/drive/v2/files/1W5pBKnitkC46b-A_kI4pKEmrFKJNHbtT/permissions",json.emit());
//    if (rest.get_LastMethodSuccess() != true) {
//        std::cout << rest.lastErrorText() << "\r\n";
//        return;
//    }

//    //  A successful response will have a status code equal to 200.
//    if (rest.get_ResponseStatusCode() != 200) {
//        std::cout << "response status code = " << rest.get_ResponseStatusCode() << "\r\n";
//        std::cout << "response status text = " << rest.responseStatusText() << "\r\n";
//        std::cout << "response header: " << rest.responseHeader() << "\r\n";
//        std::cout << "response JSON: " << jsonResponse << "\r\n";
//        return;
//    }

//    //  Show the JSON response.
//    json.Load(jsonResponse);

//    //  Show the full JSON response.
//    json.put_EmitCompact(false);
//    std::cout << json.emit() << "\r\n";

//    std::cout << "Success." << "\r\n";
//}
namespace
{
const static std::string hello_massage =
        "Google Drive File Receiver v?.?.?\n"
        "   Usage:\n"
        "--help: hello massage + usege\n"
        "--usege: hello massage + usege\n"
        "--json_key : (requered) path to json key that need for comunicate with Google drive\n"
        "--port : listen port for new connections from client\n";
}

struct Flags
{
    bool is_init{false};
    int listen_port{5555};
    std::string path_to_json_key{};

    void process_list_values(std::vector<std::string> values)
    {
        if (values.empty())
            return;

        const auto flag = values.front();
        const auto value = values.back();
        try {
            if (flag == "help")
                std::cerr << hello_massage << "\n";
            if (flag == "port")
                listen_port = std::stoi(value);
            if (flag == "json_key")
            {
                is_init = true;
                path_to_json_key = value;
            }
        }  catch (std::exception &e) {
            std::cout << e.what();
        }
    }
};


Flags parser(int ac, char** av){
    Flags flags_struct;
    if (ac == 1)
    {
        return flags_struct;
    }
    std::vector<std::string>    list_flags;
    for (int i = 1; i < ac && av[i] != nullptr; i++)
    {
        list_flags.push_back(av[i]);
        if (list_flags[i - 1].size() > 2)
        {
            list_flags[i - 1].erase(0, 2);
        }
    }

    for (const auto &flag : list_flags)
    {
        std::vector<std::string>    list_value;
        std::string value;
        std::stringstream ss(flag);
        while (getline(ss, value, '='))
        {
            list_value.push_back(value);
        }
        if (list_value.size() > 2)
            continue;
        flags_struct.process_list_values(list_value);
    }
    return flags_struct;
}

int main(int ac, char** av)
{
    Flags flags = parser(ac, av);
    if (!flags.is_init)
    {
        return 0;
    }
    google_drive::path_to_json_key = flags.path_to_json_key;
    connection_controller::listen_port = flags.listen_port;

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

