#include "performer.hpp"
#include <memory>
#include <utility>

namespace control {
[[maybe_unused]] void MenuPerformer::openMainMenu() {
    state = MenuState::MainMenu;
    isGameIsEnded = false;
}

[[maybe_unused]] void control::MenuPerformer::loadLevel(
    LevelPerformer &levelPerformer
) {
    setState(control::MenuState::Empty);
    levelPerformer.setState(control::LevelState::Running);
    isGameIsEnded = false;
    // TODO: rewrite or rename
}

[[maybe_unused]] void MenuPerformer::openLoadLevelMenu() {
    state = MenuState::LoadMenu;
    isGameIsEnded = false;
}

[[maybe_unused]] void MenuPerformer::openWonMenu() {
    // TODO updateTimer WonMenu
    state = MenuState::WonMenu;
    isGameIsEnded = true;
}

void MenuPerformer::openPauseMenu() {
    state = MenuState::PauseMenu;
}

MenuState MenuPerformer::getState() {
    return state;
}

void MenuPerformer::setState(MenuState newState) {
    state = newState;
}

void MenuPerformer::closeCurrentMenu() {
    state = MenuState::Empty;
}

void MenuPerformer::closeWindow() {
    window.close();
}

[[nodiscard]] bool MenuPerformer::getIsGameIsEnded() const {
    return isGameIsEnded;
}

[[maybe_unused]] void LevelPerformer::pause() {
    state = LevelState::Paused;
}

[[maybe_unused]] void LevelPerformer::resume() {
    state = LevelState::Running;
}

[[maybe_unused]] void LevelPerformer::reset() {
    game->getPlayer()->reset();
}

[[maybe_unused]] void LevelPerformer::moveLeft() {
    game->getPlayer()->moveLeft();
}

[[maybe_unused]] void LevelPerformer::moveRight() {
    game->getPlayer()->moveRight();
}

[[maybe_unused]] void LevelPerformer::moveDown() {
    game->getPlayer()->moveDown();
}

void LevelPerformer::jump() {
    game->getPlayer()->jump();
}

void LevelPerformer::stop() {
    game->getPlayer()->stop();
}

[[maybe_unused]] void LevelPerformer::exit() {
    state = LevelState::Empty;
}

LevelPerformer::LevelPerformer(sf::RenderWindow &window_) : Performer(window_) {
    state = LevelState::Empty;
}

void LevelPerformer::setLevel(std::unique_ptr<Platformer::Game> game_) {
    game = std::move(game_);
}

LevelState LevelPerformer::getState() {
    return state;
}

void LevelPerformer::setState(LevelState newState) {
    state = newState;
}

std::unique_ptr<Platformer::Game> &LevelPerformer::getLevel() {
    return game;
}

std::unique_ptr<Platformer::Statistics> &LevelPerformer::getStatistics() {
    return statistics;
}

std::string MenuPerformer::getLevelFilePath(int num) {
    return level_paths[num];
}

ServerPerformer::ServerPerformer(sf::RenderWindow &window_,
                                 const std::string &address,
                                 const std::string &key_directory,
                                 std::string  level_dir_path_)
    : Performer(window_),
      client(client::EstablishClient(address, key_directory)),
      level_dir_path(std::move(level_dir_path_)){
} // TODO: where is key? Every time says 'creating new key', yet key is the same

// use inside try ... catch block
bool ServerPerformer::getLevel(const std::string &level_name) {
    client::ActionReply reply = client.sendRequestGetLevel(level_name);
    if (reply.is_successful()) {
        // TODO: validate file content
        std::string level_in_json;
        google::protobuf::util::MessageToJsonString(reply.level_content(), &level_in_json);
        support::add_or_replace_json_file(level_dir_path+level_name+".json", level_in_json);
    }
    // TODO: ".json" is hard-coded: is bad?
    return true;
}

// use inside try ... catch block
bool ServerPerformer::sendLevel(const std::string &level_name) {
    client::ActionReply reply = client.sendRequestAddOrReplaceLevel(level_name);
    return reply.is_successful();
}

// use inside try ... catch block
bool ServerPerformer::deleteLevel(const std::string &level_name) {
    client::ActionReply reply = client.sendRequestDeleteLevel(level_name);
    return reply.is_successful();
}

// use inside try ... catch block
[[maybe_unused]] bool ServerPerformer::checkLevel(const std::string &level_name) {
    client::ActionReply reply = client.sendRequestCheckLevelExistence(level_name);
    return reply.is_successful();
}

// use inside try ... catch block
std::vector<std::string> ServerPerformer::loadAllAvailableLevelNames() {
    client::ActionReply reply = client.sendRequestGetAllAvailableLevelNames();
    if (reply.is_successful()) {
        std::vector<std::string> answer;
        for (auto &it : reply.possible_level_list()) {
            answer.push_back(it);
        }
        return std::move(answer);
    }
    return {};
}

void ServerPerformer::switch_in_local_set(const std::string & name)  {
    if (chosen_local_levels.count(name)) {
        chosen_local_levels.erase(name);
    } else {
        chosen_local_levels.insert(name);
    }
}

void ServerPerformer::switch_in_global_set(const std::string & name)  {
    if (chosen_global_levels.count(name)) {
        chosen_global_levels.erase(name);
    } else {
        chosen_global_levels.insert(name);
    }
}

void ServerPerformer::debug_local_set() {
    std::cout << "Debug message: levelPerformer chosen local levels:\n";
    for (auto & levelFilePath : chosen_local_levels) {
        std::cout << levelFilePath << '\n';
    }
}

void ServerPerformer::debug_global_set() {
    std::cout << "Debug message: levelPerformer chosen global levels:\n";
    for (auto & levelFilePath : chosen_global_levels) {
        std::cout << levelFilePath << '\n';
    }
}

void ServerPerformer::sendSelectedToServer() {
    std::cout << "Sending selected files to server\n";
    try {
        for (auto & levelFilePath : chosen_local_levels) {
            sendLevel(levelFilePath);
            std::cout << levelFilePath << " successfully sent to server\n";
        }
        chosen_local_levels.clear();
    } catch (const support::file_handling_exception & exception) {
        std::cerr << exception.what();
    }
    // TODO: write catch block: into game log? & on-screen message
    // TODO: why if first fails, others stop? Not fair! Redo.
}

void ServerPerformer::getSelectedFromServer() {
    std::cout << "Downloading selected files from server\n";
    try {
        for (auto & levelFilePath : chosen_global_levels) {
            getLevel(levelFilePath);
            std::cout << levelFilePath << " downloaded and saved to local\n";
        }
        chosen_global_levels.clear();
    } catch (const support::file_handling_exception & exception) {
        std::cerr << exception.what();
    }
}

void ServerPerformer::deleteSelectedFromServer() {
    std::cout << "Deleting selected files from server\n";
    try {
        for (auto & levelFilePath : chosen_global_levels) {
            deleteLevel(levelFilePath);
            std::cout << levelFilePath << " successfully deleted from server\n";
        }
        chosen_global_levels.clear();
    } catch (const support::file_handling_exception & exception) {
        std::cerr << exception.what();
    }
}

void ServerPerformer::clear_local_levels_selection() {
    chosen_local_levels.clear();
}

void ServerPerformer::clear_global_levels_selection() {
    chosen_global_levels.clear();
}

// TODO: declaration & definition orders are not the same with many files; is OK?

}  // namespace control
