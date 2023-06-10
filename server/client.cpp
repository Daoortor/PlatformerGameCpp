#include "json_file_exchange.grpc.pb.h"
#include <fstream>
#include <google/protobuf/util/json_util.h>
#include <grpcpp/grpcpp.h>
#include <string>
#include "source.hpp"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using json_file_exchange::Act;
using json_file_exchange::ActionReply;
using json_file_exchange::ActionRequest;


class LevelClient {
private:
    std::string level_dir_path = "../levels-client/";
    int32_t signature;
    void send_request_util(const std::string & action_name,
                           const std::string & level_name,
                           const std::string & level_content="") {

    }
public:
    explicit LevelClient(const std::shared_ptr<Channel> &channel)
        : stub_(json_file_exchange::Act::NewStub(channel)) {
        signature = get_unique_key();
    } // TODO: is safe to make explicit?

    ActionReply send_request_add_level(const std::string& level_name) {
        if (!file_exists(level_dir_path + level_name)) {
            throw no_such_file(level_dir_path + level_name);
        }
        ActionRequest request;
        request.set_action("add");
        request.set_level_name(level_name);
        json_file_exchange::LevelContent level_content;
        google::protobuf::util::JsonStringToMessage(file_content_string(level_dir_path + level_name), &level_content);
        std::cout << "send_request_add_level debug: level_content message content: " << level_content.DebugString() << '\n';
        // TODO: learn to throw incomplete_level_file
        request.mutable_level_content()->Swap(&level_content);
        return sendRequest(request);
    } // TODO: rewrite level_dir_path usage

    ActionReply check_level_existence(const std::string& level_name) {
        // TODO: refactor repeated code in add, check, get, delete
        ActionRequest request;
        request.set_action("check");
        request.set_level_name(level_name);
        request.set_signature(signature);
        return sendRequest(request);
    }

    ActionReply get_level(const std::string& level_name) {
        // TODO: refactor repeated code in add, check, get, delete
        ActionRequest request;
        request.set_action("get");
        request.set_level_name(level_name);
        request.set_signature(signature);
        return sendRequest(request);
        // TODO: this doesn't do much, does it? For future usage
    }

    ActionReply delete_level(const std::string& level_name) {
        // TODO: refactor repeated code in add, check, get, delete
        ActionRequest request;
        request.set_action("delete");
        request.set_level_name(level_name);
        request.set_signature(signature);
        return sendRequest(request); // TODO: look into message constructors
    }

    // takes as input 3 strings; json file must be read into string using ifstream beforehand
    ActionReply sendRequest(const ActionRequest& request) {
        std::cout << "sendRequest debug: request message content: \n==========\n"
                  << request.DebugString() << "==========\n";
        ActionReply reply;

        ClientContext context;

        Status status = stub_->sendRequest(&context, request, &reply);

        if (status.ok()) {
            return reply;
        } else {
            reply.set_result(false);
            std::cerr << "Procedure call failed\n";
            std::cerr << status.error_code() << ": " << status.error_message() << std::endl;
            return reply;
        }
    }

private:
    std::unique_ptr<Act::Stub> stub_;
};

// example function to check workability of client class
void RunClient() {

    std::string target_address("0.0.0.0:50051");
    LevelClient client(grpc::CreateChannel(target_address,
                                           grpc::InsecureChannelCredentials())); // TODO: credentials usage
    try {
        // ActionReply reply = client.send_request_add_level("test.json");
        // ActionReply reply = client.delete_level("test.json");
        // ActionReply reply = client.check_level_existence("t01-box-with-ladder.json");
        ActionReply reply = client.get_level("test.json");
        if (reply.is_successful()) {
            std::cout << "Request succeeded.\n";
            std::cout << "Request result: result=" << reply.result() << ", is_successful=" <<reply.is_successful() << '\n';
            std::string level_in_json;
            google::protobuf::util::MessageToJsonString(reply.level_content(), &level_in_json);
            std::cout << level_in_json;
            // TODO: TODO
        } else {
            std::cout << "Request failed\n";
        }
    } catch (const file_handling_exception &file_exception) {
        std::cerr << file_exception.what();
        return;
    } catch (const std::runtime_error & error) {
        std::cerr << error.what();
        return;
    }

} // TODO: clear this function during project integration

int main(int argc, char *argv[]) {
    RunClient();
    return 0;
}