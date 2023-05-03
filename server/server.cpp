#include "stringreverse.grpc.pb.h"
#include <grpcpp/grpcpp.h>
#include <string>

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using json_file_exchange::Act;
using json_file_exchange::ActionReply;
using json_file_exchange::ActionRequest;

// Server Implementation
class ReverseServiceImplementation final : public Act::Service {
    Status sendRequest(ServerContext *context, const ActionRequest *request,
                       ActionReply *reply) override {

        switch (request.action()) {
            case "add":
                // TODO: add actions: request.level_content() through MessageToJsonString() to ofstream into file
                break;
            case "del":
                // TODO: add actions: delete file by name
                break;
            case "get":
                // TODO: add actions: get string by ifstream via request.level_name(), save to reply.level_content() by JsonStringToMessage()
                break;
            case "check":
                // TODO: add actions: check file existence by name
                break;
        }

        reply->set_result(true);// TODO: multiple fail conditions
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    ReverseServiceImplementation service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // TODO: check gRPC version for line below; 1.54 should take 1 argument, installed 1.54, this requests 2
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << server_address << std::endl;
    server->Wait();
}

int main(int argc, char **argv) {
    RunServer();
    return 0;
}