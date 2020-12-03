#ifndef GOOGLEDRIVECONTROLLER_H
#define GOOGLEDRIVECONTROLLER_H

#include "lib.h"

#include <CkFileAccess.h>
#include <CkAuthGoogle.h>
#include <CkSocket.h>
#include <CkRest.h>
#include <CkOAuth2.h>
#include <CkStringBuilder.h>
#include <CkJsonArray.h>
#include <CkBinData.h>

class CkJsonObject;

class GoogleDriveController
{
    GoogleDriveController();

public:
    static GoogleDriveController& get_instance();
    GoogleDriveController(GoogleDriveController &) = delete;
    GoogleDriveController(GoogleDriveController &&) = delete;

    std::string get_access_token() const;

    bool restore_access_token_from_json();

    std::shared_ptr<CkJsonObject> get_list_files(const std::string request_body);
    std::shared_ptr<CkJsonObject> upload_file(const std::string request_body);
    std::shared_ptr<CkJsonObject> creane_new_folder(const std::string folder_name);
    std::shared_ptr<CkJsonObject> download_file(const std::string request_body);

private:
    // TODO: add ErrorCode to responce in each method
    bool do_authorization();
    bool connect();

private:
    static std::string s_access_token;
    CkRest m_rest;
};

#endif // GOOGLEDRIVECONTROLLER_H
