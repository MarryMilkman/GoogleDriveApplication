#include "GoogleDriveController.h"
#include <CkJsonObject.h>

std::string GoogleDriveController::s_access_token = "ya29.c.KpwB6AeByj1vkPXtPqCaL-d7pIn6qGdrTnBA1tD7f-aWdT8ZP4dN4NXAzeC1UTxzHPLfaN95Z1otulxPAz2lruTVc83NlChXs-1wdSMejvNkZfQ_XaVxId9IVJ3NjfgGsku6Wglipy1n2NZ-oYnMHFNK5IzZq_V_zo-OsE1yqcQrABqjQFML-JmX_m7c2Tyd7imIUJkwqhnNnAf2B1K2";


GoogleDriveController::GoogleDriveController()
{
    get_list_files("test");
//    do_authorization();
}

GoogleDriveController&
GoogleDriveController::get_instance()
{
    static GoogleDriveController google_drive_controller;

    return google_drive_controller;
}

// MARK: - Public Section

std::string
GoogleDriveController::get_access_token() const
{
    return s_access_token;
}

bool
GoogleDriveController::restore_access_token_from_json()
{
    s_access_token.clear();

    CkFileAccess fac;
    const char *jsonKey = fac.readEntireTextFile("../../GoogleAPI/GDFileSystem-da356ada5400.json","utf-8");
    if (fac.get_LastMethodSuccess() != true) {
        std::cout << fac.lastErrorText() << "\r\n";
        return false;
    }

    CkAuthGoogle gAuth;
    gAuth.put_JsonKey(jsonKey);

    // Choose a scope.
    gAuth.put_Scope("https://www.googleapis.com/auth/drive");

    // Request an access token that is valid for this many seconds.
    gAuth.put_ExpireNumSeconds(3600);

    // If the application is requesting delegated access:
    // The email address of the user for which the application is requesting delegated access,
    // then set the email address here. (Otherwise leave it empty.)
    gAuth.put_SubEmailAddress("");

    // Connect to www.googleapis.com using TLS (TLS 1.2 is the default.)
    // The Chilkat socket object is used so that the connection can be established
    // through proxies or an SSH tunnel if desired.
    CkSocket tlsSock;
    bool success = tlsSock.Connect("www.googleapis.com",443,true,5000);
    if (success != true) {
        std::cout << tlsSock.lastErrorText() << "\r\n";
        return false;
    }

    // Send the request to obtain the access token.
    success = gAuth.ObtainAccessToken(tlsSock);
    if (success != true) {
        std::cout << gAuth.lastErrorText() << "\r\n";
        return false;
    }

    s_access_token = gAuth.accessToken();
    // Examine the access token:
    std::cout << "Access Token: " << s_access_token << "\r\n";
    return true;
}


//


std::shared_ptr<CkJsonObject>
GoogleDriveController::get_list_files(const std::string request_body)
{
    if (!connect())
    {
        return nullptr;
    }

    std::shared_ptr<CkJsonObject> json_responce(new CkJsonObject);
    CkStringBuilder sbJson;
    bool success = m_rest.FullRequestNoBodySb("GET","/drive/v3/files",sbJson);
    if (success != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        json_responce->Load(m_rest.lastErrorText());
        return json_responce;
    }

    if (m_rest.get_ResponseStatusCode() != 200) {
        std::cout << "Received error response code: " << m_rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "Response body:" << "\r\n";
        std::cout << sbJson.getAsString() << "\r\n";
        restore_access_token_from_json();
        json_responce->LoadSb(sbJson);
        return json_responce;
    }

    json_responce->LoadSb(sbJson);
    json_responce->put_EmitCompact(false);
    return json_responce;
}

std::shared_ptr<CkJsonObject>
GoogleDriveController::creane_new_folder(const std::string request_body)
{
    std::shared_ptr<CkJsonObject> json_responce(new CkJsonObject);
    if (!connect())
    {
        return nullptr;
    }
    //  A multipart upload to Google Drive needs a multipart/related Content-Type
    m_rest.AddHeader("Content-Type","multipart/related");
    m_rest.put_PartSelector("1");
    m_rest.AddHeader("Content-Type","application/json; charset=UTF-8");

    json_responce->AppendString("name", request_body.c_str());
    json_responce->AppendString("description","A folder to contain test files.");
    json_responce->AppendString("mimeType","application/vnd.google-apps.folder");
    m_rest.SetMultipartBodyString(json_responce->emit());

    const char *jsonResponse = m_rest.fullRequestMultipart("POST","/upload/drive/v3/files?uploadType=multipart");
    if (m_rest.get_LastMethodSuccess() != true) {
     std::cout << m_rest.lastErrorText() << "\r\n";
     return nullptr;
    }

    if (m_rest.get_ResponseStatusCode() != 200) {
     std::cout << "response status code = " << m_rest.get_ResponseStatusCode() << "\r\n";
     std::cout << "response status text = " << m_rest.responseStatusText() << "\r\n";
     std::cout << "response header: " << m_rest.responseHeader() << "\r\n";
     std::cout << "response JSON: " << jsonResponse << "\r\n";
     return nullptr;
    }

    json_responce->Load(jsonResponse);

    json_responce->put_EmitCompact(false);
//    std::cout << json_responce->emit() << "\r\n";

    return json_responce;
}

std::shared_ptr<CkJsonObject>
GoogleDriveController::upload_file(const std::string request_body)
{
    std::shared_ptr<CkJsonObject> json_responce;

    return json_responce;
}

std::shared_ptr<CkJsonObject>
GoogleDriveController::download_file(const std::string request_body)
{
    std::shared_ptr<CkJsonObject> json_responce;

    return json_responce;
}

bool
GoogleDriveController::do_authorization()
{
    restore_access_token_from_json();
    return true;
}

bool
GoogleDriveController::connect()
{
    bool success;

    //   Provide a previously obtained OAuth2 access token.
    CkOAuth2 oauth2;

    oauth2.put_AccessToken(s_access_token.c_str());
    m_rest.SetAuthOAuth2(oauth2);

    success = m_rest.Connect("www.googleapis.com",443,true,true);
    if (success != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
    }
    return success;
}


////  Show the full JSON response.
// json_responce.put_EmitCompact(false);
// std::cout << json_responce.emit() << "\r\n";
