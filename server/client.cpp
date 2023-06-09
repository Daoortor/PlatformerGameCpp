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
public:
    LevelClient(const std::shared_ptr<Channel> &channel)
        : stub_(json_file_exchange::Act::NewStub(channel)) {
        signature = get_unique_key();
    } // TODO: is safe to make explicit?

    ActionReply add_level(const std::string& level_name) {
        if (!file_exists(level_dir_path + level_name)) {
            throw no_such_file(level_dir_path + level_name);
        }
        ActionRequest request;
        request.set_action("add");
        request.set_level_name(level_name);
        json_file_exchange::LevelContent level_content;
        auto fcs_debug = file_content_string(level_dir_path + level_name);
        std::cout << "fcs_debug: " << fcs_debug << '\n';
        google::protobuf::util::JsonStringToMessage(fcs_debug, &level_content); // TODO: why does it fail?
        std::cout << "add_level debug: level_content message content: " << level_content.DebugString() << '\n';
        // TODO: learn to throw incomplete_level_file
        request.set_allocated_level_content(&level_content);
        return sendRequest(request); // TODO: pass by ref? What do?
        // TODO: big TODO: .json fields naming & its usage in client code - CHECK!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
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
        // TODO: refactor repeated code in add, check, get, delete// TODO: refactor repeated code in add, check, delete
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
        // ActionReply reply = client.add_level("t01-box-with-ladder.json");
        // ActionReply reply = client.delete_level("t01-box-with-ladder.json");
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


    json_file_exchange::LevelContent level_content;

    level_content.set_levelname(std::string("Test"));
    level_content.set_width(100);
    level_content.set_height(100);

    json_file_exchange::LevelContent_BlockRow block_row;
    std::string block_data[] = {"air", "stone", "air"};
    block_row.ParseFromArray(block_data, 3);
    level_content.mutable_blockmap()->Add()->CopyFrom(block_row); // AddAllocated(&block_row);

    level_content.mutable_playerstartpos()->Add(50);//set_x(50);
    level_content.mutable_playerstartpos()->Add(50);
    level_content.mutable_endpos()->Add(60);
    level_content.mutable_endpos()->Add(60);

    std::string level_in_json = file_content_string("../levels-client/test.json");
    google::protobuf::util::JsonStringToMessage(level_in_json, &level_content);
    google::protobuf::util::MessageToJsonString(level_content, &level_in_json); // TODO: does it work?
    google::protobuf::util::JsonStringToMessage(level_in_json, &level_content);
    google::protobuf::util::MessageToJsonString(level_content, &level_in_json); // TODO: does it work?
    // add_or_replace_json_file("../levels-client/test.json", level_in_json);
    std::cout << level_in_json;


} // TODO: clear this function

int main(int argc, char *argv[]) {
    RunClient();
    return 0;
}