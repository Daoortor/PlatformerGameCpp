#ifndef PLATFORMERGAMECPP_SERVER_HPP
#define PLATFORMERGAMECPP_SERVER_HPP

#include <grpcpp/grpcpp.h>
#include "json_file_exchange.grpc.pb.h"

namespace server {

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using json_file_exchange::Act;
using json_file_exchange::ActionReply;
using json_file_exchange::ActionRequest;
using json_file_exchange::LevelContent;

inline const int32_t AdminID = 1443241;

class level_handling_exception : public std::runtime_error {
public:
    inline explicit level_handling_exception(const std::string &what_arg)
        : std::runtime_error("Error: " + what_arg + '\n'){};
};

class action_not_allowed : public level_handling_exception {
public:
    inline explicit action_not_allowed(const std::string &file_name)
        : level_handling_exception(
              "you don't have the right to edit or delete this file: " +
              file_name
          ){};
};

class LevelFile {
private:
    std::string m_Name;
    int32_t m_Creator;
    mutable std::mutex m_mutex;

    friend class LevelManagementServer;

public:
    LevelFile(
        int32_t creatorID,
        const std::string &fileName,
        const std::string &fileContent = ""
    );
};

class LevelManagementServer final : public Act::Service {
private:
    std::string level_dir_path = "../server/server_test_directory/";
    std::map<std::string, LevelFile> Levels;
    std::mutex unknown_level_mutex;  // is used when new level is added:

    // safer to perform 1 operation of this kind at a time

    [[maybe_unused]] void printDebug();

    static Status
    return_failure(ActionReply *reply, const std::string &error_what);

public:
    std::vector<std::string> collect_present_levels();

    void handleRequestAddOrReplace(
        int32_t authorID,
        const json_file_exchange::LevelContent &level_content,
        const std::string &level_file_path
    );

    void
    handleRequestDelete(int32_t authorID, const std::string &level_file_path);

    void
    handleRequestGet(ActionReply *reply, const std::string &level_file_path);

    void
    handleRequestCheck(ActionReply *reply, const std::string &level_file_path);

    void handleRequestGetAllNames(ActionReply *reply);

    Status sendRequest(
        ServerContext *context,
        const ActionRequest *request,
        ActionReply *reply
    ) override;
};

void RunServer();

}  // namespace server

#endif  // PLATFORMERGAMECPP_SERVER_HPP
