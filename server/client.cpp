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
using json_file_exchange::LevelContent;

using google::protobuf::util::JsonStringToMessage;
using google::protobuf::util::MessageToJsonString;

class LevelClient {
private:
    std::string level_dir_path = "../levels-client/";
    int32_t signature;

    ActionReply send_request_util_and_get_reply(
        const std::string &action_name,
        const std::string &level_name
    ) {
        ActionRequest request;
        request.set_action(action_name);
        request.set_level_name(level_name);
        request.set_signature(signature);
        if (action_name == "add") {
            LevelContent level_content;
            JsonStringToMessage(
                file_content_string(level_dir_path + level_name), &level_content
            );
            std::cout << "send_request_add_level debug: level_content message "
                         "content: "
                      << level_content.DebugString() << '\n';
            // TODO: learn to throw incomplete_level_file
            request.mutable_level_content()->Swap(&level_content);
        }
        return sendRequest(request);
    }

public:
    explicit LevelClient(const std::shared_ptr<Channel> &channel)
        : stub_(json_file_exchange::Act::NewStub(channel)) {
        signature = get_unique_key();  // TODO: use login system
    }

    void setSignature(int32_t newSignature) {
        signature = newSignature;
    }  // TODO: delete this placeholder and create real login system

    ActionReply sendRequestAddOrReplaceLevel(const std::string &level_name) {
        if (!file_exists(level_dir_path + level_name)) {
            throw no_such_file(level_dir_path + level_name);
        }
        return send_request_util_and_get_reply("add", level_name);
    }  // TODO: rewrite level_dir_path usage

    ActionReply sendRequestCheckLevelExistence(const std::string &level_name) {
        return send_request_util_and_get_reply("check", level_name);
    }

    ActionReply sendRequestGetLevel(const std::string &level_name) {
        return send_request_util_and_get_reply("get", level_name);
    }

    ActionReply sendRequestDeleteLevel(const std::string &level_name) {
        return send_request_util_and_get_reply("delete", level_name);
    }

    // takes as input 3 strings; json file must be read into string using
    // ifstream beforehand
    ActionReply sendRequest(const ActionRequest &request) {
        std::cout
            << "sendRequest debug: request message content: \n==========\n"
            << request.DebugString() << "==========\n";
        ActionReply reply;

        ClientContext context;

        Status status = stub_->sendRequest(&context, request, &reply);

        if (status.ok()) {
            return reply;
        } else {
            reply.set_result(false
            );  // TODO: what exactly is status? Read and use
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
    LevelClient client(grpc::CreateChannel(
        target_address,
        grpc::InsecureChannelCredentials()
    ));  // TODO: credentials usage
    // client.setSignature(1);
    validate_key_file_existence("../key_directory/");
    client.setSignature(get_key("../key_directory/"));
    try {
        ActionReply reply =
            client.sendRequestAddOrReplaceLevel("t01-box-with-ladder.json");
        // ActionReply reply = client.sendRequestDeleteLevel("test.json");
        // ActionReply reply =
        // client.sendRequestCheckLevelExistence("test.json"); ActionReply reply
        // = client.sendRequestGetLevel("test.json");
        if (reply.is_successful()) {
            std::cout << "Request succeeded.\n";
            std::cout << "Request result: result=" << reply.result()
                      << ", is_successful=" << reply.is_successful()
                      << ", additional_info=" << reply.additional_info()
                      << '\n';
            std::string level_in_json;
            MessageToJsonString(reply.level_content(), &level_in_json);
            std::cout << level_in_json;
            // TODO: TODO (:
        } else {
            std::cout << "Request failed"
                      << ", additional_info=" << reply.additional_info()
                      << '\n';
            // TODO: add fail logs: std::string to reply message?
        }
    } catch (const file_handling_exception &file_exception) {
        std::cerr << file_exception.what();
        return;
    } catch (const std::runtime_error &error) {
        std::cerr << error.what();
        return;
    }

}  // TODO: clear this function during project integration

int main(int argc, char *argv[]) {
    RunClient();
    return 0;
}