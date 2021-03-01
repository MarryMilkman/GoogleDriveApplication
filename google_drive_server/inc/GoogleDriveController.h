#ifndef GOOGLEDRIVECONTROLLER_H
#define GOOGLEDRIVECONTROLLER_H

#include "lib.h"

namespace google_drive {
static std::string path_to_json_key{};
}

class GoogleDriveController
{
    GoogleDriveController();

public:
    static GoogleDriveController& get_instance();
    GoogleDriveController(GoogleDriveController &) = delete;
    GoogleDriveController(GoogleDriveController &&) = delete;

    std::string get_access_token() const;

    bool restore_access_token_from_json();

    std::shared_ptr<CkJsonObject> get_list_files(const std::string parent_id);
    std::shared_ptr<CkJsonObject> creane_new_folder(const std::string folder_name, std::string parent_id);
    std::shared_ptr<CkJsonObject> upload_file(const std::string file_name,
                                              const std::string file_type,
                                              CkByteData& data,
                                              const std::string parent_id);

    std::shared_ptr<CkJsonObject> download_file(const std::string file_id,
                                                CkByteData& received_file_data);

    std::shared_ptr<CkJsonObject> remove_file(const std::string file_id);

private:
    // TODO: add ErrorCode to responce in each method
    bool do_authorization();
    bool connect();

private:
    static std::string s_access_token;
    CkRest m_rest;
};

#endif // GOOGLEDRIVECONTROLLER_H
