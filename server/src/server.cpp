#include <grpcpp/grpcpp.h>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include "json_file_exchange.grpc.pb.h"
#include "server_source.hpp"
#include "source.hpp"

namespace server {
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
// TODO: big TODO: server.hpp

class LevelFile {
private:
    std::string m_Name;
    int32_t m_Creator;
    mutable std::mutex m_mutex;

    friend class LevelManagementServer;

public:
    LevelFile(
        const std::string &fileName,
        const std::string &fileContent,
        int32_t creatorID
    )
        : m_Creator(creatorID) {
        support::add_or_replace_json_file(fileName, fileContent);
    }
};

class LevelManagementServer final : public Act::Service {
private:
    std::string level_dir_path = "../server/server_test_directory/";
    std::map<std::string, LevelFile> Levels;
    std::mutex unknown_level_mutex;  // is used when new level is added:

    // safer to perform 1 operation of this kind at a time

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
        // TODO: find out why reply->DebugString() is empty, yet actual reply
        //  properly contains false booleans
    }

public:
    std::vector<std::string> collect_present_levels() {
        // TODO: should I clean Levels map here to properly process levels
        // manually deleted after server launch? once in a while probably yes:
        // LevelFile constructor is not cheap
        std::vector<std::string> filenames;
        // TODO: save level creators to some kind of list so this info is not
        // lost when shutting server down
        for (const auto &level_file :
             std::filesystem::directory_iterator(level_dir_path)) {
            const std::string levelPath = level_file.path();
            std::cout << "Collecting present levels: there is level with path "
                      << levelPath << '\n';
            if (server::get_file_extension(levelPath) != ".json") {
                continue;
            }
            // I need to address 2 cases:
            // 1. file was added, it is not in map. So I should add it to map
            // 2. file was changed, but is already in map. Do I even need to do
            // anything with it? Of course not!
            Levels.try_emplace(
                levelPath, levelPath, support::file_content_string(levelPath),
                server::AdminID
            );
            // TODO: check correctness
            auto levelName = std::filesystem::path(level_file).stem();
            filenames.emplace_back(levelName);
        }
        std::sort(filenames.begin(), filenames.end());
        return std::move(filenames);
    }  // TODO: utilities.cpp overlap - resolve!!!!

    void handleRequestAddOrReplace(
        int32_t authorID,
        const json_file_exchange::LevelContent &level_content,
        const std::string &level_file_path
    ) {
        // Mutex protection: if there is already such level on server,
        // use its own mutex. If not,
        // use special mutex for still-not-added levels
        if (Levels.count(level_file_path)) {
            std::unique_lock lock(Levels.at(level_file_path).m_mutex);
        } else {
            std::unique_lock lock(unknown_level_mutex);
        }
        // TODO: check mutex correctness

        if (Levels.count(level_file_path) &&
            Levels.at(level_file_path).m_Creator != authorID) {
            throw action_not_allowed(level_file_path);
        }

        std::string level_in_json;
        MessageToJsonString(level_content, &level_in_json);
        std::cout << "Adding/updating a level " << level_file_path
                  << " with content:\n-----\n"
                  << level_in_json << "\n-----\n";
        if (!Levels.count(level_file_path)) {
            support::add_or_replace_json_file(level_file_path, level_in_json);
        }
        Levels.try_emplace(
            level_file_path, level_file_path, level_in_json, authorID
        );
        // TODO: confirmation checkbox "Such file already exists. Do you wish to
        //  overwrite it?" is needed?
    }

    void
    handleRequestDelete(int32_t authorID, const std::string &level_file_path) {
        std::cout << "Attempting to delete a level " << level_file_path << '\n';
        if (!Levels.count(level_file_path)) {
            throw support::no_such_file(level_file_path);
        }

        std::unique_lock lock(Levels.at(level_file_path).m_mutex);
        // TODO: check mutex correctness

        if (Levels.at(level_file_path).m_Creator != authorID) {
            throw action_not_allowed(level_file_path);
        }
        std::cout << "Deleting a level " << level_file_path << '\n';
        support::delete_file(level_file_path);
        Levels.erase(level_file_path);
    }  // TODO: check correctness

    void
    handleRequestGet(ActionReply *reply, const std::string &level_file_path) {
        if (!Levels.count(level_file_path)) {
            throw support::no_such_file(level_file_path);
        }
        std::cout << "Getting a level " << level_file_path
                  << " with content:\n"
                     "-----\n"
                  << support::file_content_string(level_file_path)
                  << "\n"
                     "-----\n";
        LevelContent requested_level_content;  // TODO: use sendRequest' own
                                               // level_content, then swap?
        JsonStringToMessage(
            support::file_content_string(level_file_path),
            &requested_level_content
        );
        std::cout << "LevelContent looks like:\n++++++++++\n"
                  << requested_level_content.DebugString() << "\n++++++++++\n";
        reply->mutable_level_content()->Swap(&requested_level_content);
        reply->set_result(true);
        // TODO: do I even need to write this method through Levels map? Seems
        // obsolete, but other variant is inconsistent
    }

    void
    handleRequestCheck(ActionReply *reply, const std::string &level_file_path) {
        std::cout << "Checking existence of level " << level_file_path << '\n';
        bool result = Levels.count(level_file_path);
        std::cout << "Check result: " << result << '\n';
        reply->set_result(result);
    }

    void handleRequestGetAllNames(ActionReply *reply) {
        // TODO: is it OK to use this function without mutexes?
        //  If server sends a file which will no longer be there next second,
        //  client will just get an error. Seems fair to me
        std::cout << "Sending all available level names to client\n";
        auto presented_level_names = collect_present_levels();
        reply->mutable_possible_level_list()->Add(
            presented_level_names.begin(), presented_level_names.end()
        );
        bool result = true;
        reply->set_result(result);
    }

    Status sendRequest(
        ServerContext *context,
        const ActionRequest *request,
        ActionReply *reply
    ) override {
        try {
            auto &action = request->action();
            std::string level_file_path =
                level_dir_path + request->level_name() + ".json";
            auto &level_content = request->level_content();
            auto author_id = request->signature();

            std::cout << "=========================\n"
                         "New request:\n"
                      << "Requested action: " << action << '\n'
                      << "Level name: " << level_file_path << '\n'
                      << "Request signature: " << author_id << '\n';
            printDebug();
            std::string additional_info;

            if (action == "add") {
                handleRequestAddOrReplace(
                    author_id, level_content, level_file_path
                );
            } else if (action == "delete") {
                handleRequestDelete(author_id, level_file_path);
            } else if (action == "get") {
                handleRequestGet(reply, level_file_path);
            } else if (action == "check") {
                handleRequestCheck(reply, level_file_path);
            } else if (action == "get_all_names") {
                handleRequestGetAllNames(reply);
            }
            reply->set_is_successful(true);
        } catch (const support::file_handling_exception &file_exception) {
            return return_failure(reply, file_exception.what());
        } catch (const level_handling_exception &level_exception) {
            return return_failure(reply, level_exception.what());
        } catch (const std::runtime_error &error) {
            return return_failure(reply, error.what());
        }
        // TODO: expand on error handling: at least return to client what's
        //  going on
        // TODO: resolve additional info causing double free
        // TODO: naming!!!!!!!!
        return Status::OK;
    }
};

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    // TODO: server address from Egor!!!
    LevelManagementServer service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    // builder.AddListeningPort(server_address,
    // grpc::SslServerCredentials(grpc::SslServerCredentialsOptions()));
    // TODO: check gRPC version for line above; in 1.54 it should take 1 arg;
    //  installed version is 1.54, but this function requests 2 args
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << server_address << std::endl;
    server->Wait();
}
}  // namespace server

int main(int argc, char **argv) {
    server::RunServer();
    // TODO: when shutting down, remember level authors!
    return 0;
}