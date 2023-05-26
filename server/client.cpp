#include <google/protobuf/util/json_util.h>
#include <grpcpp/grpcpp.h>
#include <fstream>
#include <string>
#include "json_file_exchange.grpc.pb.h"
#include "source.hpp"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using json_file_exchange::Act;
using json_file_exchange::ActionReply;
using json_file_exchange::ActionRequest;

class LevelClient {
private:
    std::string level_dir_path = "../levels-client/";  // TODO: is needed?
public:
    LevelClient(const std::shared_ptr<Channel> &channel)
        : stub_(json_file_exchange::Act::NewStub(channel)) {
    }  // TODO: is safe to make explicit?

    ActionReply add_level(const std::string &level_name) {
        ActionRequest request;
        request.set_action("add");
        request.set_level_name(level_name);
        json_file_exchange::LevelContent level_content;
        google::protobuf::util::JsonStringToMessage(
            file_content_string(level_name), &level_content
        );
        request.set_allocated_level_content(&level_content);
        return sendRequest(request);  // TODO: pass by ref? What do?
    }

    ActionReply check_level_existence(const std::string &level_name) {
        // TODO: refactor repeated code in add, check, get, delete
        ActionRequest request;
        request.set_action("check");
        request.set_level_name(level_name);
        return sendRequest(request);
    }

    ActionReply get_level(const std::string &level_name) {
        // TODO: refactor repeated code in add, check, get, delete// TODO:
        // refactor repeated code in add, check, delete
        ActionRequest request;
        request.set_action("get");
        request.set_level_name(level_name);
        return sendRequest(request);
    }

    ActionReply delete_level(const std::string &level_name) {
        // TODO: refactor repeated code in add, check, get, delete
        ActionRequest request;
        request.set_action("delete");
        request.set_level_name(level_name);
        return sendRequest(request);  // TODO: look into message constructors
    }

    // takes as input 3 strings; json file must be read into string using
    // ifstream beforehand
    ActionReply sendRequest(const ActionRequest &request) {
        ActionReply reply;

        ClientContext context;

        Status status = stub_->sendRequest(&context, request, &reply);

        if (status.ok()) {
            return reply;
        } else {
            reply.set_result(false);
            std::cerr << "Procedure call failed\n";
            std::cerr << status.error_code() << ": " << status.error_message()
                      << std::endl;
            return reply;
        }
    }

private:
    std::unique_ptr<Act::Stub> stub_;
};

// example function to check workability of client class
void RunClient() {
    std::string target_address("0.0.0.0:50051");
    LevelClient client(
        grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials())
    );

    std::string action = "check";
    std::string level_name = "t01-box-with-bladders.json";
    std::ifstream level_file("levels/" + level_name);
    std::string level_content_json_string;
    level_file >> level_content_json_string;

    ActionReply reply = client.add_level("t01-box-with-bladders.json");
    if (reply.is_successful()) {
        std::cout << "Request succeeded.\n";

        std::string level_in_json;
        google::protobuf::util::MessageToJsonString(
            reply.level_content(), &level_in_json
        );
        // TODO: now I should parse reply.level_content() into file
    } else {
        std::cout << "Request failed\n";
    }
}  // TODO: clear this function

int main(int argc, char *argv[]) {
    RunClient();

    return 0;
}