#ifndef RESPONCE_H
#define RESPONCE_H

#include <lib.h>
class CkJsonObject;
class CkSocket;

class Responce
{
public:
    Responce() = default;
    Responce(std::shared_ptr<CkJsonObject> json_responce);

    std::string form_responce();

    std::shared_ptr<CkSocket> get_socket();
    void set_socket(std::shared_ptr<CkSocket> socket);

private:
    std::shared_ptr<CkJsonObject>   m_json_responce;
    std::shared_ptr<CkSocket>       m_socket;
};

#endif // RESPONCE_H
