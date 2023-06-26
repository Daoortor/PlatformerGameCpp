#ifndef PLATFORMERGAMECPP_CLIENT_HPP
#define PLATFORMERGAMECPP_CLIENT_HPP

#include <google/protobuf/util/json_util.h>
#include <grpcpp/grpcpp.h>
#include <fstream>
#include <string>
#include "json_file_exchange.grpc.pb.h"
#include "source.hpp"

namespace client {

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using json_file_exchange::Act;
using json_file_exchange::ActionReply;
using json_file_exchange::ActionRequest;
using json_file_exchange::LevelContent;

using google::protobuf::util::JsonStringToMessage;
using google::protobuf::util::MessageToJsonString;

namespace client {
inline const int key_size = 100;  // it's not actually key size! TODO: redo!
inline const std::string key_file_name = "key";

void create_key_file(const std::string &directory);

void validate_key_file_existence(const std::string &directory);

int32_t get_key(const std::string &directory);
}  // namespace client

class LevelClient {
private:
    std::unique_ptr<Act::Stub> stub_;

    std::string level_dir_path = "../model/levels/";
    int32_t signature{};

    ActionReply send_request_util_and_get_reply(
        const std::string &action_name,
        const std::string &level_name
    );

    ActionReply sendRequest(const ActionRequest &request);

public:
    explicit LevelClient(const std::shared_ptr<Channel> &channel)
        : stub_(json_file_exchange::Act::NewStub(channel)) {
    }

    void setSignature(int32_t newSignature);

    ActionReply sendRequestAddOrReplaceLevel(const std::string &level_name);

    ActionReply sendRequestCheckLevelExistence(const std::string &level_name);

    ActionReply sendRequestGetLevel(const std::string &level_name);

    ActionReply sendRequestDeleteLevel(const std::string &level_name);

    ActionReply sendRequestGetAllAvailableLevelNames();
};

LevelClient
EstablishClient(const std::string &address, const std::string &key_directory);

// example function to check workability of client class
void RunClient();
}  // namespace client

#endif  // PLATFORMERGAMECPP_CLIENT_HPP
