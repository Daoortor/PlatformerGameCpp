#include "json_file_exchange.grpc.pb.h"
#include <filesystem>
#include <fstream>
#include <grpcpp/grpcpp.h>
#include <string>
#include "source.hpp"

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using json_file_exchange::Act;
using json_file_exchange::ActionReply;
using json_file_exchange::ActionRequest;
using json_file_exchange::LevelContent;
using google::protobuf::util::JsonStringToMessage;
using google::protobuf::util::MessageToJsonString;

class LevelManagementServer final : public Act::Service {
private:
    std::string level_dir_path = "../levels-server/";
public:
    static void handleRequestAdd(const json_file_exchange::LevelContent& level_content,
                                 const std::string & level_file_path) {
        std::string level_in_json;
        MessageToJsonString(level_content, &level_in_json);
        std::cout << "Adding a level " << level_file_path
                  << " with content:\n-----\n" << level_in_json
                  << "-----\n";
        add_or_replace_json_file(level_file_path, level_in_json);
        // TODO: confirmation checkbox "Such file already exists. Do you wish to overwrite it?"
    }
    static void handleRequestDelete(const std::string & level_file_path) {
        if (!file_exists(level_file_path)) throw no_such_file(level_file_path);
        std::cout << "Deleting a level " << level_file_path << '\n';
        delete_file(level_file_path);
    }
    static void handleRequestGet(ActionReply *reply, const std::string & level_file_path) {
        if (!file_exists(level_file_path)) throw no_such_file(level_file_path);
        std::cout << "Getting a level " << level_file_path
                  << " with content:\n-----\n" << file_content_string(level_file_path)
                  << "\n-----\n";
        LevelContent requested_level_content; // TODO: use sendRequest' own level_content, then swap?
        JsonStringToMessage(file_content_string(level_file_path), &requested_level_content);
        std::cout << "LevelContent looks like:\n++++++++++\n" << requested_level_content.DebugString() << "\n++++++++++\n";
        reply->mutable_level_content()->Swap(&requested_level_content);
        reply->set_result(true);
    }
    static void handleRequestCheck(ActionReply *reply, const std::string & level_file_path) {
        std::cout << "Checking existence of level " << level_file_path << '\n';
        std::cout << "Check result: " << file_exists(level_file_path) << '\n';
        reply->set_result(file_exists(level_file_path));
    }
    Status sendRequest(ServerContext *context, const ActionRequest *request,
                       ActionReply *reply) override {

        auto & action = request->action();
        std::string level_file_path = level_dir_path + request->level_name();
        auto & level_content = request->level_content(); // TODO: is it correct type?

        std::cout << "=========================\nNew request:\n"
                  << "Requested action: " << action << '\n'
                  << "Level name: " << level_file_path << '\n'
                  << "Request signature: " << request->signature() << '\n';
        try {
            if (action == "add") {
                handleRequestAdd(level_content, level_file_path);
            } else if (action == "delete") {
                handleRequestDelete(level_file_path);
            } else if (action == "get") {
                handleRequestGet(reply, level_file_path);
            } else if (action == "check") {
                handleRequestCheck(reply, level_file_path);
            }
            reply->set_is_successful(true);
        } catch (const file_handling_exception &file_exception) {
            std::cerr << file_exception.what();
            reply->set_is_successful(false);
            return Status::CANCELLED;
        } catch (const std::runtime_error & error) {
            std::cerr << error.what();
            return Status::CANCELLED;
        } // TODO: expand on error handling: at least return to client what's going on
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    LevelManagementServer service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // TODO: check gRPC version for line above; in 1.54 it should take 1 arg;
    //  installed version is 1.54, but this function requests 2 args
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv) {
    RunServer();
    return 0;
}