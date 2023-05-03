#include "json_file_exchange.grpc.pb.h"
#include <fstream>
#include <google/protobuf/util/json_util.h>
#include <grpcpp/grpcpp.h>
#include <string>

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using json_file_exchange::Act;
using json_file_exchange::ActionReply;
using json_file_exchange::ActionRequest;


class LevelClient {
public:
    LevelClient(const std::shared_ptr<Channel> &channel)
        : stub_(LevelClient::Act(channel)) {}

    // takes as input 3 strings; json file must be read into string using ifstream beforehand
    ActionReply sendRequest(std::string action, std::string level_name, const std::string &level_content_json_string = "") {
        ActionRequest request;
        request.set_action(action);
        request.set_level_name(level_name);
        LevelContent level_content;
        google::protobuf::util::JsonParseOptions options_parse;
        JsonStringToMessage(level_content_json_string, &level_content, options_parse);
        request.set_level_content(level_content);
        // TODO: list  pros and cons using message LevelContent over simple string, specifically with this functionality

        ActionReply reply;

        ClientContext context;

        Status status = stub_->sendRequest(&context, request, &reply);

        if (status.ok()) {
            return reply;
        } else {
            std::cerr << "Procedure call failed\n";
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            return "fail";// TODO: what to do?
        }
    }

private:
    std::unique_ptr<Act::Stub> stub_;
};

// example function to check workability of client class
void RunClient() {
    std::string target_address("0.0.0.0:50051");
    LevelClient client(grpc::CreateChannel(target_address,
                                           grpc::InsecureChannelCredentials()));

    std::string action = "check";
    std::string level_name = "t01-box-with-bladders.json";
    std::ifstream level_file("levels/" + level_name);
    std::string level_content_json_string;
    level_file >> level_content_json_string;

    ActionReply reply = client.sendRequest(action, level_name, level_content_json_string);
    auto result = reply.result();
    // TODO: now I should parse reply.level_content() into file
}

int main(int argc, char *argv[]) {
    RunClient();

    return 0;
}