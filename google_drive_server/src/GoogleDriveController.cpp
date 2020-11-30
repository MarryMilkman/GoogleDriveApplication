#include "GoogleDriveController.h"

std::string GoogleDriveController::s_access_token = "ya29.c.KpwB5gfm-5acNaeb3LFRZqj5Y0hAv8o-Sq5GK6FHJyUgRt2j6k4je6Xz752Eua1MDU0zLp-P1zJNrwBtw0sA4JUJQr3TFCPFSlN0Vxssg9i5deSPIfXcD3w5M13cF0T0NDpEbf2VjRkJa_36rllDtEhYYK3Dq2kjxk415oZN5cIEQMR1LK3KNnzyATyOqjPn5qPnKSpDA3LnJHrvB9HQ";


GoogleDriveController::GoogleDriveController()
{
    do_authorization();
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

bool
GoogleDriveController::show_list_files()
{
    bool success;

    //   Provide a previously obtained OAuth2 access token.
    CkOAuth2 oauth2;

    oauth2.put_AccessToken(s_access_token.c_str());
    m_rest.SetAuthOAuth2(oauth2);

    success = m_rest.Connect("www.googleapis.com",443,true,true);
    if (success != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        return false;
    }

    CkStringBuilder sbJson;
    success = m_rest.FullRequestNoBodySb("GET","/drive/v3/files",sbJson);
    if (success != true) {
        std::cout << m_rest.lastErrorText() << "\r\n";
        return false;
    }

    if (m_rest.get_ResponseStatusCode() != 200) {
        std::cout << "Received error response code: " << m_rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "Response body:" << "\r\n";
        std::cout << sbJson.getAsString() << "\r\n";
        return false;
    }

    CkJsonObject json;
    json.LoadSb(sbJson);

    //  Show the full JSON response.
     json.put_EmitCompact(false);
     std::cout << json.emit() << "\r\n";

     int numFiles = json.SizeOfArray("files");
     int i = 0;
     while (i < numFiles) {
         json.put_I(i);
         std::cout << "name: " << json.stringOf("files[i].name") << "\r\n";
         std::cout << "id: " << json.stringOf("files[i].id") << "\r\n";
         std::cout << "mimeType: " << json.stringOf("files[i].mimeType") << "\r\n";
         std::cout << "-" << "\r\n";
         i = i + 1;
     }
//    std::cout << "Example Completed." << "\r\n";
    return true;
}

bool
GoogleDriveController::upload_file(std::string file_path)
{
    bool success = true;

    //  This example requires the Chilkat API to have been previously unlocked.
    //  See Global Unlock Sample for sample code.

    //  This example uses a previously obtained access token having permission for the
    //  Google Drive scope.
    //  See Get Google Drive OAuth2 Access Token

    CkAuthGoogle gAuth;
    gAuth.put_AccessToken(s_access_token.c_str());

    CkRest rest;

    //  Connect using TLS.
    bool bAutoReconnect = true;
    success = rest.Connect("www.googleapis.com",443,true,bAutoReconnect);

    //  Provide the authentication credentials (i.e. the access token)
    rest.SetAuthGoogle(gAuth);

    //  -------------------------------------------------------------------------
    //  A multipart upload to Google Drive needs a multipart/related Content-Type
    rest.AddHeader("Content-Type","multipart/related");

    //  Specify each part of the request.

    //  The 1st part is JSON with information about the file.
    rest.put_PartSelector("1");
    rest.AddHeader("Content-Type","application/json; charset=UTF-8");

    //  Construct the JSON that will contain the metadata about the file data to be uploaded...
    CkJsonObject json;
    json.AppendString("name", file_path.c_str());
    json.AppendString("description", "A picture of a starfish.");
    json.AppendString("mimeType", "image/jpeg");

    //  To place the file in a folder, we must add a parents[] array to the JSON
    //  and add the folder ID.
    //  In a previous example (see Lookup Google Drive Folder ID
    //  we showed how to find the folder ID for a folder in Google Drive.

    //  Use the folder ID we already looked up..
    const char *folderId = "0";
    CkJsonArray *parents = json.AppendArray("parents");
    parents->AddStringAt(-1,folderId);
    delete parents;

    rest.SetMultipartBodyString(json.emit());

    //  The 2nd part is the file content, which will contain the binary image data.
    rest.put_PartSelector("2");
    rest.AddHeader("Content-Type","image/jpeg");

    CkBinData jpgBytes;
    success = jpgBytes.LoadFile("qa_data/jpg/starfish.jpg");

    //  Add the data to our upload
    rest.SetMultipartBodyBd(jpgBytes);

    const char *jsonResponse = rest.fullRequestMultipart("POST","/upload/drive/v3/files?uploadType=multipart");
    if (rest.get_LastMethodSuccess() != true) {
        std::cout << rest.lastErrorText() << "\r\n";
        return false;
    }

    //  A successful response will have a status code equal to 200.
    if (rest.get_ResponseStatusCode() != 200) {
        std::cout << "response status code = " << rest.get_ResponseStatusCode() << "\r\n";
        std::cout << "response status text = " << rest.responseStatusText() << "\r\n";
        std::cout << "response header: " << rest.responseHeader() << "\r\n";
        std::cout << "response JSON: " << jsonResponse << "\r\n";
        return false;
    }

    json.Load(jsonResponse);

    //  Show the full JSON response.
    json.put_EmitCompact(false);
    std::cout << json.emit() << "\r\n";

    //  A successful response looks like this:
    //  {
    //    "kind": "drive#file",
    //    "id": "0B53Q6OSTWYoldmJ0Z3ZqT2x5MFk",
    //    "name": "starfish.jpg",
    //    "mimeType": "image/jpeg"
    //  }

    //  Get the fileId:
    std::cout << "fileId: " << json.stringOf("id") << "\r\n";
    return true;
}

// MARK: - Private Section

static std::string
get_str(const char* str)
{
    if (!str)
        return std::string();
    return std::string(str);
}

bool
GoogleDriveController::show_folder_id()
{
    bool success = true;

    //  It requires the Chilkat API to have been previously unlocked.
    //  See Global Unlock Sample for sample code.

    //  This example uses a previously obtained access token having permission for the
    //  Google Drive scope.

    CkAuthGoogle gAuth;
    gAuth.put_AccessToken(s_access_token.c_str());

    CkRest rest;

    //  Connect using TLS.
    bool bAutoReconnect = true;
    success = rest.Connect("www.googleapis.com",443,true,bAutoReconnect);

    //  Provide the authentication credentials (i.e. the access token)
    rest.SetAuthGoogle(gAuth);

    CkJsonObject json;
    json.put_EmitCompact(false);

    //  Get the AAWorkArea folder that is in the Google Drive root.
    rest.AddQueryParam("q","'root' in parents and name='AAWorkArea'");
    const char *jsonResponse = rest.fullRequestNoBody("GET","/drive/v3/files");
    if (rest.get_LastMethodSuccess() != true) {
        std::cout << rest.lastErrorText() << "\r\n";
        return false;
    }

    json.Load(jsonResponse);
    std::cout << json.emit() << "\r\n";
    std::cout << "name: " << get_str(json.stringOf("files[0].name")) << "\r\n";
    std::cout << "id: " << get_str(json.stringOf("files[0].id")) << "\r\n";
    std::cout << "mimeType: " << get_str(json.stringOf("files[0].mimeType")) << "\r\n";
    std::cout << "-" << "\r\n";

    rest.ClearAllQueryParams();

    //  Now that we know the ID for the AAWorkarea directory, get the id for the FolderA having AAWorkArea as the parent.
    CkStringBuilder sbQuery;
    sbQuery.Append("name = 'FolderA' and '");
    sbQuery.Append(json.stringOf("files[0].id"));
    sbQuery.Append("' in parents");
    rest.AddQueryParamSb("q",sbQuery);

    jsonResponse = rest.fullRequestNoBody("GET","/drive/v3/files");
    if (rest.get_LastMethodSuccess() != true) {
        std::cout << rest.lastErrorText() << "\r\n";
        return false;
    }

    json.Load(jsonResponse);
    std::cout << json.emit() << "\r\n";
    std::cout << "name: " << get_str(json.stringOf("files[0].name")) << "\r\n";
    std::cout << "id: " << get_str(json.stringOf("files[0].id")) << "\r\n";
    std::cout << "mimeType: " << get_str(json.stringOf("files[0].mimeType")) << "\r\n";
    std::cout << "-" << "\r\n";
    return true;
}

bool
GoogleDriveController::do_authorization()
{
    restore_access_token_from_json();
    return true;
}
