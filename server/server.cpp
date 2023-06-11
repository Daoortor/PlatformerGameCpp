#include <grpcpp/grpcpp.h>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include "json_file_exchange.grpc.pb.h"
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

// TODO: big TODO: naming

class UserAccount {
private:
    int32_t m_id;        // does not change
    int8_t m_password;   // does not change, user does not interact with it , it
                         // is used by server only
    std::string m_Name;  // can be changed
    // TODO: is needed?
    uint action_count_past_hour;  // to prevent spam
public:
    std::string Name() {
        return m_Name;
    }

    void setName(const std::string &newName) {
        m_Name = newName;
    }
    friend class LevelManagementServer;
};

enum class Who { Everyone, Author, Admin };

class LevelFile {
private:
    std::string m_Name;
    int32_t m_Creator;
    Who m_WhoCanEdit;

    // TODO: file path should be handled from outside, but how?
    // TODO: mutex
    LevelFile(
        const std::string &fileName,
        const std::string &fileContent,
        int32_t creatorID
    ) {
        if (file_exists(fileName)) {
            throw file_already_exists();
        }  // TODO: that is not this class' job!
        m_Creator = creatorID;
        m_WhoCanEdit = Who::Author;  // TODO: implement?
        add_or_replace_json_file(fileName, fileContent);
    }

    void modify(const std::string &newFileContent, int32_t requestSignature) {
        if (m_Creator != requestSignature) {
            throw action_not_allowed(m_Name);
        }  // TODO: that is not this class' job!
        add_or_replace_json_file(m_Name, newFileContent);
    }
    friend class LevelManagementServer;
};

class LevelManagementServer final : public Act::Service {
private:
    std::string level_dir_path = "../levels-server/";
    std::map<std::string, int> player_login_key;
    std::vector<UserAccount> Users;
    std::map<std::string, LevelFile> Levels;

    void printDebug() {
        std::cout << "Current Levels map looks like: \n";
        for (auto &Level : Levels) {
            std::cout << Level.first << " - level with creator "
                      << Level.second.m_Creator << "\n";
        }
    }

    static Status
    return_failure(ActionReply *reply, const std::string &error_what) {
        std::cerr << error_what;
        reply->set_is_successful(false);
        std::cout << "Failure debug: reply is\n"
                  << reply->DebugString() << '\n';
        return Status::CANCELLED;
    }

    // TODO: update function to collect added by hand levels
public:
    void handleRequestAdd(
        int32_t authorID,
        const json_file_exchange::LevelContent &level_content,
        const std::string &level_file_path
    ) {
        if (file_exists(level_file_path)) {
            throw file_already_exists();
        }
        std::string level_in_json;
        MessageToJsonString(level_content, &level_in_json);
        std::cout << "Adding a level " << level_file_path
                  << " with content:\n-----\n"
                  << level_in_json << "-----\n";
        Levels.insert(std::make_pair(
            level_file_path, LevelFile(level_file_path, level_in_json, authorID)
        ));
        // add_or_replace_json_file(level_file_path, level_in_json);
        // TODO: confirmation checkbox "Such file already exists. Do you wish to
        // overwrite it?"
    }

    void
    handleRequestDelete(int32_t authorID, const std::string &level_file_path) {
        std::cout << "Attempting to delete a level " << level_file_path << '\n';
        if (!Levels.count(level_file_path)) {
            throw no_such_file(level_file_path);
        }
        if (Levels.at(level_file_path).m_Creator != authorID) {
            throw action_not_allowed(level_file_path);
        }
        std::cout << "Deleting a level " << level_file_path << '\n';
        delete_file(level_file_path);
        Levels.erase(level_file_path);
    }  // TODO: check correctness

    void
    handleRequestGet(ActionReply *reply, const std::string &level_file_path) {
        if (!Levels.count(level_file_path)) {
            throw no_such_file(level_file_path);
        }
        std::cout << "Getting a level " << level_file_path
                  << " with content:\n-----\n"
                  << file_content_string(level_file_path) << "\n-----\n";
        LevelContent requested_level_content;  // TODO: use sendRequest' own
                                               // level_content, then swap?
        JsonStringToMessage(
            file_content_string(level_file_path), &requested_level_content
        );
        std::cout << "LevelContent looks like:\n++++++++++\n"
                  << requested_level_content.DebugString() << "\n++++++++++\n";
        reply->mutable_level_content()->Swap(&requested_level_content);
        reply->set_result(true);
        // TODO: do I even need to write this method through Levels map? Seems
        // obsolete, but other variant - inconsistent
    }

    void
    handleRequestCheck(ActionReply *reply, const std::string &level_file_path) {
        std::cout << "Checking existence of level " << level_file_path << '\n';
        bool result = Levels.count(level_file_path);
        std::cout << "Check result: " << result << '\n';
        reply->set_result(result);
    }

    Status sendRequest(
        ServerContext *context,
        const ActionRequest *request,
        ActionReply *reply
    ) override {
        auto &action = request->action();
        std::string level_file_path = level_dir_path + request->level_name();
        auto &level_content =
            request->level_content();  // TODO: is it correct type?
        auto author_id = request->signature();

        std::cout << "=========================\nNew request:\n"
                  << "Requested action: " << action << '\n'
                  << "Level name: " << level_file_path << '\n'
                  << "Request signature: " << author_id << '\n';
        printDebug();
        std::string additional_info;
        try {
            if (action == "add") {
                handleRequestAdd(author_id, level_content, level_file_path);
            } else if (action == "delete") {
                handleRequestDelete(author_id, level_file_path);
            } else if (action == "get") {
                handleRequestGet(reply, level_file_path);
            } else if (action == "check") {
                handleRequestCheck(reply, level_file_path);
            }
            reply->set_is_successful(true);
        } catch (const file_handling_exception &file_exception) {
            std::cerr << file_exception.what();
            additional_info = file_exception.what();
            reply->set_is_successful(false);
            std::cout << "Failure debug: reply is\n"
                      << reply->DebugString() << '\n';
            return Status::CANCELLED;
        } catch (const level_exception &file_exception) {
            std::cerr << file_exception.what();
            additional_info = file_exception.what();
            reply->set_is_successful(false);
            std::cout << "Failure debug: reply is\n"
                      << reply->DebugString() << '\n';
            return Status::CANCELLED;
        } catch (const std::runtime_error &error) {
            std::cerr << error.what();
            additional_info = error.what();
            reply->set_is_successful(false);
            std::cout << "Failure debug: reply is\n"
                      << reply->DebugString() << '\n';
            return Status::CANCELLED;
        }
        // TODO: expand on error handling: at least return to client what's
        // going on
        // TODO: naming!!!!!!!!
        // TODO: repeated code
        // TODO: resolve additional info causing double free
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