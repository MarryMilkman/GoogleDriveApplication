#include <GoogleDriveController.h>

std::string GoogleDriveController::s_access_token = "ya29.c.KpwB8gdLPo1fiGSJsLhY-zAQzHN53wMiCJ5Otftwiozqpx32pvQ8Hmg4Y2vTi4kkZ7ocED2B6MUdPLT4ewPXY8SiYWiIzQ98jb10Q8ocnCe7QpuDeT3RvGfaFjJCQ9ufh0UKrfhs8xWfOUMvN0ueLKJBhdrDSxiiV3bves6oR5eXY87qAv9jyFWWL09509unbQfhQrMyehjsIedxJnkj";
GoogleDriveController::GoogleDriveController()
{
    get_list_files("");
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

//std::shared_ptr<CkJsonObject>
//GoogleDriveController::get_parent_info(const std::string file_id)
//{

//}

std::shared_ptr<CkJsonObject>
GoogleDriveController::get_list_files(const std::string parent_id)
{
    if (!connect())
    {
        return nullptr;
    }

    std::shared_ptr<CkJsonObject> json_responce(new CkJsonObject);
    CkStringBuilder sbJson;
    std::string uriPath = "/drive/v3/files";

    if (!parent_id.empty())
    {
        // ?q='BB0CHANGEDIDF5waGdzbUQ5aWs'+in+parents
        uriPath += "?q='" + parent_id + "'+in+parents";
//        std::string fild_parent = "'" + parent_id + "'+in+parents";
//        m_rest.AddQueryParam("q", fild_parent.c_str());
    }

//    m_rest.SetMultipartBodyString(json_responce->emit());

    m_rest.AddQueryParam("fields", "files(kind,id,name,mimeType,parents,modifiedTime)");
    bool success = m_rest.FullRequestNoBodySb("GET", uriPath.c_str(), sbJson);
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
//    json_responce->put_EmitCompact(false);
    return json_responce;
}

std::shared_ptr<CkJsonObject>
GoogleDriveController::creane_new_folder(const std::string folder_name, std::string parent_id)
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

    std::shared_ptr<CkJsonArray> parents = nullptr;
    if (!parent_id.empty())
    {
        parents = std::shared_ptr<CkJsonArray>(json_responce->AppendArray("parents"));
        parents->AddStringAt(-1, parent_id.c_str());
    }

    json_responce->AppendString("name", folder_name.c_str());
    json_responce->AppendString("description","A folder to contain test files.");
    json_responce->AppendString("mimeType","application/vnd.google-apps.folder");


    m_rest.SetMultipartBodyString(json_responce->emit());

    const char *jsonResponse = m_rest.fullRequestMultipart("POST","/upload/drive/v3/files?uploadType=multipart");
    if (m_rest.get_LastMethodSuccess() != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        return nullptr;
    }
    json_responce->Load(jsonResponse);
    if (m_rest.get_ResponseStatusCode() != 200) {
        std::cout << "response status code = " << m_rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "response status text = " << m_rest.responseStatusText() << "\r\n";
        std::cout << "response header: " << m_rest.responseHeader() << "\r\n";
        std::cout << "response JSON: " << *jsonResponse << "\r\n";
        return json_responce;
    }
//    json_responce->put_EmitCompact(false);
    return json_responce;
}

std::shared_ptr<CkJsonObject>
GoogleDriveController::upload_file(const std::string file_name, const std::string file_type, CkByteData& data, const std::string parent_id)
{
    std::shared_ptr<CkJsonObject> json_responce(new CkJsonObject);

    connect();
    m_rest.AddHeader("Content-Type","multipart/related");
    m_rest.put_PartSelector("1");
    m_rest.AddHeader("Content-Type","application/json; charset=UTF-8");

    json_responce->AppendString("name", file_name.c_str());
    json_responce->AppendString("description", "SomeFile");
    json_responce->AppendString("mimeType", file_type.c_str());

    std::shared_ptr<CkJsonArray> parents = nullptr;
    if (!parent_id.empty())
    {
        parents = std::shared_ptr<CkJsonArray>(json_responce->AppendArray("parents"));
        parents->AddStringAt(-1, parent_id.c_str());
    }
    m_rest.SetMultipartBodyString(json_responce->emit());

    //  The 2nd part is the file content, which will contain the binary data.
    m_rest.put_PartSelector("2");
    m_rest.AddHeader("Content-Type", file_type.c_str());

    m_rest.SetMultipartBodyBinary(data);

    const char *jsonResponse = m_rest.fullRequestMultipart("POST","/upload/drive/v3/files?uploadType=multipart");
    if (m_rest.get_LastMethodSuccess() != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        return nullptr;
    }

    //  A successful response will have a status code equal to 200.
    json_responce->Load(jsonResponse);
    if (m_rest.get_ResponseStatusCode() != 200) {
        std::cout << "response status code = " << m_rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "response status text = " << m_rest.responseStatusText() << "\r\n";
        std::cout << "response header: " << m_rest.responseHeader() << "\r\n";
        std::cout << "response JSON: " << jsonResponse << "\r\n";
        return json_responce;
    }

    //  Show the JSON response.
    json_responce->Load(jsonResponse);

    //  Show the full JSON response.
//    json_responce->put_EmitCompact(false);
    std::cout << json_responce->emit() << "\r\n";
    return json_responce;
}

std::shared_ptr<CkJsonObject>
GoogleDriveController::download_file(const std::string file_id, CkByteData& received_file_data)
{
    std::shared_ptr<CkJsonObject> json_responce(new CkJsonObject);

    connect();

    //  ------------------------------------------------------------------------------
    //  To download a file, we must know the file ID.
    //  In a previous example (see Build Local Metadata Cache
    //  we built a local cache to make it easy to lookup file IDs given a file path.
    //  Let's say we want to download "testFolder/abc/123/penguins.jpg".
    //  First we lookup the fileId in the cache.  With the fileId, we can download the file.
    CkCache gdCache;
    gdCache.put_Level(0);
    gdCache.AddRoot("C:/ckCache/googleDrive");

    //  We need to send a GET request like this:
    //  GET https://www.googleapis.com/drive/v3/files/fileId?alt=media
    //  The fileId is part of the path.
    CkStringBuilder sbPath;
    sbPath.Append("/drive/v3/files/");
    sbPath.Append(file_id.c_str());
    m_rest.AddQueryParam("alt","media");

    //  To download to memory, we'll send the request in one call, then receive
    //  the response header, and then the response body.
    //  If the response header indicates failure or an unexpected response, then the
    //  body is not the data we desire.

    //  First send the HTTP request.
    bool success = m_rest.SendReqNoBody("GET", sbPath.getAsString());
    if (success != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        return json_responce;
    }

    //  Get the response header.
    int statusCode = m_rest.ReadResponseHeader();
    if (statusCode != 200) {
        std::cout << "response status code = " << m_rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "response status text = " << m_rest.responseStatusText() << "\r\n";
        std::cout << "response header: " << m_rest.responseHeader() << "\r\n";

        //  Read the response body (which should be error text or HTML)
        const char *errResponseBody = m_rest.readRespBodyString();
        if (m_rest.get_LastMethodSuccess() == true) {
            json_responce->Load(errResponseBody);
            std::cout << "response body: " << errResponseBody << "\r\n";
        }

        return json_responce;
    }

    //  Read the response body, which should contain the file data.
    success = m_rest.ReadRespBodyBinary(received_file_data);
    if (m_rest.get_LastMethodSuccess() != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        return json_responce;
    }

    std::cout << "File downloaded." << "\r\n";
    return json_responce;
}

std::shared_ptr<CkJsonObject>
GoogleDriveController::remove_file(const std::string file_id)
{
    std::shared_ptr<CkJsonObject> json_responce(new CkJsonObject);
    if (!connect())
    {
        return json_responce;
    }

    CkStringBuilder sbPath;
    sbPath.Append("/drive/v3/files/");
    sbPath.Append(file_id.c_str());

    const char *jsonResponse = m_rest.fullRequestNoBody("DELETE",sbPath.getAsString());
    if (m_rest.get_LastMethodSuccess() != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        return json_responce;
    }

    json_responce->Load(jsonResponse);
    //  A successful response will have a status code equal to 204 and the response body is empty.
    //  (If not successful, then there should be a JSON response body with information..)
    if (m_rest.get_ResponseStatusCode() != 204) {
        std::cout << "response status code = " << m_rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "response status text = " << m_rest.responseStatusText() << "\r\n";
        std::cout << "response header: " << m_rest.responseHeader() << "\r\n";
        std::cout << "response JSON: " << jsonResponse << "\r\n";
        return json_responce;
    }

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

    m_rest.ClearAllHeaders();
    m_rest.ClearAllParts();
    m_rest.ClearAllPathParams();
    m_rest.ClearAllQueryParams();
    m_rest.ClearAuth();
    m_rest.ClearResponseBodyStream();

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
