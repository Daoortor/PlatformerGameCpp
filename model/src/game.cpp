#include "game.hpp"
#include <fstream>
#include <utility>
#include "../../tools/json.hpp"
#include "player.hpp"

using json = nlohmann::json;

namespace Platformer {
Game::~Game() = default;

void Game::update() {
    player->notifyAll();
}

Game::Game(
    std::vector<std::vector<std::unique_ptr<Block>>> board_,
    utilities::Vector playerPos,
    utilities::Vector endPos_
)
    : board(std::move(board_)), startPos(playerPos), endPos(endPos_) {
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
}

Game::Game(const std::string &filename) {
    std::ifstream f(filename);
    json levelData = json::parse(f);
    std::vector<std::vector<std::string>> blockMap = levelData["blockMap"];
    utilities::Vector playerPos = {
        levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    auto *playerObject = new Player(this, playerPos);
    player = std::unique_ptr<Player>(playerObject);
    board = Board(blockMap);
    startPos = {levelData["playerStartPos"][0], levelData["playerStartPos"][1]};
    endPos = {levelData["endPos"][0], levelData["endPos"][1]};
}

void Game::writeToFile(const std::string &name, const std::string &filepath) {
    json levelData;
    levelData["levelName"] = name;
    levelData["width"] = board.getSize().get_x();
    levelData["height"] = board.getSize().get_y();
    levelData["blockMap"] = board.getBlockMap();
    levelData["playerStartPos"] = {startPos.get_x(), startPos.get_y()};
    levelData["endPos"] = {endPos.get_x(), endPos.get_y()};
    std::ofstream out(filepath + name + ".json");
    out << std::setw(4) << levelData << std::endl;
}
}  // namespace Platformer
