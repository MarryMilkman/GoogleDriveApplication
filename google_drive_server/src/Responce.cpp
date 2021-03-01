#include <Responce.h>

Responce::Responce(std::shared_ptr<CkJsonObject> json_object)
{
    m_json_responce_google_drive = json_object;
}

std::string
Responce::form_general_responce()
{
    CkJsonObject json_responce;

    if (!m_token.empty() || !m_user_controller_error.empty())
    {
        json_responce.SetStringOf("token", m_token.c_str());
        json_responce.SetStringOf("error", m_user_controller_error.c_str());
    }
    else
    {
        if (m_json_responce_google_drive)
        {
            json_responce.AddObjectCopyAt(0, "general_response", *m_json_responce_google_drive);
            // json_responce.Set("general_response", m_json_responce_google_drive->emit());
        }
        if (m_file_data.getSize())
        {
            std::string base64_str = m_file_data.getEncoded("base64");

            json_responce.SetIntOf("file_size", base64_str.size());
        }
    }
    std::cerr << "General response for send:\n" << json_responce.emit() << "\n";

    return json_responce.emit();
}

std::string
Responce::form_responce_file_data()
{
    std::string base64_str = m_file_data.getEncoded("base64");

    return base64_str;
}

void
Responce::set_socket(std::shared_ptr<CkSocket> socket)
{
    m_socket = socket;
}

void
Responce::set_file_data(CkByteData& received_file_data)
{
    m_file_data = received_file_data;
}


std::shared_ptr<CkSocket>
Responce::get_socket()
{
    return m_socket;
}

CkByteData
Responce::get_file_data()
{
    return m_file_data;
}

void
Responce::set_token(std::string token)
{
    m_token = token;
}

std::string
Responce::get_token() const
{
    return m_token;
}


void
Responce::set_user_controller_error(std::string error)
{
    m_user_controller_error = error;
}

std::string
Responce::get_user_controller_error() const
{
    return m_user_controller_error;
}
