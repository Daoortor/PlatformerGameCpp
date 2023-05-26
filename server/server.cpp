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

class file_handling_exception : public std::runtime_error {
public:
    explicit file_handling_exception(const std::string &what_arg)
        : std::runtime_error("Error: " + what_arg){};
};

class no_such_file : public file_handling_exception {
public:
    explicit no_such_file(const std::string &file_name)
        : file_handling_exception("no such file: " + file_name){};
};

class incomplete_level_file : public file_handling_exception {
public:
    explicit incomplete_level_file(const std::string &level_name)
        : file_handling_exception("missing crucial info for level file creation/overwrite: " + level_name){};
}; // TODO: is needed at all?

// Server Implementation
class ReverseServiceImplementation final : public Act::Service {
private:
    std::string level_dir_path = "../levels-server/";
public:
    Status sendRequest(ServerContext *context, const ActionRequest *request,
                       ActionReply *reply) override {

        auto & action = request->action();
        std::string level_file_path = level_dir_path + request->level_name();
        auto & level_content = request->level_content();

        std::cout << "Requested action: " << action << '\n';
        std::cout << "Level name: " << level_file_path << '\n';

        try {
            if (action == "add") {
                std::string level_in_json;
                google::protobuf::util::MessageToJsonString(level_content, &level_in_json);
                add_or_replace_json_file(level_file_path, level_in_json);
                // TODO: confirmation checkbox "Such file already exists. Do you wish to overwrite it?"
            } else if (action == "delete") {
                if (!file_exists(level_file_path)) throw no_such_file(level_file_path);
                delete_file(level_file_path);
            } else if (action == "get") {
                // TODO: add actions: get string by ifstream via request.level_name(),
                //  save to reply.level_content() by JsonStringToMessage()
                if (!file_exists(level_file_path)) throw no_such_file(level_file_path);
                json_file_exchange::LevelContent requested_level_content;
                google::protobuf::util::JsonStringToMessage(file_content_string(level_file_path), &requested_level_content);
                reply->set_allocated_level_content(&requested_level_content);
                reply->set_result(true);
            } else if (action == "check") {
                reply->set_result(file_exists(level_file_path));
            }
            reply->set_is_successful(true);
        } catch (const file_handling_exception &file_exception) {
            std::cerr << file_exception.what();
            reply->set_is_successful(false);
            return Status::CANCELLED;
        }
        return Status::OK;
    }
    // TODO: move different actions to different private methods
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    ReverseServiceImplementation service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // TODO: check gRPC version for line below; in 1.54 it should take 1 arg;
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