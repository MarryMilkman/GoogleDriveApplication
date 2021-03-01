#ifndef RESPONCE_H
#define RESPONCE_H

#include <lib.h>

class Responce
{
public:
    Responce() = default;
    Responce(std::shared_ptr<CkJsonObject> json_responce);

    std::string form_general_responce();
    std::string form_responce_file_data();

    std::shared_ptr<CkSocket> get_socket();
    CkByteData get_file_data();
    void set_socket(std::shared_ptr<CkSocket> socket);
    void set_file_data(CkByteData& received_file_data);

    void set_token(std::string token);
    std::string get_token() const;

    void set_user_controller_error(std::string error);
    std::string get_user_controller_error() const;

private:
    std::shared_ptr<CkJsonObject>   m_json_responce_google_drive;
    std::shared_ptr<CkSocket>       m_socket;
    CkByteData                      m_file_data;

    std::string                     m_user_controller_error;
    std::string                     m_token;
};

#endif // RESPONCE_H
