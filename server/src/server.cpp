#include "server.hpp"
#include <grpcpp/grpcpp.h>
#include <filesystem>
#include <fstream>
#include <map>
#include <mutex>
#include <string>
#include "json_file_exchange.grpc.pb.h"
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

LevelFile::LevelFile(
    int32_t creatorID,
    const std::string &fileName,
    const std::string &fileContent
)
    : m_Creator(creatorID) {
    if (!fileContent.empty()) {
        support::add_or_replace_json_file(fileName, fileContent);
    }
}

[[maybe_unused]] void LevelManagementServer::printDebug() {
    std::cout << "Current Levels map looks like: \n";
    for (auto &Level : Levels) {
        std::cout << Level.first << " - level with creator "
                  << Level.second.m_Creator << "\n";
    }
}

Status LevelManagementServer::return_failure(
    ActionReply *reply,
    const std::string &error_what
) {
    std::cerr << error_what;
    reply->set_is_successful(false);
    std::cout << "Failure debug: reply is\n" << reply->DebugString() << '\n';
    return Status::CANCELLED;
    // TODO: find out why reply->DebugString() is empty, yet actual reply
    //  properly contains false booleans
}

std::vector<std::string> LevelManagementServer::collect_present_levels() {
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
        if (level_file.path().extension() != ".json") {
            continue;
        }
        // I need to address 2 cases:
        // 1. file was added, it is not in map. So I should add it to map
        // 2. file was changed, but is already in map. Do I even need to do
        // anything with it? Of course not!
        Levels.try_emplace(levelPath, server::AdminID, levelPath);
        auto levelName = std::filesystem::path(level_file).stem();
        filenames.emplace_back(levelName);
    }
    std::sort(filenames.begin(), filenames.end());
    return std::move(filenames);
}

void LevelManagementServer::handleRequestAddOrReplace(
    int32_t authorID,
    const LevelContent &level_content,
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

    if (Levels.count(level_file_path) &&
        Levels.at(level_file_path).m_Creator != authorID) {
        throw action_not_allowed(level_file_path);
    }

    std::string level_in_json;
    MessageToJsonString(level_content, &level_in_json);
    std::cout << "Adding/updating a level " << level_file_path
              << " with content:\n-----\n"
              << level_in_json << "\n-----\n";
    support::add_or_replace_json_file(level_file_path, level_in_json);
    Levels.try_emplace(level_file_path, authorID, level_file_path);
}

void LevelManagementServer::handleRequestDelete(
    int32_t authorID,
    const std::string &level_file_path
) {
    if (!Levels.count(level_file_path)) {
        throw support::no_such_file(level_file_path);
    }

    std::unique_lock lock(Levels.at(level_file_path).m_mutex);
    std::cout << "Attempting to delete a level " << level_file_path << '\n';

    if (Levels.at(level_file_path).m_Creator != authorID) {
        throw action_not_allowed(level_file_path);
    }
    std::cout << "Deleting a level " << level_file_path << '\n';
    support::delete_file(level_file_path);
    Levels.erase(level_file_path);
}

void LevelManagementServer::handleRequestGet(
    ActionReply *reply,
    const std::string &level_file_path
) {
    if (!Levels.count(level_file_path)) {
        throw support::no_such_file(level_file_path);
    }
    std::unique_lock lock(Levels.at(level_file_path).m_mutex);

    std::cout << "Getting a level " << level_file_path
              << " with content:\n"
                 "-----\n"
              << support::file_content_string(level_file_path)
              << "\n"
                 "-----\n";
    LevelContent requested_level_content;
    JsonStringToMessage(
        support::file_content_string(level_file_path), &requested_level_content
    );
    std::cout << "LevelContent looks like:\n++++++++++\n"
              << requested_level_content.DebugString() << "\n++++++++++\n";
    reply->mutable_level_content()->Swap(&requested_level_content);
    reply->set_result(true);
    // TODO: do I even need to write this method through Levels map? Seems
    // obsolete, but other variant is inconsistent
}

// this method is not used in the last version of the menu, so it is not even
// mutex protected
void LevelManagementServer::handleRequestCheck(
    ActionReply *reply,
    const std::string &level_file_path
) {
    std::cout << "Checking existence of level " << level_file_path << '\n';
    bool result = Levels.count(level_file_path);
    std::cout << "Check result: " << result << '\n';
    reply->set_result(result);
}

void LevelManagementServer::handleRequestGetAllNames(ActionReply *reply) {
    //  If server sends a file which will no longer be there next second,
    //  client will just get an error, so no protection.
    std::cout << "Sending all available level names to client\n";
    auto presented_level_names = collect_present_levels();
    reply->mutable_possible_level_list()->Add(
        presented_level_names.begin(), presented_level_names.end()
    );
    bool result = true;
    reply->set_result(result);
}

Status LevelManagementServer::sendRequest(
    ServerContext *context,
    const ActionRequest *request,
    ActionReply *reply
) {
    try {
        auto &action = request->action();
        std::string level_file_path =
            level_dir_path + request->level_name() + ".json";
        auto &level_content = request->level_content();
        auto author_id = request->signature();
        /*
        std::cout << "=========================\n"
                     "New request:\n"
                  << "Requested action: " << action << '\n'
                  << "Level name: " << level_file_path << '\n'
                  << "Request signature: " << author_id << '\n';
        printDebug();
         */
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
    // TODO: resolve additional info causing double free
    return Status::OK;
}

void RunServer() {
    std::string server_address("0.0.0.0:50051");
    // TODO: server address from Egor!!!
    LevelManagementServer service;

    ServerBuilder builder;
    builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<Server> server(builder.BuildAndStart());
    std::cout << "Server listening on port: " << server_address << std::endl;
    server->Wait();
}

}  // namespace server

int main() {
    server::RunServer();
    return 0;
}
