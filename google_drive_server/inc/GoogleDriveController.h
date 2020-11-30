#ifndef GOOGLEDRIVECONTROLLER_H
#define GOOGLEDRIVECONTROLLER_H

#include "lib.h"

#include <CkFileAccess.h>
#include <CkAuthGoogle.h>
#include <CkSocket.h>
#include <CkRest.h>
#include <CkOAuth2.h>
#include <CkStringBuilder.h>
#include <CkJsonObject.h>
#include <CkJsonArray.h>
#include <CkBinData.h>


class GoogleDriveController
{
    GoogleDriveController();

public:
    static GoogleDriveController& get_instance();
    GoogleDriveController(GoogleDriveController &) = delete;
    GoogleDriveController(GoogleDriveController &&) = delete;

    std::string get_access_token() const;

    bool restore_access_token_from_json();

    bool show_list_files();
    bool upload_file(std::string file_path);
    bool download_file(std::string file_path);

    bool show_folder_id();

private:
    // TODO: add ErrorCode to responce in each method
    bool do_authorization();

private:
    static std::string s_access_token;
    CkRest m_rest;
};

#endif // GOOGLEDRIVECONTROLLER_H
