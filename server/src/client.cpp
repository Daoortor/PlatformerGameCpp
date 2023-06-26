#include "client.hpp"

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
void create_key_file(const std::string &directory) {
    std::ofstream file(directory + key_file_name, std::ios::trunc);
    for (size_t i = 0; i < key_size; ++i) {
        file << random();
    }
    file.close();
}

void validate_key_file_existence(const std::string &directory) {
    if (!support::file_exists(directory + key_file_name)) {
        std::cout << "No key file, creating new\n";
        create_key_file(directory);
    }
}

int32_t get_key(const std::string &directory) {
    std::ifstream file(directory + key_file_name);
    std::string content(
        (std::istreambuf_iterator<char>(file)),
        (std::istreambuf_iterator<char>())
    );
    std::hash<std::string> hasher;
    return static_cast<int32_t>(hasher(content));
}

}  // namespace client

ActionReply LevelClient::send_request_util_and_get_reply(
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
            support::file_content_string(level_dir_path + level_name + ".json"),
            &level_content
        );
        std::cout
            << "send_request_add_level debug: level_content message content: "
            << level_content.DebugString() << '\n';
        // TODO: learn to throw incomplete_level_file
        request.mutable_level_content()->Swap(&level_content);
    }
    return sendRequest(request);
}

ActionReply LevelClient::sendRequest(const ActionRequest &request) {
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
        std::cerr << status.error_code() << ": " << status.error_message()
                  << std::endl;
        return reply;
    }
}

void LevelClient::setSignature(int32_t newSignature) {
    signature = newSignature;
}  // TODO: delete this placeholder and create real login system... Or not?

ActionReply LevelClient::sendRequestAddOrReplaceLevel(
    const std::string &level_name
) {
    if (!support::file_exists(level_dir_path + level_name + ".json")) {
        throw support::no_such_file(level_dir_path + level_name + ".json");
    }
    if (level_name.length() >= 2 && level_name[0] == '.' &&
        level_name[1] == '.') {
        throw support::leading_dots_in_file_name(level_name);
    }
    return send_request_util_and_get_reply("add", level_name);
}

// TODO: rewrite level_dir_path usage

ActionReply LevelClient::sendRequestCheckLevelExistence(
    const std::string &level_name
) {
    return send_request_util_and_get_reply("check", level_name);
}

ActionReply LevelClient::sendRequestGetLevel(const std::string &level_name) {
    return send_request_util_and_get_reply("get", level_name);
}

ActionReply LevelClient::sendRequestDeleteLevel(const std::string &level_name) {
    return send_request_util_and_get_reply("delete", level_name);
}

ActionReply LevelClient::sendRequestGetAllAvailableLevelNames() {
    ActionRequest request;
    request.set_action("get_all_names");
    request.set_signature(signature);
    return sendRequest(request);
}

LevelClient
EstablishClient(const std::string &address, const std::string &key_directory) {
    const std::string &target_address(address);
    LevelClient client(grpc::CreateChannel(
        target_address,
        grpc::InsecureChannelCredentials()
    ));  // TODO: credentials usage
    client::validate_key_file_existence(key_directory);
    client.setSignature(client::get_key(key_directory));
    return std::move(client);
}

// example function to check workability of client class
void RunClient() {
    LevelClient client = EstablishClient("0.0.0.0:50051", "../key_directory/");
    try {
        // ActionReply reply =
        // client.sendRequestAddOrReplaceLevel("t01-box-with-ladder.json");
        // ActionReply reply = client.sendRequestDeleteLevel("test.json");
        // ActionReply reply =
        // client.sendRequestCheckLevelExistence("test.json");
        ActionReply reply = client.sendRequestGetLevel("../source.hpp");
        if (reply.is_successful()) {
            std::cout << "Request succeeded.\n";
            std::cout << "Request result: result=" << reply.result()
                      << ", is_successful=" << reply.is_successful()
                      << ", additional_info=" << reply.additional_info()
                      << '\n';
            std::string level_in_json;
            MessageToJsonString(reply.level_content(), &level_in_json);
            std::cout << level_in_json;
        } else {
            std::cout << "Request failed"
                      << ", additional_info=" << reply.additional_info()
                      << '\n';
        }
    } catch (const support::file_handling_exception &file_exception) {
        std::cerr << file_exception.what();
        return;
    } catch (const std::runtime_error &error) {
        std::cerr << error.what();
        return;
    }
}
}  // namespace client