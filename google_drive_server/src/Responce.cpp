#include <Responce.h>
#include <CkJsonObject.h>

Responce::Responce(std::shared_ptr<CkJsonObject> json_object)
{
    m_json_responce = json_object;
}

std::string
Responce::form_responce()
{
    std::string responce;
    if (m_json_responce != nullptr)
    {
        responce += "SUCCESS\n";
        responce += m_json_responce->emit();
    }
    else
    {
        responce += "ERROR\n";
        responce += "No data\n";
    }
    return responce;
}

void
Responce::set_socket(std::shared_ptr<CkSocket> socket)
{
    m_socket = socket;
}

std::shared_ptr<CkSocket>
Responce::get_socket()
{
    return m_socket;
}
