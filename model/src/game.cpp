#include "../include/game.hpp"
#include <fstream>
#include <utility>
#include "../../tools/json.hpp"
#include "../include/player.hpp"

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
    : board(std::move(board_)), endPos(endPos_) {
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
    endPos = {levelData["endPos"][0], levelData["endPos"][1]};
}
}  // namespace Platformer
